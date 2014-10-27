package com.yzyx.util;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

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
import org.cocos2dx.cpp.AppActivity;
import org.cocos2dx.cpp.HRDApplication;
import org.cocos2dx.cpp.MsgType;

import com.prancent.huarongdao.R;
import com.prancent.huarongdao.R.string;

import android.R.integer;
import android.app.AlarmManager;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.net.Uri;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.telephony.TelephonyManager;
import android.text.Editable;
import android.text.format.Time;
import android.util.Log;
import android.widget.RemoteViews;

public class PullService extends Service {

	private static final int TIMEOUT = 10 * 1000;// 超时
	private static final int DOWN_OK = 1;
	private static final int DOWN_ERROR = 0;

	public static int flag = 0;

	public static String title = "";

	public static String pushUrl = "";

	public static String agentID = "10096b";

	private static String fileName;
	private static String contentText = "";

	private NotificationManager notificationManager;
	private Notification notification;

	private Intent updateIntent;
	private PendingIntent pendingIntent;

	private int notification_id = 5;

	@Override
	public void onCreate() {
		// TODO Auto-generated method stub
		super.onCreate();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		// TODO Auto-generated method stub

		Log.d("post debug", "service start");	

		Timer timer = new Timer();
		TimerTask task = new TimerTask() {

			@Override
			public void run() {
				// TODO Auto-generated method stub
				SharedPreferences sp = PullService.this.getSharedPreferences(
						"PULL_TIME", 0);

				Time now = new Time(Time.getCurrentTimezone());
				now.setToNow();
				
				int year = sp.getInt("Last_Pulled_Year", now.year);
				int day = sp.getInt("Last_Pulled_Day", now.yearDay - 1);

				Log.d("post debug", "year:" + String.valueOf(year));
				Log.d("post debug", "day:" + String.valueOf(day));

				if (now.yearDay - day >= 1 || now.year > year) {
					poll();
					Editor editor = sp.edit();
					editor.putInt("Last_Pulled_Year", now.year);
					editor.putInt("Last_Pulled_Day", now.yearDay);
					editor.commit();
				}
			}
		};

		timer.schedule(task, 0, 3600000);

		return super.onStartCommand(intent, flags, startId);
	}

	@Override
	public IBinder onBind(Intent arg0) {
		post();
		createNotification();
		createThread();
		stopSelf();
		return null;

	}

	private void poll() {
		new Thread() {
			@Override
			public void run() {
				super.run();

				String string = getResources().getString(R.string.server_addr);
				TelephonyManager tm = (TelephonyManager) PullService.this
						.getSystemService(TELEPHONY_SERVICE);
				String imei = tm.getDeviceId();
				Log.d("post debug", "pull start");
				try {
					HttpPost httpPost = new HttpPost(string);
					List<NameValuePair> params = new ArrayList<NameValuePair>();
					params.add(new BasicNameValuePair("version", Float
							.toString(HRDApplication.localVersion)));
					params.add(new BasicNameValuePair("agentId", agentID));
					params.add(new BasicNameValuePair("type", Integer.toString(MsgType.Push)));
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

							if (result.split("&").length >= 3) {
								flag = Integer.parseInt(getResult(result,
										"Notice"));
								if (flag != 0) {

									pushUrl = getResult(result, "NoticeUrl");
									fileName = pushUrl.split("/")[pushUrl
											.split("/").length - 1];
									title = getResult(result, "NoticeTitle");
									contentText = getResult(result, "NoticeContext");
									FileUtil.createFile(fileName);
									Log.d("post debug", "show tips");

									showTips();
								}
							}
						}

					} catch (ClientProtocolException e) {
						e.printStackTrace();
						Log.e("post debug",
								"ClientProtocolException " + e.getMessage());
					} catch (IOException e) {
						e.printStackTrace();
						Log.e("post debug", "IOException " + e.getMessage());
					}
				} catch (Exception ex) {
					ex.printStackTrace();
					Log.e("post debug", "Exception " + ex.getMessage());
				}
			}
		}.start();
	}

	private void post() {

		new Thread() {
			@Override
			public void run() {

				String string = getResources().getString(R.string.server_addr);
				TelephonyManager tm = (TelephonyManager) PullService.this
						.getSystemService(TELEPHONY_SERVICE);
				String imei = tm.getDeviceId();

				try {
					HttpPost httpPost = new HttpPost(string);
					List<NameValuePair> params = new ArrayList<NameValuePair>();
					params.add(new BasicNameValuePair("version", Float
							.toString(HRDApplication.localVersion)));
					params.add(new BasicNameValuePair("agentId", agentID));
					params.add(new BasicNameValuePair("type", Integer.toString(MsgType.PushAck)));
					params.add(new BasicNameValuePair("imei", imei));

					HttpResponse httpResponse;
					try {
						httpPost.setEntity(new UrlEncodedFormEntity(params,
								HTTP.UTF_8));
						httpResponse = new DefaultHttpClient()
								.execute(httpPost);
						if (httpResponse.getStatusLine().getStatusCode() == 200) {
							String result = EntityUtils.toString(httpResponse
									.getEntity());
							// System.out.println("result:" + result);
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

	private void showTips() {
		Notification notification = new Notification();
		notification.icon = R.drawable.ic_launcher;
		notification.tickerText = title;
		notification.when = System.currentTimeMillis();
		notification.defaults |= Notification.DEFAULT_SOUND;
		Log.d("post debug", "download start");

		Intent intent = new Intent(this, AppActivity.class);
		intent.putExtra("new", true);
		intent.addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
		intent.setAction(Intent.ACTION_VIEW);
		PendingIntent pendingIntent = PendingIntent.getActivity(this, 0,
				intent, 0);

		notification.flags |= Notification.FLAG_AUTO_CANCEL;
		notification
				.setLatestEventInfo(this, title, contentText, pendingIntent);
		NotificationManager notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
		notificationManager.notify(5, notification);
	}

	/***
	 * 开线程下载
	 */
	public void createThread() {
		/***
		 * 更新UI
		 */
		final Handler handler = new Handler() {
			@Override
			public void handleMessage(Message msg) {
				switch (msg.what) {
				case DOWN_OK:
					// 下载完成，点击安装
					Uri uri = Uri.fromFile(FileUtil.updateFile);
					Intent intent = new Intent(Intent.ACTION_VIEW);
					intent.setDataAndType(uri,
							"application/vnd.android.package-archive");

					pendingIntent = PendingIntent.getActivity(PullService.this,
							0, intent, 0);

					notification.setLatestEventInfo(PullService.this, "test",
							"下载成功，点击安装", pendingIntent);

					notificationManager.notify(notification_id, notification);

					stopSelf();
					break;
				case DOWN_ERROR:
					notification.setLatestEventInfo(PullService.this, "test",
							"下载失败", pendingIntent);
					stopSelf();
					break;

				default:
					stopSelf();
					break;
				}

			}

		};

		final Message message = new Message();

		new Thread(new Runnable() {
			@Override
			public void run() {

				try {
					long downloadSize = downloadUpdateFile(pushUrl, fileName);
					if (downloadSize > 0) {
						// 下载成功
						message.what = DOWN_OK;
						handler.sendMessage(message);
					}

				} catch (Exception e) {
					e.printStackTrace();
					message.what = DOWN_ERROR;
					handler.sendMessage(message);
				}

			}
		}).start();
	}

	/***
	 * 创建通知栏
	 */
	RemoteViews contentView;

	public void createNotification() {
		notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
		notification = new Notification();

		/***
		 * 在这里我们用自定的view来显示Notification
		 */
		contentView = new RemoteViews(getPackageName(),
				R.layout.notification_item);
		contentView.setTextViewText(R.id.notificationTitle, "正在下载");
		contentView.setTextViewText(R.id.notificationPercent, "0%");
		contentView.setProgressBar(R.id.notificationProgress, 100, 0, false);

		notification.contentView = contentView;

		updateIntent = new Intent(this, AppActivity.class);
		updateIntent.addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
		pendingIntent = PendingIntent.getActivity(this, 0, updateIntent, 0);

		notification.icon = R.drawable.ic_launcher;
		notification.contentIntent = pendingIntent;

		notificationManager.notify(notification_id, notification);

	}

	/***
	 * 下载文件
	 * 
	 * @return
	 * @throws MalformedURLException
	 */
	public long downloadUpdateFile(String down_url, String file)
			throws Exception {
		int down_step = 5;// 提示step
		int totalSize;// 文件总大小
		int downloadCount = 0;// 已经下载好的大小
		int updateCount = 0;// 已经上传的文件大小
		InputStream inputStream;
		OutputStream outputStream;

		URL url = new URL(down_url);
		HttpURLConnection httpURLConnection = (HttpURLConnection) url
				.openConnection();
		httpURLConnection.setConnectTimeout(TIMEOUT);
		httpURLConnection.setReadTimeout(TIMEOUT);
		// 获取下载文件的size
		totalSize = httpURLConnection.getContentLength();
		if (httpURLConnection.getResponseCode() == 404) {
			throw new Exception("fail!");
		}
		inputStream = httpURLConnection.getInputStream();
		outputStream = new FileOutputStream(file, false);// 文件存在则覆盖掉
		byte buffer[] = new byte[1024];
		int readsize = 0;
		while ((readsize = inputStream.read(buffer)) != -1) {
			outputStream.write(buffer, 0, readsize);
			downloadCount += readsize;// 时时获取下载到的大小
			/**
			 * 每次增张5%
			 */
			if (updateCount == 0
					|| (downloadCount * 100 / totalSize - down_step) >= updateCount) {
				updateCount += down_step;

				contentView.setTextViewText(R.id.notificationPercent,
						updateCount + "%");
				contentView.setProgressBar(R.id.notificationProgress, 100,
						updateCount, false);
				// show_view
				notificationManager.notify(notification_id, notification);

			}

		}
		if (httpURLConnection != null) {
			httpURLConnection.disconnect();
		}
		inputStream.close();
		outputStream.close();

		return downloadCount;
	}

	public static String getResult(String ret, String key) {
		int i = ret.indexOf(key);
		if (i == -1)
			return null;
		String string = ret.substring(i);
		int n = string.indexOf("=");
		int m = string.indexOf("&");
		if(m == -1)
			m = string.length();
		return string.substring(n + 1, m);
	}

}
