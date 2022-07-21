cmake_minimum_required(VERSION 3.15)
project(livox_sdk)

# C++ config.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set position independent code.
# This enables support for using the livox SDK in a ROS nodelet.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Default to a release build.
set(CMAKE_BUILD_TYPE Release)

# Include threads.
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# Include the catkin package.
find_package(catkin REQUIRED)

# Define the catkin package for this project.
catkin_package(
	LIBRARIES ${PROJECT_NAME}
	INCLUDE_DIRS sdk_core/include
)

# Sources.
set(LIVOX_SOURCES
    sdk_core/src/third_party/FastCRC/FastCRC_tables.hpp
    sdk_core/src/third_party/FastCRC/FastCRCsw.cpp
    sdk_core/src/base/io_loop.h
    sdk_core/src/base/io_loop.cpp
    sdk_core/src/base/thread_base.h
    sdk_core/src/base/thread_base.cpp
    sdk_core/src/base/io_thread.h
    sdk_core/src/base/io_thread.cpp
    sdk_core/src/base/network/network_util.h
    sdk_core/src/base/logging.h
    sdk_core/src/base/logging.cpp
    sdk_core/src/base/noncopyable.h
    sdk_core/src/livox_sdk.cpp
    sdk_core/src/device_discovery.h
    sdk_core/src/device_discovery.cpp
    sdk_core/src/device_manager.h
    sdk_core/src/device_manager.cpp
    sdk_core/src/comm/comm_port.cpp
    sdk_core/src/comm/sdk_protocol.h
    sdk_core/src/comm/sdk_protocol.cpp
    sdk_core/src/data_handler/data_handler.h
    sdk_core/src/data_handler/data_handler.cpp
    sdk_core/src/data_handler/hub_data_handler.h
    sdk_core/src/data_handler/hub_data_handler.cpp
    sdk_core/src/data_handler/lidar_data_handler.h
    sdk_core/src/data_handler/lidar_data_handler.cpp
    sdk_core/src/command_handler/command_handler.h
    sdk_core/src/command_handler/command_handler.cpp
    sdk_core/src/command_handler/command_channel.h
    sdk_core/src/command_handler/command_channel.cpp
    sdk_core/src/command_handler/hub_command_handler.h
    sdk_core/src/command_handler/hub_command_handler.cpp
    sdk_core/src/command_handler/lidar_command_handler.h
    sdk_core/src/command_handler/lidar_command_handler.cpp
    sdk_core/src/command_handler/command_impl.cpp
    sdk_core/src/command_handler/command_impl.h
    sdk_core/src/base/multiple_io/multiple_io_base.h
    sdk_core/src/base/multiple_io/multiple_io_base.cpp
    sdk_core/src/base/multiple_io/multiple_io_factory.h
    sdk_core/src/base/multiple_io/multiple_io_epoll.h
    sdk_core/src/base/multiple_io/multiple_io_epoll.cpp
    sdk_core/src/base/multiple_io/multiple_io_poll.h
    sdk_core/src/base/multiple_io/multiple_io_poll.cpp
    sdk_core/src/base/multiple_io/multiple_io_select.h
    sdk_core/src/base/multiple_io/multiple_io_select.cpp
    sdk_core/src/base/multiple_io/multiple_io_kqueue.h
    sdk_core/src/base/multiple_io/multiple_io_kqueue.cpp
    sdk_core/src/base/wake_up/wake_up_pipe.h
)
if(WIN32)
	list(APPEND LIVOX_SOURCES sdk_core/src/base/network/win/network_util.cpp)
	list(APPEND LIVOX_SOURCES sdk_core/src/base/wake_up/win/wake_up_pipe.cpp)
else(WIN32)
	list(APPEND LIVOX_SOURCES sdk_core/src/base/network/unix/network_util.cpp)
	list(APPEND LIVOX_SOURCES sdk_core/src/base/wake_up/unix/wake_up_pipe.cpp)
endif (WIN32)

# Includes.
set(LIVOX_PRIVATE_INCLUDES
    sdk_core/include
    sdk_core/include/third_party/FastCRC
    sdk_core/include/third_party/spdlog
    sdk_core/include/third_party/cmdline
    sdk_core/src)

# Add the Livox SDK library.
add_library(${PROJECT_NAME} ${LIVOX_SOURCES})
target_link_libraries(${PROJECT_NAME} Threads::Threads)
target_include_directories(${PROJECT_NAME}
    PRIVATE ${LIVOX_PRIVATE_INCLUDES})

# Include headers.
include_directories(sdk_core/include)

# Install the catkin package.
install(TARGETS
	${PROJECT_NAME}
    ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})

## Install headers
install(FILES
	sdk_core/include/livox_def.h
	sdk_core/include/livox_sdk.h
	DESTINATION ${CATKIN_GLOBAL_INCLUDE_DESTINATION})
