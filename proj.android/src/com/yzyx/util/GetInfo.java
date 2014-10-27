package com.yzyx.util;

import android.content.Context;
import android.os.Build;
import android.provider.Settings;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.DisplayMetrics;

public class GetInfo {

    public static String getUid(Context context) {
        String returnStr = "None";
        try {
            String s = getString(context, "com.prancent.deviceid1");
            if (!TextUtils.isEmpty(s)) {
                myLog.debug("getUid= " + s);
                return s;
            } else {
                try {
                    String a = String.valueOf(Build.VERSION.SDK_INT);
                    String b = Build.MODEL;
                    String c = Build.ID;
                    String d = Build.DISPLAY;
                    String e = Build.PRODUCT;
                    String f = Build.DEVICE;
                    String g = Build.BOARD;
                    String h = Build.BRAND;
                    String i = Build.SERIAL;
                    String j = Build.MANUFACTURER;
                    String k = "Android";

                    String buildInfo = addParam(a, b, c, d, e, f, g, h, i, j, k);
                    myLog.debug("getUid= " + buildInfo);
                    returnStr = MyDes.desEncrypt(buildInfo);
                    putString(context, "com.prancent.deviceid1", returnStr);

                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return returnStr;
    }

    public static String getUa(Context context,String version) {
        String returnStr = "";
        try {
            returnStr = getString(context, "com.prancent.deviceid2");
            myLog.debug("getUa= " + returnStr);
            if (TextUtils.isEmpty(returnStr)) {
                DisplayMetrics displayMetrics = context.getResources().getDisplayMetrics();
                //density=2.0, width=768, height=1280, scaledDensity=2.0, xdpi=159.895, ydpi=160.157
                String str = String.valueOf(displayMetrics.widthPixels) + "|" + displayMetrics.heightPixels + "|Android|" + version + "|" + displayMetrics.density;
                myLog.debug("ua = " + str);
                returnStr = MyDes.desEncrypt(str);
                putString(context, "com.prancent.deviceid2", returnStr);
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return returnStr;
    }

    public static String getUt(Context context) {
        String returnStr = "";
        try {
            returnStr=getString(context,"com.prancent.deviceid3");
            myLog.debug("getUt = " + returnStr);
            if(TextUtils.isEmpty(returnStr)) {
                String android_id = Settings.Secure.getString(context.getContentResolver(), Settings.Secure.ANDROID_ID);
                if (TextUtils.isEmpty(android_id)) {
                    android_id = "none";
                }
                String imei = getImei(context);
                if (TextUtils.isEmpty(imei)) {
                    imei = "none";
                }
                String str1 = android_id + "|Android|" + imei;
                myLog.debug("getUt = " + str1);
                returnStr=MyDes.desEncrypt(str1);
                putString(context, "com.prancent.deviceid3", returnStr);

            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return returnStr;
    }
    private static String getImei(Context context) {
        String deviceId;
        try {
            TelephonyManager tm = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
            deviceId = tm.getDeviceId();
        } catch (Exception ex) {
            deviceId = "";
            ex.printStackTrace();
        }
        return deviceId;
    }

    private static String getString(Context context, String string) {
        return Settings.System.getString(context.getContentResolver(), string);
    }
    private static void putString(Context context,String name,String value) {
        try {
            Settings.System.putString(context.getContentResolver(), name, value);
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }



    private static String addParam(String... args) {
        StringBuilder stringBuilder = new StringBuilder();
        boolean exist = false;
        for (String s : args) {
            stringBuilder.append(changeParam(s)).append("|");
            exist = true;
        }
        if (exist) stringBuilder.deleteCharAt(stringBuilder.length() - 1);
        return stringBuilder.toString();
    }
    private static String changeParam(String string){
        if(TextUtils.isEmpty(string)){
            return string;
        }
        return string.replace('|','_');
    }
}
