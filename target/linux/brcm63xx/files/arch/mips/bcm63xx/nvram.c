/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2008 Maxime Bizon <mbizon@freebox.fr>
 * Copyright (C) 2008 Florian Fainelli <florian@openwrt.org>
 * Copyright (C) 2012 Jonas Gorski <jonas.gorski@gmail.com>
 */

#define pr_fmt(fmt) "bcm63xx_nvram: " fmt

#include <linux/init.h>
#include <linux/crc32.h>
#include <linux/export.h>
#include <linux/kernel.h>
#include <linux/if_ether.h>
#include <linux/sizes.h>
#include <linux/spinlock.h>

#include <bcm63xx_nvram.h>

/*
 * nvram structure
 */
struct bcm963xx_nvram {
	u32	version;
	u8	reserved1[256];
	u8	name[16];
	u32	main_tp_number;
	u32	psi_size;
	u32	mac_addr_count;
	u8	mac_addr_base[ETH_ALEN];
	u8	reserved2[2];
	u32	checksum_old;
	u8	reserved3[720];
	u32	checksum_high;
};

#define BCM63XX_DEFAULT_PSI_SIZE	SZ_64K

static struct bcm963xx_nvram nvram;
static int mac_addr_used;

/*
 * nvram sproms
 */
struct bcm63xx_nvram_sprom {
	int bus;
	int slot;
	u8 macaddr[ETH_ALEN];
};

static int bcm63xx_nvram_num_sproms;
static struct bcm63xx_nvram_sprom bcm63xx_nvram_sproms[NVRAM_MAX_SPROMS] = {
	{
		.slot = -1,
		.bus = -1,
	},
	{
		.slot = -1,
		.bus = -1,
	},
};

spinlock_t bcm63xx_sprom_lock;

/*
 * Required export for WL
 */
#define NVRAM_SPACE 0x8000
char nvram_buf[NVRAM_SPACE];
EXPORT_SYMBOL(nvram_buf);

void __init bcm63xx_nvram_init(void *addr)
{
	unsigned int check_len;
	u32 crc, expected_crc;
	u8 hcs_mac_addr[ETH_ALEN] = { 0x00, 0x10, 0x18, 0xff, 0xff, 0xff };

	/* Initialize bcma sprom lock */
	spin_lock_init(&bcm63xx_sprom_lock);

	/* extract nvram data */
	memcpy(&nvram, addr, sizeof(nvram));
	memcpy(&nvram_buf, addr, NVRAM_SPACE);

	/* check checksum before using data */
	if (nvram.version <= 4) {
		check_len = offsetof(struct bcm963xx_nvram, reserved3);
		expected_crc = nvram.checksum_old;
		nvram.checksum_old = 0;
	} else {
		check_len = sizeof(nvram);
		expected_crc = nvram.checksum_high;
		nvram.checksum_high = 0;
	}

	crc = crc32_le(~0, (u8 *)&nvram, check_len);

	if (crc != expected_crc)
		pr_warn("nvram checksum failed, contents may be invalid (expected %08x, got %08x)\n",
			expected_crc, crc);

	/* Cable modems have a different NVRAM which is embedded in the eCos
	 * firmware and not easily extractible, give at least a MAC address
	 * pool.
	 */
	if (BCMCPU_IS_3368()) {
		memcpy(nvram.mac_addr_base, hcs_mac_addr, ETH_ALEN);
		nvram.mac_addr_count = 2;
	}
}

u8 *bcm63xx_nvram_get_name(void)
{
	return nvram.name;
}
EXPORT_SYMBOL(bcm63xx_nvram_get_name);

int bcm63xx_nvram_get_mac_address(u8 *mac)
{
	u8 *oui;
	int count;

	if (mac_addr_used >= nvram.mac_addr_count) {
		pr_err("not enough mac addresses\n");
		return -ENODEV;
	}

	memcpy(mac, nvram.mac_addr_base, ETH_ALEN);
	oui = mac + ETH_ALEN/2 - 1;
	count = mac_addr_used;

	while (count--) {
		u8 *p = mac + ETH_ALEN - 1;

		do {
			(*p)++;
			if (*p != 0)
				break;
			p--;
		} while (p != oui);

		if (p == oui) {
			pr_err("unable to fetch mac address\n");
			return -ENODEV;
		}
	}

	mac_addr_used++;
	return 0;
}
EXPORT_SYMBOL(bcm63xx_nvram_get_mac_address);

unsigned int bcm63xx_nvram_get_psi_size(void)
{
	/* max is 64k, but some vendors use higher values */
	if (nvram.psi_size > 0 && nvram.psi_size <= 512)
		return nvram.psi_size * SZ_1K;

	return BCM63XX_DEFAULT_PSI_SIZE;
}
EXPORT_SYMBOL(bcm63xx_nvram_get_psi_size);

int bcm63xx_nvram_get_sprom_mac_address(int bus, int slot, u8 *mac_sprom)
{
	int i;
	struct bcm63xx_nvram_sprom *sprom = NULL;

	for (i = 0; i < bcm63xx_nvram_num_sproms; i++) {
		if (bcm63xx_nvram_sproms[i].bus == bus && bcm63xx_nvram_sproms[i].slot == slot) {
			sprom = &bcm63xx_nvram_sproms[i];
			break;
		}
	}

	if (!sprom) {
		int status = 0;
		u8 mac[ETH_ALEN];

		spin_lock(&bcm63xx_sprom_lock);

		if (bcm63xx_nvram_num_sproms < NVRAM_MAX_SPROMS) {
			sprom = &bcm63xx_nvram_sproms[bcm63xx_nvram_num_sproms];
			bcm63xx_nvram_num_sproms++;

			status = bcm63xx_nvram_get_mac_address(mac);
			if (status)
				pr_err("unable to get mac address\n");
		} else {
			pr_err("exceeded number of sproms\n");
			status = -EINVAL;
		}

		spin_unlock(&bcm63xx_sprom_lock);

		if (status)
			return status;

		sprom->bus = bus;
		sprom->slot = slot;
		memcpy(sprom->macaddr, mac, ETH_ALEN);
	}

	memcpy(mac_sprom, sprom->macaddr, ETH_ALEN);

	return 0;
}
EXPORT_SYMBOL(bcm63xx_nvram_get_sprom_mac_address);
