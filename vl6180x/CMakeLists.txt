# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(hello_world)

target_sources(app PRIVATE src/main.c)

target_sources(app PRIVATE
    src/vl6180x/vl6180x_api.c
    src/vl6180x/vl6180x_i2c.c
)

zephyr_library_include_directories(src/vl6180x)
