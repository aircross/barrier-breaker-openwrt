#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

LOADADDR = 0x80010000		# RAM start + 16M 
KERNEL_ENTRY = $(LOADADDR)	# Newer kernels add a jmp to the kernel_entry at the start of the binary
RAMSIZE = 0x01000000		# 64MB

LOADER_MAKEOPTS= \
		KDIR=$(KDIR) \
		LOADADDR=$(LOADADDR) \
		KERNEL_ENTRY=$(KERNEL_ENTRY) \
		RAMSIZE=$(RAMSIZE)

define trxalign/jffs2-128k
-a 0x20000
endef
define trxalign/jffs2-64k
-a 0x10000
endef
define trxalign/squashfs
-a 1024
endef

define Image/LimitName16
$(shell expr substr "$(1)" 1 16)
endef

define Image/Build/CFE
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
		--output $(BIN_DIR)/openwrt-$(4)-$(1)-cfe.bin \
		--boardid $(2) --chipid $(3) --entry $(KERNEL_ENTRY) \
		--load-addr $(LOADADDR) --rsa-signature "$(5)" \
		--info1 "-$(call Image/LimitName16,$(4))" --info2 $(1) \
		$(6) $(7) $(8) $(9)
endef

define Image/Build/CFEFIXUP
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
		--output $(BIN_DIR)/openwrt-$(5)-$(1)-cfe.bin \
		--boardid $(2) --chipid $(4) --entry $(KERNEL_ENTRY) \
		--load-addr $(LOADADDR) --rsa-signature "$(6)" \
		--info1 "+$(call Image/LimitName16,$(3))" --info2 $(1) \
		$(7) $(8) $(9) $(10)
endef

define Image/Build/CFEHW553
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
		--output $(BIN_DIR)/openwrt-$(5)-$(1)-cfe.bin \
		--boardid $(2) --chipid $(3) --entry $(LOADADDR) \
		--load-addr $(LOADADDR) --tag-version 7 \
		--block-size 0x20000 --image-offset $(4)
endef

define Image/Build/CFEHW65x
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
		--output $(BIN_DIR)/openwrt-$(5)-$(1)-cfe.bin \
		--boardid $(2) --chipid $(3) --entry $(LOADADDR) \
		--load-addr $(LOADADDR) --tag-version 7 \
		--block-size 0x10000 --image-offset $(4) $(6)
endef

define Image/Build/CFEHW556
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
	--output $(BIN_DIR)/openwrt-$(2)-$(1)-cfe.bin \
	--boardid $(2) --chipid $(3) --entry $(LOADADDR) \
	--load-addr $(LOADADDR) --tag-version 8 --rsa-signature "$(5)" \
	--image-offset $(4) --info1 "$(6)" --block-size 0x20000
endef

define Image/Build/CFEAGPF
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
		--output $(BIN_DIR)/openwrt-$(5)-$(1)-cfe.bin \
		--boardid $(2) --chipid $(3) --entry $(LOADADDR) \
		--load-addr $(LOADADDR) --tag-version 8 \
		--signature2 IMAGE --block-size 0x20000 \
		--image-offset $(4) --info1 "-$(call Image/LimitName16,$(5))" --info2 $(1)
endef

define Image/Build/RG100A
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
		--output $(BIN_DIR)/openwrt-$(5)-$(1)-cfe.bin \
		--boardid $(2) --chipid $(3) --entry $(LOADADDR) \
		--load-addr $(LOADADDR) --block-size 0x20000 \
		--image-offset $(4) --info1 "-$(call Image/LimitName16,$(5))" --info2 $(1)
endef

define Image/Build/RedBoot
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/openwrt-$(1)-vmlinux.elf
	gzip -9 -c $(KDIR)/vmlinux > $(KDIR)/vmlinux.bin.gz
	$(STAGING_DIR_HOST)/bin/lzma e $(KDIR)/vmlinux $(KDIR)/vmlinux.bin.l7
	dd if=$(KDIR)/vmlinux.bin.l7 of=$(BIN_DIR)/openwrt-$(1)-vmlinux.lzma bs=65536 conv=sync
	dd if=$(KDIR)/vmlinux.bin.gz of=$(BIN_DIR)/openwrt-$(1)-vmlinux.gz bs=65536 conv=sync
endef

define Image/Build/SPW303V
	# Generate the tagged image
	$(STAGING_DIR_HOST)/bin/imagetag -i $(KDIR)/vmlinux.lzma.cfe -f $(KDIR)/root.$(1) \
		--output $(BIN_DIR)/openwrt-$(4)-$(1)-cfe.bin.tmp \
		--boardid $(2) --chipid $(3) --entry $(KERNEL_ENTRY) \
		--load-addr $(LOADADDR) --rsa-signature "$(5)" \
		--pad 4 $(6) $(7) $(8) $(9)
	# Fix up header
	$(STAGING_DIR_HOST)/bin/spw303v -i $(BIN_DIR)/openwrt-$(4)-$(1)-cfe.bin.tmp \
		-o $(BIN_DIR)/openwrt-$(4)-$(1)-cfe-sysupgrade.bin
	$(STAGING_DIR_HOST)/bin/xorimage -i $(BIN_DIR)/openwrt-$(4)-$(1)-cfe-sysupgrade.bin \
		-o $(BIN_DIR)/openwrt-$(4)-$(1)-cfe-factory.bin
	rm -f $(BIN_DIR)/openwrt-$(4)-$(1)-cfe.bin.tmp
endef

define Image/Build/CFEOLD
	$(TOPDIR)/scripts/brcmImage.pl -t -p	\
		-b $(2) -c $(3)			\
		-k $(KDIR)/vmlinux.lzma.cfe	\
		-r $(KDIR)/root.$(1)		\
		-o $(BIN_DIR)/openwrt-$(2)-$(1)-cfe.bin
endef

define Image/Build/HCS
	$(STAGING_DIR_HOST)/bin/hcsmakeimage --magic_byte=$(3) \
		--rev_maj=$(4) --rev_min=$(5) --input_file=$(6) \
		--output_file=$(BIN_DIR)/openwrt-$(2)-$(1).bin
endef

define Build/Clean
	$(MAKE) -C lzma-loader clean
endef

define Image/Prepare
	# Standard LZMA kernel
	cat $(KDIR)/vmlinux | $(STAGING_DIR_HOST)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma

	# CFE is a LZMA nazi! It took me hours to find out the parameters!
	# Also I think lzma has a bug cause it generates different output depending on
	# if you use stdin / stdout or not. Use files instead of stdio here, cause
	# otherwise CFE will complain and not boot the image.
	$(STAGING_DIR_HOST)/bin/lzma e -d22 -fb64 -a1 $(KDIR)/vmlinux $(KDIR)/vmlinux.lzma.tmp

	# Strip out the length, CFE doesn't like this
	dd if=$(KDIR)/vmlinux.lzma.tmp of=$(KDIR)/vmlinux.lzma.cfe bs=5 count=1
	dd if=$(KDIR)/vmlinux.lzma.tmp of=$(KDIR)/vmlinux.lzma.cfe ibs=13 obs=5 skip=1 seek=1 conv=notrunc
	rm -f $(KDIR)/vmlinux.lzma.tmp

	# Build the LZMA loader
	rm -f $(KDIR)/loader.gz
	$(MAKE) -C lzma-loader \
		BUILD_DIR="$(KDIR)" \
		TARGET="$(KDIR)" \
		clean install

	echo -ne "\\x00" >> $(KDIR)/loader.gz
	rm -f $(KDIR)/fs_mark
	touch $(KDIR)/fs_mark
	$(call prepare_generic_squashfs,$(KDIR)/fs_mark)
endef

ifeq ($(CONFIG_TARGET_brcm63xx_brcm3368),y)
  define Image/Build/Initramfs
	# Netgear CVG834G
	$(call Image/Build/HCS,initramfs,cvg834g,a020,0001,0022,$(KDIR)/vmlinux-initramfs)
  endef

  # Netgear CVG834G
  define Image/Build/Profile/CVG834G
	$(call Image/Build/HCS,$(1),cvg834g,a020,0001,0022,$(KDIR)/vmlinux)
  endef
endif

ifeq ($(CONFIG_TARGET_brcm63xx_brcm6318),y)
  # Generic
  define Image/Build/Profile/96318REF
	$(call Image/Build/CFE,$(1),96318REF,6318,96318REF-generic)
  endef
  define Image/Build/Profile/96318REF_P300
	$(call Image/Build/CFE,$(1),96318REF_P300,6318,96318REF_P300-generic)
  endef
endif

ifeq ($(CONFIG_TARGET_brcm63xx_brcm63268),y)
  # Generic
  define Image/Build/Profile/963268BU_P300
	$(call Image/Build/CFE,$(1),963268BU_P300,63268,963268BU_P300-generic)
  endef
  define Image/Build/Profile/963269BHR
	$(call Image/Build/CFE,$(1),963269BHR,63268,963269BHR-generic)
  endef

  # Ayecom VW6339GU
  define Image/Build/Profile/VW6339GU
	$(call Image/Build/CFE,$(1),VW6339GU,63268,VW6339GU-generic)
  endef
endif

ifeq ($(CONFIG_TARGET_brcm63xx_brcm6328),y)
  # Generic
  define Image/Build/Profile/96328AVNG
	$(call Image/Build/CFE,$(1),96328avng,6328,96328avng-4M-generic,,--pad 2)
	$(call Image/Build/CFE,$(1),96328avng,6328,96328avng-8M-generic,,--pad 4)
	$(call Image/Build/CFE,$(1),96328avng,6328,96328avng-16M-generic,,--pad 8)
  endef
  define Image/Build/Profile/963281TAN
	$(call Image/Build/CFE,$(1),963281TAN,6328,963281TAN-4M-generic,,--pad 2)
	$(call Image/Build/CFE,$(1),963281TAN,6328,963281TAN-8M-generic,,--pad 4)
	$(call Image/Build/CFE,$(1),963281TAN,6328,963281TAN-16M-generic,,--pad 8)
  endef

  # ADB P.DG A4001N
  define Image/Build/Profile/PDGA4001N
	$(call Image/Build/CFE,$(1),963281T_TEF,6328,PDG-A4001N,,--pad 8)
  endef

  # Comtrend AR-5381u
  define Image/Build/Profile/AR5381U
	$(call Image/Build/CFE,$(1),96328A-1241N,6328,AR-5381u,,--pad 8)
  endef
  # Comtrend AR-5387un
  define Image/Build/Profile/AR5387UN
	$(call Image/Build/CFE,$(1),96328A-1441N1,6328,AR-5387un,,--pad 8)
  endef

  # D-Link DSL-2740B/DSL-2741B rev F1
  define Image/Build/Profile/DSL274XB
	$(call Image/Build/CFE,$(1),AW4339U,6328,DSL274XB-F1-AU,,--signature2="4.06.01.AUF1" --pad 4)
	$(call Image/Build/CFE,$(1),AW4339U,6328,DSL274XB-F1-EU,,--signature2="4.06.01.EUF1" --pad 4)
  endef

  # Sagem F@ST2704 v2
  define Image/Build/Profile/FAST2704V2
	$(call Image/Build/CFE,$(1),F@ST2704V2,6328,F@ST2704V2-cfe)
	$(call Image/Build/CFE,$(1),F@ST2704V2,6328,F@ST2704V2,OpenWRT-$(REVISION))
  endef
endif

ifeq ($(CONFIG_TARGET_brcm63xx_brcm6338),y)
  # Generic
  define Image/Build/Profile/6338GW
	$(call Image/Build/CFE,$(1),6338GW,6338,6338GW-generic,,)
  endef
  define Image/Build/Profile/6338W
	$(call Image/Build/CFE,$(1),6338W,6338,6338W-generic,,)
  endef
  define Image/Build/Profile/RTA1320_16M
	$(call Image/Build/CFE,$(1),RTA1320_16M,6338,RTA1320_16M,,--layoutver 5)
  endef

  # TP-Link TD-8810A, TD-8810B, TD-8811A, TD-8811B
  define Image/Build/Profile/8L2M8M
	$(call Image/Build/CFE,$(1),8L-2M-8M,6338,TL-8810_8811)
  endef
endif

ifeq ($(CONFIG_TARGET_brcm63xx_brcm6345),y)
  # Generic
  define Image/Build/Profile/96345GW2
	$(call Image/Build/CFE,$(1),96345GW2,6345,96345GW2-generic)
	$(call Image/Build/CFE,$(1),96345GW2,6345,96345GW2-bc221,,--layoutver 5)
	$(call Image/Build/CFE,$(1),96345GW2,6345,92345GW2-rev,OpenWRT-$(REVISION))
  endef
endif

ifeq ($(CONFIG_TARGET_brcm63xx_brcm6348),y)
  # Generic
  define Image/Build/Profile/96348GW
	$(call Image/Build/CFE,$(1),96348GW,6348,96348GW-generic,,)
	$(call Image/Build/CFE,$(1),96348GW,6348,96348GW-bc221,,--layoutver 5)
  endef
  define Image/Build/Profile/96348GW10
    $(call Image/Build/CFE,$(1),96348GW-10,6348,96348GW-10-generic)
  endef
  define Image/Build/Profile/96348GW11
	$(call Image/Build/CFE,$(1),96348GW-11,6348,96348GW-11-generic)
  endef
  define Image/Build/Profile/96348R
	$(call Image/Build/CFE,$(1),96348R,6348,96348R-generic,,)
  endef
  define Image/Build/Profile/RTA1025W_16
	$(call Image/Build/CFE,$(1),RTA1025W_16,6348,RTA1025W_16,,--layoutver 5)
  endef

  # Asmax AR1004G
  define Image/Build/Profile/AR1004G
	$(call Image/Build/CFEFIXUP,$(1),96348GW-10,AR1004G,6348,AR1004G)
  endef

  # Belkin F5D7633
  define Image/Build/Profile/F5D7633
	$(call Image/Build/CFE,$(1),96348GW-10,6348,F5D7633)
  endef

  # BT Voyager V210_BTR
  define Image/Build/Profile/V210_BB
	$(call Image/Build/CFE,$(1),V210_BB,6348,BTV210_BTR,,--layoutver 5)
  endef
  # BT Voyager V210_ROI, V210_WB
  define Image/Build/Profile/V210
	$(call Image/Build/CFE,$(1),V210,6348,BTV210_ROI_WB,,--layoutver 5)
  endef
  # BT Voyager V2091_BTR
  define Image/Build/Profile/V2091_BB
	$(call Image/Build/CFE,$(1),V2091_BB,6348,BTV2091_BTR,,--layoutver 5)
  endef
  # BT Voyager V2091_ROI, V2091_WB
  define Image/Build/Profile/V2091
	$(call Image/Build/CFE,$(1),V2091,6348,BTV2091_ROI_WB,,--layoutver 5)
  endef
  # BT Voyager V220V, V220V_MGCP_BTR
  define Image/Build/Profile/RTA1052V
	$(call Image/Build/CFE,$(1),RTA1052V,6348,BTV220V_MGCP_BTR,,--layoutver 5)
  endef
  # BT Voyager V2110, V2110_AA, V2110_ROI
  define Image/Build/Profile/V2110
	$(call Image/Build/CFE,$(1),V2110,6348,BTV2110,,--layoutver 5)
  endef
  # BT Voyager V2500V, V2500V_SIP_CLUB, V2500V_AA
  define Image/Build/Profile/V2500V_BB
	$(call Image/Build/CFE,$(1),V2500V_BB,6348,BTV2500V,,--layoutver 5)
  endef

  # Comtrend CT-536+, CT-5621
  define Image/Build/Profile/CT536_CT5621
	$(call Image/Build/CFEFIXUP,$(1),96348GW-11,CT536_CT5621,6348,CT536_CT5621)
  endef
  # Comtrend CT-5365
  define Image/Build/Profile/CT5365
	$(call Image/Build/CFE,$(1),96348A-122,6348,CT-5365)
  endef

  # Davolink DV201AMR
  define Image/Build/Profile/DV201AMR
	$(call Image/Build/CFEOLD,$(1),DV201AMR,6348)
  endef

  # D-Link DSL-2640B, rev B2
  define Image/Build/Profile/DSL2640BB2
	$(call Image/Build/CFE,$(1),D-4P-W,6348,DSL2640B-B2)
  endef

  # NetGear DG834GT, DG834PN
  define Image/Build/Profile/DG834GT_DG834PN
	$(call Image/Build/CFE,$(1),96348GW-10,6348,DG834GT_DG834PN)
  endef

  # Sagem F@ST2404
  define Image/Build/Profile/FAST2404
	$(call Image/Build/CFE,$(1),F@ST2404,6348,F@ST2404-cfe)
	$(call Image/Build/CFE,$(1),F@ST2404,6348,F@ST2404,OpenWRT-$(REVISION))
  endef
  # Sagem F@ST2604
  define Image/Build/Profile/FAST2604
	$(call Image/Build/CFE,$(1),F@ST2604,6348,F@ST2604-cfe)
	$(call Image/Build/CFE,$(1),F@ST2604,6348,F@ST2604,OpenWRT-$(REVISION))
  endef

  # T-Com SpeedPort W500V
  define Image/Build/Profile/SPW500V
	$(call Image/Build/CFEFIXUP,$(1),96348GW,SPW500V,6348,SPW500V)
  endef

  # Tecom GW6000
  define Image/Build/Profile/GW6000
	$(call Image/Build/CFEFIXUP,$(1),96348GW,GW6000,6348,GW6000)
  endef
  # Tecom GW6200
  define Image/Build/Profile/GW6200
	$(call Image/Build/CFEFIXUP,$(1),96348GW,GW6200,6348,GW6200,$(shell printf '\x99'))
  endef

  # TP-Link 8900GB
  define Image/Build/Profile/TL8900GB
	$(call Image/Build/CFE,$(1),96348GW-11,6348,TL-8900GB,$(shell printf 'PRID\x89\x10\x00\x02'))
  endef

  # USR 9108
  define Image/Build/Profile/USR9108
	$(call Image/Build/CFE,$(1),96348GW-A,6348,USR9108)
  endef
endif

ifeq ($(CONFIG_TARGET_brcm63xx_brcm6358),y)
  # Generic
  define Image/Build/Profile/96358VW
	$(call Image/Build/CFE,$(1),96358VW,6358,96358VW-generic)
  endef
  define Image/Build/Profile/96358VW2
	$(call Image/Build/CFE,$(1),96358VW2,6358,96358VW2-generic)
  endef
  define Image/Build/Profile/RG100A_DB120
	$(call Image/Build/RG100A,$(1),96358VW2,6358,0x20000,RG100A_DB120)
  endef

  # Comtrend CT-6373
  define Image/Build/Profile/CT6373
	$(call Image/Build/CFE,$(1),CT6373-1,6358,CT-6373)
  endef

  # D-Link DSL-2650U
  define Image/Build/Profile/DSL2650U
	$(call Image/Build/CFE,$(1),96358VW2,6358,DSL-2650U)
  endef
  # D-Link DSL-2740B/DSL-2741B rev C2
  define Image/Build/Profile/DSL274XBC2
	$(call Image/Build/CFEFIXUP,$(1),96358GW,AW4139,6358,DSL274XB-C2)
  endef
  # D-Link DVA-G3810
  define Image/Build/Profile/DVAG3810BN
	$(call Image/Build/CFEFIXUP,$(1),96358VW,DVAG3810BN,6358,DVAG3810BN)
  endef

  # Huawei HG520v
  define Image/Build/Profile/HG520
	$(call Image/Build/CFE,$(1),HW6358GW_B,6358,HG520v)
  endef
  # Huawei HG553
  define Image/Build/Profile/HG553
	$(call Image/Build/CFEHW553,$(1),HW553,6358,0x20000,HW553)
  endef
  # Huawei HG556a
  define Image/Build/Profile/HG556
	$(call Image/Build/CFEHW556,$(1),HW556,6358,0x20000,EchoLife_HG556a,OpenWRT-$(REVISION))
  endef

  # NeufBox 4
  define Image/Build/Profile/NB4
	$(call Image/Build/CFE,$(1),96358VW,6358,NB4,OpenWRT-$(REVISION))
  endef

  # Pirelli AGV2+W
  define Image/Build/Profile/AGV2W
	$(call Image/Build/CFEAGPF,$(1),AGPF-S0,6358,0x20000,AGV2+W-cfe)
	$(call Image/Build/CFEAGPF,$(1),AGPF-S0,6358,0x20000,AGV2+W)
  endef
  # Pirelli A226G
  define Image/Build/Profile/A226G
	$(call Image/Build/CFEAGPF,$(1),DWV-S0,6358,0x10000,A226G-cfe)
	$(call Image/Build/CFEAGPF,$(1),DWV-S0,6358,0x10000,A226G)
  endef
  # Pirelli A226M
  define Image/Build/Profile/A226M
	$(call Image/Build/CFEAGPF,$(1),DWV-S0,6358,0x20000,A226M-cfe)
	$(call Image/Build/CFEAGPF,$(1),DWV-S0,6358,0x20000,A226M)
  endef

  # Sagem F@ST2504
  define Image/Build/Profile/FAST2504
	$(call Image/Build/CFE,$(1),F@ST2504n,6362,F@ST2504n,OpenWRT-$(REVISION))
  endef

  # T-Com SpeedPort W303V
  define Image/Build/Profile/SPW303V
	$(call Image/Build/SPW303V,$(1),96358-502V,6358,SPW303V)
  endef

  # Telsey CPVA642
  define Image/Build/Profile/CPVA642
	$(call Image/Build/CFE,$(1),CPVA642,6358,CPA-ZNTE60T,,--signature "Telsey Tlc",--signature2 "99.99.999",--second-image-flag "0")
  endef
endif

ifeq ($(CONFIG_TARGET_brcm63xx_brcm6362),y)
  # NeufBox 6
  define Image/Build/Profile/NB6
	$(call Image/Build/CFE,$(1),NB6-SER-r0,6362,NEUFBOX6,OpenWRT-$(REVISION))
  endef
endif

ifeq ($(CONFIG_TARGET_brcm63xx_brcm6368),y)
  # Generic
  define Image/Build/Profile/96368MVNgr
	$(call Image/Build/CFE,$(1),96368MVNgr,6368,96368MVNgr-generic)
  endef
  define Image/Build/Profile/96368MVWG
	$(call Image/Build/CFE,$(1),96368MVWG,6368,96368MVWG-generic)
  endef

  # Comtrend VR-3025u
  define Image/Build/Profile/VR3025U
	$(call Image/Build/CFE,$(1),96368M-1541N,6368,VR-3025u,,--pad 16)
  endef
  # Comtrend VR-3025un
  define Image/Build/Profile/VR3025UN
	$(call Image/Build/CFE,$(1),96368M-1341N,6368,VR-3025un,,--pad 4)
  endef
  # Comtrend WAP-5813n
  define Image/Build/Profile/WAP5813N
	$(call Image/Build/CFE,$(1),96369R-1231N,6368,WAP-5813n,,--pad 4)
  endef

  # Huawei HG622
  define Image/Build/Profile/HG622
	$(call Image/Build/CFE,$(1),96368MVWG_hg622,6368,HG622,,--pad 4)
  endef

  # Huawei HG655b/d
  define Image/Build/Profile/HG655x
	$(call Image/Build/CFEHW65x,$(1),HW65x,6368,0x20000,HG655x,--pad 4)
  endef

  # Netgear DGND3700/DGND3800B DGND3700_3800B
  define Image/Build/Profile/DGND3700_3800B
	$(call Image/Build/CFEFIXUP,$(1),96368MVWG,DGND3700_3800B,6368,DGND3700_3800B,,--image-offset 0x20000)
  endef

  # ZyXEL P870HW-51a v2
  define Image/Build/Profile/P870HW51A_V2
	$(call Image/Build/CFEFIXUP,$(1),96368VVW,P870HW-51a_v2,6368,P870HW-51a_v2)
  endef
endif

define Image/Build
  dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync

  $(call Image/Build/Profile/$(PROFILE),$(1))

  ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	$(call Image/Build/Initramfs)
  endif
endef

$(eval $(call BuildImage))
