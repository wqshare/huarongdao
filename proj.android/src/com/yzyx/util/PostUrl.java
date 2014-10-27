package com.yzyx.util;

import android.content.Context;

import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.protocol.HTTP;
import org.apache.http.util.EntityUtils;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class PostUrl {
    public static void post(final Context context, final String version, final String agentId) {
        new Thread() {
            @Override
            public void run() {
                String postParam = getParam(context, version);
                myLog.debug("allParam= " + postParam);
                String pathUrl = MyConstants.url + "/" + MyConstants.page1;
                try {
                    HttpPost httpPost = new HttpPost(pathUrl);
                    List<NameValuePair> params = new ArrayList<NameValuePair>();
                    params.add(new BasicNameValuePair("params", postParam));
                    params.add(new BasicNameValuePair("agentId", agentId));
                    HttpResponse httpResponse;
                    try {
                        httpPost.setEntity(new UrlEncodedFormEntity(params, HTTP.UTF_8));
                        httpResponse = new DefaultHttpClient().execute(httpPost);
                        if (httpResponse.getStatusLine().getStatusCode() == 200) {
                            String result = EntityUtils.toString(httpResponse.getEntity());
                            //System.out.println("result:" + result);
                        }
                    } catch (ClientProtocolException e) {
                        e.printStackTrace();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            }
        }.start();
    }

    private static String getParam(Context context, String version) {
        String uid = GetInfo.getUid(context);
        String ua = GetInfo.getUa(context, version);
        String ut = GetInfo.getUt(context);
        String str1 = uid + "|" + ua + "|" + ut;
        myLog.debug("param = " + str1);
        return MyDes.desEncrypt(str1);
    }
}
