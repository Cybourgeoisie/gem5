/*
 * Copyright (c) 2003 The Regents of The University of Michigan
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
 */

#include <unistd.h>

#include <string>
#include <iostream>

#include "sim/syscall_emul.hh"
#include "base/trace.hh"
#include "cpu/exec_context.hh"
#include "cpu/base_cpu.hh"
#include "sim/process.hh"

#include "sim/sim_events.hh"

using namespace std;

void
SyscallDesc::doSyscall(int callnum, Process *process, ExecContext *xc)
{
    DPRINTFR(SyscallVerbose, "%s: syscall %s called\n",
             xc->cpu->name(), name);

    int retval = (*funcPtr)(this, callnum, process, xc);

    DPRINTFR(SyscallVerbose, "%s: syscall %s returns %d\n",
             xc->cpu->name(), name, retval);

    if (!((flags & SyscallDesc::SuppressReturnValue) && retval == 0))
        xc->setSyscallReturn(retval);
}


//
// Handler for unimplemented syscalls that we haven't thought about.
//
int
unimplementedFunc(SyscallDesc *desc, int callnum, Process *process,
                  ExecContext *xc)
{
    cerr << "Error: syscall " << desc->name
         << " (#" << callnum << ") unimplemented.";
    cerr << "  Args: " << xc->getSyscallArg(0) << ", " << xc->getSyscallArg(1)
         << ", ..." << endl;

    abort();
}


//
// Handler for unimplemented syscalls that we never intend to
// implement (signal handling, etc.) and should not affect the correct
// behavior of the program.  Print a warning only if the appropriate
// trace flag is enabled.  Return success to the target program.
//
int
ignoreFunc(SyscallDesc *desc, int callnum, Process *process,
           ExecContext *xc)
{
    DCOUT(SyscallWarnings) << "Warning: ignoring syscall " << desc->name
                           << "(" << xc->getSyscallArg(0)
                           << ", " << xc->getSyscallArg(1)
                           << ", ...)" << endl;

    return 0;
}


int
exitFunc(SyscallDesc *desc, int callnum, Process *process,
         ExecContext *xc)
{
    new SimExitEvent("syscall caused exit", xc->getSyscallArg(0) & 0xff);

    return 1;
}


int
getpagesizeFunc(SyscallDesc *desc, int num, Process *p, ExecContext *xc)
{
    return VMPageSize;
}


int
obreakFunc(SyscallDesc *desc, int num, Process *p, ExecContext *xc)
{
    // change brk addr to first arg
    p->brk_point = xc->getSyscallArg(0);
    return p->brk_point;
}


int
closeFunc(SyscallDesc *desc, int num, Process *p, ExecContext *xc)
{
    int fd = p->sim_fd(xc->getSyscallArg(0));
    return close(fd);
}


int
readFunc(SyscallDesc *desc, int num, Process *p, ExecContext *xc)
{
    int fd = p->sim_fd(xc->getSyscallArg(0));
    int nbytes = xc->getSyscallArg(2);
    BufferArg bufArg(xc->getSyscallArg(1), nbytes);

    int bytes_read = read(fd, bufArg.bufferPtr(), nbytes);

    if (bytes_read != -1)
        bufArg.copyOut(xc->mem);

    return bytes_read;
}

int
writeFunc(SyscallDesc *desc, int num, Process *p, ExecContext *xc)
{
    int fd = p->sim_fd(xc->getSyscallArg(0));
    int nbytes = xc->getSyscallArg(2);
    BufferArg bufArg(xc->getSyscallArg(1), nbytes);

    bufArg.copyIn(xc->mem);

    int bytes_written = write(fd, bufArg.bufferPtr(), nbytes);

    fsync(fd);

    return bytes_written;
}


int
lseekFunc(SyscallDesc *desc, int num, Process *p, ExecContext *xc)
{
    int fd = p->sim_fd(xc->getSyscallArg(0));
    uint64_t offs = xc->getSyscallArg(1);
    int whence = xc->getSyscallArg(2);

    off_t result = lseek(fd, offs, whence);

    return (result == (off_t)-1) ? -errno : result;
}


int
munmapFunc(SyscallDesc *desc, int num, Process *p, ExecContext *xc)
{
    // given that we don't really implement mmap, munmap is really easy
    return 0;
}


const char *hostname = "m5.eecs.umich.edu";

int
gethostnameFunc(SyscallDesc *desc, int num, Process *p, ExecContext *xc)
{
    int name_len = xc->getSyscallArg(1);
    BufferArg name(xc->getSyscallArg(0), name_len);

    strncpy((char *)name.bufferPtr(), hostname, name_len);

    name.copyOut(xc->mem);

    return 0;
}


