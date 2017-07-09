/*
 * i-scream libstatgrab
 * http://www.i-scream.org
 * Copyright (C) 2000-2004 i-scream
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 *
 * $Id: statgrab.h,v 1.58 2006/03/11 13:11:21 tdb Exp $
 */

#ifndef STATGRAB_H
#define STATGRAB_H


//#include <pdh.h>
#include <sys/types.h>

/* FIXME typedefs for 32/64-bit types */
/* FIXME maybe tidy up field names? */
/* FIXME comments for less obvious fields */

namespace SAM
{

template <typename T>
static double B_TO_MB(T bytes) { return (double(bytes) * (1.0 / double(1024*1024))); }

typedef enum {
	SG_ERROR_NONE = 0,
	SG_ERROR_ASPRINTF,
	SG_ERROR_DEVICES,
	SG_ERROR_DEVSTAT_GETDEVS,
	SG_ERROR_DEVSTAT_SELECTDEVS,
	SG_ERROR_DISKINFO,
	SG_ERROR_ENOENT,
	SG_ERROR_GETIFADDRS,
	SG_ERROR_GETMNTINFO,
	SG_ERROR_GETPAGESIZE,
	SG_ERROR_HOST,
	SG_ERROR_KSTAT_DATA_LOOKUP,
	SG_ERROR_KSTAT_LOOKUP,
	SG_ERROR_KSTAT_OPEN,
	SG_ERROR_KSTAT_READ,
	SG_ERROR_KVM_GETSWAPINFO,
	SG_ERROR_KVM_OPENFILES,
	SG_ERROR_MALLOC,
	SG_ERROR_MEMSTATUS,
	SG_ERROR_OPEN,
	SG_ERROR_OPENDIR,
	SG_ERROR_PARSE,
	SG_ERROR_PDHADD,
	SG_ERROR_PDHCOLLECT,
	SG_ERROR_PDHOPEN,
	SG_ERROR_PDHREAD,
	SG_ERROR_PERMISSION,
	SG_ERROR_PSTAT,
	SG_ERROR_SETEGID,
	SG_ERROR_SETEUID,
	SG_ERROR_SETMNTENT,
	SG_ERROR_SOCKET,
	SG_ERROR_SWAPCTL,
	SG_ERROR_SYSCONF,
	SG_ERROR_SYSCTL,
	SG_ERROR_SYSCTLBYNAME,
	SG_ERROR_SYSCTLNAMETOMIB,
	SG_ERROR_UNAME,
	SG_ERROR_UNSUPPORTED,
	SG_ERROR_XSW_VER_MISMATCH
} sg_error;

void sg_set_error(sg_error code, const char *arg);
void sg_set_error_with_errno(sg_error code, const char *arg);
sg_error sg_get_error();
const char *sg_get_error_arg();
int sg_get_error_errno();
const char *sg_str_error(sg_error code);


typedef struct {
	long long total;
	long long free;
	long long used;
	long long cache;
} sg_mem_stats;

sg_mem_stats * sg_get_mem_stats(sg_mem_stats *pmem_stat);
//char *sg_f_read_line(FILE *f, const char *string);
} // namespace SAM

#endif
