#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=brcm63xx
BOARDNAME:=Broadcom BCM63xx
SUBTARGETS:=brcm3368 brcm6318 brcm6328 brcm6338 brcm6345 brcm6348 brcm6358 brcm6362 brcm6368 brcm63268
FEATURES:=squashfs usb atm pci pcmcia usbgadget
LINUX_VERSION:=3.10.34
MAINTAINER:=Florian Fainelli <florian@openwrt.org>

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += swconfig kmod-input-gpio-keys-polled kmod-button-hotplug

define Target/Description
	Build firmware images for Broadcom based xDSL/routers.
	Broadcom 3368/6318/6328/6338/6345/6348/6358/6362/6368/63268.
endef

$(eval $(call BuildTarget))
