#include <thread>
#include "can_driver.h"

void Receive()
{
    //    //     receive
    std::vector<std::string> can_name = {"can0", "can1"};
    //    //        can_name.push_back("can1");
    //    //        CanDriver can1;
    //    //        char *data[8]={};
    //    //
    //    //        can1.Init(can_name);
    //    //        can1.CanRead(data,0x581);
    //    //        can1.CanRead();
    //
    char data[8];
    CanDriver can2;
    can2.Connect(can_name);

}

int main()
{
#if 0
    std::thread t1(Receive);
    //    t1.detach();
    char data[8] = {0x40, 0x01, 0x10, 0x25, 0x12, 0x66, 0x77, 0x19};
    std::vector<std::string> can_name1 = {"can0", "can1"};
    sleep(1);
    //    t1.join();
    CanDriver can1;
    can1.Connect(can_name1);
    can1.Write(data, sizeof(data), 0x581);
    can1.Write(data, sizeof(data), 0x220);
#endif

#if 1
    std::thread t1(Receive);
    //    t1.detach();
    char data[8] = {0x40, 0x01, 0x10, 0x25, 0x12, 0x66, 0x77, 0x19};
    CanDriver can0;
    can0.Connect("can0");
    can0.Write(data, sizeof(data), 0x220);
    can0.Write(data, sizeof(data), 0x240);

#endif
    return 0;
}