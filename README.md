# Masters Project Group 233 - ROS 2 & Gazebo Workspace

A hardware-agnostic, containerized **ROS 2 Humble** and **Gazebo Fortress** development environment for Group 233 (Autonomous Kiwi Drive Robot Simulation, Control & Navigation).

---

## Prerequisites & Hardware Support

Before starting, ensure you have the following installed on your Linux host machine:
- **Git**
- **Docker** and **Docker Compose** (`docker compose` plugin)

### GPU Acceleration Support
The environment automatically adapts to your hardware when launched via `./launch_masters.sh`:
- **NVIDIA GPUs:** Full hardware acceleration via `docker-compose.nvidia.yml` (requires **[NVIDIA Container Toolkit](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/latest/install-guide.html)** on the host).
- **AMD & Intel GPUs:** Full hardware acceleration out of the box via Linux Direct Rendering Infrastructure (`/dev/dri`) and Mesa—no extra toolkit required.
- **No GPU (CPU-only):** Automatically falls back to Mesa CPU software rendering (`llvmpipe`).

---

## Quick Start: From Zero to Running Gazebo

### 1. Clone the Repository
Clone this workspace to your computer and navigate into the folder:

```bash
git clone git@github.com:Hemish-Patel/masters_project_233.git
cd masters_project_233
```
*(Or using HTTPS: `git clone https://github.com/Hemish-Patel/masters_project_233.git`)*

---

### 2. Build the Docker Image (Run Once)
This downloads Ubuntu 22.04, ROS 2 Humble, Gazebo Fortress (`ros-humble-ros-gz`), `ros2_control`, and Navigation2 / SLAM dependencies defined in the `Dockerfile`:

```bash
sudo docker compose build
```

---

### 3. Launch the Container
This script automatically detects your host hardware (NVIDIA, AMD, or Intel) and boots the container with the correct GPU acceleration:

```bash
./launch_masters.sh
```

---

### 4. Compile the ROS 2 Workspace
Once inside the container (`root@<hostname>:/ros2_ws#`), compile your packages using `colcon` and source the workspace:

```bash
colcon build --merge-install
source install/setup.bash
```

> **Note:** Your compiled `build/`, `install/`, and `log/` folders are saved inside persistent Docker volumes. You **do not** need to run `colcon build` again unless you edit or add packages in `src/`.

---

### 5. Verify Gazebo Fortress Simulation
Inside the container, you can test that Gazebo Fortress and GPU rendering are working by running:

```bash
ign gazebo shapes.sdf
```
*(Or launch via ROS 2: `ros2 launch ros_gz_sim gz_sim.launch.py gz_args:="shapes.sdf"`)*

---

## Subsequent Runs (Daily Usage)

After completing the first-time setup above, both `/opt/ros/humble/setup.bash` and `/ros2_ws/install/setup.bash` are **automatically sourced** every time the container starts.

Next time you open a terminal, simply run:

1. **On your host machine:**
   ```bash
   cd ~/masters_project_233
   ./launch_masters.sh
   ```
2. **Inside the container:**
   Run your robot launch files directly!

---

## Development Workflow & File Storage

| Container Path | Host Location | Purpose |
| :--- | :--- | :--- |
| `/ros2_ws/src` | `./src` (in this repo folder) | **Live-synced source code.** Edit or create ROS 2 C++/Python packages in VS Code on your host machine; changes appear immediately inside the container. |
| `/ros2_ws/build` | Docker volume (`ros2_build`) | Persistent CMake/Colcon intermediate build files. |
| `/ros2_ws/install` | Docker volume (`ros2_install`) | Persistent compiled ROS 2 binaries and environment hooks. |
| `/ros2_ws/log` | Docker volume (`ros2_log`) | Persistent `colcon` build and runtime logs. |

### Rebuilding After Editing Code in `src/`
Whenever you modify C++ code, launch files, URDF/Xacro, or add a new package inside `src/`, re-run inside the container:

```bash
colcon build --merge-install
source install/setup.bash
```

### Resetting / Cleaning the Build Cache
If you ever want to perform a completely clean rebuild (wiping `build/`, `install/`, and `log/` without touching `src/`), run this on your **host machine**:

```bash
sudo docker compose down -v
```
