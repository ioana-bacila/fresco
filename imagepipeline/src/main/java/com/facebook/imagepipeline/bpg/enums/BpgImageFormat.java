package com.facebook.imagepipeline.bpg.enums;

public enum BpgImageFormat {
  BPG_FORMAT_GRAY(0),
  BPG_FORMAT_420(1),
  BPG_FORMAT_422(2),
  BPG_FORMAT_444(3),
  BPG_FORMAT_420_VIDEO(4),
  BPG_FORMAT_422_VIDEO(5);

  private int val;

  BpgImageFormat(int i) {
    this.val = i;
  }

  public int getVal() {
        return val;
      }
}