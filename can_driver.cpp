/** @file       can_driver.h
 *  @brief      Can driver.
 *  @author     Yong Yu
 *  @version    1.0
 *  @date       2023-08-08
 *  @copyright  Heli Co., Ltd. All rights reserved.
 */

#include <can_driver.h>


bool CanDriver::Connect(const std::string& can_name)
{
    _fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (_fd < 0)
    {
        std::cout << "failed open CAN!" << std::endl;
        return false;
    }

    strcpy(_ifr.ifr_name, can_name.c_str());

    if (ioctl(_fd, SIOCGIFINDEX, &_ifr) < 0)
    {
        std::cout << "failed CAN ioctl!" << std::endl;
        return false;
    }
    _addr.can_family = AF_CAN;
    _addr.can_ifindex = _ifr.ifr_ifindex;
    _connected = bind(_fd, (sockaddr*)&_addr, sizeof(_addr));
    if (_connected < 0)
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
    for (int i = 0; i < (int)_can_name.size(); ++i)
    {
        strcpy(_ifr.ifr_name, _can_name[i].c_str());
        if (ioctl(_fd, SIOCGIFINDEX, &_ifr) < 0)
        {
            std::cout << "failed CAN ioctl!" << i << std::endl;
            return false;
        }
        _sock_can[i].can_family = AF_CAN;
        _sock_can[i].can_ifindex = _ifr.ifr_ifindex;
    }
    _addr.can_family = AF_CAN;
    if (_can_name.size() == 1)
    {
        _addr.can_ifindex = _ifr.ifr_ifindex;
    }
    else
    {
        _addr.can_ifindex = 0;
    }

    _connected = bind(_fd, (sockaddr*)&_addr, sizeof(_addr));
    if (_connected < 0)
    {
        std::cout << "failed CAN bind" << std::endl;
        return false;
    }
    return true;
}

bool CanDriver::Receive(can_frame& frame, uint32_t id)
{
    //    setsockopt(_fd, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
    struct can_filter rfilter;
    rfilter.can_id = id;
    rfilter.can_mask = CAN_EFF_MASK;
    setsockopt(_fd, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

    socklen_t len = sizeof(_addr);
    recvfrom(_fd, &frame, sizeof(struct can_frame), 0, (struct sockaddr*)&_addr, &len);
    return true;
}

bool CanDriver::Read(can_frame& frame)
{
    int nbytes = read(_fd, &frame, sizeof(frame));

    if (nbytes < 0)
    {
        std::cout << "can read error" << std::endl;
        return false;
    }
    return true;
}

bool CanDriver::Read(can_frame& frame, uint32_t id)
{
    can_filter rfilter;
    rfilter.can_id = id;
    rfilter.can_mask = CAN_EFF_MASK;
    setsockopt(_fd, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));
    int nbytes = read(_fd, &frame, sizeof(frame));

    if (nbytes < 0)
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

bool CanDriver::Write(const void* data, size_t bytes, uint32_t id)
{
    _frame.can_id = id;
    _frame.can_dlc = bytes;
    memcpy(_frame.data, data, bytes);
    int nbytes;
    nbytes = write(_fd, &_frame, sizeof(_frame));

    if (nbytes != sizeof(_frame))
    {
        std::cout << "Send Error frame" << std::endl;
        return false;
    }
    return true;
}

bool CanDriver::Send(const std::string& can_name, const void* data, size_t bytes, uint32_t id)
{
    _frame.can_id = id;
    _frame.can_dlc = bytes;
    memcpy(_frame.data, data, bytes);
    int nbytes = -1;
    int index;
    std::vector<std::string>::iterator it = std::find(_can_name.begin(), _can_name.end(), can_name);
    index = std::distance(_can_name.begin(), it);

    for (int i = 0; i < (int)_can_name.size(); ++i)
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
    return true;
}

void CanDriver::Close()
{
    close(_fd);
}
