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
 * $Id: memory_stats.c,v 1.36 2010/02/21 10:04:26 tdb Exp $
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef SOLARIS
#include <unistd.h>
#include <kstat.h>
#endif
#if defined(LINUX) || defined(CYGWIN) || defined(__linux__) || defined(__linux)
#include <stdio.h>
#include <string.h>
#endif
#if defined(FREEBSD) || defined(DFBSD)
#include <sys/types.h>
#include <sys/sysctl.h>
#include <unistd.h>
#endif
#if defined(NETBSD)
#include <sys/param.h>
#include <sys/time.h>
#include <uvm/uvm.h>
#endif
#if defined(OPENBSD)
#include <sys/param.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/unistd.h>
#endif
#ifdef HPUX
#include <sys/param.h>
#include <sys/pstat.h>
#include <unistd.h>
#endif
#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include "memoryStats.h"

namespace SAM
{

char *sg_f_read_line(FILE *f, const char *string)
{
    /* Max line length. 8k should be more than enough */
    static char line[8192];

    while((fgets(line, sizeof(line), f))!=NULL){
        if(strncmp(string, line, strlen(string))==0){
            return line;
        }
    }

    //  sg_set_error(SG_ERROR_PARSE, NULL);
    return NULL;
}

sg_mem_stats * sg_get_mem_stats(sg_mem_stats *pmem_stat)
{
#ifdef HPUX
	struct pst_static *pstat_static;
	struct pst_dynamic pstat_dynamic;
	long long pagesize;
#endif
#ifdef SOLARIS
	kstat_ctl_t *kc;
	kstat_t *ksp;
	kstat_named_t *kn;
	long totalmem;
	int pagesize;
#endif
#if defined(LINUX) || defined(CYGWIN) || defined(__linux__) || defined(__linux)
	char *line_ptr;
	unsigned long long value;
	FILE *f;
#endif
#if defined(FREEBSD) || defined(DFBSD)
	int mib[2];
	u_long physmem;
	size_t size;
	u_long free_count;
	u_long cache_count;
	u_long inactive_count;
	int pagesize;
#endif
#if defined(NETBSD)
	struct uvmexp *uvm;
#endif
#if defined(OPENBSD)
	int mib[2];
	struct vmtotal vmtotal;
	size_t size;
	int pagesize, page_multiplier;
#endif
#ifdef WIN32
	MEMORYSTATUSEX memstats;
#endif

#ifdef HPUX
	if((pagesize=sysconf(_SC_PAGESIZE)) == -1){
		//sg_set_error_with_errno(SG_ERROR_SYSCONF, "_SC_PAGESIZE");
		return NULL;
	}

	if (pstat_getdynamic(&pstat_dynamic, sizeof(pstat_dynamic), 1, 0) == -1) {
		//sg_set_error_with_errno(SG_ERROR_PSTAT, "pstat_dynamic");
		return NULL;
	}
	pstat_static = sg_get_pstat_static();
	if (pstat_static == NULL) {
		return NULL;
	}

	/* FIXME Does this include swap? */
	pmem_stat->total = ((long long) pstat_static->physical_memory) * pagesize;
	pmem_stat->free = ((long long) pstat_dynamic.psd_free) * pagesize;
	pmem_stat->used = pmem_stat->total - pmem_stat->free;
#endif
#ifdef SOLARIS
	if((pagesize=sysconf(_SC_PAGESIZE)) == -1){
		//sg_set_error_with_errno(SG_ERROR_SYSCONF, "_SC_PAGESIZE");
		return NULL;	
	}

	if((totalmem=sysconf(_SC_PHYS_PAGES)) == -1){
		//sg_set_error_with_errno(SG_ERROR_SYSCONF, "_SC_PHYS_PAGES");
		return NULL;
	}

	if ((kc = kstat_open()) == NULL) {
		sg_set_error(SG_ERROR_KSTAT_OPEN, NULL);
		return NULL;
	}
	if((ksp=kstat_lookup(kc, "unix", 0, "system_pages")) == NULL){
		sg_set_error(SG_ERROR_KSTAT_LOOKUP, "unix,0,system_pages");
		return NULL;
	}
	if (kstat_read(kc, ksp, 0) == -1) {
		sg_set_error(SG_ERROR_KSTAT_READ, NULL);
		return NULL;
	}
	if((kn=kstat_data_lookup(ksp, "freemem")) == NULL){
		sg_set_error(SG_ERROR_KSTAT_DATA_LOOKUP, "freemem");
		return NULL;
	}
	kstat_close(kc);

	pmem_stat->total = (long long)totalmem * (long long)pagesize;
	pmem_stat->free = ((long long)kn->value.ul) * (long long)pagesize;
	pmem_stat->used = pmem_stat->total - pmem_stat->free;
#endif

#if defined(LINUX) || defined(CYGWIN) || defined(__linux__) || defined(__linux)
	if ((f = fopen("/proc/meminfo", "r")) == NULL) {
		//sg_set_error_with_errno(SG_ERROR_OPEN, "/proc/meminfo");
		return NULL;
	}

	while ((line_ptr = sg_f_read_line(f, "")) != NULL) {
		if (sscanf(line_ptr, "%*s %llu kB", &value) != 1) {
			continue;
		}
		value *= 1024;

		if (strncmp(line_ptr, "MemTotal:", 9) == 0) {
			pmem_stat->total = value;
		} else if (strncmp(line_ptr, "MemFree:", 8) == 0) {
			pmem_stat->free = value;
		} else if (strncmp(line_ptr, "Cached:", 7) == 0) {
			pmem_stat->cache = value;
		}
	}

	fclose(f);
	pmem_stat->used = pmem_stat->total - pmem_stat->free;
#endif

#if defined(FREEBSD) || defined(DFBSD)
	/* Returns bytes */
	mib[0] = CTL_HW;
	mib[1] = HW_PHYSMEM;
	size = sizeof physmem;
	if (sysctl(mib, 2, &physmem, &size, NULL, 0) < 0) {
		//sg_set_error_with_errno(SG_ERROR_SYSCTL, "CTL_HW.HW_PHYSMEM");
		return NULL;
	}
	pmem_stat->total = physmem;

	/*returns pages*/
	size = sizeof free_count;
	if (sysctlbyname("vm.stats.vm.v_free_count", &free_count, &size, NULL, 0) < 0){
		sg_set_error_with_errno(SG_ERROR_SYSCTLBYNAME,
		                        "vm.stats.vm.v_free_count");
		return NULL;
	}

	size = sizeof inactive_count;
	if (sysctlbyname("vm.stats.vm.v_inactive_count", &inactive_count , &size, NULL, 0) < 0){
		sg_set_error_with_errno(SG_ERROR_SYSCTLBYNAME,
		                        "vm.stats.vm.v_inactive_count");
		return NULL;
	}

	size = sizeof cache_count;
	if (sysctlbyname("vm.stats.vm.v_cache_count", &cache_count, &size, NULL, 0) < 0){
		sg_set_error_with_errno(SG_ERROR_SYSCTLBYNAME,
		                        "vm.stats.vm.v_cache_count");
		return NULL;
	}

	/* Because all the vm.stats returns pages, I need to get the page size.
	 * After that I then need to multiple the anything that used vm.stats to
	 * get the system statistics by pagesize 
	 */
	pagesize = getpagesize();
	pmem_stat->cache=cache_count*pagesize;

	/* Of couse nothing is ever that simple :) And I have inactive pages to
	 * deal with too. So I'm going to add them to free memory :)
	 */
	pmem_stat->free=(free_count*pagesize)+(inactive_count*pagesize);
	pmem_stat->used=physmem-pmem_stat->free;
#endif

#if defined(NETBSD)
	if ((uvm = sg_get_uvmexp()) == NULL) {
		return NULL;
	}

	pmem_stat->total = uvm->pagesize * uvm->npages;
	pmem_stat->cache = uvm->pagesize * (uvm->filepages + uvm->execpages);
	pmem_stat->free = uvm->pagesize * (uvm->free + uvm->inactive);
	pmem_stat->used = pmem_stat->total - pmem_stat->free;
#endif

#if defined(OPENBSD)
	/* The code in this section is based on the code in the OpenBSD
	 * top utility, located at src/usr.bin/top/machine.c in the
	 * OpenBSD source tree.
	 *
	 * For fun, and like OpenBSD top, we will do the multiplication
	 * converting the memory stats in pages to bytes in base 2.
	 */

	/* All memory stats in OpenBSD are returned as the number of pages.
	 * To convert this into the number of bytes we need to know the
	 * page size on this system.
	 */
	pagesize = sysconf(_SC_PAGESIZE);

	/* The pagesize gives us the base 10 multiplier, so we need to work
	 * out what the base 2 multiplier is. This means dividing
	 * pagesize by 2 until we reach unity, and counting the number of
	 * divisions required.
	 */
	page_multiplier = 0;

	while (pagesize > 1) {
		page_multiplier++;
		pagesize >>= 1;
	}

	/* We can now ret the the raw VM stats (in pages) using the
	 * sysctl interface.
	 */
	mib[0] = CTL_VM;
	mib[1] = VM_METER;
	size = sizeof(vmtotal);

	if (sysctl(mib, 2, &vmtotal, &size, NULL, 0) < 0) {
		bzero(&vmtotal, sizeof(vmtotal));
		sg_set_error_with_errno(SG_ERROR_SYSCTL, "CTL_VM.VM_METER");
		return NULL;
	}

	/* Convert the raw stats to bytes, and return these to the caller
	 */
	pmem_stat->used = (vmtotal.t_rm << page_multiplier);   /* total real mem in use */
	pmem_stat->cache = 0;                                  /* no cache stats */
	pmem_stat->free = (vmtotal.t_free << page_multiplier); /* free memory pages */
	pmem_stat->total = (pmem_stat->used + pmem_stat->free);
#endif

#ifdef WIN32
	memstats.dwLength = sizeof(memstats);
	if (!GlobalMemoryStatusEx(&memstats)) {
		//sg_set_error_with_errno(SG_ERROR_MEMSTATUS, NULL);
		return NULL;
	}
	pmem_stat->free = memstats.ullAvailPhys;
	pmem_stat->total = memstats.ullTotalPhys;
	pmem_stat->used = pmem_stat->total - pmem_stat->free;
	//if(read_counter_large(SG_WIN32_MEM_CACHE, &pmem_stat->cache))
    {
		pmem_stat->cache = 0;
	}
#endif
	return pmem_stat;
}


} // namespace djg

