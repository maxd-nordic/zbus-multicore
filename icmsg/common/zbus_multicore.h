/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ZBUS_MULTICORE_H__
#define __ZBUS_MULTICORE_H__

#include <zephyr/zbus/zbus.h>

struct zbus_multicore_tx
{
	const struct zbus_channel *chan;
	uint32_t channel_id;
};

struct zbus_multicore_rx
{
	const struct zbus_channel *chan;
	uint32_t channel_id;
};

struct zbus_multicore_channel
{
	const struct zbus_channel *chan;
};

#define ZBUS_MULTICORE_CHANNEL_ADD(_chan)                                                 \
	const STRUCT_SECTION_ITERABLE(zbus_multicore_channel, _zbus_multicore_chan_##_chan) = { \
	    .chan = &_chan,                                                               \
	};                                                                                \
	extern struct zbus_observer _zbus_multicore_listener;                             \
	ZBUS_CHAN_ADD_OBS(_chan, _zbus_multicore_listener, 0);

#define ZBUS_MULTICORE_FORWARDER_ADD(_chan, _source, _dest)                             \
	COND_CODE_1(                                                                    \
	    IS_ENABLED(CONFIG_##_source),                                               \
	    (STRUCT_SECTION_ITERABLE(zbus_multicore_tx, _zbus_multicore_tx_##_chan) = { \
		 .chan = &_chan,                                                        \
		 .channel_id = UINT32_MAX,                                              \
	     }),                                                                        \
	    (STRUCT_SECTION_ITERABLE(zbus_multicore_rx, _zbus_multicore_rx_##_chan) = { \
		 .chan = &_chan,                                                        \
		 .channel_id = UINT32_MAX,                                              \
	     }))

int init_zbus_multicore(void);

#endif /* __ZBUS_MULTICORE_H__ */
