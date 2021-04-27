// COPYRIGHT 2021 Flávio Lobo Vaz, José Costa, Mário Travassos, Tomás Fidalgo
#include "../src/delay.h"

#ifdef DELAY
    int delay = DELAY;  // in milisec
#else
    int delay = 0;
#endif
