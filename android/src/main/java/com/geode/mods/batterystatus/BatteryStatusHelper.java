package com.geode.mods.batterystatus;

import android.content.Intent;
import android.content.IntentFilter;
import android.os.BatteryManager;
import android.os.Build;
import android.os.PowerManager;
import android.content.Context;

public final class BatteryStatusHelper {
    private BatteryStatusHelper() {}

    public static int getLevel(Context context) {
        if (context == null) return -1;
        IntentFilter ifilter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
        Intent status = context.registerReceiver(null, ifilter);
        if (status == null) return -1;

        int level = status.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);
        int scale = status.getIntExtra(BatteryManager.EXTRA_SCALE, -1);
        if (level < 0 || scale <= 0) return -1;
        return Math.round((level * 100f) / scale);
    }

    public static boolean isCharging(Context context) {
        if (context == null) return false;
        IntentFilter ifilter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
        Intent status = context.registerReceiver(null, ifilter);
        if (status == null) return false;

        int s = status.getIntExtra(BatteryManager.EXTRA_STATUS, -1);
        return s == BatteryManager.BATTERY_STATUS_CHARGING || s == BatteryManager.BATTERY_STATUS_FULL;
    }

    public static boolean isBatterySaver(Context context) {
        if (context == null) return false;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
            if (pm == null) return false;
            return pm.isPowerSaveMode();
        }
        return false;
    }
}
