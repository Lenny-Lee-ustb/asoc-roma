#include "include/tmotor_common.hpp"

ros::Publisher Tmotor_pos;
ros::Subscriber joy_sub;
ros::Subscriber Control_sub;

//判断是否开启手柄模式 xbox_mode_on>0: 开启；<0关闭
int xbox_mode_on = -1;
int xbox_power = 0;
int xbox_power_last = 0;

double K_S = 4.0;
double D_S = 0.3;
//电弹簧模式参数

std_msgs::Float32MultiArray tmotor_pos_msgs;
//电机位置信息

Tmotor tmotor[4];

//监测调零后电机状态
void flagTest2(int id)
{
	if (tmotor[id].flag == 5)
	{
		if (abs(tmotor[id].pos_zero - tmotor[id].pos_now) < 0.1)
		{
			tmotor[id].flag = 3;
		}
		else
		{
			tmotor[id].flag = 4;
		}
	}
}

//监测电机状态
void flagTest(int id)
{
	if (tmotor[id].flag == 0)
	{
		//如果电流增大超过阈值->碰到机械限位->设当前位置为绝对零点->确定相对零点->进入flag1
		if (abs(tmotor[id].t_now) > 8)
		{
			tmotor[id].pos_abszero = tmotor[id].pos_now;
			tmotor[id].pos_zero = tmotor[id].pos_des = tmotor[id].pos_abszero - 2.5;
			tmotor[id].flag = 1;
		}
	}

	//快速调相对零点；接近时进入flag2
	if (tmotor[id].flag == 1)
	{
		if (abs(tmotor[id].pos_zero - tmotor[id].pos_now) < 0.5)
		{
			tmotor[id].flag = 2;
		}
	}
	//慢速调相对零点；十分接近时进入flag3；调零完毕，可以进入手柄控制模式
	if (tmotor[id].flag == 2)
	{
		if (abs(tmotor[id].pos_zero - tmotor[id].pos_now) < 0.1)
		{
			tmotor[id].zeroPointSet = 1;
			tmotor[id].flag = 3;
		}
	}

	//电弹簧模式工作状态，接近零点启用flag3，远离零点启用flag4（常规电弹簧模式），flag3的目的是避免零点附近的电机震荡
	if ((tmotor[id].flag == 4) && (abs(tmotor[id].pos_now - tmotor[id].pos_zero) < 0.15))
	{
		tmotor[id].flag = 3;
	}
	else if ((tmotor[id].flag == 3) && (abs(tmotor[id].pos_now - tmotor[id].pos_zero) >= 0.15))
	{
		tmotor[id].flag = 4;
	}
}

// upper_controller_callback 由上位机修正tmotor的零点位置
void ControlCallback(const std_msgs::Float32MultiArray &ctrl_cmd)
{
	for (int id = 0; id < 4; id++)
	{
		// tmotor[id].pos_des = ctrl_cmd.data[id*3];
		// tmotor[id].vel_des = ctrl_cmd.data[id*3+1];
		// tmotor[id].t_des   = ctrl_cmd.data[id*3+2];
		tmotor[id].pos_zero = ctrl_cmd.data[id];
	}
}

//joy按键回调函数
void buttonCallback(const sensor_msgs::Joy::ConstPtr &joy)
{
	//只有四个电机都调零完毕才能手柄控制，flag5为手柄控制模式
	if ((tmotor[0].zeroPointSet == 1) && (tmotor[1].zeroPointSet == 1) && (tmotor[2].zeroPointSet == 1) && (tmotor[3].zeroPointSet == 1))
	{
		xbox_power = joy->buttons[7];
		float move_up = -(joy->axes[2]) + 1;
		float move_down = -(joy->axes[5]) + 1;

		if (xbox_power > xbox_power_last)
		{
			xbox_mode_on = -xbox_mode_on;
		}
		xbox_power_last = xbox_power;

		if (move_up != 0)
		{
			for (int id = 0; id < 4; id++)
			{
				tmotor[id].flag = 5;
				tmotor[id].vel_des = 4 * move_up;
				tmotor[id].pos_des = 0;
				tmotor[id].t_des = 0;
				tmotor[id].kd = 2;
				tmotor[id].kp = 0;
			}
		}

		if (move_down != 0)
		{
			for (int id = 0; id < 4; id++)
			{
				tmotor[id].flag = 5;
				tmotor[id].vel_des = -(4 * move_down);
				tmotor[id].pos_des = 0;
				tmotor[id].t_des = 0;
				tmotor[id].kd = 2;
				tmotor[id].kp = 0;
			}
		}

		if (joy->buttons[3] == 1)
		{
			tmotor[0].flag = 5;
			tmotor[0].vel_des = 6;
			tmotor[0].pos_des = 0;
			tmotor[0].t_des = 0;
			tmotor[0].kd = 5;
			tmotor[0].kp = 0;
		}

		if (joy->buttons[2] == 1)
		{
			tmotor[1].flag = 5;
			tmotor[1].vel_des = 6;
			tmotor[1].pos_des = 0;
			tmotor[1].t_des = 0;
			tmotor[1].kd = 5;
			tmotor[1].kp = 0;
		}

		if (joy->buttons[1] == 1)
		{
			tmotor[3].flag = 5;
			tmotor[3].vel_des = 6;
			tmotor[3].pos_des = 0;
			tmotor[3].t_des = 0;
			tmotor[3].kd = 5;
			tmotor[3].kp = 0;
		}

		if (joy->buttons[0] == 1)
		{
			tmotor[2].flag = 5;
			tmotor[2].vel_des = 6;
			tmotor[2].pos_des = 0;
			tmotor[2].t_des = 0;
			tmotor[2].kd = 5;
			tmotor[2].kp = 0;
		}

		if (joy->axes[7] == 1)
		{
			tmotor[0].flag = 5;
			tmotor[0].vel_des = -4;
			tmotor[0].pos_des = 0;
			tmotor[0].t_des = 0;
			tmotor[0].kd = 5;
			tmotor[0].kp = 0;
		}

		if (joy->axes[6] == 1)
		{
			tmotor[1].flag = 5;
			tmotor[1].vel_des = -4;
			tmotor[1].pos_des = 0;
			tmotor[1].t_des = 0;
			tmotor[1].kd = 5;
			tmotor[1].kp = 0;
		}

		if (joy->axes[7] == -1)
		{
			tmotor[2].flag = 5;
			tmotor[2].vel_des = -4;
			tmotor[2].pos_des = 0;
			tmotor[2].t_des = 0;
			tmotor[2].kd = 5;
			tmotor[2].kp = 0;
		}

		if (joy->axes[6] == -1)
		{
			tmotor[3].flag = 5;
			tmotor[3].vel_des = -4;
			tmotor[3].pos_des = 0;
			tmotor[3].t_des = 0;
			tmotor[3].kd = 5;
			tmotor[3].kp = 0;
		}

		if ((move_up == 0) && (move_down == 0) && (joy->buttons[0] == 0) && (joy->buttons[1] == 0) && (joy->buttons[2] == 0) && (joy->buttons[3] == 0) && (joy->axes[6] == 0) && (joy->axes[7] == 0))
		{

			if (xbox_mode_on > 0)
			{
				for (int id = 0; id < 4; id++)
				{
					tmotor[id].pos_des = tmotor[id].pos_now;
					tmotor[id].vel_des = 0;
					tmotor[id].t_des = 0;
					tmotor[id].kp = 20;
					tmotor[id].kd = 0;
				}
			}
			else
			{
				for (int id = 0; id < 4; id++)
				{
					flagTest2(id);
				}
			}
		}
	}
}

//收报函数
void rxThread(int s)
{
	int i;
	struct can_frame frame;
	int nbytes;
	for (i = 0;; i++)
	{
		ros::spinOnce();
		nbytes = read(s, &frame, sizeof(struct can_frame));
		if (nbytes < 0)
		{
			perror("Read");
			break;
		}
		uint16_t motorID, pos, vel, t;
		float f_pos, f_vel, f_t;
		int ID;

		motorID = frame.data[0];
		ID = int(motorID - 0x00) - 1;
		pos = ((uint16_t)frame.data[1] << 8) | frame.data[2];
		vel = ((uint16_t)frame.data[3] << 4) | (frame.data[4] >> 4);
		t = ((uint16_t)(frame.data[4] & 0xf) << 8) | frame.data[5];
		rxCounter++;

		f_pos = uint_to_float(pos, P_MIN, P_MAX, 16);
		f_vel = uint_to_float(vel, V_MIN, V_MAX, 12);
		f_t = uint_to_float(t, T_MIN, T_MAX, 12);

		tmotor[ID].pos_now = f_pos;
		tmotor[ID].vel_now = f_vel;
		tmotor[ID].t_now = f_t;

		std::this_thread::sleep_for(std::chrono::nanoseconds(1000000));
	}
}

//设置tmotor各个flag对应的电机参数
void motorParaSet(int id)
{
	switch (tmotor[id].flag)
	{
	case 0:
		tmotor[id].t_des = 0;
		tmotor[id].vel_des = 2;
		tmotor[id].pos_des = 0;
		tmotor[id].kp = 0;
		tmotor[id].kd = 4;
		break;
	case 1:
		tmotor[id].t_des = 0;
		tmotor[id].vel_des = -2.5;
		tmotor[id].pos_des = 0;
		tmotor[id].kp = 0;
		tmotor[id].kd = 5;
		break;
	case 2:
		tmotor[id].t_des = 0;
		tmotor[id].vel_des = -0.4;
		tmotor[id].pos_des = tmotor[id].pos_zero;
		tmotor[id].kp = 0;
		tmotor[id].kd = 5;
		break;
	case 3:
		tmotor[id].t_des = 0;
		tmotor[id].vel_des = 0;
		tmotor[id].pos_des = tmotor[id].pos_zero;
		tmotor[id].kp = 5;
		tmotor[id].kd = 0;
		break;
	case 4:

		//F=kx+电机自身阻尼补偿（0.6）+速度阻尼
		if (tmotor[id].pos_now - tmotor[id].pos_zero > 0)
		{
			if (tmotor[id].vel_now > 0)
			{
				tmotor[id].t_des = -(tmotor[id].pos_now - tmotor[id].pos_zero) * K_S - 0.6 - D_S * abs(tmotor[id].vel_now);
			}
			else
			{
				tmotor[id].t_des = -(tmotor[id].pos_now - tmotor[id].pos_zero) * K_S - 0.6 + D_S * abs(tmotor[id].vel_now);
			}
		}
		else
		{
			if (tmotor[id].vel_now > 0)
			{
				tmotor[id].t_des = -(tmotor[id].pos_now - tmotor[id].pos_zero) * K_S + 0.6 - D_S * abs(tmotor[id].vel_now);
			}
			else
			{
				tmotor[id].t_des = -(tmotor[id].pos_now - tmotor[id].pos_zero) * K_S + 0.6 + D_S * abs(tmotor[id].vel_now);
			}
		}

		tmotor[id].pos_des = 0;
		tmotor[id].vel_des = 0;
		tmotor[id].kd = 0;
		tmotor[id].kp = 0;
		break;

	default:
		break;
	}
}

//init CAN_Frame member data
void frameDataSet(struct can_frame &frame, int id)
{
	float f_p, f_v, f_kp, f_kd, f_t;
	uint16_t p, v, kp, kd, t;

	//如果电机flag不为5，表示电机由上位机而非手柄控制，给定电机参数
	if (tmotor[id].flag != 5)
	{
		flagTest(id);
		motorParaSet(id);
	}

	f_p = tmotor[id].pos_des;
	f_v = tmotor[id].vel_des;
	f_t = tmotor[id].t_des;
	f_kp = tmotor[id].kp;
	f_kd = tmotor[id].kd;

	//参考AK80-6电机使用手册，将各参数的浮点型转化为整型后保存在data中
	p = float_to_uint(f_p, P_MIN, P_MAX, 16);
	v = float_to_uint(f_v, V_MIN, V_MAX, 12);
	kp = float_to_uint(f_kp, KP_MIN, KP_MAX, 12);
	kd = float_to_uint(f_kd, KD_MIN, KD_MAX, 12);
	t = float_to_uint(f_t, T_MIN, T_MAX, 12);
	frame.data[0] = p >> 8;
	frame.data[1] = p & 0xFF;
	frame.data[2] = v >> 4;
	frame.data[3] = ((v & 0xF) << 4) | (kp >> 8);
	frame.data[4] = kp & 0xFF;
	frame.data[5] = kd >> 4;
	frame.data[6] = ((kd & 0xF) << 4) | (t >> 8);
	frame.data[7] = t & 0xff;
}

//打印信息
void printTmotorInfo(int id)
{
	ROS_INFO("\n-----\nID[%d]\nflag[%d] \npos_now is %.2f\npos_des is %.2f \nt_now is %.2f \npos_zero is %.2f \nxbox_mode: %d\nstop_flag:%d\n",
			 tmotor[id].id,
			 tmotor[id].flag,
			 tmotor[id].pos_now,
			 tmotor[id].pos_des,
			 tmotor[id].t_now,
			 tmotor[id].pos_zero,
			 xbox_mode_on,
			 Stop_flag);
}

//发报函数
void txThread(int s)
{
	struct can_frame frame;
	frame.can_id = 0x01;
	frame.can_dlc = 8;

	int nbytes;

	for (int i = 0;; i++)
	{
		tmotor_pos_msgs.data.resize(4);
		for (int id = 0; id < 4; id++)
		{
			frame.can_id = 0x00 + id + 1;
			frameDataSet(frame, id);
			if (Stop_flag == 1)
			{
				for (int j = 0; j < 8; j++)
				{
					frame.data[j] = 0xff;
				}
				frame.data[7] = 0xfd; // exit T-motor control mode!
			}

			nbytes = write(s, &frame, sizeof(struct can_frame));
			if (nbytes == -1)
			{
				printf("send error\n");
				printf("please check battary!!\n");
				exit(1);
			}
			txCounter++;
			//printf("tx is %d;",txCounter);
			printTmotorInfo(id);

			tmotor_pos_msgs.data[id] = tmotor[id].pos_now;

			std::this_thread::sleep_for(std::chrono::nanoseconds(1000000));
		}
		Tmotor_pos.publish(tmotor_pos_msgs);
	}
}

int main(int argc, char **argv)
{

	ros::init(argc, argv, "tmotorTest2");
	ros::NodeHandle n;
	ros::Rate loop_rate(10);
	signal(SIGINT, signalCallback);

	int s;
	struct sockaddr_can addr;
	struct ifreq ifr;
	const char *ifname = "can2";

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
	{
		perror("Error while opening socket");
		return -1;
	}

	strcpy(ifr.ifr_name, ifname);
	ioctl(s, SIOCGIFINDEX, &ifr);

	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	//printf("%s at index %d\n", ifr.ifr_name, ifr.ifr_ifindex);
	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("Error in socket bind");
		return -2;
	}
	//can连接完毕

	for (int i = 0; i < 4; i++)
	{
		tmotor[i].id = i;
	}
	//设置对应tmotor的id

	struct can_frame frame;
	for (int id = 1; id < 5; id++)
	{
		canCheck(frame, s, id);
	}
	//检查can通讯连接

	joy_sub = n.subscribe<sensor_msgs::Joy>("joy", 10, buttonCallback);
	Control_sub = n.subscribe("suspension_cmd", 10, ControlCallback);
	Tmotor_pos = n.advertise<std_msgs::Float32MultiArray>("Tmotor_pos", 100);
	//发布及订阅节点

	std::thread canTx(txThread, s);
	sleep(0.1);
	std::thread canRx(rxThread, s);
	//开启收报/发报线程

	while (ros::ok())
	{
		ros::spinOnce();
		//此函数必须开，否则无法启用回调函数
		loop_rate.sleep();
	}

	return 0;
}
