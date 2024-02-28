# ZBus Multicore tinkering workspace

This is a workspace to try out implementation ideas for ZBus Multicore.

## Getting started

Before getting started, make sure you have a proper nRF Connect SDK development environment.
Follow the official
[Installation guide](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/getting_started.html).

### Initialization

The first step is to initialize the workspace folder (``zbus-multicore`) where
the application and all nRF Connect SDK modules will be cloned. Run the following
command:

```shell
# initialize workspace for the ncs-example-application (main branch)
west init -m https://github.com/maxd-nordic/zbus-multicore.git --mr main zbus-multicore
# update nRF Connect SDK modules
cd zbus-multicore
west update
```

### Building and running

To build the application, run the following command:

```shell
west build -b nrf5340dk_nrf5340_cpuapp icmsg
```
