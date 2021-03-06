/*
 * Copyright (c) 2003-2005 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Gabe Black
 */

#ifndef __ARCH_SPARC_LINUX_LINUX_HH__
#define __ARCH_SPARC_LINUX_LINUX_HH__

#include "kern/linux/linux.hh"

class SparcLinux : public Linux
{
  public:

    typedef struct {
        uint32_t st_dev;
        char __pad1[4];
        uint64_t st_ino;
        uint32_t st_mode;
        uint16_t st_nlink;
        uint32_t st_uid;
        uint32_t st_gid;
        uint32_t st_rdev;
        char __pad2[4];
        int64_t st_size;
        int64_t st_atimeX;
        int64_t st_mtimeX;
        int64_t st_ctimeX;
        int64_t st_blksize;
        int64_t st_blocks;
        uint64_t __unused4[2];
    } tgt_stat;

    static OpenFlagTransTable openFlagTable[];

    static const int TGT_O_RDONLY       = 0x00000000;   //!< O_RDONLY
    static const int TGT_O_WRONLY       = 0x00000001;   //!< O_WRONLY
    static const int TGT_O_RDWR         = 0x00000002;   //!< O_RDWR
    static const int TGT_O_NONBLOCK     = 0x00004000;   //!< O_NONBLOCK
    static const int TGT_O_APPEND       = 0x00000008;   //!< O_APPEND
    static const int TGT_O_CREAT        = 0x00000200;   //!< O_CREAT
    static const int TGT_O_TRUNC        = 0x00000400;   //!< O_TRUNC
    static const int TGT_O_EXCL         = 0x00000800;   //!< O_EXCL
    static const int TGT_O_NOCTTY       = 0x00008000;   //!< O_NOCTTY
    static const int TGT_O_SYNC         = 0x00002000;   //!< O_SYNC
//    static const int TGT_O_DRD        = 0x00010000;   //!< O_DRD
//    static const int TGT_O_DIRECTIO   = 0x00020000;   //!< O_DIRECTIO
//    static const int TGT_O_CACHE      = 0x00002000;   //!< O_CACHE
//    static const int TGT_O_DSYNC      = 0x00008000;   //!< O_DSYNC
//    static const int TGT_O_RSYNC      = 0x00040000;   //!< O_RSYNC

    static const int NUM_OPEN_FLAGS;

    static const unsigned TGT_MAP_ANONYMOUS = 0x20;
    static const unsigned TGT_MAP_FIXED     = 0x10;

    typedef struct {
        int64_t  uptime;    /* Seconds since boot */
        uint64_t loads[3];  /* 1, 5, and 15 minute load averages */
        uint64_t totalram;  /* Total usable main memory size */
        uint64_t freeram;   /* Available memory size */
        uint64_t sharedram; /* Amount of shared memory */
        uint64_t bufferram; /* Memory used by buffers */
        uint64_t totalswap; /* Total swap space size */
        uint64_t freeswap;  /* swap space still available */
        uint16_t procs;     /* Number of current processes */
        uint64_t totalhigh; /* Total high memory size */
        uint64_t freehigh;  /* Available high memory size */
        uint64_t mem_unit;  /* Memory unit size in bytes */
    } tgt_sysinfo;

    //@{
    /// ioctl() command codes.
    /// These were calculated using the SPARC Linux headers on an x86
    /// machine and thus may not be correct.  It would be good to
    /// verify/update these values on an actual SPARC Linux machine.
    static const unsigned TGT_TCGETA    = 0x40125401;
    static const unsigned TGT_TCSETAW   = 0x80125403;
    static const unsigned TGT_TCGETS    = 0x40385408;
    static const unsigned TGT_FIONREAD  = 0x4004667f;
    static const unsigned TGT_TIOCGETP  = 0x40067408;
    static const unsigned TGT_TIOCSETP  = 0x80067409;
    static const unsigned TGT_TIOCSETN  = 0x8006740a;
    //@}

    static bool
    isTtyReq(unsigned req)
    {
        switch (req) {
          case TGT_TIOCGETP:
          case TGT_TIOCSETP:
          case TGT_TIOCSETN:
          case TGT_TCGETS:
          case TGT_TCGETA:
          case TGT_TCSETAW:
            return true;
          default:
            return false;
        }
    }
};

class Sparc32Linux : public SparcLinux
{
  public:

    typedef struct {
        uint64_t st_dev;
        uint64_t st_ino;
        uint32_t st_mode;
        uint32_t st_nlink;
        uint32_t st_uid;
        uint32_t st_gid;
        uint64_t st_rdev;
        uint8_t __pad3[8];
        int64_t st_size;
        int32_t st_blksize;
        uint8_t __pad4[8];
        int64_t st_blocks;
        uint64_t st_atimeX;
        uint64_t st_atime_nsec;
        uint64_t st_mtimeX;
        uint64_t st_mtime_nsec;
        uint64_t st_ctimeX;
        uint64_t st_ctime_nsec;
        uint32_t __unused4;
        uint32_t __unused5;
    } tgt_stat64;

    typedef struct {
        int32_t  uptime;    /* Seconds since boot */
        uint32_t loads[3];  /* 1, 5, and 15 minute load averages */
        uint32_t totalram;  /* Total usable main memory size */
        uint32_t freeram;   /* Available memory size */
        uint32_t sharedram; /* Amount of shared memory */
        uint32_t bufferram; /* Memory used by buffers */
        uint32_t totalswap; /* Total swap space size */
        uint32_t freeswap;  /* swap space still available */
        uint16_t procs;     /* Number of current processes */
        uint32_t totalhigh; /* Total high memory size */
        uint32_t freehigh;  /* Available high memory size */
        uint32_t mem_unit;  /* Memory unit size in bytes */
    } tgt_sysinfo;

    /// Resource constants for getrlimit() (overide some generics).
    static const unsigned TGT_RLIMIT_NPROC = 7;
    static const unsigned TGT_RLIMIT_NOFILE = 6;
};

#endif
