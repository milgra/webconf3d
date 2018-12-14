package com.milgra.kinetic;

import android.app.Activity;

import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.InterstitialAd;


public class AdHandler
{

    InterstitialAd mInterstitialAd;
    private boolean hasad;
    private boolean hasunitid;

    /* create AdHandler */

    public AdHandler( Activity activity )
    {
        hasad = false;
        hasunitid = false;
        mInterstitialAd = new InterstitialAd( activity );
        mInterstitialAd.setAdListener( new AdListener( )
        {
            @Override
            public void onAdClosed( )
            {
                hide( );
            }
        });
    }

    /* show ad if ready */

    public void show( String ad_appid , String ad_unitid )
    {
        if ( !hasunitid )
        {
            hasunitid = true;
            mInterstitialAd.setAdUnitId(ad_unitid);
        }

        if ( !hasad )
        {
            hasad = true;
            AdRequest adRequest = new AdRequest.Builder( )
                    .addTestDevice(AdRequest.DEVICE_ID_EMULATOR)
                    .addTestDevice("18A5DAEA105179FA5DF5AE3977B9C35D")
                    .build( );

            mInterstitialAd.loadAd( adRequest );
        }
        else if (mInterstitialAd.isLoaded() )
        {
            mInterstitialAd.show();
        }
    }

    public void hide( )
    {
        hasad = false;
    }
}
