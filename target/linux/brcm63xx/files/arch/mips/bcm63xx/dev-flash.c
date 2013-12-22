/*
 * Broadcom BCM63xx flash registration
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2008 Maxime Bizon <mbizon@freebox.fr>
 * Copyright (C) 2008 Florian Fainelli <florian@openwrt.org>
 * Copyright (C) 2012 Jonas Gorski <jonas.gorski@gmail.com>
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>

#include <bcm63xx_board.h>
#include <bcm63xx_cpu.h>
#include <bcm63xx_dev_flash.h>
#include <bcm63xx_dev_hsspi.h>
#include <bcm63xx_regs.h>
#include <bcm63xx_io.h>

int bcm63xx_attached_flash = -1;

static struct mtd_partition mtd_partitions[] = {
	{
		.name		= "cfe",
		.offset		= 0x0,
		.size		= 0x40000,
	}
};

static struct mtd_part_parser_data bcm63xx_parser_data;

static const char *bcm63xx_part_types[] = { "bcm63xxpart", "RedBoot", NULL };

static struct physmap_flash_data flash_data = {
	.width			= 2,
	.parts			= mtd_partitions,
	.part_probe_types	= bcm63xx_part_types,
	.pp_data		= &bcm63xx_parser_data,
};

static struct resource mtd_resources[] = {
	{
		.start		= 0,	/* filled at runtime */
		.end		= 0,	/* filled at runtime */
		.flags		= IORESOURCE_MEM,
	}
};

static struct platform_device mtd_dev = {
	.name			= "physmap-flash",
	.resource		= mtd_resources,
	.num_resources		= ARRAY_SIZE(mtd_resources),
	.dev			= {
		.platform_data	= &flash_data,
	},
};

static struct flash_platform_data bcm63xx_flash_data = {
	.part_probe_types	= bcm63xx_part_types,
	.pp_data		= &bcm63xx_parser_data,
};

static struct spi_board_info bcm63xx_spi_flash_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.mode		= 0,
		.max_speed_hz	= 781000,
		.modalias	= "m25p80",
		.platform_data	= &bcm63xx_flash_data,
	},
};

static int __init bcm63xx_detect_flash_type(void)
{
	u32 val;

	switch (bcm63xx_get_cpu_id()) {
	case BCM6328_CPU_ID:
		val = bcm_misc_readl(MISC_STRAPBUS_6328_REG);
		if (val & STRAPBUS_6328_HSSPI_CLK_FAST)
			bcm63xx_spi_flash_info[0].max_speed_hz = 33333334;
		else
			bcm63xx_spi_flash_info[0].max_speed_hz = 16666667;

		if (val & STRAPBUS_6328_BOOT_SEL_SERIAL)
			bcm63xx_attached_flash = BCM63XX_FLASH_TYPE_SERIAL;
		else
			bcm63xx_attached_flash = BCM63XX_FLASH_TYPE_NAND;
		break;
	case BCM6338_CPU_ID:
	case BCM6345_CPU_ID:
	case BCM6348_CPU_ID:
		/* no way to auto detect so assume parallel */
		bcm63xx_attached_flash = BCM63XX_FLASH_TYPE_PARALLEL;
		break;
	case BCM3368_CPU_ID:
	case BCM6358_CPU_ID:
		val = bcm_gpio_readl(GPIO_STRAPBUS_REG);
		if (val & STRAPBUS_6358_BOOT_SEL_PARALLEL)
			bcm63xx_attached_flash = BCM63XX_FLASH_TYPE_PARALLEL;
		else
			bcm63xx_attached_flash = BCM63XX_FLASH_TYPE_SERIAL;
		break;
	case BCM6362_CPU_ID:
		val = bcm_misc_readl(MISC_STRAPBUS_6362_REG);
		if (val & STRAPBUS_6362_BOOT_SEL_SERIAL)
			bcm63xx_attached_flash = BCM63XX_FLASH_TYPE_SERIAL;
		else
			bcm63xx_attached_flash = BCM63XX_FLASH_TYPE_NAND;
		break;
	case BCM6368_CPU_ID:
		val = bcm_gpio_readl(GPIO_STRAPBUS_REG);
		if (val & STRAPBUS_6368_SPI_CLK_FAST)
			bcm63xx_spi_flash_info[0].max_speed_hz = 20000000;

		switch (val & STRAPBUS_6368_BOOT_SEL_MASK) {
		case STRAPBUS_6368_BOOT_SEL_NAND:
			bcm63xx_attached_flash = BCM63XX_FLASH_TYPE_NAND;
			break;
		case STRAPBUS_6368_BOOT_SEL_SERIAL:
			bcm63xx_attached_flash = BCM63XX_FLASH_TYPE_SERIAL;
			break;
		case STRAPBUS_6368_BOOT_SEL_PARALLEL:
			bcm63xx_attached_flash = BCM63XX_FLASH_TYPE_PARALLEL;
			break;
		default:
			return -EINVAL;
		}
	default:
		return -EINVAL;
	}

	return 0;
}

int __init bcm63xx_flash_register(int num_caldata, struct bcm63xx_caldata *caldata)
{
	u32 val;
	unsigned int i;

	for (i = 0; i < num_caldata; i++)
		bcm63xx_parser_data.caldata[i] = caldata[i].caldata_offset;

	bcm63xx_detect_flash_type();

	switch (bcm63xx_attached_flash) {
	case BCM63XX_FLASH_TYPE_PARALLEL:
		/* read base address of boot chip select (0) */
		val = bcm_mpi_readl(MPI_CSBASE_REG(0));
		val &= MPI_CSBASE_BASE_MASK;

		/* BT Voyager 2500V has 8 Meg flash in two 4 Meg banks */
		/* Loading from CFE always uses Bank 0 */
		if (!strcmp(board_get_name(), "V2500V_BB")) {
			pr_info("V2500V: Start in Bank 0\n");
			val = val + 0x400000; // Select Bank 0 start address
		}

		mtd_resources[0].start = val;
		mtd_resources[0].end = 0x1FFFFFFF;

		return platform_device_register(&mtd_dev);
	case BCM63XX_FLASH_TYPE_SERIAL:
		if (BCMCPU_IS_6328() || BCMCPU_IS_6362()) {
			bcm63xx_spi_flash_info[0].bus_num = 1;
			bcm63xx_flash_data.max_transfer_len = HSSPI_BUFFER_LEN;
		}

		return spi_register_board_info(bcm63xx_spi_flash_info,
					ARRAY_SIZE(bcm63xx_spi_flash_info));
	case BCM63XX_FLASH_TYPE_NAND:
		pr_warn("unsupported NAND flash detected\n");
		return -ENODEV;
	default:
		pr_err("flash detection failed for BCM%x: %d\n",
		       bcm63xx_get_cpu_id(), bcm63xx_attached_flash);
		return -ENODEV;
	}
}
