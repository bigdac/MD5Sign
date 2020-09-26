package com.li.md5sign;

import android.annotation.SuppressLint;
import android.app.Application;

/**
 * @author 版本：1.0
 * 创建日期：2020/7/2 19
 * 描述：
 */

public class BaseApplication extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        MD5SignUtils.checkSignature(this);
    }
}
