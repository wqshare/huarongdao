package com.yzyx.util;

import android.util.Log;

public class myLog {
    public static void info(String str) {
        if (str != null)
            Log.i(MyConstants.TAG, str);
    }

    public static void debug(String str) {
        if (str != null && MyConstants.DEBUG)
            Log.d(MyConstants.TAG, str);
    }
}
