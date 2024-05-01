/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define I2C_DEV_NODE DT_NODELABEL(i2c1)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct device *const i2c_dev = DEVICE_DT_GET(I2C_DEV_NODE);
uint32_t i2c_cfg = I2C_SPEED_SET(I2C_SPEED_STANDARD) | I2C_MODE_CONTROLLER;

int main(void)
{
	int ret;
	bool led_state = true;

	//i2c stuff
	unsigned char i2c_data [10] = "i2c data\n";
	uint32_t i2c_cfg_tmp;
	uint16_t i2c_addr_slave = 0x06;

	//set up led
	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	//set up i2c
	if (!device_is_ready(i2c_dev)) {
		printf("I2C device is not ready\n");
		return 0;
	}

	/* 1. Verify i2c_configure() */
	if (i2c_configure(i2c_dev, i2c_cfg)) {
		printf("I2C config failed\n");
		return 0;
	}

	/* 2. Verify i2c_get_config() */
	if (i2c_get_config(i2c_dev, &i2c_cfg_tmp)) {
		printf("I2C get_config failed\n");
		return 0;
	}
	if (i2c_cfg != i2c_cfg_tmp) {
		printf("I2C get_config returned invalid config\n");
		return 0;
	}


	while (1) {
		// toggle LED
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			return 0;
		}

		led_state = !led_state;
		printf("LED state: %s\n", led_state ? "ON" : "OFF");

		//send i2c
		if (i2c_write(i2c_dev, i2c_data, 9, i2c_addr_slave)) {
			printf("Fail to send i2c\n");
		}

		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}