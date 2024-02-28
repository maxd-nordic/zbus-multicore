/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
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
struct data_packet {
	uint32_t channel_id;
	union data
	{
		struct sensor_data sensor;
		struct some_other_data other;
	} data;
};


/* init ipc connection and get ready for recv/send */
int init_ipc(void);

/* send a message */
int ipc_send(struct data_packet *msg);

/* channel for sensor data sent from remote to host */
ZBUS_CHAN_DECLARE(sensor_chan);


#endif /* __COMMON_H__ */
