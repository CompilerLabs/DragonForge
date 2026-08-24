#ifndef DRAGON__standard__buffering
#define DRAGON__standard__buffering

/* Include */
// anvil
#include "../../Anvil.h"

// standard files
#include "../../../BuildTemps/DragonForgePrint.c"
#include "../../../BuildTemps/DragonForgeCast.c"
#include "../../../BuildTemps/DragonForgeBuffer.c"
#include "../../../BuildTemps/DragonForgeCurrent.c"
#include "../../../BuildTemps/DragonForgeList.c"
#include "../../../BuildTemps/DragonForgeContext.c"
#include "../../../BuildTemps/DragonForgeCheck.c"
#include "../../../BuildTemps/DragonForgeError.c"
#include "../../../BuildTemps/DragonForgeJson.c"
#include "../../../BuildTemps/DragonForgeTime.c"
#include "../../../BuildTemps/DragonForgeAnvil.c"
#include "../../../BuildTemps/DragonForgeCompile.c"
#include "../../../BuildTemps/DragonForgeJustRun.c"

/* Turn C Files Into Buffers */
// bufferify any file
BASIC__buffer STANDARD__bufferify__any_file(unsigned char* buffer, unsigned int length) {
    return BASIC__create__buffer(buffer, BASIC__calculate__address_from_buffer_index(buffer, length - 1));
}

#endif
