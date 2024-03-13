package com.raylib.game;
public class NativeLoader extends android.app.NativeActivity {

    public android.app.Activity getActivity(){
      return this;
    }
    
    static {
        System.loadLibrary("main");
    }
    
}
