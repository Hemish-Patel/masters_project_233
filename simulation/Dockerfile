FROM osrf/ros:humble-desktop

# Install basic development extensions and utilities
RUN apt-get update && apt-get install -y \
    nano \
    python3-colcon-common-extensions \
    mesa-utils \
    wget \
    gnupg \
    && rm -rf /var/lib/apt/lists/*

# Force ROS 2 to link with Gazebo Fortress (official LTS pairing for Humble)
ENV IGNITION_VERSION=fortress

# Install Gazebo Fortress, ros_gz bridges, ros2_control, and Nav2/SLAM packages
RUN apt-get update && apt-get install -y \
    ros-humble-ros-gz \
    ros-humble-ign-ros2-control \
    ros-humble-ros2-control \
    ros-humble-ros2-controllers \
    ros-humble-xacro \
    ros-humble-joint-state-publisher \
    ros-humble-joint-state-publisher-gui \
    ros-humble-robot-localization \
    ros-humble-navigation2 \
    ros-humble-nav2-bringup \
    ros-humble-slam-toolbox \
    ros-humble-teleop-twist-keyboard \
    && rm -rf /var/lib/apt/lists/*

# Enable NVIDIA GPU passthrough for X11/GLX rendering
ENV NVIDIA_VISIBLE_DEVICES \
    ${NVIDIA_VISIBLE_DEVICES:-all}
ENV NVIDIA_DRIVER_CAPABILITIES \
    ${NVIDIA_DRIVER_CAPABILITIES:+$NVIDIA_DRIVER_CAPABILITIES,}graphics,compute,utility

# Set up the workspace
RUN mkdir -p /ros2_ws/src
WORKDIR /ros2_ws

# Auto-source ROS 2 and workspace overlay (if built) on container entry
RUN echo "source /opt/ros/humble/setup.bash" >> ~/.bashrc && \
    echo "if [ -f /ros2_ws/install/setup.bash ]; then source /ros2_ws/install/setup.bash; fi" >> ~/.bashrc

CMD ["/bin/bash"]
