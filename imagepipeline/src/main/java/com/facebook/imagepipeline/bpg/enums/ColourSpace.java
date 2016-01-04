package com.facebook.imagepipeline.bpg.enums;

public enum ColourSpace {
  BPG_CS_YCbCr(0),
  BPG_CS_RGB(1),
  BPG_CS_YCgCo(2),
  BPG_CS_YCbCr_BT709(3),
  BPG_CS_YCbCr_BT2020(4),

  // not supported in the 0.9.6 version
  BPG_CS_BT2020_CT_LUMIN(5);

  private int val;

  ColourSpace(int i) {
      this.val = i;
  }

  public int getVal() {
        return val;
      }
}