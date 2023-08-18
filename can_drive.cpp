//
// Created by yuong on 23-7-31.
//

#include "can_drive.h"

bool CanDriver::Connect(const std::string& can_name)
{
    _fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (_fd < 0)
    {
        std::cout << "failed open CAN!" << std::endl;
        return false;
    }
    // 获取can0的接口索引
    strcpy(_ifr.ifr_name, can_name.c_str());
    if (ioctl(_fd, SIOCGIFINDEX, &_ifr) < 0)
    {
        std::cout << "failed CAN ioctl!" << std::endl;
        return false;
    }
    _addr.can_family = AF_CAN;
    // 把can0接口的索引给addr
    _addr.can_ifindex = _ifr.ifr_ifindex;
    // 把addr绑定到sicket上
    if (bind(_fd, (sockaddr*)&_addr, sizeof(_addr)) < 0)
    {
        std::cout << "failed CAN bind" << std::endl;
        return false;
    }

    return true;
}

bool CanDriver::Connect(const std::vector<std::string>& can_name)
{
    // init
    this->_can_name = can_name;
    _fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (_fd < 0)
    {
        std::cout << "failed open CAN!" << std::endl;
        return false;
    }
    _sock_can.resize(_can_name.size());
    for (int i = 0; i < _can_name.size(); ++i)
    {
        strcpy(_ifr.ifr_name, _can_name[i].c_str());
        if (ioctl(_fd, SIOCGIFINDEX, &_ifr) < 0)
        {
            std::cout << "failed CAN ioctl!" << i << std::endl;
            return false;
        }
        _sock_can[i].can_family = AF_CAN;
        _sock_can[i].can_ifindex = _ifr.ifr_ifindex;  //  指定can索引
    }
    _addr.can_family = AF_CAN;
    if (_can_name.size() == 1) { _addr.can_ifindex = _ifr.ifr_ifindex; }
    else { _addr.can_ifindex = 0; }

    if (bind(_fd, (sockaddr*)&_addr, sizeof(_addr)) < 0)
    {
        std::cout << "failed CAN bind" << std::endl;
        return false;
    }
}

bool CanDriver::Connected(short port, char* server_ip)
{
    sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));  // 初始化服务器地址

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "sockfd=" << sockfd << std::endl;

    if (sockfd < 0)
    {
        perror("socket error");
        exit(-1);
    }
    // 当连接成功或者失败的时候才会返回，
    if (connect(sockfd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cout << "CAN connect failed!" << std::endl;
        close(sockfd);
        return false;
    }
    else { std::cout << "CAN connect success!" << std::endl; }
    return true;
}

bool CanDriver::Read(void* data, uint32_t id)
{
    struct can_filter rfilter;
    rfilter.can_id = id;
    int count = 0;
    setsockopt(_fd, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

    while (true)
    {
        can_frame frame;
        socklen_t len = sizeof(_addr);
        //        std::cout<<"PAUSE"<<std::endl;
        recvfrom(_fd, &frame, sizeof(struct can_frame), 0, (struct sockaddr*)&_addr, &len);
        printf("Receive OK count=%d\n", count);

        // 打印接收到的报文数据
        //        std::cout << "ID=" << std::hex << frame.can_id << " DLC=" << std::hex << (int)frame.can_dlc;
        //        for (int i = 0; i < frame.can_dlc; ++i)
        //        {
        //            std::cout << " data[" << i << "]=" << std::hex << (int)frame.data[i];
        //        }
        //        std::cout << std::endl;
        //        count++;
    }
}

bool CanDriver::Read(void* data)
{
    can_frame frame;
    int nbytes = read(_fd, &frame, sizeof(frame));

    if (nbytes > 0)
    {
        std::cout << "ID=" << std::hex << frame.can_id << " DLC=" << std::hex << (int)frame.can_dlc;
        for (int i = 0; i < frame.can_dlc; ++i)
        {
            std::cout << " data[" << i << "]=" << std::hex << (int)frame.data[i];
        }
        std::cout << std::endl;
    }
    else
    {
        std::cout << "can read error" << std::endl;
        return false;
    }

    return true;
}

bool CanDriver::Read(const std::string& can_name, void* data, uint32_t id)
{
    if (!Connect(can_name))
    {
        std::cout << "failed connect" << std::endl;
        return false;
    }

    can_frame frame;
    can_filter rfilter;
    rfilter.can_id = id;
    //    rfilter.can_mask = CAN_SFF_MASK;
    setsockopt(_fd, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

    int nbytes = read(_fd, &frame, sizeof(frame));

    if (nbytes > 0)
    {
        memcpy(data, frame.data, sizeof(data));
        std::cout << "ID=" << std::hex << frame.can_id << " DLC=" << std::hex << (int)frame.can_dlc;
        for (int i = 0; i < frame.can_dlc; ++i)
        {
            std::cout << " data[" << i << "]=" << std::hex << (int)frame.data[i];
        }
        std::cout << std::endl;
    }
    else
    {
        std::cout << "can read error" << std::endl;
        return false;
    }
    return true;
}

bool CanDriver::Write(const can_frame& frame)
{
    _frame = frame;
    int nbytes;
    nbytes = write(_fd, &_frame, sizeof(_frame));
    if (nbytes != sizeof(_frame))
    {
        std::cout << "Send Error frame" << std::endl;
        return false;
    }
    return true;
}

#if 0
bool CanDriver::Write(const void* data, size_t bytes, uint32_t id)
{
    //    unsigned char* tmp_data = (unsigned char*)data;
    //    size_t length
    _frame.can_id = id;
    _frame.can_dlc = bytes;
    memcpy(_frame.data, data, bytes);
//    std::cout << "ID=" << std::hex << _frame.can_id << " DLC=" << std::hex << (int)_frame.can_dlc;
//    for (int i = 0; i < _frame.can_dlc; ++i)
//    {
//        std::cout << " data[" << i << "]=" << std::hex << (int)_frame.data[i];
//    }
//    std::cout << std::endl;
    int nbytes = write(_fd, &_frame, sizeof(_frame));
    if (nbytes != sizeof(_frame))
    {
        std::cout << "Send Error frame" << std::endl;
        return false;
    }

    return true;
}
#endif

bool CanDriver::Write(const void* data, size_t bytes, uint32_t id)
{
    _frame.can_id = id;
    _frame.can_dlc = bytes;
    memcpy(_frame.data, data, bytes);
    int nbytes;
    int count = 0;

    while (true)
    {
        printf("Send OK count=%d\n", count);
        for (int i = 0; i < _can_name.size(); ++i)
        {
            nbytes = sendto(_fd,
                &_frame,
                sizeof(struct can_frame),
                0,
                (struct sockaddr*)&_sock_can[i],
                sizeof(_sock_can[i]));
        }

        if (nbytes != sizeof(_frame))
        {
            std::cout << "Send Error frame" << std::endl;
            return false;
        }
        else
        {
            //            std::cout << "ID=" << std::hex << _frame.can_id << " DLC=" << std::hex << (int)_frame.can_dlc;
            //            for (int i = 0; i < _frame.can_dlc; ++i)
            //            {
            //                std::cout << " data[" << i << "]=" << std::hex << (int)_frame.data[i];
            //            }
            //            std::cout<<std::endl;
        }
        count++;
        sleep(0.01);
    }
    return true;
}

bool CanDriver::Write(const std::string& can_name, const void* data, size_t bytes, uint32_t id)
{
    _frame.can_id = id;
    _frame.can_dlc = bytes;
    memcpy(_frame.data, data, bytes);
    int nbytes;
    int count = 0;
    int index;

    std::vector<std::string>::iterator it = std::find(_can_name.begin(), _can_name.end(), can_name);
    index = std::distance(_can_name.begin(), it);
    while (true)
    {
        printf("Send OK count=%d\n", count);
        for (int i = 0; i < _can_name.size(); ++i)
        {
            nbytes = sendto(_fd,
                &_frame,
                sizeof(struct can_frame),
                0,
                (struct sockaddr*)&_sock_can[index],
                sizeof(_sock_can[index]));
        }

        if (nbytes != sizeof(_frame))
        {
            std::cout << "Send Error frame" << std::endl;
            return false;
        }
        else
        {
            //            std::cout << "ID=" << std::hex << _frame.can_id << " DLC=" << std::hex << (int)_frame.can_dlc;
            //            for (int i = 0; i < _frame.can_dlc; ++i)
            //            {
            //                std::cout << " data[" << i << "]=" << std::hex << (int)_frame.data[i];
            //            }
            //            std::cout<<std::endl;
        }
        count++;
        sleep(0.01);
    }
    return true;
}

void CanDriver::Close() { close(_fd); }