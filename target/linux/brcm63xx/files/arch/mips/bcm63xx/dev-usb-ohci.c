/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2008 Maxime Bizon <mbizon@freebox.fr>
 * Copyright (C) 2013 Florian Fainelli <florian@openwrt.org>
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/usb/ohci_pdriver.h>
#include <linux/dma-mapping.h>
#include <linux/clk.h>
#include <linux/delay.h>

#include <bcm63xx_cpu.h>
#include <bcm63xx_regs.h>
#include <bcm63xx_io.h>
#include <bcm63xx_usb_priv.h>
#include <bcm63xx_dev_usb_ohci.h>

static struct resource ohci_resources[] = {
	{
		.start		= -1, /* filled at runtime */
		.end		= -1, /* filled at runtime */
		.flags		= IORESOURCE_MEM,
	},
	{
		.start		= -1, /* filled at runtime */
		.flags		= IORESOURCE_IRQ,
	},
};

static u64 ohci_dmamask = DMA_BIT_MASK(32);

static struct clk *usb_host_clock;

static int bcm63xx_ohci_power_on(struct platform_device *pdev)
{
	usb_host_clock = clk_get(&pdev->dev, "usbh");
	if (IS_ERR_OR_NULL(usb_host_clock))
		return -ENODEV;

	clk_prepare_enable(usb_host_clock);

	bcm63xx_usb_priv_ohci_cfg_set();

	return 0;
}

static void bcm63xx_ohci_power_off(struct platform_device *pdev)
{
	if (!IS_ERR_OR_NULL(usb_host_clock)) {
		clk_disable_unprepare(usb_host_clock);
		clk_put(usb_host_clock);
	}
}

static struct usb_ohci_pdata bcm63xx_ohci_pdata = {
	.big_endian_desc	= 1,
	.big_endian_mmio	= 1,
	.no_big_frame_no	= 1,
	.num_ports		= 1,
	.power_on		= bcm63xx_ohci_power_on,
	.power_off		= bcm63xx_ohci_power_off,
	.power_suspend		= bcm63xx_ohci_power_off,
};

static struct platform_device bcm63xx_ohci_device = {
	.name		= "ohci-platform",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(ohci_resources),
	.resource	= ohci_resources,
	.dev		= {
		.platform_data		= &bcm63xx_ohci_pdata,
		.dma_mask		= &ohci_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
};

int __init bcm63xx_ohci_register(void)
{
	if (BCMCPU_IS_6345() || BCMCPU_IS_6338())
		return -ENODEV;

	ohci_resources[0].start = bcm63xx_regset_address(RSET_OHCI0);
	ohci_resources[0].end = ohci_resources[0].start;
	ohci_resources[0].end += RSET_OHCI_SIZE - 1;
	ohci_resources[1].start = bcm63xx_get_irq_number(IRQ_OHCI0);

	return platform_device_register(&bcm63xx_ohci_device);
}
