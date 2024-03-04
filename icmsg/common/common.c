#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/ipc/ipc_service.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "common.h"
#include "zbus_multicore.h"

LOG_MODULE_REGISTER(common, LOG_LEVEL_INF);

ZBUS_CHAN_DEFINE(sensor_chan,
		 struct sensor_data,
		 NULL, NULL,
		 ZBUS_OBSERVERS_EMPTY,
		 ZBUS_MSG_INIT(0));

ZBUS_MULTICORE_CHANNEL_ADD(sensor_chan);
ZBUS_MULTICORE_FORWARDER_ADD(sensor_chan, BOARD_NRF5340DK_NRF5340_CPUNET, BOARD_NRF5340DK_NRF5340_CPUAPP);
