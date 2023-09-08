//
// Created by yuong on 23-7-31.
//

#ifndef EXAMPLE_CAN_DRIVE_H
#define EXAMPLE_CAN_DRIVE_H
#include <arpa/inet.h>
#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>

class CanDriver
{
public:
    bool Connect(const std::string& can_name);
    bool Connect(const std::vector<std::string>& can_name);
    bool Connected(short port, char *server_ip);
    bool Read(void* data);
    bool Read(void *data, uint32_t id);
    bool Read(const std::string& can_name, void* data, uint32_t id );
//    bool RecvCan(uint32_t id);
//    bool SendCan(const void *data, size_t bytes, uint32_t id,std::string can_name);
    bool Write(const can_frame& frame);
    bool Write(const void* data, size_t bytes, uint32_t id);
    bool Write(const std::string& can_name, const void* data, size_t bytes, uint32_t id);
    void Close();

private:
    int _fd;
    ifreq _ifr;
    sockaddr_can _addr;
    can_frame _frame;
    std::vector<std::string> _can_name;
    std::vector<sockaddr_can> _sock_can;
};

#endif  // EXAMPLE_CAN_DRIVE_H
