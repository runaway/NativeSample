#include <jni.h>
#include <string>

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
    std::string hello = "Hello from C++";

    //"/storage/emulated/0/Download/image/"
    SaveFile("/sdcard/Download/image/NativeTest.txt", "NativeTest\n", 11);


    return env->NewStringUTF(hello.c_str());
}
