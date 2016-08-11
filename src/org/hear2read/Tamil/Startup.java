package org.hear2read.Tamil;

import android.app.Application;
import android.content.Context;
import android.content.Intent;
import android.content.res.AssetManager;
import android.speech.tts.TextToSpeech;
import android.util.Log;
import android.widget.Toast;


import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;

import android.speech.tts.TextToSpeech;

import static org.hear2read.Tamil.CheckVoiceData.VOICE_LIST_FILE;


/**
 * Created by perilon on 6/17/16.
 */

/* The point here is to run on startup.  Copies data from assets directory to phone storage. */

public class Startup extends Application {
    //private static Startup instance;
    private static Context mContext;

    public static Context getContext() {
        //  return instance.getApplicationContext();
        return mContext;
    }

    public void copyFile(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[1024];
        int read;
        while((read = in.read(buffer)) != -1){
            out.write(buffer, 0, read);
        }
    }

    public void copyAssets(String assetFileName, String outFilePath) {
        AssetManager assetManager = getAssets();
        String[] files = null;
        try {
            files = assetManager.list("");
            for (int i = 0; i < files.length; i++) {
                Log.e("file:", files[i]);
            }
        } catch (IOException e) {
            Log.e("tag", "Failed to get asset file list.", e);
        }
        if (files != null) for (String filename : files) {
            if (filename.equals(assetFileName)) {
                InputStream in = null;
                OutputStream out = null;
                try {
                    in = assetManager.open(filename);
                    File outFile = new File(outFilePath, filename);
                    out = new FileOutputStream(outFile);
                    copyFile(in, out);
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

    @Override
    public void onCreate() {
        super.onCreate();

         final String LOG_TAG = "Flite_Java_" + Startup.class.getSimpleName();

        mContext = getApplicationContext();


        //  Hardcoding this, for now, for simplicity.  Just match to whatever's in the
        //  assets directory.
        String VOX_NAME = "female;sxv.cg.flitevox";

        String FLITE_DATA_PATH = Voice.getDataStorageBasePath();


        int result = TextToSpeech.Engine.CHECK_VOICE_DATA_PASS;
        Intent returnData = new Intent();
        returnData.putExtra(TextToSpeech.Engine.EXTRA_VOICE_DATA_ROOT_DIRECTORY,
                FLITE_DATA_PATH);


        // Copy the voices list, whether or not there's one already in phone storage
        new File(FLITE_DATA_PATH + "cg/").mkdirs();
        copyAssets("voices_tamil.list", FLITE_DATA_PATH + "/cg");
        String VOICE_LIST_FILE = FLITE_DATA_PATH + "cg/voices_tamil.list";

        if (!Utility.pathExists(FLITE_DATA_PATH + "cg/tam/IND/" + VOX_NAME)) {
            new File(FLITE_DATA_PATH + "cg/tam/IND/").mkdirs();
            Toast toast = Toast.makeText(mContext, "Installing Tamil Voice:\n " +
                    VOX_NAME, Toast.LENGTH_LONG);
            toast.show();
            copyAssets(VOX_NAME, FLITE_DATA_PATH + "/cg/tam/IND/");
            toast = Toast.makeText(mContext, "Tamil Voice Is Ready.", Toast.LENGTH_LONG);
            toast.show();

        } else {
            // if a .flitevox file does exist already, see if it's the same as the one specified
            // in the voice list file; if not, copy over the new one


            ArrayList<String> vL = null;
            try {
                vL = Utility.readLines(VOICE_LIST_FILE);
            } catch (IOException e) {
                // Ignore exception, since we will return empty anyway.
            }



            ArrayList<String> available = new ArrayList<String>();

            ArrayList<Voice> voiceList = getVoices();
            if (voiceList.isEmpty()) {
                Log.e(LOG_TAG,"Problem reading voices list. This shouldn't happen!");
                result = TextToSpeech.Engine.CHECK_VOICE_DATA_FAIL;
                // setResult(result, returnData);
                // finish();
            }

            for(Voice vox:voiceList) {
                if(vox.isAvailable()) {
                    Log.e(LOG_TAG, "available");
                    available.add(vox.getName());
                } else {
                    Log.e(LOG_TAG, "unavailable");
                    // unavailable.add(vox.getName());
                }
            }
            returnData.putStringArrayListExtra("availableVoices", available);


            String[] voiceInfo = vL.get(0).split("\t");  // there's only the one voice, for now
            String correctMD5sum = voiceInfo[1];

            MessageDigest md;
            try {
                md = MessageDigest.getInstance("MD5");
            } catch (NoSuchAlgorithmException e) {
                Log.e(LOG_TAG, "MD5 could not be computed");
                return;
            }


            String mVoicePath = FLITE_DATA_PATH + "cg/tam/IND/" + VOX_NAME;

            FileInputStream fis;
            try {
                fis = new FileInputStream(mVoicePath);
            }
            catch (FileNotFoundException e) {
                Log.e(LOG_TAG, "Voice File not found: " + mVoicePath);
                return;
            }

            byte[] dataBytes = new byte[1024];
            int nread = 0;
            try {
                while ((nread = fis.read(dataBytes)) != -1) {
                    md.update(dataBytes, 0, nread);
                }
            } catch (IOException e) {
                Log.e(LOG_TAG, "Could not read voice file: " + mVoicePath);
                return;
            }
            finally {
                try {
                    fis.close();
                } catch (IOException e) {
                    // Ignoring this exception.
                }
            }

            byte[] mdbytes = md.digest();

            StringBuffer sb = new StringBuffer();
            for (int i = 0; i < mdbytes.length; i++) {
                sb.append(Integer.toString((mdbytes[i] & 0xff) + 0x100, 16).substring(1));
            }


            if (!sb.toString().equals(correctMD5sum)) {
                // replace whatever's there with this new voice
                Toast toast = Toast.makeText(mContext, "Installing New Tamil Voice:\n " +
                        VOX_NAME, Toast.LENGTH_LONG);
                toast.show();
                copyAssets(VOX_NAME, FLITE_DATA_PATH + "/cg/tam/IND/");
                toast = Toast.makeText(mContext, "New Tamil Voice Is Ready", Toast.LENGTH_LONG);
                toast.show();
                returnData.putStringArrayListExtra("availableVoices", available);

            } else {
//                 everything is fine
                Toast toast = Toast.makeText(mContext, "Tamil Voice Is Ready.", Toast.LENGTH_LONG);
                toast.show();
                returnData.putStringArrayListExtra("availableVoices", available);
                return;
            }
        }
    }


    public static ArrayList<Voice> getVoices() {
        ArrayList<String> voiceList = null;
        try {
            voiceList = Utility.readLines(VOICE_LIST_FILE);
        } catch (IOException e) {
            // Ignore exception, since we will return empty anyway.
        }
        if (voiceList == null) {
            voiceList = new ArrayList<String>();
        }

        ArrayList<Voice> voices = new ArrayList<Voice>();

        for(String strLine:voiceList) {
            Voice vox = new Voice(strLine);
            if (!vox.isValid())
                continue;
            voices.add(vox);
        }

        return voices;
    }
}
