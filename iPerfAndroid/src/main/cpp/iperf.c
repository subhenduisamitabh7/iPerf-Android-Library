//
// Created by shubhendu on 16/05/24.
//
#include <jni.h>
#include <stdlib.h>
#include <time.h>
#include <android/log.h>
#include <jni.h>
#include <stdio.h>
#include <sys/select.h>
#include <android/log.h>
#include <string.h>
#include <assert.h>
#include "src/iperf_api.h"
#include "src/iperf.h"

jobject g_obj;
jmethodID onRecvThroughpuyMethodID;
static JavaVM *jvm;

static void vc_reporter_callback(struct iperf_test *test) {
    // To Store test progress and output bandwidth
    float outputprogress;
    float outputbandwidth;

    __android_log_print(ANDROID_LOG_DEBUG, "IPERF", "####### vc_reporter_callback %lld",(long long) test);

    // See iperf_reporter_callback
    {
        extern double timeval_diff(struct timeval *tv0, struct timeval *tv1);

        struct iperf_stream *stream = NULL;
        struct iperf_interval_results *interval_results = NULL;
        iperf_size_t bytes = 0;
        double bandwidth = 0.0;
        int retransmits = 0;
        int total_packets = 0, lost_packets = 0;
        double avg_jitter = 0.0, lost_percent = 0.0;

        SLIST_FOREACH(stream, &test->streams, streams) {
            interval_results = TAILQ_LAST(&stream->result->interval_results, irlisthead);
            bytes += interval_results->bytes_transferred;

            if (test->protocol->id == Ptcp) {
                if (test->mode == SENDER && test->sender_has_retransmits) {
                    retransmits += interval_results->interval_retrans;
                }
            } else {
                total_packets += interval_results->interval_packet_count;
                lost_packets += interval_results->interval_cnt_error;
                avg_jitter += interval_results->jitter;
            }
        }

        stream = SLIST_FIRST(&test->streams);

        if (stream) {
            interval_results = TAILQ_LAST(&stream->result->interval_results, irlisthead);
            bandwidth = (double) bytes / (double) interval_results->interval_duration;
            avg_jitter /= test->num_streams;

            if (total_packets > 0) {
                lost_percent = 100.0 * lost_packets / total_packets;
            } else {
                lost_percent = 0.0;
            }

            // NSLog(@"Bandwidth on %d streams: %.2f Mbits/s (retransmits: %d, lost: %.2f%%, jitter: %.0f, interval: %.2fs)", test->num_streams, bandwidth * 8 / 1000000, retransmits, lost_percent, avg_jitter * 1000.0, interval_results->interval_duration);
//            status.bandwidth = bandwidth * 8 / 1000000;
            outputbandwidth = bandwidth * 8 / 1000000;


            if (test->timer) {
                float test_duration = (float) test->timer->usecs / SEC_TO_US;
                float test_elapsed = 0.0;

                // The timer bumps the duration before calling back, and is always 1 second too late
                // Remove 1 second to make sure we report accurate progress
                test_elapsed =
                        test_duration - (test->timer->time.secs - test->stats_timer->time.secs) -
                        1.0;
                outputprogress = test_elapsed / test_duration;

//                status.progress = test_elapsed / test_duration;
                // NSLog(@"Test duration: %.2fs (%d), elapsed: %.2f, progress: %.2f", test_duration, test->duration, test_elapsed, status.progress);
            } else {
                outputprogress = 1.0;
//                status.progress = 1.0;
            }

            // Send the status back to UI, make sure test is running on main thread
            //_callback(status);
            __android_log_print(ANDROID_LOG_DEBUG, "IPERF", "####### outputbandwidth %f",
                                (float) outputbandwidth);
            __android_log_print(ANDROID_LOG_DEBUG, "IPERF", "####### outputprogress %f",
                                (float) outputprogress);
        }
    }

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    assert (rs == JNI_OK);

    // Send data to Android
    (*env)->CallVoidMethod(env, g_obj, onRecvThroughpuyMethodID, (double)outputbandwidth, (double)outputprogress);
}

JNIEXPORT jlong JNICALL
Java_com_shubhendu_iperfandroid_NativeLib_startPerfTestJNI(JNIEnv *env, jobject thiz,
                                                           jstring j_logfile, jstring j_template,
                                                           jstring j_host, jint j_port, jint j_duration,
                                                           jint j_stream, jint j_reverse) {

    /////////////////

    // convert local to global reference
    // (local will die after this method call)
    g_obj = (*env)->NewGlobalRef(env, thiz);

    // save refs for callback
    jclass g_clazz = (*env)->GetObjectClass(env, g_obj);
    if(g_clazz != NULL) {
        onRecvThroughpuyMethodID = (*env)->GetMethodID(env, g_clazz,"onReceiveThroughput","(DD)V");
    }

    jint rs = (*env)->GetJavaVM(env, &jvm);
    assert (rs == JNI_OK);

    /////////////////////




    __android_log_print(ANDROID_LOG_DEBUG, "IPERF", "startperfios");
    struct iperf_test *test = iperf_new_test();
    __android_log_print(ANDROID_LOG_DEBUG, "IPERF", "new test %lld", (long long)test);

    int stream = j_stream; //5;
    int port = j_port; // 5201;
    int duration = j_duration; //30;
    int reverse = j_reverse; // 1;


    if (!test) {
        return 0;
    }

    if (iperf_defaults(test) < 0) {
        return 0;
    }

    iperf_set_test_role(test, 'c');
    iperf_set_test_num_streams(test, (int) stream);

    iperf_set_test_reverse(test, reverse);

    const char *hostname = (*env)->GetStringUTFChars(env, j_host, 0);
    if (hostname != NULL) {
        iperf_set_test_server_hostname(test, hostname);
    }
    __android_log_print(ANDROID_LOG_DEBUG, "IPERF", "host name set");

    iperf_set_test_server_port(test, (int) port);
    iperf_set_test_duration(test, (int) duration);
    const char *template = (*env)->GetStringUTFChars(env, j_template, 0);
    iperf_set_test_template(test, (char *) template);
    (*env)->ReleaseStringUTFChars(env, j_template, template);

    const char *logfile = (*env)->GetStringUTFChars(env, j_logfile, 0);
    test->logfile = strdup(logfile);
    (*env)->ReleaseStringUTFChars(env, j_logfile, logfile);


    test->settings->connect_timeout = 3000;
    i_errno = IENONE;
    test->reporter_callback = vc_reporter_callback;

    test->outfile = fopen(test->logfile, "a+");
    iperf_run_client(test);
    __android_log_print(ANDROID_LOG_DEBUG, "IPERF", "iperf_free_test");
    iperf_free_test(test);
}

