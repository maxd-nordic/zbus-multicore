#
# Copyright (c) 2024 Nordic Semiconductor
#
# SPDX-License-Identifier: Apache-2.0
#

zephyr_include_directories(.)
zephyr_sources(common.c)
zephyr_linker_sources(SECTIONS iterables.ld)
