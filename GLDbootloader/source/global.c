#include "bootloader/global.h"

bootloader_global g_bootloader;

/*****************************************************************************/
void global_bootloader_init()
{
    g_bootloader.ptr.nPtrJump = 0;
    g_bootloader.ptr.nPtrBuf = 0;
    g_bootloader.ptr.nPtrExe = 0;
    g_bootloader.ptr.nPtrData = 0;
    g_bootloader.ptr.nPtrIo = 0;
    g_bootloader.ptr.nPtrFlash = 0;
    g_bootloader.ptr.nPtrDev1 = 0;
    g_bootloader.ptr.nPtrDev2 = 0;
}

/*****************************************************************************/
