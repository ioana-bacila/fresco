package com.facebook.imagepipeline.bpg;

public class BpgSupportStatus {
  public static final boolean sIsBpgSupportRequired = false;

  public static final boolean sIsSimpleWebpSupported = false;

  public static final boolean sIsExtendedWebpSupported = isExtendedWebpSupported();

  /**
   * Checks whether underlying platform supports extended WebPs
   */
  private static boolean isExtendedWebpSupported() {
     return false;
  }

  /**
   *  The bpg format will not be opened by displaying .bpg files as it is not yet supported in Android.
   *  The BPG will be displayed as BMP files which is platform independent.
   *  The BPG images are a good encoding/decoding format as they are small in size and high quality
   *  when displayed after being decoded to a machine readable file.
   *  Android 5+ (Lollipop) does include support for HEVC format.
   *
   * See secion 5.1.2. Image Codecs for android 6:
   * <a href="http://static.googleusercontent.com/media/source.android.com/ro//compatibility/android-cdd.pdf">
   *   http://static.googleusercontent.com/media/source.android.com/ro//compatibility/android-cdd.pdf</a>
   */
  public static boolean isBpgPlatformSupported(
      final byte[] imageHeaderBytes,
      final int offset,
      final int headerSize) {
    return false;
  }

}
