package org.cocos2dx.cpp;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.R.anim;
import android.R.integer;
import android.app.AlertDialog;
import android.app.ListActivity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.media.Image;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.storage.StorageManager;
import android.util.Log;
import android.util.Pair;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;

import com.prancent.huarongdao.R;
import com.yzyx.util.PostUrl;

public class MoreGameActivity extends ListActivity {

	protected List<MoreGameInfoEntity> mData;
	protected DownLoadHelper _dHelper;
	protected String path;
	protected Handler handler = new Handler() {

		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			try {
				Log.d("post debug", "download xml done");

				mData = (List<MoreGameInfoEntity>) msg.obj;

				if (mData.isEmpty()) {
					new AlertDialog.Builder(MoreGameActivity.this)
							.setTitle("通知").setMessage("正在玩命开发中，敬请期待！")
							.setPositiveButton("OK", new OnClickListener() {

								public void onClick(DialogInterface arg0,
										int arg1) {
									MoreGameActivity.this.finish();
								}
							}).create().show();
				} else {

					MyAdapter adapter = new MyAdapter(MoreGameActivity.this);
					setListAdapter(adapter);
					getListView().setClickable(false);
				}

			} catch (Exception e) {
				e.printStackTrace();
			}
		}

	};

	protected Handler imgHandler = new Handler() {

		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			try {
				Log.d("post debug", "download img done");

				Pair<ImageView, Bitmap> extra = (Pair<ImageView, Bitmap>) msg.obj;
				extra.first.setImageBitmap(extra.second);

			} catch (Exception e) {
				e.printStackTrace();
			}
		}

	};

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		_dHelper = new DownLoadHelper(this);

		this.getListView().setBackgroundColor(Color.WHITE);

		path = "tmp.xml";
		Log.d("post debug", "start download xml");
		_dHelper.downLoadAndParseMoreGameInfo(path);
	}

	public void onGetDataFinished(List<MoreGameInfoEntity> list) {
		Message msg = new Message();
		msg.obj = list;
		handler.sendMessage(msg);
	}

	public void onLoadImageDone(ImageView view, Bitmap image) {
		Message msg = new Message();
		Pair<ImageView, Bitmap> extraPair = new Pair<ImageView, Bitmap>(view,
				image);
		msg.obj = extraPair;
		imgHandler.sendMessage(msg);
	}

	public final class ViewHolder {
		public ImageView img;
		public TextView title;
		public TextView info;
		public Button viewBtn;
	}

	private class MyAdapter extends BaseAdapter {

		private LayoutInflater mInflater;

		public MyAdapter(Context context) {
			this.mInflater = LayoutInflater.from(context);
		}

		public int getCount() {
			return mData.size();
		}

		public Object getItem(int arg0) {
			return null;
		}

		public long getItemId(int arg0) {
			return 0;
		}

		public View getView(int position, View convertView, ViewGroup parent) {

			ViewHolder holder = null;
			if (convertView == null) {
				convertView = mInflater.inflate(R.layout.list_item_view, null);
				holder = new ViewHolder();

				holder.img = (ImageView) convertView.findViewById(R.id.img);
				holder.info = (TextView) convertView.findViewById(R.id.info);
				holder.title = (TextView) convertView.findViewById(R.id.title);
				holder.viewBtn = (Button) convertView
						.findViewById(R.id.viewBtn);

				convertView.setTag(holder);

			} else {
				holder = (ViewHolder) convertView.getTag();
			}

			String icon_url = mData.get(position).get_icon_url();

			_dHelper.loadImageFromURL(icon_url, holder.img);
			holder.title.setText(mData.get(position).get_title());
			holder.info.setText(mData.get(position).get_info());
			holder.viewBtn.setTag(position);
			holder.viewBtn.setOnClickListener(new View.OnClickListener() {

				@Override
				public void onClick(View view) {
					// TODO Auto-generated method stub
					// download
					Log.d("post debug", mData.get((Integer) view.getTag())
							.get_download_url());

					try {
						_dHelper.downLoadAPKWithNotification(
								mData.get((Integer) view.getTag())
										.get_download_url(),
								mData.get((Integer) view.getTag()).get_title()
										+ ".apk", "huarongdao", (Integer) view
										.getTag());
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
			});

			return convertView;
		}

	}

}
