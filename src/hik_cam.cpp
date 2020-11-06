#include "../include/hik_cam.h"
#include "color.h"
#include "seaskyline.h"
#include "kmeans_minibatch.h"

std::string HikvisionCamera::expandUserPath(std::string path)
{
    if (not path.empty() and path[0] == '~') {
        assert(path.size() == 1 or path[1] == '/');  // or other error handling
        char const* home = getenv("HOME");
        if (home or ((home = getenv("USERPROFILE")))) {
            path.replace(0, 1, home);
        }
        else {
            char const *hdrive = getenv("HOMEDRIVE"),
                    *hpath = getenv("HOMEPATH");
            assert(hdrive);  // or other error handling
            assert(hpath);
            path.replace(0, 1, std::string(hdrive) + hpath);
        }
    }
    return path;
}
    
void HikvisionCamera::decodeCallback(int nPort, char *pBuf, int nSize, FRAME_INFO *pFrameInfo)
{
    int lFrameType = pFrameInfo->nType;

    if (lFrameType == T_YV12)
    {

        cv::Mat picBGR;
        cv::Mat picYV12 = cv::Mat(pFrameInfo->nHeight * 3/2, pFrameInfo->nWidth, CV_8UC1, pBuf);
        cv::cvtColor(picYV12, picBGR, cv::COLOR_YUV2BGR_YV12);

        int x=rand()%16+1;
        if(x!=1) return;
        if(x==1){
            // 海天线检测
            cv::Point h1(0, 0), h2(0, 0);
            find_horinzon_line(picBGR, 0, 0.0, 2.0, h1, h2);
            cv::line(picBGR,h1,h2,Scalar(255,0,0),2,cv::LINE_AA);
            // 检测颜色，返回colorList
            std::vector<std::vector<Point>> colorList = colorDetect(picBGR);
            // 根据海天线过滤colorList，得到colorListFiltered
            std::vector<std::vector<Point>> colorListFilered;
            for(int i=0;i<colorList.size();i++){
                std::vector<Point> colorPoints = filterByLine(h1,h2,colorList[i]);
                colorListFilered.push_back(colorPoints);
            }
            // 利用colorListFiltered
            int flag=0;
            char outcolorstr[256]={0};
            for(int i=0;i<colorListFilered.size();i++){
                drawBlockColorCircle(picBGR,colorListFilered[i],COLOR(i));
                cv::Rect roi_rect = getColorCirclesRect(colorListFilered[i]);
                drawColorCirclesRect(picBGR,roi_rect,COLOR(i));
                if(roi_rect!=cv::Rect()){
                    flag=1;
                    switch (i) {
                        case YELLOW:
                            sprintf(outcolorstr,"YELLOW  ");break;
                        case GREEN:
                            sprintf(outcolorstr,"GREEN   ");break;
                        case BLACK:
                            sprintf(outcolorstr,"BLACK   ");break;
                        case RED:
                            sprintf(outcolorstr,"RED ");break;
                        
                    }
                }

            }
            
            if(flag==0) ROS_INFO("NO BALL");
            else ROS_INFO(outcolorstr);
        }

        const char* root_path="/home/d402/hikvision_ros/img_cache/";
        char path[256]={0};
        // int idx=25;
        // sprintf(path,"%s%d%s",root_path,idx,ext);
        const char* imgname="tmp.jpg";
        sprintf(path,"%s%s",root_path,imgname);
        x=rand()%16+1;
        if(x==1) cv::imwrite(path,picBGR);
        #ifndef NDEBUG
            ROS_INFO("[%s] imwrite to %s",camera_name.c_str(),path);
        #endif
        
        sensor_msgs::CameraInfoPtr ci(new sensor_msgs::CameraInfo(camera_info_mgr->getCameraInfo()));
        ci->header = camera_info_mgr->getCameraInfo().header;
        ci->header.stamp = ros::Time::now();

        sensor_msgs::ImagePtr msg = cv_bridge::CvImage(ci->header, "bgr8", picBGR).toImageMsg();

        // publish the image
        image_pub.publish(*msg, *ci);

        #ifndef NDEBUG
            ROS_INFO("[%s] Stream CallBack, Convert YV12 to sensor_msgs.",camera_name.c_str());
        #endif
    }
}

void HikvisionCamera::dataCallback(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize)
{
    int dRet = 0;
    
    // //-------设备抓图 NET_DVR_CaptureJPEGPicture-------------
    // NET_DVR_JPEGPARA struJpegPARA={0};
    // struJpegPARA.wPicSize=0xff;//??????
    // struJpegPARA.wPicQuality=2;//0最好，1较好，2一般

    // bool iCaptureJpegValue;
    // int iChannel=1;
    // const char* root_path="/home/deepdriving/hikvision_ros/img_cache/";
    // const char* ext=".jpg";
    // char path[256]={0};
    // // int idx=25;
    // // sprintf(path,"%s%d%s",root_path,idx,ext);
    // const char* imgname="tmp.jpg";
    // sprintf(path,"%s%s",root_path,imgname);
    // iCaptureJpegValue=NET_DVR_CaptureJPEGPicture(user_id,iChannel,&struJpegPARA,path);
    // if(iCaptureJpegValue) printf("[CaptureJPGPicture]---%s:%d success, \n",ip_addr.c_str(),iChannel);
    // else printf("[CaptureJPGPicture]---%s:%d failed, error = %d\n",ip_addr.c_str(),iChannel,NET_DVR_GetLastError());
    // //------------------------------------------------------

    
    switch (dwDataType)
    {
        case NET_DVR_SYSHEAD:    //system head
        {
            if (!PlayM4_GetPort(&port)) //unavailable lPort
            {
                break;
            }
            if (dwBufSize > 0)
            {
                if (!PlayM4_SetStreamOpenMode(port, STREAME_REALTIME))  //set strean mode
                {
                    break;
                }
                if (!PlayM4_OpenStream(port, pBuffer, dwBufSize, image_width * image_height))
                {
                    dRet = PlayM4_GetLastError(port);
                    break;
                }
                //decoding callback
                if (!PlayM4_SetDecCallBackMend(port, decodeCallback_, this))
                {
                    dRet = PlayM4_GetLastError(port);
                    break;
                }
                //open decoding
                if (!PlayM4_Play(port, 0)) // play the video stream
                {
                    dRet = PlayM4_GetLastError(port);
                    break;
                }
            }
        } break;


        case NET_DVR_STREAMDATA:
        {
            if (dwBufSize > 0 && port != -1)
            {
                if (!PlayM4_InputData(port, pBuffer, dwBufSize))
                {
                    break;
                }
            }
        } break;

        default:
        {
            if (dwBufSize > 0 && port != -1)
            {
                if (!PlayM4_InputData(port, pBuffer, dwBufSize))
                {
                    break;
                }
            }
        } break;
    }
    if(dRet != 0){
        ROS_INFO("[%s] Error Occurs when data callback, get: %s",camera_name.c_str(),
                 get_error_str(NET_DVR_GetLastError()).c_str());
    }
}

bool HikvisionCamera::setCameraInfo(sensor_msgs::SetCameraInfo::Request& req,
                   sensor_msgs::SetCameraInfo::Response& rsp)
{
    ROS_INFO("New camera info received");
    sensor_msgs::CameraInfo &info = req.camera_info;
    info.header.frame_id = frame_id;

    if (info.width != image_width || info.height != image_height) {
        rsp.success = 0;
        char buf[100];
        sprintf(buf,"Camera_info resolution %ix%i does not match current video "
                    "setting, camera running at resolution %ix%i.",info.width,info.height,image_width,image_height);
        rsp.status_message = buf;
        ROS_ERROR("%s", rsp.status_message.c_str());
        return true;
    }
    if(camera_info_url.empty())
        camera_info_url = expandUserPath("~/.ros/camera_info/" + camera_name + ".yaml");

    if(!camera_calibration_parsers::writeCalibration(camera_info_url,camera_name,info))
    {
        rsp.status_message = "Error formatting camera_info for storage. filename = "+camera_info_url;
        rsp.success = 0;
    }
    else
    {
        rsp.success = 1;
        rsp.status_message = "Write camera_info to "+camera_info_url+" success.";
    }

    if (!rsp.success) {
        ROS_ERROR("[ %s ] %s",camera_name.c_str(), rsp.status_message.c_str());
    }
    else{
        ROS_INFO("[ %s ] %s",camera_name.c_str(), rsp.status_message.c_str());
    }
    return true;
}

bool HikvisionCamera::initHikSDK()
{
    NET_DVR_Init();
    #ifndef NDEBUG
            NET_DVR_SetLogToFile(3, "./hiklog");
    #endif
    NET_DVR_USER_LOGIN_INFO struLoginInfo = {0};
    NET_DVR_DEVICEINFO_V40  struDeviceInfoV40 = {0};
    struLoginInfo.bUseAsynLogin = false;

    struLoginInfo.wPort = port;
    memcpy(struLoginInfo.sDeviceAddress,ip_addr.c_str(), NET_DVR_DEV_ADDRESS_MAX_LEN);
    memcpy(struLoginInfo.sUserName, usr_name.c_str(), NAME_LEN);
    memcpy(struLoginInfo.sPassword, password.c_str(), NAME_LEN);
    user_id = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);

    if (user_id < 0)
    {
        ROS_INFO("[%s] Login fail, get: %s",camera_name.c_str(), get_error_str(NET_DVR_GetLastError()).c_str());
        return false;
    }
    return true;


}
bool HikvisionCamera::setRealPlayCallBack(){
    //Set callback function of getting stream.
    NET_DVR_PREVIEWINFO struPlayInfo = {0};
    struPlayInfo.hPlayWnd     = 0;
    struPlayInfo.lChannel     = channel;  //channel NO
    struPlayInfo.dwLinkMode   = link_mode;
    struPlayInfo.bBlocked     = 1;
    struPlayInfo.dwDisplayBufNum = 1;

    data_play_handler = NET_DVR_RealPlay_V40(user_id, &struPlayInfo, nullptr, nullptr);

    if (data_play_handler < 0)
    {
        int i = NET_DVR_GetLastError();
        if(i == 0)
        {
            sleep(1);
            data_play_handler = NET_DVR_RealPlay_V40(user_id, &struPlayInfo, nullptr, nullptr);
        }
        if (data_play_handler < 0) {
            ROS_ERROR("[%s] Get RealPlay Handler error, get: %s",camera_name.c_str(),
                      get_error_str(NET_DVR_GetLastError()).c_str());
            return false;
        }
    }
    ROS_INFO("[%s] Set Play Handler successully.",camera_name.c_str());

    //Set callback function of getting stream.
    int iRet = NET_DVR_SetStandardDataCallBackEx(data_play_handler, dataCallback, this);
    if (!iRet)
    {
        ROS_ERROR("[%s] Set DataCall error, get: %s", camera_name.c_str(), get_error_str(NET_DVR_GetLastError()).c_str());
        return false;
    }

    else ROS_INFO("[%s] Set Data Callback successully.",camera_name.c_str());
    return true;
}
void HikvisionCamera::initROSIO(ros::NodeHandle& priv_node)
{
    /// camera parameter
    priv_node.param("camera_frame_id", frame_id, std::string("hik_camera"));
    priv_node.param("camera_name", camera_name,  std::string("hik_camera"));
    priv_node.param("camera_info_url", camera_info_url, std::string(""));

    priv_node.param<std::string>("ip_addr", ip_addr,"192.168.5.100");
    ROS_INFO("[%s] ip address:\t%s", camera_name.c_str(), ip_addr.c_str());

    priv_node.param<std::string>("usr_name",usr_name,"admin");
    ROS_INFO("[%s] user name: \t%s", camera_name.c_str(), usr_name.c_str());

    priv_node.param<std::string>("password",password,"ht123456");
    ROS_INFO("[%s] password:  \t%s", camera_name.c_str(), password.c_str());

    priv_node.param<int>("port",port, 8000);
    ROS_INFO("[%s] port:      \t%d", camera_name.c_str(), port);

    priv_node.param<int>("channel",channel,1);
    ROS_INFO("[%s] channel:   \t%d", camera_name.c_str(), channel);

    priv_node.param<int>("link_mode",link_mode, 0);
    if(link_mode < 0 || link_mode >5)
    {
        ROS_WARN("[%s] value %d for link_mode is illegal, set to default value 0 (tcp)",camera_name.c_str(), link_mode);
    }

    std::string _mode []  = {"tcp", "udp", "multicast","rtp","rtp/rtsp", "rstp/http"};
    ROS_INFO("[%s] link mode: \t%s", camera_name.c_str(), _mode[link_mode].c_str());

    priv_node.param<int>("image_width",image_width,1280);
    ROS_INFO("[%s] image width:  \t%d", camera_name.c_str(), image_width);

    priv_node.param<int>("image_height",image_height, 720);
    ROS_INFO("[%s] image height: \t%d", camera_name.c_str(), image_height);

    
    camera_info_mgr.reset(new camera_info_manager::CameraInfoManager(priv_node, camera_name, camera_info_url));
    if (!camera_info_mgr->isCalibrated())
    {
        camera_info_mgr->setCameraName(camera_name);
        sensor_msgs::CameraInfo camera_info;
        camera_info.header.frame_id = frame_id;
        camera_info.width = (unsigned int)(image_width);
        camera_info.height = (unsigned int)(image_height);
        camera_info_mgr->setCameraInfo(camera_info);
    }


    image_transport::ImageTransport it(priv_node);
    image_pub = it.advertiseCamera(camera_name, 1);

    SetCameraInfoSrv = priv_node.advertiseService(camera_name+"/set_camera_info",&HikvisionCamera::setCameraInfo,this);
}

void HikvisionCamera::run()
{
    ros::NodeHandle priv_node("~");

    initROSIO(priv_node);

    if(initHikSDK()&&setRealPlayCallBack())
    {
        ros::spin();
    }
}
 /*
void HikvisionCamera::run() {
    ros::NodeHandle priv_node("~");

    initROSIO(priv_node);
    if(!initHikSDK()) return;

    NET_DVR_SetCapturePictureMode(1);// JPEG mode
    sensor_msgs::CameraInfoPtr ci(new sensor_msgs::CameraInfo(camera_info_mgr->getCameraInfo()));
    ci->header = camera_info_mgr->getCameraInfo().header;
    ci->header.stamp = ros::Time::now();

    ros::Rate rate(2);
    while(ros::ok()){
        DWORD dwPicSize = image_width * image_height;
        char pPicBuf[dwPicSize];
        DWORD lpSizeReturned;
        if(NET_DVR_CapturePictureBlock_New(user_id, pPicBuf, dwPicSize, &lpSizeReturned)){
            cv::Mat pic = cv::Mat(image_height,image_width,CV_8UC3,pPicBuf);

            sensor_msgs::ImagePtr msg = cv_bridge::CvImage(ci->header, "bgr8", pic).toImageMsg();
            // publish the image
            image_pub.publish(*msg, *ci);
        }
        ROS_INFO("NET_DVR_CapturePictureBlock_New");
        rate.sleep();
    }
}
*/
HikvisionCamera::~HikvisionCamera()
{

    if(user_id)
    {
        NET_DVR_Logout_V30(user_id);
    }
    if(data_play_handler>0)
    {
        NET_DVR_StopRealPlay(data_play_handler);
    }

    NET_DVR_Cleanup();

    ROS_INFO("[%s] END",camera_name.c_str());
}
