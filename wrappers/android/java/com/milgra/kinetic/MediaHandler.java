package com.milgra.kinetic;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.SurfaceTexture;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.opengl.GLES20;
import android.opengl.GLUtils;
import android.util.Log;
import android.view.Surface;
import android.webkit.MimeTypeMap;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.Collection;
import java.util.HashMap;

import static android.opengl.GLES11Ext.GL_TEXTURE_EXTERNAL_OES;


public class MediaHandler
{

    private Activity activity;
    private HashMap<String,VideoHandler> videoHandlers;
    private HashMap<String,AudioHandler> audioHandlers;

    public native void onMediaHandlerOutput( String name, String path, int texture, int width, int height );

    /* constructor */

    public MediaHandler(Activity theActivity)
    {
        activity = theActivity;
        audioHandlers = new HashMap<String,AudioHandler>();
        videoHandlers = new HashMap<String,VideoHandler>();
    }

    /* preload media */

    public void loadMedia( String url , int texture )
    {
        Log.e("KINETIC","loadMedia "+url+" to "+texture);
        if ( url.contains("http://") ) loadRemoteMedia( url , texture );
        else loadLocalMedia( url , texture );
    }

    /* play or set audio volume */

    public void playMedia( String url , int texture , float volume , int loop )
    {
        Log.e("KINETIC","playMedia "+url+" to "+texture+" volume "+volume+" loop "+loop);
        MimeTypeMap mime = MimeTypeMap.getSingleton();
        String type = mime.getMimeTypeFromExtension( MimeTypeMap.getFileExtensionFromUrl(url) );

        if ( type.contains("image") )
        {

        }
        else if ( type.contains("audio") )
        {
            AudioHandler handler = audioHandlers.get(url);
            if ( handler != null ) {
                handler.mediaPlayer.setLooping(loop == 1);
                handler.mediaPlayer.setVolume( volume, volume );
                if ( !handler.mediaPlayer.isPlaying() ) handler.mediaPlayer.start();
            }
        }
        else if ( type.contains("video") )
        {

        }
    }

    /* stop media if it is playing */

    public void stopMedia( String url )
    {
        Log.e("KINETIC","stopMedia "+url);
        MimeTypeMap mime = MimeTypeMap.getSingleton();
        String type = mime.getMimeTypeFromExtension( MimeTypeMap.getFileExtensionFromUrl(url) );

        if ( type.contains("image") )
        {

        }
        else if ( type.contains("audio") )
        {
            AudioHandler handler = audioHandlers.get(url);
            if ( handler != null ) handler.mediaPlayer.pause();
        }
        else if ( type.contains("video") )
        {
            VideoHandler handler = videoHandlers.get(url);
            if ( handler != null ) handler.mediaPlayer.pause();
        }
    }

    /* load remote media */

    public void loadRemoteMedia( String url , int texture )
    {
        MimeTypeMap mime = MimeTypeMap.getSingleton();
        String type = mime.getMimeTypeFromExtension( MimeTypeMap.getFileExtensionFromUrl(url) );

        if ( type.contains("image") )
        {
            try
            {
                InputStream is = (InputStream) new URL(url).getContent();
                Bitmap bitmap = BitmapFactory.decodeStream(is);
                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D,texture);
                GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);

                onMediaHandlerOutput("medialoaded",url,texture,bitmap.getWidth(),bitmap.getHeight());
            }
            catch (Exception exception)
            {
                Log.e("KINETIC","loadRemoteMedia error "+exception);
            }
        }
        else if ( type.contains("audio") )
        {

        }
        else if ( type.contains("video") )
        {
            VideoHandler videoHandler = new VideoHandler(url,texture);
            videoHandlers.put( url, videoHandler );
        }
    }

    /* load local media */

    public void loadLocalMedia( String url , int texture )
    {
        MimeTypeMap mime = MimeTypeMap.getSingleton();
        String type = mime.getMimeTypeFromExtension( MimeTypeMap.getFileExtensionFromUrl(url) );

        if ( type.contains("image") )
        {

        }
        else if ( type.contains("audio") )
        {
            AudioHandler handler = audioHandlers.get(url);
            if ( handler == null ) {
                handler = new AudioHandler();
                handler.mediaPlayer = new MediaPlayer();
                audioHandlers.put( url , handler );
            }
            try
            {
                // have to read up file size first for android 4.0.3 and 4.0.4
                File file = new File(url);
                long length = file.getTotalSpace();
                FileInputStream is = new FileInputStream(file);

                handler.mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
                handler.mediaPlayer.setDataSource(is.getFD(), 0l, length);
                //mediaPlayer.setOnPreparedListener(this);
                handler.mediaPlayer.prepareAsync();
            }
            catch (IOException exception)
            {
                Log.e("KINETIC","loadLocalMedia error "+exception);
            }
        }
        else if ( type.contains("video") )
        {
            VideoHandler handler = videoHandlers.get(url);
            if ( handler != null ) handler.mediaPlayer.stop();
        }
    }

    /* render to texture iteration */

    public int renderVideoToTexture()
    {
        Collection<VideoHandler> handlers = videoHandlers.values();
        for (VideoHandler handler : handlers )
        {
            boolean finished = handler.renderVideoToTexture();
            if ( finished ) videoHandlers.remove( handler.mediaurl );
        }

        if ( videoHandlers.size() > 0 ) return 1;
        else return 0;
    }

    /* app goes background */

    public void onPause()
    {
        for ( AudioHandler handler : audioHandlers.values( ) )
        {
            if ( handler.mediaPlayer.isPlaying( ) )
            {
                handler.startonresume = true;
                handler.mediaPlayer.pause();
            }
        }

        for ( VideoHandler handler: videoHandlers.values( ) )
        {
            if ( handler.mediaPlayer.isPlaying( ) )
            {
                handler.startonresume = true;
                handler.mediaPlayer.pause();
            }
        }
    }

    /* app comes foreground */

    public void onResume()
    {
        for ( AudioHandler handler : audioHandlers.values( ) )
        {
            if ( handler.startonresume )
            {
                handler.startonresume = false;
                handler.mediaPlayer.start();
            }
        }

        for ( VideoHandler handler: videoHandlers.values( ) )
        {
            if ( handler.startonresume )
            {
                handler.startonresume = false;
                handler.mediaPlayer.start();
            }
        }
    }
}

/* videohandler for rendering video to texture */

class VideoHandler implements SurfaceTexture.OnFrameAvailableListener
{
    private boolean finished;
    private boolean updateSurface = false;
    private Surface surface;

    private SurfaceTexture mSurface;
    private int mTextureID[] = new int[1];

    public String mediaurl;
    public boolean startonresume;
    public MediaPlayer mediaPlayer;


    public native void onMediaHandlerOutput( String name, String path, int texture, int width, int height );

    /* constructor */

    public VideoHandler( String url , int texture )
    {
        mediaurl = url;
        finished = false;
        mediaPlayer = new MediaPlayer();

        Log.e("MILANINFO","LOADVIDEOTOTETURE"+Thread.currentThread());

        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glGenTextures( 1, mTextureID,0 );
        GLES20.glBindTexture(GL_TEXTURE_EXTERNAL_OES, mTextureID[0]);
        GLES20.glTexParameteri( GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE );
        GLES20.glTexParameteri( GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE );
        GLES20.glTexParameteri( GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST );
        GLES20.glTexParameteri( GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST );

        mSurface = new SurfaceTexture(mTextureID[0]);

        mSurface.setOnFrameAvailableListener(this);

        surface = new Surface(mSurface);
        mediaPlayer.setSurface(surface);

        mediaPlayer.setOnErrorListener(new MediaPlayer.OnErrorListener() {
            public boolean onError(MediaPlayer mp, int what, int extra) {
                Log.e("MILANINFO","onError");
                mp.reset();
                return false;
            }
        });

        mediaPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion(MediaPlayer mediaPlayer) {
                Log.e("MILANINFO","onCompletion");
                finished = true;
            }
        });

        mediaPlayer.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
            public void onPrepared(MediaPlayer mp) {
                Log.e("MILANINFO","onPrepared"+Thread.currentThread());
                mp.getVideoWidth();

                onMediaHandlerOutput("medialoaded",mediaurl,mTextureID[0],mp.getVideoWidth(),mp.getVideoHeight());

                mp.start();
            }
        });

        try
        {
            mediaPlayer.setDataSource(url);
            mediaPlayer.prepareAsync();

        } catch (Exception e) {
            Log.e("MILANINFO","cannot load video"+e);
        }
    }

    public boolean renderVideoToTexture()
    {
        if ( updateSurface ) {
            updateSurface = false;

            GLES20.glActiveTexture( GLES20.GL_TEXTURE1 );
            GLES20.glBindTexture(GL_TEXTURE_EXTERNAL_OES, mTextureID[0]);
            mSurface.updateTexImage();
        }
        return finished;
    }

    public void destroy()
    {
        mediaPlayer.stop();
        GLES20.glDeleteTextures(1,mTextureID,0);
    }

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture)
    {
        updateSurface = true;
    }
}

class AudioHandler
{
    public boolean startonresume;
    public MediaPlayer mediaPlayer;
}