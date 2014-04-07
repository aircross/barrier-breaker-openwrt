define Profile/NB4
  NAME:=NeufBox 4
  PACKAGES:=kmod-b43 kmod-usb-core kmod-usb-ohci kmod-usb2 wpad-mini
endef
define Profile/NB4/Description
	Package set for the NeufBox 4
endef
$(eval $(call Profile,NB4))
