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

void detectAndDraw( Mat& img, CascadeClassifier& cascade,
                    CascadeClassifier& nestedCascade,
                    double scale, bool tryflip );

string cascadeName;
string nestedCascadeName;

int DetectObj()
{
    VideoCapture capture;
    Mat frame, image;
    string inputName;
    bool tryflip;
    CascadeClassifier cascade, nestedCascade;
    double scale;

    cascadeName = "/sdcard/Download/image/haarcascade_frontalface_alt.xml";
    nestedCascadeName = "/sdcard/Download/image/haarcascade_eye.xml";
    scale = 1;
    if (scale < 1)
        scale = 1;
    tryflip = false;
    inputName = "/sdcard/Download/image/faces3.jpg";
    //printf("");

    if ( !nestedCascade.load( nestedCascadeName ) )
    {
        printf("WARNING: Could not load classifier cascade for nested objects");;
    }
    if( !cascade.load( cascadeName ) )
    {
        printf("ERROR: Could not load classifier cascade");
        //help();
        return -1;
    }
    if( inputName.empty() || (isdigit(inputName[0]) && inputName.size() == 1) )
    {
        int camera = inputName.empty() ? 0 : inputName[0] - '0';
        if(!capture.open(camera))
            printf("Capture from camera # %d didn't work", camera);
    }
    else if( inputName.size() )
    {
        image = imread( inputName, 1 );
        if( image.empty() )
        {
            if(!capture.open( inputName ))
                printf("Could not read %s image", inputName.c_str());
        }
    }
    else
    {
        image = imread( "/sdcard/Download/image/1127.bmp", 1);
        if(image.empty()) printf("Couldn't read image");
    }

    if( capture.isOpened() )
    {
        printf("Video capturing has been started ...");

        for(;;)
        {
            capture >> frame;
            if( frame.empty() )
                break;

            Mat frame1 = frame.clone();
            detectAndDraw( frame1, cascade, nestedCascade, scale, tryflip );

            char c = (char)waitKey(10);
            if( c == 27 || c == 'q' || c == 'Q' )
                break;
        }
    }
    else
    {
        printf("Detecting face(s) in %s", inputName.c_str());
        if( !image.empty() )
        {
            detectAndDraw( image, cascade, nestedCascade, scale, tryflip );
            waitKey(0);
        }
        else if( !inputName.empty() )
        {
            /* assume it is a text file containing the
            list of the image filenames to be processed - one per line */
            FILE* f = fopen( inputName.c_str(), "rt" );
            if( f )
            {
                char buf[1000+1];
                while( fgets( buf, 1000, f ) )
                {
                    int len = (int)strlen(buf);
                    while( len > 0 && isspace(buf[len-1]) )
                        len--;
                    buf[len] = '\0';
                    //printf("file " << buf << endl;
                    image = imread( buf, 1 );
                    if( !image.empty() )
                    {
                        detectAndDraw( image, cascade, nestedCascade, scale, tryflip );
                        char c = (char)waitKey(0);
                        if( c == 27 || c == 'q' || c == 'Q' )
                            break;
                    }
                    else
                    {
                        printf("Aw snap, couldn't read image ");
                    }
                }
                fclose(f);
            }
        }
    }

    return 0;
}

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

    DetectObj();
    printf("JNICALL After imwrite");

    return env->NewStringUTF(hello.c_str());
}


void detectAndDraw( Mat& img, CascadeClassifier& cascade,
                    CascadeClassifier& nestedCascade,
                    double scale, bool tryflip )
{
    double t = 0;
    vector<Rect> faces, faces2;
    const static Scalar colors[] =
            {
                    Scalar(255,0,0),
                    Scalar(255,128,0),
                    Scalar(255,255,0),
                    Scalar(0,255,0),
                    Scalar(0,128,255),
                    Scalar(0,255,255),
                    Scalar(0,0,255),
                    Scalar(255,0,255)
            };
    Mat gray, smallImg;

    cvtColor( img, gray, COLOR_BGR2GRAY );
    double fx = 1 / scale;
    resize( gray, smallImg, Size(), fx, fx, INTER_LINEAR_EXACT );
    equalizeHist( smallImg, smallImg );

    t = (double)getTickCount();
    cascade.detectMultiScale( smallImg, faces,
                              1.1, 2, 0
                                      //|CASCADE_FIND_BIGGEST_OBJECT
                                      //|CASCADE_DO_ROUGH_SEARCH
                                      |CASCADE_SCALE_IMAGE,
                              Size(30, 30) );
    if( tryflip )
    {
        flip(smallImg, smallImg, 1);
        cascade.detectMultiScale( smallImg, faces2,
                                  1.1, 2, 0
                                          //|CASCADE_FIND_BIGGEST_OBJECT
                                          //|CASCADE_DO_ROUGH_SEARCH
                                          |CASCADE_SCALE_IMAGE,
                                  Size(30, 30) );
        for( vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); ++r )
        {
            faces.push_back(Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
        }
    }
    t = (double)getTickCount() - t;
    printf( "detection time = %g ms\n", t*1000/getTickFrequency());

    for ( size_t i = 0; i < faces.size(); i++ )
    {
        Rect r = faces[i];
        Mat smallImgROI;
        vector<Rect> nestedObjects;
        Point center;
        Scalar color = colors[i%8];
        int radius;

        double aspect_ratio = (double)r.width/r.height;
        if( 0.75 < aspect_ratio && aspect_ratio < 1.3 )
        {
            center.x = cvRound((r.x + r.width*0.5)*scale);
            center.y = cvRound((r.y + r.height*0.5)*scale);
            radius = cvRound((r.width + r.height)*0.25*scale);
            circle( img, center, radius, color, 3, 8, 0 );
        }
        else
            rectangle( img, Point(cvRound(r.x*scale), cvRound(r.y*scale)),
                       Point(cvRound((r.x + r.width-1)*scale), cvRound((r.y + r.height-1)*scale)),
                       color, 3, 8, 0);
        printf( "Before NativeSample if( nestedCascade.empty() ) = %f\n", aspect_ratio);

        if( nestedCascade.empty() )
            continue;
        smallImgROI = smallImg( r );
        nestedCascade.detectMultiScale( smallImgROI, nestedObjects,
                                        1.1, 2, 0
                                                |CASCADE_FIND_BIGGEST_OBJECT
                                                //|CASCADE_DO_ROUGH_SEARCH
                                                //|CASCADE_DO_CANNY_PRUNING
                                                //|CASCADE_SCALE_IMAGE
                                         ,
                                        Size(30, 30) );
        printf( "After NativeSample nestedCascade.detectMultiScale nestedObjects.size() = %d\n", nestedObjects.size());

        for ( size_t j = 0; j < nestedObjects.size(); j++ )
        {
            Rect nr = nestedObjects[j];
            center.x = cvRound((r.x + nr.x + nr.width*0.5)*scale);
            center.y = cvRound((r.y + nr.y + nr.height*0.5)*scale);
            radius = cvRound((nr.width + nr.height)*0.25*scale);
            circle( img, center, radius, color, 3, 8, 0 );
        }
    }
    //imshow( "result", img );
    printf( "Before NativeSample imwrite = %d\n", faces.size());
    imwrite("/sdcard/Download/image/result.png", img);
}
