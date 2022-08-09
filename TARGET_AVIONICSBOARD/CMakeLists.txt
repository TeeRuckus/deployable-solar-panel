# Copyright (c) 2022 Curtin University All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause

add_library(mbed-avionicsboard INTERFACE)

target_sources(mbed-avionicsboard
    INTERFACE
        PeripheralPins.c
)

target_include_directories(mbed-avionicsboard
    INTERFACE
        .
)

target_link_libraries(mbed-avionicsboard INTERFACE mbed-stm32l562xe)
