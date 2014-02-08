#ifndef __BCM63XX_FLASH_H
#define __BCM63XX_FLASH_H

#include <board_bcm963xx.h>

enum {
	BCM63XX_FLASH_TYPE_PARALLEL,
	BCM63XX_FLASH_TYPE_SERIAL,
	BCM63XX_FLASH_TYPE_NAND,
};

void bcm63xx_flash_detect(void);

int __init bcm63xx_flash_register(int num_caldata, struct bcm63xx_caldata *caldata);

int bcm63xx_flash_get_type(void);

#endif /* __BCM63XX_FLASH_H */
