# ASOC-ROMA

#### 操作流程
1. 小车上电
2. 下位机
   1. 物理初始化-固定轮组与车体的位置
   2. 




#### 介绍
ASOC robot path tracking project


#### 软件架构
- 各package的内容：
- localization：调用了robot_localization包，在params文件夹下配置EKF
- path_load：发布固定规划路线
- wt931：IMU启动模块
- rt500：RTK模块（未完成）


#### 安装教程

1.  make的时候可能会缺少包，比如serial和nmea


#### 使用说明

monitor 
rostopic hz /Roll_low /Roll_high /velocity_low /velocity_high /sendI_low /sendI_high 



