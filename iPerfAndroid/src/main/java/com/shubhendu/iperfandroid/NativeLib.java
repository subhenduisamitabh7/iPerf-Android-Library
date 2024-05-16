package com.shubhendu.iperfandroid;

public class NativeLib {

    // Used to load the 'iperfandroid' library on application startup.
    static {
        System.loadLibrary("iperfandroid");
    }

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
}