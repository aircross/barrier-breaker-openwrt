define Profile/CT6373
  NAME:=Comtrend CT-6373
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci kmod-usb2 wlc
endef
define Profile/CT6373/Description
	Package set for the CT-6373
endef
$(eval $(call Profile,CT6373))
