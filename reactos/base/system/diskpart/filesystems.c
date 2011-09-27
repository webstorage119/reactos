/*
 * PROJECT:         ReactOS DiskPart
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            base/system/diskpart/filesystems.c
 * PURPOSE:         Manages all the partitions of the OS in
 *					an interactive way
 * PROGRAMMERS:     Lee Schroeder
 */
#include "diskpart.h"

BOOL filesystems_main(INT argc, WCHAR **argv)
{
    return TRUE;
}


VOID help_filesystems(INT argc, WCHAR **argv)
{
    PrintResourceString(IDS_HELP_CMD_FILESYSTEMS);
}