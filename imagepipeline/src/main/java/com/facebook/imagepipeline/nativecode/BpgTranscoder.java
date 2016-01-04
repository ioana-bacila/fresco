package com.facebook.imagepipeline.nativecode;

import com.facebook.common.internal.DoNotStrip;
import com.facebook.common.internal.Throwables;
import com.facebook.imagepipeline.bpg.BpgImageInfo;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

@DoNotStrip
public class BpgTranscoder {
  static {
    ImagePipelineNativeLoader.load();
  }

  public static void transcodeBpgToBmp(
      final byte[] imageHeaderBytes,
      InputStream inputStream,
      OutputStream outputStream) {
    try {
      BpgImageInfo bpgImageInfo = new BpgImageInfo();
      int imageHeaderSize = bpgImageInfo.
                              getImageInfoFromBuf(
                                  imageHeaderBytes,
                                  imageHeaderBytes.length);
      if (imageHeaderSize < 0) {
        throw new RuntimeException("Could not decode the BPG header");
      }

      int imageBodySize = bpgImageInfo.getPictureDataLen();
      byte[] imageBodyBytes = new byte[imageBodySize];

      System.arraycopy(
          imageHeaderBytes,
          imageHeaderSize,
          imageBodyBytes,
          0,
          imageHeaderBytes.length - imageHeaderSize);

      int totalBytesToRead = imageBodySize - imageHeaderBytes.length;
      if (fillBuffer(totalBytesToRead, imageBodyBytes, inputStream)
              != totalBytesToRead) {
        throw new RuntimeException("Could not read the BPG body");
      }

      outputStream.write(
              decodeBodyBuffer(
                  bpgImageInfo,
                  imageBodyBytes,
                  imageHeaderSize,
                  imageBodySize));

    } catch (IOException ioe) {
      throw Throwables.propagate(ioe);
    }

  }

  public static void transcodeFullBpgToBmp(
      InputStream inputStream,
      OutputStream outputStream) throws IOException {
    byte[] buffer = new byte[1024];
    int readBytes;

    ByteArrayOutputStream bpgStream = new ByteArrayOutputStream();
    while ((readBytes = inputStream.read(buffer, 0, buffer.length)) != -1) {
      bpgStream.write(buffer, 0, readBytes);
    }

    outputStream.write(
        decodeFullImageBuffer(
            bpgStream.toByteArray(),
            bpgStream.size()));

  }

  private static int fillBuffer(
      int bytesToRead,
      byte[] buffer,
      InputStream inputStream) throws IOException {
    int totalBytesRead = bytesToRead;
    int readBytes;
    int offset = buffer.length - bytesToRead;

    while (totalBytesRead > 0) {
      readBytes = inputStream.read(buffer, offset, 1024);
      offset += readBytes;
      totalBytesRead -= readBytes;
    }

    return offset;
  }

  /**
   * Gets the full image size in bytes
   * */
  @DoNotStrip
  public static native int getBmpBufferSize(int width, int height);

  /**
   * Decodes the image body bytes without the header
   * */
  public static native byte[] decodeBodyBuffer(BpgImageInfo bpgImageInfo, byte[] encBuffer, int idx, int encBufferSize);

  //    /**
  //     * Decodes the header bytes only and returns the information about the BPG image
  //     * */
  //    public static native BpgImageInfo decodeHeader(byte[] encBuffer, int encBufferSize);

  /**
   * Decodes full image with both header and body
   * */
  @DoNotStrip
  public static native byte[] decodeFullImageBuffer(byte[] encBuffer, int encBufferSize);

}
