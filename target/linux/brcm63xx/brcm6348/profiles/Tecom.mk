define Profile/GW6000
  NAME:=Tecom GW6000
  PACKAGES:=kmod-b43 kmod-usb-core kmod-usb-ohci wpad-mini
endef
define Profile/GW6000/Description
	Package set for the GW6000
endef
$(eval $(call Profile,GW6000))

define Profile/GW6200
  NAME:=Tecom GW6200
  PACKAGES:=kmod-b43 kmod-usb-core kmod-usb-ohci wpad-mini
endef
define Profile/GW6200/Description
	Package set for the GW6200
endef
$(eval $(call Profile,GW6200))
