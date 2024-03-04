/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>

#include "common.h"
#include "zbus_multicore.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(host, LOG_LEVEL_INF);

ZBUS_SUBSCRIBER_DEFINE(sensor_sub, 4);
ZBUS_CHAN_ADD_OBS(temperature_chan, sensor_sub, 4);
ZBUS_CHAN_ADD_OBS(humidity_chan, sensor_sub, 4);

int main(void)
{
	int ret = 0;

	LOG_INF("IPC-service HOST demo started");

	ret = init_zbus_multicore();
	if (ret) {
		LOG_ERR("Failed to initialize ZBus Multicore: %d", ret);
		return ret;
	}

	LOG_INF("ZBus Multicore sample started");

	while (true) {
		struct temperature_data sensor = {0};
		const struct zbus_channel *chan = NULL;
		zbus_sub_wait(&sensor_sub, &chan, K_FOREVER);
		if (&temperature_chan == chan) {
			ret = zbus_chan_read(&temperature_chan, &sensor, K_NO_WAIT);
			if (ret == 0) {
				LOG_INF("Received sensor data: temperature %f, ts %u",
					sensor.temperature, sensor.ts);
			}
		} else if (&humidity_chan == chan) {
			struct humidity_data hum = {0};
			ret = zbus_chan_read(&humidity_chan, &hum, K_NO_WAIT);
			if (ret == 0) {
				LOG_INF("Received sensor data: humidity %f, ts %u",
					hum.humidity, hum.ts);
			}
		}
	}
	return 0;
}
