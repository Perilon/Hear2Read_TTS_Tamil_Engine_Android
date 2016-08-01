/*************************************************************************/
/*                                                                       */
/*                  Language Technologies Institute                      */
/*                     Carnegie Mellon University                        */
/*                         Copyright (c) 2010                            */
/*                        All Rights Reserved.                           */
/*                                                                       */
/*  Permission is hereby granted, free of charge, to use and distribute  */
/*  this software and its documentation without restriction, including   */
/*  without limitation the rights to use, copy, modify, merge, publish,  */
/*  distribute, sublicense, and/or sell copies of this work, and to      */
/*  permit persons to whom this work is furnished to do so, subject to   */
/*  the following conditions:                                            */
/*   1. The code must retain the above copyright notice, this list of    */
/*      conditions and the following disclaimer.                         */
/*   2. Any modifications must be clearly marked as such.                */
/*   3. Original authors' names are not deleted.                         */
/*   4. The authors' names are not used to endorse or promote products   */
/*      derived from this software without specific prior written        */
/*      permission.                                                      */
/*                                                                       */
/*  CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK         */
/*  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      */
/*  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   */
/*  SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE      */
/*  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    */
/*  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   */
/*  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          */
/*  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       */
/*  THIS SOFTWARE.                                                       */
/*                                                                       */
/*************************************************************************/
/*             Author:  Alok Parlikar (aup@cs.cmu.edu)                   */
/*               Date:  April 2010                                       */
/*************************************************************************/

package org.hear2read.Tamil;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;

import android.app.Activity;
import android.content.Intent;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.speech.tts.TextToSpeech;
import android.util.Log;
import android.widget.Toast;

/* Checks if the voice data is installed
 * for flite
 */

public class CheckVoiceData extends Activity {
	private final static String LOG_TAG = "Flite_Java_" + CheckVoiceData.class.getSimpleName();
	private final static String FLITE_DATA_PATH = Voice.
			getDataStorageBasePath();
	public final static String VOICE_LIST_FILE = FLITE_DATA_PATH+"cg/voices_tamil.list";


	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		int result = TextToSpeech.Engine.CHECK_VOICE_DATA_PASS;
		Intent returnData = new Intent();
		returnData.putExtra(TextToSpeech.Engine.EXTRA_VOICE_DATA_ROOT_DIRECTORY,
				FLITE_DATA_PATH);

		ArrayList<String> available = new ArrayList<String>();
		ArrayList<String> unavailable = new ArrayList<String>();

		/* First, make sure that the directory structure we need exists
		 * There should be a "cg" folder inside the flite data directory
		 * which will store all the clustergen voice data files.
		 */


		if(!Utility.pathExists(FLITE_DATA_PATH+"cg")) {
			// Create the directory.

			Log.e(LOG_TAG, "Flite data directory missing. Trying to create it.");
			boolean success = false;

			try {
				Log.e(LOG_TAG,FLITE_DATA_PATH);
				success = new File(FLITE_DATA_PATH+"cg").mkdirs();
			}
			catch (Exception e) {
				Log.e(LOG_TAG,"Could not create directory structure. "+e.getMessage());
				success = false;
			}

			if(!success) {
				Log.e(LOG_TAG, "Failed");
				// Can't do anything without appropriate directory structure.
				result = TextToSpeech.Engine.CHECK_VOICE_DATA_FAIL;
				setResult(result, returnData);
				finish();
			}
		}

		/* Connect to CMU TTS server and get the list of voices available,
		 * if we don't already have a file.
		 */

		if(!Utility.pathExists(VOICE_LIST_FILE)) {
			Log.e(LOG_TAG, "Voice list file doesn't exist. Try getting it from server.");

			//DownloadVoiceList(null);

			// Copy the voices list, whether or not there's one already in phone storage
			new File(FLITE_DATA_PATH + "cg/").mkdirs();
			copyAssets("voices_tamil.list", FLITE_DATA_PATH + "/cg");
			String VOICE_LIST_FILE = FLITE_DATA_PATH + "cg/voices_tamil.list";

		}

		/* At this point, we MUST have a voices_tamil.list file. If this file is not there,
		 * possibly because Internet connection was not available, we must create a dummy
		 *
		 */
		if(!Utility.pathExists(VOICE_LIST_FILE)) {
			try {
				Log.w(LOG_TAG, "Voice list not found, creating dummy list.");
				BufferedWriter out = new BufferedWriter(new FileWriter(VOICE_LIST_FILE));
				out.write("eng-USA-male_rms");
				out.close();
			} catch (IOException e) {
				Log.e(LOG_TAG, "Failed to create voice list dummy file.");
				// Can't do anything without that file.
				result = TextToSpeech.Engine.CHECK_VOICE_DATA_FAIL;
				setResult(result, returnData);
				finish();
			}
		}
		/* Go through each line in voices_tamil.list file and see
		 * if the data for that voice is installed.
		 */

		ArrayList<Voice> voiceList = getVoices();
		if (voiceList.isEmpty()) {
			Log.e(LOG_TAG,"Problem reading voices list. This shouldn't happen!");
			result = TextToSpeech.Engine.CHECK_VOICE_DATA_FAIL;
			setResult(result, returnData);
			finish();
		}

		for(Voice vox:voiceList) {
			if(vox.isAvailable()) {
				available.add(vox.getName());
			} else {
				unavailable.add(vox.getName());
			}
		}
		returnData.putStringArrayListExtra("availableVoices", available);
		returnData.putStringArrayListExtra("unavailableVoices", unavailable);
		setResult(result, returnData);
		finish();
	}


	public static void DownloadVoiceList(Runnable callback) {
		// Download the voice list and call back to notify of update
//		String voiceListURL = Voice.getDownloadURLBasePath() + "voices_tamil.list?q=1";


//		 FileDownloader fdload = new FileDownloader();
//		 fdload.saveUrlAsFile(voiceListURL, VOICE_LIST_FILE);
//		 while(!fdload.finished) {}
//
//		String[] assets = null;

		String VOX_NAME = "female;sxv.cg.flitevox";

		//if (!Utility.pathExists(FLITE_DATA_PATH + "cg/tam/IND/" + VOX_NAME)) {
			new File(FLITE_DATA_PATH + "cg/tam/IND/").mkdirs();
			//Toast toast = Toast.makeText(mContext, "Installing Tamil Voice:\n " +
			//		VOX_NAME, Toast.LENGTH_LONG);
			//toast.show();
			CheckVoiceData cvd = new CheckVoiceData();
			cvd.copyAssets(VOX_NAME, FLITE_DATA_PATH + "/cg/tam/IND/");
			//toast = Toast.makeText(mContext, "Tamil Voice Is Ready.", Toast.LENGTH_LONG);
			//toast.show();

		//}

		boolean savedVoiceList = true;

		if(!savedVoiceList)
			Log.w(LOG_TAG,"Could not update voice list from server");
		else
			Log.w(LOG_TAG,"Successfully updated voice list from server");

		if (callback != null) {
			callback.run();
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

}
