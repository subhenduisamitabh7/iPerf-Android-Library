package com.shubhendu.iperfandroid;

public interface IPPerfTestListener {
    void onReceiveIPerfData(double throughput, double progress);
}
