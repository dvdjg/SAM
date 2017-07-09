/** @file getCPUTime.h
*   @brief Performance functions.
*   @date   12-04-2007 (dd-mm-yyyy)
*   @author David Jurado González
*   @version 1.0
*/

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#ifndef GETCPUTIME_H__
#define GETCPUTIME_H__


namespace SAM
{
    /**
     * @desc Current process execution times running user or keernel code.
     * @param double & dUserTime User time to be returned in seconds.
     * @param double & dKernelTime Kernel time to be returned in seconds.
     * @return int.
     */
    void getCPUTime(double & dUserTime, double & dKernelTime);

    ///**
    // * @desc Initializes CPU measure counters.
    // */
    //void initCurrentlyUsedCPU();

    ///**
    // * @desc Initializes CPU measure counters for the calling process.
    // */
    //void initCurrentlyUsedCPUByCurrentProcess();

    /**
     * @desc Returns the total physical memory in bytes.
     * @return long long Use __int64 or the LONGLONG macro in VC++ 6.0.
     */
    long long getTotalSystemMemory();

    /**
     * @desc Returns the time of day (seconds since 1970) as a double.
     * @return double.
     */
    double dtime();

    /**
     * @desc Returns the time today 0:00 in seconds since 1970 as a double.
     * @return double.
     */
    double dday();

    /**
     * @desc Returns current thread cpu time in seconds.
     * @return double.
     */
    int calling_thread_cpu_time(double&);

    /**
     * @desc Returns global memory amounts in bytes.
     * @param long long &totalVirtualMem Total system virtual used.
     * @param long long &virtualMemUsed Current process virtual memory used.
     * @param long long &totalPhysMem Total system physical memory used.
     * @param long long &physMemUsed Current process physical memory used.
     * @return void.
     */
    void GlobalMemory(long long &totalVirtualMem, 
                        long long &virtualMemUsed, 
                        long long &totalPhysMem, 
                        long long &physMemUsed);

    /**
     * @desc Returns process memory in bytes.
     * @param long long &virtualMemUsedByMe Current process virtual memory used.
     * @param long long &physMemUsedByMe Current process physical memory used.
     * @return void.
     */
    void ProcessMemory(long long &virtualMemUsedByMe, 
                 long long &physMemUsedByMe);

    
    
    /**
     * @desc Initializes the needed resources for measuring system CPU use.
     * @note This is a dumb for linux.
     * @return void.
     */
    void initCurrentlyUsedCPU();

    /**
     * @desc Retrieves percentage of measured system CPU used.
     *  Percentage of time the processor is busy by measuring the percentage of time
     *  the thread of the Idle process is running and then subtracting that from 100 percent.
     *  This measurement is the amount of processor utilization.
     *  Although you might sometimes see high values for the counter
     *  (70 percent or greater depending on your workload and environment),
     *  it might not indicate a problem; you need more data to understand this activity.
     *  For example, high processor-time values typically occur when you are starting a new
     *  process and should not cause concern.
     * @return double Accumulation of seconds.
     */
    double getCurrentlyUsedCPUPercentage();

    /**
     * @desc Initializes the needed resources for measuring current process CPU use.
     * @note This is a dumb for linux.
     * @return void.
     */
    void initCurrentlyUsedCPUByCurrentProcess();

    /**
     * @desc Retrieves measured current process CPU used.
     * @return double Accumulation of seconds.
     */
    double getCurrentlyUsedCPUByCurrentProcessPercentage();

    /**
     * @brief Número de procesadores.
     *
     * @return int
     */
    int getNumProcessors();

} // namespace SAM

#endif // GETCPUTIME_H

