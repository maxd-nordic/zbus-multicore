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

ZBUS_SUBSCRIBER_DEFINE(sensor_sub, 4);
ZBUS_CHAN_ADD_OBS(sensor_chan, sensor_sub, 4);

int main(void)
{
	int ret = 0;

	LOG_INF("IPC-service HOST demo started");

	ret = init_ipc();
	if (ret) {
		return ret;
	}

	LOG_INF("ZBus Multicore sample started");

	while (true) {
		struct sensor_data sensor = {0};
		const struct zbus_channel *chan = NULL;
		zbus_sub_wait(&sensor_sub, &chan, K_FOREVER);
		if (&sensor_chan == chan) {
			ret = zbus_chan_read(&sensor_chan, &sensor, K_NO_WAIT);
			if (ret == 0) {
				LOG_INF("Received sensor data: temperature %f, ts %u",
					sensor.temperature, sensor.ts);
			}
		}
	}
	return 0;
}
