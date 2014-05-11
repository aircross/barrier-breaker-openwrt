#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/pcmcia-bcm63xx
  SUBMENU:=$(PCMCIA_MENU)
  TITLE:=Broadcom BCM63xx PCMCIA support
  DEPENDS:=@TARGET_brcm63xx +kmod-pcmcia-rsrc
  KCONFIG:=CONFIG_PCMCIA_BCM63XX
  FILES:=$(LINUX_DIR)/drivers/pcmcia/bcm63xx_pcmcia.ko
  AUTOLOAD:=$(call AutoLoad,41,bcm63xx_pcmcia)
endef

define KernelPackage/pcmcia-bcm63xx/description
  Kernel support for PCMCIA/CardBus controller on the BCM63xx SoC
endef

$(eval $(call KernelPackage,pcmcia-bcm63xx))

define KernelPackage/bcm63xx-udc
  SUBMENU:=$(USB_MENU)
  TITLE:=Broadcom BCM63xx UDC support
  DEPENDS:=@TARGET_brcm63xx +kmod-usb-gadget
  KCONFIG:=CONFIG_USB_BCM63XX_UDC
  FILES:=$(LINUX_DIR)/drivers/usb/gadget/bcm63xx_udc.ko
  AUTOLOAD:=$(call AutoLoad,51,bcm63xx_udc)
  $(call AddDepends/usb)
endef

define KernelPackage/bcm63xx-udc/description
  Kernel support for the USB gadget (device) controller on the BCM63xx SoC
endef

$(eval $(call KernelPackage,bcm63xx-udc))

define KernelPackage/bcm63xx-smp
  SUBMENU:=SMP Support
  TITLE:=Broadcom BCM63xx SMP support
  DEPENDS:=@(TARGET_brcm63xx_brcm6318||TARGET_brcm63xx_brcm6328||TARGET_brcm63xx_brcm6362||TARGET_brcm63xx_brcm6368||TARGET_brcm63xx_brcm63268)
  KCONFIG:= \
	CONFIG_CPU_RMAP=y \
	CONFIG_MUTEX_SPIN_ON_OWNER=y \
	CONFIG_NR_CPUS=2 \
	CONFIG_RCU_STALL_COMMON=y \
	CONFIG_RFS_ACCEL=y \
	CONFIG_RPS=y \
	CONFIG_SMP=y \
	CONFIG_SMP_UP=y \
	CONFIG_STOP_MACHINE=y \
	CONFIG_TREE_RCU=y \
	CONFIG_USE_GENERIC_SMP_HELPERS=y \
	CONFIG_XPS=y
endef

define KernelPackage/bcm63xx-smp/description
  Kernel support for SMP (Dual Core) on the BCM63xx SoC
endef

$(eval $(call KernelPackage,bcm63xx-smp))
