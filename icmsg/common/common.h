/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#define PACKET_SIZE_START	(40)
#define DATA_SIZE		(100)
#define SENDING_TIME_MS		(1000)

#include <zephyr/zbus/zbus.h>

struct sensor_data {
	float temperature;
	uint32_t ts;
};

struct some_other_data {
	uint32_t some_data;
	uint32_t ts;
};

/* channel for sensor data sent from remote to host */
ZBUS_CHAN_DECLARE(sensor_chan);


#endif /* __COMMON_H__ */
