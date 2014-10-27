package org.cocos2dx.cpp;

import com.yzyx.util.PullService;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningServiceInfo;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class MyBroadcastReceiver extends BroadcastReceiver {

	@Override
	public void onReceive(Context context, Intent intent) {
		// TODO Auto-generated method stub

		boolean isServiceRunning = false;
		if (intent.getAction().equals(Intent.ACTION_USER_PRESENT)) {

			// ¼ì²éService×´Ì¬

			ActivityManager manager = (ActivityManager) context
					.getSystemService(Context.ACTIVITY_SERVICE);
			for (RunningServiceInfo service : manager
					.getRunningServices(Integer.MAX_VALUE)) {

				Log.d("post debug", service.service.getClassName());

				if ("com.yzyx.PullService".equals(service.service
						.getClassName()))
				{
					isServiceRunning = true;
				}

			}
			if (!isServiceRunning) {
				Intent i = new Intent(context, PullService.class);
				context.startService(i);
			}
		}

	}

}
