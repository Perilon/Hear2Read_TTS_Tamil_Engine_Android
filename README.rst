============================
Hear2Read Text to Speech Tamil Engine for Android
============================

:Authors: Alok Parlikar, Andrew Wilkinson

Introduction
============

Flite (festival-lite) is a small, fast-runtime speech synthesis engine
developed at Carnegie Mellon University and primarily designed for
small embedded devices. Flite is an open-source synthesis engine that
can be used to provide text-to-speech functionality on smartphones and
similar devices. For more information about Flite, visit http://www.cmuflite.org.

This application wraps the Flite engine so that it can be installed as
an application on Android devices. This app gets installed as a TTS
library, thereby allowing other applications to make use of Flite for
Tamil language synthesis.  Specifically, this app has been tested with
@Voice Aloud Reader, Voice Dream Reader, GoRead, and Talkback on Android 5.1 (Lollipop).

This application has been developed with support from Hear2Read and
is based on the "Flite TTS Engine for Android" app developed by
Alok Parlikar available at https://github.com/happyalu/Flite-TTS-Engine-for-Android.

For information about Hear2Read, visit http://www.hear2read.org/.

Installing this Application
===========================

This app is available on the Google Play Store at
https://play.google.com/store/apps/details?id=org.hear2read.Tamil&hl=en.
Alternatively, using this source code, you can generate an .apk file and install it on
your device.  Devices running Android versions 2.2 (Froyo) or later are
supported.

After installing this application, all that is needed to install the voice data
is to open the app once.  It will automatically copy the necessary files to phone SD
storage under the directory "hear2read-data."  Next:

On Android 5.1:

1.  Enter "Settings" → "Language & Input" → "Text-to-speech output"
2.  Select Hear2Read Tamil under "Preferred engine"
3.  Tap the gear icon next to "Hear2Read Tamil" and specify "Language" as "Tamil (India,female;sxv)"
4.  Select the speech rate, if desired, under "Speech rate"

You can tap "Listen to an example" to verify that the engine is working.

On Android 6.1:

1.  Enter "Settings" → "Language and input" → "Text-to-speech"
2.  Select Hear2Read Tamil under "Preferred TTS engine"
3.  Tap the gear icon next to "Hear2Read Tamil" and specify "Language" as "Tamil (India,female;sxv)"
4.  Select the speech rate, if desired, using the slider.  A sample sentence will play.


Building this App from Source
=============================

Instructions here have been tested on Linux and are currently being tested for Mac OS X.

Requirements
------------
In order to build this application, you need the following:

- The source code for Flite, which is included in this distribution in the directory "flite".
This version of Flite contains updates for Tamil and other Indic languages that are not yet part of the
standard Flite distribution.
- Android NDK Release 10d
- Android SDK Release 24.0.2

The NDK can be found, as of August 1, 2016, at the following links:

- Windows 32-bit : http://dl.google.com/android/ndk/android-ndk-r10d-windows-x86.exe
- Windows 64-bit : http://dl.google.com/android/ndk/android-ndk-r10d-windows-x86_64.exe
- Mac OS X 64-bit : http://dl.google.com/android/ndk/android-ndk-r10d-darwin-x86_64.bin
- Mac OS X 32-bit : http://dl.google.com/android/ndk/android-ndk-r10d-darwin-x86.bin
- Linux 32-bit (x86) : http://dl.google.com/android/ndk/android-ndk-r10d-linux-x86.bin
- Linux 64-bit (x86) : http://dl.google.com/android/ndk/android-ndk-r10d-linux-x86_64.bin

(Change permissions to +x if necessary.)

The SDK can be most easily installed through downloading Android Studio and using its
built-in SDK Manager toolkit:
- https://developer.android.com/studio/index.html

Application Build Steps
-----------------------

*Export necessary environment variables* ::

    export FLITEDIR=/path/to/flite
    export FLITE_APP_DIR=/path/to/this/application
    export ANDROID_NDK=/path/to/android/ndk
    export ANDROID_SDK=/path/to/android/sdk

*Build Flite Engine for multiple architectures* ::

    cd $FLITEDIR
    ./configure --with-langvox=android --target=armeabi-android
    make
    ./configure --with-langvox=android --target=armeabiv7a-android
    make
    ./configure --with-langvox=android --target=x86-android
    make
    ./configure --with-langvox=android --target=mips-android
    make

*Build shared libraries* ::

    cd $FLITE_APP_DIR
    $ANDROID_NDK/ndk-build

*Build installable application package (debug mode)* ::

    cd $FLITE_APP_DIR
    ant debug -Dsdk.dir=$ANDROID_SDK

Upon completing these steps, the file "FliteEngine-debug-unaligned.apk" should appear
in the "bin" directory.  This file can be opened with Package Installer on an Android device.





