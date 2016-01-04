package com.facebook.imagepipeline.producers;

import com.facebook.common.internal.Preconditions;
import com.facebook.common.references.CloseableReference;
import com.facebook.common.util.TriState;
import com.facebook.imageformat.ImageFormat;
import com.facebook.imageformat.ImageFormatChecker;
import com.facebook.imagepipeline.bpg.BpgHeaderInfo;
import com.facebook.imagepipeline.image.EncodedImage;
import com.facebook.imagepipeline.memory.PooledByteBuffer;
import com.facebook.imagepipeline.memory.PooledByteBufferFactory;
import com.facebook.imagepipeline.memory.PooledByteBufferOutputStream;
import com.facebook.imagepipeline.nativecode.BpgTranscoder;

import java.io.InputStream;
import java.util.concurrent.Executor;

public class BpgTranscodeProducer implements Producer<EncodedImage> {
  private static final String PRODUCER_NAME = "BpgTranscodeProducer";

  private final Executor mExecutor;
  private final PooledByteBufferFactory mPooledByteBufferFactory;
  private final Producer<EncodedImage> mInputProducer;

  public BpgTranscodeProducer(
      Executor executor,
      PooledByteBufferFactory pooledByteBufferFactory,
      Producer<EncodedImage> inputProducer) {
    mExecutor = Preconditions.checkNotNull(executor);
    mPooledByteBufferFactory = Preconditions.checkNotNull(pooledByteBufferFactory);
    mInputProducer = Preconditions.checkNotNull(inputProducer);
  }

  @Override
  public void produceResults(Consumer<EncodedImage> consumer, ProducerContext context) {
    mInputProducer.produceResults(new BpgTranscodeConsumer(consumer, context), context);
  }

  private class BpgTranscodeConsumer extends DelegatingConsumer<EncodedImage, EncodedImage> {
    private final ProducerContext mContext;
    private TriState mShouldTranscodeWhenFinished;

    public BpgTranscodeConsumer(
        final Consumer<EncodedImage> consumer,
        final ProducerContext context) {
      super(consumer);
      mContext = context;
      mShouldTranscodeWhenFinished = TriState.UNSET;
    }

    @Override
    protected void onNewResultImpl(EncodedImage newResult, boolean isLast) {
      if (newResult != null) {
        transcodeLastResult(newResult, getConsumer(), mContext);
      } else {
        getConsumer().onNewResult(newResult, isLast);
      }
    }
  }

  //TODO refactor
  private void transcodeLastResult(
      final EncodedImage originalResult,
      final Consumer<EncodedImage> consumer,
      final ProducerContext producerContext) {
    Preconditions.checkNotNull(originalResult);
    final EncodedImage encodedImageCopy = EncodedImage.cloneOrNull(originalResult);
    final StatefulProducerRunnable<EncodedImage> runnable =
        new StatefulProducerRunnable<EncodedImage>(
            consumer,
            producerContext.getListener(),
            PRODUCER_NAME,
            producerContext.getId()) {
          @Override
          protected EncodedImage getResult() throws Exception {
            PooledByteBufferOutputStream outputStream = mPooledByteBufferFactory.newOutputStream();
            try {
              doTranscode(encodedImageCopy, outputStream);
              CloseableReference<PooledByteBuffer> ref =
                  CloseableReference.of(outputStream.toByteBuffer());
              try {
                EncodedImage encodedImage = new EncodedImage(ref);
                encodedImage.copyMetaDataFrom(encodedImageCopy);
                return encodedImage;
              } finally {
                CloseableReference.closeSafely(ref);
              }
            } finally {
              outputStream.close();
            }
          }

          @Override
          protected void disposeResult(EncodedImage result) {
            EncodedImage.closeSafely(result);
          }

          @Override
          protected void onSuccess(EncodedImage result) {
            EncodedImage.closeSafely(encodedImageCopy);
            super.onSuccess(result);
          }

          @Override
          protected void onFailure(Exception e) {
            EncodedImage.closeSafely(encodedImageCopy);
            super.onFailure(e);
          }

          @Override
          protected void onCancellation() {
            EncodedImage.closeSafely(encodedImageCopy);
            super.onCancellation();
          }
        };
    mExecutor.execute(runnable);
  }


  /**
   *  bpg format cannot be opened as is on any android platform and it should be decoded to bitmaps
   *  */
  private static TriState shouldTranscode(final EncodedImage encodedImage) {
    Preconditions.checkNotNull(encodedImage);
    return TriState.YES;
  }

  private static void doTranscode(
      final EncodedImage encodedImage,
      final PooledByteBufferOutputStream outputStream) throws Exception {
    InputStream imageInputStream = encodedImage.getInputStream();
    final byte[] imageHeaderBytes = new byte[BpgHeaderInfo.EXTENDED_BPG_HEADER_LENGTH];
    ImageFormat imageFormat = ImageFormatChecker
                                .getImageFormat_WrapIOException(
                                    imageHeaderBytes,
                                    imageInputStream);
    switch (imageFormat) {
      case BPG_SIMPLE:
      case BPG_EXTENDED:
        BpgTranscoder.transcodeBpgToBmp(imageHeaderBytes, imageInputStream, outputStream);
        break;

      case BPG_LOSSLESS:
      case BPG_EXTENDED_WITH_ALPHA:
//        BpgTranscoder.transcodeBpgToPng(imageInputStream, outputStream);
        //not supported yet
        throw new IllegalArgumentException("Cannot decode to PNG yet");
      default:
        throw new IllegalArgumentException("Wrong image format");
    }
  }
}
