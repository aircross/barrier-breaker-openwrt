#!/bin/sh
#
# Copyright (C) 2007 OpenWrt.org
#
#

board_name=""
board_model=""
status_led=""
status_led2=""
sys_mtd_part=""
brcm63xx_has_reset_button=""
ifname=""

brcm63xx_detect() {
	board_name=$(awk 'BEGIN{FS="[ \t:/]+"} /system type/ {print $4}' /proc/cpuinfo)

	if [ "$board_name" = "96358VW" ] && [ -n "$(swconfig dev eth1 help 2>/dev/null)" ]; then
		board_name="DVAG3810BN"
	fi

	case "$board_name" in
		"96318REF")
			board_mode="96318REF"
			ifname="eth0"
			;;
		"96318REF_P300")
			board_mode="96318REF_P300"
			ifname="eth0"
			;;
		"963268BU_P300")
			board_model="963268BU_P300"
			status_led="963268BU_P300:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"963269BHR")
			board_model="963269BHR"
			ifname="eth0"
			;;
		"96328avng")
			board_model="96328avng"
			status_led="96328avng::power"
			ifname="eth0"
			;;
		"96328A-1241N")
			board_model="Comtrend AR-5381u"
			status_led="AR-5381u:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"96328A-1441N1")
			board_model="Comtrend AR-5387un"
			status_led="AR-5387un:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"963281TAN")
			board_model="963281TAN"
			status_led="963281TAN::power"
			ifname="eth0"
			;;
		"963281T_TEF")
			board_model="ADB P.DG A4001N"
			status_led="A4001N:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"96348A-122")
			board_model="Comtrend CT-5365"
			status_led="CT-5365:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"96348GW")
			board_model="96348GW"
			status_led="96348GW:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth1"
			;;
		"96348GW-11")
			board_model="96348GW-11"
			status_led="96348GW-11:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth1"
			;;
		"96348W3")
			board_model="96348W3"
			status_led="96348W3:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth1"
			;;
		"96358-502V")
			board_model="SPW303V"
			status_led="spw303v:green:power+adsl"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"96358VW")
			board_model="96358VW"
			status_led="96358VW:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"96368M-1341N")
			board_model="Comtrend VR-3025un"
			status_led="VR-3025un:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"96368M-1541N")
			board_model="Comtrend VR-3025u"
			status_led="VR-3025u:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"96368MVWG_hg622")
			board_model="Huawei HG622"
			status_led="HG622:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"HW65x")
			board_model="Huawei HG655x"
			status_led="HW65x:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"96369R-1231N")
			board_model="Comtrend WAP-5813n"
			status_led="WAP-5813n:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"AGPF-S0")
			board_model="AGPF-S0"
			status_led="AGPF-S0:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"AR1004G")
			board_model="AR1004G"
			status_led="AR1004G:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"AW4139" |\
		"AW4339U")
			board_model="DSL-274XB"
			status_led="dsl-274xb:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"CPVA642")
			board_model="CPVA642"
			status_led="CPVA642:green:power:"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"CT536_CT5621")
			board_model="CT536_CT5621"
			status_led="CT536_CT5621:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"CVG834G_E15R3921")
			board_model="CVG834G_E15R3921"
			status_led="CVG834G:green:power"
			ifname="eth0"
			;;
		"D-4P-W")
			board_model="D-4P-W"
			status_led="D-4P-W:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"DGND3700_3800B")
			board_model="Netgear DGND3700/DGND3800B"
			status_led="DGND3700_3800B:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"DVAG3810BN")
			board_model="DVAG3810BN"
			status_led="DVAG3810BN::power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"DWV-S0")
			board_model="DWV-S0"
			status_led="DWV-S0:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"F@ST2504n")
			board_model="F@ST2504n"
			status_led="fast2504n:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"GW6000")
			board_model="GW6000"
			brcm63xx_has_reset_button="true"
			ifname="eth1"
			;;
		"GW6200")
			board_model="GW6200"
			status_led="GW6200:green:line1"
			status_led2="GW6200:green:tel"
			brcm63xx_has_reset_button="true"
			ifname="eth1"
			;;
		"HW553")
			board_model="Huawei HG553"
			status_led="HW553:blue:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"HW556")
			board_model="Huawei HG556a (Unknown)"
			status_led="HW556:red:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"HW556_A")
			board_model="Huawei HG556a (Ver A - Ralink)"
			status_led="HW556:red:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"HW556_B")
			board_model="Huawei HG556a (Ver B - Atheros)"
			status_led="HW556:red:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"HW556_C")
			board_model="Huawei HG556a (Ver C - Atheros)"
			status_led="HW556:red:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"HW6358GW_B")
			board_model="Huawei HG520v"
			status_led="HW520:green:net"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"NB6")
			board_model="NB6"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"P870HW-51a_v2")
			board_model="ZyXEL P870HW-51a v2"
			status_led="P870HW-51a:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"RTA770BW")
			board_model="RTA770BW"
			status_led="RTA770BW:green:diag"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"RTA770W")
			board_model="RTA770W"
			status_led="RTA770W:green:diag"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"SPW500V")
			board_model="SPW500V"
			status_led="SPW500V:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"V2110")
			board_model="V2110"
			status_led="V2110:power:green"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		"VW6339GU")
			board_model="VW6339GU"
			status_led="VW6339GU:green:power"
			brcm63xx_has_reset_button="true"
			ifname="eth0"
			;;
		*)
			;;
	esac

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"
	[ -e "/tmp/sysinfo/board_name" ] || echo "$board_name" > /tmp/sysinfo/board_name
	[ -e "/tmp/sysinfo/model" ] || echo "$board_model" > /tmp/sysinfo/model
}

brcm63xx_detect
