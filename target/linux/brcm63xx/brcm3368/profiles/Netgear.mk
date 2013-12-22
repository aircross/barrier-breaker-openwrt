define Profile/CVG834G
  NAME:=Netgear CVG834G
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci kmod-usb2 wlc
endef
define Profile/CVG834G/Description
	Package set for the CVG834G
endef
$(eval $(call Profile,CVG834G))
