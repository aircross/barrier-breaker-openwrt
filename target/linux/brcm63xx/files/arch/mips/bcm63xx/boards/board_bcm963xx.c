/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2008 Maxime Bizon <mbizon@freebox.fr>
 * Copyright (C) 2008 Florian Fainelli <florian@openwrt.org>
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <linux/ssb/ssb.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <linux/export.h>
#include <linux/platform_data/b53.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_gpio.h>
#include <linux/spi/74x164.h>
#include <linux/rtl8367.h>
#include <asm/addrspace.h>
#include <bcm63xx_board.h>
#include <bcm63xx_cpu.h>
#include <bcm63xx_dev_uart.h>
#include <bcm63xx_regs.h>
#include <bcm63xx_io.h>
#include <bcm63xx_nvram.h>
#include <bcm63xx_dev_pci.h>
#include <bcm63xx_dev_enet.h>
#include <bcm63xx_dev_dsp.h>
#include <bcm63xx_dev_flash.h>
#include <bcm63xx_dev_hsspi.h>
#include <bcm63xx_dev_pcmcia.h>
#include <bcm63xx_dev_spi.h>
#include <bcm63xx_dev_usb_ehci.h>
#include <bcm63xx_dev_usb_ohci.h>
#include <bcm63xx_dev_usb_usbd.h>
#include <bcm63xx_sprom_bcma.h>
#include <board_bcm963xx.h>
#include <pci_ath9k_fixup.h>
#include <pci_rt2x00_fixup.h>

#include <uapi/linux/bcm933xx_hcs.h>
#include <uapi/linux/bcm963xx_tag.h>

#define PFX	"board_bcm963xx: "

#define HCS_OFFSET_128K			0x20000

#define BCM963XX_KEYS_POLL_INTERVAL	20
#define BCM963XX_KEYS_DEBOUNCE_INTERVAL	(BCM963XX_KEYS_POLL_INTERVAL * 3)

#define CFE_OFFSET_64K			0x10000
#define CFE_OFFSET_128K			0x20000

#define NB4_PID_OFFSET		0xff80
#define NB4_74X164_GPIO_BASE	64
#define NB4_SPI_GPIO_MOSI	7
#define NB4_SPI_GPIO_CLK	6
#define NB4_74HC64_GPIO(X)	(NB4_74X164_GPIO_BASE + (X))
#define NB6_GPIO_RTL8367_SDA	18
#define NB6_GPIO_RTL8367_SCK	20

#define CT6373_PID_OFFSET		0xff80
#define CT6373_74X164_GPIO_BASE	64
#define CT6373_SPI_GPIO_MOSI	7
#define CT6373_SPI_GPIO_CLK		6
#define CT6373_74HC64_GPIO(X)	(CT6373_74X164_GPIO_BASE + (X))


static struct board_info board;

/*
 * known 3368 boards
 */
#ifdef CONFIG_BCM63XX_CPU_3368
static struct board_info __initdata board_cvg834g = {
	.name				= "CVG834G_E15R3921",
	.expected_cpu_id		= 0x3368,

	.has_uart0			= 1,
	.has_uart1			= 1,

	.has_enet0			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},

	.leds = {
		{
			.name		= "CVG834G:green:power",
			.gpio		= 37,
			.default_trigger= "default-on",
		},
	},

	.ephy_reset_gpio		= 36,
	.ephy_reset_gpio_flags		= GPIOF_INIT_HIGH,
};
#endif

/*
 * known 6318 boards
 */
#ifdef CONFIG_BCM63XX_CPU_6318
static struct board_info __initdata board_96318ref = {
	.name				= "96318REF",
	.expected_cpu_id		= 0x6318,

	.has_uart0			= 1,
	.has_pci			= 1,

	.has_usbd			= 1,

	.usbd = {
		.use_fullspeed		= 0,
		.port_no		= 0,
	},

	.has_enetsw			= 1,

	.has_ehci0			= 1,
	.num_usbh_ports			= 1,

	.enetsw = {
		.used_ports = {
			[0] = {
				.used	= 1,
				.phy_id = 1,
				.name	= "Port 1",
			},
			[1] = {
				.used	= 1,
				.phy_id	= 2,
				.name	= "Port 2",
			},
			[2] = {
				.used	= 1,
				.phy_id	= 3,
				.name	= "Port 3",
			},
			[3] = {
				.used	= 1,
				.phy_id	= 4,
				.name	= "Port 4",
			},
		},
	},

	.leds = {
		{
			.name		= "96318REF:red:post-failed",
			.gpio		= 11,
			.active_low	= 1,
		},
		{
			.name		= "96318REF:green:inet",
			.gpio		= 8,
			.active_low	= 1,
		},
		{
			.name		= "96318REF:red:inet-fail",
			.gpio		= 9,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc			= "wps",
			.gpio			= 33,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_WPS_BUTTON,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc			= "reset",
			.gpio			= 34,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_RESTART,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

static struct board_info __initdata board_96318ref_p300 = {
	.name				= "96318REF_P300",
	.expected_cpu_id		= 0x6318,

	.has_uart0			= 1,
	.has_pci			= 1,

	.has_usbd			= 1,

	.usbd = {
		.use_fullspeed		= 0,
		.port_no		= 0,
	},

	.has_enetsw			= 1,

	.has_ehci0			= 1,
	.num_usbh_ports			= 1,

	.enetsw = {
		.used_ports = {
			[0] = {
				.used	= 1,
				.phy_id = 1,
				.name	= "Port 1",
			},
			[1] = {
				.used	= 1,
				.phy_id	= 2,
				.name	= "Port 2",
			},
			[2] = {
				.used	= 1,
				.phy_id	= 3,
				.name	= "Port 3",
			},
			[3] = {
				.used	= 1,
				.phy_id	= 4,
				.name	= "Port 4",
			},
		},
	},

	.leds = {
		{
			.name		= "96318REF_P300:red:post-failed",
			.gpio		= 11,
			.active_low	= 1,
		},
		{
			.name		= "96318REF_P300:green:inet",
			.gpio		= 8,
			.active_low	= 1,
		},
		{
			.name		= "96318REF_P300:red:inet-fail",
			.gpio		= 9,
			.active_low	= 1,
		},
		{
			.name		= "96318REF_P300::usb-pwron",
			.gpio		= 13,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
	},

	.buttons = {
		{
			.desc			= "wps",
			.gpio			= 33,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_WPS_BUTTON,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc			= "reset",
			.gpio			= 34,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_RESTART,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};
#endif

/*
 * known 6328 boards
 */
#ifdef CONFIG_BCM63XX_CPU_6328
static struct board_info __initdata board_96328avng = {
	.name				= "96328avng",
	.expected_cpu_id		= 0x6328,

	.has_uart0			= 1,
	.has_pci			= 1,
	.has_usbd			= 0,

	.usbd = {
		.use_fullspeed		= 0,
		.port_no		= 0,
	},

	.has_enetsw			= 1,

	.enetsw = {
		.used_ports = {
			[0] = {
				.used	= 1,
				.phy_id = 1,
				.name	= "Port 1",
			},
			[1] = {
				.used	= 1,
				.phy_id	= 2,
				.name	= "Port 2",
			},
			[2] = {
				.used	= 1,
				.phy_id	= 3,
				.name	= "Port 3",
			},
			[3] = {
				.used	= 1,
				.phy_id	= 4,
				.name	= "Port 4",
			},
		},
	},

	.leds = {
		{
			.name		= "96328avng::internet-fail",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "96328avng::dsl",
			.gpio		= 3,
			.active_low	= 1,
		},
		{
			.name		= "96328avng::power",
			.gpio		= 4,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "96328avng::power-fail",
			.gpio		= 8,
			.active_low	= 1,
		},
		{
			.name		= "96328avng::wps",
			.gpio		= 9,
			.active_low	= 1,
		},
		{
			.name		= "96328avng::internet",
			.gpio		= 11,
			.active_low	= 1,
		},
	},
};

static struct board_info __initdata board_AR5381u = {
	.name					= "96328A-1241N",
	.expected_cpu_id			= 0x6328,

	.has_uart0				= 1,
	.has_pci				= 1,
	.has_ohci0				= 1,
	.has_ehci0				= 1,
	.num_usbh_ports				= 1,
	.has_enetsw				= 1,

	.enetsw = {
		.used_ports = {
			[0] = {
				.used		= 1,
				.phy_id		= 1,
				.name		= "Port 1",
			},
			[1] = {
				.used		= 1,
				.phy_id		= 2,
				.name		= "Port 2",
			},
			[2] = {
				.used		= 1,
				.phy_id		= 3,
				.name		= "Port 3",
			},
			[3] = {
				.used		= 1,
				.phy_id		= 4,
				.name		= "Port 4",
			},
		},
	},

	.leds = {
		{
			.name			= "AR-5381u:green:power",
			.gpio			= 4,
			.active_low		= 1,
			.default_trigger	= "default-on",
		},
		{
			.name			= "AR-5381u:red:alarm",
			.gpio			= 2,
			.active_low		= 1,
		},
		{
			.name			= "AR-5381u:green:inet",
			.gpio			= 3,
			.active_low		= 1,
		},
	},

	.buttons = {
		{
			.desc			= "reset",
			.gpio			= 23,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_RESTART,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

static struct board_info __initdata board_AR5387un = {
	.name					= "96328A-1441N1",
	.expected_cpu_id			= 0x6328,

	.has_uart0				= 1,
	.has_pci				= 1,
	.has_ohci0				= 1,
	.has_ehci0				= 1,
	.num_usbh_ports				= 1,
	.has_enetsw				= 1,

	.enetsw = {
		.used_ports = {
			[0] = {
				.used		= 1,
				.phy_id		= 1,
				.name		= "Port 1",
			},
			[1] = {
				.used		= 1,
				.phy_id		= 2,
				.name		= "Port 2",
			},
			[2] = {
				.used		= 1,
				.phy_id		= 3,
				.name		= "Port 3",
			},
			[3] = {
				.used		= 1,
				.phy_id		= 4,
				.name		= "Port 4",
			},
		},
	},

	.leds = {
		{
			.name			= "AR-5387un:green:power",
			.gpio			= 8,
			.default_trigger	= "default-on",
		},
		{
			.name			= "AR-5387un:red:power",
			.gpio			= 4,
		},
		{
			.name			= "AR-5387un:green:inet",
			.gpio			= 7,
		},
		{
			.name			= "AR-5387un:red:inet",
			.gpio			= 1,
		},
		{
			.name			= "AR-5387un:green:dsl",
			.gpio			= 11,
			.active_low		= 1,
		},
	},

	.buttons = {
		{
			.desc			= "reset",
			.gpio			= 23,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_RESTART,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

static struct board_info __initdata board_963281TAN = {
	.name				= "963281TAN",
	.expected_cpu_id		= 0x6328,

	.has_uart0			= 1,
	.has_pci			= 1,

	.has_enetsw			= 1,

	.enetsw = {
		.used_ports = {
			[0] = {
				.used	= 1,
				.phy_id = 1,
				.name	= "Port 1",
			},
			[1] = {
				.used	= 1,
				.phy_id	= 2,
				.name	= "Port 2",
			},
			[2] = {
				.used	= 1,
				.phy_id	= 3,
				.name	= "Port 3",
			},
			[3] = {
				.used	= 1,
				.phy_id	= 4,
				.name	= "Port 4",
			},
		},
	},

	.leds = {
		{
			.name		= "963281TAN::internet",
			.gpio		= 1,
			.active_low	= 1,
		},
		{
			.name		= "963281TAN::power",
			.gpio		= 4,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "963281TAN::internet-fail",
			.gpio		= 7,
			.active_low	= 1,
		},
		{
			.name		= "963281TAN::power-fail",
			.gpio		= 8,
			.active_low	= 1,
		},
		{
			.name		= "963281TAN::wps",
			.gpio		= 9,
			.active_low	= 1,
		},
		{
			.name		= "963281TAN::dsl",
			.gpio		= 11,
			.active_low	= 1,
		},

	},
};

static struct board_info __initdata board_A4001N = {
	.name					= "963281T_TEF",
	.expected_cpu_id			= 0x6328,

	.has_uart0				= 1,
	.has_pci				= 1,
	.has_ohci0				= 1,
	.has_ehci0				= 1,
	.num_usbh_ports				= 1,
	.has_enetsw				= 1,

	.enetsw = {
		.used_ports = {
			[0] = {
				.used		= 1,
				.phy_id		= 1,
				.name		= "Port 1",
			},
			[1] = {
				.used		= 1,
				.phy_id		= 2,
				.name		= "Port 2",
			},
			[2] = {
				.used		= 1,
				.phy_id		= 3,
				.name		= "Port 3",
			},
			[3] = {
				.used		= 1,
				.phy_id		= 4,
				.name		= "Port 4",
			},
		},
	},

	.leds = {
		{
			.name			= "A4001N:green:power",
			.gpio			= 4,
			.active_low		= 1,
			.default_trigger	= "default-on",
		},
		{
			.name			= "A4001N:red:power",
			.gpio			= 8,
			.active_low		= 1,
		},
		{
			.name			= "A4001N:green:inet",
			.gpio			= 11,
			.active_low		= 1,
		},
		{
			.name			= "A4001N:red:inet",
			.gpio			= 2,
			.active_low		= 1,
		},
		{
			.name			= "A4001N:green:ppp",
			.gpio			= 3,
			.active_low		= 1,
		},
		{
			.name			= "A4001N:red:ppp",
			.gpio			= 5,
			.active_low		= 1,
		},
		{
			.name			= "A4001N:green:3g",
			.gpio			= 6,
			.active_low		= 1,
		},
		{
			.name			= "A4001N:red:3g",
			.gpio			= 7,
			.active_low		= 1,
		},
		{
			.name			= "A4001N:green:wlan",
			.gpio			= 9,
			.active_low		= 1,
		},
		{
			.name			= "A4001N:red:wlan",
			.gpio			= 10,
			.active_low		= 1,
		},
		{
			.name			= "A4001N:green:eth",
			.gpio			= 31,
			.active_low		= 1,
		},
		{
			.name			= "A4001N:red:eth",
			.gpio			= 20,
			.active_low		= 1,
		},
	},

	.buttons = {
		{
			.desc			= "reset",
			.gpio			= 23,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_RESTART,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc			= "wlan",
			.gpio			= 24,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_WLAN,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

static struct board_info __initdata board_dsl_274xb_f1 = {
	.name				= "AW4339U",
	.expected_cpu_id		= 0x6328,

	.has_uart0			= 1,
	.has_pci			= 1,

	.has_caldata			= 1,
	.caldata = {
		{
			.vendor		= PCI_VENDOR_ID_ATHEROS,
			.caldata_offset	= 0x7d1000,
			.slot		= 0,
			.led_pin	= -1,
		},
	},

	.has_enetsw			= 1,

	.enetsw = {
		.used_ports = {
			[0] = {
				.used	= 1,
				.phy_id = 1,
				.name	= "Port 4",
			},
			[1] = {
				.used	= 1,
				.phy_id	= 2,
				.name	= "Port 3",
			},
			[2] = {
				.used	= 1,
				.phy_id	= 3,
				.name	= "Port 2",
			},
			[3] = {
				.used	= 1,
				.phy_id	= 4,
				.name	= "Port 1",
			},
		},
	},

	.leds = {
		{
			.name		= "dsl-274xb:red:internet",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "dsl-274xb:green:dsl",
			.gpio		= 3,
			.active_low	= 1,
		},
		{
			.name		= "dsl-274xb:green:power",
			.gpio		= 4,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "dsl-274xb:red:power",
			.gpio		= 8,
			.active_low	= 1,
		},
		{
			.name		= "dsl-274xb:blue:wps",
			.gpio		= 9,
			.active_low	= 1,
		},
		{
			.name		= "dsl-274xb:green:internet",
			.gpio		= 11,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc		= "wifi",
			.gpio		= 10,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= BTN_0,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "reset",
			.gpio		= 23,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "wps",
			.gpio		= 24,
			.active_low	= 1,
			.code		= KEY_WPS_BUTTON,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

static struct board_info __initdata board_FAST2704V2 = {
	.name				= "F@ST2704V2",
	.expected_cpu_id		= 0x6328,

	.has_uart0			= 1,
	.has_pci			= 1,
	.has_ohci0			= 1,
	.has_ehci0			= 1,
	.has_usbd			= 1,

	.has_enetsw			= 1,

	.enetsw = {
		.used_ports = {
			[0] = {
				.used	= 1,
				.phy_id = 1,
				.name	= "Port 1",
			},
			[1] = {
				.used	= 1,
				.phy_id	= 2,
				.name	= "Port 2",
			},
			[2] = {
				.used	= 1,
				.phy_id	= 3,
				.name	= "Port 3",
			},
			[3] = {
				.used	= 1,
				.phy_id	= 4,
				.name	= "Port 4",
			},
		},
	},

	.leds = {
		/* front LEDs */
		{
			.name			= "F@ST2704V2:green:power",
			.gpio			= 4,
			.active_low		= 1,
			.default_trigger	= "default-on",
		},
		{
			.name			= "F@ST2704V2:red:power",
			.gpio			= 5,
			.active_low		= 1,
		},
		{
			.name			= "F@ST2704V2:red:inet",
			.gpio			= 2,
			.active_low		= 1,
		},
		{
			.name			= "F@ST2704V2:green:dsl",
			.gpio			= 3,
			.active_low		= 1,
		},
		{
			.name			= "F@ST2704V2:green:inet",
			.gpio			= 11,
			.active_low		= 1,
		},
		{
			.name			= "F@ST2704V2:green:usb",
			.gpio			= 1,
			.active_low		= 1,
		},

		/* side button LEDs */
		{
			.name			= "F@ST2704V2:green:wps",
			.gpio			= 10,
			.active_low		= 1,
		},

		/* FIXME: can't control gpio0 line in "out" state, needs further investigation */
		/*
		{
			.name			= "F@ST2704V2:green:rfkill",
			.gpio			= 0,
			.active_low		= 1,
		},
		*/

	},
	.buttons = {
		{
			.desc			= "reset",
			.gpio			= 23,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_RESTART,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc			= "wps",
			.gpio			= 24,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_WPS_BUTTON,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc			= "rfkill",
			.gpio			= 15,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_WLAN,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};
#endif

/*
 * known 6338 boards
 */
#ifdef CONFIG_BCM63XX_CPU_6338
static struct board_info __initdata board_96338gw = {
	.name				= "96338GW",
	.expected_cpu_id		= 0x6338,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.enet0 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.has_ohci0			= 1,

	.leds = {
		{
			.name		= "96338GW:green:adsl",
			.gpio		= 3,
			.active_low	= 1,
		},
		{
			.name		= "96338GW:green:ses",
			.gpio		= 5,
			.active_low	= 1,
		},
		{
			.name		= "96338GW:green:ppp-fail",
			.gpio		= 4,
			.active_low	= 1,
		},
		{
			.name		= "96338GW:green:power",
			.gpio		= 0,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "96338GW:green:stop",
			.gpio		= 1,
			.active_low	= 1,
		}
	},
};

static struct board_info __initdata board_96338w = {
	.name				= "96338W",
	.expected_cpu_id		= 0x6338,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.enet0 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.leds = {
		{
			.name		= "96338W:green:adsl",
			.gpio		= 3,
			.active_low	= 1,
		},
		{
			.name		= "96338W:green:ses",
			.gpio		= 5,
			.active_low	= 1,
		},
		{
			.name		= "96338W:green:ppp-fail",
			.gpio		= 4,
			.active_low	= 1,
		},
		{
			.name		= "96338W:green:power",
			.gpio		= 0,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "96338W:green:stop",
			.gpio		= 1,
			.active_low	= 1,
		},
	},
};

static struct board_info __initdata board_96338w2_e7t = {
	.name				= "96338W2_E7T",
	.expected_cpu_id		= 0x6338,

	.has_enet0			= 1,

	.enet0 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

		.leds = {
		{
			.name		= "96338W2_E7T:green:ppp",
			.gpio		= 4,
			.active_low	= 1,
		},
		{
			.name		= "96338W2_E7T:green:ppp-fail",
			.gpio		= 5,
			.active_low	= 1,
		},
		{
			.name		= "96338W2_E7T:green:power",
			.gpio		= 0,
			.active_low	= 1,
			.default_trigger = "default-on",

		},
	},
};

static struct board_info __initdata board_rta1320_16m = {
	.name				= "RTA1320_16M",
	.expected_cpu_id		= 0x6338,

	.has_uart0			= 1,
	.has_enet0			= 1,

	.enet0 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

		.leds = {
		{
			.name		= "RTA1320_16M:green:adsl",
			.gpio		= 3,
			.active_low	= 1,
		},
		{
			.name		= "RTA1320_16M:green:ppp",
			.gpio		= 4,
			.active_low	= 1,
		},
		{
			.name		= "RTA1320_16M:green:power",
			.gpio		= 0,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "RTA1320_16M:green:stop",
			.gpio		= 1,
			.active_low	= 1,
		},
	},
};

// Actually this board is the very same as the rta770bw,
// where the additional 'b' within the name just
// just indicates 'Annex B'. The ADSL Modem itself is able
// to handle both Annex A as well as Annex B -
// the loaded firmware makes the only difference
static struct board_info __initdata board_rta770w = {
	.name				= "RTA770W",
	.expected_cpu_id		= 0x6345,

	.has_uart0			= 1,

	.has_enet0			= 1,

	.enet0 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.leds = {
		{
			.name		= "RTA770W:green:usb",
			.gpio		= 7,
			.active_low	= 1,
		},
		{
			.name		= "RTA770W:green:adsl",
			.gpio		= 8,
		},
		{
			.name		= "RTA770W:green:diag",
			.gpio		= 10,
			.active_low	= 1,
		},
		{
			.name		= "RTA770W:green:wlan",
			.gpio		= 11,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 13,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};
#endif

/*
 * known 6345 boards
 */
#ifdef CONFIG_BCM63XX_CPU_6345
static struct board_info __initdata board_96345gw2 = {
	.name				= "96345GW2",
	.expected_cpu_id		= 0x6345,

	.has_uart0			= 1,
};

static struct board_info __initdata board_rta770bw = {
	.name				= "RTA770BW",
	.expected_cpu_id		= 0x6345,

	.has_uart0			= 1,

	.has_enet0			= 1,

	.enet0 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.leds = {
		{
			.name		= "RTA770BW:green:usb",
			.gpio		= 7,
			.active_low	= 1,
		},
		{
			.name		= "RTA770BW:green:adsl",
			.gpio		= 8,
		},
		{
			.name		= "RTA770BW:green:diag",
			.gpio		= 10,
			.active_low	= 1,
		},
		{
			.name		= "RTA770BW:green:wlan",
			.gpio		= 11,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 13,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};
#endif

/*
 * known 6348 boards
 */
#ifdef CONFIG_BCM63XX_CPU_6348
static struct board_info __initdata board_96348r = {
	.name				= "96348R",
	.expected_cpu_id		= 0x6348,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},

	.leds = {
		{
			.name		= "96348R:green:adsl-fail",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "96348R:green:ppp",
			.gpio		= 3,
			.active_low	= 1,
		},
		{
			.name		= "96348R:green:ppp-fail",
			.gpio		= 4,
			.active_low	= 1,
		},
		{
			.name		= "96348R:green:power",
			.gpio		= 0,
			.active_low	= 1,
			.default_trigger = "default-on",

		},
		{
			.name		= "96348R:green:stop",
			.gpio		= 1,
			.active_low	= 1,
		},
	},
};

static struct board_info __initdata board_96348gw_10 = {
	.name				= "96348GW-10",
	.expected_cpu_id		= 0x6348,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},
	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.has_ohci0			= 1,
	.has_pccard			= 1,

	.has_dsp			= 1,
	.dsp = {
		.gpio_rst		= 6,
		.gpio_int		= 34,
		.cs			= 2,
		.ext_irq		= 2,
	},

	.leds = {
		{
			.name		= "96348GW-10:green:adsl-fail",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "96348GW-10:green:ppp",
			.gpio		= 3,
			.active_low	= 1,
		},
		{
			.name		= "96348GW-10:green:ppp-fail",
			.gpio		= 4,
			.active_low	= 1,
		},
		{
			.name		= "96348GW-10:green:power",
			.gpio		= 0,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "96348GW-10:green:stop",
			.gpio		= 1,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 6,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

static struct board_info __initdata board_96348gw_11 = {
	.name				= "96348GW-11",
	.expected_cpu_id		= 0x6348,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},


	.has_ohci0 = 1,
	.has_pccard = 1,

	.leds = {
		{
			.name		= "96348GW-11:green:adsl-fail",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "96348GW-11:green:ppp",
			.gpio		= 3,
			.active_low	= 1,
		},
		{
			.name		= "96348GW-11:green:ppp-fail",
			.gpio		= 4,
			.active_low	= 1,
		},
		{
			.name		= "96348GW-11:green:power",
			.gpio		= 0,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "96348GW-11:green:stop",
			.gpio		= 1,
			.active_low	= 1,
		},
	},
	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 33,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};


/* BT Voyager 2110 */
static struct board_info __initdata board_V2110 = {
	.name				= "V2110",
	.expected_cpu_id		= 0x6348,

	.has_uart0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.leds = {
		{
			.name           = "V2110:green:power",
			.gpio           = 0,
			.active_low     = 1,
			.default_trigger = "default-on",
		},
		{
			.name           = "V2110:red:power",
			.gpio           = 1,
			.active_low     = 1,
		},
		{
			.name           = "V2110:green:adsl",
			.gpio           = 2,
			.active_low     = 1,
		},
		{       .name           = "V2110:green:ppp",
			.gpio           = 3,
			.active_low     = 1,
		},
		{
			.name           = "V2110:green:wireless",
			.gpio           = 6,
			.active_low     = 1,
		},
	},

	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 33,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};


static struct board_info __initdata board_ct536_ct5621 = {
	.name				= "CT536_CT5621",
	.expected_cpu_id		= 0x6348,

	.has_uart0			= 1,
	.has_enet0			= 0,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.has_ohci0 = 1,
	.has_pccard = 1,

	.leds = {
		{
			.name		= "CT536_CT5621:green:adsl-fail",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "CT536_CT5621:green:power",
			.gpio		= 0,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
	},
	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 33,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

static struct board_info __initdata board_CT5365 = {
	.name				= "96348A-122",
	.expected_cpu_id		= 0x6348,

	.has_uart0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.has_ohci0 = 1,

	.leds = {
		{
			.name		= "CT-5365:green:power",
			.gpio		= 0,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "CT-5365:red:alarm",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "CT-5365:green:wps",
			.gpio		= 6,
			.active_low	= 1,
		},
	},
	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 33,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "wlan",
			.gpio		= 34,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_WLAN,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "wps",
			.gpio		= 35,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_WPS_BUTTON,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

static struct board_info __initdata board_96348gw = {
	.name				= "96348GW",
	.expected_cpu_id		= 0x6348,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},
	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.has_ohci0 = 1,

	.has_dsp			= 1,
	.dsp = {
		.gpio_rst		= 6,
		.gpio_int		= 34,
		.ext_irq		= 2,
		.cs			= 2,
	},

	.leds = {
		{
			.name		= "96348GW:green:adsl-fail",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "96348GW:green:ppp",
			.gpio		= 3,
			.active_low	= 1,
		},
		{
			.name		= "96348GW:green:ppp-fail",
			.gpio		= 4,
			.active_low	= 1,
		},
		{
			.name		= "96348GW:green:power",
			.gpio		= 0,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "96348GW:green:stop",
			.gpio		= 1,
			.active_low	= 1,
		},
	},
	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 36,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

static struct board_info __initdata board_gw6200 = {
	.name				= "GW6200",
	.expected_cpu_id		= 0x6348,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},
	.enet1 = {
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.has_ohci0 = 1,

	.has_dsp			= 1,
		.dsp = {
		.gpio_rst		= 8, /* FIXME: What is real GPIO here? */
		.gpio_int		= 34,
		.ext_irq		= 2,
		.cs			= 2,
	},

	.leds = {
		{
			.name		= "GW6200:green:line1",
			.gpio		= 4,
			.active_low	= 1,
		},
		{
			.name		= "GW6200:green:line2",
			.gpio		= 5,
			.active_low	= 1,
		},
		{
			.name		= "GW6200:green:line3",
			.gpio		= 6,
			.active_low	= 1,
		},
		{
			.name		= "GW6200:green:tel",
			.gpio		= 7,
			.active_low	= 1,
		},
	},
	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 36,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

static struct board_info __initdata board_gw6000 = {
	.name				= "GW6000",
	.expected_cpu_id		= 0x6348,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},
	.enet1 = {
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.has_ohci0 = 1,

	.has_dsp			= 1,
	.dsp = {
		.gpio_rst		= 6,
		.gpio_int		= 34,
		.ext_irq		= 2,
		.cs			= 2,
	},

	/* GW6000 has no GPIO-controlled leds */

	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 36,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};



static struct board_info __initdata board_FAST2404 = {
	.name				= "F@ST2404",
	.expected_cpu_id		= 0x6348,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.has_ohci0			= 1,
	.has_pccard			= 1,
};

static struct board_info __initdata board_FAST2604 = {
	.name						= "F@ST2604",
	.expected_cpu_id			= 0x6348,

	.has_uart0					= 1,
	.has_pci					= 1,
	.has_ohci0					= 1,

	.has_enet1					= 1,
	.enet1 = {
		.has_phy			= 1,
		.phy_id				= 0,
		.force_speed_100		= 1,
		.force_duplex_full		= 1,
	},

	.leds = {
		{
			.name				= "F@ST2604:green:power",
			.gpio				= 0,
			.active_low			= 1,
			.default_trigger	= "default-on",
		},
		{
			.name				= "F@ST2604:red:power",
			.gpio				= 1,
			.active_low			= 1,
		},
		{
			.name				= "F@ST2604:red:inet",
			.gpio				= 4,
			.active_low			= 1,
		},
		{
			.name				= "F@ST2604:green:wps",
			.gpio				= 5,
			.active_low			= 1,
		},
	},
	.buttons = {
		{
			.desc				= "reset",
			.gpio				= 33,
			.active_low			= 1,
			.type				= EV_KEY,
			.code				= KEY_RESTART,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

static struct board_info __initdata board_rta1025w_16 = {
	.name				= "RTA1025W_16",
	.expected_cpu_id		= 0x6348,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},
	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.has_phy		= 1,
		.phy_id			= 0,
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},
};


static struct board_info __initdata board_DV201AMR = {
	.name				= "DV201AMR",
	.expected_cpu_id		= 0x6348,

	.has_uart0			= 1,
	.has_pci			= 1,
	.has_ohci0			= 1,

	.has_enet0			= 1,
	.has_enet1			= 1,
	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},
	.enet1 = {
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},
};

static struct board_info __initdata board_96348gw_a = {
	.name				= "96348GW-A",
	.expected_cpu_id		= 0x6348,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},
	.enet1 = {
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.has_ohci0 = 1,

	.leds = {
		{
			.name		= "96348GW-A::adsl",
			.gpio		= 3,
			.active_low	= 1,
		},
		{
			.name		= "96348GW-A::usb",
			.gpio		= 0,
			.active_low	= 1,
		}
	},
};

static struct board_info __initdata board_96348_D4PW = {
	.name				= "D-4P-W",
	.expected_cpu_id		= 0x6348,

	.has_enet1			= 1,
	.has_pci			= 1,
	.has_uart0 			= 1,

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.leds = {
		{
			.name		= "D-4P-W:green:power",
			.gpio		= 0,
			.active_low	= 1,
		},
		{
			.name		= "D-4P-W::status",
			.gpio		= 3,
			.active_low	= 1,
		},
		{
			.name		= "D-4P-W:green:internet",
			.gpio		= 4,
			.active_low	= 1,
		},
		{
			.name		= "D-4P-W:red:internet",
			.gpio		= 5,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 7,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

static struct board_info __initdata board_spw500v = {
	.name				= "SPW500V",
	.expected_cpu_id		= 0x6348,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},

	.has_dsp			= 1,
	.dsp = {
		.gpio_rst		= 6,
		.gpio_int		= 34,
		.ext_irq		= 2,
		.cs			= 2,
	},

	.leds = {
		{
			.name		= "SPW500V:red:power",
			.gpio		= 1,
			.active_low	= 1,
		},
		{
			.name		= "SPW500V:green:power",
			.gpio		= 0,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "SPW500V:green:ppp",
			.gpio		= 3,
			.active_low	= 1,
		},
		{	.name		= "SPW500V:green:pstn",
			.gpio		= 28,
			.active_low	= 1,
		},
		{
			.name		= "SPW500V:green:voip",
			.gpio		= 32,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 33,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

static struct board_info __initdata board_96348sv = {
	.name				= "MAGIC",
	.expected_cpu_id		= 0x6348,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},
	.enet1 = {
		/* it has BP_ENET_EXTERNAL_PHY */
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.has_ohci0			= 1,
	.has_pccard			= 1,

	.has_dsp			= 1,
	.dsp = {
		.gpio_rst		= 25,
		.gpio_int		= 34,
		.cs			= 2,
		.ext_irq		= 2,
	},

	.leds = {
		{
			.name		= "MAGIC:green:voip",
			.gpio		= 22,
			.active_low	= 1,
		},
		{
			.name		= "MAGIC:green:adsl",
			.gpio		= 5,
			.active_low	= 1,
		},
		{
			.name		= "MAGIC:green:wifi",
			.gpio		= 28,
		},
		{
			.name		= "MAGIC:green:usb",
			.gpio		= 35,
			.active_low	= 1,
		},
		{
			.name		= "MAGIC:green:hpna",
			.gpio		= 4,
			.active_low	= 1,
		},
		{
			.name		= "MAGIC:green:power",
			.gpio		= 0,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "MAGIC:green:stop",
			.gpio		= 1,
			.active_low	= 1,
		},
	},
};

static struct board_info __initdata board_V2500V_BB = {
	.name				= "V2500V_BB",
        .expected_cpu_id                = 0x6348,

        .has_uart0                      = 1,
        .has_enet0                      = 1,
        .has_enet1                      = 1,
        .has_pci                        = 1,

        .enet0 = {
                .has_phy                = 1,
                .use_internal_phy       = 1,
        },
        .enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
                .force_speed_100        = 1,
                .force_duplex_full      = 1,
	},

	.leds =	{
		{
			.name		= "V2500V_BB:green:power",
			.gpio		= 0,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "V2500V_BB:red:power",
			.gpio		= 1,
			.active_low	= 1,
		},
		{
			.name		= "V2500V_BB:green:adsl",
			.gpio		= 2,
			.active_low	= 1,
		},
		{	.name		= "V2500V_BB:green:ppp",
			.gpio		= 3,
			.active_low	= 1,
		},
		{
			.name		= "V2500V_BB:green:wireless",
			.gpio		= 6,
			.active_low	= 1,
		},
	},
	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 31,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

static struct board_info __initdata board_CPVA502plus = {
	.name				= "CPVA502+",
	.expected_cpu_id		= 0x6348,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},
	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.leds = {
		{
			.name		= "CPVA502+:green:phone",
			.gpio		= 0,
			.active_low	= 1,
		},
		{
			.name		= "CPVA502+:green:link",
			.gpio		= 5,
			.active_low	= 1,
		},
		{
			.name		= "CPVA502+:green:feth1", /* FIXME:does gpio4 enable eth1 phy? */
			.gpio		= 4,
			.active_low	= 1,
		},
	},
};

/* NetGear DG834G v4 */
static struct board_info __initdata board_96348W3 = {
 	.name	 			= "96348W3",
	.expected_cpu_id		= 0x6348,

	.has_uart0	 		= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.has_ohci0	 		= 1,

	.leds = {
		{
			.name		= "96348W3:green:power",
			.gpio		= 0,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "96348W3:red:power",
			.gpio		= 1,
			.active_low	= 1,
		},
		{
			.name		= "96348W3::adsl",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "96348W3::internet",
			.gpio		= 3,
			.active_low	= 1,
		},
	},
	.buttons = {
			{
			.desc		= "reset",
			.gpio		= 6,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
			},
	},
};

static struct board_info __initdata board_96348gw_10_AR1004G = {
	.name				= "AR1004G",
	.expected_cpu_id		= 0x6348,

	.has_uart0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,
	
	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.leds = {
		{
			.name		= "AR1004G:green:inet",
			.gpio		= 3,
			.active_low	= 1,
		},
		{
			.name		= "AR1004G:green:power",
			.gpio		= 0,
			.active_low	= 1,
		},
		{
			.name		= "AR1004G:red:power",
			.gpio		= 6,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 33,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

#endif

/*
 * known 6358 boards
 */
#ifdef CONFIG_BCM63XX_CPU_6358
static struct board_info __initdata board_96358vw = {
	.name				= "96358VW",
	.expected_cpu_id		= 0x6358,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},


	.has_ohci0 = 1,
	.has_pccard = 1,
	.has_ehci0 = 1,

	.leds = {
		{
			.name		= "96358VW:green:adsl-fail",
			.gpio		= 15,
			.active_low	= 1,
		},
		{
			.name		= "96358VW:green:ppp",
			.gpio		= 22,
			.active_low	= 1,
		},
		{
			.name		= "96358VW:green:ppp-fail",
			.gpio		= 23,
			.active_low	= 1,
		},
		{
			.name		= "96358VW:green:power",
			.gpio		= 4,
			.default_trigger = "default-on",
		},
		{
			.name		= "96358VW:green:stop",
			.gpio		= 5,
		},
	},
};

static struct board_info __initdata board_96358vw2 = {
	.name				= "96358VW2",
	.expected_cpu_id		= 0x6358,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},


	.has_ohci0 = 1,
	.has_pccard = 1,
	.has_ehci0 = 1,
	.num_usbh_ports			= 2,

	.leds = {
		{
			.name		= "96358VW2:green:adsl",
			.gpio		= 22,
			.active_low	= 1,
		},
		{
			.name		= "96358VW2:green:ppp-fail",
			.gpio		= 23,
		},
		{
			.name		= "96358VW2:green:power",
			.gpio		= 5,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "96358VW2:green:stop",
			.gpio		= 4,
			.active_low	= 1,
		},
	},
};

static struct board_info __initdata board_CPVA642 = {
	.name                           = "CPVA642",
	.expected_cpu_id                = 0x6358,

	.has_uart0			= 1,
	.has_enet1                      = 1,
	.has_pci                        = 1,

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100        = 1,
		.force_duplex_full      = 1,
 	},

	.has_ohci0 = 1,
	.has_ehci0 = 1,

	.leds = {
		{
			.name		= "CPVA642:red:power",
			.gpio		= 14,
			.active_low	= 1,
		},
		{
			.name		= "CPVA642:green:power",
			.gpio		= 11,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "CPVA642:red:wifi",
			.gpio		= 6,
			.active_low	= 1,
		},
		{
			.name		= "CPVA642:green:wifi",
			.gpio		= 28,
			.active_low	= 0,
		},
		{
			.name		= "CPVA642:red:link",
			.gpio		= 9,
			.active_low	= 1,
		},
		{
			.name		= "CPVA642:green:link",
			.gpio		= 10,
			.active_low	= 1,
		},
		{
			.name		= "CPVA642:green:ether",
			.gpio		= 1,
			.active_low	= 1,
		},
		{
			.name		= "CPVA642:green:phone1",
			.gpio		= 4,
			.active_low	= 1,
		},
		{
			.name		= "CPVA642:green:phone2",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "CPVA642:green:usb",
			.gpio		= 3,
			.active_low	= 1,
		},
    },

	.buttons = {
		{
			.desc           = "reset",
			.gpio           = 36,
			.active_low     = 1,
			.type           = EV_KEY,
			.code           = KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "wps",
			.gpio		= 37,
			.type		= EV_KEY,
			.code		= KEY_WPS_BUTTON,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};


static struct board_info __initdata board_AGPFS0 = {
	.name				= "AGPF-S0",
	.expected_cpu_id		= 0x6358,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.has_ohci0 = 1,
	.has_ehci0 = 1,

	.leds = {
		{
			.name		= "AGPF-S0:red:power",
			.gpio		= 5,
			.active_low	= 1,
		},
		{
			.name		= "AGPF-S0:green:power",
			.gpio		= 4,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "AGPF-S0:red:service",
			.gpio		= 7,
			.active_low	= 1,
		},
		{
			.name		= "AGPF-S0:green:service",
			.gpio		= 6,
			.active_low	= 1,
		},
		{
			.name		= "AGPF-S0:green:adsl",
			.gpio		= 9,
			.active_low	= 1,
		},
		{
			.name		= "AGPF-S0:red:adsl",
			.gpio		= 10,
			.active_low	= 1,
		},
		{
			.name		= "AGPF-S0:red:wifi",
			.gpio		= 23,
			.active_low	= 1,
		},
		{
			.name		= "AGPF-S0:green:wifi",
			.gpio		= 22,
			.active_low	= 1,
		},
		{
			.name		= "AGPF-S0:green:internet",
			.gpio		= 25,
			.active_low	= 1,
		},
		{
			.name		= "AGPF-S0:red:internet",
			.gpio		= 24,
			.active_low	= 1,
		},
		{
			.name		= "AGPF-S0:red:usr1",
			.gpio		= 27,
			.active_low	= 1,
		},
		{
			.name		= "AGPF-S0:green:usr1",
			.gpio		= 26,
			.active_low	= 1,
		},
		{
			.name		= "AGPF-S0:red:usr2",
			.gpio		= 30,
			.active_low	= 1,
		},
		{
			.name		= "AGPF-S0:green:usr2",
			.gpio		= 29,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc           = "reset",
			.gpio           = 37,
			.active_low     = 1,
			.type           = EV_KEY,
			.code           = KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "wps",
			.gpio		= 34,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_WPS_BUTTON,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

static struct board_info __initdata board_DWVS0 = {
	.name				= "DWV-S0",
	.expected_cpu_id		= 0x6358,

	.has_uart0			= 1,

	.has_enet0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.has_ohci0			= 1,
	.has_ehci0			= 1,

	.leds = {
		{
			.name		= "DWV-S0:red:power",
			.gpio		= 5,
			.active_low	= 1,
		},
		{
			.name		= "DWV-S0:green:power",
			.gpio		= 4,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "DWV-S0:red:internet",
			.gpio		= 6,
			.active_low	= 1,
		},
		{
			.name		= "DWV-S0:green:internet",
			.gpio		= 7,
			.active_low	= 1,
		},
		{
			.name		= "DWV-S0:green:ADSL",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "DWV-S0:red:ADSL",
			.gpio		= 12,
			.active_low	= 1,
		},
		{
			.name		= "DWV-S0:red:wifi",
			.gpio		= 10,
			.active_low	= 1,
		},
		{
			.name		= "DWV-S0:green:VoIP",
			.gpio		= 9,
			.active_low	= 1,
		},
		{
			.name		= "DWV-S0:red:VoIP",
			.gpio		= 0,
			.active_low	= 1,
		},
		{
			.name		= "DWV-S0:red:ethernet",
			.gpio		= 1,
			.active_low	= 1,
		},
		{
			.name		= "DWV-S0:green:ethernet",
			.gpio		= 8,
			.active_low	= 1,
		},
		{
			.name		= "DWV-S0:red:USB",
			.gpio		= 11,
			.active_low	= 1,
		},
		{
			.name		= "DWV-S0:green:USB",
			.gpio		= 3,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 37,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "wps",
			.gpio		= 34,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_WPS_BUTTON,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

/* D-Link DSL-274xB revison C2/C3 */
static struct board_info __initdata board_dsl_274xb_rev_c = {
	.name				= "AW4139",
	.expected_cpu_id		= 0x6358,

	.has_uart0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.leds = {
		{
			.name		= "dsl-274xb:green:power",
			.gpio		= 5,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "dsl-274xb:red:power",
			.gpio		= 4,
			.active_low	= 1,
		},
		{
			.name		= "dsl-274xb:green:adsl",
			.gpio		= 9,
			.active_low	= 1,
		},
		{
			.name		= "dsl-274xb:green:internet",
			.gpio		= 2,
		},
		{
			.name		= "dsl-274xb:red:internet",
			.gpio		= 10,
		},
	},

	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 34,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

struct spi_gpio_platform_data nb4_spi_gpio_data = {
	.sck		= NB4_SPI_GPIO_CLK,
	.mosi		= NB4_SPI_GPIO_MOSI,
	.miso		= SPI_GPIO_NO_MISO,
	.num_chipselect	= 1,
};


static struct platform_device nb4_spi_gpio = {
	.name = "spi_gpio",
	.id   = 1,
	.dev = {
		.platform_data = &nb4_spi_gpio_data,
	},
};

static struct platform_device * __initdata nb4_devices[] = {
	&nb4_spi_gpio,
};

const struct gen_74x164_chip_platform_data nb4_74x164_platform_data = {
	.base = NB4_74X164_GPIO_BASE
};

static struct spi_board_info nb4_spi_devices[] = {
	{
		.modalias = "74x164",
		.max_speed_hz = 781000,
		.bus_num = 1,
		.controller_data = (void *) SPI_GPIO_NO_CHIPSELECT,
		.mode = SPI_MODE_0,
		.platform_data = &nb4_74x164_platform_data
	}
};

static struct board_info __initdata board_nb4_ser_r0 = {
	.name				= "NB4-SER-r0",
	.expected_cpu_id		= 0x6358,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},


	.has_ohci0 = 1,
	.has_pccard = 1,
	.has_ehci0 = 1,
	.num_usbh_ports			= 2,

	.leds = {
		{
			.name		= "NB4-SER-r0:white:adsl",
			.gpio		= NB4_74HC64_GPIO(4),
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r0:white:traffic",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r0:white:tel",
			.gpio		= NB4_74HC64_GPIO(3),
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r0:white:tv",
			.gpio		= NB4_74HC64_GPIO(2),
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r0:white:wifi",
			.gpio		= 15,
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r0:white:alarm",
			.gpio		= NB4_74HC64_GPIO(0),
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r0:red:service",
			.gpio		= 29,
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r0:green:service",
			.gpio		= 30,
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r0:blue:service",
			.gpio		= 4,
			.active_low	= 1,
		},
	},
	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 34,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "wps",
			.gpio		= 37,
			.type		= EV_KEY,
			.code		= KEY_WPS_BUTTON,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "service",
			.gpio		= 27,
			.type		= EV_KEY,
			.code		= BTN_0,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "clip",
			.gpio		= 31,
			.type		= EV_KEY,
			.code		= BTN_1,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
	.devs = nb4_devices,
	.num_devs = ARRAY_SIZE(nb4_devices),
	.spis = nb4_spi_devices,
	.num_spis = ARRAY_SIZE(nb4_spi_devices),
};

static struct board_info __initdata board_nb4_ser_r1 = {
	.name				= "NB4-SER-r1",
	.expected_cpu_id		= 0x6358,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},


	.has_ohci0 = 1,
	.has_pccard = 1,
	.has_ehci0 = 1,
	.num_usbh_ports			= 2,

	.leds = {
		{
			.name		= "NB4-SER-r1:white:adsl",
			.gpio		= NB4_74HC64_GPIO(4),
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r1:white:traffic",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r1:white:tel",
			.gpio		= NB4_74HC64_GPIO(3),
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r1:white:tv",
			.gpio		= NB4_74HC64_GPIO(2),
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r1:white:wifi",
			.gpio		= 15,
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r1:white:alarm",
			.gpio		= NB4_74HC64_GPIO(0),
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r1:red:service",
			.gpio		= 29,
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r1:green:service",
			.gpio		= 30,
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r1:blue:service",
			.gpio		= 4,
			.active_low	= 1,
		},
	},
	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 34,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "wps",
			.gpio		= 37,
			.type		= EV_KEY,
			.code		= KEY_WPS_BUTTON,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "service",
			.gpio		= 27,
			.type		= EV_KEY,
			.code		= BTN_0,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "clip",
			.gpio		= 31,
			.type		= EV_KEY,
			.code		= BTN_1,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
	.devs = nb4_devices,
	.num_devs = ARRAY_SIZE(nb4_devices),
	.spis = nb4_spi_devices,
	.num_spis = ARRAY_SIZE(nb4_spi_devices),
};

static struct board_info __initdata board_nb4_ser_r2 = {
	.name				= "NB4-SER-r2",
	.expected_cpu_id		= 0x6358,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},


	.has_ohci0 = 1,
	.has_pccard = 1,
	.has_ehci0 = 1,
	.num_usbh_ports			= 2,

	.leds = {
		{
			.name		= "NB4-SER-r2:white:adsl",
			.gpio		= NB4_74HC64_GPIO(4),
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r2:white:traffic",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r2:white:tel",
			.gpio		= NB4_74HC64_GPIO(3),
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r2:white:tv",
			.gpio		= NB4_74HC64_GPIO(2),
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r2:white:wifi",
			.gpio		= 15,
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r2:white:alarm",
			.gpio		= NB4_74HC64_GPIO(0),
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r2:red:service",
			.gpio		= 29,
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r2:green:service",
			.gpio		= 30,
			.active_low	= 1,
		},
		{
			.name		= "NB4-SER-r2:blue:service",
			.gpio		= 4,
			.active_low	= 1,
		},
	},
	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 34,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "wps",
			.gpio		= 37,
			.type		= EV_KEY,
			.code		= KEY_WPS_BUTTON,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "service",
			.gpio		= 27,
			.type		= EV_KEY,
			.code		= BTN_0,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "clip",
			.gpio		= 31,
			.type		= EV_KEY,
			.code		= BTN_1,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
	.devs = nb4_devices,
	.num_devs = ARRAY_SIZE(nb4_devices),
	.spis = nb4_spi_devices,
	.num_spis = ARRAY_SIZE(nb4_spi_devices),
};

static struct board_info __initdata board_nb4_fxc_r1 = {
	.name				= "NB4-FXC-r1",
	.expected_cpu_id		= 0x6358,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},


	.has_ohci0 = 1,
	.has_pccard = 1,
	.has_ehci0 = 1,
	.num_usbh_ports			= 2,

	.leds = {
		{
			.name		= "NB4-FXC-r1:white:adsl",
			.gpio		= NB4_74HC64_GPIO(4),
			.active_low	= 1,
		},
		{
			.name		= "NB4-FXC-r1:white:traffic",
			.gpio		= 2,
		},
		{
			.name		= "NB4-FXC-r1:white:tel",
			.gpio		= NB4_74HC64_GPIO(3),
			.active_low	= 1,
		},
		{
			.name		= "NB4-FXC-r1:white:tv",
			.gpio		= NB4_74HC64_GPIO(2),
			.active_low	= 1,
		},
		{
			.name		= "NB4-FXC-r1:white:wifi",
			.gpio		= 15,
		},
		{
			.name		= "NB4-FXC-r1:white:alarm",
			.gpio		= NB4_74HC64_GPIO(0),
			.active_low	= 1,
		},
		{
			.name		= "NB4-FXC-r1:red:service",
			.gpio		= 29,
		},
		{
			.name		= "NB4-FXC-r1:green:service",
			.gpio		= 30,
		},
		{
			.name		= "NB4-FXC-r1:blue:service",
			.gpio		= 4,
		},
	},
	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 34,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "wps",
			.gpio		= 37,
			.type		= EV_KEY,
			.code		= KEY_WPS_BUTTON,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "service",
			.gpio		= 27,
			.type		= EV_KEY,
			.code		= BTN_0,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "clip",
			.gpio		= 31,
			.type		= EV_KEY,
			.code		= BTN_1,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
	.devs = nb4_devices,
	.num_devs = ARRAY_SIZE(nb4_devices),
	.spis = nb4_spi_devices,
	.num_spis = ARRAY_SIZE(nb4_spi_devices),
};

static struct board_info __initdata board_nb4_fxc_r2 = {
	.name				= "NB4-FXC-r2",
	.expected_cpu_id		= 0x6358,

	.has_uart0			= 1,
	.has_enet0			= 1,
	.has_enet1			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},


	.has_ohci0 = 1,
	.has_pccard = 1,
	.has_ehci0 = 1,
	.num_usbh_ports			= 2,

	.leds = {
		{
			.name		= "NB4-FXC-r2:white:adsl",
			.gpio		= NB4_74HC64_GPIO(4),
			.active_low	= 1,
		},
		{
			.name		= "NB4-FXC-r2:white:traffic",
			.gpio		= 2,
		},
		{
			.name		= "NB4-FXC-r2:white:tel",
			.gpio		= NB4_74HC64_GPIO(3),
			.active_low	= 1,
		},
		{
			.name		= "NB4-FXC-r2:white:tv",
			.gpio		= NB4_74HC64_GPIO(2),
			.active_low	= 1,
		},
		{
			.name		= "NB4-FXC-r2:white:wifi",
			.gpio		= 15,
		},
		{
			.name		= "NB4-FXC-r2:white:alarm",
			.gpio		= NB4_74HC64_GPIO(0),
			.active_low	= 1,
		},
		{
			.name		= "NB4-FXC-r2:red:service",
			.gpio		= 29,
		},
		{
			.name		= "NB4-FXC-r2:green:service",
			.gpio		= 30,
		},
		{
			.name		= "NB4-FXC-r2:blue:service",
			.gpio		= 4,
		},
	},
	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 34,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "wps",
			.gpio		= 37,
			.type		= EV_KEY,
			.code		= KEY_WPS_BUTTON,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "service",
			.gpio		= 27,
			.type		= EV_KEY,
			.code		= BTN_0,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "clip",
			.gpio		= 31,
			.type		= EV_KEY,
			.code		= BTN_1,
			.active_low	= 1,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
	.devs = nb4_devices,
	.num_devs = ARRAY_SIZE(nb4_devices),
	.spis = nb4_spi_devices,
	.num_spis = ARRAY_SIZE(nb4_spi_devices),
};


struct spi_gpio_platform_data ct6373_spi_gpio_data = {
	.sck		= CT6373_SPI_GPIO_CLK,
	.mosi		= CT6373_SPI_GPIO_MOSI,
	.miso		= SPI_GPIO_NO_MISO,
	.num_chipselect	= 1,
};

static struct platform_device ct6373_spi_gpio = {
	.name = "spi_gpio",
	.id   = 1,
	.dev = {
		.platform_data = &ct6373_spi_gpio_data,
	},
};

static struct platform_device * __initdata ct6373_devices[] = {
	&ct6373_spi_gpio,
};

const struct gen_74x164_chip_platform_data ct6373_74x164_platform_data = {
	.base = CT6373_74X164_GPIO_BASE
};

static struct spi_board_info ct6373_spi_devices[] = {
	{
		.modalias = "74x164",
		.max_speed_hz = 781000,
		.bus_num = 1,
		.controller_data = (void *) SPI_GPIO_NO_CHIPSELECT,
		.mode = SPI_MODE_0,
		.platform_data = &ct6373_74x164_platform_data
	}
};

static struct board_info __initdata board_ct6373_1 = {
	.name				= "CT6373-1",
	.expected_cpu_id	= 0x6358,

	.has_uart0			= 1,
	.has_pci			= 1,
	.has_ohci0			= 1,
	.has_ehci0			= 1,

	.has_enet1			= 1,
	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.leds = {
		{
			.name		= "CT6373-1:green:power",
			.gpio		= 0,
			.default_trigger = "default-on",
		},
		{
			.name		= "CT6373-1:green:usb",
			.gpio		= 3,
			.active_low	= 1,
		},
		{
			.name		= "CT6373-1:green:wlan",
			.gpio		= 9,
			.active_low	= 1,
		},
		{
			.name		= "CT6373-1:green:adsl",
			.gpio		= CT6373_74HC64_GPIO(0),
			.active_low	= 1,
		},
		{
			.name		= "CT6373-1:green:line",
			.gpio		= CT6373_74HC64_GPIO(1),
			.active_low	= 1,
		},
		{
			.name		= "CT6373-1:green:fxs1",
			.gpio		= CT6373_74HC64_GPIO(2),
			.active_low	= 1,
		},
		{
			.name		= "CT6373-1:green:fxs2",
			.gpio		= CT6373_74HC64_GPIO(3),
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 35,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},

	.devs = ct6373_devices,
	.num_devs = ARRAY_SIZE(ct6373_devices),
	.spis = ct6373_spi_devices,
	.num_spis = ARRAY_SIZE(ct6373_spi_devices),
};

static struct board_info __initdata board_HW520 = {
	.name					= "HW6358GW_B",
	.expected_cpu_id		= 0x6358,

	.has_uart0				= 1,
	.has_pci				= 1,
	.has_ohci0				= 1,
	.has_ehci0				= 1,

	.has_enet1				= 1,
	.enet1 = {
		.has_phy			= 1,
		.phy_id				= 0,
		.force_speed_100        = 1,
		.force_duplex_full      = 1,
	},

	.leds = {
		{
			.name		= "HW520:green:net",
			.gpio		= 32,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 37,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

static struct board_info __initdata board_HW553 = {
	.name                           = "HW553",
	.expected_cpu_id                = 0x6358,

	.has_uart0			= 1,

	.has_enet1                      = 1,
	.has_pci                        = 1,

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100        = 1,
		.force_duplex_full      = 1,
	},

	.has_ohci0 = 1,
	.has_ehci0 = 1,
	.num_usbh_ports			= 2,

	.leds = {
		{
			.name		= "HW553:red:lan",
			.gpio		= 34,
			.active_low	= 1,
		},
		{
			.name		= "HW553:blue:lan",
			.gpio		= 35,
			.active_low	= 1,
		},
		{
			.name		= "HW553:red:adsl",
			.gpio		= 22,
			.active_low	= 1,
		},
		{
			.name		= "HW553:blue:adsl",
			.gpio		= 23,
			.active_low	= 1,
		},
		{
			.name		= "HW553:red:power",
			.gpio		= 5,
			.active_low	= 1,
			.default_trigger = "default-on",
		},

		{
			.name		= "HW553:blue:power",
			.gpio		= 4,
			.active_low	= 1,
		},
		{
			.name		= "HW553:red:wifi",
			.gpio		= 25,
			.active_low	= 1,
		},
		{
			.name		= "HW553:red:internetkey",
			.gpio		= 12,
			.active_low	= 1,
		},
		{
			.name		= "HW553:blue:internetkey",
			.gpio		= 13,
			.active_low	= 1,
		},
	},
};

static struct board_info __initdata board_HW556 = {
	.name					= "HW556",
	.expected_cpu_id			= 0x6358,

	.has_uart0				= 1,
	.has_pci				= 1,
	.has_ohci0				= 1,
	.has_ehci0				= 1,
	.num_usbh_ports				= 2,

	.has_caldata				= 1,
	.caldata = {
		{
			.caldata_offset		= 0xe00000,
		},
	},

	.has_enet1				= 1,
	.enet1 = {
		.has_phy			= 1,
		.phy_id				= 0,
		.force_speed_100		= 1,
		.force_duplex_full		= 1,
	},

	.leds = {
		{
			.name			= "HW556:red:message",
			.gpio			= 0,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:hspa",
			.gpio			= 1,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:dsl",
			.gpio			= 2,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:power",
			.gpio			= 3,
			.active_low		= 1,
			.default_trigger	= "default-on",
		},
		{
			.name			= "HW556:red:all",
			.gpio			= 6,
			.active_low		= 1,
			.default_trigger	= "default-on",
		},
	},

	.buttons = {
		{
			.desc			= "help",
			.gpio			= 8,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_HELP,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc			= "wlan",
			.gpio			= 9,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_WLAN,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc			= "restart",
			.gpio			= 10,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_RESTART,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc			= "reset",
			.gpio			= 11,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_CONFIG,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};
static struct board_info __initdata board_HW556_A = {
	.name					= "HW556_A",
	.expected_cpu_id			= 0x6358,

	.has_uart0				= 1,
	.has_pci				= 1,
	.has_ohci0				= 1,
	.has_ehci0				= 1,
	.num_usbh_ports				= 2,

	.has_caldata				= 1,
	.caldata = {
		{
			.vendor			= PCI_VENDOR_ID_RALINK,
			.caldata_offset		= 0xeffe00,
			.slot			= 1,
			.eeprom			= "rt2x00.eeprom",
		},
	},

	.has_enet1				= 1,
	.enet1 = {
		.has_phy			= 1,
		.phy_id				= 0,
		.force_speed_100		= 1,
		.force_duplex_full		= 1,
	},

	.leds = {
		{
			.name			= "HW556:green:lan1",
			.gpio			= 0,
			.active_low		= 1,
		},
		{
			.name			= "HW556:green:lan2",
			.gpio			= 1,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:dsl",
			.gpio			= 2,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:power",
			.gpio			= 3,
			.active_low		= 1,
			.default_trigger	= "default-on",
		},
		{
			.name			= "HW556:red:message",
			.gpio			= 12,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:lan1",
			.gpio			= 13,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:hspa",
			.gpio			= 15,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:lan2",
			.gpio			= 22,
			.active_low		= 1,
		},
		{
			.name			= "HW556:green:lan3",
			.gpio			= 23,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:lan3",
			.gpio			= 26,
			.active_low		= 1,
		},
		{
			.name			= "HW556:green:lan4",
			.gpio			= 27,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:lan4",
			.gpio			= 28,
			.active_low		= 1,
		},
	},

	.buttons = {
		{
			.desc			= "help",
			.gpio			= 8,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_HELP,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc			= "wlan",
			.gpio			= 9,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_WLAN,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc			= "restart",
			.gpio			= 10,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_RESTART,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc			= "reset",
			.gpio			= 11,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_CONFIG,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};
static struct board_info __initdata board_HW556_B = {
	.name					= "HW556_B",
	.expected_cpu_id			= 0x6358,

	.has_uart0				= 1,
	.has_pci				= 1,
	.has_ohci0				= 1,
	.has_ehci0				= 1,
	.num_usbh_ports				= 2,

	.has_caldata				= 1,
	.caldata = {
		{
			.vendor			= PCI_VENDOR_ID_ATHEROS,
			.caldata_offset		= 0xf7e000,
			.slot			= 1,
			.endian_check		= 1,
			.led_pin		= 2,
		},
	},

	.has_enet1				= 1,
	.enet1 = {
		.has_phy			= 1,
		.phy_id				= 0,
		.force_speed_100		= 1,
		.force_duplex_full		= 1,
	},

	.leds = {
		{
			.name			= "HW556:red:message",
			.gpio			= 0,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:hspa",
			.gpio			= 1,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:dsl",
			.gpio			= 2,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:power",
			.gpio			= 3,
			.active_low		= 1,
			.default_trigger	= "default-on",
		},
		{
			.name			= "HW556:red:all",
			.gpio			= 6,
			.active_low		= 1,
			.default_trigger	= "default-on",
		},
		{
			.name			= "HW556:green:lan1",
			.gpio			= 12,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:lan1",
			.gpio			= 13,
			.active_low		= 1,
		},
		{
			.name			= "HW556:green:lan2",
			.gpio			= 15,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:lan2",
			.gpio			= 22,
			.active_low		= 1,
		},
		{
			.name			= "HW556:green:lan3",
			.gpio			= 23,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:lan3",
			.gpio			= 26,
			.active_low		= 1,
		},
		{
			.name			= "HW556:green:lan4",
			.gpio			= 27,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:lan4",
			.gpio			= 28,
			.active_low		= 1,
		},
	},

	.buttons = {
		{
			.desc			= "help",
			.gpio			= 8,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_HELP,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc			= "wlan",
			.gpio			= 9,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_WLAN,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc			= "restart",
			.gpio			= 10,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_RESTART,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc			= "reset",
			.gpio			= 11,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_CONFIG,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};
static struct board_info __initdata board_HW556_C = {
	.name					= "HW556_C",
	.expected_cpu_id			= 0x6358,

	.has_uart0				= 1,
	.has_pci				= 1,
	.has_ohci0				= 1,
	.has_ehci0				= 1,
	.num_usbh_ports				= 2,

	.has_caldata				= 1,
	.caldata = {
		{
			.vendor			= PCI_VENDOR_ID_ATHEROS,
			.caldata_offset		= 0xefe000,
			.slot			= 1,
			.endian_check		= 1,
			.led_pin		= 2,
		},
	},

	.has_enet1				= 1,
	.enet1 = {
		.has_phy			= 1,
		.phy_id				= 0,
		.force_speed_100		= 1,
		.force_duplex_full		= 1,
	},

	.leds = {
		{
			.name			= "HW556:red:message",
			.gpio			= 0,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:hspa",
			.gpio			= 1,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:dsl",
			.gpio			= 2,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:power",
			.gpio			= 3,
			.active_low		= 1,
			.default_trigger	= "default-on",
		},
		{
			.name			= "HW556:red:all",
			.gpio			= 6,
			.active_low		= 1,
			.default_trigger	= "default-on",
		},
		{
			.name			= "HW556:green:lan1",
			.gpio			= 12,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:lan1",
			.gpio			= 13,
			.active_low		= 1,
		},
		{
			.name			= "HW556:green:lan2",
			.gpio			= 15,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:lan2",
			.gpio			= 22,
			.active_low		= 1,
		},
		{
			.name			= "HW556:green:lan3",
			.gpio			= 23,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:lan3",
			.gpio			= 26,
			.active_low		= 1,
		},
		{
			.name			= "HW556:green:lan4",
			.gpio			= 27,
			.active_low		= 1,
		},
		{
			.name			= "HW556:red:lan4",
			.gpio			= 28,
			.active_low		= 1,
		},
	},

	.buttons = {
		{
			.desc			= "help",
			.gpio			= 8,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_HELP,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc			= "wlan",
			.gpio			= 9,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_WLAN,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc			= "restart",
			.gpio			= 10,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_RESTART,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc			= "reset",
			.gpio			= 11,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_CONFIG,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

 /* T-Home Speedport W 303V Typ B */
static struct board_info __initdata board_spw303v = {
	.name			= "96358-502V",
	.expected_cpu_id	= 0x6358,

	.has_uart0		= 1,
	.has_enet0		= 1,
	.has_pci		= 1,

	.enet0 = {
		.has_phy 		= 1,
		.use_internal_phy 	= 1,
	},

	.leds = {
		{
			.name		= "spw303v:green:power+adsl",
			.gpio		= 22,
			.active_low 	= 1,
		},
		{
			.name		= "spw303v:red:power+adsl",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "spw303v:green:ppp",
			.gpio		= 5,
			.active_low	= 1,
		},
		{
			.name		= "spw303v:green:ses",
			.gpio		= 0,
			.active_low	= 1,
		},
		{
			.name		= "spw303v:green:voip",
			.gpio		= 27,
			.active_low	= 1,
		},
		{
			.name		= "spw303v:green:pots",
			.gpio		= 31,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 11,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "ses",
			.gpio		= 37,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_WPS_BUTTON,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	}
};

/* D-Link DVA-G3810BN/TL */
static struct board_info __initdata board_DVAG3810BN = {
	.name			= "DVAG3810BN",
	.expected_cpu_id	= 0x6358,

	.has_uart0		= 1,
	.has_enet0		= 1,
	.has_enet1		= 1,
	.has_pci		= 1,

	.enet0 = {
		.has_phy		= 0,
		.use_internal_phy	= 1,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},


	.has_ohci0		= 1,
	.has_pccard		= 1,
	.has_ehci0		= 1,

	.leds = {
		{
			.name	= "DVAG3810BN::voip",
			.gpio	= 1,
		},
		{
			.name	= "DVAG3810BN::dsl",
			.gpio	= 22,
			.active_low  = 1,
		},
		{
			.name	= "DVAG3810BN::internet",
			.gpio	= 23,
			.active_low  = 1,
		},
		{
			.name	= "DVAG3810BN::power",
			.gpio	= 4,
			.default_trigger = "default-on",
		},
		{
			.name	= "DVAG3810BN::stop",
			.gpio	= 5,
		},
	},

	.buttons = {
		{
			.desc	= "reset",
			.gpio	= 34,
			.active_low = 1,
			.type	= EV_KEY,
			.code	= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};
#endif

#ifdef CONFIG_BCM63XX_CPU_6362
static struct rtl8367_extif_config nb6_rtl8367_extif0_cfg = {
	.mode = RTL8367_EXTIF_MODE_RGMII,
	.txdelay = 1,
	.rxdelay = 5,
	.ability = {
		.force_mode = 1,
		.txpause = 1,
		.rxpause = 1,
		.link = 1,
		.duplex = 1,
		.speed = RTL8367_PORT_SPEED_1000,
	},
};

static struct rtl8367_platform_data nb6_rtl8367_data = {
	.gpio_sda	= NB6_GPIO_RTL8367_SDA,
	.gpio_sck	= NB6_GPIO_RTL8367_SCK,
	.extif0_cfg	= &nb6_rtl8367_extif0_cfg,
};

static struct platform_device nb6_rtl8367_device = {
	.name		= RTL8367_DRIVER_NAME,
	.id		= -1,
	.dev = {
		.platform_data	= &nb6_rtl8367_data,
	}
};

static struct platform_device * __initdata nb6_devices[] = {
	&nb6_rtl8367_device,
};

static struct board_info __initdata board_nb6 = {
	.name				= "NB6",
	.expected_cpu_id		= 0x6362,

	.has_uart0			= 1,

	.has_ohci0			= 1,
	.has_ehci0			= 1,
	.num_usbh_ports			= 2,

	.has_enetsw			= 1,

	.enetsw = {
		.used_ports = {
			[4] = {
				.used	= 1,
				.phy_id	= 0xff,
				.bypass_link = 1,
				.force_speed = 1000,
				.force_duplex_full = 1,
				.name	= "RGMII",
			},
		},
	},

	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 24,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
			.active_low	= 1,
		},
		{
			.desc		= "wps",
			.gpio		= 25,
			.type		= EV_KEY,
			.code		= KEY_WPS_BUTTON,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
			.active_low	= 1,
		},
		{
			.desc		= "wlan",
			.gpio		= 12,
			.type		= EV_KEY,
			.code		= KEY_WLAN,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
			.active_low	= 1,
		},
		{
			.desc		= "service",
			.gpio		= 10,
			.type		= EV_KEY,
			.code		= BTN_0,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
			.active_low	= 1,
		},
	},

	.devs = nb6_devices,
	.num_devs = ARRAY_SIZE(nb6_devices),
};

static struct board_info __initdata board_fast2504n = {
	.name				= "F@ST2504n",
	.expected_cpu_id		= 0x6362,

	.has_uart0			= 1,

	.has_enetsw			= 1,

	.enetsw = {
		.used_ports = {
			[0] = {
				.used	= 1,
				.phy_id	= 1,
				.name	= "Port 1",
			},

			[1] = {
				.used	= 1,
				.phy_id	= 2,
				.name	= "Port 2",
			},

			[2] = {
				.used	= 1,
				.phy_id	= 3,
				.name	= "Port 3",
			},

			[3] = {
				.used	= 1,
				.phy_id	= 4,
				.name	= "Port 4",
			},
		},
	},

	.leds = {
		{
			.name		= "fast2504n:orange:power",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "fast2504n:green:power",
			.gpio		= 10,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "fast2504n:red:internet",
			.gpio		= 26,
			.active_low	= 1,
		},
		{
			.name		= "fast2504n:green:ok",
			.gpio		= 28,
			.active_low	= 1,
		},
		{
			.name		= "fast2504n:orange:ok",
			.gpio		= 29,
			.active_low	= 1,
		},
		{
			.name		= "fast2504n:orange:wlan",
			.gpio		= 30,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 24,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
			.active_low	= 1,
		},
		{
			.desc		= "wps",
			.gpio		= 25,
			.type		= EV_KEY,
			.code		= KEY_WPS_BUTTON,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
			.active_low	= 1,
		},
	},
};
#endif

/*
 * known 6368 boards
 */
#ifdef CONFIG_BCM63XX_CPU_6368
static struct board_info __initdata board_96368mvwg = {
	.name				= "96368MVWG",
	.expected_cpu_id		= 0x6368,

	.has_uart0			= 1,
	.has_pci			= 1,

	.has_usbd			= 1,

	.usbd = {
		.use_fullspeed		= 0,
		.port_no		= 0,
	},

	.has_enetsw			= 1,

	.enetsw = {
		.used_ports = {
			[1] = {
				.used	= 1,
				.phy_id	= 2,
				.name	= "port1",
			},

			[2] = {
				.used	= 1,
				.phy_id	= 3,
				.name	= "port2",
			},

			[4] = {
				.used	= 1,
				.phy_id	= 0x12,
				.name	= "port0",
			},

			[5] = {
				.used	= 1,
				.phy_id	= 0x11,
				.name	= "port3",
			},
		},
	},

	.leds = {
		{
			.name		= "96368MVWG:green:adsl",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "96368MVWG:green:ppp",
			.gpio		= 5,
		},
		{
			.name		= "96368MVWG:green:power",
			.gpio		= 22,
			.default_trigger = "default-on",
		},
		{
			.name		= "96368MVWG:green:wps",
			.gpio		= 23,
			.active_low	= 1,
		},
		{
			.name		= "96368MVWG:red:ppp-fail",
			.gpio		= 31,
		},
	},

	.has_ohci0 = 1,
	.has_ehci0 = 1,
};

static struct board_info __initdata board_96368mvngr = {
	.name				= "96368MVNgr",
	.expected_cpu_id		= 0x6368,

	.has_uart0			= 1,
	.has_pci			= 1,
	.has_enetsw			= 1,

	.enetsw = {
		.used_ports = {
			[0] = {
				.used	= 1,
				.phy_id	= 1,
				.name	= "port1",
			},

			[1] = {
				.used	= 1,
				.phy_id	= 2,
				.name	= "port2",
			},

			[2] = {
				.used	= 1,
				.phy_id	= 3,
				.name	= "port3",
			},

			[3] = {
				.used	= 1,
				.phy_id	= 4,
				.name	= "port4",
			},
		},
	},

	.leds = {
		{
			.name		= "96368MVNgr:green:adsl",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "96368MVNgr:green:inet",
			.gpio		= 5,
		},
		{
			.name		= "96368MVNgr:green:power",
			.gpio		= 22,
			.default_trigger = "default-on",
		},
		{
			.name		= "96368MVNgr:green:wps",
			.gpio		= 23,
			.active_low	= 1,
		},
		{
			.name		= "96368MVNgr:green:inet-fail",
			.gpio		= 3,
		},
	},

	.has_ohci0 = 1,
	.has_ehci0 = 1,
};

static struct b53_platform_data DGND3700_3800B_b53_pdata = {
	.alias    = "eth0",
};

static struct spi_board_info DGND3700_3800B_spi_devices[] = {
	{
		.modalias = "b53-switch",
		.max_speed_hz = 781000,
		.bus_num = 0,
		.chip_select = 1,
		.platform_data = &DGND3700_3800B_b53_pdata,
	}
};

static struct board_info __initdata board_DGND3700_3800B = {
	.name				= "DGND3700_3800B",
	.expected_cpu_id		= 0x6368,

	.has_uart0			= 1,
	.has_pci			= 1,
	.has_ohci0			= 1,
	.has_ehci0			= 1,

	.has_caldata				= 1,
	.caldata = {
		{
			.caldata_offset		= 0x1e40000,
		},
	},

	.has_enetsw			= 1,
	.enetsw = {
		.used_ports = {
			[5] = {
				.used  = 1,
				.phy_id  = 0xff,
				.bypass_link = 1,
				.force_speed = 1000,
				.force_duplex_full = 1,
				.name  = "RGMII",
			},
		},
	},

	.leds = {
		{
			.name		= "DGND3700_3800B:green:dsl",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "DGND3700_3800B:red:inet",
			.gpio		= 4,
			.active_low	= 1,
		},
		{
			.name		= "DGND3700_3800B:green:inet",
			.gpio		= 5,
			.active_low	= 1,
		},
		{
			.name		= "DGND3700_3800B:green:wps",
			.gpio		= 11,
			.active_low	= 1,
		},
		{
			.name		= "DGND3700_3800B:green:usb-front",
			.gpio		= 13,
			.active_low	= 1,
		},
		{
			.name		= "DGND3700_3800B:green:usb-back",
			.gpio		= 14,
			.active_low	= 1,
		},
		{
			.name		= "DGND3700_3800B:red:power",
			.gpio		= 22,
			.active_low	= 1,
		},
		{
			.name		= "DGND3700_3800B:green:lan",
			.gpio		= 23,
			.active_low	= 1,
		},
		{
			.name		= "DGND3700_3800B:green:power",
			.gpio		= 24,
			.active_low	= 1,
			.default_trigger = "default-on",
		},
		{
			.name		= "DGND3700_3800B:green:wifi2g",
			.gpio		= 26,
			.active_low	= 1,
		},
		{
			.name		= "DGND3700_3800B:blue:wifi5g",
			.gpio		= 27,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc			= "wlan",
			.gpio			= 10,
			.active_low		= 1,
			.type			= EV_KEY,
			.code			= KEY_WLAN,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "reset",
			.gpio		= 12,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "wps",
			.gpio		= 35,
			.active_low	= 1,
			.type		= EV_KEY,
			.code		= KEY_WPS_BUTTON,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},

	.spis = DGND3700_3800B_spi_devices,
	.num_spis = ARRAY_SIZE(DGND3700_3800B_spi_devices),
};

static struct board_info __initdata board_HG622 = {
	.name				= "96368MVWG_hg622",
	.expected_cpu_id	= 0x6368,

	.has_uart0			= 1,
	.has_pci			= 1,
	.has_ohci0			= 1,
	.has_ehci0			= 1,

	.has_caldata		= 1,
	.caldata = {
		{
			.vendor			= PCI_VENDOR_ID_RALINK,
			.caldata_offset	= 0xfa0000,
			.slot			= 1,
			.eeprom			= "rt2x00.eeprom",
		},
	},

	.has_enetsw			= 1,
	.enetsw = {
		.used_ports = {
			[0] = {
				.used		= 1,
				.phy_id		= 1,
				.name		= "port1",
			},
			[1] = {
				.used		= 1,
				.phy_id		= 2,
				.name		= "port2",
			},
			[2] = {
				.used		= 1,
				.phy_id		= 3,
				.name		= "port3",
			},
			[3] = {
				.used		= 1,
				.phy_id		= 4,
				.name		= "port4",
			},
		},
	},

	.leds = {
		{
			.name		= "HG622:green:adsl",
			.gpio		= 2,
		},
		{
			.name		= "HG622:green:inet",
			.gpio		= 5,
			.active_low	= 1,
		},
		{
			.name		= "HG622:green:power",
			.gpio		= 22,
			.default_trigger = "default-on",
			.active_low	= 1,
		},
		{
			.name		= "HG622:green:usb",
			.gpio		= 11,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc           = "reset",
			.gpio			= 34,
			.type			= EV_KEY,
			.code			= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
			.active_low		= 1,
		},
	},
};

static struct board_info __initdata board_HG655b = {
	.name				= "HW65x",
	.expected_cpu_id	= 0x6368,

	.has_uart0			= 1,
	.has_pci			= 1,
	.has_ohci0			= 1,
	.has_ehci0			= 1,
	.num_usbh_ports			= 2,

	.has_caldata			= 1,
	.caldata = {
		{
			.vendor			= PCI_VENDOR_ID_RALINK,
			.caldata_offset		= 0x7c0000,
			.slot			= 1,
			.eeprom			= "rt2x00.eeprom",
		},
	},

	.has_enetsw			= 1,
	.enetsw = {
		.used_ports = {
			[0] = {
				.used		= 1,
				.phy_id		= 1,
				.name		= "port1",
			},
			[1] = {
				.used		= 1,
				.phy_id		= 2,
				.name		= "port2",
			},
			[2] = {
				.used		= 1,
				.phy_id		= 3,
				.name		= "port3",
			},
			[3] = {
				.used		= 1,
				.phy_id		= 4,
				.name		= "port4",
			},
		},
	},

	.leds = {
		{
			.name		= "HW65x:green:dsl",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "HW65x:green:inet",
			.gpio		= 5,
			.active_low	= 1,
		},
		{
			.name		= "HW65x:green:lan1",
			.gpio		= 6,
			.active_low	= 1,
		},
		{
			.name		= "HW65x:green:lan2",
			.gpio		= 7,
			.active_low	= 1,
		},
		{
			.name		= "HW65x:green:lan3",
			.gpio		= 8,
			.active_low	= 1,
		},
		{
			.name		= "HW65x:green:lan4",
			.gpio		= 9,
			.active_low	= 1,
		},
		{
			.name		= "HW65x:green:usb",
			.gpio		= 14,
			.active_low	= 1,
		},
		{
			.name		= "HW65x:green:power",
			.gpio		= 22,
			.default_trigger = "default-on",
			.active_low	= 1,
		},
		{
			.name		= "HW65x:green:voip",
			.gpio		= 25,
			.active_low	= 1,
		},
		{
			.name		= "HW65x:green:wps",
			.gpio		= 27,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc           = "reset",
			.gpio			= 34,
			.type			= EV_KEY,
			.code			= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
			.active_low		= 1,
		},
		{
			.desc           = "wps",
			.gpio			= 12,
			.type			= EV_KEY,
			.code			= KEY_WPS_BUTTON,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
			.active_low		= 1,
		},
	},
};

static struct board_info __initdata board_P870HW51A_V2 = {
	.name				= "P870HW-51a_v2",
	.expected_cpu_id		= 0x6368,

	.has_uart0			= 1,
	.has_pci			= 1,
	.has_ohci0			= 1,
	.has_ehci0			= 1,

	.has_enetsw			= 1,
	.enetsw = {
		.used_ports = {
			[0] = {
				.used	= 1,
				.phy_id	= 1,
				.name	= "port1",
			},
			[1] = {
				.used	= 1,
				.phy_id	= 2,
				.name	= "port2",
			},
			[2] = {
				.used	= 1,
				.phy_id	= 3,
				.name	= "port3",
			},
			[3] = {
				.used	= 1,
				.phy_id	= 4,
				.name	= "port4",
			},
		},
	},

	.leds = {
		{
			.name		= "P870HW-51a:green:power",
			.gpio		= 0,
			.default_trigger = "default-on",
		},
		{
			.name		= "P870HW-51a:green:dsl",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "P870HW-51a:green:inet",
			.gpio		= 22,
			.active_low	= 1,
		},
		{
			.name		= "P870HW-51a:orange:wps",
			.gpio		= 24,
			.active_low	= 1,
		},
		{
			.name		= "P870HW-51a:red:inet",
			.gpio		= 33,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 34,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
			.active_low	= 1,
		},
		{
			.desc		= "wps",
			.gpio		= 35,
			.type		= EV_KEY,
			.code		= KEY_WPS_BUTTON,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
			.active_low	= 1,
		},
	},
};

static struct board_info __initdata board_VR3025u = {
	.name					= "96368M-1541N",
	.expected_cpu_id		= 0x6368,

	.has_uart0				= 1,
	.has_pci				= 1,
	.has_ohci0				= 1,
	.has_ehci0				= 1,

	.has_enetsw				= 1,
	.enetsw = {
		.used_ports = {
			[0] = {
				.used		= 1,
				.phy_id		= 1,
				.name		= "port1",
			},
			[1] = {
				.used		= 1,
				.phy_id		= 2,
				.name		= "port2",
			},
			[2] = {
				.used		= 1,
				.phy_id		= 3,
				.name		= "port3",
			},
			[3] = {
				.used		= 1,
				.phy_id		= 4,
				.name		= "port4",
			},
		},
	},

	.leds = {
		{
			.name		= "VR-3025u:green:dsl",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "VR-3025u:green:inet",
			.gpio		= 5,
		},
		{
			.name		= "VR-3025u:green:power",
			.gpio		= 22,
			.default_trigger = "default-on",
		},
		{
			.name		= "VR-3025u:red:power",
			.gpio		= 24,
		},
		{
			.name		= "VR-3025u:red:inet",
			.gpio		= 31,
		},
	},

	.buttons = {
		{
			.desc			= "reset",
			.gpio			= 34,
			.type			= EV_KEY,
			.code			= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
			.active_low		= 1,
		},
	},
};

static struct board_info __initdata board_VR3025un = {
	.name					= "96368M-1341N",
	.expected_cpu_id		= 0x6368,

	.has_uart0				= 1,
	.has_pci				= 1,
	.has_ohci0				= 1,
	.has_ehci0				= 1,

	.has_enetsw				= 1,
	.enetsw = {
		.used_ports = {
			[0] = {
				.used		= 1,
				.phy_id		= 1,
				.name		= "port1",
			},
			[1] = {
				.used		= 1,
				.phy_id		= 2,
				.name		= "port2",
			},
			[2] = {
				.used		= 1,
				.phy_id		= 3,
				.name		= "port3",
			},
			[3] = {
				.used		= 1,
				.phy_id		= 4,
				.name		= "port4",
			},
		},
	},

	.leds = {
		{
			.name		= "VR-3025un:green:dsl",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "VR-3025un:green:inet",
			.gpio		= 5,
		},
		{
			.name		= "VR-3025un:green:power",
			.gpio		= 22,
			.default_trigger = "default-on",
		},
		{
			.name		= "VR-3025un:red:power",
			.gpio		= 24,
		},
		{
			.name		= "VR-3025un:red:inet",
			.gpio		= 31,
		},
	},

	.buttons = {
		{
			.desc			= "reset",
			.gpio			= 34,
			.type			= EV_KEY,
			.code			= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
			.active_low		= 1,
		},
	},
};

static struct b53_platform_data WAP5813n_b53_pdata = {
	.alias    = "eth0",
};

static struct spi_board_info WAP5813n_spi_devices[] = {
	{
		.modalias = "b53-switch",
		.max_speed_hz = 781000,
		.bus_num = 0,
		.chip_select = 0,
		.platform_data = &WAP5813n_b53_pdata,
	}
};

static struct board_info __initdata board_WAP5813n = {
	.name				= "96369R-1231N",
	.expected_cpu_id		= 0x6368,

	.has_uart0			= 1,
	.has_pci			= 1,
	.has_ohci0			= 1,
	.has_ehci0			= 1,

	.has_enetsw			= 1,
	.enetsw = {
		.used_ports = {
			[4] = {
				.used  = 1,
				.phy_id  = 0xff,
				.bypass_link = 1,
				.force_speed = 1000,
				.force_duplex_full = 1,
				.name  = "RGMII",
			},
		},
	},

	.leds = {
		{
			.name		= "WAP-5813n:green:power",
			.gpio		= 22,
			.default_trigger = "default-on",
		},
		{
			.name		= "WAP-5813n:red:power",
			.gpio        = 24,
		},
		{
			.name		= "WAP-5813n:green:inet",
			.gpio		= 5,
		},
		{
			.name		= "WAP-5813n:red:inet",
			.gpio		= 31,
		},
		{
			.name		= "WAP-5813n:green:wps",
			.gpio		= 23,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc			= "wlan",
			.gpio			= 32,
			.active_low	= 1,
			.type			= EV_KEY,
			.code			= KEY_WLAN,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc			= "reset",
			.gpio			= 34,
			.active_low	= 1,
			.type			= EV_KEY,
			.code			= KEY_RESTART,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc			= "wps",
			.gpio			= 35,
			.active_low	= 1,
			.type			= EV_KEY,
			.code			= KEY_WPS_BUTTON,
			.debounce_interval	= BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},

	.spis = WAP5813n_spi_devices,
	.num_spis = ARRAY_SIZE(WAP5813n_spi_devices),
};
#endif

/*
 * known 63268/63269 boards
 */
#ifdef CONFIG_BCM63XX_CPU_63268
static struct board_info __initdata board_963268bu_p300 = {
	.name				= "963268BU_P300",
	.expected_cpu_id		= 0x63268,

	.has_uart0			= 1,

	.has_ehci0			= 1,
	.has_ohci0			= 1,
	.num_usbh_ports			= 1,

	.has_usbd			= 1,

	.usbd = {
		.use_fullspeed		= 0,
		.port_no		= 0,
	},

	.has_enetsw			= 1,

	.enetsw = {
		.used_ports = {
			[0] = {
				.used	= 1,
				.phy_id	= 17,
				.name	= "FE1",
			},

			[3] = {
				.used	= 1,
				.phy_id	= 4,
				.name	= "GbE2",
			},

			[4] = {
				.used	= 1,
				.phy_id	= 0,
				.name	= "GbE3",
			},

			[5] = {
				.used	= 1,
				.phy_id	= 1,
				.name	= "GbE1",
			},

			[6] = {
				.used	= 1,
				.phy_id	= 24,
				.name	= "GbE4",
			},

			[7] = {
				.used	= 1,
				.phy_id	= 25,
				.name	= "GbE5",
			},
		},
	},

	.leds = {
		{
			.name		= "963268BU_P300:green:power",
			.gpio		= 20,
			.active_low	= 1,
		},
		{
			.name		= "963268BU_P300:red:power",
			.gpio		= 21,
			.active_low	= 1,
		},
		{
			.name		= "963268BU_P300:green:internet",
			.gpio		= 8,
			.active_low	= 1,
		},
		{
			.name		= "963268BU_P300:red:internet",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "963268BU_P300:green:adsl",
			.gpio		= 3,
			.active_low	= 1,
		},
		{
			.name		= "963268BU_P300:green:wps",
			.gpio		= 7,
			.active_low	= 1,
		},
		{
			.name		= "963268BU_P300:green:voip1",
			.gpio		= 4,
			.active_low	= 1,
		},
		{
			.name		= "963268BU_P300:green:voip2",
			.gpio		= 5,
			.active_low	= 1,
		},
		{
			.name		= "963268BU_P300:green:pots",
			.gpio		= 6,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 32,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "wps",
			.gpio		= 33,
			.type		= EV_KEY,
			.code		= KEY_WPS_BUTTON,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

static struct board_info __initdata board_963269bhr = {
	.name				= "963269BHR",
	.expected_cpu_id		= 0x63268,

	.has_uart0			= 1,

	.has_pci			= 1,

	.has_ehci0			= 1,

	.has_enetsw			= 1,

	.enetsw = {
		.used_ports = {
			[0] = {
				.used	= 1,
				.phy_id	= 1,
				.name	= "port1",
			},

			[1] = {
				.used	= 1,
				.phy_id	= 2,
				.name	= "port2",
			},

			[2] = {
				.used	= 1,
				.phy_id	= 3,
				.name	= "port3",
			},

			[3] = {
				.used	= 1,
				.phy_id	= 4,
				.name	= "port4",
			},
		},
	},

	.leds = {
		{
			.name		= "963629BHR:green:usb1",
			.gpio		= 9,
			.active_low	= 1,
		},
		{
			.name		= "963629BHR:green:usb2",
			.gpio		= 10,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 32,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};

static struct board_info __initdata board_vw6339gu = {
	.name				= "VW6339GU",
	.expected_cpu_id		= 0x63268,

	.has_uart0			= 1,

	.has_ehci0			= 1,
	.has_ohci0			= 1,
	.num_usbh_ports			= 1,

	.has_enetsw			= 1,

	.enetsw = {
		.used_ports = {
			[0] = {
				.used	= 1,
				.phy_id	= 1,
				.name	= "LAN2",
			},

			[1] = {
				.used	= 1,
				.phy_id	= 2,
				.name	= "LAN3",
			},

			[2] = {
				.used	= 1,
				.phy_id	= 3,
				.name	= "LAN4",
			},

			[3] = {
				.used	= 1,
				.phy_id	= 4,
				.name	= "LAN1",
			},

			[4] = {
				.used	= 1,
				.phy_id	= 7,
				.name	= "WAN",
			},
		},
	},

	.leds = {
		{
			.name		= "VW6339GU:green:power",
			.gpio		= 1,
			.active_low	= 1,
		},
		{
			.name		= "VW6339GU:red:power",
			.gpio		= 0,
			.active_low	= 1,
		},
		{
			.name		= "VW6339GU:green:internet",
			.gpio		= 8,
			.active_low	= 1,
		},
		{
			.name		= "VW6339GU:red:internet",
			.gpio		= 2,
			.active_low	= 1,
		},
		{
			.name		= "VW6339GU:green:dsl",
			.gpio		= 3,
			.active_low	= 1,
		},
		{
			.name		= "VW6339GU:green:wps",
			.gpio		= 7,
			.active_low	= 1,
		},
		{
			.name		= "VW6339GU:green:usb",
			.gpio		= 15,
			.active_low	= 1,
		},
	},

	.buttons = {
		{
			.desc		= "reset",
			.gpio		= 32,
			.type		= EV_KEY,
			.code		= KEY_RESTART,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
		{
			.desc		= "wps",
			.gpio		= 34,
			.type		= EV_KEY,
			.code		= KEY_WPS_BUTTON,
			.debounce_interval = BCM963XX_KEYS_DEBOUNCE_INTERVAL,
		},
	},
};
#endif

/*
 * all boards
 */
static const struct board_info __initconst *bcm963xx_boards[] = {
#ifdef CONFIG_BCM63XX_CPU_3368
	&board_cvg834g,
#endif
#ifdef CONFIG_BCM63XX_CPU_6318
	&board_96318ref,
	&board_96318ref_p300,
#endif
#ifdef CONFIG_BCM63XX_CPU_6328
	&board_96328avng,
	&board_AR5381u,
	&board_AR5387un,
	&board_963281TAN,
	&board_A4001N,
	&board_dsl_274xb_f1,
	&board_FAST2704V2,
#endif
#ifdef CONFIG_BCM63XX_CPU_6338
	&board_96338gw,
	&board_96338w,
	&board_96338w2_e7t,
	&board_rta1320_16m,
#endif
#ifdef CONFIG_BCM63XX_CPU_6345
	&board_96345gw2,
	&board_rta770bw,
	&board_rta770w,
#endif
#ifdef CONFIG_BCM63XX_CPU_6348
	&board_96348r,
	&board_96348gw,
	&board_gw6000,
	&board_gw6200,
	&board_96348gw_10,
	&board_96348gw_11,
	&board_FAST2404,
	&board_FAST2604,
	&board_DV201AMR,
	&board_96348gw_a,
	&board_rta1025w_16,
	&board_96348_D4PW,
	&board_spw500v,
	&board_96348sv,
	&board_V2500V_BB,
	&board_V2110,
	&board_ct536_ct5621,
	&board_CT5365,
	&board_CPVA502plus,
	&board_96348W3,
	&board_96348gw_10_AR1004G,
#endif

#ifdef CONFIG_BCM63XX_CPU_6358
	&board_96358vw,
	&board_96358vw2,
	&board_AGPFS0,
	&board_CPVA642,
	&board_DWVS0,
	&board_dsl_274xb_rev_c,
	&board_nb4_ser_r0,
	&board_nb4_ser_r1,
	&board_nb4_ser_r2,
	&board_nb4_fxc_r1,
	&board_nb4_fxc_r2,
	&board_ct6373_1,
	&board_HW520,
	&board_HW553,
	&board_HW556,
	&board_HW556_A,
	&board_HW556_B,
	&board_HW556_C,
	&board_spw303v,
	&board_DVAG3810BN,
#endif

#ifdef CONFIG_BCM63XX_CPU_6362
	&board_nb6,
	&board_fast2504n,
#endif

#ifdef CONFIG_BCM63XX_CPU_6368
	&board_96368mvwg,
	&board_96368mvngr,
	&board_DGND3700_3800B,
	&board_HG622,
	&board_HG655b,
	&board_P870HW51A_V2,
	&board_VR3025u,
	&board_VR3025un,
	&board_WAP5813n,
#endif
#ifdef CONFIG_BCM63XX_CPU_63268
	&board_963268bu_p300,
	&board_963269bhr,
	&board_vw6339gu,
#endif
};

/*
 * Register a sane SPROMv2 to make the on-board
 * bcm4318 WLAN work
 */
#ifdef CONFIG_SSB_PCIHOST
struct ssb_sprom bcm63xx_sprom = {
	.revision		= 0x02,
	.board_rev		= 0x17,
	.country_code		= 0x0,
	.ant_available_bg	= 0x3,
	.pa0b0			= 0x15ae,
	.pa0b1			= 0xfa85,
	.pa0b2			= 0xfe8d,
	.pa1b0			= 0xffff,
	.pa1b1			= 0xffff,
	.pa1b2			= 0xffff,
	.gpio0			= 0xff,
	.gpio1			= 0xff,
	.gpio2			= 0xff,
	.gpio3			= 0xff,
	.maxpwr_bg		= 0x004c,
	.itssi_bg		= 0x00,
	.boardflags_lo		= 0x2848,
	.boardflags_hi		= 0x0000,
};
EXPORT_SYMBOL(bcm63xx_sprom);

int bcm63xx_get_fallback_sprom(struct ssb_bus *bus, struct ssb_sprom *out)
{
	if (bus->bustype == SSB_BUSTYPE_PCI) {
		memcpy(out, &bcm63xx_sprom, sizeof(struct ssb_sprom));
		return 0;
	} else {
		printk(KERN_ERR PFX "unable to fill SPROM for given bustype.\n");
		return -EINVAL;
	}
}
#endif

/*
 * return board name for /proc/cpuinfo
 */
const char *board_get_name(void)
{
	return board.name;
}

static void __init boardid_fixup(u8 *boot_addr)
{
	struct bcm_tag *tag = (struct bcm_tag *)(boot_addr + CFE_OFFSET_64K);
	char *board_name = (char *)bcm63xx_nvram_get_name();

	if (BCMCPU_IS_6358()) {
		if (!strcmp(board_name, "96358VW")) {
			u8 *p = boot_addr + NB4_PID_OFFSET;

			/* Extract nb4 PID */
			if (!memcmp(p, "NB4-", 4)) {
				memcpy(board_name, p, sizeof("NB4-XXX-rX"));
				return;
			}
		} else if (!strcmp(board_name, "HW556")) {
			/*
			 * HW556 has different wlan caldatas depending on
			 * hardware version.
			 * Detect hardware version and change board id
			 */
			u8 cal_data_ath9k[4] = { 0xa5, 0x5a, 0, 0 };
			u8 cal_data_rt3062[4] = { 0x62, 0x30, 1, 0 };

			if (!memcmp(boot_addr + 0xeffe00,
				    &cal_data_rt3062, 4)) {
				/* Ralink 0xeffe00 */
				memcpy(board_name, "HW556_A", 7);
			} else if (!memcmp(boot_addr + 0xf7e000,
					   &cal_data_ath9k, 4)) {
				/* Atheros 0xf7e000 */
				memcpy(board_name, "HW556_B", 7);
			} else if (!memcmp(boot_addr + 0xefe000,
					   &cal_data_ath9k, 4)) {
				/* Atheros 0xefe000 */
				memcpy(board_name, "HW556_C", 7);
			}
		}
	}

	if (BCMCPU_IS_6362() && (!strncmp(board_name, "NB6-", sizeof("NB6-") - 1))) {
		board_name[sizeof("NB6") - 1] = '\0';
		return ;
	}

	/* check if bcm_tag is at 64k offset */
	if (strncmp(board_name, tag->board_id, BOARDID_LEN) != 0) {
		/* else try 128k */
		tag = (struct bcm_tag *)(boot_addr + CFE_OFFSET_128K);
		if (strncmp(board_name, tag->board_id, BOARDID_LEN) != 0) {
			/* No tag found */
			printk(KERN_DEBUG "No bcm_tag found!\n");
			return;
		}
	}
	/* check if we should override the boardid */
	if (tag->information1[0] != '+')
		return;

	strncpy(board_name, &tag->information1[1], BOARDID_LEN);

	printk(KERN_INFO "Overriding boardid with '%s'\n", board_name);
}

/*
 * early init callback, read nvram data from flash and checksum it
 */
void __init board_prom_init(void)
{
	unsigned int i;
	u8 *boot_addr, *cfe;
	char cfe_version[32];
	char *board_name = NULL;
	u32 val;
	struct bcm_hcs *hcs;

	/* read base address of boot chip select (0)
	 * 6328/6362 do not have MPI but boot from a fixed address
	 */
	if (BCMCPU_IS_6318() || BCMCPU_IS_6328() || BCMCPU_IS_6362() || BCMCPU_IS_63268()) {
		val = 0x18000000;
	} else {
		val = bcm_mpi_readl(MPI_CSBASE_REG(0));
		val &= MPI_CSBASE_BASE_MASK;
	}
	boot_addr = (u8 *)KSEG1ADDR(val);
	printk(KERN_INFO PFX "Boot address 0x%08x\n",(unsigned int)boot_addr);

	/* BT Voyager 2500V (RTA1046VW PCB) has 8 Meg flash used as two */
	/* banks of 4 Meg. The byte at 0xBF800000 identifies the back to use.*/
	/* Loading firmware from the CFE Prompt always loads to Bank 0 */
	/* Do an early check of CFE and then select bank 0 */

	if (boot_addr == (u8 *)0xbf800000) {
		u8 *tmp_boot_addr = (u8*)0xbfc00000;

		bcm63xx_nvram_init(tmp_boot_addr + BCM963XX_NVRAM_OFFSET);
		if (!strcmp(bcm63xx_nvram_get_name(), "V2500V_BB")) {
			printk(KERN_INFO PFX "V2500V: nvram bank 0\n");
			boot_addr = tmp_boot_addr;
		}
	}

	/* dump cfe version */
	cfe = boot_addr + BCM963XX_CFE_VERSION_OFFSET;
	if (strstarts(cfe, "cfe-")) {
		if(cfe[4] == 'v') {
			if(cfe[5] == 'd')
				snprintf(cfe_version, 11, "%s", (char *) &cfe[5]);
			else if (cfe[10] > 0)
				snprintf(cfe_version, sizeof(cfe_version), "%u.%u.%u-%u.%u-%u",
					 cfe[5], cfe[6], cfe[7], cfe[8], cfe[9], cfe[10]);
			else
				snprintf(cfe_version, sizeof(cfe_version), "%u.%u.%u-%u.%u",
					 cfe[5], cfe[6], cfe[7], cfe[8], cfe[9]);
		} else {
			snprintf(cfe_version, 12, "%s", (char *) &cfe[4]);
		}
	} else
		strcpy(cfe_version, "unknown");
	printk(KERN_INFO PFX "CFE version: %s\n", cfe_version);

	bcm63xx_nvram_init(boot_addr + BCM963XX_NVRAM_OFFSET);

	if (BCMCPU_IS_3368()) {
		hcs = (struct bcm_hcs *)boot_addr;
		board_name = hcs->filename;
	} else {
		if (strcmp(cfe_version, "unknown") != 0) {
			/* cfe present */
			boardid_fixup(boot_addr);
		}
		board_name = bcm63xx_nvram_get_name();
	}
	/* find board by name */
	for (i = 0; i < ARRAY_SIZE(bcm963xx_boards); i++) {
		if (strncmp(board_name, bcm963xx_boards[i]->name, 16))
			continue;
		/* copy, board desc array is marked initdata */
		memcpy(&board, bcm963xx_boards[i], sizeof(board));
		break;
	}

	/* bail out if board is not found, will complain later */
	if (!board.name[0]) {
		char name[17];
		memcpy(name, board_name, 16);
		name[16] = 0;
		printk(KERN_ERR PFX "unknown bcm963xx board: %s\n",
		       name);
		return;
	}

	/* setup pin multiplexing depending on board enabled device,
	 * this has to be done this early since PCI init is done
	 * inside arch_initcall */
	val = 0;

#ifdef CONFIG_PCI
	if (board.has_pci) {
		bcm63xx_pci_enabled = 1;
		if (BCMCPU_IS_6348())
			val |= GPIO_MODE_6348_G2_PCI;

		if (BCMCPU_IS_6368())
			val |= GPIO_MODE_6368_PCI_REQ1 |
				GPIO_MODE_6368_PCI_GNT1 |
				GPIO_MODE_6368_PCI_INTB |
				GPIO_MODE_6368_PCI_REQ0 |
				GPIO_MODE_6368_PCI_GNT0;
	}
#endif

	if (board.has_pccard) {
		if (BCMCPU_IS_6348())
			val |= GPIO_MODE_6348_G1_MII_PCCARD;

		if (BCMCPU_IS_6368())
			val |= GPIO_MODE_6368_PCMCIA_CD1 |
				GPIO_MODE_6368_PCMCIA_CD2 |
				GPIO_MODE_6368_PCMCIA_VS1 |
				GPIO_MODE_6368_PCMCIA_VS2;
	}

	if (board.has_enet0 && !board.enet0.use_internal_phy) {
		if (BCMCPU_IS_6348())
			val |= GPIO_MODE_6348_G3_EXT_MII |
				GPIO_MODE_6348_G0_EXT_MII;
	}

	if (board.has_enet1 && !board.enet1.use_internal_phy) {
		if (BCMCPU_IS_6348())
			val |= GPIO_MODE_6348_G3_EXT_MII |
				GPIO_MODE_6348_G0_EXT_MII;
		else if (BCMCPU_IS_6358())
			val |= GPIO_MODE_6358_ENET1_MII_CLK_INV;
	}

	bcm_gpio_writel(val, GPIO_MODE_REG);

#if IS_ENABLED(CONFIG_USB)
	if (BCMCPU_IS_6318() && (board.has_ehci0 || board.has_ohci0)) {
		val = bcm_gpio_readl(GPIO_PINMUX_SEL0_6318);
		val &= ~GPIO_PINMUX_SEL0_GPIO13_MASK;
		val |= GPIO_PINMUX_SEL0_GPIO13_PWRON;
		bcm_gpio_writel(val, GPIO_PINMUX_SEL0_6318);
	}
#endif
}

/*
 * second stage init callback, good time to panic if we couldn't
 * identify on which board we're running since early printk is working
 */
void __init board_setup(void)
{
	if (!board.name[0])
		panic("unable to detect bcm963xx board");
	printk(KERN_INFO PFX "board name: %s\n", board.name);

	/* make sure we're running on expected cpu */
	if (bcm63xx_get_cpu_id() != board.expected_cpu_id)
		panic("unexpected CPU for bcm963xx board");
}

static struct gpio_led_platform_data bcm63xx_led_data;

static struct platform_device bcm63xx_gpio_leds = {
	.name			= "leds-gpio",
	.id			= 0,
	.dev.platform_data	= &bcm63xx_led_data,
};

static struct gpio_keys_platform_data bcm63xx_gpio_keys_data = {
	.poll_interval  = BCM963XX_KEYS_POLL_INTERVAL,
};

static struct platform_device bcm63xx_gpio_keys_device = {
	.name		= "gpio-keys-polled",
	.id		= 0,
	.dev.platform_data = &bcm63xx_gpio_keys_data,
};

/*
 * third stage init callback, register all board devices.
 */
int __init board_register_devices(void)
{
	int button_count = 0;
	int led_count = 0;
	int usbh_ports = 0;
	int i;

	if (board.has_uart0)
		bcm63xx_uart_register(0);

	if (board.has_uart1)
		bcm63xx_uart_register(1);

	if (board.has_pccard)
		bcm63xx_pcmcia_register();

	if (board.has_enet0 &&
	    !bcm63xx_nvram_get_mac_address(board.enet0.mac_addr))
		bcm63xx_enet_register(0, &board.enet0);

	if (board.has_enet1 &&
	    !bcm63xx_nvram_get_mac_address(board.enet1.mac_addr))
		bcm63xx_enet_register(1, &board.enet1);

	if (board.has_enetsw &&
	    !bcm63xx_nvram_get_mac_address(board.enetsw.mac_addr))
		bcm63xx_enetsw_register(&board.enetsw);

	if ((board.has_ohci0 || board.has_ehci0)) {
		usbh_ports = board.num_usbh_ports;

		if (!usbh_ports || WARN_ON(usbh_ports > 1 && board.has_usbd))
			usbh_ports = 1;
	}

	if (board.has_usbd)
		bcm63xx_usbd_register(&board.usbd);

	if (board.has_ehci0)
		bcm63xx_ehci_register(usbh_ports);

	if (board.has_ohci0)
		bcm63xx_ohci_register(usbh_ports);

	if (board.has_dsp)
		bcm63xx_dsp_register(&board.dsp);

	/* Generate MAC address for WLAN and register our SPROM,
	 * do this after registering enet devices
	 */
#ifdef CONFIG_SSB_PCIHOST
	if (!board.has_caldata &&
	    !bcm63xx_nvram_get_mac_address(bcm63xx_sprom.il0mac)) {
		memcpy(bcm63xx_sprom.et0mac, bcm63xx_sprom.il0mac, ETH_ALEN);
		memcpy(bcm63xx_sprom.et1mac, bcm63xx_sprom.il0mac, ETH_ALEN);
		if (ssb_arch_register_fallback_sprom(
				&bcm63xx_get_fallback_sprom) < 0)
			pr_err(PFX "failed to register fallback SPROM\n");
	}
#endif

	bcm63xx_spi_register();

	bcm63xx_hsspi_register();

#ifdef CONFIG_BCMA
	if (!board.has_caldata &&
		bcma_arch_register_fallback_sprom(
		&bcm63xx_get_bcma_fallback_sprom) < 0)
		pr_err(PFX "failed to register BCMA fallback SPROM\n");
#endif

	if (board.num_devs)
		platform_add_devices(board.devs, board.num_devs);

	if (board.num_spis)
		spi_register_board_info(board.spis, board.num_spis);

	bcm63xx_flash_register(board.has_caldata, board.caldata);

	/* count number of LEDs defined by this device */
	while (led_count < ARRAY_SIZE(board.leds) && board.leds[led_count].name)
		led_count++;

	if (led_count) {
		bcm63xx_led_data.num_leds = led_count;
		bcm63xx_led_data.leds = board.leds;

		platform_device_register(&bcm63xx_gpio_leds);
	}

	if (board.ephy_reset_gpio && board.ephy_reset_gpio_flags)
		gpio_request_one(board.ephy_reset_gpio,
				board.ephy_reset_gpio_flags, "ephy-reset");

	/* count number of BUTTONs defined by this device */
	while (button_count < ARRAY_SIZE(board.buttons) && board.buttons[button_count].desc)
		button_count++;

	if (button_count) {
		bcm63xx_gpio_keys_data.nbuttons = button_count;
		bcm63xx_gpio_keys_data.buttons = board.buttons;

		platform_device_register(&bcm63xx_gpio_keys_device);
	}

	/* register any fixups */
	for (i = 0; i < board.has_caldata; i++) {
		switch (board.caldata[i].vendor) {
		case PCI_VENDOR_ID_ATHEROS:
			pci_enable_ath9k_fixup(board.caldata[i].slot,
				board.caldata[i].caldata_offset, board.caldata[i].endian_check,
				board.caldata[i].led_pin);
			break;
		case PCI_VENDOR_ID_RALINK:
			pci_enable_rt2x00_fixup(board.caldata[i].slot,
				board.caldata[i].eeprom);
			break;
		}
	}

	return 0;
}
