package com.yzyx.util;

import android.util.Base64;

import javax.crypto.Cipher;
import javax.crypto.SecretKey;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.DESKeySpec;
import javax.crypto.spec.IvParameterSpec;

public class MyDes {
    public static String desEncrypt(String message) {
        return desEncrypt(message, MyConstants.desKey);
    }

    private static String desEncrypt(String message, String key) {
        String str = "None";
        try {
            Cipher cipher = Cipher.getInstance("DES/CBC/PKCS5Padding");
            DESKeySpec desKeySpec = new DESKeySpec(key.getBytes("UTF-8"));
            SecretKeyFactory keyFactory = SecretKeyFactory.getInstance("DES");
            SecretKey secretKey = keyFactory.generateSecret(desKeySpec);
            IvParameterSpec iv = new IvParameterSpec(key.getBytes("UTF-8"));
            cipher.init(Cipher.ENCRYPT_MODE, secretKey, iv);
            byte[] bytes = cipher.doFinal(message.getBytes("UTF-8"));
            str = Base64.encodeToString(bytes, Base64.DEFAULT);
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return str;
    }
}
