define Profile/HG520
  NAME:=Huawei HG520v
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/HG520/Description
	Package set for the HG520v
endef
$(eval $(call Profile,HG520))

define Profile/HG553
  NAME:=Huawei HG553
  PACKAGES:=kmod-b43 kmod-usb-core kmod-usb-ohci kmod-usb2 wpad-mini
endef
define Profile/HG553/Description
	Package set for the HG553
endef
$(eval $(call Profile,HG553))

define Profile/HG556
  NAME:=Huawei HG556a
  PACKAGES:=kmod-ath9k kmod-rt2800-pci kmod-usb-ohci kmod-usb-core kmod-usb2 wpad-mini
endef
define Profile/HG556/Description
	Package set for the HG556a
endef
$(eval $(call Profile,HG556))
