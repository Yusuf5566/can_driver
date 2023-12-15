/** @file       can_driver.h
 *  @brief      Can driver.
 *  @author     Yong Yu
 *  @version    1.0
 *  @date       2023-08-08
 *  @copyright  Heli Co., Ltd. All rights reserved.
 */

#ifndef __LX_COMMON_CAN_DRIVER_H__
#define __LX_COMMON_CAN_DRIVER_H__

#include <linux/can/raw.h>
#include <net/if.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <vector>

class CanDriver
{
public:
    // single can interface
    bool Connect(const std::string& can_name);
    // multiple can interface
    bool Connect(const std::vector<std::string>& can_name);
    bool Connected() { return _connected == 0; }
    // single can interface
    bool Read(can_frame& frame);
    bool Read(can_frame& frame, uint32_t id);
    // multiple can interface
    bool Receive(can_frame& frame, uint32_t id);
    // single can interface
    bool Write(const can_frame& frame);
    bool Write(const void* data, size_t bytes, uint32_t id);
    // multiple can interface
    bool Send(const std::string& can_name, const void* data, size_t bytes, uint32_t id);
    void Close();

private:
    int _fd;
    ifreq _ifr;
    sockaddr_can _addr;
    can_frame _frame;
    std::vector<std::string> _can_name;
    std::vector<sockaddr_can> _sock_can;
    int _connected = -1;
};

#endif  // __LX_COMMON_CAN_DRIVER_H__
