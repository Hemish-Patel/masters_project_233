#include <iostream> 
#include <termios.h>
#include <unistd.h>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

#include <X11/Xlib.h>
#include <X11/keysym.h>

Display* dpy = XOpenDisplay(NULL);
    

bool isKeyPressed(Display* dpy, const char keys[32], KeySym sym) {
    KeyCode kc = XKeysymToKeycode(dpy, sym);
    return (keys[kc / 8] & (1 << (kc % 8))) != 0;
}

double linear_speed = 0.5; // m/s
double angular_speed = 1.5; // rad/s
rclcpp::WallRate loop_rate(50);

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("omniwheel_manualcontrol_node");
    auto publisher = node->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);

    if (!dpy) {
            std::cerr << "Failed to open X11 display" << std::endl;
            return -1;
        }

    char keys[32];

    while(rclcpp::ok()) {
        geometry_msgs::msg::Twist msg;

        
        XQueryKeymap(dpy, keys);

        if (isKeyPressed(dpy, keys, XK_w)) msg.linear.x += linear_speed;
        if (isKeyPressed(dpy, keys, XK_s)) msg.linear.x += -linear_speed;
        if (isKeyPressed(dpy, keys, XK_a)) msg.linear.y += linear_speed;
        if (isKeyPressed(dpy, keys, XK_d)) msg.linear.y += -linear_speed;
        if (isKeyPressed(dpy, keys, XK_q)) msg.angular.z += angular_speed;
        if (isKeyPressed(dpy, keys, XK_e)) msg.angular.z += -angular_speed;

        publisher->publish(msg);
        rclcpp::spin_some(node);
        loop_rate.sleep();
    }
    XCloseDisplay(dpy);
    rclcpp::shutdown();
    return 0;

}