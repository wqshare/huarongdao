package org.cocos2dx.cpp;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import android.content.Context;
import android.util.Log;
import android.util.Xml;

public class MoreGameInfoParser {

	XmlPullParser parser = null;
	List<MoreGameInfoEntity> infos = null;
	InputStream is;
	Context _context;

	public MoreGameInfoParser(Context context) {
		this._context = context;
	}

	public List<MoreGameInfoEntity> getData(String xmlPath) {

		try {
			Log.d("post debug", "xmlPath: " + xmlPath);
			parser = Xml.newPullParser();
			is = _context.openFileInput(xmlPath);
			int eventType = parser.getEventType();
			MoreGameInfoEntity currentInfoEntity = new MoreGameInfoEntity();

			parser.setInput(is, "UTF-8");
			while (XmlPullParser.END_DOCUMENT != eventType) {
				switch (eventType) {
				case XmlPullParser.START_DOCUMENT:
					infos = new ArrayList<MoreGameInfoEntity>();
					break;
				case XmlPullParser.START_TAG:
					String name = parser.getName();
					if (name.equalsIgnoreCase("recommend")) {
						currentInfoEntity = new MoreGameInfoEntity();
					} else if (currentInfoEntity != null) {

						if (name.equalsIgnoreCase("icon_uri")) {
							currentInfoEntity.set_icon_url(parser.nextText());
						} else if (name.equalsIgnoreCase("title")) {
							currentInfoEntity.set_title(parser.nextText());
						} else if (name.equalsIgnoreCase("info")) {
							currentInfoEntity.set_info(parser.nextText());
						} else if (name.equalsIgnoreCase("download_uri")) {
							currentInfoEntity.set_download_url(parser
									.nextText());
						}

					}
					break;
				case XmlPullParser.END_TAG:
					if (currentInfoEntity != null
							&& parser.getName().equalsIgnoreCase("recommend")) {
						infos.add(currentInfoEntity);
						currentInfoEntity = null;
					}
					break;
				case XmlPullParser.END_DOCUMENT:
					break;
				default:
					break;
				}

				eventType = parser.next();
			}

			is.close();

		} catch (Exception e) {
			e.printStackTrace();
		}
		return infos;
	}

}
