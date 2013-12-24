define Profile/DGND3700_3800B
  NAME:=Netgear DGND3700/DGND3800B
  PACKAGES:=kmod-b43 kmod-usb-core kmod-usb-ohci kmod-usb2 wpad-mini
endef
define Profile/DGND3700_3800B/Description
	Package set for the DGND3700/DGND3800B
endef
$(eval $(call Profile,DGND3700_3800B))
