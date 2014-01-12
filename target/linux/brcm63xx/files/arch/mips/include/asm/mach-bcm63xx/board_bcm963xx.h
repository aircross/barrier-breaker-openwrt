#ifndef BOARD_BCM963XX_H_
#define BOARD_BCM963XX_H_

#include <linux/types.h>
#include <linux/gpio.h>
#include <linux/gpio_keys.h>
#include <linux/leds.h>
#include <bcm63xx_dev_enet.h>
#include <bcm63xx_dev_usb_usbd.h>
#include <bcm63xx_dev_dsp.h>
#include <pci_ath9k_fixup.h>
#include <pci_rt2x00_fixup.h>

/*
 * flash mapping
 */
#define BCM963XX_CFE_VERSION_OFFSET	0x570
#define BCM963XX_NVRAM_OFFSET		0x580

struct bcm63xx_caldata {
	unsigned int	vendor;
	unsigned int	slot;
	u32		caldata_offset;
	/* Atheros */
	unsigned int	endian_check:1;
	int		led_pin;
	/* Ralink */
	char*		eeprom;
};

/*
 * board definition
 */
struct board_info {
	u8		name[16];
	unsigned int	expected_cpu_id;

	/* enabled feature/device */
	unsigned int	has_enet0:1;
	unsigned int	has_enet1:1;
	unsigned int	has_enetsw:1;
	unsigned int	has_pci:1;
	unsigned int	has_pccard:1;
	unsigned int	has_ohci0:1;
	unsigned int	has_ehci0:1;
	unsigned int	has_usbd:1;
	unsigned int	has_dsp:1;
	unsigned int	has_caldata:2;

	/* wifi calibration data config */
	struct bcm63xx_caldata caldata[2];

	/* ethernet config */
	struct bcm63xx_enet_platform_data enet0;
	struct bcm63xx_enet_platform_data enet1;
	struct bcm63xx_enetsw_platform_data enetsw;

	/* USB config */
	struct bcm63xx_usbd_platform_data usbd;

	/* DSP config */
	struct bcm63xx_dsp_platform_data dsp;

	/* External PHY reset GPIO */
	unsigned int ephy_reset_gpio;

	/* External PHY reset GPIO flags from gpio.h */
	unsigned long ephy_reset_gpio_flags;

	/* Additional platform devices */
	struct platform_device **devs;
	unsigned int	num_devs;

	/* Additional platform devices */
	struct spi_board_info *spis;
	unsigned int	num_spis;
};

#endif /* ! BOARD_BCM963XX_H_ */
