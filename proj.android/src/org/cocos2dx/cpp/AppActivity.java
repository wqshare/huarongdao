/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 ****************************************************************************/
package org.cocos2dx.cpp;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
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
import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;
import org.cocos2dx.lib.Cocos2dxHelper;

import android.R.integer;
import android.R.string;
import android.app.AlertDialog;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;

import com.android.easou.epay.EPayActivity;
import com.android.easou.epay.pay.Epay;
import com.bodong.dianjinweb.DianJinPlatform;
import com.bodong.dianjinweb.banner.DianJinBanner;
import com.bodong.dianjinweb.listener.AppActiveListener;
import com.yzyx.util.PostUrl;
import com.yzyx.util.PullService;
import com.yzyx.util.UpdateService;
import com.prancent.huarongdao.*;

import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.preference.PreferenceManager.OnActivityResultListener;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

public class AppActivity extends Cocos2dxActivity {

	private static final int ID_NOTIFICATION = 1;
	private static HRDApplication myApplication;
	private DianJinBanner banner = null;

	private static native void showPayResult(int requestCode);

	private static native void reCharge1();

	private static native void reCharge3();

	private static native void updateMissionFile(String path);

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		// checkVersion();

		/*
		 * ================================== dianjin sdk
		 * ====================================
		 */
		DianJinPlatform.initialize(AppActivity.this, 56251,
				"e7865739d99ea003d3d115b9cafcd43f");
		DianJinPlatform.hideFloatView(AppActivity.this);
		DianJinPlatform.setAppActivedListener(new AppActiveListener() {

			@Override
			public void onSuccess(long reward) {
				Log.d("com.yzyx.huarongdao", "++++1");
				Toast.makeText(AppActivity.this, "激活成功，获得1本葵花宝典奖励！：",
						Toast.LENGTH_SHORT).show();
				reCharge1();

			}

			@Override
			public void onError(int errorCode, String errorMessage) {
				switch (errorCode) {
				case DianJinPlatform.DIANJIN_NET_ERROR:// 网络不稳定
					Toast.makeText(AppActivity.this, errorMessage,
							Toast.LENGTH_SHORT).show();
					break;
				case DianJinPlatform.DIANJIN_DUPLICATE_ACTIVATION:// 重复激活
					Toast.makeText(AppActivity.this, errorMessage,
							Toast.LENGTH_SHORT).show();
					break;

				case DianJinPlatform.DIANJIN_ADVERTSING_EXPIRED:// 应用已下架
					Toast.makeText(AppActivity.this, errorMessage,
							Toast.LENGTH_SHORT).show();
					break;

				case DianJinPlatform.DIANJIN_ACTIVATION_FAILURE:// 激活失败
					Toast.makeText(AppActivity.this, errorMessage,
							Toast.LENGTH_SHORT).show();
					break;

				default:
					break;
				}
			}
		});
		/*
		 * ================================== dianjin sdk
		 * ====================================
		 */
		this.hideSystemUI();

		Cocos2dxHelper
				.addOnActivityResultListener(new OnActivityResultListener() {
					@Override
					public boolean onActivityResult(int requestCode,
							int resultCode, Intent data) {
						Log.d("com.yzyx.huarongdao", " requestCode: "
								+ requestCode + " resultCode： " + resultCode);
						if (requestCode == Epay.REQUESTCODE) {
							switch (resultCode) {
							case Epay.FEE_RESULT_SUCCESS:
								Toast.makeText(AppActivity.this,
										"支付成功, 感谢您的支持，三本葵花宝典已经入手！",
										Toast.LENGTH_LONG).show();
								reCharge3();
								postRecharge(1);
								break;
							case Epay.FEE_RESULT_FAILED:
								Toast.makeText(AppActivity.this, "支付失败",
										Toast.LENGTH_SHORT).show();
								postRecharge(3);
								break;
							case Epay.FEE_RESULT_CANCELED:
								Toast.makeText(AppActivity.this, "支付取消",
										Toast.LENGTH_SHORT).show();
								postRecharge(2);
							default:
								break;
							}
							showPayResult(requestCode);
						}
						return true;
					}

				});
	}

	@Override
	protected void onNewIntent(Intent intent) {
		// TODO Auto-generated method stub
		super.onNewIntent(intent);
		Log.d("post debug", "onNewIntent");
		bindService(new Intent(this, PullService.class),
				new ServiceConnection() {

					@Override
					public void onServiceDisconnected(ComponentName arg0) {
						// TODO Auto-generated method stub

					}

					@Override
					public void onServiceConnected(ComponentName arg0,
							IBinder arg1) {
						// TODO Auto-generated method stub
						Log.d("post debug", "onServiceConnected");
					}
				}, Service.BIND_AUTO_CREATE);

	}

	@Override
	public Cocos2dxGLSurfaceView onCreateView() {
		Cocos2dxGLSurfaceView glSurfaceView = new Cocos2dxGLSurfaceView(this);
		glSurfaceView.setEGLConfigChooser(5, 6, 5, 0, 16, 8);
		return glSurfaceView;
	}

	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		DianJinPlatform.destory(this);
	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);
		if (hasFocus) {
			this.hideSystemUI();
		}
	}

	public void enterOffWall() {
		DianJinPlatform.showOfferWall(this);
		postDianJinInfo();
	}

	private void hideSystemUI() {
		// Set the IMMERSIVE flag.
		// Set the content to appear under the system bars so that the content
		// doesn't resize when the system bars hide and show.
		View rootView = this.getWindow().getDecorView();
		rootView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE
				| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
				| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
				| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION // hide
														// nav
														// bar
				| View.SYSTEM_UI_FLAG_FULLSCREEN // hide
													// status
													// bar
				| View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
	}

	public void payForCheat() {
		Intent intent = new Intent();
		intent.setClass(AppActivity.this, EPayActivity.class);

		intent.putExtra(Epay.EPAY_FEE, 300);// 计费金额以分为单位
		intent.putExtra(Epay.EPAY_CPID, 1006);// CpID，在后台申请
		intent.putExtra(Epay.EPAY_APPFEE_ID, 1001);// 计费ID，在后台申请
		intent.putExtra(
				// 用户私钥
				Epay.EPAY_CP_PRIVATE_KEY,
				"MIICdgIBADANBgkqhkiG9w0BAQEFAASCAmAwggJcAgEAAoGBAN6lbhAMap2TpVc9B8Y74z73EDhm9MXEqKRH/STPrfsKuAGP5OnqonFszkU6tN1q4FJ2jRWOPfAcJshH2/YJ/rVq2pIYfJztqrYf0usb6WNRA8HoEfhaSHQSOy6+Ln3UfL21p3KNsN/aDUK022NxQPkmiko7vDknnpYuwPnt/cKVAgMBAAECgYBZTAZZv3EfZDFDTQIKOsjI3zSNOpXfoCF3Sjw4gGcqMcFdNwY731Rpo5cTYxlZWN1BlbS+OX+UBuZzG4cYJigWgZMyE9xSwS5HV7JJWgemFPI6jPjFsi6KZpqFjMURr24SC2V/T3TFJZxux9rbC1v3JrVc1Uy46fh+x3uOuauH/QJBAPACFAKdHbplMEZdjnlUVmKeESmmJxFGVy4p3EfJKhRdEPp3usSS+SdXdz39vekVa6jNr/iFrPT1Mc025TRdlxsCQQDtezPF95YNwU4y46F9Ub2PIhgbKBLScET8kCCnAkNpo+lftW7UCKxTgHy87MDeSSfOKN7DVFQhlaSfTafdnDgPAkB8/XgSTGR4982GN57FDOjX/t2NRRw/YNRyFBbFw7uIU1dOOSAo0ymyFVjiDczjH+arSEMGOAd0omTHLHl9JehVAkEA2UPcDAQFXjQ6DMjA19xOpoK0tzOkdK6ODtr1ZJXnguCZBB0Ddi0xK3vjoxyfHz6ikjRGCUI/bjJv1+a96zue9QJAU9SBecfVh/zx+L38LDZMh5Di54+kXfOwDdRxZc58qazL2JDylv3/EhxyAYdkD8+GT8ePZH6MG9jEKq9mShZjbA==");// 用户私钥
		intent.putExtra(Epay.EPAY_CP_ORDER_ID, "1006");// 用户定单ID

		intent.putExtra(Epay.EPAY_APPNAME, "葵花宝典");// 商品名称

		this.startActivityForResult(intent, Epay.REQUESTCODE);
	}

	public void notifyMe() {
		NotificationManager notifyManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
		Notification notification = new Notification();

		notification.icon = com.prancent.huarongdao.R.drawable.ic_launcher;
		notification.defaults |= Notification.DEFAULT_SOUND;
		notification.tickerText = "hello";
		notification.when = java.lang.System.currentTimeMillis();

		Intent notificationIntent = new Intent(this, AppActivity.class);
		PendingIntent contentIntent = PendingIntent.getActivity(this, 0,
				notificationIntent, 0);
		notification.setLatestEventInfo(getApplicationContext(), "HuaRongDao",
				"test", contentIntent);

		notifyManager.notify(ID_NOTIFICATION, notification);
	}

	public void checkVersion() {
		myApplication = (HRDApplication) getApplication();
		if (HRDApplication.localVersion < HRDApplication.serverVersion) {

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
											AppActivity.this,
											UpdateService.class);
									updateIntent
											.putExtra(
													"app_name",
													getResources()
															.getString(
																	com.prancent.huarongdao.R.string.app_name));
									Log.d("debug", "Start Service");

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

	public static Object getInstance() {
		return getContext();
	}

	public static void post() {
		PostUrl.post(getContext(), Float.toString(HRDApplication.localVersion),
				"10096b");
	}

	public void updateMission() {
		new Thread() {
			@Override
			public void run() {
				super.run();

				String string = getResources().getString(R.string.server_addr);
				TelephonyManager tm = (TelephonyManager) AppActivity.this
						.getSystemService(TELEPHONY_SERVICE);
				String imei = tm.getDeviceId();
				Log.d("post debug", "updateMission start");
				try {
					HttpPost httpPost = new HttpPost(string);
					List<NameValuePair> params = new ArrayList<NameValuePair>();
					params.add(new BasicNameValuePair("version", Float
							.toString(HRDApplication.localVersion)));
					params.add(new BasicNameValuePair("agentId",
							PullService.agentID));
					params.add(new BasicNameValuePair("type", Integer
							.toString(MsgType.MissionUpdate)));
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

							String updateAddr = PullService.getResult(result,
									"updateAddress");

							String path = Environment
									.getExternalStorageDirectory()
									+ "/"
									+ "huarongdao"
									+ updateAddr.split("/")[updateAddr
											.split("/").length - 1];
							File file = new File(path);
							if (file.exists())
								file.delete();
							file.createNewFile();

							InputStream inputStream;
							OutputStream outputStream;
							URL url = new URL(updateAddr);
							HttpURLConnection httpURLConnection = (HttpURLConnection) url
									.openConnection();
							httpURLConnection.setConnectTimeout(10000);
							httpURLConnection.setReadTimeout(10000);

							if (httpURLConnection.getContentLength() <= 0)
								return;

							if (httpURLConnection.getResponseCode() == 404) {
								throw new Exception("fail!");
							}

							inputStream = httpURLConnection.getInputStream();
							outputStream = new FileOutputStream(file);

							byte[] buff = new byte[1024];
							while (inputStream.read(buff) != -1) {
								outputStream.write(buff);
							}
							outputStream.flush();

							if (httpURLConnection != null)
								httpURLConnection.disconnect();

							inputStream.close();
							outputStream.close();

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

	public void postMissionIdx(final int missionIdx) {
		new Thread() {
			@Override
			public void run() {

				String string = getResources().getString(R.string.server_addr);
				TelephonyManager tm = (TelephonyManager) AppActivity.this
						.getSystemService(TELEPHONY_SERVICE);
				String imei = tm.getDeviceId();

				try {
					HttpPost httpPost = new HttpPost(string);
					List<NameValuePair> params = new ArrayList<NameValuePair>();
					params.add(new BasicNameValuePair("version", Float
							.toString(HRDApplication.localVersion)));
					params.add(new BasicNameValuePair("agentId",
							PullService.agentID));
					params.add(new BasicNameValuePair("type", Integer
							.toString(MsgType.MissionIdx)));
					params.add(new BasicNameValuePair("imei", imei));
					params.add(new BasicNameValuePair("guanka", Integer
							.toString(missionIdx)));

					HttpResponse httpResponse;
					try {
						httpPost.setEntity(new UrlEncodedFormEntity(params,
								HTTP.UTF_8));
						httpResponse = new DefaultHttpClient()
								.execute(httpPost);
						if (httpResponse.getStatusLine().getStatusCode() == 200) {
							Log.d("post debug", "postMissionIdx success");
							String result = EntityUtils.toString(httpResponse
									.getEntity());
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

	public void postRecharge(final int status) {
		new Thread() {
			@Override
			public void run() {

				String string = getResources().getString(R.string.server_addr);
				TelephonyManager tm = (TelephonyManager) AppActivity.this
						.getSystemService(TELEPHONY_SERVICE);
				String imei = tm.getDeviceId();

				try {
					HttpPost httpPost = new HttpPost(string);
					List<NameValuePair> params = new ArrayList<NameValuePair>();
					params.add(new BasicNameValuePair("version", Float
							.toString(HRDApplication.localVersion)));
					params.add(new BasicNameValuePair("agentId",
							PullService.agentID));
					params.add(new BasicNameValuePair("type", Integer
							.toString(MsgType.RechargeAck)));
					params.add(new BasicNameValuePair("imei", imei));
					params.add(new BasicNameValuePair("smsType", Integer
							.toString(status)));

					HttpResponse httpResponse;
					try {
						httpPost.setEntity(new UrlEncodedFormEntity(params,
								HTTP.UTF_8));
						httpResponse = new DefaultHttpClient()
								.execute(httpPost);
						if (httpResponse.getStatusLine().getStatusCode() == 200) {
							Log.d("post debug", "postRecharge success");
							String result = EntityUtils.toString(httpResponse
									.getEntity());
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

	public void postDianJinInfo() {
		new Thread() {
			@Override
			public void run() {

				String string = getResources().getString(R.string.server_addr);
				TelephonyManager tm = (TelephonyManager) AppActivity.this
						.getSystemService(TELEPHONY_SERVICE);
				String imei = tm.getDeviceId();

				try {
					HttpPost httpPost = new HttpPost(string);
					List<NameValuePair> params = new ArrayList<NameValuePair>();
					params.add(new BasicNameValuePair("version", Float
							.toString(HRDApplication.localVersion)));
					params.add(new BasicNameValuePair("agentId",
							PullService.agentID));
					params.add(new BasicNameValuePair("type", Integer
							.toString(MsgType.DianJinAck)));
					params.add(new BasicNameValuePair("imei", imei));

					HttpResponse httpResponse;
					try {
						httpPost.setEntity(new UrlEncodedFormEntity(params,
								HTTP.UTF_8));
						httpResponse = new DefaultHttpClient()
								.execute(httpPost);
						if (httpResponse.getStatusLine().getStatusCode() == 200) {
							Log.d("post debug", "postDianJinAck success");
							String result = EntityUtils.toString(httpResponse
									.getEntity());
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

	public static void share() {

		Intent intent = new Intent(Intent.ACTION_SEND); // 启动分享发送的属性
		intent.setType("text/plain"); // 分享发送的数据类型
		String msg = "我正在玩华容道,刚刚打破新记录,想来挑战我么?下载链接 "
				+ getContext().getResources().getString(R.string.apk_addr);
		intent.putExtra(Intent.EXTRA_TEXT, msg); // 分享的内容
		getContext().startActivity(Intent.createChooser(intent, "选择分享"));// 目标应用选择对话框的标题

	}
	
	Handler showMoreHandler = new Handler(){

		@Override
		public void handleMessage(Message msg) {			
			super.handleMessage(msg);
			
			new AlertDialog.Builder(AppActivity.this).setTitle("通知")
			.setMessage("网络不可用，请重试！")
			.setPositiveButton("OK", null)
			.create().show();			
		}
		
	};

	public void showMoreGame() {
		ConnectivityManager manager = (ConnectivityManager) getSystemService(CONNECTIVITY_SERVICE);
		NetworkInfo networkInfo = manager.getActiveNetworkInfo();
		if (networkInfo != null && networkInfo.isAvailable())
			this.startActivity(new Intent(this, MoreGameActivity.class));
		else
			showMoreHandler.sendEmptyMessage(0);

	}
}
