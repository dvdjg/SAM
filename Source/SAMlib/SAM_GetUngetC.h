#ifndef SAM_GETUNGETC_H
#define SAM_GETUNGETC_H

#include <QTextStream>
#include <QStack>

namespace SAM
{
    /**
     * @brief Clase adaptadora para que un QTextStream permita la operación ungetc()
     *  (reinserción de caracteres por la salida del stream) aunque sea de sólo lectura.
     *
     *  Esto es necesario para el parser de JSON de SAM::JsonUtil, el cual tan solo
     *  necesita analizar un carácter por adelantado.
     */
    class Getungetc
    {
        QTextStream &m_s;
        QStack<QChar> m_stack;
    public:
        Getungetc(QTextStream &s) : m_s(s) {}

        QChar getc();
        void ungetc(QChar c);

        void skipWhiteSpace();
        bool atEnd() const;
    };

    QChar Getungetc::getc()
    {
        QChar c;
        if(m_stack.isEmpty()) {
            m_s >> c;
        } else {
            c = m_stack.pop();
        }

        return c;
    }

    void Getungetc::ungetc(QChar c)
    {
        m_stack.push(c);
    }

    void Getungetc::skipWhiteSpace()
    {
        while(!atEnd())
        {
            QChar c=getc();
            if(!c.isSpace()) {
                ungetc(c);
                break;
            }
        }
    }

    bool Getungetc::atEnd() const
    {
        return m_stack.isEmpty() && m_s.atEnd();
    }
}

#endif // SAM_GETUNGETC_H
