import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
import xacro


def generate_launch_description():
    pkg_dir = get_package_share_directory('omniwheel_description')
    xacro_file = os.path.join(pkg_dir, 'urdf', 'omniwheel.urdf.xacro')

    robot_description_raw = xacro.process_file(xacro_file).toxml()

    node_robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        output='screen',
        parameters=[{'robot_description': robot_description_raw, 'use_sim_time': True}],
    )

    gz_sim = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(get_package_share_directory('ros_gz_sim'), 'launch',
            'gz_sim.launch.py')
        ),
        launch_arguments={'gz_args': '-r empty.sdf'}.items(),
    )

    spawn_entity = Node(
        package='ros_gz_sim',
        executable='create',
        output='screen',
        arguments=[
            '-topic', 'robot_description',
            '-name', 'omniwheel_robot',
            '-z', '0.05',
        ],
    )

    # Bridge the 3 individual wheel speed topics (rad/s) between ROS 2 and Gazebo
    bridge = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        output='screen',
        arguments=[
            '/wheel_1/cmd_vel@std_msgs/msg/Float64]ignition.msgs.Double',
            '/wheel_2/cmd_vel@std_msgs/msg/Float64]ignition.msgs.Double',
            '/wheel_3/cmd_vel@std_msgs/msg/Float64]ignition.msgs.Double',
        ],
    )

    return LaunchDescription([
        node_robot_state_publisher,
        gz_sim,
        spawn_entity,
        bridge,
    ])