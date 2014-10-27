package org.cocos2dx.cpp;

public class MoreGameInfoEntity {

	public String get_icon_url() {
		return _icon_uri;
	}

	public void set_icon_url(String _icon_url) {
		this._icon_uri = _icon_url;
	}

	public String get_title() {
		return _title;
	}

	public void set_title(String _title) {
		this._title = _title;
	}

	public String get_info() {
		return _info;
	}

	public void set_info(String _info) {
		this._info = _info;
	}

	public String get_download_url() {
		return _download_uri;
	}

	public void set_download_url(String _download_url) {
		this._download_uri = _download_url;
	}

	private String _icon_uri = null;
	private String _title = null;
	private String _info = null;
	private String _download_uri = null;

}
