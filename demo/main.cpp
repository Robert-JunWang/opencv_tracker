

/////////////////////////////////////////////////
#include <opencv2/core/ocl.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking/tracker.hpp>

using namespace cv;
using namespace std;

// Convert to string
#define SSTR(x) static_cast<std::ostringstream&>(            \
                    (std::ostringstream() << std::dec << x)) \
                    .str()

#include <iostream>

#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

Ptr<Tracker> build_tracker(string trackerType) {
    // List of tracker types in OpenCV 3.4.1
    // string trackerTypes[8] = {"BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN", "MOSSE", "CSRT"};
    // // vector <string> trackerTypes(types, std::end(types));

    // // Create a tracker
    // string trackerType = trackerTypes[2];

    Ptr<Tracker> tracker;

    if (trackerType == "BOOSTING") {
        tracker = TrackerBoosting::create();
    } else if (trackerType == "MIL") {
        tracker = TrackerMIL::create();
    } else if (trackerType == "KCF") {
        tracker = TrackerKCF::create();
    } else if (trackerType == "TLD") {
        tracker = TrackerTLD::create();
    } else if (trackerType == "MEDIANFLOW") {
        tracker = TrackerMedianFlow::create();
    } else if (trackerType == "GOTURN") {
        tracker = TrackerGOTURN::create();
    } else if (trackerType == "MOSSE") {
        tracker = TrackerMOSSE::create();
    } else if (trackerType == "CSRT") {
        tracker = TrackerCSRT::create();
    } else {
        printf("Invalid type: %s\n", trackerType.c_str());
    }

    return tracker;
}


static void benchmark(Ptr<Tracker> tracker, const Mat& frame, const Rect2d& bbox) {
    tracker->init(frame, bbox);

    int total = 200;
    Rect2d out;
    for(int i = 0; i < 10; ++i) {
        bool ok = tracker->update(frame, out);
    }
    auto tStart = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < total; ++i) {
        bool ok = tracker->update(frame, out);
    }

    auto tEnd = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration<float, std::milli>(tEnd - tStart).count();
    ms /= total;
    printf("Speed for %.0fx%.0f: %.2f ms, %.2f FPS\n", 
    bbox.width, bbox.height,
    ms, 1000.0/ms);

    // Rect2d box2 = Rect2d(bbox.x-bbox.width,
    // bbox.y-bbox.height,
    // 331,
    // 331);

    // tracker->init(frame, box2);

    // tStart = std::chrono::high_resolution_clock::now();

    // for(int i = 0; i < total; ++i) {
    //     bool ok = tracker->update(frame, out);
    // }

    // tEnd = std::chrono::high_resolution_clock::now();
    // ms = std::chrono::duration<float, std::milli>(tEnd - tStart).count();
    // ms /= total;
    // printf("Speed for %.0fx%.0f: %.2f ms, %.2f FPS\n", 
    // box2.width, box2.height,
    // ms, 1000.0/ms);

}

static void printUsage() {
    printf("\n");
    printf("tracker <type> (KCF | TLD |CSRT) <bench_mark>(optional, 1: benchmark) <video_file>(optional) >\n");

    fflush(stdout);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    auto tracker = build_tracker(argv[1]);
    if (tracker == nullptr) {
        return 1;
    }

    // Create a VideoCapture object and use camera to capture the video
    VideoCapture cap(0);
    std::string video_name;

    bool isBenchmark = false;
    if (argc >= 3) {
        if(argv[2][0] == '1')
        isBenchmark = true;
    }

    if (argc >= 4) {
        cap = VideoCapture(argv[3]);
        video_name = argv[3];
        //cout << "Loading video:" << video_name << endl;
        std::string key("/");
        std::size_t found = video_name.rfind(key);
        if (found != std::string::npos) {
            video_name = video_name.substr(found + 1, video_name.length() - found - 1);
            cout << video_name << endl;
        }
    }


    // Check if camera opened successfully
    if (!cap.isOpened()) {
        cout << "Error opening video stream" << endl;
        return -1;
    }

    // // Default resolution of the frame is obtained.The default resolution is system dependent.
    // int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    // int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

    Rect2d bbox;
    map<string, Rect2d> default_rois = {
        {string("vehicle001.mp4"), Rect2d(489, 365, 112, 56)}};

    bool isFirstFrame = true;
    while (1) {
        Mat frame;

        // Capture frame-by-frame
        cap >> frame;

        // If the frame is empty, break immediately
        if (frame.empty())
            break;

        if (isFirstFrame) {
            if(default_rois.find(video_name)!=default_rois.end()) {
                bbox = default_rois[video_name];
            } else {
                bbox = selectROI("Select Target", frame);
                destroyWindow("Select Target");
            }

            if(isBenchmark) {
                benchmark(tracker, frame, bbox);
                break;

            } else {
                imshow("Tracking", frame);
                tracker->init(frame, bbox);
                isFirstFrame = false;
                continue;
            }


        }

        // Update the tracking result
        bool ok = tracker->update(frame, bbox);

        if (ok) {
            // Tracking success : Draw the tracked object
            rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);
        } else {
            // Tracking failure detected.
            putText(frame, "Tracking failure detected", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
        }

        // Display the resulting frame
        imshow("Tracking", frame);

        // Press  ESC on keyboard to  exit
        char c = (char)waitKey(1);
        if (c == 27)
            break;
    }

    // When everything done, release the video capture and write object
    cap.release();
    // video.release();

    // Closes all the windows
    destroyAllWindows();
    return 0;
}