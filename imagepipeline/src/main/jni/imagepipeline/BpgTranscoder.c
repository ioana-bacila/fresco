#include <jni.h>
#include <stdlib.h>
#include <android/log.h>

#include "libbpg/libbpg.h"
#include "libbpg/bpgdec.h"

#include "BpgTranscoder.h"

JNIEXPORT jint JNICALL Java_com_facebook_imagepipeline_nativecode_BpgTranscoder_getBmpBufferSize(JNIEnv *env, jclass class, jint width, jint height)
{
    return (jint)get_bmp_buf_size((int) width, (int) height);
}

JNIEXPORT jbyteArray JNICALL Java_com_facebook_imagepipeline_nativecode_BpgTranscoder_decodeBodyBuffer(JNIEnv *env, jclass class, jobject bpgImgInfo, jbyteArray encBuffer, jint index, jint encBufferSize)
{
    BPGDecoderContext *img = bpg_decoder_open();
    BPGImageInfo i, *info = &i;

    jclass imgInfoClass = (*env)->GetObjectClass(env, bpgImgInfo);
    jmethodID widthMethod = (*env)->GetMethodID(env, imgInfoClass, "getWidth", "()I");
    info->width =  (*env)->CallIntMethod(env, bpgImgInfo, widthMethod);

    jmethodID heightMethod = (*env)->GetMethodID(env, imgInfoClass, "getPictureDataLen", "()I");
    info->height =  (*env)->CallIntMethod(env, bpgImgInfo, heightMethod);

    jmethodID pictureDataLenMethod = (*env)->GetMethodID(env, imgInfoClass, "getHeight", "()I");
    info->picture_data_length =  (*env)->CallIntMethod(env, bpgImgInfo, pictureDataLenMethod);

    jmethodID getFormatMethod = (*env)->GetMethodID(env, imgInfoClass, "getFormat", "()I");
    info->format = (*env)->CallByteMethod(env, bpgImgInfo, getFormatMethod);

    jmethodID getHasAlphaMethod = (*env)->GetMethodID(env, imgInfoClass, "hasAlpha", "()I");
    info->has_alpha = (*env)->CallBooleanMethod(env, bpgImgInfo, heightMethod) == JNI_TRUE ? 1 : 0;

    jmethodID getColourSpaceMethod = (*env)->GetMethodID(env, imgInfoClass, "getColourSpace", "()I");
    info->color_space = (*env)->CallByteMethod(env, bpgImgInfo, getColourSpaceMethod);

    jmethodID getBitDepthMethod = (*env)->GetMethodID(env, imgInfoClass, "getBitDepth", "()I");
    info->bit_depth = (*env)->CallByteMethod(env, bpgImgInfo, getBitDepthMethod);

    jmethodID hasPremultipliedAlphaMethod = (*env)->GetMethodID(env, imgInfoClass, "hasPremultipliedAlpha", "()I");
    info->premultiplied_alpha = (*env)->CallBooleanMethod(env, bpgImgInfo, hasPremultipliedAlphaMethod) == JNI_TRUE ? 1 : 0;

    jmethodID hasWPlaneMethod = (*env)->GetMethodID(env, imgInfoClass, "hasWPlane", "()I");
    info->has_w_plane = (*env)->CallBooleanMethod(env, bpgImgInfo, hasWPlaneMethod) == JNI_TRUE ? 1 : 0;

    jmethodID hasLimitedRangeMethod = (*env)->GetMethodID(env, imgInfoClass, "hasLimitedRange", "()I");
    info->limited_range = (*env)->CallBooleanMethod(env, bpgImgInfo, hasLimitedRangeMethod) == JNI_TRUE ? 1 : 0;

    jmethodID hasAnimationMethod = (*env)->GetMethodID(env, imgInfoClass, "hasAnimation", "()I");
    info->has_animation = (*env)->CallBooleanMethod(env, bpgImgInfo, hasAnimationMethod) == JNI_TRUE ? 1 : 0;

    jmethodID hasLoopCountMethod = (*env)->GetMethodID(env, imgInfoClass, "hasLoopCount", "()I");
    info->loop_count = (*env)->CallBooleanMethod(env, bpgImgInfo, hasLoopCountMethod) == JNI_TRUE ? 1 : 0;

    copy_context_from_info(img, info, NULL);

    jboolean isCopy;
    jbyte* cEncArray = (*env)->GetByteArrayElements(env, encBuffer, &isCopy);
    if(NULL == cEncArray){
        __android_log_print(ANDROID_LOG_INFO, "decodeBufferV2", "FAILED to allocate cEncArray");
        return NULL;
    } else {
        jint res = bpg_decoder_decode_body(img, cEncArray, (int)encBufferSize, (int)index);
        int *dec_len = get_bmp_buf_size(info->width, info->height);

        uint8_t* buf_dec;
        bmp_save_to_buf(img, &buf_dec, &dec_len);

        jbyteArray decBuffer = (*env)->NewByteArray(env, buf_dec);
        if(NULL == decBuffer){
            (*env)->ReleaseByteArrayElements(env, buf_dec, cEncArray, JNI_ABORT);
            __android_log_print(ANDROID_LOG_INFO, "decodeBodyBuffer", "FAILED to allocate decBuffer : dec_len=%d", *dec_len);
            return NULL;
        }
        else{
            (*env)->SetByteArrayRegion(env, decBuffer, 0, *dec_len, buf_dec);
        }
        (*env)->ReleaseByteArrayElements(env, encBuffer, cEncArray, JNI_ABORT);

        return decBuffer;
    }
}

JNIEXPORT jbyteArray JNICALL Java_com_facebook_imagepipeline_nativecode_BpgTranscoder_decodeFullImageBuffer(JNIEnv *env, jclass class, jbyteArray encBuffer, jint encBufferSize)
{
    jboolean isCopy;
    jbyteArray decBuffer;
    //get c-style array
    jbyte* cEncArray = (*env)->GetByteArrayElements(env, encBuffer, &isCopy);
    if(NULL == cEncArray){
        __android_log_print(ANDROID_LOG_INFO, "decodeBufferV2", "FAILED to allocate cEncArray");
        return NULL;
    }
    else{
        uint8_t* buf_dec;
        unsigned int dec_len = 0;
        bpg_decoder_decode_buf(cEncArray, encBufferSize, &buf_dec, &dec_len, BMP);

        //convert back to java-style array
        decBuffer = (*env)->NewByteArray(env, dec_len);
        if(NULL == decBuffer){
            (*env)->ReleaseByteArrayElements(env, encBuffer, cEncArray, JNI_ABORT);
            __android_log_print(ANDROID_LOG_INFO, "decodeBodyBuffer", "FAILED to allocate decBuffer : outBufferSize=%d", dec_len);
            return NULL;
        }
        else{
            (*env)->SetByteArrayRegion(env, decBuffer, 0, dec_len, buf_dec);
        }
        (*env)->ReleaseByteArrayElements(env, encBuffer, cEncArray, JNI_ABORT);
    }
    return decBuffer;
}

static JNINativeMethod method_table[] = {
        {"getBmpBufferSize", "([BI)I", (void *) Java_com_facebook_imagepipeline_nativecode_BpgTranscoder_getBmpBufferSize},
        {"decodeBodyBuffer", "([BI)[B", (void *) Java_com_facebook_imagepipeline_nativecode_BpgTranscoder_decodeBodyBuffer},
        {"decodeFullImageBuffer", "([BI)[B", (void *) Java_com_facebook_imagepipeline_nativecode_BpgTranscoder_decodeFullImageBuffer},
};

static int method_table_size = sizeof(method_table) / sizeof(method_table[0]);
jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if ((*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    } else {
        jclass clazz = (*env)->FindClass(env, "com/facebook/imagepipeline/nativecode/BpgTranscoder");
        if (clazz) {
            jint ret = (*env)->RegisterNatives(env, clazz, method_table, method_table_size);
            (*env)->DeleteLocalRef(env, clazz);
            return ret == 0 ? JNI_VERSION_1_6 : JNI_ERR;
        } else {
            return JNI_ERR;
        }
    }
}
