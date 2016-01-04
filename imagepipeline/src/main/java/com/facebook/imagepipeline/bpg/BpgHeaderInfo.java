package com.facebook.imagepipeline.bpg;

import com.facebook.common.soloader.SoLoaderShim;
import com.facebook.imagepipeline.bpg.enums.BpgImageFormat;
import com.facebook.imagepipeline.bpg.enums.ColourSpace;
import com.facebook.imageutils.BytesPatternMatcher;

public class BpgHeaderInfo {
  static {
    SoLoaderShim.loadLibrary("libbpg");
  }

  /**
   * Each BPG header should consist of at least 16 bytes and start
   * with "BPGû" bytes.
   * More detailed description if WebP can be found here:
   * <a href="http://bellard.org/bpg/bpg_spec.txt">
   *   http://bellard.org/bpg/bpg_spec.txt</a>
   */
  public static final int SIMPLE_BPG_MAX_HEADER_LENGTH = 16;
  public static final int SIMPLE_BPG_MIN_HEADER_LENGTH = 5;
  public static final int EXTENDED_BPG_HEADER_LENGTH = 40;

  private static final byte[] BPG_HEADER = new byte[] {'B', 'P', 'G', (byte)0xFB};

  private static final int BPG_FORMAT_POSITION = 4;
  private static final int BPG_EXTENSION_FLAG_POSITION = 5;

  public static  boolean isBpgHeader(final byte[] imageHeaderBytes) {
    return BytesPatternMatcher.matchBytePattern(imageHeaderBytes, 0, BPG_HEADER);
  }

  public static boolean isSimpleBpgImage(final byte[] imageHeaderBytes) {
    return (imageHeaderBytes[BPG_FORMAT_POSITION] >> 5) == 1
      && imageHeaderBytes.length <= SIMPLE_BPG_MAX_HEADER_LENGTH
      && imageHeaderBytes.length >= SIMPLE_BPG_MIN_HEADER_LENGTH ?
      true : false;
  }

  public static boolean isAnimatedBpgImage(final byte[] imageHeaderBytes) {
    int animation_flag = imageHeaderBytes[BPG_EXTENSION_FLAG_POSITION] & 1;
    return animation_flag == 0 ? true : false;
  }

  public static boolean isExtendedBpgImage(final byte[] imageHeaderBytes) {
    return ((imageHeaderBytes[BPG_EXTENSION_FLAG_POSITION] >> 3) & 1) == 1 ? true : false;
  }

  public static boolean isExtendedWithAlphaBpgImage(byte[] imageHeaderBytes) {
    return (isExtendedBpgImage(imageHeaderBytes) &&
            hasAlpha(imageHeaderBytes[BPG_FORMAT_POSITION],
                     imageHeaderBytes[BPG_EXTENSION_FLAG_POSITION]));
  }

  public static boolean hasAlpha(byte alpha1, byte alpha2) {
    boolean alphaFlag1  = ((alpha1 >> 4) & 1) == 1 ? true : false;
    boolean alphaFlag2  = ((alpha2 >> 2) & 1) == 1 ? true : false;

    return (alphaFlag1 && alphaFlag2);
  }

  /** The bpg format will not be opened by displaying .bpg files as it is not yet supported in Android.
   *  The BPG will be displayed as BMP files which is platform independent.
   *  The BPG images are a good encoding/decoding format as they are small in size and high quality
   *  when displayed after being decoded to a machine readable file.
   *  Android 5+ (Lollipop) does include support for HEVC format.
   */
  public static BpgImageFormat getBpgImageFormat(final int imgFormatByte){

    switch (imgFormatByte) {
      case -4:
          return BpgImageFormat.BPG_FORMAT_420_VIDEO;
      case -3:
          return BpgImageFormat.BPG_FORMAT_422_VIDEO;
      case 0:
          return BpgImageFormat.BPG_FORMAT_GRAY;
      case 1:
          return BpgImageFormat.BPG_FORMAT_420;
      case 2:
          return BpgImageFormat.BPG_FORMAT_422;
      case 3:
          return BpgImageFormat.BPG_FORMAT_444;
      default:
          return null;
    }
  }

  public static ColourSpace getColourSpace(final int colourSpaceByte) {

    switch (colourSpaceByte) {
      case 0:
          return ColourSpace.BPG_CS_YCbCr;
      case 1:
          return ColourSpace.BPG_CS_RGB;
      case 2:
          return ColourSpace.BPG_CS_YCgCo;
      case 3:
          return ColourSpace.BPG_CS_YCbCr_BT709;
      case 4:
          return ColourSpace.BPG_CS_YCbCr_BT2020;
      default:
          return null;
    }
  }
}
