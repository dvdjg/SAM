

#if defined(_WIN32) && !defined(__CYGWIN__)
#  ifndef WINVER
#    define WINVER 0x0501
#  endif // WINVER
#  ifndef _WIN32_WINNT
#    define _WIN32_WINNT 0x0501
#  endif // _WIN32_WINNT
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#  endif // WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <time.h>
#  include <psapi.h>
#  include <pdh.h>
#   include <pdhmsg.h>
#else
#  include <unistd.h>
#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>
#  include <fcntl.h>
#  include <sys/time.h>
#  include <sys/times.h>
#  include <sys/vtimes.h>
#  include <sys/sysinfo.h>
#  include <sys/types.h>
#  include <sys/resource.h>
#endif

#include <sys/timeb.h>
#include <cmath>

#include "getCPUTime.h"

#define SECONDS_PER_DAY 86400


namespace SAM
{

    int
    clockTicks ()
    {
        return (
            #if defined(CLK_TCK)
                    CLK_TCK
            #else
                    sysconf(_SC_CLK_TCK)
            #endif
                    );
    }
    /*
* (c) The GRASP/AQUA Project, Glasgow University, 1994-1998
*
* $Id: getCPUTime.c,v 1.7 2000/04/06 17:54:01 rrt Exp $
*
* getCPUTime Runtime Support
*/

    /*
* Our caller wants a pointer to four StgInts,
* user seconds, user nanoseconds, system seconds, system nanoseconds.
* Yes, the timerval has unsigned components, but nanoseconds take only
* 30 bits, and our CPU usage would have to be over 68 years for the 
* seconds to overflow 31 bits.
*/

#ifndef _WIN32
    void getCPUTime(double & dUserTime, double & dKernelTime)
    {
        /* getrusage() is right royal pain to deal with when targetting multiple
    versions of Solaris, since some versions supply it in libc (2.3 and 2.5),
    while 2.4 has got it in libucb (I wouldn't be too surprised if it was back
    again in libucb in 2.6..)

    Avoid the problem by resorting to times() instead.
    defined(HAVE_GETRUSAGE) &&
    */
#if ! irix_TARGET_OS && ! solaris2_TARGET_OS
        struct rusage t;

        getrusage(RUSAGE_SELF, &t);

        dUserTime = double(t.ru_utime.tv_sec) + t.ru_utime.tv_usec / 1000000.0;
        dKernelTime = double(t.ru_stime.tv_sec) + t.ru_stime.tv_usec / 1000000.0;

#else
# if defined(HAVE_TIMES)
        struct tms t;
#  if defined(CLK_TCK)
#   define ticks CLK_TCK
#  else
        long ticks;
        ticks = sysconf(_SC_CLK_TCK);
#  endif

        times(&t);

        dUserTime = double(t.tms_utime) / ticks;
        dKernelTime = double(t.tms_stime) / ticks;

# else
        //return 0;
# endif
#endif
        //return 1;
    }

#else

#ifdef _WIN32
    /* 100ns units per sec, really */
#define NS_PER_SEC 10000000LL
#define FT2usecs(ll,ft)    \
    (ll)=(ft).dwHighDateTime; \
    (ll) <<= 32;              \
    (ll) |= (ft).dwLowDateTime;
#endif

    /* cygwin32 or mingw32 version */
    void getCPUTime(double & dUserTime, double & dKernelTime)
    {
        FILETIME creationTime, exitTime, kernelTime, userTime;
        unsigned long long uT, kT;

        /* ToDo: pin down elapsed times to just the OS thread(s) that
    are evaluating/managing Haskell code.
    */
        if (!GetProcessTimes (GetCurrentProcess(), &creationTime,
                              &exitTime, &kernelTime, &userTime))
        {
            /* Probably on a Win95 box..*/
            dUserTime = 0;
            dKernelTime = 0;
        }
        else
        {
            FT2usecs(uT, userTime);
            FT2usecs(kT, kernelTime);

            dUserTime = uT / 10000000.0;
            dKernelTime = kT / 10000000.0;
        }
    }

#endif /* _WIN32 */


    long long getTotalSystemMemory()
    {
#if defined(_WIN32) && !defined(__CYGWIN__)
        MEMORYSTATUSEX status;
        status.dwLength = sizeof(status);
        GlobalMemoryStatusEx(&status);
        return status.ullTotalPhys;
#else
        long long pages = sysconf(_SC_PHYS_PAGES);
        long long page_size = sysconf(_SC_PAGE_SIZE);
        return pages * page_size;
#endif
    }


#define EPOCHFILETIME_SEC (11644473600.)
#define TEN_MILLION 10000000.

#ifdef _WIN32
    static PDH_HQUERY cpuQuery;
    static PDH_HCOUNTER cpuTotal;

    void initCurrentlyUsedCPU()
    {
        // Porcentaje de tiempo que el procesador está ejecutando subprocesos no inactivos.
        PDH_STATUS pdhStatus = ERROR_SUCCESS;
        pdhStatus = PdhOpenQuery(NULL, NULL, &cpuQuery);
        pdhStatus = PdhAddCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
        if(pdhStatus != ERROR_SUCCESS) {
            /// @todo Hacer una versión multiidioma: http://en.verysource.com/code/3604946_1/platforminfo.cpp.html
            // La cadena del contador depende del idioma!!!
            pdhStatus = PdhAddCounter(cpuQuery, L"\\Procesador(_Total)\\% de tiempo de procesador", NULL, &cpuTotal);
        }
        pdhStatus = PdhCollectQueryData(cpuQuery);
    }


    double getCurrentlyUsedCPUPercentage()
    {
        PDH_STATUS pdhStatus = ERROR_SUCCESS;
        double ret = -1.0;
        PDH_FMT_COUNTERVALUE counterVal;

        // Éste es un valor agregado basado en el número total de CPU detectadas,
        // físicas y lógicas, y dividido entre la suma del recuento de CPU.
        pdhStatus = PdhCollectQueryData(cpuQuery);
        if(pdhStatus == ERROR_SUCCESS)
        {
            pdhStatus = PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
            if(pdhStatus == ERROR_SUCCESS && (counterVal.CStatus == PDH_CSTATUS_VALID_DATA
                    || counterVal.CStatus == PDH_CSTATUS_NEW_DATA)) {
                ret = counterVal.doubleValue;
            }
        }

        return ret;
    }

    static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
    static int numProcessors;
    static HANDLE self;


    void initCurrentlyUsedCPUByCurrentProcess()
    {
        SYSTEM_INFO sysInfo;
        FILETIME ftime, fsys, fuser;

        GetSystemInfo(&sysInfo);
        numProcessors = sysInfo.dwNumberOfProcessors;

        GetSystemTimeAsFileTime(&ftime);
        memcpy(&lastCPU, &ftime, sizeof(FILETIME));

        self = GetCurrentProcess();
        GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
        memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
        memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
    }


    double getCurrentlyUsedCPUByCurrentProcessPercentage()
    {
        FILETIME ftime, fsys, fuser;
        ULARGE_INTEGER now, sys, user;
        double percent;

        GetSystemTimeAsFileTime(&ftime);
        memcpy(&now, &ftime, sizeof(FILETIME));

        GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
        memcpy(&sys, &fsys, sizeof(FILETIME));
        memcpy(&user, &fuser, sizeof(FILETIME));
        percent = double((sys.QuadPart - lastSysCPU.QuadPart) +
                         (user.QuadPart - lastUserCPU.QuadPart));
        percent /= (now.QuadPart - lastCPU.QuadPart);
        percent /= numProcessors;
        lastCPU = now;
        lastUserCPU = user;
        lastSysCPU = sys;

        return percent * 100;
    }
#else
    static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;


    void initCurrentlyUsedCPU(){
        FILE* file = fopen("/proc/stat", "r");
        fscanf(file, "cpu %Ld %Ld %Ld %Ld", &lastTotalUser, &lastTotalUserLow,
               &lastTotalSys, &lastTotalIdle);
        fclose(file);
    }


    double getCurrentlyUsedCPUPercentage(){
        double percent;
        FILE* file;
        unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;


        file = fopen("/proc/stat", "r");
        fscanf(file, "cpu %Ld %Ld %Ld %Ld", &totalUser, &totalUserLow,
               &totalSys, &totalIdle);
        fclose(file);


        if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
                totalSys < lastTotalSys || totalIdle < lastTotalIdle){
            //Overflow detection. Just skip this value.
            percent = -1.0;
        }
        else{
            total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
                    (totalSys - lastTotalSys);
            percent = total;
            total += (totalIdle - lastTotalIdle);
            percent /= total;
            percent *= 100;
        }


        lastTotalUser = totalUser;
        lastTotalUserLow = totalUserLow;
        lastTotalSys = totalSys;
        lastTotalIdle = totalIdle;

        return percent;
    }


    static clock_t lastCPU, lastSysCPU, lastUserCPU;
    static int numProcessors;


    void initCurrentlyUsedCPUByCurrentProcess(){
        FILE* file;
        struct tms timeSample;
        char line[128];


        lastCPU = times(&timeSample);
        lastSysCPU = timeSample.tms_stime;
        lastUserCPU = timeSample.tms_utime;


        file = fopen("/proc/cpuinfo", "r");
        numProcessors = 0;
        while(fgets(line, 128, file) != NULL){
            if (strncmp(line, "processor", 9) == 0) numProcessors++;
        }
        fclose(file);
    }


    double getCurrentlyUsedCPUByCurrentProcessPercentage(){
        struct tms timeSample;
        clock_t now;
        double percent;


        now = times(&timeSample);
        if (now <= lastCPU || timeSample.tms_stime < lastSysCPU ||
                timeSample.tms_utime < lastUserCPU){
            //Overflow detection. Just skip this value.
            percent = -1.0;
        }
        else{
            percent = (timeSample.tms_stime - lastSysCPU) +
                    (timeSample.tms_utime - lastUserCPU);
            percent /= (now - lastCPU);
            percent /= numProcessors;
            percent *= 100;
        }
        lastCPU = now;
        lastSysCPU = timeSample.tms_stime;
        lastUserCPU = timeSample.tms_utime;


        return percent;
    }
#endif

#ifdef _WIN32
    void ProcessMemory(long long &virtualMemUsedByMe,
                       long long &physMemUsedByMe)
    {
        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
        virtualMemUsedByMe = (long long) pmc.PrivateUsage;
        physMemUsedByMe = (long long) pmc.WorkingSetSize;
    }
#else
    static long long parseLine(char* line)
    {
        long long i = (long long) strlen(line);
        while (*line < '0' || *line > '9') line++;
        line[i-3] = '\0';
        i = atoll(line);
        return i;
    }

    static long long getValue(const char *szWhat,  //< "/proc/self/status"
                              const char *szWhere) //< "VmSize:"
    {
        //Note: this value is in KB!
        FILE* file = fopen(szWhat, "r");

        long long result = -1;
        if(file)
        {
            char line[128];

            while (fgets(line, 128, file) != NULL)
            {
                if (strncmp(line, szWhere, strlen(szWhere)) == 0)
                {
                    result = parseLine(line);
                    break;
                }
            }

            fclose(file);
        }

        return result;
    }

    void ProcessMemory(long long &virtualMemUsedByMe,
                       long long &physMemUsedByMe)
    {
        virtualMemUsedByMe = getValue("/proc/self/status", "VmSize:") * 1000;
        physMemUsedByMe = getValue("/proc/self/status", "VmRSS:") * 1000;
    }

#endif


    void GlobalMemory(long long &totalVirtualMem,
                      long long &virtualMemUsed,
                      long long &totalPhysMem,
                      long long &physMemUsed)
    {
#ifdef _WIN32
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        totalVirtualMem = (long long) (memInfo.ullTotalPageFile);
        virtualMemUsed = (long long) (memInfo.ullTotalPageFile - memInfo.ullAvailPageFile);
        totalPhysMem = (long long) memInfo.ullTotalPhys;
        physMemUsed = (long long) (memInfo.ullTotalPhys - memInfo.ullAvailPhys);
#else
        struct ::sysinfo memInfo;

        ::sysinfo(&memInfo);
        totalVirtualMem = memInfo.totalram;
        //Add other values in next statement to avoid int overflow on right hand side...
        totalVirtualMem += memInfo.totalswap;
        totalVirtualMem *= memInfo.mem_unit;

        virtualMemUsed = memInfo.totalram - memInfo.freeram;

        //Add other values in next statement to avoid int overflow on right hand side...
        virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
        virtualMemUsed *= memInfo.mem_unit;

        totalPhysMem = memInfo.totalram;
        //Multiply in next statement to avoid int overflow on right hand side...
        totalPhysMem *= memInfo.mem_unit;
        physMemUsed = memInfo.totalram - memInfo.freeram;
        //Multiply in next statement to avoid int overflow on right hand side...
        physMemUsed *= memInfo.mem_unit;
#endif
    }


    // return time of day (seconds since 1970) as a double
    //
    double dtime()
    {
#ifdef _WIN32
        LARGE_INTEGER time;
        FILETIME sysTime;
        double t;
        GetSystemTimeAsFileTime(&sysTime);
        time.LowPart = sysTime.dwLowDateTime;
        time.HighPart = sysTime.dwHighDateTime;  // Time is in 100 ns units
        t = (double)time.QuadPart;    // Convert to 1 s units
        t /= TEN_MILLION;                /* In seconds */
        t -= EPOCHFILETIME_SEC;     /* Offset to the Epoch time */
        return t;
#else
        struct timeval tv;
        gettimeofday(&tv, 0);
        return tv.tv_sec + (tv.tv_usec/1.e6);
#endif
    }

#ifdef _WIN32

    int thread_cpu_time(HANDLE thread_handle, double& cpu)
    {
        FILETIME creationTime, exitTime, kernelTime, userTime;

        if (GetThreadTimes(
                    thread_handle, &creationTime, &exitTime, &kernelTime, &userTime)
                ) {
            ULARGE_INTEGER tKernel, tUser;
            LONGLONG totTime;

            tKernel.LowPart  = kernelTime.dwLowDateTime;
            tKernel.HighPart = kernelTime.dwHighDateTime;
            tUser.LowPart    = userTime.dwLowDateTime;
            tUser.HighPart   = userTime.dwHighDateTime;
            totTime = tKernel.QuadPart + tUser.QuadPart;

            // Runtimes in 100-nanosecond units
            cpu = totTime * 1.e-7;
        } else {
            return -1;
        }
        return 0;
    }

    int process_cpu_time(HANDLE process_handle, double& cpu)
    {
        FILETIME creationTime, exitTime, kernelTime, userTime;

        if (GetProcessTimes( process_handle, &creationTime, &exitTime, &kernelTime, &userTime))
        {
            ULARGE_INTEGER tKernel, tUser;
            LONGLONG totTime;

            tKernel.LowPart  = kernelTime.dwLowDateTime;
            tKernel.HighPart = kernelTime.dwHighDateTime;
            tUser.LowPart    = userTime.dwLowDateTime;
            tUser.HighPart   = userTime.dwHighDateTime;
            totTime = tKernel.QuadPart + tUser.QuadPart;

            // Runtimes in 100-nanosecond units
            cpu = totTime / 1.e7;
        } else {
            return -1;
        }
        return 0;
    }

    static void get_elapsed_time(double& cpu)
    {
        static double start_time;

        double now = dtime();
        if (start_time)
            cpu = now - start_time;
        else
            cpu = 0;

        start_time = now;
    }

    int calling_thread_cpu_time(double& cpu)
    {
        if (thread_cpu_time(GetCurrentThread(), cpu)) // If this doesn't work...
            get_elapsed_time(cpu); // ... use this workaround.

        return 0;
    }

#else

    // Unix: pthreads doesn't provide an API for getting per-process CPU time,
    // so just get the thread's CPU time
    int calling_thread_cpu_time(double &cpu_t)
    {
        struct ::rusage ru;

        int retval = ::getrusage(RUSAGE_SELF, &ru);
        if (retval)
            return retval; // ERR_GETRUSAGE

        cpu_t = (double)ru.ru_utime.tv_sec + ((double)ru.ru_utime.tv_usec) / 1e6;
        cpu_t += (double)ru.ru_stime.tv_sec + ((double)ru.ru_stime.tv_usec) / 1e6;

        return 0;
    }

#endif

    // return time today 0:00 in seconds since 1970 as a double
    //
    double dday() {
        double now=dtime();
        return (now-fmod(now, SECONDS_PER_DAY));
    }

//#ifdef _WIN32
//    double GetProcessorSpeed()
//    {
//        LARGE_INTEGER qwWait, qwStart, qwCurrent;
//        QueryPerformanceCounter(&qwStart);
//        QueryPerformanceFrequency(&qwWait);
//        qwWait.QuadPart >>= 5;
//        unsigned long long Start = __rdtsc();
//        do
//        {
//            QueryPerformanceCounter(&qwCurrent);
//        }while(qwCurrent.QuadPart - qwStart.QuadPart < qwWait.QuadPart);

//        double dCPUSpeedMHz = ((__rdtsc() - Start) << 5) / 1000000.0;

//        return dCPUSpeedMHz;
//    }
//#endif

    int getNumProcessors()
    {
        return numProcessors;
    }


} // namespace djg

