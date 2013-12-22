define Profile/VR3025U
  NAME:=Comtrend VR-3025u
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci kmod-usb2 wlc
endef
define Profile/VR3025U/Description
	Package set for the VR-3025u
endef
$(eval $(call Profile,VR3025U))
