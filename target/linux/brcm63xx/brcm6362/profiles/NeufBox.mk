define Profile/NB6
  NAME:=NeufBox 6
  PACKAGES:=kmod-b43 kmod-usb-core kmod-usb-ohci kmod-usb2 wpad-mini
endef
define Profile/NB6/Description
	Package set for the NeufBox 6
endef
$(eval $(call Profile,NB6))
