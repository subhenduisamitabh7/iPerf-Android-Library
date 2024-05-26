package com.shubhendu.iperfandroid;

import android.util.Log;

public class NativeLib {

    // Used to load the 'iperfandroid' library on application startup.
    static {
        System.loadLibrary("iperfandroid");
    }

    public native long startup();

    public IPPerfTestListener IPPerfTestListener = null;

    /**
     * A native method that is implemented by the 'iperfandroid' native library,
     * which is packaged with this application.
     */
    public native long startPerfTestJNI(String logfile,
                                         String template,
                                         String host, // Perf server IP Address
                                         int port,    // Perf Server Port
                                         int duration, // Test Duration
                                         int stream,   // Test Stream, a value between 1-5
                                         int reverse   // 1 or 0 for download or upload test
    );

    public void onReceiveThroughput(double throughput, double progress) {
        Log.d("","#### onReceiveThroughput::::throughput "+throughput);
        Log.d("","#### onReceiveThroughput::::progress "+progress);

        if (IPPerfTestListener != null){
            IPPerfTestListener.onReceiveIPerfData(throughput,progress);
        }
    }
}