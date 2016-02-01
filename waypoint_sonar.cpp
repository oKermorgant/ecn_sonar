#include <sstream>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include "ros/ros.h"
#include <nav_msgs/Odometry.h>
#include <freefloating_gazebo/BodySetpoint.h>

using namespace std;

class Listener
    {
    public : nav_msgs::Odometry last_msg;
        bool test;
        void Callback(const nav_msgs::Odometry& msg) {last_msg = msg; test = true;};
        };

int main(int argc, char **argv) {

    Listener listener;

    ros::init(argc, argv, "waypoint_node");
    ros::NodeHandle nh;
    ros::Subscriber sub = nh.subscribe("/ecn_auv/state", 1, & Listener::Callback, & listener);
    ros::Publisher publisher = nh.advertise<freefloating_gazebo::BodySetpoint>("/ecn_auv/body_setpoint", 1);
    ros::Rate loop_rate(100);

    int traj ;
    cout<<"Choose trajectory"<<endl<<"0 = straight line"<<endl<<"1 = Hector's square"<<endl;
    cin>>traj;
    int indice = 1;
    int n_points = 4;

    freefloating_gazebo::BodySetpoint cmd[n_points];

    // pour faire une ligne droite

    if (traj == 0){
        cmd[0].pose.position.x = -20;
        cmd[0].pose.position.y = 0;
        cmd[0].pose.position.z = -5;
        cmd[0].pose.orientation.x = 1;
        cmd[0].pose.orientation.y = 0;
        cmd[0].pose.orientation.z = 0;
        cmd[0].pose.orientation.w = 1;

        cmd[1].pose.position.x = 20;
        cmd[1].pose.position.y = 0;
        cmd[1].pose.position.z = -5;
        cmd[1].pose.orientation.x = 1;
        cmd[1].pose.orientation.y = 0;
        cmd[1].pose.orientation.z = 0;
        cmd[1].pose.orientation.w = 1;

        cmd[2].pose.position.x = 20;
        cmd[2].pose.position.y = 0;
        cmd[2].pose.position.z = -5;
        cmd[2].pose.orientation.x = 1;
        cmd[2].pose.orientation.y = 0;
        cmd[2].pose.orientation.z = 0;
        cmd[2].pose.orientation.w = 1;

        cmd[3].pose.position.x = -20;
        cmd[3].pose.position.y = 0;
        cmd[3].pose.position.z = -5;
        cmd[3].pose.orientation.x = 1;
        cmd[3].pose.orientation.y = 0;
        cmd[3].pose.orientation.z = 0;
        cmd[3].pose.orientation.w = 1;
    }
    else if (traj == 1){    // 0 pour une ligne droite
        // 1 pour la trajectoire d'Hector
        cmd[0].pose.position.x = -5;
        cmd[0].pose.position.y = -5;
        cmd[0].pose.position.z = -5;
        cmd[0].pose.orientation.x = 1;
        cmd[0].pose.orientation.y = 0;
        cmd[0].pose.orientation.z = 0;
        cmd[0].pose.orientation.w = 1;

        cmd[1].pose.position.x = 5;
        cmd[1].pose.position.y = -5;
        cmd[1].pose.position.z = -5;
        cmd[1].pose.orientation.x = 0.707;
        cmd[1].pose.orientation.y = -0.707;
        cmd[1].pose.orientation.z = -0.707;    // 0 pour une ligne droite
        // 1 pour la trajectoire d'Hector
        cmd[1].pose.orientation.w = 0.707;

        cmd[2].pose.position.x = 5;
        cmd[2].pose.position.y = 5;
        cmd[2].pose.position.z = -5;
        cmd[2].pose.orientation.x = 0;
        cmd[2].pose.orientation.y = 1;
        cmd[2].pose.orientation.z = 1;
        cmd[2].pose.orientation.w = 0;

        cmd[3].pose.position.x = -5;
        cmd[3].pose.position.y = 5;
        cmd[3].pose.position.z = -5;
        cmd[3].pose.orientation.x = -0.707;
        cmd[3].pose.orientation.y = -0.707;
        cmd[3].pose.orientation.z = -0.707;
        cmd[3].pose.orientation.w = -0.707;
    }

    while(ros::ok())
        {
        if (listener.test) {

        float x = listener.last_msg.pose.pose.position.x;
        float y = listener.last_msg.pose.pose.position.y;
        float z = listener.last_msg.pose.pose.position.z;
        float ox = listener.last_msg.pose.pose.orientation.x;
        float oy = listener.last_msg.pose.pose.orientation.y;
        float oz = listener.last_msg.pose.pose.orientation.z;
        float ow = listener.last_msg.pose.pose.orientation.w;

        float diff = (cmd[indice].pose.position.x-x)*(cmd[indice].pose.position.x-x)+(cmd[indice].pose.position.y-y)*(cmd[indice].pose.position.y-y)+(cmd[indice].pose.position.z-z)*(cmd[indice].pose.position.z-z);
        float diffo = (cmd[indice].pose.orientation.x-ox)*(cmd[indice].pose.orientation.x-ox)+(cmd[indice].pose.orientation.y-oy)*(cmd[indice].pose.orientation.y-oy);
        diffo = diffo+(cmd[indice].pose.orientation.z-oz)*(cmd[indice].pose.orientation.z-oz)+(cmd[indice].pose.orientation.w-ow)*(cmd[indice].pose.orientation.w-ow);
        //cout << "diff: " << diff << " - x: " << x << " - y: " << y << " - z: " << z << endl;
        if((diff<2.)&&(diffo<2.))
        {
                cout << "Arrived to the point number " << indice << endl;
                indice = (indice + 1) % 4;
                cout << "Going to the point number " << indice << endl;
        }


        publisher.publish(cmd[indice]);
        }
        ros::spinOnce();
        loop_rate.sleep();
        }

    cout << "clear" << endl;
    return 0;
}
