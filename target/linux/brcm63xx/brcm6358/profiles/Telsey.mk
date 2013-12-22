define Profile/CPVA642
  NAME:=Telsey CPVA642
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci kmod-usb2 wlc
endef
define Profile/CPVA642/Description
	Package set for the CPVA642
endef
$(eval $(call Profile,CPVA642))
