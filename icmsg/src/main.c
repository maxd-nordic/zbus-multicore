/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>

#include "common.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(host, LOG_LEVEL_INF);

int main(void)
{
	int ret = 0;

	LOG_INF("IPC-service HOST demo started");

	ret = init_ipc();
	if (ret) {
		return ret;
	}

	LOG_INF("ZBus Multicore sample started");
	return 0;
}
