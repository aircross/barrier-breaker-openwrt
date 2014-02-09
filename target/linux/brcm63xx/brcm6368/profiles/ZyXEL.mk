define Profile/P870HW51A_V2
  NAME:=ZyXEL P870HW-51a v2
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/P870HW51A_V2/Description
	Package set for the P870HW-51a v2
endef
$(eval $(call Profile,P870HW51A_V2))
