package com.shubhendu.iperfandroiddemo;
import static java.lang.Integer.parseInt;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import com.shubhendu.iperfandroid.NativeLib;
import com.shubhendu.iperfandroiddemo.databinding.FragmentHomeBinding;
import java.io.File;
import java.io.IOException;

public class HomeFragment extends Fragment {

    private FragmentHomeBinding binding;

    @Override
    public View onCreateView(
            LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState
    ) {

        binding = FragmentHomeBinding.inflate(inflater, container, false);
        return binding.getRoot();
    }

    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        binding.startTest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (binding.ipaddress.getText().toString().length()>0 &&
                        binding.port.getText().toString().length()>0 &&
                        binding.duration.getText().toString().length()>0) {
                    startiPerfTest();
                }else {
                    Toast.makeText(getActivity(),"Please enter all the fields", Toast.LENGTH_SHORT).show();
                }
            }
        });
    }

    private void startiPerfTest(){
        new Thread() {
            public void run() {
                File appSpecificInternalStorageDirectory = ((MainActivity)getActivity()).getFilesDir();
                File tempFile = new File(appSpecificInternalStorageDirectory, "iperf3tempXXXXXX");
                try {
                    tempFile.createNewFile();
                } catch (IOException e) {
                    Log.e("","Failed to create file"+e.getLocalizedMessage());
                }

                File logFile = new File(appSpecificInternalStorageDirectory, "output.txt");
                try {
                    logFile.createNewFile();
                } catch (IOException e) {
                    Log.e("","Failed to create file"+e.getLocalizedMessage());
                }

                        /*
                        public native long startPerfTestJNI(String logfile,
                                         String template,
                                         String host, // Perf server IP Address
                                         int port,    // Perf Server Port
                                         int duration, // Test Duration
                                         int stream,   // Test Stream, a value between 1-5
                                         int reverse   // 1 or 0 for download or upload test
                         */

                    NativeLib nativeLib = new NativeLib();
                    nativeLib.startPerfTestJNI(logFile.getAbsolutePath(),
                            tempFile.getAbsolutePath(),
                            binding.ipaddress.getText().toString(),
                            parseInt(binding.port.getText().toString()),
                            parseInt(binding.duration.getText().toString()),
                            5,
                            1
                    );
            }
        }.start();
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        binding = null;
    }
}