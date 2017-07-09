#ifndef SAM_CPUINFO_H
#define SAM_CPUINFO_H

#include <QString>
#include <QVariant>

namespace SAM
{
    class CPUInfo
    {
    protected:
#ifdef Q_OS_WIN
        void ParseIndetifier();

        QString m_identifier;       // eg: x86 Family 6 Model 6 Stepping 2
#endif
        QString	m_processorName;	// eg: AMD Athlon(tm) XP 1600+
        QString	m_vendorName;       // eg: AuthenticAMD
        unsigned long long m_speed; // MHz
        short m_family;             // family number
        short m_model;              // model number
        short m_stepping;           // stepping number

    public:
        CPUInfo();

        bool QueryCPUInfo();

        QString GetCPUName() const;
        QString GetVendorName() const;

        unsigned int GetSpeedMHz() const;

        short int GetCPUFamily() const;
        short int GetCPUModel() const;
        short int GetCPUStepping() const;

        QVariant toVariant() const;
    };
}

#endif // SAM_CPUINFO_H
