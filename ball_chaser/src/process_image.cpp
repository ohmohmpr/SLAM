#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ROS_INFO_STREAM("Drive the robot");

    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv))
        ROS_ERROR("Failed to call service safe_move");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    int ball_position = 0;
    int left = img.step / 3;
    int center = img.step / 3 * 2;
    int right = img.step;
    int c_left = 0;
    int c_center = 0;
    int c_right = 0;
    
    // TODO: Loop through each pixel in the image and check if there's a bright white one
    for (int i = 0; i < img.height; i++) {
        for (int j = 0; j < img.step; j+=3) {
            int index = i * j;
            if ((img.data[index] == white_pixel) && (img.data[index + 1] == white_pixel) && (img.data[index + 2] == white_pixel) ) {
                // ROS_INFO_STREAM("I: " << i);
                // ROS_INFO_STREAM("HEIGHT: " << img.height);
                // ROS_INFO_STREAM("INDEX: " << index);
                // ROS_INFO_STREAM("LEFT:" << left);
                // ROS_INFO_STREAM("CENTER:" <<center);
                // ROS_INFO_STREAM("RIGHT:" << right);
                // ROS_INFO_STREAM("STEP:" << img.step);
                // ROS_INFO_STREAM("J:" << j);
                if ((left < j) && (j < center)) {
                    // ball_position = 2;
                    // ROS_INFO_STREAM("STRAIGHT");
                    c_center++;
                } else if (j <= left) {
                    // ball_position = 1;
                    // ROS_INFO_STREAM("GO LEFT");
                    c_left++;
                } else if ((center <= j) && (j < right)) {
                    // ball_position = 3;
                    // ROS_INFO_STREAM("GO RIGHT");
                    c_right++;
                }
            }
            if (ball_position != 0)
            {
                break;
            }
        }
        if (ball_position != 0)
        {
            break;
        }
    }
    ROS_INFO_STREAM("LEFT:" << c_left);
    ROS_INFO_STREAM("CENTER:" <<c_center);
    ROS_INFO_STREAM("RIGHT:" << c_right);
    if ((c_right < c_left) && (c_center < c_left)) {
        drive_robot(0.1, 0.1);
        ROS_INFO_STREAM("GO LEFT");
    }
    else if ((c_left < c_center) && (c_right < c_center)) {
        drive_robot(0.1, 0.0);
        ROS_INFO_STREAM("STRAIGHT");
    }
    else if ((c_center <= c_right) && (c_left < c_right)) {
        drive_robot(0.1, -0.1);
        ROS_INFO_STREAM("GO RIGHT");
    }
    else {
        drive_robot(0.0, 0.0);
    }
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
