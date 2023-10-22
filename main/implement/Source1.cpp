#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<fstream>
using namespace cv;
using namespace std;

int main() {
    // Open the webcam
   
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cout << "NO"<<endl;

    }
    // Set image dimensions
    int width =1000;
    int height = 760;

    // Set capture frame size
    cap.set(CAP_PROP_FRAME_WIDTH, width);
    cap.set(CAP_PROP_FRAME_HEIGHT, height);

    // Calculate frame area
    int area = width * height;

    
    // Initialize average frame
    Mat frame, avg;
    
    cap.read(frame);
    blur(frame, avg, Size(4, 4));
    //avg = frame;
    Mat avgFloat = Mat(avg.size(), CV_32FC3);
    avg.convertTo(avgFloat, CV_32FC3);

    int location_count = 0;
    int count = 0;
    int COUNT = 15;
    int ant_x[18], ant_y[18];
    for (int i = 0; i < 18; i++) {
        ant_x[i] = 0;
        ant_y[i] = 0;
    }
    int ant_x_avg = 0, ant_y_avg = 0;

    int fx=100, fy=100;
    int ex =758, ey = 50;
    int dir_x=0;
    ifstream in;
    ofstream out;
    ofstream out_location;
    out.open("slope.txt");
    out_location.open("location.txt");
    while (cap.isOpened()) {
        
        // Read a frame
        cap.read(frame);
        line(frame, Point(fx, fy), Point(ex, ey), Scalar(0, 255, 255), 2);
        // If the reading is unsuccessful, break out of the loop
        if (frame.empty())
            break;

        Mat avgblack_hsv;
        cvtColor(avg, avgblack_hsv, COLOR_BGR2HSV);
        Mat avgblack;
        inRange(avgblack_hsv, Scalar(0, 0, 0), Scalar(180, 255, 46), avgblack);

        Mat avgblack_o;
        bitwise_and(avg, avg, avgblack_o, avgblack = avgblack);

        Mat black_hsv;
        cvtColor(frame, black_hsv, COLOR_BGR2HSV);
        Mat black;
        inRange(black_hsv, Scalar(0, 0, 0), Scalar(180, 255, 46), black);

        Mat black_o;
        bitwise_and(frame, frame, black_o, black = black);

        // Calculate the difference between the current frame and the average frame
        Mat diff;
        absdiff(avgblack_o, black_o, diff);
        

        // Convert the image to grayscale
        Mat gray;
        cvtColor(black_o, gray, COLOR_BGR2GRAY);

        // Threshold to identify regions of interest
        threshold(gray, gray, 25, 255, THRESH_BINARY);

        // Morphological operations to remove noise
        Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
        morphologyEx(gray, gray, MORPH_OPEN, kernel, Point(-1, -1), 2);
        morphologyEx(gray, gray, MORPH_CLOSE, kernel, Point(-1, -1), 2);

        // Find contours
        vector<vector<Point>> contours;
        findContours(gray.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);


        for (const auto& c : contours) {
            // Ignore small areas
            if (contourArea(c)>1000)
                continue;
            

            
            // Calculate bounding rectangle
            Rect boundingRect = cv::boundingRect(c);
            if (count < COUNT) {
                if (boundingRect.x - ant_x[count]>=0) {
                    dir_x++;
                }
                else {
                    dir_x--;
                }
                ant_x[count] = boundingRect.x;
                ant_x_avg += ant_x[count];
                
                ant_y[count] = boundingRect.y;
                ant_y_avg += ant_y[count];

                count++;
                cout << "count:" << count << endl;
            }
            else {
                ant_x_avg /= COUNT;
                ant_y_avg /= COUNT;
                float xy = 0,xx=0;

                for (int i = 0; i < COUNT; i++) {
                    xy += (ant_x[i] - ant_x_avg) * (ant_y[i] - ant_y_avg);
                    xx += (ant_x[i] - ant_x_avg) * (ant_x[i] - ant_x_avg);
                    cout << "xyxx:"<<xy<<" " << xx << endl;
                }
                float slope_tan = xy / xx;
                fx = ant_x[0];
                fy = ant_y[0];
                int b = ant_y[0] - slope_tan * ant_x[0];
                ex = 100;
                ey = slope_tan * 100 + b;
                int slope;
                if (slope_tan>0 && dir_x>0)slope = atan(slope_tan) * 180.0 / 3.14;
                else if (slope_tan<=0 && dir_x>0)slope = atan(slope_tan) * 180.0 / 3.14+360;
                else if (slope_tan>0 && dir_x<=0)slope = atan(slope_tan) * 180.0 / 3.14+180;
                else slope = atan(slope_tan) * 180.0 / 3.14+180;
               
                
                out << slope << endl;
                cout <<slope<< endl;
                count = 0;
                ant_x_avg = 0;
                ant_y_avg = 0;
            }
            cout << boundingRect.x << " " << boundingRect.y<< endl;
            out_location<< boundingRect.x << " " << boundingRect.y << endl;
            location_count++;
            if (location_count > 1000) {
                out_location.close();
                out_location.open("location.txt");
                location_count = 0;
            }
            // Draw the rectangle
            rectangle(frame, boundingRect.tl(), boundingRect.br(), Scalar(0, 255, 0), 2);
        }

        // Draw contours for debugging
        

        // Display the result
        imshow("frame", frame);

        // Break the loop if 'q' is pressed
        if (waitKey(1) == 'q')
            break;

        // Update the average frame
        accumulateWeighted(frame, avgFloat, 0.01);
        convertScaleAbs(avgFloat, avg);
    }

    cap.release();
    destroyAllWindows();
    out.close();
    out_location.close();
    return 0;
}

