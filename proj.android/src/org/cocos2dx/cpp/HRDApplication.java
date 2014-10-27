package org.cocos2dx.cpp;

import com.prancent.huarongdao.R;
import com.yzyx.util.PullService;
import com.yzyx.util.UpdateService;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.protocol.HTTP;
import org.apache.http.util.EntityUtils;

import android.R.integer;
import android.app.AlertDialog;
import android.app.Application;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Looper;
import android.telephony.TelephonyManager;
import android.util.Log;

public class HRDApplication extends Application {

	public static int localVersion = 0;// 本地安装版本

	public static int serverVersion = 0;// 服务器版本

	public static String downloadDir = "huarongdao/";
	
	public static boolean flag = false;
	
	public static String pushUrl = ""; 

	@Override
	public void onCreate() {
		// TODO Auto-generated method stub
		super.onCreate();

		try {
			Log.d("post debug",
					PullService.getResult("sdjfksjdlkfjsldjflsjdlf& what=test&ksdfsd", "what"));
			PackageInfo packageInfo = getApplicationContext()
					.getPackageManager().getPackageInfo(getPackageName(), 0);
			localVersion = packageInfo.versionCode;		
			getServerVersion();
			
			startPollingService();
		} catch (NameNotFoundException e) {
			// TODO: handle exception
			e.printStackTrace();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

	public void getServerVersion() throws Exception {

		new Thread() {
			@Override
			public void run() {
				super.run();

				String string = getResources().getString(R.string.server_addr);
				Log.d("post debug", "Start check version");
				try {
					HttpPost httpPost = new HttpPost(string);
					List<NameValuePair> params = new ArrayList<NameValuePair>();
					params.add(new BasicNameValuePair("version", Integer
							.toString(localVersion)));
					params.add(new BasicNameValuePair("agentId", "10096b"));
					params.add(new BasicNameValuePair("type", Integer.toString(MsgType.CheckUpdate)));
					TelephonyManager tm = (TelephonyManager) HRDApplication.this
							.getSystemService(TELEPHONY_SERVICE);
					String imei = tm.getDeviceId();
					params.add(new BasicNameValuePair("imei", imei));
					HttpResponse httpResponse;
					try {
						httpPost.setEntity(new UrlEncodedFormEntity(params,
								HTTP.UTF_8));
						httpResponse = new DefaultHttpClient()
								.execute(httpPost);
						if (httpResponse.getStatusLine().getStatusCode() == 200) {

							HttpEntity httpEntity = httpResponse.getEntity();
							String result = EntityUtils.toString(httpEntity);
							Log.d("post debug", "result:" + result);

							serverVersion = Integer
									.parseInt(PullService.getResult(result, "version"));
							UpdateService.down_url = PullService.getResult(result, "address");
							Log.d("post debug", "ServerVersion:" + serverVersion);
							Log.d("post debug", "DownLoadUrl:" + UpdateService.down_url);								
							
							checkVersion();
						}

					} catch (ClientProtocolException e) {
						e.printStackTrace();
						Log.e("post debug","ClientProtocolException " + e.getMessage());
					} catch (IOException e) {
						e.printStackTrace();
						Log.e("post debug","IOException " + e.getMessage());
					}
				} catch (Exception ex) {
					ex.printStackTrace();
					Log.e("post debug", "Exception " + ex.getMessage());
				}
			}
		}.start();

	}
	
	
	public void checkVersion() {
		if (HRDApplication.localVersion < HRDApplication.serverVersion) {

			Looper.prepare();
			// 发现新版本，提示用户更新
			AlertDialog.Builder alert = new AlertDialog.Builder(this);
			alert.setTitle("游戏升级")
					.setMessage("发现新版本,建议立即更新使用.")
					.setPositiveButton("更新",
							new DialogInterface.OnClickListener() {
								@Override
								public void onClick(DialogInterface dialog,
										int which) {

									Intent updateIntent = new Intent(
											getApplicationContext(),
											UpdateService.class);
									updateIntent.putExtra(
											"app_name",
											getResources().getString(
													com.prancent.huarongdao.R.string.app_name));
									Log.d("post debug", "Start Service");

									startService(updateIntent);
								}
							})
					.setNegativeButton("取消",
							new DialogInterface.OnClickListener() {
								@Override
								public void onClick(DialogInterface dialog,
										int which) {
									dialog.dismiss();
								}
							});
			alert.create().show();
		}

	}
	
	
	public void startPollingService()
	{
		IntentFilter intentFilter = new IntentFilter(Intent.ACTION_USER_PRESENT);
		MyBroadcastReceiver myBroadcastReceiver = new MyBroadcastReceiver();
		registerReceiver(myBroadcastReceiver, intentFilter);
	}

}
