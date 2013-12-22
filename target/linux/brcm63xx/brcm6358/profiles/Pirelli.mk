define Profile/AGV2W
  NAME:=Pirelli AGV2+W
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci kmod-usb2 wlc
endef
define Profile/AGV2W/Description
	Package set for the AGV2+W
endef
$(eval $(call Profile,AGV2W))

define Profile/A226G
  NAME:=Pirelli A226G
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci kmod-usb2 wlc
endef
define Profile/A226G/Description
	Package set for the A226G
endef
$(eval $(call Profile,A226G))

define Profile/A226M
  NAME:=Pirelli A226M
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci kmod-usb2 wlc
endef
define Profile/A226M/Description
	Package set for the A226M
endef
$(eval $(call Profile,A226M))
