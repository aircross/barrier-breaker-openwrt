define Profile/HG622
  NAME:=Huawei HG622
  PACKAGES:=kmod-rt2800-pci kmod-usb-ohci kmod-usb-core kmod-usb2 wpad-mini
endef
define Profile/HG622/Description
	Package set for the HG622
endef
$(eval $(call Profile,HG622))

define Profile/HG655x
  NAME:=Huawei HG655x
  PACKAGES:=kmod-rt2800-pci kmod-usb-ohci kmod-usb-core kmod-usb2 wpad-mini
endef
define Profile/HG655x/Description
	Package set for the HG655b HG655d
endef
$(eval $(call Profile,HG655x))
