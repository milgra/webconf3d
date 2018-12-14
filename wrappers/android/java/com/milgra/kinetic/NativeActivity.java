
package com.milgra.kinetic;

import android.annotation.TargetApi;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.content.res.AssetManager;
import android.graphics.Rect;
import android.net.Uri;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.Toast;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Timer;
import java.util.TimerTask;


public class NativeActivity extends android.app.NativeActivity
{

    static
    {
        System.loadLibrary("NativeActivity");
    }

    private FrameLayout.LayoutParams mEditTextLayoutParams;
    private MediaHandler mediaHandler;
    private TextWatcher mTextWatcher;
    private AdHandler adHandler;
    private EditText mTextEdit;

    private int lastwidth = 0;
    private int lastheight = 0;

    public native void onNativeActivityOutput( String name, String path, int texture, int width, int height );

    @Override
    protected void onCreate( Bundle savedInstanceState )
    {
        super.onCreate( savedInstanceState );

        getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_HIDDEN);

        copyAssets( );
        requestFullScreen( );

        mTextEdit = new EditText(this);
        adHandler = new AdHandler(this);
        mediaHandler = new MediaHandler(this);
        mTextWatcher = new MyTextWatcher();
        mEditTextLayoutParams = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.WRAP_CONTENT, FrameLayout.LayoutParams.WRAP_CONTENT);

        mEditTextLayoutParams.gravity = Gravity.TOP;
        mEditTextLayoutParams.setMargins(0, 0, 0, 0);

        mTextEdit.setLayoutParams(mEditTextLayoutParams);
        mTextEdit.setVisibility(View.VISIBLE);
        mTextEdit.addTextChangedListener(mTextWatcher);

        addContentView( mTextEdit, mEditTextLayoutParams);
        mTextEdit.bringToFront();
        getWindow().getDecorView().getViewTreeObserver().addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {
            @Override
            public void onGlobalLayout() {
                Timer timer = new Timer( );
                timer.schedule(new TimerTask() {
                    @Override
                    public void run() {
                        Rect r = new Rect();
                        View rootview = getWindow().getDecorView(); // this = activity
                        rootview.getWindowVisibleDisplayFrame(r);
                        onNativeActivityOutput("framechanged", "", 0, r.width(), r.height());
                    }
                }, 200);
            }
        });
    }


    protected void onPause()
    {
        super.onPause();
        mediaHandler.onPause();
    }

    @TargetApi(19)
    protected void onResume()
    {
        super.onResume();
        mediaHandler.onResume();

        //Hide toolbar
        int SDK_INT = android.os.Build.VERSION.SDK_INT;
        if(SDK_INT >= 11 && SDK_INT < 14)
        {
            getWindow().getDecorView().setSystemUiVisibility(View.STATUS_BAR_HIDDEN);
        }
        else if(SDK_INT >= 14 && SDK_INT < 19)
        {
            getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_LOW_PROFILE);
        }
        else if(SDK_INT >= 19)
        {
            setImmersiveSticky();
        }

    }

    /* output event from the controller */

    public void onOutput(String name , String url, String unitid, int texture , float volume, int loop )
    {
        if ( name.equals("showkeyboard") )
        {
            this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    mTextEdit.setText("");
                    mTextEdit.requestFocus();
                    InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
                    imm.showSoftInput(mTextEdit, InputMethodManager.SHOW_IMPLICIT);
                }
            });
        }
        else if ( name.equals("hidekeyboard") )
        {
            this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
                    imm.hideSoftInputFromWindow(mTextEdit.getWindowToken(),0);
                }
            });
        }
        else if ( name.equals("copy") ) { }
        else if ( name.equals("paste") ) { }
        else if ( name.equals("showad") )
        {
            final String ad_appid = url;
            final String ad_unitid = unitid;
            this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    adHandler.show( ad_appid , ad_unitid );
                }
            });
        }
        else if ( name.equals("loadmedia") )
        {
            mediaHandler.loadMedia(url,texture);
        }
        else if ( name.equals("playmedia") )
        {
            mediaHandler.playMedia(url,texture,volume,loop);
        }
        else if ( name.equals("stopmedia") )
        {
            mediaHandler.stopMedia(url);
        }
        else if ( name.equals("selectmedia") )
        {

        }
    }

    public int renderVideoToTexture()
    {
        return mediaHandler.renderVideoToTexture();
    }

    /* requests full screen */

    void requestFullScreen()
    {
        int SDK_INT = android.os.Build.VERSION.SDK_INT;
        if(SDK_INT >= 19)
        {
            setImmersiveSticky();

            View decorView = getWindow().getDecorView();
            decorView.setOnSystemUiVisibilityChangeListener(
                    new View.OnSystemUiVisibilityChangeListener() {
                        @Override
                        public void onSystemUiVisibilityChange(int visibility) {
                            setImmersiveSticky();
                        }
                    });
        }
    }

    /* sets full screen */

    @TargetApi(19)    
    void setImmersiveSticky()
    {
        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
    }

    /* opens up market for rating */

    public void rateApp()
    {
        try
        {
            startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse(("market://details?id=") +getPackageName())));
        } catch (ActivityNotFoundException e1) {
            try
            {
                startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse(("http://play.google.com/store/apps/details?id=") +getPackageName())));
            } catch (ActivityNotFoundException e2) {
                Toast.makeText(this, "You don't have any app that can open this link", Toast.LENGTH_SHORT).show();
            }
        }
    }

    /* copy non-media assets to interanl storage for POSIX file operations */

    private void copyAssets()
    {
        AssetManager assetManager = getAssets();
        String[] files = null;
        try {
            files = assetManager.list("");
        } catch (IOException e) {
            Log.e("tag", "Failed to get asset file list.", e);
        }
        if (files != null) for (String filename : files) {
            InputStream in = null;
            OutputStream out = null;
            String[] sub = null;
            try {
                sub = assetManager.list(filename);
            } catch (IOException e) {
                Log.e("tag", "Failed to get asset file list.", e);
            }

            if ( sub.length == 0 ) {

                try {
                    in = assetManager.open(filename);
                    File outFile = new File(this.getFilesDir()+"/", filename);
                    if(!outFile.exists()) {
                        outFile.createNewFile();
                        out = new FileOutputStream(outFile);
                        copyFile(in, out);
                        Log.e("tag", "Copied " + filename + " to " + outFile.getAbsolutePath() );
                    }
                } catch (IOException e) {
                    Log.e("tag", "Failed to copy asset file: " + filename, e);
                } finally {
                    if (in != null) {
                        try {
                            in.close();
                        } catch (IOException e) {
                            // NOOP
                        }
                    }
                    if (out != null) {
                        try {
                            out.close();
                        } catch (IOException e) {
                            // NOOP
                        }
                    }
                }
            }
        }
    }

    private void copyFile(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[1024];
        int read;
        while((read = in.read(buffer)) != -1){
            out.write(buffer, 0, read);
        }
    }
}

class MyTextWatcher implements TextWatcher {

    public native void onNativeKeyDown( String string, int keycode );

    @Override
    public void onTextChanged(CharSequence s, int start, int before, int count) {
        if ( count > 0 ) {
            if (s.length()>0 && s.subSequence(s.length()-1, s.length()).toString().equalsIgnoreCase("\n")) {
                onNativeKeyDown("", KeyEvent.KEYCODE_ENTER);
            }
            else if ( count == 0 ) {
                onNativeKeyDown("", KeyEvent.KEYCODE_DEL);
            }
            else
            {
                onNativeKeyDown(s.subSequence(s.length()-1, s.length()).toString(), 0);
            }
        }
    }

    @Override
    public void beforeTextChanged(CharSequence s, int start, int count, int after) {

    }

    @Override
    public void afterTextChanged(Editable s) {

    }
}

