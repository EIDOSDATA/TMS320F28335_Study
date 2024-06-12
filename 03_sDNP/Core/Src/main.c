/*
 *  ======== main.c ========
 */

#include "ap.h"

// These are defined by the linker (see F28335.cmd)
extern uint16 RamfuncsLoadStart;
extern uint16 RamfuncsLoadEnd;
extern uint16 RamfuncsRunStart;

/*
 *  ======== main ========
 */
Int main()
{
    hwInit();
    apInit();
    apMain();

    return (0);
}

void Startup(void)
{
    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
}

