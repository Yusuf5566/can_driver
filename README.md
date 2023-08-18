# can

CAN总线的读写封装

1. 在Linux下配置虚拟can口
sudo modprobe vcan
sudo ip link add dev can0 type vcan
sudo ip link set up can0
ifconfig –a

2.启动can0
candump can0
