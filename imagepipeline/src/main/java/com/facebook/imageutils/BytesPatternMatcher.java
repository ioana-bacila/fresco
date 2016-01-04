package com.facebook.imageutils;

public class BytesPatternMatcher {

  public static boolean matchBytePattern(
          final byte[] byteArray,
          final int offset,
          final byte[] pattern) {
    if (pattern.length + offset > byteArray.length) {
      return false;
    }

    for (int i = 0; i < pattern.length; ++i) {
      if (byteArray[i + offset] != pattern[i]) {
        return false;
      }
    }

    return true;
  }
}
