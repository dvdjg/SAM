#ifndef SAM_QENUMTOSTRING_H
#define SAM_QENUMTOSTRING_H

#include <QString>
#include <QMetaEnum>

namespace SAM
{
    /**
     *  Clase auxiliar para transformar un tipo enumerado registrado en Qt a su
     *  representación en forma de texto.
     */
    template<typename T>
    class QEnumToString
    {
    protected:
        QMetaEnum m_metaEnum;
    public:
        typedef T type;
        QEnumToString(const char * szEnumName)
        {
            const QMetaObject &mo = T::staticMetaObject;
            int index = mo.indexOfEnumerator(szEnumName);
            m_metaEnum = T::staticMetaObject.enumerator(index);
        }

        const char * valueToKey(int value)
        {
            const char * szRet = m_metaEnum.valueToKey(value);
            return szRet;
        }

        const char * operator()(int value)
        {
            return valueToKey(value);
        }
    };

}
#endif // SAM_QENUMTOSTRING_H
