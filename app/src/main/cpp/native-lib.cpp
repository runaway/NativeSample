#include <jni.h>
#include <string>


#include <cstdio>

#include<stdio.h>
//#include<cpu-features.h>

#include <android/log.h>

#include <opencv/cv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

//#include <opencv2/opencv.hpp>
#define TAG "log"
#define printf(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)

#define  LOG_TAG    "native-dev"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

using namespace std;
using namespace cv;


#if 1
extern "C"
{
void SaveFile(char* pcFilePath, char* pcBuf, int iLength)
{
    FILE *pFile;
    //char buffer[] = {'x', 'y', 'z'};
    pFile = fopen(pcFilePath, "wb");
    fwrite(pcBuf, sizeof(char), iLength, pFile);
    fclose(pFile);
}

};
#endif
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_david_nativesample_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from Native8 C++";

    //"/storage/emulated/0/Download/image/"
    //SaveFile("/sdcard/Download/image/NativeTest.txt", "NativeTest\n", 11);
    //LOGI("JNICALL Before imread");
    printf("JNICALL Before imread");
    Mat matTemp;
    matTemp = cv::imread("/sdcard/Download/image/1127.bmp");
    printf("JNICALL After imread");
    //cvtColor();
    imwrite("/sdcard/Download/image/1127.png", matTemp);
    printf("JNICALL After imwrite");

    return env->NewStringUTF(hello.c_str());
}
