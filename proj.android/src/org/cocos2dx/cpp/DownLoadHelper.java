package org.cocos2dx.cpp;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.protocol.HTTP;
import org.apache.http.util.EntityUtils;

import android.R.anim;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources.NotFoundException;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Environment;
import android.os.Handler;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.widget.ImageView;
import android.widget.RemoteViews;
import android.widget.RemoteViews.RemoteView;

import com.prancent.huarongdao.R;
import com.yzyx.util.FileUtil;
import com.yzyx.util.PullService;

public class DownLoadHelper {

	protected Context _context;
	public String recommendFilePath;
	public Bitmap image;
	Handler handler;
	File apkFile;

	public DownLoadHelper(Context context) {
		_context = context;
	}

	public void downLoadAndParseMoreGameInfo(final String storePath) {

		new Thread() {

			public void run() {
				try {
					super.run();

					recommendFilePath = storePath;
					String postUri = _context.getResources().getString(
							R.string.server_addr);
					TelephonyManager tm = (TelephonyManager) _context
							.getSystemService(_context.TELEPHONY_SERVICE);
					String imei = tm.getDeviceId();

					HttpPost post = new HttpPost(postUri);
					List<NameValuePair> params = new ArrayList<NameValuePair>();
					params.add(new BasicNameValuePair("version", Integer
							.toString(HRDApplication.localVersion)));
					params.add(new BasicNameValuePair("agentId", "10096b"));
					params.add(new BasicNameValuePair("imei", imei));
					params.add(new BasicNameValuePair("type", Integer
							.toString(MsgType.RecommendSYN)));

					post.setEntity(new UrlEncodedFormEntity(params, HTTP.UTF_8));
					HttpResponse httpResponse = new DefaultHttpClient()
							.execute(post);
					if (httpResponse.getStatusLine().getStatusCode() == 200) {
						HttpEntity entity = httpResponse.getEntity();
						String result = EntityUtils.toString(entity);
						String xml_url = PullService.getResult(result, "url");

						// start download
						HttpGet get = new HttpGet(xml_url);
						httpResponse = new DefaultHttpClient().execute(get);
						if (httpResponse.getStatusLine().getStatusCode() == 200) {
							InputStream inputStream = httpResponse.getEntity()
									.getContent();

							Log.d("post debug", "recommendFilePath: "
									+ recommendFilePath);

							// OutputStream outputStream = new FileOutputStream(
							// recommendFilePath, false);
							OutputStream outputStream = _context
									.openFileOutput(recommendFilePath,
											Context.MODE_PRIVATE);
							byte[] buffer = new byte[4096];
							int count;
							while ((count = inputStream.read(buffer)) != -1) {
								outputStream.write(buffer, 0, count);
							}

							inputStream.close();
							outputStream.close();
							// start parse
							Log.d("post debug", "start parser xml");
							MoreGameInfoParser parser = new MoreGameInfoParser(
									DownLoadHelper.this._context);
							if (_context != null)
								((MoreGameActivity) _context)
										.onGetDataFinished(parser
												.getData(recommendFilePath));

						}
					}
				} catch (NotFoundException e) {
					e.printStackTrace();
				} catch (UnsupportedEncodingException e) {
					e.printStackTrace();
				} catch (ClientProtocolException e) {
					e.printStackTrace();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}

		}.start();
	}

	public void loadImageFromURL(final String imgUrl, final ImageView view) {

		new Thread() {

			public void run() {
				HttpGet get = new HttpGet(imgUrl);
				try {

					HttpResponse response = new DefaultHttpClient()
							.execute(get);

					if (response.getStatusLine().getStatusCode() == 404)
						throw new ClientProtocolException();

					InputStream inputStream = response.getEntity().getContent();
					ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
					byte[] buffer = new byte[4096];
					int count;
					while ((count = inputStream.read(buffer)) != -1) {
						outputStream.write(buffer, 0, count);
					}
					byte[] imageArray = outputStream.toByteArray();
					inputStream.close();
					outputStream.close();

					if (_context != null)
						((MoreGameActivity) _context).onLoadImageDone(view,
								BitmapFactory.decodeByteArray(imageArray, 0,
										imageArray.length));

				} catch (ClientProtocolException e) {
					e.printStackTrace();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}

		}.start();

	}

	public void downLoadAPKWithNotification(final String apkURL,
			final String apkName, final String storePath,
			final int notificationID) throws IOException {

		handler = new Handler();

		if (Environment.getExternalStorageState().equals(
				Environment.MEDIA_MOUNTED)) {
			apkFile = new File(Environment.getExternalStorageDirectory() + "/"
					+ storePath + "/" + apkName);
			if (!apkFile.exists())
				apkFile.createNewFile();
		}

		NotificationManager manager = (NotificationManager) _context
				.getSystemService(Context.NOTIFICATION_SERVICE);

		// notify start download
		Notification notification = new Notification();
		RemoteViews contentViews = new RemoteViews(_context.getPackageName(),
				R.layout.notification_item);
		Intent intent = new Intent(_context, AppActivity.class);
		PendingIntent contentIntent = PendingIntent.getActivity(_context, 0,
				intent, PendingIntent.FLAG_UPDATE_CURRENT);

		contentViews.setTextViewText(R.id.notificationTitle, "正在下载");
		contentViews.setTextViewText(R.id.notificationPercent, "0%");
		contentViews.setProgressBar(R.id.notificationProgress, 100, 0, false);

		notification.defaults = Notification.DEFAULT_SOUND
				| Notification.DEFAULT_VIBRATE;
		notification.flags |= Notification.FLAG_AUTO_CANCEL;
		notification.contentIntent = contentIntent;
		notification.contentView = contentViews;
		notification.icon = R.drawable.ic_launcher;
		notification.when = System.currentTimeMillis();
		notification.tickerText = "正在下载...";

		manager.notify(notificationID, notification);

		// start download thread
		new Thread() {

			public void run() {
				super.run();

				try {
					HttpGet get = new HttpGet(apkURL);
					HttpResponse response = new DefaultHttpClient()
							.execute(get);
					if (response.getStatusLine().getStatusCode() == 404) {
						throw new ClientProtocolException();
					}

					InputStream inputStream = response.getEntity().getContent();
					OutputStream outputStream = new FileOutputStream(apkFile,
							false);
					int count;
					long totalSize = response.getEntity().getContentLength();
					long receiveSize = 0;
					float precent = 0.0f;
					byte[] buffer = new byte[4096];
					while ((count = inputStream.read(buffer)) != -1) {
						outputStream.write(buffer, 0, count);
						receiveSize += count;

						// change precent in notification
						if (receiveSize * 100 / totalSize - precent >= 5.0) {
							precent = receiveSize * 100 / totalSize;
							NotificationManager manager = (NotificationManager) _context
									.getSystemService(Context.NOTIFICATION_SERVICE);

							Notification notification = new Notification();
							RemoteViews contentViews = new RemoteViews(
									_context.getPackageName(),
									R.layout.notification_item);
							Intent intent = new Intent(_context,
									AppActivity.class);
							PendingIntent contentIntent = PendingIntent
									.getActivity(_context, 0, intent,
											PendingIntent.FLAG_UPDATE_CURRENT);

							contentViews.setTextViewText(
									R.id.notificationTitle, "正在下载");
							contentViews.setTextViewText(
									R.id.notificationPercent,
									String.valueOf(precent) + "%");
							contentViews.setProgressBar(
									R.id.notificationProgress, 100,
									(int) precent, false);

							notification.flags |= Notification.FLAG_AUTO_CANCEL;
							notification.contentIntent = contentIntent;
							notification.contentView = contentViews;
							notification.icon = R.drawable.ic_launcher;
							notification.when = System.currentTimeMillis();
							notification.tickerText = "正在下载...";

							manager.notify(notificationID, notification);
						}
					}

					inputStream.close();
					outputStream.flush();
					outputStream.close();

					// notify to install
					NotificationManager manager = (NotificationManager) _context
							.getSystemService(Context.NOTIFICATION_SERVICE);

					Notification notification = new Notification();
					Intent intent = new Intent(Intent.ACTION_VIEW);
					intent.setDataAndType(Uri.fromFile(apkFile),
							"application/vnd.android.package-archive");
					PendingIntent contentIntent = PendingIntent.getActivity(
							_context, 0, intent,
							PendingIntent.FLAG_UPDATE_CURRENT);

					notification.defaults = Notification.DEFAULT_SOUND
							| Notification.DEFAULT_VIBRATE;
					notification.flags |= Notification.FLAG_AUTO_CANCEL;
					notification.contentIntent = contentIntent;
					notification.icon = android.R.drawable.stat_sys_download_done;
					notification.when = System.currentTimeMillis();
					notification.setLatestEventInfo(_context, apkName,
							"下载完成，点击安装", contentIntent);

					manager.notify(notificationID, notification);

				} catch (ClientProtocolException e) {
					e.printStackTrace();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}

		}.start();

	}
}
