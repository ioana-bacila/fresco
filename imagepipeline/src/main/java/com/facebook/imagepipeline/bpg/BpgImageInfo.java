package com.facebook.imagepipeline.bpg;

import com.facebook.common.soloader.SoLoaderShim;

public class BpgImageInfo {
  static {
    SoLoaderShim.loadLibrary("libbpg");
  }

  int width;
  int height;
  int pictureDataLen;
  byte format;
  boolean hasAlpha;
  byte colourSpace;
  byte bitDepth;
  boolean hasPremultipliedAlpha;
  boolean hasWPlane;
  boolean limitedRange;
  boolean hasAnimation;
  short loopCount;

  public int getWidth() {
    return width;
  }

  public int getHeight() {
    return height;
  }

  public int getPictureDataLen() {
    return pictureDataLen;
  }

  public byte getFormat() {
    return format;
  }

  public boolean hasAlpha() {
    return hasAlpha;
  }

  public byte getColourSpace() {
    return colourSpace;
  }

  public byte getBitDepth() {
    return bitDepth;
  }

  public boolean hasPremultipliedAlpha() {
    return hasPremultipliedAlpha;
  }

  public boolean hasWPlane() {
    return hasWPlane;
  }

  public boolean hasLimitedRange() {
    return limitedRange;
  }

  public boolean hasAnimation() {
    return hasAnimation;
  }

  public short hasLoopCount() {
    return loopCount;
  }

  public native int getImageInfoFromBuf(byte[] headerBuf, int buf_len);
}
