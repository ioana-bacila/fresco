#include <jni.h>
#include <stdio.h>
#include <android/log.h>

#include "libbpg/libbpg.h"
#include "libbpg/bpgdec.h"

#include "BpgImageInfo.h"

JNIEXPORT jint JNICALL Java_com_facebook_imagepipeline_bpg_BpgImageInfo_getImageInfoFromBuf(JNIEnv *env, jobject thisObj, jbyteArray headerBuf, jint buf_len)
{
    BPGImageInfo i, *info = &i;

    jboolean isCopy;
    jbyte* header_buf = (*env)->GetByteArrayElements(env, headerBuf, &isCopy);
    int idx = bpg_decoder_get_info_from_buf(info, NULL, header_buf, (int) buf_len);

    jclass bpgImageInstance = (*env)->GetObjectClass(env, thisObj);

    jfieldID fidWidth = (*env)->GetFieldID(env, bpgImageInstance, "width", "I");
    jint width = (*env)->GetIntField(env, thisObj, fidWidth);
    width = (jint)info->width;
    (*env)->SetIntField(env, thisObj, fidWidth, width);

    jfieldID fidHeight = (*env)->GetFieldID(env, bpgImageInstance, "height", "I");
    jint height = (*env)->GetIntField(env, thisObj, fidHeight);
    height = (jint)info->height;
    (*env)->SetIntField(env, thisObj, fidHeight, height);

    jfieldID fidPictureDataLen = (*env)->GetFieldID(env, bpgImageInstance, "pictureDataLen", "I");
    jint pictureDataLen = (*env)->GetIntField(env, thisObj, fidHeight);
    pictureDataLen = (jint)info->picture_data_length;
    (*env)->SetIntField(env, thisObj, fidPictureDataLen, pictureDataLen);

    jfieldID fidFormat = (*env)->GetFieldID(env, bpgImageInstance, "format", "I");
    jbyte format = (*env)->GetByteField(env, thisObj, fidFormat);
    format = (jbyte)info->format;
    (*env)->SetByteField(env, thisObj, fidFormat, format);

    jfieldID fidHasAlpha = (*env)->GetFieldID(env, bpgImageInstance, "hasAlpha", "I");
    jboolean hasAlpha = (*env)->GetBooleanField(env, thisObj, fidHasAlpha);
    hasAlpha = (jboolean)(info->has_alpha == 1);
    (*env)->SetBooleanField(env, thisObj, fidHasAlpha, hasAlpha);

    jfieldID fidColourSpace = (*env)->GetFieldID(env, bpgImageInstance, "colourSpace", "I");
    jbyte colourSpace = (*env)->GetByteField(env, thisObj, fidColourSpace);
    colourSpace = (jbyte)info->color_space;
    (*env)->SetByteField(env, thisObj, fidColourSpace, colourSpace);

    jfieldID fidBitDepth = (*env)->GetFieldID(env, bpgImageInstance, "bitDepth", "I");
    jbyte bitDepth = (*env)->GetByteField(env, thisObj, fidBitDepth);
    bitDepth = (jbyte)info->bit_depth;
    (*env)->SetByteField(env, thisObj, fidBitDepth, bitDepth);

    jfieldID fidHasPremultipliedAlpha = (*env)->GetFieldID(env, bpgImageInstance, "hasPremultipliedAlpha", "I");
    jboolean hasPremultipliedAlpha = (*env)->GetBooleanField(env, thisObj, fidHasPremultipliedAlpha);
    hasPremultipliedAlpha = (jboolean)(info->premultiplied_alpha == 1);
    (*env)->SetBooleanField(env, thisObj, fidHasPremultipliedAlpha, hasPremultipliedAlpha);

    jfieldID fidHasWPlane = (*env)->GetFieldID(env, bpgImageInstance, "hasWPlane", "I");
    jboolean hasWPlane = (*env)->GetBooleanField(env, thisObj, fidHasWPlane);
    hasWPlane = (jboolean)(info->has_w_plane == 1);
    (*env)->SetBooleanField(env, thisObj, fidHasWPlane, hasWPlane);

    jfieldID fidLimitedRange = (*env)->GetFieldID(env, bpgImageInstance, "limitedRange", "I");
    jboolean limitedRange = (*env)->GetBooleanField(env, thisObj, fidLimitedRange);
    limitedRange = (jboolean)(info->limited_range == 1);
    (*env)->SetBooleanField(env, thisObj, fidLimitedRange, limitedRange);

    jfieldID fidHasAnimation = (*env)->GetFieldID(env, bpgImageInstance, "hasAnimation", "I");
    jboolean hasAnimation = (*env)->GetBooleanField(env, thisObj, fidHasAnimation);
    hasAnimation = (jboolean)(info->has_animation == 1);
    (*env)->SetBooleanField(env, thisObj, fidHasAnimation, hasAnimation);

    jfieldID fidLoopCount = (*env)->GetFieldID(env, bpgImageInstance, "loopCount", "I");
    jshort loopCount = (*env)->GetShortField(env, thisObj, fidLoopCount);
    loopCount = (jboolean)(info->has_animation == 1);
    (*env)->SetBooleanField(env, thisObj, fidLoopCount, loopCount);

    return (jint)idx;
}

static JNINativeMethod method_table[] = {
        {"getImageInfoFromBuf", "([BI)I", (void *) Java_com_facebook_imagepipeline_bpg_BpgImageInfo_getImageInfoFromBuf},
};

static int method_table_size = sizeof(method_table) / sizeof(method_table[0]);
jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if ((*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    } else {
        jclass clazz = (*env)->FindClass(env, "com/facebook/imagepipeline/bpg/BpgImage");
        if (clazz) {
            jint ret = (*env)->RegisterNatives(env, clazz, method_table, method_table_size);
            (*env)->DeleteLocalRef(env, clazz);
            return ret == 0 ? JNI_VERSION_1_6 : JNI_ERR;
        } else {
            return JNI_ERR;
        }
    }
}
