define Profile/FAST2504
  NAME:=Sagem F@ST2504
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/FAST2504/Description
	Package set for the F@ST2504
endef
$(eval $(call Profile,FAST2504))
