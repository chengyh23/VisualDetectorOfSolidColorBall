无人船比赛：检测球及其颜色

（标签）一般使用只需看RELEASE ，调试需要参考DEBUG

## RELEASE 编译
```sh
git clone https://github.com/sysu18364015/visual.git
cd visual
mkdir build && cd build
cmake ..
make
```

## RELEASE 使用

在visual/build/文件夹下：
```sh
roscore
source <path/to/devel/setup.sh>
roslaunch hikvision_ros hik.launch
```
两个关键的ROS publisher:

image发布者:topic名称`/hik_camera`,topic数据类型 `sensor_msgs::Image`

color发布者:topic名称`/ballcolor`,topic数据类型 `turtlesim::Color`

## DEBUG 关于相机实时图像回传
***frame频率***

使用rand降低处理频率，这部分代码在src/hik_cam.cpp中：
```
void HikvisionCamera::decodeCallback(int nPort, char *pBuf, int nSize, FRAME_INFO *pFrameInfo)
{
    int lFrameType = pFrameInfo->nType;

    if (lFrameType == T_YV12)
    {
        int x=rand()%16+1;  
        if(x!=1) return;

        cv::Mat picBGR;
        cv::Mat picYV12 = cv::Mat(pFrameInfo->nHeight * 3/2, pFrameInfo->nWidth, CV_8UC1, pBuf);
        cv::cvtColor(picYV12, picBGR, cv::COLOR_YUV2BGR_YV12);
        COLOR c = detect_pipeline(picBGR);
        ...
```
***原图还是加了检测结果的图***

detect_pipeline.h中定义是否画block颜色检测、矩形框、形状的圆形检测

## DEBUG 改参数

params.h

**颜色统计值** src/color.cpp--colorClassify

## DEBUG 可执行文件
在visual/build/devel/lib/hikvision_ros/文件夹下可以看到编译出来的几个executable：

**localtest**

涉及主要的函数调用为：src/localtest.cpp中：decodeCallback--detect_pipeline

使用方法：

首先 图像所在文件夹下生成imgs.txt
```shell script
ls *.jpg > imgs.txt
```
然后 在visual/build/devel/lib/hikvision_ros/下执行
```shell scripth
./localtest path/to/images/
```

**hik_cam_node**

src/hik_cam.cpp中：decodeCallback--detect_pipeline

## DEBUG 算法流程

find_horinzon_line

colorDetect

	colorDetectBlock：得到属于各颜色的block的list

		colorClassify

		框内颜色占比例：这个GRID中不到1/3的像素点为这个颜色，则认为结果不对

		框内颜色一致性：这个GRID RGB STDERR大于阈值，则认为结果不对

	seaskyline.cpp中的filterByLine

		取线的最高和最低点均值作为阈值，过滤掉海天线上方的

	getColorCirclesRect：各颜色的block的list用矩形框住（GRID左上角坐标，目标框应包含至右下角）

		GRID数量太少则忽略

		去掉NUM_OUTIERS个最大最小值

		rectangle长宽比：如果太大认为它不是球，过滤掉
		
		矩形最上方距离海天线：如果大于图像高度的1/6，过滤
        
        矩形最上方如果在图像高度的下1/4之下，过滤

	取面积最大的长方形对应的那个颜色作为返回值


## 可能用到的修改
***parameters***

You can specify some camera and steam parameters by `hik.launch`

```xml
<arg name="ip_addr" default="192.168.5.100"/>
<arg name="user_name" default="admin"/>
<arg name="password" default="admin"/>
<arg name="port" default="8000"/>
<arg name="channel" default="1"/>
```

Or in command line

```sh
roslaunch hikvision_ros hik.launch ip_addr:=192.168.5.100 password:=123456
```



***support for camera_calibration***

you can use [camera_calibration](http://wiki.ros.org/camera_calibration/)  to calibrate hikvision camera, **hik_ros**  provides *set_camera_info* sevice for receiving and storing camera's calibration parameters. 

***example***

```sh
# open camera
roslaunch hikvision_ros hik.launch

# see topic name
rostopic list

# [output]
# ➜  ~ rostopic list
# /hik_cam_node/camera_info
# /hik_cam_node/hik_camera
# ...

# calibrate
rosrun camera_calibration cameracalibrator.py --size 11x8 --square 0.03 image:=/hik_cam_node/hik_camera  camera:=/hik_cam_node/hik_camera
```

then begin calibration. After calibration you straightly press **commit** button,  **hik_ros** has the ability to save the calibration parameter to `camera_info_url`, which is set in launch file OR use default path (  `~/.ros/camera_info` )   

```sh
# [output]
#[ INFO] [1551582484.454024618]: New camera info received
#[ INFO] [1551582484.454296067]: [ hik_camera ] Write camera_info to ~/.ros/camera_info/hik_camera.yaml success.
```
