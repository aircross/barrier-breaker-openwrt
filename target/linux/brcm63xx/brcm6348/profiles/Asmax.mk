define Profile/AR1004G
  NAME:=Asmax AR1004G
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/AR1004G/Description
	Package set for the AR1004G
endef
$(eval $(call Profile,AR1004G))
