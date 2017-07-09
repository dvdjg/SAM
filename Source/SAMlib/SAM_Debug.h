#ifndef SAM_DEBUG_H
#define SAM_DEBUG_H

//#include <QDebug>
#include <QMap>
#include <QTextStream>
#include <QContiguousCache>

namespace SAM
{
    class Debug
    {
    protected:
        struct Stream {
            Stream(QIODevice *device) : ts(device), ref(1), type(QtDebugMsg), space(true), message_output(false) {}
            Stream(QString *string) : ts(string, QIODevice::WriteOnly), ref(1), type(QtDebugMsg), space(true), message_output(false) {}
            Stream(QtMsgType t) : ts(&buffer, QIODevice::WriteOnly), ref(1), type(t), space(true), message_output(true) {}
            QTextStream ts;
            QString buffer;
            int ref;
            QtMsgType type;
            bool space;
            bool message_output;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
            QMessageLogContext context;
#endif
        } *stream;
    public:
        inline Debug(QIODevice *device) : stream(new Stream(device)) {}
        inline Debug(QString *string) : stream(new Stream(string)) {}
        inline Debug(QtMsgType t) : stream(new Stream(t)) {}
        inline Debug(const Debug &o):stream(o.stream) { ++stream->ref; }
        inline Debug &operator=(const Debug &other);
        inline ~Debug() {
            if (!--stream->ref) {
                if(stream->message_output) {
                    QT_TRY {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
                        qt_message_output(stream->type,
                                          stream->context,
                                          stream->buffer);
#else
                        qt_message_output(stream->type, stream->buffer.toLocal8Bit().data());
#endif
                    } QT_CATCH(std::bad_alloc&) { /* We're out of memory - give up. */ }
                }
                delete stream;
            }
        }

        inline Debug &space() { stream->space = true; stream->ts << ' '; return *this; }
        inline Debug &nospace() { stream->space = false; return *this; }
        inline Debug &maybeSpace() { if (stream->space) stream->ts << ' '; return *this; }

        bool autoInsertSpaces() const { return stream->space; }
        void setAutoInsertSpaces(bool b) { stream->space = b; }

        inline Debug &operator<<(QChar t) { stream->ts << '\'' << t << '\''; return maybeSpace(); }
        inline Debug &operator<<(bool t) { stream->ts << (t ? "true" : "false"); return maybeSpace(); }
        inline Debug &operator<<(char t) { stream->ts << t; return maybeSpace(); }
        inline Debug &operator<<(signed short t) { stream->ts << t; return maybeSpace(); }
        inline Debug &operator<<(unsigned short t) { stream->ts << t; return maybeSpace(); }
        inline Debug &operator<<(signed int t) { stream->ts << t; return maybeSpace(); }
        inline Debug &operator<<(unsigned int t) { stream->ts << t; return maybeSpace(); }
        inline Debug &operator<<(signed long t) { stream->ts << t; return maybeSpace(); }
        inline Debug &operator<<(unsigned long t) { stream->ts << t; return maybeSpace(); }
        inline Debug &operator<<(qint64 t) { stream->ts << QString::number(t); return maybeSpace(); }
        inline Debug &operator<<(quint64 t) { stream->ts << QString::number(t); return maybeSpace(); }
        inline Debug &operator<<(float t) { stream->ts << t; return maybeSpace(); }
        inline Debug &operator<<(double t) { stream->ts << t; return maybeSpace(); }
        inline Debug &operator<<(const char* t) { stream->ts << QString::fromUtf8(t).trimmed(); return maybeSpace(); }
        inline Debug &operator<<(const ushort* t) { stream->ts << QString::fromUtf16(t).trimmed(); return maybeSpace(); }
        inline Debug &operator<<(const QString & t) { stream->ts << t; return maybeSpace(); }
        inline Debug &operator<<(const QStringRef & t) { return operator<<(t.toString()); }
        inline Debug &operator<<(const QLatin1String &t) { stream->ts << t; return maybeSpace(); }
        //inline Debug &operator<<(const QByteArray & t) { stream->ts  << t; return maybeSpace(); }
        inline Debug &operator<<(const void * t) { stream->ts << t; return maybeSpace(); }
        inline Debug &operator<<(QTextStreamFunction f)
        { stream->ts << f; return *this; }

        inline Debug &operator<<(QTextStreamManipulator m)
        { stream->ts << m; return *this; }

        /**
         * @brief Imprime el resultado entre comillas dobles.
         *
         * @param t
         * @return Debug &operator >>
         */
        template<typename T>
        inline Debug &operator>>(const T & t)
        {
            bool space = stream->space;
            stream->space = false;
            stream->ts << '\"';
            (*this) << t;
            stream->ts << '\"';
            stream->space = space;
            return maybeSpace();
        }
    };

inline Debug& operator<<(Debug& debug, const QByteArray &ba)
{
    debug.nospace() << '[';
    for (int i = 0; i < ba.count(); ++i) {
        if (i)
            debug << ", ";
        debug << (unsigned char) ba.at(i);
    }
    debug << ']';
    return debug.space();
}

template <class T>
inline Debug& operator<<(Debug& debug, const QList<T> &list)
{
    debug.nospace() << '(';
    for (typename QList<T>::size_type i = 0; i < list.count(); ++i) {
        if (i)
            debug << ", ";
        debug << list.at(i);
    }
    debug << ')';
    return debug.space();
}

template <typename T>
inline Debug& operator<<(Debug& debug, const QVector<T> &vec)
{
    debug.nospace() << "QVector";
    return operator<<(debug, vec.toList());
}

template <class aKey, class aT>
inline Debug& operator<<(Debug& debug, const QMap<aKey, aT> &map)
{
    debug.nospace() << "QMap(";
    for (typename QMap<aKey, aT>::const_iterator it = map.constBegin();
         it != map.constEnd(); ++it) {
        debug << '(' << it.key() << ", " << it.value() << ')';
    }
    debug << ')';
    return debug.space();
}

#if QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
template <class aKey, class aT>
inline Debug& operator<<(Debug& debug, const QHash<aKey, aT> &hash)
{
    debug.nospace() << "QHash(";
    for (typename QHash<aKey, aT>::const_iterator it = hash.constBegin();
            it != hash.constEnd(); ++it)
        debug << '(' << it.key() << ", " << it.value() << ')';
    debug << ')';
    return debug.space();
}
#endif // QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)

#if QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
template <class T1, class T2>
inline Debug& operator<<(Debug& debug, const QPair<T1, T2> &pair)
{
    debug.nospace() << "QPair(" << pair.first << ',' << pair.second << ')';
    return debug.space();
}
#endif // QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)

template <typename T>
inline Debug& operator<<(Debug& debug, const QSet<T> &set)
{
    debug.nospace() << "QSet";
    return operator<<(debug, set.toList());
}

template <class T>
inline Debug& operator<<(Debug& debug, const QContiguousCache<T> &cache)
{
    debug.nospace() << "QContiguousCache(";
    for (int i = cache.firstIndex(); i <= cache.lastIndex(); ++i) {
        debug << cache[i];
        if (i != cache.lastIndex())
            debug << ", ";
    }
    debug << ')';
    return debug.space();
}

template <class T>
inline Debug& operator<<(Debug& debug, const QFlags<T> &flags)
{
    debug.nospace() << "QFlags(";
    bool needSeparator = false;
    for (uint i = 0; i < sizeof(T) * 8; ++i) {
        if (flags.testFlag(T(1 << i))) {
            if (needSeparator)
                debug.nospace() << '|';
            else
                needSeparator = true;
            debug.nospace() << "0x" << QByteArray::number(T(1 << i), 16).constData();
        }
    }
    debug << ')';
    return debug.space();
}

class NoDebug
{
public:
    inline NoDebug(){}
    inline NoDebug(const Debug &){}
    inline ~NoDebug(){}

#if !defined( QT_NO_TEXTSTREAM )
    inline NoDebug &operator<<(QTextStreamFunction) { return *this; }
    inline NoDebug &operator<<(QTextStreamManipulator) { return *this; }
#endif
    inline NoDebug &space() { return *this; }
    inline NoDebug &nospace() { return *this; }
    inline NoDebug &maybeSpace() { return *this; }

    template<typename T>
    inline NoDebug &operator<<(const T &) { return *this; }

    template<typename T>
    inline NoDebug &operator>>(const T &) { return *this; }
};

}

#endif // SAM_DEBUG_H
