package com.yzyx.util;

import java.io.File;
import java.io.IOException;

import org.cocos2dx.cpp.HRDApplication;

import android.os.Environment;



public class FileUtil {

	public static File updateDir = null;
	public static File updateFile = null;
	
	public static void createFile(String name) {
		if (android.os.Environment.MEDIA_MOUNTED.equals(android.os.Environment
				.getExternalStorageState())) {
			updateDir = new File(Environment.getExternalStorageDirectory()
					+ "/" + HRDApplication.downloadDir);
			updateFile = new File(updateDir + "/" + name + ".apk");

			if (!updateDir.exists()) {
				updateDir.mkdirs();
			}
			if (!updateFile.exists()) {
				try {
					updateFile.createNewFile();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}

		}
	}
}
