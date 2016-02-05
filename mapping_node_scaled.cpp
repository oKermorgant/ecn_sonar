#include <sstream>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include "ros/ros.h"
#include <sensor_msgs/LaserScan.h>
#include <freefloating_gazebo/BodySetpoint.h>
#include <nav_msgs/Odometry.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace cv;
using namespace std;

class Listener
    {
    public : sensor_msgs::LaserScan last_msg_LaserScan;
    public : nav_msgs::Odometry last_msg_Pose;
    bool test_LaserScan;
    bool test_Pose;
    void Callback_LaserScan(const sensor_msgs::LaserScan& msg) {last_msg_LaserScan = msg; test_LaserScan = true;}
    void Callback_Pose(const nav_msgs::Odometry& msg) {last_msg_Pose = msg; test_Pose = true;};
    };

// Repartition of noise function
float noise_repartition(float intensity);

float noise_repartition(float intensity){
    return intensity/255;
}


int main(int argc, char **argv){

    Listener listener_LaserScan;
    Listener listener_Pose;

    ros::init(argc, argv, "display_node");
    ros::NodeHandle nh;
    ros::Subscriber sub_LaserScan = nh.subscribe("/ecn_auv/sonar", 1, & Listener::Callback_LaserScan, & listener_LaserScan);
    ros::Subscriber sub_Pose = nh.subscribe("/ecn_auv/state", 1, & Listener::Callback_Pose, & listener_Pose);
    ros::Rate loop_rate(100);

    namedWindow("Sea Bed");
    startWindowThread();

    // Create black empty images
    float scale = 20;
    int sizex = 45*scale;
    int sizey = 40*scale;
    Mat K = Mat::zeros(sizex, sizey, CV_8UC3);
    Mat display_image = K;
    Mat scale_image = Mat::zeros(sizey/10, sizey/5, CV_8UC3);
    int loop = 0;
    float x;
    float y;
    float x_prec = 0;
    float y_prec = 0;
    float xmin;
    float xmax;

    while(ros::ok())
        {
        if (listener_LaserScan.test_LaserScan && listener_Pose.test_Pose) {

            int size = listener_LaserScan.last_msg_LaserScan.ranges.size();
            int middle_up = sizey/2;
            int dist ;

            float angle_min = listener_LaserScan.last_msg_LaserScan.angle_min;
            float angle_max = listener_LaserScan.last_msg_LaserScan.angle_max;
            float angle_increment = listener_LaserScan.last_msg_LaserScan.angle_increment;

            float xP = listener_Pose.last_msg_Pose.pose.pose.position.x;
            float yP = listener_Pose.last_msg_Pose.pose.pose.position.y;

            // To calculate the angle
/*
            float q0 = listener.last_msg_Pose.pose.pose.orientation.x;
            float q1 = listener.last_msg_Pose.pose.pose.orientation.y;
            float q2 = listener.last_msg_Pose.pose.pose.orientation.z;
            float q3 = listener.last_msg_Pose.pose.pose.orientation.w;
            float alpha = atan2(2*(q0*q1+q2*q3),1-2*(q1*q1+q2*q2));
*/
            xmin = 10000000;
            xmax = 0;

            for (int i = 0 ; i<size ; i++) {
                float alpha = angle_min+i*angle_increment;
                float range = listener_LaserScan.last_msg_LaserScan.ranges[i];
                float intensities = listener_LaserScan.last_msg_LaserScan.intensities[i];
                x = -scale*range*sin(alpha)+(rand() % 2 - 1)+middle_up;
                y = xP;

                // Extracting min and max of x

                if (x>xmax){
                    xmax = x;
                }
                if (x<xmin){
                    xmin = x;
                }

                // Greyscale profile
                float final_intensities=intensities/(range*range);
                float light = 6;
                Scalar greyscale = Scalar::all(light*final_intensities);
                // Drawing out the line, more and more with the range
                line(display_image, Point(x, 1), Point(x+range*range/(4*scale), 1), greyscale,1,8,0);

            }

            if (loop>1){
                dist = abs(scale*(y-y_prec)) ;
                // Scrolling image dist-further
                for (int j = 1 ; j<sizey-dist ; j++){
                    display_image.row(sizey-dist-1-j).copyTo(display_image.row(sizey-j));
                }
                // Copying the first line on dist
                for (int n = sizey-dist-1 ; n<sizey ; n++){
                    display_image.row(1).copyTo(display_image.row(sizey-n));

                    // Adding noise
                    Mat noise = Mat::zeros(1, sizey, CV_8UC3);
                    // Mat eq = Mat::zeros(1, sizey, CV_8UC3);
                    for (int m = 0 ; m<sizey ; m++){
                        if (!(m<xmin || m>xmax)){
                            // eq.col(m)= Scalar::all(10);
                            int grey = rand() % 50 ;
                            // int grey2 = grey*noise_repartition(grey);
                            noise.col(m)= Scalar::all(grey);
                        }
                    }
                    Mat display_noise = display_image.row(1)+noise;
                    display_noise.row(0).copyTo(display_image.row(sizey-n+1));

                    namedWindow( "rline", CV_WINDOW_AUTOSIZE );
                    imshow("rline",noise);

//                    Mat noise = Mat(Size(xmax-xmin,1),CV_8UC3);
 //                   normalize(display_image.row(sizey-n), display_image.row(sizey-n), 0.0, 2.0, CV_MINMAX, CV_64F);
   //                 randn(noise, 0, 0.1);
     //               display_image.row(sizey-n) = display_image.row(sizey-n) + noise;
                }
                // Erasing the 1st line
                K.row(0).copyTo(display_image.row(1));
            }

            // Display
            imshow("Sea Bed", display_image );

            float resolution_y = 100;
            waitKey(resolution_y);

            y_prec = y ;
            loop = loop+1;
        }

        ros::spinOnce();
        loop_rate.sleep();
        }

    cout << "clear" << endl;
    return 0;
}
