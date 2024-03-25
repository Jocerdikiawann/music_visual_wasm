package com.raylib.game;

public class NativeLoader extends android.app.NativeActivity {
    
    static {
        System.loadLibrary("main");
    }
   
    private static android.app.NativeActivity activity;

    @Override
    public void onCreate(android.os.Bundle savedInstanceState){
      super.onCreate(savedInstanceState);
      activity = this;
    } 

    public android.app.NativeActivity getActivity(){
     return activity;
    }   
}
