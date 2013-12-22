define Profile/SPW303V
  NAME:=T-Com SpeedPort W303V
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci kmod-usb2 wlc
endef
define Profile/SPW303V/Description
	Package set for the SpeedPort W303V
endef
$(eval $(call Profile,SPW303V))
