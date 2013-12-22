define Profile/NB4
  NAME:=NeufBox 4
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci kmod-usb2 wlc
endef
define Profile/NB4/Description
	Package set for the NeufBox 4
endef
$(eval $(call Profile,NB4))
