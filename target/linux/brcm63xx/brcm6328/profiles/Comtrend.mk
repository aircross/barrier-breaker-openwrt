define Profile/AR5381U
  NAME:=Comtrend AR-5381u
  PACKAGES:=kmod-usb-core kmod-usb2
endef
define Profile/AR5381U/Description
	Package set for the AR-5381u
endef
$(eval $(call Profile,AR5381U))

define Profile/AR5387UN
  NAME:=Comtrend AR-5387un
  PACKAGES:=kmod-usb-core kmod-usb2
endef
define Profile/AR5387UN/Description
	Package set for the AR-5387un
endef
$(eval $(call Profile,AR5387UN))
