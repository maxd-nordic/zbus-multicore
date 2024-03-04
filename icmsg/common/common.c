#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/ipc/ipc_service.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "common.h"
#include "zbus_multicore.h"

LOG_MODULE_REGISTER(common, LOG_LEVEL_INF);

ZBUS_CHAN_DEFINE(temperature_chan,
		 struct temperature_data,
		 NULL, NULL,
		 ZBUS_OBSERVERS_EMPTY,
		 ZBUS_MSG_INIT(0));

ZBUS_CHAN_DEFINE(humidity_chan,
		 struct humidity_data,
		 NULL, NULL,
		 ZBUS_OBSERVERS_EMPTY,
		 ZBUS_MSG_INIT(0));

ZBUS_CHAN_DEFINE(led_chan,
		 struct led_data,
		 NULL, NULL,
		 ZBUS_OBSERVERS_EMPTY,
		 ZBUS_MSG_INIT(0));

ZBUS_MULTICORE_CHANNEL_ADD(temperature_chan);
ZBUS_MULTICORE_FORWARDER_ADD(temperature_chan, BOARD_NRF5340DK_NRF5340_CPUNET, BOARD_NRF5340DK_NRF5340_CPUAPP);

ZBUS_MULTICORE_CHANNEL_ADD(humidity_chan);
ZBUS_MULTICORE_FORWARDER_ADD(humidity_chan, BOARD_NRF5340DK_NRF5340_CPUNET, BOARD_NRF5340DK_NRF5340_CPUAPP);

ZBUS_MULTICORE_CHANNEL_ADD(led_chan);
ZBUS_MULTICORE_FORWARDER_ADD(led_chan, BOARD_NRF5340DK_NRF5340_CPUAPP, BOARD_NRF5340DK_NRF5340_CPUNET);
