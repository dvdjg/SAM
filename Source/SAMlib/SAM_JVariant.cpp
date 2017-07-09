#include <QStringList>
#include <QVariant>
#include <QDateTime>

#include "SAM_Log.h"
#include "SAM_Helpers.h"
#include "SAM_JVariant.h"
#include "SAM_JsonUtil.h"

namespace SAM
{

    static JVariant s_jv; /**< Variable global para cuando hay que devolver un JVariant con errores. */

    JVariant::operator QDateTime  () const { return toDateTime(); }

    QString JVariant::toJson() const
    {
        return JsonUtil::stringify(*this);
    }

    JVariant & JVariant::parse(QTextStream & s)
    {
        return *this = JsonUtil::parseJson(s);
    }

    JVariant & JVariant::parse(QString & s)
    {
        return *this = JsonUtil::parseJson(s);
    }

    JVariant & JVariant::parseFile(QString & fileName)
    {
        return *this = JsonUtil::parseJsonFile(fileName);
    }

    JVariant & JVariant::operator=(const QStringList & v)
    {
        QVariant::setValue(v);
        return *this;
    }

    JVariant & JVariant::operator=(const QList<QVariant> & v)
    {
        QVariant::setValue(v);
        return *this;
    }

    int JVariant::remove(int i)
    {
        TRACE(PARSNG);

        int nRemoved = 0;

        if(i >= 0)
        {
            QVariant::Type varType = type();

            if(varType == QVariant::List)
            {
                QList<QVariant>& ret = asList();

                if(i < ret.size()){
                    ret.removeAt(i);
                    nRemoved = 1;
                }
            }
            else if(varType == QVariant::StringList)
            {
                QStringList& ret = asStringList();

                if(i < ret.size()){
                    ret.removeAt(i);
                    nRemoved = 1;
                }
            }
            else
            {
                QString s = QString::number(i);

               if(varType == QVariant::Map)
                {
                    QMap<QString, QVariant>& ret = asMap();
                    nRemoved = ret.remove(s);
                }
#if QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
               else if(varType == QVariant::Hash)
                {
                    QHash<QString, QVariant>& ret = asHash();
                    nRemoved = ret.remove(s);
                }
#endif
                else
                {
                    LOG_INFO(PARSNG) <<  "El nodo no tiene un tipo soportado por JSON para borrar "
                                      << i << ": " << QVariant::typeToName(varType);
                }
            }
        }

        return nRemoved;
    }

    int JVariant::remove(const QString & s)
    {
        TRACE(PARSNG);

        int nRemoved = 0;

        QVariant::Type varType = type();

        if(varType == QVariant::Map)
        {
            QMap<QString, QVariant>& ret = asMap();
            nRemoved = ret.remove(s);
        }
#if QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
        else if(varType == QVariant::Hash)
        {
            QHash<QString, QVariant>& ret = asHash();
            nRemoved = ret.remove(s);
        }
#endif // QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
        else
        {
            bool ok;
            int i = s.toInt(&ok);

            if(ok)
            {
                // Intentar borrar con la cadena transformada a entero.
                if(varType == QVariant::List)
                {
                    QList<QVariant>& ret = asList();

                    if(i < ret.size()){
                        ret.removeAt(i);
                        nRemoved = 1;
                    }
                }
                else if(varType == QVariant::StringList)
                {
                    QStringList& ret = asStringList();

                    if(i < ret.size()){
                        ret.removeAt(i);
                        nRemoved = 1;
                    }
                }
                else
                {
                    ok = false;
                }
            }

            if(!ok)
            {
                LOG_WARN(CONF) <<  "El nodo no tiene un tipo soportado por JSON para borrar con remove("
                    << s << ") :" << QVariant::typeToName(varType);
            }
        }

        return nRemoved;
    }


    JVariant JVariant::operator[](int i) const
    {
        return at(i);
    }

    JVariant JVariant::at(int i) const
    {
        TRACE(PARSNG);

        if(i >= 0)
        {
            QVariant::Type varType = type();

            if(varType == QVariant::List) {
                return toList()[i];
            } else if(varType == QVariant::StringList) {
                return QVariant(toStringList()[i]);
            } else {
                QString s = QString::number(i);

                if(varType == QVariant::Map)
                    return toMap()[s];
                else if(varType == QVariant::Hash)
                    return toHash()[s];
            }

            LOG_DEBUG(CONF) <<  "El nodo tiene un tipo" << QVariant::typeToName(varType)
                            << "no soportado por JSON para indexar con [" << i << "]";
        }

        return JVariant();
    }

    bool JVariant::contains(int i) const
    {
        TRACE(PARSNG);

        QVariant::Type varType = type();

        if(varType == QVariant::StringList)
        {
            return i >= 0 && i < toStringList().size();
        }
        else if(varType == QVariant::List)
        {
            return i >= 0 && i < toList().size();
        }
        else
        {
            QString s = QString::number(i);

            if(varType == QVariant::Map)
                return toMap().contains(s);
            else if(varType == QVariant::Hash)
                return toHash().contains(s);
        }

        LOG_DEBUG(CONF) <<  "El nodo tiene un tipo" << QVariant::typeToName(varType)
                        << "no soportado por JSON para indexar con [" << i << "]";

        return false;
    }

    JVariant JVariant::operator[](const QString & s) const
    {
        return at(s);
    }

    JVariant JVariant::at(const QString & s) const
    {
        TRACE(PARSNG);

        QVariant::Type varType = type();

        if(varType == QVariant::Map)
            return toMap()[s];
        else if(varType == QVariant::Hash)
            return toHash()[s];
        else if(varType == QVariant::StringList)
            return QVariant(toStringList()[s.toInt()]);
        else if(varType == QVariant::List)
            return toList()[s.toInt()];

        LOG_ALERT(CONF) << "El nodo tiene un tipo" << QVariant::typeToName(varType)
                        << "no soportado por JSON para indexar con [" >> s << "]";

        return JVariant();
    }

    bool JVariant::contains(const QString & s) const
    {
        TRACE(PARSNG);

        QVariant::Type varType = type();

        if(varType == QVariant::Map)
        {
            return toMap().contains(s);
        }
        else if(varType == QVariant::Hash)
        {
            return toHash().contains(s);
        }
        else
        {
            int i = s.toInt();

            if(varType == QVariant::StringList)
                return i >= 0 && i < toStringList().size();
            else if(varType == QVariant::List)
                return i >= 0 && i < toList().size();
        }

        return false;
    }

    JVariant JVariant::operator[](const char * sz) const
    {
        return at(sz);
    }

    JVariant JVariant::at(const char * sz) const
    {
        return at((const QString &) sz);
    }

    int JVariant::size() const
    {
        TRACE(PARSNG);
        int ret = 0;

        QVariant::Type varType = type();

        if(varType == QVariant::List)
            ret = toList().size();
        else if(varType == QVariant::StringList)
            ret = toStringList().size();
        if(varType == QVariant::Map)
            ret = toMap().size();
        else if(varType == QVariant::Hash)
            ret = toHash().size();

        return ret;
    }

    JVariant & JVariant::merge(const JVariant & other)
    {
        TRACE(PARSNG);
        QString key;
        QVariant value;

        if(isNull()) {
            setValue((QVariant) other);
        } else if(other.type() == QVariant::Map) {
            QMapIterator<QString, QVariant> i(other.toMap());
            while (i.hasNext()){
                i.next(); key = i.key(); value = i.value();
                at(key).merge(value); // Copia y posiblemente sobreescribe los valores
            }
#if QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
        } else if(other.type() == QVariant::Hash) {
            QHashIterator<QString, QVariant> i(other.toHash());
            while (i.hasNext()){
                i.next(); key = i.key(); value = i.value();
                at(key).merge(value); // Copia y posiblemente sobreescribe los valores
            }
#endif // QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
        } else if(other.type() == QVariant::List) {
            QListIterator<QVariant> i(other.toList());
            int idx = 0;
            while (i.hasNext()){
                value = i.next();
                at(idx++).merge(value); // Copia y posiblemente sobreescribe los valores
            }
        } else if(!other.isNull()) {
            setValue((QVariant) other);
        }

        return *this;
    }

    int & JVariant::asInt()
    {
        TRACE(PARSNG);

        if(type() != QVariant::Int)
        {
            // Si éste nodo no tiene el tipo int, lo convierte
            setValue(this->toInt());
        }

        int & ret = as< int >();

        return ret;
    }

    uint & JVariant::asUInt()
    {
        TRACE(PARSNG);

        if(type() != QVariant::UInt)
        {
            // Si éste nodo no tiene el tipo int, lo convierte
            setValue(this->toUInt());
        }

        uint & ret = as< uint >();

        return ret;
    }

    qlonglong & JVariant::asLongLong()
    {
        TRACE(PARSNG);

        if(type() != QVariant::LongLong)
        {
            // Si éste nodo no tiene el tipo qlonglong, lo convierte
            setValue(this->toLongLong());
        }

        qlonglong & ret = as< qlonglong >();

        return ret;
    }

    qulonglong & JVariant::asULongLong()
    {
        TRACE(PARSNG);

        if(type() != QVariant::ULongLong)
        {
            // Si éste nodo no tiene el tipo qulonglong, lo convierte
            setValue(this->toULongLong());
        }

        qulonglong & ret = as< qulonglong >();

        return ret;
    }

    double & JVariant::asDouble()
    {
        TRACE(PARSNG);

        if(type() != QVariant::Double)
        {
            // Si éste nodo no tiene el tipo double, lo convierte
            setValue(this->toDouble());
        }

        double & ret = as< double >();

        return ret;
    }

    QString & JVariant::asString()
    {
        TRACE(PARSNG);

        if(type() != QVariant::String)
        {
            // Si éste nodo no tiene el tipo QString, lo convierte
            setValue(this->toString());
        }

        QString & ret = as< QString >();

        return ret;
    }

    QByteArray & JVariant::asByteArray()
    {
        TRACE(PARSNG);

        if(type() != QVariant::ByteArray)
        {
            // Si éste nodo no tiene el tipo QByteArray, lo convierte
            setValue(this->toByteArray());
        }

        QByteArray & ret = as< QByteArray >();

        return ret;
    }

    QList<QVariant> & JVariant::asList()
    {
        TRACE(PARSNG);

        if(type() != QVariant::List)
        {
            // Si éste nodo no tiene el tipo List, lo convierte a QList<QVariant>
            setValue(this->toList());
        }

        QList<QVariant>& ret = as< QList<QVariant> >();

        return ret;
    }

    QStringList & JVariant::asStringList()
    {
        TRACE(PARSNG);

        QVariant::Type varType = type();

        if(varType != QVariant::StringList)
        {
            // Si éste nodo no tiene el tipo StringList, lo convierte a QStringList
            setValue(this->toStringList());
        }

        QStringList& ret = as< QStringList >();

        return ret;
    }

    QMap<QString, QVariant> & JVariant::asMap()
    {
        TRACE(PARSNG);

        QVariant::Type varType = type();

        if(varType != QVariant::Map)
        {
            // Si éste nodo no tiene el tipo Map, lo convierte a QMap<QString, QVariant>
            setValue(this->toMap());
        }

        QMap<QString, QVariant>& ret = as< QMap<QString, QVariant> >();

        return ret;
    }

#if QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
    QHash<QString, QVariant> & JVariant::asHash()
    {
        TRACE(PARSNG);

        QVariant::Type varType = type();

        if(varType != QVariant::Hash)
        {
            // Si éste nodo no tiene el tipo Hash, lo convierte a QHash<QString, QVariant>
            setValue(this->toHash());
        }

        QHash<QString, QVariant>& ret = as< QHash<QString, QVariant> >();

        return ret;
    }
#endif // QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)

    JVariant & JVariant::operator[](int i)
    {
        return at(i);
    }

    JVariant & JVariant::first()
    {
        TRACE(PARSNG);

        // Convierte el objeto a QList<QVariant>
        QList<QVariant> & lst = asList();

        if(lst.isEmpty())
            lst.append(QVariant());

        return (JVariant &) lst.first();
    }


    JVariant JVariant::first() const
    {
        TRACE(PARSNG);

        if(type() == QVariant::List)
        {
            QList<QVariant> lst = toList();

            if(!lst.isEmpty())
                return lst.first();
        }

        return JVariant();
    }

    JVariant & JVariant::last()
    {
        TRACE(PARSNG);

        // Convierte el objeto a QList<QVariant>
        QList<QVariant> & lst = asList();

        if(lst.isEmpty())
            lst.append(QVariant());

        return (JVariant &) lst.last();
    }

    JVariant JVariant::last() const
    {
        TRACE(PARSNG);

        if(type() == QVariant::List)
        {
            QList<QVariant> lst = toList();

            if(!lst.isEmpty())
                return lst.last();
        }

        return JVariant();
    }

    JVariant & JVariant::at(int i)
    {
        TRACE(PARSNG);

        if(isNull())
        {
            // Si éste nodo no tiene un tipo establecido, lo convierte a QList<QVariant>
            QList<QVariant> lst;
            setValue(lst);
        }

        if(i >= 0)
        {
            QVariant::Type varType = type();

            if(varType == QVariant::List)
            {
                QList<QVariant>& ret = asList();

                // Reserva espacio creando elementos a null si fuera necesario
                while(ret.size() <= i)
                    ret.append(QVariant());

                return (JVariant &)ret.at(i);
            }
//            else if(varType == QVariant::StringList)
//            {
//                QStringList& ret = as< QStringList >();

//                // Reserva espacio creando elementos a null si fuera necesario
//                while(ret.size() <= i)
//                    ret.append(QString());

//                return (JVariant &)ret.at(i);
//            }
            else
            {
                QString s = QString::number(i);

                if(varType == QVariant::Map)
                    return (JVariant &) asMap()[s];
#if QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
                else if(varType == QVariant::Hash)
                    return (JVariant &) asHash()[s];
#endif
            }

            LOG_DEBUG(CONF) <<  "El nodo no tiene un tipo soportado por JSON para indexar con ["
                             << i << "]: " << QVariant::typeToName(varType);
        }

        s_jv.clear();
        return s_jv;
    }

    JVariant & JVariant::operator[](const QString & s)
    {
        return at(s);
    }

    JVariant & JVariant::at(const QString & s)
    {
        TRACE(PARSNG);

        if(isNull())
        {
            // Si éste nodo no tiene un tipo establecido, lo convierte a QMap<QString, QVariant>
            QMap<QString, QVariant> map;
            setValue(map);
        }

        QVariant::Type varType = type();

        if(varType == QVariant::Map)
        {
            return (JVariant &)asMap()[s];
        }
#if QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
        else if(varType == QVariant::Hash)
        {
            return (JVariant &)asHash()[s];
        }
#endif

        LOG_DEBUG(CONF) <<  "El nodo no tiene un tipo soportado por JSON para indexar con ["
                        >> s << "]: " << QVariant::typeToName(varType);

        s_jv.clear();
        return s_jv;
    }

    JVariant & JVariant::operator[](const char * sz)
    {
        return at(sz);
    }

    JVariant & JVariant::at(const char * sz)
    {
        return at((const QString &) sz);
    }

    JVariant & JVariant::operator[] (const QStringList & path)
    {
        return at(path);
    }

    JVariant & JVariant::at(const QStringList & path)
    {
        TRACE(PARSNG);

        JVariant * p = this;

        QStringListIterator it(path);

        while(it.hasNext())
        {
            QString key(it.next());

            p = &p->at(key);
        }

        return *p;
    }

    JVariant JVariant::operator[] (const QStringList & path) const
    {
        return at(path);
    }

    JVariant JVariant::at(const QStringList & path) const
    {
        TRACE(PARSNG);

        JVariant p = *this;

        QStringListIterator it(path);

        while(it.hasNext())
        {
            QString key(it.next());

            p = p.at(key);
        }

        return p;
    }

//    JVariant & JVariant::operator=(const QStringRef & v)
//    {
//        setValue(v.toString());
//        return *this;
//    }

    JVariant JVariant::operator-() const
    {
        QVariant::Type varType = type();

        if(varType == QVariant::Int)
            return JVariant(-toInt());
        if(varType == QVariant::Double)
            return JVariant(-toDouble());
        if(varType == QVariant::LongLong)
            return JVariant(-toLongLong());

        // Si no es un tipo numérico con signo, devolverlo tal cual
        return *this;
    }

}

