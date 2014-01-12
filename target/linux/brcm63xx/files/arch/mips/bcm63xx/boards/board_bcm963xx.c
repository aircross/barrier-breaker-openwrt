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
#include <linux/platform_data/b53.h>
#include <linux/export.h>
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

	.has_enet0			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},

	.ephy_reset_gpio		= 36,
	.ephy_reset_gpio_flags		= GPIOF_INIT_HIGH,
};
#endif

/*
 * known 6328 boards
 */
#ifdef CONFIG_BCM63XX_CPU_6328
static struct board_info __initdata board_96328avng = {
	.name				= "96328avng",
	.expected_cpu_id		= 0x6328,

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
};

static struct board_info __initdata board_AR5381u = {
	.name					= "96328A-1241N",
	.expected_cpu_id			= 0x6328,

	.has_pci				= 1,
	.has_ohci0				= 1,
	.has_ehci0				= 1,
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
};

static struct board_info __initdata board_AR5387un = {
	.name					= "96328A-1441N1",
	.expected_cpu_id			= 0x6328,

	.has_pci				= 1,
	.has_ohci0				= 1,
	.has_ehci0				= 1,
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
};

static struct board_info __initdata board_963281TAN = {
	.name				= "963281TAN",
	.expected_cpu_id		= 0x6328,

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
};

static struct board_info __initdata board_A4001N = {
	.name					= "963281T_TEF",
	.expected_cpu_id			= 0x6328,

	.has_pci				= 1,
	.has_ohci0				= 1,
	.has_ehci0				= 1,
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
};

static struct board_info __initdata board_dsl_274xb_f1 = {
	.name				= "AW4339U",
	.expected_cpu_id		= 0x6328,

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
};
#endif

/*
 * known 6338 boards
 */
#ifdef CONFIG_BCM63XX_CPU_6338
static struct board_info __initdata board_96338gw = {
	.name				= "96338GW",
	.expected_cpu_id		= 0x6338,

	.has_enet0			= 1,
	.enet0 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.has_ohci0			= 1,
};

static struct board_info __initdata board_96338w = {
	.name				= "96338W",
	.expected_cpu_id		= 0x6338,

	.has_enet0			= 1,
	.enet0 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
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
};

static struct board_info __initdata board_rta1320_16m = {
	.name				= "RTA1320_16M",
	.expected_cpu_id		= 0x6338,

	.has_enet0			= 1,

	.enet0 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
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
};

static struct board_info __initdata board_rta770bw = {
	.name				= "RTA770BW",
	.expected_cpu_id		= 0x6345,

	.has_enet0			= 1,

	.enet0 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},
};

static struct board_info __initdata board_rta770w = {
	.name				= "RTA770W",
	.expected_cpu_id		= 0x6345,

	.has_enet0			= 1,

	.enet0 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
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

	.has_enet0			= 1,
	.has_pci			= 1,

	.enet0 = {
		.has_phy		= 1,
		.use_internal_phy	= 1,
	},
};

static struct board_info __initdata board_96348gw_10 = {
	.name				= "96348GW-10",
	.expected_cpu_id		= 0x6348,

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
};

static struct board_info __initdata board_96348gw_11 = {
	.name				= "96348GW-11",
	.expected_cpu_id		= 0x6348,

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
};


/* BT Voyager 2110 */
static struct board_info __initdata board_V2110 = {
	.name				= "V2110",
	.expected_cpu_id		= 0x6348,

	.has_enet1			= 1,
	.has_pci			= 1,

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},
};


static struct board_info __initdata board_ct536_ct5621 = {
	.name				= "CT536_CT5621",
	.expected_cpu_id		= 0x6348,

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
};

static struct board_info __initdata board_CT5365 = {
	.name				= "96348A-122",
	.expected_cpu_id		= 0x6348,

	.has_enet1			= 1,
	.has_pci			= 1,

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.has_ohci0 = 1,
};

static struct board_info __initdata board_96348gw = {
	.name				= "96348GW",
	.expected_cpu_id		= 0x6348,

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
};

static struct board_info __initdata board_gw6200 = {
	.name				= "GW6200",
	.expected_cpu_id		= 0x6348,

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
};

static struct board_info __initdata board_gw6000 = {
	.name				= "GW6000",
	.expected_cpu_id		= 0x6348,

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
};



static struct board_info __initdata board_FAST2404 = {
	.name				= "F@ST2404",
	.expected_cpu_id		= 0x6348,

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

	.has_pci					= 1,
	.has_ohci0					= 1,

	.has_enet1					= 1,
	.enet1 = {
		.has_phy			= 1,
		.phy_id				= 0,
		.force_speed_100		= 1,
		.force_duplex_full		= 1,
	},
};

static struct board_info __initdata board_rta1025w_16 = {
	.name				= "RTA1025W_16",
	.expected_cpu_id		= 0x6348,

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
};


static struct board_info __initdata board_DV201AMR = {
	.name				= "DV201AMR",
	.expected_cpu_id		= 0x6348,

	.has_pci			= 1,
	.has_ohci0			= 1,

	.has_enet0			= 1,
	.has_enet1			= 1,
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
};

static struct board_info __initdata board_96348gw_a = {
	.name				= "96348GW-A",
	.expected_cpu_id		= 0x6348,

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
};

static struct board_info __initdata board_96348_D4PW = {
	.name				= "D-4P-W",
	.expected_cpu_id		= 0x6348,

	.has_enet1			= 1,
	.has_pci			= 1,

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},
};

static struct board_info __initdata board_spw500v = {
	.name				= "SPW500V",
	.expected_cpu_id		= 0x6348,

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
};

static struct board_info __initdata board_96348sv = {
	.name				= "MAGIC",
	.expected_cpu_id		= 0x6348,

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
};

static struct board_info __initdata board_V2500V_BB = {
	.name				= "V2500V_BB",
        .expected_cpu_id                = 0x6348,

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
};

static struct board_info __initdata board_CPVA502plus = {
	.name				= "CPVA502+",
	.expected_cpu_id		= 0x6348,

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
};

/* NetGear DG834G v4 */
static struct board_info __initdata board_96348W3 = {
 	.name	 			= "96348W3",
	.expected_cpu_id		= 0x6348,

	.has_enet1			= 1,
	.has_pci			= 1,

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
	},

	.has_ohci0	 		= 1,
};

#endif

/*
 * known 6358 boards
 */
#ifdef CONFIG_BCM63XX_CPU_6358
static struct board_info __initdata board_96358vw = {
	.name				= "96358VW",
	.expected_cpu_id		= 0x6358,

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
};

static struct board_info __initdata board_96358vw2 = {
	.name				= "96358VW2",
	.expected_cpu_id		= 0x6358,

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
};

static struct board_info __initdata board_CPVA642 = {
	.name                           = "CPVA642",
	.expected_cpu_id                = 0x6358,

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
};


static struct board_info __initdata board_AGPFS0 = {
	.name				= "AGPF-S0",
	.expected_cpu_id		= 0x6358,

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
};

static struct board_info __initdata board_DWVS0 = {
	.name				= "DWV-S0",
	.expected_cpu_id		= 0x6358,

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
};

/* D-Link DSL-274xB revison C2/C3 */
static struct board_info __initdata board_dsl_274xb_rev_c = {
	.name				= "AW4139",
	.expected_cpu_id		= 0x6358,

	.has_enet1			= 1,
	.has_pci			= 1,

	.enet1 = {
		.has_phy		= 1,
		.phy_id			= 0,
		.force_speed_100	= 1,
		.force_duplex_full	= 1,
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

	.devs = nb4_devices,
	.num_devs = ARRAY_SIZE(nb4_devices),
	.spis = nb4_spi_devices,
	.num_spis = ARRAY_SIZE(nb4_spi_devices),
};

static struct board_info __initdata board_nb4_ser_r1 = {
	.name				= "NB4-SER-r1",
	.expected_cpu_id		= 0x6358,

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

	.devs = nb4_devices,
	.num_devs = ARRAY_SIZE(nb4_devices),
	.spis = nb4_spi_devices,
	.num_spis = ARRAY_SIZE(nb4_spi_devices),
};

static struct board_info __initdata board_nb4_ser_r2 = {
	.name				= "NB4-SER-r2",
	.expected_cpu_id		= 0x6358,

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

	.devs = nb4_devices,
	.num_devs = ARRAY_SIZE(nb4_devices),
	.spis = nb4_spi_devices,
	.num_spis = ARRAY_SIZE(nb4_spi_devices),
};

static struct board_info __initdata board_nb4_fxc_r1 = {
	.name				= "NB4-FXC-r1",
	.expected_cpu_id		= 0x6358,

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

	.devs = nb4_devices,
	.num_devs = ARRAY_SIZE(nb4_devices),
	.spis = nb4_spi_devices,
	.num_spis = ARRAY_SIZE(nb4_spi_devices),
};

static struct board_info __initdata board_nb4_fxc_r2 = {
	.name				= "NB4-FXC-r2",
	.expected_cpu_id		= 0x6358,

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

	.devs = ct6373_devices,
	.num_devs = ARRAY_SIZE(ct6373_devices),
	.spis = ct6373_spi_devices,
	.num_spis = ARRAY_SIZE(ct6373_spi_devices),
};

static struct board_info __initdata board_HW520 = {
	.name					= "HW6358GW_B",
	.expected_cpu_id		= 0x6358,

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
};

static struct board_info __initdata board_HW553 = {
	.name                           = "HW553",
	.expected_cpu_id                = 0x6358,

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
};

static struct board_info __initdata board_HW556 = {
	.name					= "HW556",
	.expected_cpu_id			= 0x6358,

	.has_pci				= 1,
	.has_ohci0				= 1,
	.has_ehci0				= 1,

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
};
static struct board_info __initdata board_HW556_A = {
	.name					= "HW556_A",
	.expected_cpu_id			= 0x6358,

	.has_pci				= 1,
	.has_ohci0				= 1,
	.has_ehci0				= 1,

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
};
static struct board_info __initdata board_HW556_B = {
	.name					= "HW556_B",
	.expected_cpu_id			= 0x6358,

	.has_pci				= 1,
	.has_ohci0				= 1,
	.has_ehci0				= 1,

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
};
static struct board_info __initdata board_HW556_C = {
	.name					= "HW556_C",
	.expected_cpu_id			= 0x6358,

	.has_pci				= 1,
	.has_ohci0				= 1,
	.has_ehci0				= 1,

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
};

 /* T-Home Speedport W 303V Typ B */
static struct board_info __initdata board_spw303v = {
	.name			= "96358-502V",
	.expected_cpu_id	= 0x6358,

	.has_enet0		= 1,
	.has_pci		= 1,

	.enet0 = {
		.has_phy 		= 1,
		.use_internal_phy 	= 1,
	},
};

/* D-Link DVA-G3810BN/TL */
static struct board_info __initdata board_DVAG3810BN = {
	.name			= "DVAG3810BN",
	.expected_cpu_id	= 0x6358,

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

	.has_ohci0			= 1,
	.has_ehci0			= 1,

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

	.devs = nb6_devices,
	.num_devs = ARRAY_SIZE(nb6_devices),
};
#endif

/*
 * known 6368 boards
 */
#ifdef CONFIG_BCM63XX_CPU_6368
static struct board_info __initdata board_HG622 = {
	.name				= "96368MVWG_hg622",
	.expected_cpu_id	= 0x6368,

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
};

static struct board_info __initdata board_P870HW51A = {
	.name				= "96368VVW",
	.expected_cpu_id		= 0x6368,

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
};

static struct board_info __initdata board_VR3025u = {
	.name					= "96368M-1541N",
	.expected_cpu_id		= 0x6368,

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
};

static struct board_info __initdata board_VR3025un = {
	.name					= "96368M-1341N",
	.expected_cpu_id		= 0x6368,

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

	.spis = WAP5813n_spi_devices,
	.num_spis = ARRAY_SIZE(WAP5813n_spi_devices),
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

	.spis = DGND3700_3800B_spi_devices,
	.num_spis = ARRAY_SIZE(DGND3700_3800B_spi_devices),
};

static struct board_info __initdata board_96368mvwg = {
	.name				= "96368MVWG",
	.expected_cpu_id		= 0x6368,

	.has_pci			= 1,
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

	.has_ohci0 = 1,
	.has_ehci0 = 1,
};

static struct board_info __initdata board_96368mvngr = {
	.name				= "96368MVNgr",
	.expected_cpu_id		= 0x6368,

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

	.has_ohci0 = 1,
	.has_ehci0 = 1,
};
#endif

/*
 * all boards
 */
static const struct board_info __initconst *bcm963xx_boards[] = {
#ifdef CONFIG_BCM63XX_CPU_3368
	&board_cvg834g,
#endif
#ifdef CONFIG_BCM63XX_CPU_6328
	&board_96328avng,
	&board_AR5381u,
	&board_AR5387un,
	&board_963281TAN,
	&board_A4001N,
	&board_dsl_274xb_f1,
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
#endif

#ifdef CONFIG_BCM63XX_CPU_6368
	&board_DGND3700_3800B,
	&board_HG622,
	&board_P870HW51A,
	&board_VR3025u,
	&board_VR3025un,
	&board_WAP5813n,
	&board_96368mvwg,
	&board_96368mvngr,
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
	if (BCMCPU_IS_6328() || BCMCPU_IS_6362()) {
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

/*
 * third stage init callback, register all board devices.
 */
int __init board_register_devices(void)
{
	int i;

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

	if (board.has_usbd)
		bcm63xx_usbd_register(&board.usbd);

	if (board.has_ehci0)
		bcm63xx_ehci_register();

	if (board.has_ohci0)
		bcm63xx_ohci_register();

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
#ifdef CONFIG_BCMA
	if (!board.has_caldata &&
		bcma_arch_register_fallback_sprom(
		&bcm63xx_get_bcma_fallback_sprom) < 0)
		pr_err(PFX "failed to register BCMA fallback SPROM\n");
#endif

	bcm63xx_hsspi_register();

	bcm63xx_spi_register();

	if (board.num_devs)
		platform_add_devices(board.devs, board.num_devs);

	if (board.num_spis)
		spi_register_board_info(board.spis, board.num_spis);

	bcm63xx_flash_register(board.has_caldata, board.caldata);

	if (board.ephy_reset_gpio && board.ephy_reset_gpio_flags)
		gpio_request_one(board.ephy_reset_gpio,
				board.ephy_reset_gpio_flags, "ephy-reset");

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
