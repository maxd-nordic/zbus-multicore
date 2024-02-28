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

static void agent_cb(const void *data, size_t len);

static void ep_recv(const void *data, size_t len, void *priv)
{
	if (len != sizeof(struct data_packet))
	{
		LOG_ERR("Unexpected message length");
		return;
	}

	LOG_INF("Received message with size %d", len);

	agent_cb(data, len);
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

int ipc_send(struct data_packet *msg)
{
	int ret;
	ret = ipc_service_send(&ep, msg, sizeof(struct data_packet));
	if (ret < 0)
	{
		LOG_ERR("ipc_service_send() failure");
		return ret;
	}

	return ret;
}


ZBUS_CHAN_DEFINE(sensor_chan,
		 struct sensor_data,
		 NULL, NULL,
		 ZBUS_OBSERVERS_EMPTY,
		 ZBUS_MSG_INIT(0));

#if defined(CONFIG_BOARD_NRF5340DK_NRF5340_CPUAPP)

static void agent_cb(const void *data, size_t len)
{
	struct data_packet *packet = (struct data_packet *)data;

	LOG_INF("Received message with channel_id %d", packet->channel_id);

	if (packet->channel_id == 0)
	{
		const struct sensor_data *msg = &packet->data.sensor;
		zbus_chan_pub(&sensor_chan, msg, K_FOREVER);
	}
}


#elif defined(CONFIG_BOARD_NRF5340DK_NRF5340_CPUNET)

static void agent_cb(const void *data, size_t len)
{
}

static void listener_callback(const struct zbus_channel *chan)
{
	const struct sensor_data *msg = zbus_chan_const_msg(chan);

	struct data_packet packet = {
		.channel_id = 0,
	};
	memcpy((void *)&packet.data.sensor, (void *)msg, sizeof(struct sensor_data));

	size_t len = offsetof(struct data_packet, data.sensor) + sizeof(struct sensor_data);

	int ret = ipc_service_send(&ep, &packet, len);
	if (ret < 0) {
		LOG_ERR("ipc_service_send() failed with ret %d", ret);
	}
}
ZBUS_LISTENER_DEFINE(agent_listener, listener_callback);
ZBUS_CHAN_ADD_OBS(sensor_chan, agent_listener, 4);

#endif
