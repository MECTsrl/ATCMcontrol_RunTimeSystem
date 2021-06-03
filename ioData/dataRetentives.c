/*
 * Copyright 2021 Mect s.r.l
 *
 * This file is part of FarosPLC.
 *
 * FarosPLC is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * FarosPLC is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * FarosPLC. If not, see http://www.gnu.org/licenses/.
*/

/*
 * Filename: dataRetentive.c
 */

#include "dataImpl.h"

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define __4CFILE__	"dataRetentive.c"

#if defined(RTS_CFG_MECT_RETAIN)
#endif

/* ---------------------------------------------------------------------------- */

#define RETENTIVE_FILE 			"/local/retentive"

static void *ptRetentive = NULL;
static off_t lenRetentive = 0;

/* ---------------------------------------------------------------------------- */

u_int32_t *initRetentives()
{
    int fd;
    struct stat sb;

    ptRetentive = NULL;
    lenRetentive = 0;

    fd = open (RETENTIVE_FILE, O_RDWR | O_SYNC);
    if (fd == -1) {
        perror ("open");
        goto exit_failure;
    }
    if (fstat (fd, &sb) == -1) {
        perror ("fstat");
        goto exit_failure;
    }
    if (!S_ISREG (sb.st_mode)) {
        fprintf (stderr, "%s is not a file\n", RETENTIVE_FILE);
        goto exit_failure;
    }
    lenRetentive = sb.st_size;
    if (lenRetentive != LAST_RETENTIVE * 4) {
        fprintf(stderr, "Wrong retentive file size: got %ld expecting %u.\n", lenRetentive, LAST_RETENTIVE * 4);
        goto exit_failure;
    }
    ptRetentive = mmap(0, lenRetentive, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptRetentive == MAP_FAILED) {
        perror ("mmap");
        goto exit_failure;
    }
    if (close(fd) == -1) {
        perror ("close");
        goto exit_failure;
    }
#ifdef MECT_RETAIN_DEBUG
    if (ioctl(configfd, RETENTIVE_GPIOCTRL) < 0)
    {
         printf("ioctl failed and returned errno %s \n",strerror(errno));
    }
    fprintf(stderr,"[%s], called ioctl\n", __func__);
#endif
    return (u_int32_t *)ptRetentive;

exit_failure:
    if (ptRetentive && ptRetentive != MAP_FAILED) {
        munmap(ptRetentive, lenRetentive);
        ptRetentive = NULL;
        lenRetentive = 0;
    }
    if (fd > 0) {
        close(fd);
    }
    return NULL;
}

void syncRetentives()
{
    if (ptRetentive) {
        msync(ptRetentive, lenRetentive, MS_SYNC);
    }
}

void dumpRetentives()
{
    int uRes;

//#ifdef RTS_NATIVE_RETAIN
//    /* Stop Retain Update task
//     */
//    uRes = msgTXCommand(MSG_RT_CLOSE, Q_LIST_RET, Q_RESP_VMM_RET, VMM_TO_IPC_MSG_LONG, TRUE);
//    if (uRes != OK)
//    {
//        printf("%s CANNOT dump retain variables!\n", __func__);
//    }
//#else

    if (ptRetentive) {
        uRes = msync(ptRetentive, lenRetentive, MS_SYNC);
        if (uRes != 0) {
            fprintf(stderr,"%s CANNOT sync retain variables!\n", __func__);
        }
        uRes = munmap(ptRetentive, lenRetentive);
        if (uRes != 0) {
            fprintf(stderr,"%s CANNOT unmap retain variables!\n", __func__);
        }
        sync();
    }
}
