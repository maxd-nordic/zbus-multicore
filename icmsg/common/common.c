#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/ipc/ipc_service.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "common.h"

LOG_MODULE_REGISTER(common, LOG_LEVEL_INF);

K_SEM_DEFINE(bound_sem, 0, 1);

static void ep_bound(void *priv)
{
	k_sem_give(&bound_sem);
	LOG_INF("Ep bounded");
}

static struct ipc_ept ep;

static void ep_recv(const void *data, size_t len, void *priv)
{
	LOG_INF("Received message with size %d", len);

	struct data_packet_header *header = (struct data_packet_header *)data;
	uint8_t *payload = (uint8_t *)data + sizeof(struct data_packet_header);
	LOG_INF("Received message with channel_id %d", header->channel_id);

	STRUCT_SECTION_FOREACH(zbus_multicore_rx, agent)
	{
		if (agent->channel_id == header->channel_id)
		{
			zbus_chan_pub(agent->chan, payload, K_FOREVER);
			break;
		}
	}
}

static struct ipc_ept_cfg ep_cfg = {
    .cb = {
	.bound = ep_bound,
	.received = ep_recv,
    },
};

int init_ipc(void)
{
	const struct device *ipc0_instance;
	int ret;

	ipc0_instance = DEVICE_DT_GET(DT_NODELABEL(ipc0));
	ret = ipc_service_open_instance(ipc0_instance);
	if ((ret < 0) && (ret != -EALREADY))
	{
		LOG_ERR("ipc_service_open_instance() failure");
		return ret;
	}

	ret = ipc_service_register_endpoint(ipc0_instance, &ep, &ep_cfg);
	if (ret < 0)
	{
		LOG_ERR("ipc_service_register_endpoint() failure");
		return ret;
	}

	LOG_INF("waiting for epc bound");

	k_sem_take(&bound_sem, K_FOREVER);

	return ret;
}



static void listener_callback_iterables(const struct zbus_channel *chan)
{
	const void *msg = zbus_chan_const_msg(chan);
	uint8_t buf[64];

	STRUCT_SECTION_FOREACH(zbus_multicore_tx, agent)
	{
		if (agent->chan == chan)
		{
			struct data_packet_header *header = (struct data_packet_header *)buf;
			header->channel_id = agent->channel_id;

			if (chan->message_size > sizeof(buf) - sizeof(struct data_packet_header))
			{
				LOG_ERR("Message too big");
				return;
			}

			memcpy(buf + sizeof(struct data_packet_header), msg, chan->message_size);

			int ret = ipc_service_send(&ep, buf, sizeof(struct data_packet_header) + chan->message_size);
			if (ret < 0)
			{
				LOG_ERR("ipc_service_send() failure: %d", ret);
			}
			break;
		}
	}
}

void init_zbus_multicore(void)
{
	size_t current_id = 0;
	STRUCT_SECTION_FOREACH(zbus_multicore_channel, channel) {
		STRUCT_SECTION_FOREACH(zbus_multicore_rx, agent) {
			if (agent->chan == channel->chan) {
				agent->channel_id = current_id;
			}
		}

		STRUCT_SECTION_FOREACH(zbus_multicore_tx, agent) {
			if (agent->chan == channel->chan) {
				agent->channel_id = current_id;
			}
		}
		current_id++;
	}
}
ZBUS_LISTENER_DEFINE(_zbus_multicore_listener, listener_callback_iterables);

ZBUS_CHAN_DEFINE(sensor_chan,
		 struct sensor_data,
		 NULL, NULL,
		 ZBUS_OBSERVERS_EMPTY,
		 ZBUS_MSG_INIT(0));

ZBUS_CHAN_ADD_OBS(sensor_chan, _zbus_multicore_listener, 0);

ZBUS_MULTICORE_CHANNEL_ADD(sensor_chan);
ZBUS_MULTICORE_FORWARDER_ADD(sensor_chan, BOARD_NRF5340DK_NRF5340_CPUNET, BOARD_NRF5340DK_NRF5340_CPUAPP);
