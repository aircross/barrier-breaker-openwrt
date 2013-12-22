define Profile/GW6000
  NAME:=Tecom GW6000
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci wlc
endef
define Profile/GW6000/Description
	Package set for the GW6000
endef
$(eval $(call Profile,GW6000))

define Profile/GW6200
  NAME:=Tecom GW6200
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci wlc
endef
define Profile/GW6200/Description
	Package set for the GW6200
endef
$(eval $(call Profile,GW6200))
