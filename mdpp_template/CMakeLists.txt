# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(mdpp_template)

target_sources(app PRIVATE src/main.c)

# Custom files and folders

target_sources(app PRIVATE
    src/remote_service/remote.c
)

zephyr_library_include_directories(src/remote_service)

target_sources(app PRIVATE
    src/adxl345_i2c/adxl345.c
)

zephyr_library_include_directories(src/adxl345_i2c)