#include "zbus_multicore.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/ipc/ipc_service.h>
#include <zephyr/logging/log.h>

#define ZBUS_MULTICORE_FLAG_ACK BIT(0)

struct data_packet_header
{
	uint32_t channel_id;
};

struct zbus_multicore_control_message {
	struct data_packet_header header;
	uint32_t orginial_channel_id;
	uint32_t flags;
};

LOG_MODULE_REGISTER(zbus_multicore, LOG_LEVEL_INF);

K_SEM_DEFINE(bound_sem, 0, 1);

static void ep_bound(void *priv)
{
	k_sem_give(&bound_sem);
	LOG_INF("Ep bounded");
}

STRUCT_SECTION_ITERABLE(zbus_multicore_channel, _zbus_multicore_system_channel) = {0};

static struct ipc_ept ep;

static void ep_recv(const void *data, size_t len, void *priv)
{
	struct data_packet_header *header = (struct data_packet_header *)data;
	uint8_t *payload = (uint8_t *)data + sizeof(struct data_packet_header);
	LOG_INF("Received message with channel_id %d and size %d", header->channel_id, len - sizeof(struct data_packet_header));

	STRUCT_SECTION_FOREACH(zbus_multicore_rx, agent)
	{
		if (agent->channel_id == header->channel_id)
		{
			zbus_chan_pub(agent->chan, payload, K_FOREVER);
			// if this is a blocking forwarder, we need to send an ack
			if (header->flags & ZBUS_MULTICORE_FLAG_BLOCKING) {
				struct zbus_multicore_control_message ack = {
					.header = {
						.channel_id = _zbus_multicore_system_channel.channel_id,
						.flags = ZBUS_MULTICORE_FLAG_ACK,
					},
					.orginial_channel_id = header->channel_id,
				};
				int ret = ipc_service_send(&ep, &ack, sizeof(ack));
				if (ret < 0)
				{
					LOG_ERR("ipc_service_send() failure: %d", ret);
				}
			}
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

			LOG_INF("Sending message with channel_id %d and size %d", header->channel_id, chan->message_size);
			int ret = ipc_service_send(&ep, buf, sizeof(struct data_packet_header) + chan->message_size);
			if (ret < 0)
			{
				LOG_ERR("ipc_service_send() failure: %d", ret);
			}
			break;
		}
	}
}

ZBUS_LISTENER_DEFINE(_zbus_multicore_listener, listener_callback_iterables);

int init_zbus_multicore(void)
{
	int ret = 0;

	ret = init_ipc();
	if (ret)
	{
		return ret;
	}

	size_t current_id = 0;
	STRUCT_SECTION_FOREACH(zbus_multicore_channel, channel)
	{
		STRUCT_SECTION_FOREACH(zbus_multicore_rx, agent)
		{
			if (agent->chan == channel->chan)
			{
				agent->channel_id = current_id;
			}
		}

		STRUCT_SECTION_FOREACH(zbus_multicore_tx, agent)
		{
			if (agent->chan == channel->chan)
			{
				agent->channel_id = current_id;
			}
		}
		channel->channel_id = current_id++;
	}

	return ret;
}
