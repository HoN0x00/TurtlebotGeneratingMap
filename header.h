#ifndef _HEADER_

#define _HEADER_

#include <ros/ros.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <tf/tf.h>
#include <iomanip>
#include <boost/thread/mutex.hpp>
#include <ros/callback_queue.h>
#include <time.h>
#include <iostream>
#include <string>
#include <nav_msgs/Odometry.h>
#include <nav_msgs/OccupancyGrid.h>
#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/Image.h>
#include <geometry_msgs/Twist.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cv_bridge/cv_bridge.h>

#include <turtlebot_actions/TurtlebotMoveAction.h>
#include <actionlib/client/simple_action_client.h>

#define ANGLE_COUNT 200
//using by depthCheck_depth

boost::mutex mutex[5];
//0 - nav_msgs::Odometry msg  /odom
//1 - nav_msgs::OccupancyGrid msg map
//2 - sensor_msgs::Image msg  /camera/rgb/image_color
//3 - sensor_msgs::Image msg  /camera/depth_registered/image_raw
//4 - sensor_msgs::LaserScan g_scan

nav_msgs::Odometry g_odom;
nav_msgs::OccupancyGrid g_map;
sensor_msgs::Image g_rgb;
sensor_msgs::Image g_depth;
sensor_msgs::LaserScan g_scan;
cv::Mat g_display;
int g_distance;

void odomMsgCallback(const nav_msgs::Odometry &msg)
{
    mutex[0].lock(); {
        g_odom = msg;
    } mutex[0].unlock();
}
void occupancyGrid_Callback(const nav_msgs::OccupancyGrid &msg)
{
  mutex[1].lock(); {
      g_map = msg;
  } mutex[1].unlock();
}
void poseMessageReceivedRGB(const sensor_msgs::Image& msg)
{
  mutex[2].lock(); {
      g_rgb = msg;
  } mutex[2].unlock();
}
void poseMessageReceivedDepthRaw(const sensor_msgs::Image& msg)
{
  mutex[3].lock(); {
      g_depth = msg;
  } mutex[3].unlock();
}
void scanMsgCallback(const sensor_msgs::LaserScan& msg)
{
    mutex[4].lock(); {
        g_scan = msg;
    } mutex[4].unlock();
}

void error_Handle(std::string message)
{
  std::cout << message << std::endl;
  exit(1);
}

int distancelimit() //거리입력
{
    int return_value = 0;

    std::cout << "Input exit distance (>1 Meter) : ";
    std::cin >> return_value;

    if(return_value < 1)
    {
      std::string message = "input_error";
      error_Handle(message);
    }

    return return_value;
}

int depthCheck_depth()  //1 is block , 0 is nonblock
{
  int blockandnonblock=0; //0: block, 1:nonblock
  sensor_msgs::LaserScan msg;

  mutex[4].lock(); {
     msg = g_scan;
  } mutex[4].unlock();

  int size_ranges_index = msg.ranges.size();
  int size_ranges_index_divide = size_ranges_index/2;

  for(int i = 0;i<ANGLE_COUNT;i++)
  {
    if(msg.ranges[size_ranges_index_divide+i]<1)
    {
      blockandnonblock = 1;
    }
  }

  for(int i = 0;i<ANGLE_COUNT;i++)
  {
    if(msg.ranges[size_ranges_index_divide-i]<1)
    {
      blockandnonblock = 1;
    }
  }

  return blockandnonblock;
}



#endif