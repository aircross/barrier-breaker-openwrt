define Profile/P870HW51AV2
  NAME:=ZyXEL P870HW-51a v2
  PACKAGES:=kmod-brcm-wl wlc
endef
define Profile/P870HW51AV2/Description
	Package set for the P870HW-51a v2
endef
$(eval $(call Profile,P870HW51AV2))
