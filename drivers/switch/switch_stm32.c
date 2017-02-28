/*
 * Copyright (C) 2017 Emcraft Systems
 * Sergei Miroshnichenko <sergeimir@emcraft.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <fdt.h>
#include <libfdt.h>
#include <fdt_support.h>
#include <asm/arch/stm32f2_gpio.h>

struct gpio_banks {
	const char *name;
	enum stm32f2_gpio_port port;
};

static struct gpio_banks gpio_banks[] = {
	{ "GPIOA", STM32F2_GPIO_PORT_A },
	{ "GPIOB", STM32F2_GPIO_PORT_B },
	{ "GPIOC", STM32F2_GPIO_PORT_C },
	{ "GPIOD", STM32F2_GPIO_PORT_D },
	{ "GPIOE", STM32F2_GPIO_PORT_E },
	{ "GPIOF", STM32F2_GPIO_PORT_F },
	{ "GPIOG", STM32F2_GPIO_PORT_G },
	{ "GPIOH", STM32F2_GPIO_PORT_H },
	{ "GPIOI", STM32F2_GPIO_PORT_I },
	{ "GPIOJ", STM32F2_GPIO_PORT_J },
	{ "GPIOK", STM32F2_GPIO_PORT_K },
};

void fdt_fixup_switch(void *fdt)
{
	int switch_node = fdt_node_offset_by_compatible(fdt, -1, "gpio-configuration-switch");

	if (switch_node != -FDT_ERR_NOTFOUND) {
		int pinctrl_node, pins_node, offset, nextoffset;
		const u32 *pinctrl_h;
		uint32_t tag;

		pinctrl_h = fdt_getprop(fdt, switch_node, "pinctrl-0", NULL);
		if (!pinctrl_h) {
			error("%s: no pinctrl defined for the switch node\n", __func__);
			return;
		}

		pinctrl_node = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(*pinctrl_h));
		/*
		 * pins_node is the "st,pins" node
		 */
		pins_node = fdt_next_node(fdt, pinctrl_node, NULL);

		/*
		 * Set up pinctrl for the signal gpios: pullups, etc.
		 */
		offset = pins_node;
		while ((tag = fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END_NODE) {
			if (tag == FDT_PROP) {
				const int pin_size = 6;
				const struct fdt_property *fdt_prop = fdt_offset_ptr(fdt, offset,
										     sizeof(*fdt_prop));
				int len = fdt32_to_cpu(fdt_prop->len);
				u32 *v = (u32*)fdt_prop->data;
				int gpio_bank_off;
				const char *bank_name;
				int i, port = -1;
				struct stm32f2_gpio_dsc iomux;
				int err;
				/*
				 * Example of data format:
				 * status_pullup = <&gpioe 5 IN PULL_UP PUSH_PULL LOW_SPEED>;
				 * v[0] - gpio bank
				 * v[1] - gpio pin
				 * v[2] - direction
				 * v[3] - NO_PULL/PULL_UP/PULL_DOWN
				 * v[4] - PUSH_PULL/OPEN_DRAIN
				 * v[5] - speed
				 */
				int pin = fdt32_to_cpu(v[1]);
				int direction_in = !fdt32_to_cpu(v[2]);
				int pullup = fdt32_to_cpu(v[3]) == 1;

				if (len % (pin_size * sizeof(u32))) {
					printf("%s: invalid pinctrl value\n", __func__);
				}
				gpio_bank_off = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(v[0]));
				bank_name = (const char *)fdt_getprop(fdt, gpio_bank_off, "st,bank-name", NULL);
				for (i = 0; i < sizeof(gpio_banks)/sizeof(gpio_banks[0]); ++i) {
					if (0 == strcmp(bank_name, gpio_banks[i].name)) {
						port = gpio_banks[i].port;
						break;
					}
				}

				if (port < 0) {
					error("%s: unsupported bank name: %s\n", __func__, bank_name);
					return;
				}

				if (!direction_in) {
					error("%s: only input switch gpios are supported\n", __func__);
					return;
				}

				iomux.port = port;
				iomux.pin = pin;
				err = stm32f2_gpio_config(&iomux, pullup ? STM32F2_GPIO_ROLE_GPIN_PULLUP : STM32F2_GPIO_ROLE_GPIN);
				if (err) {
					error("%s: gpio configuration failed: %d\n", __func__, err);
					return;
				}
			}
			offset = nextoffset;
		}

		/*
		 * Delay for the gpios to establish
		 */
		udelay(200);

		/*
		 * Find every node with the "configuration-switch-gpios" property set and change its status accordingly
		 */
		for (offset = fdt_next_node(fdt, -1, NULL);
		     offset >= 0;
		     offset = fdt_next_node(fdt, offset, NULL)) {
			int len;
			struct fdt_property *prop = fdt_get_property_w(fdt, offset, "configuration-switch-gpios", &len);
			if (prop) {
				const int pin_size = 3;
				int len = fdt32_to_cpu(prop->len);
				u32 *v = (u32*)prop->data;
				int port = -1;
				int pin = fdt32_to_cpu(v[1]);
				int active_low = !!fdt32_to_cpu(v[2]);
				int gpio_value;
				struct stm32f2_gpio_dsc iomux;
				int i;
				int gpio_bank_off = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(v[0]));
				const char *bank_name = (const char *)fdt_getprop(fdt, gpio_bank_off, "st,bank-name", NULL);
				const char *status;
				int need_to_enable;
				for (i = 0; i < sizeof(gpio_banks)/sizeof(gpio_banks[0]); ++i) {
					if (0 == strcmp(bank_name, gpio_banks[i].name)) {
						port = gpio_banks[i].port;
						break;
					}
				}

				if (port < 0) {
					error("%s: unsupported bank name: %s\n", __func__, bank_name);
					return;
				}

				if (len % (pin_size * sizeof(u32))) {
					printf("%s: invalid pinctrl value\n", __func__);
				}

				iomux.port = port;
				iomux.pin = pin;
				gpio_value = stm32f2_gpout_get(&iomux);
				need_to_enable = (gpio_value && !active_low) || (!gpio_value && active_low);
				status = need_to_enable ? "okay" : "disabled";
				printf("%s: Setting node %s to \"%s\" as instructed by gpio %d.%d (value %d, active %s)\n",
				       __func__, fdt_get_name(fdt, offset, NULL), status,
				       port, pin, gpio_value, active_low ? "low" : "high");
				fdt_setprop(fdt, offset, "status", status, strlen(status) + 1);

				if (!need_to_enable) {
					const u32 *pinctrl_h;
					int pinctrl_node;
					int pins_node;
					int err;

					pinctrl_h = fdt_getprop(fdt, offset, "pinctrl-0", NULL);
					if (!pinctrl_h) {
						continue;
					}

					pinctrl_node = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(*pinctrl_h));
					pins_node = fdt_next_node(fdt, pinctrl_node, NULL);

					err = fdt_del_node(fdt, pins_node);
					if (err) {
						error("%s[%d]: failed to remove the st-pins node: %d\n", __func__, __LINE__, err);
					}
					pins_node = fdt_add_subnode(fdt, pinctrl_node, "st,pins");
					if (pins_node <= 0) {
						error("%s[%d]: failed to add empty st-pins node: %d\n", __func__, __LINE__, pins_node);
					}
				}
			}
		}
	} else {
		debug("%s: no switch node\n", __func__);
	}
}
