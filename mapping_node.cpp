#include <sstream>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include "ros/ros.h"
#include <sensor_msgs/LaserScan.h>
#include <freefloating_gazebo/BodySetpoint.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace cv;
using namespace std;

class Listener
    {
    public : sensor_msgs::LaserScan last_msg;
        bool test;
        void Callback(const sensor_msgs::LaserScan& msg) {last_msg = msg; test = true;}
        };

// Function headers
void MyLine( Mat img, Point start, Point end, Scalar color);

// Function declaration
void MyLine( Mat img, Point start, Point end, Scalar color)
{
  int thickness = 1;
  int lineType = 8;
  line( img,
    start,
    end,
    color,
    thickness,
    lineType );
}

int main(int argc, char **argv){

    Listener listener;

    ros::init(argc, argv, "display_node");
    ros::NodeHandle nh;
    ros::Subscriber sub = nh.subscribe("/ecn_auv/sonar", 1, & Listener::Callback, & listener);
    ros::Rate loop_rate(100);

    namedWindow("Sea Bed");
    startWindowThread();

    // Create black empty images
    float scale = 21;
    int sizex = 45*scale;
    int sizey = 40*scale;
    Mat display_image = Mat::zeros( sizex, sizey, CV_8UC3 );
    //int loop = 0;
    int image_size=sizey;

    while(ros::ok())
        {
        if (listener.test) {

            int size = listener.last_msg.ranges.size();
            int seq = listener.last_msg.header.seq;
            int middle_up = sizey/2;
            float x_prec = 0;
            float y_prec = 0;

            float angle_min = listener.last_msg.angle_min;
            float angle_max = listener.last_msg.angle_max;
            float angle_increment = listener.last_msg.angle_increment;
            // cout<<angle_min<<"    "<<angle_max<<"    "<<angle_increment<<endl;

            for (int i = 0 ; i<size ; i++) {
                float alpha = angle_min+i*angle_increment;
                float range = scale*listener.last_msg.ranges[i];
                float intensities = listener.last_msg.intensities[i];
                float x = -range*sin(alpha)+middle_up;
                Scalar white(255,255,255);
                Scalar blue(255,0,0);
                if (intensities < 5000){
                    MyLine( display_image, Point(x, 1), Point(x, 1), white);
                }
                if (intensities > 5000){
                    MyLine( display_image, Point(x, 1), Point(x, 1), blue);
                }
            }
            // Display
            imshow("Sea Bed", display_image );

            for (int j = 1 ; j<image_size; j++){
                Mat M1=display_image.row(image_size-j);
                display_image.row(image_size-j-1).copyTo(M1);
            }
            waitKey(100);
        }

        ros::spinOnce();
        loop_rate.sleep();
        }

    cout << "clear" << endl;
    return 0;
}
