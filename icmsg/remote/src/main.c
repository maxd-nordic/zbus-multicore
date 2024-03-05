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
LOG_MODULE_REGISTER(remote, LOG_LEVEL_INF);

int main(void)
{
	int ret = 0;

	LOG_INF("IPC-service REMOTE demo started");

	ret = init_zbus_multicore();
	if (ret) {
		LOG_ERR("Failed to initialize ZBus Multicore: %d", ret);
		return ret;
	}

	LOG_INF("ZBus Multicore sample started");

	while (true) {
		struct temperature_data temp = {
			.temperature = 23.0,
			.ts = k_uptime_get_32(),
		};
		LOG_INF("Sending sensor data: temperature %f, ts %u",
			temp.temperature, temp.ts);
		zbus_chan_pub(&temperature_chan, &temp, K_NO_WAIT);

		struct humidity_data hum = {
			.humidity = 50.0,
			.ts = k_uptime_get_32(),
		};
		LOG_INF("Sending sensor data: humidity %f, ts %u",
			hum.humidity, hum.ts);
		zbus_chan_pub(&humidity_chan, &hum, K_NO_WAIT);
		k_sleep(K_SECONDS(1));

		struct led_data led = {0};
		zbus_chan_read(&led_chan, &led, K_FOREVER);
		LOG_INF("Received led data: led %d, ts %u", led.led, led.ts);
	}
	return 0;
}
