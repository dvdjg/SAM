#include <QTextStream>

#include "SAM_CPUInfo.h"

#ifdef Q_OS_WIN
#include <Windows.h>

#define CPU_REG_KEY		HKEY_LOCAL_MACHINE
#define CPU_REG_SUBKEY	"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"

#define CPU_SPEED		"~MHz"
#define CPU_IDENTIFIER	"Identifier"
#define CPU_NAME		"ProcessorNameString"
#define CPU_VENDOR		"VendorIdentifier"
#else
#include <stdio.h>
#endif

namespace SAM
{

    CPUInfo::CPUInfo() : m_speed(0), m_family(0), m_model(0), m_stepping(0)
    {
        QueryCPUInfo();
    }

#ifdef Q_OS_WIN
    void CPUInfo::ParseIndetifier()
    {
        int index = m_identifier.indexOf("Family");
        m_identifier = m_identifier.mid(index);

        QString temp;
        QTextStream ssIdentifier(&m_identifier);
        ssIdentifier >> temp >> m_family >> temp >> m_model >> temp >> m_stepping;
    }

    bool CPUInfo::QueryCPUInfo()
    {
        HKEY hKey;
        DWORD dwType;
        DWORD dwSize;
        LONG ret = RegOpenKeyExA(CPU_REG_KEY,
                     CPU_REG_SUBKEY,
                     0,
                     KEY_READ,
                     &hKey);

        // key could not be opened
        if(ret != ERROR_SUCCESS)
            return false;

        // get the speed of CPU
        dwSize = sizeof(m_speed);
        ret = RegQueryValueExA(hKey,			// key handle
                        CPU_SPEED,				// "~MHz"
                        NULL,					// reserved, must be NULL
                        &dwType,				// returns the data type
                        (LPBYTE)&m_speed,		// data buffer
                        &dwSize);				// size of the buffer

        if(ret != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return false;
        }

        // get CPU indetifier (family, model, stepping)
        // first call is to determine the size of buffer
        dwSize = 0;
        ret = RegQueryValueExA(hKey,	CPU_IDENTIFIER, NULL, &dwType, NULL,	&dwSize);

        QByteArray sIdentifier(dwSize, 0);

        // second call will retrieve the data
        ret = RegQueryValueExA(hKey,	CPU_IDENTIFIER, NULL, &dwType, (LPBYTE)sIdentifier.data(), &dwSize);

        if(ret != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return false;
        }

        // get CPU name
        dwSize = 0;
        ret = RegQueryValueExA(hKey,	CPU_NAME, NULL, &dwType, NULL,	&dwSize);

        QByteArray sProcessorName(dwSize, 0);
        ret = RegQueryValueExA(hKey,	CPU_NAME, NULL, &dwType, (LPBYTE)sProcessorName.data(), &dwSize);

        if(ret != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return false;
        }

        // get vendor name
        dwSize = 0;
        ret = RegQueryValueExA(hKey,	CPU_VENDOR, NULL, &dwType, NULL,	&dwSize);

        QByteArray sVendorName(dwSize, 0);
        ret = RegQueryValueExA(hKey,	CPU_VENDOR, NULL, &dwType, (LPBYTE)sVendorName.data(), &dwSize);

        if(ret != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return false;
        }

        RegCloseKey(hKey);

        m_identifier    = QString::fromLatin1(sIdentifier);
        m_processorName = QString::fromLatin1(sProcessorName);
        m_vendorName    = QString::fromLatin1(sVendorName);

        ParseIndetifier();

        return true;
    }

#elif defined(Q_OS_LINUX)

    bool CPUInfo::QueryCPUInfo()
    {
        FILE* file;
        char line[128];

        file = fopen("/proc/cpuinfo", "r");
        int numProcessors = 0;
        while(fgets(line, 128, file) != NULL){
            if(strncmp(line, "processor", 9) == 0) {
                numProcessors++;
            } else if(strncmp(line, "model name", 10) == 0) {
                m_processorName = QString::fromLocal8Bit(line + 13).trimmed();
            } else if(strncmp(line, "vendor_id", 9) == 0) {
                m_vendorName = QString::fromLocal8Bit(line + 12).trimmed();
            } else if(strncmp(line, "cpu MHz", 7) == 0) {
                QString num(QString::fromLocal8Bit(line + 11).trimmed());
                m_speed = (long long) num.toDouble();
            } else if(strncmp(line, "cpu family", 9) == 0) {
                m_family = QString::fromLocal8Bit(line + 12).toShort();
            } else if(strncmp(line, "model", 5) == 0) {
                m_model = QString::fromLocal8Bit(line + 8).toShort();
            } else if(strncmp(line, "stepping", 8) == 0) {
                m_stepping = QString::fromLocal8Bit(line + 11).toShort();
            }
        }
        fclose(file);

        return numProcessors > 0;
    }
#endif

    unsigned int CPUInfo::GetSpeedMHz() const
    {
        return m_speed;
    }

    QString CPUInfo::GetCPUName() const
    {
        return m_processorName;
    }

    QString CPUInfo::GetVendorName() const
    {
        return m_vendorName;
    }

    short int CPUInfo::GetCPUFamily() const
    {
        return m_family;
    }

    short int CPUInfo::GetCPUModel() const
    {
        return m_model;
    }

    short int CPUInfo::GetCPUStepping() const
    {
        return m_stepping;
    }

    QVariant CPUInfo::toVariant() const
    {
        QVariantMap ret;

        ret["CPUName"] = GetCPUName();
        ret["vendorName"] = GetVendorName();
        ret["speedGHz"] = GetSpeedMHz() / 1000.0;
        ret["CPUFamily"] = GetCPUFamily();
        ret["CPUModel"] = GetCPUModel();
        ret["CPUStepping"] = GetCPUStepping();

        return ret;
    }
}
