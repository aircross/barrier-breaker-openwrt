define Profile/PDGA4001N
  NAME:=ADB P.DG A4001N
  PACKAGES:=kmod-brcmsmac kmod-usb-core kmod-usb-ohci kmod-usb2 wpad-mini
endef
define Profile/PDGA4001N/Description
	Package set for the P.DG A4001N
endef
$(eval $(call Profile,PDGA4001N))
