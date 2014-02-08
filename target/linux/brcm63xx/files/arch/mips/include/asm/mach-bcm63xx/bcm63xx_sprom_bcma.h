#ifndef BCM63XX_SPROM_BCMA_H_
#define BCM63XX_SPROM_BCMA_H_

#include <linux/bcma/bcma.h>

int bcm63xx_get_bcma_fallback_sprom(struct bcma_bus *bus,
	struct ssb_sprom *out);

#endif /* ! BCM63XX_SPROM_BCMA_H_ */
