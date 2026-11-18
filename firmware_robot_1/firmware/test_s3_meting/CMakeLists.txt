cmake_minimum_required(VERSION 3.16)
# Gebruikt dezelfde componenten als de robotfirmware (Mpu6050, MotorDriver,
# BalansFilter, Ontwerpparameters), zodat we precies de echte code meten.
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../esp32s3/components")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(test_s3_meting)
