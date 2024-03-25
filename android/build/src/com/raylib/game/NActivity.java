package com.raylib.game;

public class NActivity extends android.app.Activity {
  private static android.app.Activity activity;
  @Override
  public void onCreate(android.os.Bundle savedInstanceState){
    super.onCreate(savedInstanceState);
    activity = this;
  } 

  public static android.app.Activity getActivity(){
    return activity;
  }
}
