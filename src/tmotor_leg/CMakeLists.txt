cmake_minimum_required(VERSION 3.0.2)
project(tmotor_leg)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES tmotortest
#  CATKIN_DEPENDS roscpp rospy std_msgs
#  DEPENDS system_lib
)


include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)

add_executable(tmotortest 
src/tmotor_single_test.cpp
src/include/tmotor_common.hpp)
target_link_libraries(tmotortest ${catkin_LIBRARIES})

add_executable(tmotor_controller 
src/tmotor_controller.cpp
src/include/tmotor_common.hpp)
target_link_libraries(tmotor_controller ${catkin_LIBRARIES})

add_executable(tmotor_controller2 
src/tmotor_controller2.cpp
src/include/tmotor_common.hpp)
target_link_libraries(tmotor_controller2 ${catkin_LIBRARIES})

add_executable(tmotor_controller3 
src/tmotor_controller3.cpp
src/include/tmotor_common.hpp)
target_link_libraries(tmotor_controller3 ${catkin_LIBRARIES})


add_executable(tmotor_auto 
src/tmotor_auto.cpp
src/include/tmotor_common.hpp)
target_link_libraries(tmotor_auto ${catkin_LIBRARIES})

add_executable(tmotor_joy
src/tmotor_joy.cpp
src/include/tmotor_common.hpp)
target_link_libraries(tmotor_joy ${catkin_LIBRARIES})

add_executable(tmotor2_xboxtest1
src/tmotor2_xboxtest1.cpp
src/include/tmotor_common.hpp)
target_link_libraries(tmotor2_xboxtest1 ${catkin_LIBRARIES})

add_executable(tmotor2_singletest1
src/tmotor2_singletest1.cpp
src/include/tmotor_common.hpp)
target_link_libraries(tmotor2_singletest1 ${catkin_LIBRARIES})

add_executable(tmotor_controller_uppercontroller
src/tmotor_controller_uppercontroller.cpp
src/include/tmotor_common.hpp)
target_link_libraries(tmotor_controller_uppercontroller ${catkin_LIBRARIES})

add_executable(tmotor_controller_xbox
src/tmotor_controller_xbox.cpp
src/include/tmotor_common.hpp)
target_link_libraries(tmotor_controller_xbox ${catkin_LIBRARIES})

add_executable(tmotor2_uppercontroller1
src/tmotor2_uppercontroller1.cpp
src/include/tmotor_common.hpp)
target_link_libraries(tmotor2_uppercontroller1 ${catkin_LIBRARIES})

add_executable(tmotor3_singletest
src/tmotor3_singletest.cpp
src/include/tmotor_common.hpp)
target_link_libraries(tmotor3_singletest ${catkin_LIBRARIES})

add_executable(tmotor_imu
src/tmotor_imu.cpp
src/include/tmotor_common.hpp)
target_link_libraries(tmotor_imu ${catkin_LIBRARIES})

add_executable(tmotor_vel_pos
src/tmotor_vel_pos.cpp
src/include/tmotor_common.hpp)
target_link_libraries(tmotor_vel_pos ${catkin_LIBRARIES})

add_executable(tmotor_vel_pos3
src/tmotor_vel_pos3.cpp
src/include/tmotor_common2.hpp)
target_link_libraries(tmotor_vel_pos3 ${catkin_LIBRARIES})