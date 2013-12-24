define Profile/CT536_CT5621
  NAME:=Comtrend CT-536+/CT-5621
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/CT536_CT5621/Description
	Package set for the CT-536+/CT-5621
endef
$(eval $(call Profile,CT536_CT5621))

define Profile/CT5365
  NAME:=Comtrend CT-5365
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/CT5365/Description
	Package set for the CT-5365
endef
$(eval $(call Profile,CT5365))
