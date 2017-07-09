
#include <QTextStream>
#include <QStringList>
#include <QFile>
#include <QDateTime>
#include <QTime>
#include <limits>

#include "SAM_GetUngetC.h"
#include "SAM_Log.h"
#include "SAM_Helpers.h"
#include "SAM_JsonUtil.h"
#include "SAM_JVariant.h"
#include "SAM_ZIP.h"


namespace SAM
{

    QVariant JsonUtil::parseStringValue(QString value)
    {
        TRACE(PARSNG);

        QVariant ret; // null

        // Intenta almacenar el valor como un tipo básico Qt
        if(value == QLatin1String("SI") || value == QLatin1String("true")) {
            ret = true;
        }else if(value == QLatin1String("NO") || value == QLatin1String("false")) {
            ret = false;
        }
        else if(!value.isEmpty())
        {
            QStringList lst(value.split(',', QString::SkipEmptyParts));

            if(lst.count() > 1) {
                ret = lst; // Como lista de QStrings
            } else {
                bool ok;
                int i = value.toInt(&ok);

                if(ok) {
                    ret = i; // Como entero
                } else {
                    qlonglong ll = value.toLongLong(&ok);

                    if(ok) {
                        ret = ll; // Como qint64
                    } else {
                        double d = value.toDouble(&ok);

                        if(ok) {
                            ret = d; // Como double
                        } else {
                            ret = value; // Como cadena
                        }
                    }
                }
            }
        }

        return ret;
    }

    QVariant JsonUtil::parseScv(QString str)
    {
        QTextStream st(&str, QIODevice::ReadOnly);

        return parseScv(st);
    }

    QVariant JsonUtil::parseScv(QTextStream & in)
    {
        TRACE(PARSNG);

        int nParsed = 0;
        JVariant ret;

        QString line, tag, headerName, lastName, lastTypeParam, lastType;

        // Analiza el archivo línea a línea
        while( !in.atEnd() )
        {
            line = in.readLine().trimmed();

            // Ignora las líneas de comentarios
            if(line.length() == 0 || line.at(0) == ';' || line.at(0) == '*' || line.at(0) == '#')
                continue;

            // Captura los nombres de los TAGs
            if(line.at(0) == '[')
            {
                tag = line.mid(1, line.length()-2);
                headerName.clear();
                lastTypeParam.clear();
                lastType.clear();

                continue;
            }

            // Captura las asignaciones válidas
            QString name = line.section('=', 0, 0).trimmed();
            QString value = line.section('=', 1).trimmed();

            bool bIsJson = false;
            if(value.size() > 0)
            {
                QChar first = value.at(0);
                QChar last, expected = QChar::Null; // = value.at(value.size()-1);

                if(first == '\"') {
                    expected = '\"';
                } else if(first == '{') {
                    expected = '}';
                } else  if(first == '[') {
                    expected = ']';
                }

                if(expected != QChar::Null)
                {
                    bIsJson = true;
                    line = value;
                    while( true )
                    {
                        last = line.at(line.size()-1);

                        if(last == expected || in.atEnd()) {
                            break;
                        }
                        line = in.readLine().trimmed();
                        value += line;
                    }
                }
            }

//            if(bIsJson)
//            {
//                // Intentar parsearlo como si el valor fuera un documento JSON
//                (*tree)[name] = parseStringValue(value);
//            }
            if(value.endsWith(","))
            {
                // Si la cadena acaba en coma, quitarla: Formato de archivos de configuración
                value.chop(1);
                value = value.trimmed();
            }

            // Toma como nombre principal el de la primera etiqueta que se encuentre tras el TAG
            if(headerName.isEmpty())
            {
                // No considerar las reglas de nombre de elemento para la tabla T_CONFIG
                if(tag != "T_CONFIG" && (name == "NOMBRE" || name == "ID"))
                {
                    // Capturar el nombre de la etiqueta que encabezará los elementos siguientes
                    headerName = name;
                    lastTypeParam.clear();
                    lastType.clear();
                }
                else
                {
                    // Si no se sigue el formato del SAM, almacenar la etiqueta tal cual
                    JVariant * tree = &ret;

                    if(!tag.isEmpty()) {
                        // Permitir rutas en el tag: LOCAL/PRIMARY/VOIP_PORT
                        QStringList path(tag.split('/', QString::SkipEmptyParts));
                        tree = &(*tree)[path];
                    }

                    // Intenta almacenar el valor como un tipo básico Qt
                    if(bIsJson) {
                        (*tree)[name] = JsonUtil::parseJson(value);
                    } else {
                        (*tree)[name] = parseStringValue(value);
                    }

                    ++nParsed;

                    continue;
                }
            }

            if(name == headerName)
            {
                lastName = value;
            }
            else if(name == "TIPO_PARAM")
            {
                lastTypeParam = value;
            }
            else
            {
                JVariant * tree = &ret;

                if(!tag.isEmpty()) {
                    // Permitir rutas en el tag del estilo: LOCAL/PRIMARY/VOIP_PORT
                    QStringList path(tag.split('/', QString::SkipEmptyParts));
                    tree = &(*tree)[path];
                }

                if(!lastTypeParam.isEmpty() && lastTypeParam != tag)
                    tree = &(*tree)[lastTypeParam];

                if(headerName == "ID") {
                    int idx(lastName.toInt());
                    tree = &(*tree)[idx]; // Crea un List
                    (*tree)["ID"] = idx;
                } else {
                    tree = &(*tree)[lastName]; // Crea un Map
                }

                // Analiza una extrañeza dentro del formato de archivo del SCV del tipo:
                // ID_ENL= 1, HUECO= 1,
                // ID_ENL= 2, HUECO= 2,
                // ID_ENL= 3, HUECO= 3,
                // . . .
                bool bAssignList = false;
                QStringList strList = value.split(',', QString::SkipEmptyParts);

                if(strList.size() > 1)
                {
                    QString key = strList.first();
                    for (int i = 1; i < strList.size(); ++i)
                    {
                        QString val(strList[i]);
                        QStringList strValues = val.split('=', QString::SkipEmptyParts);
                        if(strValues.size() > 1)
                        {
                            QString k = strValues[0].trimmed();
                            QString v = strValues[1].trimmed();
                            (*tree)[name][key][k] = parseStringValue(v);

                            bAssignList = true;
                        }
                        else
                        {
                            bAssignList = false;
                            break;
                        }
                    }
                }

                if(!bAssignList) {
                    (*tree)[name] = parseStringValue(value);
                }

                if(name == "TIPO") {
                    lastType = value.toUpper();
                } else if(name == "VALOR") {
                    // Insertar el valor de forma "nativa" machacando los tags que se hubieran detectado:
                    // INT: valor entero
                    // STRING: valor de cadena
                    // BOOLEAN: valor booleano (SI / NO)
                    // LIST: lista de cadenas separadas por comas
                    // IP: dirección IP

                    if(lastType == "INT")
                    {
                        (*tree) = value.toInt();
                    }
                    else if(lastType == "BOOLEAN")
                    {
                        (*tree) = value.compare("NO", Qt::CaseInsensitive) != 0;
                    }
                    else if(lastType == "LIST")
                    {
                        QStringList slist = value.split(',');

                        (*tree) = slist; // Listas de cadenas separadas por comas
                    }
                    else if(lastType == "IP" || lastType == "STRING")
                    {
                        (*tree) = value;
                    }
                    else // No se encontró un tipo conocido. Intentar encontrarlo según el valor
                    {
                        (*tree) = parseStringValue(value);
                    }
                }

                ++nParsed;
            }
        }

        LOG_INFO(PARSNG) <<  "Analizadas" << nParsed << "entradas del archivo SCV.";

        return ret;
    }

    QVariant JsonUtil::parseFile(QString settingsFilePath, fn_parse_type fn_parse)
    {
        TRACE(PARSNG);

        QVariant ret;
        QFile file(settingsFilePath);

        if(file.open(QIODevice::ReadOnly))
        {
            // Zip format "local file header" fields:
            quint32 signature;

            QDataStream s(&file);
            s.setByteOrder(QDataStream::LittleEndian);
            s >> signature;
            if (signature == 0x04034b50) // zip local file header magic number
            {
                file.close();

                // Es un zip
                UnzipFileInfo fileInfo;

                QByteArray fileData;
                QDataStream decompressedFile(&fileData, QIODevice::WriteOnly);

                int read = unzipFirstFile(
                            qPrintable(settingsFilePath),
                            decompressedFile,
                            &fileInfo);

                LOG_INFO(PARSNG) << "Se han descomprimido" << read
                                 << "bytes del archivo comprimido" >> fileInfo.szFileName
                                 << "encontrado dentro de" >> settingsFilePath;

                QTextStream in(fileData);

                ret = fn_parse(in);
            }
            else
            {
                // Consideramos que es un archivo de texto normal
                file.seek(0);
                QTextStream in(&file);

                ret = fn_parse(in);
                file.close();
            }

            if(ret.isNull()) {
                LOG_WARN(CONF) <<  "No se ha podido analizar el archivo"
                               >> settingsFilePath << ".";
            } else {
                LOG_INFO(PARSNG) <<  "Analizado el archivo" >> settingsFilePath;
            }
        }
        else
        {
            LOG_ERROR(CONF) <<  "No se puede abrir el archivo de configuración" >> settingsFilePath
                << ". Compruebe que existe y que tiene permisos de lectura:" >> file.errorString();
        }

        return ret;
    }


    QString JsonUtil::stringify(QString in, bool bInQuotations)
    {
        TRACE(PARSNG);

        QString out;
        out.reserve(in.size()+2);

        if(bInQuotations) {
            out.append(QChar('\"'));
        }

        for(QString::ConstIterator i = in.constBegin(); i != in.constEnd(); i++)
        {
            QChar qchar(*i);

            if( qchar == QChar('\b'))
                out.append("\\b");
            else if( qchar == QChar('\f'))
                out.append("\\f");
            else if( qchar == QChar('\n'))
                out.append("\\n");
            else if( qchar == QChar('\r'))
                out.append("\\r");
            else if( qchar == QChar('\t'))
                out.append("\\t");
            else if( qchar == QChar('\f'))
                out.append("\\f");
            else if( qchar == QChar('\\'))
                out.append("\\\\");
            else if( qchar == QChar('\"'))
                out.append("\\\"");
            else if(!qchar.isPrint())
            {
                // Si no es imprimible, obtiene su secuencia Unicode UTF-16
                int val = qchar.digitValue();
                out.append(QString("\\u%1").arg(QString::number(val, 16), 4, '0'));
            }
            else
                out.append(*i);
        }

        if(bInQuotations) {
            out.append(QChar('\"'));
        }

        return out;
    }

    QString JsonUtil::stringify(QVariant v)
    {
        TRACE(PARSNG);

        if (v.isNull()){
            return "null";
        }
        QVariant::Type t = v.type();
        if (t == QVariant::String) {
            return stringify(v.toString());
        }
        else if (t == QVariant::Bool) {
            static const QString strTrue("true");
            static const QString strFalse("false");
            return v.toBool() ? strTrue : strFalse;
        } else if(t ==  QVariant::Int) {
            return QString::number(v.toInt());
        } else if(t ==  QVariant::UInt) {
            return QString::number(v.toUInt());
//        } else if(t ==  QVariant::Url) {
//            return v.toUrl();
        } else if(t ==  QVariant::LongLong) {
            return QString::number(v.toLongLong());
        } else if(t ==  QVariant::ULongLong) {
            return QString::number(v.toULongLong());
        } else if(t ==  QVariant::Double) {
            double dbl = v.toDouble();
            if(dbl >= -std::numeric_limits<double>::max()
            && dbl <= std::numeric_limits<double>::max()){
                return QString::number(dbl);
            } else {
                static const QString strNull("null");
                return strNull; // Representación JSON para (nan, inf, -inf).
            }
        } else if(t ==  QVariant::DateTime) {
            return QString("\"%1\"").arg(v.toDateTime().toString(g_dateTimeFormat));
        } else if(t ==  QVariant::Time) {
            return QString("\"%1\"").arg(v.toTime().toString(g_dateTimeFormat));
        } else if(t == QVariant::Map) {
            QString r="{";
            QMap<QString, QVariant> map = v.toMap();
            QMapIterator<QString, QVariant> i(map);
            while (i.hasNext()){
                i.next();
                r+=QString("%1:%2,")
                        .arg(stringify(i.key()))
                        .arg(stringify(i.value()));
            }
            if(r.length()>1)
                r.chop(1); // Quita la última coma
            r+="}";
            return r;
#if QT_VERSION >= 0x040800
        } else if(t == QVariant::Hash) {
            QString r="{";
            QHash<QString, QVariant> map = v.toHash();
            QHashIterator<QString, QVariant> i(map);
            while (i.hasNext()){
                i.next();
                r+=QString("%1:%2,")
                        .arg(stringify(i.key()))
                        .arg(stringify(i.value()));
            }
            if(r.length()>1)
                r.chop(1); // Quita la última coma
            r+="}";
            return r;
#endif // QT_VERSION >= 0x040800
        } else if(t == QVariant::StringList) {
            QString r="[";
            QStringList l = v.toStringList();
            foreach(QString i, l){
                r+=stringify(i)+",";
            }
            if(r.length()>1)
                r.chop(1);
            r+="]";
            return r;
        } else if(t == QVariant::List) {
            QString r="[";
            QList<QVariant> l = v.toList();
            foreach(QVariant i, l){
                r+=stringify(i)+",";
            }
            if(r.length()>1)
                r.chop(1);
            r+="]";
            return r;
        } else if(t == QVariant::ByteArray) {
            QByteArray ba = v.toByteArray();
            QString base64(ba.toBase64());

            return QString("\"%1\"").arg(base64);
        } else {
            // Intenta transformar el objeto no soportado a cadena de texto
            return stringify(v.toString());
        }

        return QString();
    }

    class ParseJsonInner : public Getungetc
    {
    public:
        ParseJsonInner(QTextStream & s) : Getungetc(s) {}

        QVariant parseValue(bool & error);
        QMap<QString, QVariant> parseObject (bool & error);
        QList<QVariant> parseArray (bool & error);
        QString parseString (bool & error);
        QVariant parseLiteral (bool & error);
    };

    QVariant JsonUtil::parseJson(QTextStream & s)
    {
        TRACE(PARSNG);

        ParseJsonInner parser(s);
        bool error=false;
        QVariant v = parser.parseValue(error);
        if(error)
            return QVariant();
        return v;
    }


    QVariant JsonUtil::parseJson(QString str)
    {
        TRACE(PARSNG);

        QTextStream ts(&str);
        return parseJson(ts);
    }

    QVariant ParseJsonInner::parseValue(bool & error)
    {
        TRACE(PARSNG);

        QVariant ret;
        skipWhiteSpace();
        QChar c;
        while(!atEnd() && !error){
            c=getc();
            if (c=='{'){
                ret = parseObject(error); break;
            } else if (c=='"'){
                ret = parseString(error); break;
            } else if (c=='['){
                ret = parseArray(error); break;
            } else {
                ungetc(c);
                ret = parseLiteral(error); break;
            }
            skipWhiteSpace();
        }
        return ret;
    }


    QMap<QString, QVariant> ParseJsonInner::parseObject(bool & error)
    {
        TRACE(PARSNG);

        skipWhiteSpace();
        QMap<QString, QVariant> o;
        QString key;
        bool atVal=false;

        QChar c;
        while(!atEnd() && !error){
            c=getc();
            if (c=='}'){
                return o;
            } else if (c==',' || c==':'){
                /*
                  They're syntactic sugar, since key:value come in bundles anyway
                  Could check for error handling. too lazy.
                */
            } else if (atVal){
                ungetc(c);
                QVariant v = parseValue(error);
                o[key]=v;
                atVal=false;
            } else if (c=='"'){
                key=parseString(error);
                atVal=true;
            } else {
                break;
            }
            skipWhiteSpace();
        }
        error=true;
        return QMap<QString, QVariant>();
    }


    QList<QVariant> ParseJsonInner::parseArray(bool & error)
    {
        TRACE(PARSNG);

        skipWhiteSpace();
        QList<QVariant> l;
        QChar c;
        while(!atEnd() && !error){
            c=getc();
            if (c==']'){
                return l;
            } else if (c==','){
            } else {
                ungetc(c);
                QVariant v = parseValue(error);
                l.append(v);
            }
            skipWhiteSpace();
        }
        error=true;
        return QList<QVariant>();
    }


    QString ParseJsonInner::parseString (bool & error)
    {
        TRACE(PARSNG);

        QString str;
        QChar c;
        while(!atEnd() && !error)
        {
            c=getc();
            if(c=='"')
            {
                return str;
            }
            else if(c=='\\')
            {
                c=getc();
                if(c=='b'){
                    str.append('\b');
                }else if(c=='f'){
                    str.append('\f');
                }else if(c=='n'){
                    str.append('\n');
                }else if(c=='r'){
                    str.append('\r');
                }else if(c=='t'){
                    str.append('\t');
                }else if(c=='f'){
                    str.append('\f');
                }else if(c=='u'){
                    QString k;
                    for (int i = 0; i < 4; i++ ) {
                        c=getc();
                        k.append(c);
                    }
                    bool ok;
                    int i = k.toInt(&ok, 16);
                    if (ok)
                        str.append(QChar(i));
                } else {
                    str.append(c);
                }
            } else {
                str.append(c);
            }
        }
        error=true;
        return QString();
    }

    QVariant ParseJsonInner::parseLiteral(bool & error)
    {
        TRACE(PARSNG);

        QChar c;
        while(!atEnd() && !error){
            c=getc();
            if (c=='t'){
                c=getc();//r
                c=getc();//u
                c=getc();//e
                return true;
            } else if (c=='f'){
                c=getc();//a
                c=getc();//l
                c=getc();//s
                c=getc();//e
                return false;
            } else if(c=='n'){
                c=getc();//u
                c=getc();//l
                c=getc();//l
                return QVariant();
            } else if(c=='-' || c.isDigit()){
                QString n;
                while(( c.isDigit()  || (c=='.') || (c=='E') || (c=='e') || (c=='-') || (c=='+') )){
                    n.append(c);
                    if(atEnd() ||  error)
                        break;
                    c=getc();
                }
                ungetc(c);
                if(n.contains('.')) {
                    return n.toDouble();
                } else {
                    bool ok = false;
                    int result = n.toInt(&ok);
                    if(ok){
                        return result;
                    }
                    return n.toLongLong();
                }
            }
        }
        error=true;
        return QVariant();
    }
}
