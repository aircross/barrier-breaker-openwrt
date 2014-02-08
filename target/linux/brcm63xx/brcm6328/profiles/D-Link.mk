define Profile/DSL274XB
  NAME:=D-Link DSL-2740B/DSL-2741B rev F1
  PACKAGES:=kmod-ath9k kmod-usb-ohci kmod-usb-core kmod-usb2 wpad-mini
endef
define Profile/DSL274XB/Description
	Package set for the DSL-2740B/DSL-2741B rev F1
endef
$(eval $(call Profile,DSL274XB))
