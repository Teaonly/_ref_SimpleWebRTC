package com.beginvision.simplertc;

import java.io.*; 
import java.net.*;

import android.net.*;
import android.util.Log;

public class NativeAgent{

    public static native int jmain(String target, int port, String myname); 

    public static void init() { 
        System.loadLibrary("jingle_simplertc_so");
    }   
}
