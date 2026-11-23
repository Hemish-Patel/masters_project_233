#include <cmath>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/float64.hpp"

struct WheelSpeeds {
    double w1; // rad/s
    double w2; // rad/s
    double w3; // rad/s
};

// Robot physical constants (keep consistent with URDF)
constexpr double L   = 0.170; // Distance from centre to wheel (m)
constexpr double R_W = 0.050; // Omni-wheel radius (m)

// Wheel mounting angles in radians (W1 = 180 deg, W2 = -60 deg, W3 = +60 deg)
constexpr double THETA_1 = M_PI;
constexpr double THETA_2 = -M_PI / 3.0;
constexpr double THETA_3 =  M_PI / 3.0;

WheelSpeeds computeInverseKinematics(double vx, double vy, double wz) {
    // Inverse kinematics Jacobian: V_wi = -sin(theta_i)*vx + cos(theta_i)*vy + L*wz
    // Angular velocity: w_i = V_wi / r_w (negated as joint +Z axis points radially outward)
    double v_w1 = -std::sin(THETA_1) * vx + std::cos(THETA_1) * vy + L * wz;
    double v_w2 = -std::sin(THETA_2) * vx + std::cos(THETA_2) * vy + L * wz;
    double v_w3 = -std::sin(THETA_3) * vx + std::cos(THETA_3) * vy + L * wz;

    return {
        -v_w1 / R_W,
        -v_w2 / R_W,
        -v_w3 / R_W
    };
}

class OmniwheelKinematicsNode : public rclcpp::Node {
public:
    OmniwheelKinematicsNode() : Node("omniwheel_kinematics_node") {
        pub_w1_ = this->create_publisher<std_msgs::msg::Float64>("/wheel_1/cmd_vel", 10);
        pub_w2_ = this->create_publisher<std_msgs::msg::Float64>("/wheel_2/cmd_vel", 10);
        pub_w3_ = this->create_publisher<std_msgs::msg::Float64>("/wheel_3/cmd_vel", 10);

        sub_cmd_vel_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "/cmd_vel", 10,
            std::bind(&OmniwheelKinematicsNode::cmdVelCallback, this, std::placeholders::_1));

        RCLCPP_INFO(this->get_logger(), "Omni-wheel kinematics node started, listening on /cmd_vel");
    }

private:
    void cmdVelCallback(const geometry_msgs::msg::Twist::SharedPtr msg) {
        WheelSpeeds speeds = computeInverseKinematics(
            msg->linear.x,
            msg->linear.y,
            msg->angular.z
        );

        std_msgs::msg::Float64 m1, m2, m3;
        m1.data = speeds.w1;
        m2.data = speeds.w2;
        m3.data = speeds.w3;

        pub_w1_->publish(m1);
        pub_w2_->publish(m2);
        pub_w3_->publish(m3);
    }

    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_w1_, pub_w2_, pub_w3_;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub_cmd_vel_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<OmniwheelKinematicsNode>());
    rclcpp::shutdown();
    return 0;
}
