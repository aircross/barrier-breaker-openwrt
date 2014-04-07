define Profile/VR3025U
  NAME:=Comtrend VR-3025u
  PACKAGES:=kmod-b43 kmod-usb-core kmod-usb-ohci kmod-usb2 wpad-mini
endef
define Profile/VR3025U/Description
	Package set for the VR-3025u
endef
$(eval $(call Profile,VR3025U))

define Profile/VR3025UN
  NAME:=Comtrend VR-3025un
  PACKAGES:=kmod-b43 kmod-usb-core kmod-usb-ohci kmod-usb2 wpad-mini
endef
define Profile/VR3025UN/Description
	Package set for the VR-3025un
endef
$(eval $(call Profile,VR3025UN))
