package com.li.md5sign;

import android.content.Context;

/**
 * @author li
 * 版本：1.0
 * 创建日期：2020/7/2 10
 * 描述：
 */
public class MD5SignUtils {
    static {
        System.loadLibrary("native-lib");
    }

    public static native String signParams(String params);

    public static native void  checkSignature(Context context);
}
