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

    // Create white empty images
    float scale = 21;
    int sizex = 15*scale;
    int sizey = 40*scale;
    Mat display_image = Mat::zeros( sizex, sizey, CV_8UC3 );

    while(ros::ok())
        {
        if (listener.test) {

            int size = listener.last_msg.ranges.size();
            int seq = listener.last_msg.header.seq;
            cout<<seq<<endl;
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
                float x = -range*sin(alpha)+middle_up;
                float y = range*cos(alpha);

                if (i>0){
                    Scalar white(255,255,255);
                    // To draw points
                    MyLine( display_image, Point(x, y), Point(x, y), white);
                    // To draw lines (uncomment)
                    // MyLine( display_image, Point(x_prec, y_prec), Point(x, y), white);
                }
                x_prec = x;
                y_prec = y;
            }

            // Display
            imshow("Sea Bed", display_image );
            // ?? autosize (true);

            waitKey(100);
            display_image = Mat::zeros( sizex, sizey, CV_8UC3 );
        }

        ros::spinOnce();
        loop_rate.sleep();
        }

    cout << "clear" << endl;
    return 0;
}
