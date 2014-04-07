define Profile/FAST2704V2
  NAME:=Sagem F@ST2704 v2
  PACKAGES:=kmod-b43 kmod-usb-core kmod-usb-ohci kmod-usb2 wpad-mini
endef
define Profile/FAST2704V2/Description
	Package set for the F@ST2704 v2
endef
$(eval $(call Profile,FAST2704V2))
