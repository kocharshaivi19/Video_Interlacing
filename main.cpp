/**
    Iris Automation
    main.cpp
    Purpose: Interlaces video frames using OpenCV in c++ in Multithreaded structure

    @author Shaivi Kochar
    @version 1.0 3/30/17
*/
#include <iostream>
#include <stdio.h>  /* defines FILENAME_MAX */
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <mutex>
#include <thread>
#include <queue>
#include <unistd.h>
#define GetCurrentDir getcwd

using namespace std;
using namespace cv;


/*
     PROPOSED METHOD:
     1. Class Interlacing: Constructor of the class opens the writer to write video. Destructor releases the writer
     after the process ends. Used variables as Private and Functions as public.
     2. Using intiVideo, I aim to grab the minimum width, height, frame rate and total frames.
     3. Called readVideo in Threads (main and t1)
     4. Parallely readVideo access the writeVideo method to write to the output file alternatively using mutex.

     - Used VideoCapture to read the Videos from Opencv lib
     - Used thread lib to create threads
     - Used mutex lib to access critical sections

*/

/*
   1. Interlacing class that includes variables and functions to read, store and write Video Frames
   processing on two different threads. They commonly access one Queue to store the frame in an alternative
   fashion. Things like, Video Frame Size, Type, Frames per second is kept to the minimum of all the given Videos.
*/

class Interlacing{
private:
    mutex mtx;
    double min_width;
    double min_height;
    double min_mf;
    double min_fps;
    bool isColor;
    int count1;
    int count2;
    string outfile;
    VideoWriter writer;

public:
    Interlacing(string outfile);
    ~Interlacing();
    void openWriter();
    void initVideo(string url);
    void readVideo(string url, string name);
    void writeVideo(Mat frame, string name);
};

/*
   Constructor that initialises the values
*/
Interlacing::Interlacing(string file) {
    min_width = DBL_MAX;
    min_height = DBL_MAX;
    min_mf = DBL_MAX;
    min_fps = DBL_MAX;
    isColor = true;
    count1 = 0;
    count2 = 0;
    outfile = file;

}

/*
   Destructor to release the opened video
*/
Interlacing::~Interlacing() {
    writer.release();
}

/*
   Open the file for VideoWriter to write the grabbed frames.
*/
void Interlacing::openWriter() {

    writer.open(outfile, CV_FOURCC('H','2','6','4'), min_fps, Size(640, 360), isColor);
}

/*
   The stitching into one output stream should be completed inside the threads using semaphores
   to access a shared resource (the output file).
*/
void Interlacing::writeVideo(Mat frame, string name) {
    mtx.lock();
    cout << "Thread writing: " << name << endl;
    if(!writer.isOpened()){
        cout << "ERROR: Failed to write the video" << endl;
        exit(0);
    }
    bool isColor = (frame.type() == CV_8UC3);
    // cout << "isColor: " << isColor << endl;
    if (!isColor){
        Mat new_frame;
        frame.convertTo(new_frame,CV_8UC3);
        new_frame.copyTo(frame);
    }
    resize(frame, frame, Size(640, 360), INTER_CUBIC);
    writer.write(frame);
    imshow("inter", frame);
    waitKey(5);
    // cout << "Done: " << name << endl;
    mtx.unlock();
}

/*
   Read the video to set properties of the frame upto the length of the smaller video size
   and pass it onto writeVideo function
*/
void Interlacing::readVideo(string url, string name) {
    // cout << "Thread " << name << endl;
    VideoCapture cap(url);
    if (!cap.isOpened()){
        cout << "Error Empty Error" << endl;
    }
    // cout << "min_mf: " << min_mf << endl;
    while (true){
        if (name.compare("1")){
            count1 += 1 ; //do count1++;
        }
        else{
            count2 += 1 ;
        }
        Mat frame;
        cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
        cap.set(CV_CAP_PROP_FRAME_HEIGHT, 360);
        cap.set(CV_CAP_PROP_FPS, min_fps);
        if (count1 < min_mf or count2 < min_mf) {
             cout << "count1: " << count1 << "count2: " << count2 << endl;
            if (cap.read(frame)){
                writeVideo(frame, name);
            }
            else {
                // cout << "count1: " << count1 << " " << "count2: " << count2 << endl;
                break;
            }
        }

    }
}

/*
   Function that extracts the ninimum width, height, and frame rate of the two videos.
*/
void Interlacing::initVideo(string url) {
    VideoCapture cap(url);
    if (!cap.isOpened()) {
        cout << "Error while reading the video! Empty" << endl;
    }
    double total_frames = cap.get(7); // get the total frames
    double wi = cap.get(3); // get the width of the frame
    double hi = cap.get(4); // get the height of the frame
    double fps = cap.get(5); // get the
    cout << "Frames in Video: " << total_frames << " Frames per sec: " << fps << endl;
    cout << "Frame width: " << wi << " , Frame height: " << hi << endl;
    if (min_mf > total_frames) {
        min_mf = total_frames;
    }
    if (min_width > wi) {
        min_width = wi;
    }

    if (min_height > hi) {
        min_height = hi;
    }
    if (min_fps > fps) {
        min_fps = fps;
    }
    cout << "Min Frames in Video: " << min_mf << endl;
}

int main(int argc, const char *argv[]) {

    vector<string> capture_source = {
            "../data/dynamic_test.mp4",
            "../data/field_trees.mp4"
    };
    string outfile = "../interlaced.avi";

    Interlacing it(outfile);

    for (int i=0; i<capture_source.size(); i++) {
        it.initVideo(capture_source[i]);
    }

    it.openWriter();

    std::thread t1(&Interlacing::readVideo, &it, capture_source[0], "1");

    it.readVideo(capture_source[1], "main");

    t1.join();

    return 0;
}