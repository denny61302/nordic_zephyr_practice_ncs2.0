# SPDX-License-Identifier: Apache-2.0

# For the sake of demonstration, we add the driver directory as a zephyr module
# by hand. If your driver is a project that's managed by west, you can remove this line.
cmake_minimum_required(VERSION 3.20.0)

list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/my_oot_drivers)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(out_of_tree_driver)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/my_oot_drivers/drivers/my_uart_peripheral/)

FILE(GLOB app_sources src/*.c)
FILE(GLOB my_drv my_oot_drivers/drivers/my_uart_peripheral/*.c)

target_sources(app PRIVATE ${my_drv})
target_sources(app PRIVATE ${app_sources})