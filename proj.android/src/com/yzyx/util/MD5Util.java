package com.yzyx.util;



import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
public class MD5Util {
    public static String toHexString(byte[] paramArrayOfByte, String paramString, boolean paramBoolean)
    {
        StringBuilder localStringBuilder = new StringBuilder();
        int i = paramArrayOfByte.length;
        for (byte aParamArrayOfByte : paramArrayOfByte) {
            String str = Integer.toHexString(0xFF & aParamArrayOfByte);
            if (paramBoolean)
                str = str.toUpperCase();
            if (str.length() == 1)
                localStringBuilder.append("0");
            localStringBuilder.append(str).append(paramString);
        }
        return localStringBuilder.toString();
    }

    public static String toMd5(byte[] paramArrayOfByte, boolean paramBoolean)
    {
        try
        {
            MessageDigest localMessageDigest = MessageDigest.getInstance("MD5");
            localMessageDigest.reset();
            localMessageDigest.update(paramArrayOfByte);
            return toHexString(localMessageDigest.digest(), "", paramBoolean);
        }
        catch (NoSuchAlgorithmException localNoSuchAlgorithmException)
        {
            throw new RuntimeException(localNoSuchAlgorithmException);
        }
    }
}
