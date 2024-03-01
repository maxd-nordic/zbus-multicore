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
struct data_packet_header {
	uint32_t channel_id;
};

struct zbus_multicore_tx {
	const struct zbus_channel *chan;
	uint32_t channel_id;
};

struct zbus_multicore_rx {
	const struct zbus_channel *chan;
	uint32_t channel_id;
};

struct zbus_multicore_channel {
	const struct zbus_channel *chan;
	uint32_t channel_id;
};

#define ZBUS_MULTICORE_CHANNEL_ADD(_chan)	\
	STRUCT_SECTION_ITERABLE(zbus_multicore_channel, _zbus_multicore_chan_##_chan) = { \
		.chan = &_chan, \
		.channel_id = UINT32_MAX, \
	};


#define ZBUS_MULTICORE_FORWARDER_ADD(_chan, _source, _dest) \
	COND_CODE_1( \
		IS_ENABLED(CONFIG_##_source), \
		(STRUCT_SECTION_ITERABLE(zbus_multicore_tx, _zbus_multicore_tx_##_chan) = { \
			.chan = &_chan, \
			.channel_id = UINT32_MAX, \
		}), \
		(STRUCT_SECTION_ITERABLE(zbus_multicore_rx, _zbus_multicore_rx_##_chan) = { \
			.chan = &_chan, \
			.channel_id = UINT32_MAX, \
		}) \
	)
/* init ipc connection and get ready for recv/send */
int init_ipc(void);

void init_zbus_multicore(void);

/* channel for sensor data sent from remote to host */
ZBUS_CHAN_DECLARE(sensor_chan);


#endif /* __COMMON_H__ */
