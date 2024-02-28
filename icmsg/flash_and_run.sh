#!/usr/bin/env bash
west build -b nrf5340dk_nrf5340_cpuapp --sysbuild
nrfjprog --program build/icmsg/zephyr/zephyr.hex --chiperase --verify -f NRF53 --coprocessor CP_APPLICATION
nrfjprog --program build/remote/zephyr/zephyr.hex --chiperase --verify -f NRF53 --coprocessor CP_NETWORK
nrfjprog -d
