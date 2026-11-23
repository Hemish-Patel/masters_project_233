# Masters Project Group 233 - Autonomous Omni-Wheel Robot

A hardware-agnostic, containerised **ROS 2 Humble** and **Gazebo Fortress** development environment for **MECH 5080 Masters Project Group 233** (Autonomous 3-Wheel Omni-Directional Robot Simulation, Control & Navigation).

---

## Table of Contents
1. [Repository Structure](#1-repository-structure)
2. [Environment & Docker Setup](#2-environment--docker-setup)
3. [Docker & Workspace Architecture](#3-docker--workspace-architecture)
4. [Simulation Architecture](#4-simulation-architecture)
5. [Launching & Controlling the Simulation](#5-launching--controlling-the-simulation)
6. [Development Process & Instructions](#6-development-process--instructions)

---

## 1. Repository Structure

```text
masters_project_233/
├── README.md                   # Project overview & setup guide
├── simulation/                 # Dockerised ROS 2 Humble + Gazebo Fortress simulation
│   ├── Dockerfile              # Container OS, ROS 2 Humble & Gazebo Fortress image definition
│   ├── docker-compose.yml      # Base container configuration & persistent volumes
│   ├── docker-compose.nvidia.yml # NVIDIA GPU hardware acceleration override
│   ├── launch_masters.sh       # Hardware-detecting container launch script
│   └── src/                    # ROS 2 workspace source packages
├── firmware/                   # Low-level microcontroller firmware
├── electrical_schematics/      # Electrical schematics & PCB/wiring diagrams
└── submission_documents/       # Project reports, presentations & submission files
```

---

## 2. Environment & Docker Setup

### Prerequisites & Hardware Support
Before starting, ensure you have the following installed on your Linux host machine:
- **Git**
- **Docker** and **Docker Compose** (`docker compose` plugin)

#### GPU Acceleration Support
The environment automatically adapts to your hardware when launched via `./simulation/launch_masters.sh`:
- **NVIDIA GPUs:** Full hardware acceleration via `simulation/docker-compose.nvidia.yml` (requires **[NVIDIA Container Toolkit](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/latest/install-guide.html)** on the host).
- **AMD & Intel GPUs:** Full hardware acceleration out of the box via Linux Direct Rendering Infrastructure (`/dev/dri`) and Mesa, with no extra toolkit required.
- **No GPU (CPU-only):** Automatically falls back to Mesa CPU software rendering (`llvmpipe`).

### First-Time Setup: From Zero to Running

#### Step 1: Clone the Repository
```bash
git clone git@github.com:Hemish-Patel/masters_project_233.git
cd masters_project_233
```
*(Or using HTTPS: `git clone https://github.com/Hemish-Patel/masters_project_233.git`)*

#### Step 2: Build the Docker Image (Run Once)
This downloads Ubuntu 22.04, ROS 2 Humble, Gazebo Fortress (`ros-humble-ros-gz`), `ros2_control`, and Navigation2 / SLAM dependencies defined in `simulation/Dockerfile`:
```bash
cd simulation
sudo docker compose build
```

#### Step 3: Launch the Container
This script automatically detects your host hardware (NVIDIA, AMD, or Intel) and boots the container with the correct GPU acceleration. You can run it from either the repository root or inside `simulation/`:
```bash
# From the repository root:
./simulation/launch_masters.sh

# Or from inside simulation/:
./launch_masters.sh
```

#### Step 4: Compile the ROS 2 Workspace (Inside the Container)
Once inside the container (`root@<hostname>:/ros2_ws#`), compile the workspace packages using `colcon` and source the overlay:
```bash
colcon build --merge-install
source install/setup.bash
```

> **Note:** After completing the first-time setup above, both `/opt/ros/humble/setup.bash` and `/ros2_ws/install/setup.bash` are **automatically sourced** every time the container starts.

---

## 3. Docker & Workspace Architecture

The simulation environment lives inside `simulation/`, separating the **Docker infrastructure** from the **ROS 2 source packages** (`simulation/src/`), and linking them together via bind mounts and persistent volumes:

| Container Path | Host Location | Purpose |
| :--- | :--- | :--- |
| `/ros2_ws/src` | `./simulation/src` (in this repo folder) | **Live-synced source code.** Edit or create ROS 2 C++/Python packages in VS Code on your host machine; changes appear immediately inside the container. |
| `/ros2_ws/build` | Docker volume (`ros2_build`) | Persistent CMake/Colcon intermediate build files. |
| `/ros2_ws/install` | Docker volume (`ros2_install`) | Persistent compiled ROS 2 binaries and environment hooks. |
| `/ros2_ws/log` | Docker volume (`ros2_log`) | Persistent `colcon` build and runtime logs. |

- **`simulation/Dockerfile`:** Defines the Ubuntu 22.04 + ROS 2 Humble + Gazebo Fortress image and pre-installs the simulation, control, and navigation dependencies.
- **`simulation/docker-compose.yml` & `simulation/docker-compose.nvidia.yml`:** Passes X11 display sockets, `/dev/dri` GPU devices, and NVIDIA PRIME render offload into the container so Gazebo and RViz2 windows open natively on the host desktop.
- **`simulation/launch_masters.sh`:** Grants local X11 display access (`xhost +local:root`), checks for an active NVIDIA GPU via `nvidia-smi`, and starts the container.

---

## 4. Simulation Architecture

### Simulation Package Structure

- `simulation/src/` - ROS 2 Workspace Source Directory
  - **`omniwheel_description/`** - 3D Robot Model (URDF/Xacro) & Gazebo Spawn Launch
    - `urdf/omniwheel.urdf.xacro` - Parameterised chassis and 3 omni-wheels with directional friction
    - `launch/spawn_robot.launch.py` - Launches Gazebo Fortress, spawns the robot and bridges wheel topics
    - `CMakeLists.txt` & `package.xml`
  - **`omniwheel_control/`** - C++ Kinematic & Manual Control Nodes
    - `src/omniwheel_kinematics_node.cpp` - Converts `/cmd_vel` ($v_x, v_y, \omega_z$) into the 3 wheel speeds (rad/s)
    - `src/omniwheel_manualcontrol_node.cpp` - X11 keyboard teleop node (WASD + QE) publishing to `/cmd_vel`
    - `CMakeLists.txt` & `package.xml`

### How ROS 2 Packages Work (`package.xml` & `CMakeLists.txt`)
In C++, `.cpp` source files cannot be executed directly; they must be compiled into binaries and linked against ROS 2 libraries (`rclcpp`, `geometry_msgs`, `std_msgs`). Every ROS 2 package uses two configuration files so `colcon build` handles this automatically:
1. **`package.xml` (Package Manifest & Dependency List):**
   - Identifies the directory as a ROS 2 package (`<name>...</name>`) and declares which ROS 2 libraries (`<depend>rclcpp</depend>`, etc.) it requires. Without `package.xml`, `colcon build` ignores the directory.
2. **`CMakeLists.txt` (Build & Compiler Recipe):**
   - In **`omniwheel_description`:** Uses `install(DIRECTORY urdf launch DESTINATION share/${PROJECT_NAME})` to copy `.xacro` and `.launch.py` files into `/ros2_ws/install/` so `ros2 launch` can find them.
   - In **`omniwheel_control`:** Uses `add_executable(...)` and `ament_target_dependencies(...)` to compile C++ files in `src/` into binaries and install them into `/ros2_ws/install/lib/` so `ros2 run` can execute them.
3. **`src/` and `include/` inside a package:**
   - `src/`: Holds the package's `.cpp` source files.
   - `include/`: Holds optional `.hpp` C++ header files.

### Control & Hardware Isolation Pipeline
To match the physical robot architecture (where the Raspberry Pi 4 sends body velocity commands $(v_x, v_y, \omega_z)$ to a low-level microcontroller that calculates individual wheel speeds and drives the 3 motors), the simulation keeps wheel actuation isolated behind the same interface:

1. **High-Level Control / Teleop (`omniwheel_manualcontrol_node` or Nav2):** Publishes desired body twist $(v_x, v_y, \omega_z)$ on `/cmd_vel` (`geometry_msgs/msg/Twist`).
2. **Low-Level Kinematic Controller (`omniwheel_kinematics_node`):** Subscribes to `/cmd_vel`, applies the inverse kinematics Jacobian $J$:
   $$V_{w,i} = -\sin(\theta_i)\,v_x + \cos(\theta_i)\,v_y + L\,\omega_z, \quad \omega_i = \frac{V_{w,i}}{r_w}$$
   and publishes the 3 target wheel angular velocities (in `rad/s`) to `/wheel_1/cmd_vel`, `/wheel_2/cmd_vel`, and `/wheel_3/cmd_vel` (`std_msgs/msg/Float64`).
3. **Gazebo Bridge & Motors (`ros_gz_bridge` -> `JointController`):** Forwards the 3 wheel velocity commands to Gazebo Fortress's joint velocity controllers.

Each omni-wheel in `omniwheel.urdf.xacro` uses **anisotropic (1-directional) surface friction** (`mu1 = 0.0` along the wheel axle `0 0 1` to model free-sliding passive rollers, and `mu2 = 1.0` along the drive tangent for traction).

---

## 5. Launching & Controlling the Simulation

### Step 1: Launch Gazebo & Spawn the Omni-Wheel Robot
Start the container (`./simulation/launch_masters.sh`) and run:
```bash
ros2 launch omniwheel_description spawn_robot.launch.py
```

### Step 2: Open a Second Terminal Inside the Running Container
On your **host machine**, open a new terminal tab (`Ctrl+Shift+T`) and attach to the running container:
```bash
sudo docker exec -it $(sudo docker ps -q) bash
```

### Step 3 (Option A): Manually Command Individual Wheel Speeds (in `rad/s`)
To test individual wheel motors directly without running the kinematic controller:
```bash
# Spin all 3 wheels at 5.0 rad/s (rotates robot in place)
ign topic -t /wheel_1/cmd_vel -m ignition.msgs.Double -p "data: 5.0"
ign topic -t /wheel_2/cmd_vel -m ignition.msgs.Double -p "data: 5.0"
ign topic -t /wheel_3/cmd_vel -m ignition.msgs.Double -p "data: 5.0"

# Stop all 3 wheels
ign topic -t /wheel_1/cmd_vel -m ignition.msgs.Double -p "data: 0.0"
ign topic -t /wheel_2/cmd_vel -m ignition.msgs.Double -p "data: 0.0"
ign topic -t /wheel_3/cmd_vel -m ignition.msgs.Double -p "data: 0.0"
```
*(Or via the ROS 2 bridge using `-1` to send a single message: `ros2 topic pub -1 /wheel_1/cmd_vel std_msgs/msg/Float64 "{data: 5.0}"`)*

### Step 3 (Option B): Run the C++ Omni-Wheel Controller
In your second container terminal, start the C++ kinematics node:
```bash
ros2 run omniwheel_control omniwheel_kinematics_node
```
Then send body velocity commands $(v_x, v_y, \omega_z)$ on `/cmd_vel` from a third terminal:
```bash
# Drive straight forward along +X at 0.2 m/s
ros2 topic pub /cmd_vel geometry_msgs/msg/Twist "{linear: {x: 0.2, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.0}}"

# Strafe sideways along +Y at 0.2 m/s
ros2 topic pub /cmd_vel geometry_msgs/msg/Twist "{linear: {x: 0.0, y: 0.2, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.0}}"
```

To use your keyboard to control the robot, in another terminal start the manual control node:
```bash
ros2 run omniwheel_control omniwheel_manualcontrol_node
```
Use **WASD** for planar movement ($X/Y$) and **Q / E** for rotation.

---

## 6. Development Process & Instructions

### Rebuilding After Editing Code in `simulation/src/`
Whenever you modify C++ source files, launch files, URDF/Xacro models, or `CMakeLists.txt` inside `simulation/src/`, re-run inside the container:
```bash
cd /ros2_ws
colcon build --merge-install
source install/setup.bash
```

### Creating a New ROS 2 C++ Package
To create an additional C++ package inside `/ros2_ws/src`:
```bash
cd /ros2_ws/src
ros2 pkg create <package_name> --build-type ament_cmake --dependencies rclcpp std_msgs geometry_msgs
chmod -R 777 <package_name>
```
*(Run `chmod -R 777 <package_name>` inside the container so VS Code on the host machine has write permissions to edit the new files.)*

### Resetting / Cleaning the Build Cache
If you ever want to perform a completely clean rebuild (wiping `build/`, `install/`, and `log/` volumes without touching `simulation/src/`), run this on your **host machine**:
```bash
cd simulation
sudo docker compose down -v
```
