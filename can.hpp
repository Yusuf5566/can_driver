//
// Created by yuong on 23-7-26.
//
#include <iostream>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

int CanRead()
{
    int s, nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    struct can_filter rfilter[1];
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);  // 创建套接字s
    std::cout<<"s="<<s<<std::endl;
    strcpy(ifr.ifr_name, "can0");
    ioctl(s, SIOCGIFINDEX, &ifr);           // 指定 can0 设备
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(s, (struct sockaddr *)&addr, sizeof(addr));  // 将套接字s与 can0 绑定
                                                      // 定义接收规则，只接收表示符等于 0x11 的报文
    rfilter[0].can_id = 0x11;
    rfilter[0].can_mask = CAN_SFF_MASK;
    // 设置过滤规则
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));
    // 循环接收报文
    while (1)
    {
        // param1表示输入输出类型；param2读取的参数；param3读取的参数的长度
        nbytes = read(s, &frame, sizeof(frame));  // 接收报文
                                                  // 显示报文
        if (nbytes > 0)
        {
            std::cout<<"ID ="<<frame.can_id
                      <<" DLC="<<frame.can_dlc
                      <<" data[0]"<<frame.data[0]<<std::endl;
        }
    }
    close(s);
    return 0;
}

int CanWrite()
{
    CanRead();
    int s, nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame[2] = {{0}};
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);  // 创建套接字
    std::cout<<"s="<<s<<std::endl;
    strcpy(ifr.ifr_name, "can0");
    ioctl(s, SIOCGIFINDEX, &ifr);           // 指定 can0 设备
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(s, (struct sockaddr *)&addr, sizeof(addr));  // 将套接字与 can0 绑定
    // 禁用过滤规则，本进程不接收报文，只负责发送
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
    // 生成两个报文
    frame[0].can_id = 0x11;
    frame[0].can_dlc = 1;
    frame[0].data[0] = 'Y';
    frame[1].can_id = 0x22;
    frame[1].can_dlc = 1;
    frame[1].data[0] = 'N';
    std::cout<<"hello!"<<std::endl;
    // 循环发送两个报文
    while (1)
    {
        nbytes = write(s, &frame[0], sizeof(frame[0]));  // 发送 frame[0]
        std::cout<<"nbytes="<<nbytes<<std::endl;
        if (nbytes != sizeof(frame[0]))
        {
            std::cout<<"Send Error frame[0]"<<std::endl;
            break;  // 发送错误，退出
        }
        sleep(1);
        nbytes = write(s, &frame[1], sizeof(frame[1]));  // 发送 frame[1]
        if (nbytes != sizeof(frame[0]))
        {
            std::cout<<"Send Error frame[1]"<<std::endl;
            break;
        }
        sleep(1);
    }
    close(s);
    return 0;
}
