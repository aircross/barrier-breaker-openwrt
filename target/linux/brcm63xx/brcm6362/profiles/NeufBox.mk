define Profile/NB6
  NAME:=NeufBox 6
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci kmod-usb2 wlc
endef
define Profile/NB6/Description
	Package set for the NeufBox 6
endef
$(eval $(call Profile,NB6))
