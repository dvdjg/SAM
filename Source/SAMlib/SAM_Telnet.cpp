/****************************************************************************
** 
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
** 
** This file is part of a Qt Solutions component.
**
** Commercial Usage  
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
** 
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
** 
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
** 
** GNU General Public License Usage 
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
** 
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
** 
** If you are unsure which license is appropriate for your use, please
** contact Nokia at qt-info@nokia.com.
** 
****************************************************************************/

#include <QList>
#include <QMap>
#include <QPair>
#include <QVariant>
#include <QTcpSocket>
#include <QBuffer>
#include <QTimer>
#include <QRegExp>

#ifdef Q_OS_WIN
#  include <winsock2.h>
#endif
#if defined (Q_OS_UNIX)
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#endif

#define QTTELNET_DEBUG
#include "keepSocketAlive.h"

#include "SAM_JsonUtil.h"
#include "SAM_QEnumToString.h"
#include "SAM_Log.h"
#include "SAM_Telnet.h"

namespace SAM
{

class TelnetAuth
{
public:
    enum State { AuthIntermediate, AuthSuccess, AuthFailure };

    TelnetAuth(char code) : st(AuthIntermediate), cd(code) {}
    virtual ~TelnetAuth() {}

    int code() const { return cd; }
    State state() const { return st; }
    void setState(State state) { st = state; }

    virtual QByteArray authStep(const QByteArray &data) = 0;

private:
    State st;
    int   cd;
};


class TelnetAuthNull : public TelnetAuth
{
public:
    TelnetAuthNull() : TelnetAuth(0) {}

    QByteArray authStep(const QByteArray &data);
};

QByteArray TelnetAuthNull::authStep(const QByteArray &data)
{
    TRACE(TELNET);
    Q_ASSERT(data[0] == Telnet::Authentication);

    if (data.size() < 2 || data[1] != Telnet::SEND)
        return QByteArray();

    const char buf[8] = {Telnet::IAC, Telnet::SB, Telnet::Authentication,
                         Telnet::IS, Telnet::AUTHNULL, 0, // CLIENT|ONE-WAY
                         Telnet::IAC, Telnet::SE};
    setState(AuthSuccess);
    return QByteArray(buf, sizeof(buf));
}

class TelnetPrivate : public QObject
{
    Q_OBJECT

public:
    TelnetPrivate(Telnet *parent);
    ~TelnetPrivate();

    QMap<char, bool> modes;
    QList< QPair<char, char> > osent;

    Telnet *d;
    /**
     * @brief socket Socket que se usará en la comunicación por Telnet.
     * @todo Usar la clase TcpServer en su lugar para tener un mayor control del contenido.
     */
    QTcpSocket *socket;
    QBuffer buffer; ///< Buffer para recomponer las respuestas que se reciben del Servidor de Telnet.
    QStringList responseList; ///< Historial de mensajes recibidos del Telnet en respuesta de comandos.
    int maxStrings; ///< Cantidad máxima de cadenas que se almacenarán en `responseList`.

    QSize windowSize;

    bool nocheckp;
    bool triedlogin, triedpass, firsttry;

    QMap<int, TelnetAuth*> auths;
    TelnetAuth *curauth;
    bool nullauth;
    bool logedIn;

    QRegExp loginp, passp, promptp;
    QString login, pass;

    /**
     * @brief timerToRetry Temporizador para reintentar operaciones con un socket que no responde.
     * - Logarse de nuevo si se cierra la conexión.
     * - Esperar la respuesta a un comando.
     */
    QTimer *timerToRetry;
    QString hostName;
    quint16 port;

    bool allowOption(int oper, int opt);
    /**
     * @brief Envia una serie mínima de opciones de control al Servidor de Telnet.
     * @return El número de bytes que se han enviado.
     */
    qint64 sendOptions();
    qint64 sendCommand(const QByteArray &command);
    qint64 sendCommand(const char *command, int length);
    qint64 sendCommand(const char operation, const char option);
    void sendString(const QString &str);
    bool replyNeeded(char operation, char option);
    void setMode(char operation, char option);
    bool alreadySent(char operation, char option);
    void addSent(char operation, char option);
    void sendWindowSize();

    int  parsePlaintext(const QByteArray &data);
    int parseIAC(const QByteArray &data);
    bool isOperation(const char c);
    bool isCommand(const char c);
    QByteArray getSubOption(const QByteArray &data);
    void parseSubAuth(const QByteArray &data);
    void parseSubTT(const QByteArray &data);
    void parseSubNAWS(const QByteArray &data);
    /**
     * @brief Devuelve el valor opuesto a la operación que se envía.
     * @param operation
     * @param positive
     * @return
     */
    char opposite(char operation, bool positive);

    void consume();

    void setSocket(QTcpSocket *socket);

    bool isConnected() const;
    bool isUnconnected() const;
    void loggedIn();

public slots:
    qint64 socketConnected();
    void socketConnectionClosed();
    void socketReadyRead();
    void socketError(QAbstractSocket::SocketError error);

    /**
     * @brief Procesa la petición de reconexión establecida con Telnet::setTimeToRetryIfDisconnected().
     *
     * @sa Telnet::setTimeToRetryIfDisconnected()
     */
    void onTimerToRetry();
};

TelnetPrivate::TelnetPrivate(Telnet *parent)
    : d(parent), socket(0), maxStrings(10),
      nocheckp(false),
      triedlogin(false), triedpass(false), firsttry(true),
      curauth(0), nullauth(false), logedIn(false),
      loginp("ogin:\\s*$"), passp("assword:\\s*$"),
      promptp("\\$|#|>\\s*$"), // Parser de un prompt estándar: cualquier cosa que acabe en `$`, `#`, o `>`
      timerToRetry(new QTimer(parent)),
      port(0)
{
    buffer.open(QBuffer::ReadWrite);
    setSocket(new QTcpSocket(this));
    timerToRetry->setSingleShot(false);
    connect(timerToRetry, SIGNAL(timeout()),
            this, SLOT(onTimerToRetry()));
}

TelnetPrivate::~TelnetPrivate()
{
    TRACE(TELNET);

    delete socket;
    delete curauth;
}

void TelnetPrivate::onTimerToRetry()
{
    TRACE(TELNET);

    LOG_ALERT(TELNET) << "Timeout cuando se esperaba una respusta.";

    if(isUnconnected())
    {
        LOG_DEBUG(TELNET) << "Reintentando la conexión a" << hostName << ':' << port;
        d->close();
        d->connectToHost(hostName, port);
    }

    if(socket->bytesAvailable()) {
        // Si hubieran datos, intentar consumirlos en lugar de notificar el timeout
        socketReadyRead();
    } else {
        emit d->responseTimeout();
    }
}

bool TelnetPrivate::isConnected() const
{
    return socket->state() == QAbstractSocket::ConnectedState;
}

bool TelnetPrivate::isUnconnected() const
{
    return socket->state() == QAbstractSocket::UnconnectedState;
}

void TelnetPrivate::setSocket(QTcpSocket *s)
{
    TRACE(TELNET);
    if (socket) {
        d->logout();
        socket->flush();
    }
    delete socket;
    socket = s;
    if (socket)
    {
        connect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
        connect(socket, SIGNAL(disconnected()),
                this, SLOT(socketConnectionClosed()));
        connect(socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
        connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(socketError(QAbstractSocket::SocketError)));
    }
}

/*
   Returns the opposite value of the one we pass in.
*/
char TelnetPrivate::opposite(char operation, bool positive)
{
    TRACE(TELNET);

    // Hay problemas con el operador ternario con MinGW 4.7
    char ret = 0;
    if (operation == Telnet::DO)
    {
        if(positive)
            ret = Telnet::WILL;
        else
            ret = Telnet::WONT;
    }
    else if (operation == Telnet::DONT) // Not allowed to say WILL
    {
        ret = Telnet::WONT;
    }
    else if (operation == Telnet::WILL)
    {
        if(positive)
            ret = Telnet::DO;
        else
            ret = Telnet::DONT;
    }
    else if (operation == Telnet::WONT) // Not allowed to say DO
    {
        ret = Telnet::DONT;
    }
    return ret;
}

void TelnetPrivate::consume()
{
    TRACE(TELNET);
    qint64 pos = buffer.pos();
    buffer.seek(0);
    QByteArray data = buffer.read(pos);

    int currpos = 0;
    int prevpos = -1;
    while (prevpos < currpos && currpos < data.size())
    {
        int nParsed = 0;
        prevpos = currpos;
        const char c = char(data[currpos]);
        if (c == Telnet::DM) {
            nParsed = 1;
        } else if (c == Telnet::IAC) {
            nParsed = parseIAC(data.mid(currpos));
        } else { // Assume plain text
            nParsed = parsePlaintext(data.mid(currpos));
        }
        currpos += nParsed;
    }

//    if(currpos == 0 && pos > 4) {
//        // Si no se ha conseguido parsear nada pero hay datos en el buffer
//        // entonces desperdicia el primer byte.
//        ++currpos;
//        LOG_WARN(TELNET) << "Ha ocurrido un error de análisis de secuencias de Telnet.";
//    }

    buffer.seek(0);
    if (currpos < data.size()) {
        // Reinsertar lo que no se haya podido analizar
        buffer.write(data.mid(currpos));
    }
}

bool TelnetPrivate::isCommand(const char c)
{
    return (c == Telnet::DM);
}

bool TelnetPrivate::isOperation(const char c)
{
    return (c == Telnet::WILL || c == Telnet::WONT
         || c == Telnet::DO   || c == Telnet::DONT);
}

QByteArray TelnetPrivate::getSubOption(const QByteArray &data)
{
    TRACE(TELNET);
    Q_ASSERT(!data.isEmpty() && data[0] == Telnet::IAC);

    if (data.size() < 4 || data[1] != Telnet::SB)
        return QByteArray();

    for (int i = 2; i < data.size() - 1; ++i) {
        if (data[i] == Telnet::IAC && data[i+1] == Telnet::SE) {
            return data.mid(2, i-2);
        }
    }
    return QByteArray();
}

void TelnetPrivate::parseSubNAWS(const QByteArray &data)
{
    Q_UNUSED(data);
}

void TelnetPrivate::parseSubTT(const QByteArray &data)
{
    TRACE(TELNET);
    Q_ASSERT(!data.isEmpty() && data[0] == Telnet::TerminalType);

    if (data.size() < 2 || data[1] != Telnet::SEND)
        return;

    // Se envía un tipo de terminal desconocido
    const char c1[4] = { Telnet::IAC, Telnet::SB,
                         Telnet::TerminalType, Telnet::IS};
    sendCommand(c1, sizeof(c1));
    sendString("UNKNOWN");
    const char c2[2] = { Telnet::IAC, Telnet::SE };
    sendCommand(c2, sizeof(c2));
}

void TelnetPrivate::parseSubAuth(const QByteArray &data)
{
    TRACE(TELNET);
    Q_ASSERT(data[0] == Telnet::Authentication);

    if (!curauth && data[1] == Telnet::SEND) {
        int pos = 2;
        while (pos < data.size() && !curauth) {
            curauth = auths[data[pos]];
            pos += 2;

            if (curauth) {
                emit d->loginRequired();
                break;
            }
        }
        if (!curauth) {
            curauth = new TelnetAuthNull;
            nullauth = true;
            if (loginp.isEmpty() && passp.isEmpty()) {
                // emit q->loginRequired();
                nocheckp = true;
            }
        }
    }

    if (curauth) {
        const QByteArray a = curauth->authStep(data);
        if (!a.isEmpty())
            sendCommand(a);
        TelnetAuth::State state = curauth->state();
        if (state == TelnetAuth::AuthFailure)
        {
            emit d->loginFailed();
        }
        else if (state == TelnetAuth::AuthSuccess)
        {
            if (loginp.isEmpty() && passp.isEmpty()) {
                loggedIn();
            }
            if (!nullauth)
                nocheckp = true;
        }
    }
}

void TelnetPrivate::loggedIn()
{
    logedIn = true;
    buffer.seek(0);
    emit d->loggedIn();
}

/*
  returns number of bytes consumed
*/
int TelnetPrivate::parseIAC(const QByteArray &data)
{
    TRACE(TELNET);
    int ret = 0;
    int dataSize = data.size();
    if(dataSize > 0)
    {
        Q_ASSERT(char(data.at(0)) == Telnet::IAC);

        if (dataSize >= 3 && isOperation(data[1]))  // IAC, Operation, Option
        {
            const Telnet::Operation operation = (Telnet::Operation) data[1];
            const Telnet::Option option = (Telnet::Option) data[2];
            LOG_DEBUG(TELNET) << "Analizando Operación" >> Telnet::getEnumString(operation)
                              << ", opción" >> Telnet::getEnumString(option);
            if (operation == Telnet::WillNot && option == Telnet::Logout) {
                d->close();
            }
            else
            {
                if (operation == Telnet::DoNot && option == Telnet::Authentication) {
                    if (loginp.isEmpty() && passp.isEmpty()) {
                        loggedIn();
                    }
                    nullauth = true;
                }
                if (replyNeeded(operation, option)) {
                    bool allowed = allowOption(operation, option);
                    char opossed = opposite(operation, allowed);
                    sendCommand(opossed, option);
                    setMode(operation, option);
                }
            }
            ret = 3;
        }
        else
        {
            if (dataSize >= 2 && isCommand(data[1]))  // IAC Command
            {
                LOG_ALERT(TELNET) << "Ignorando IAC CM";
                ret = 2;
            }
            else
            {
                QByteArray suboption = getSubOption(data);
                if (!suboption.isEmpty())
                {
                    LOG_WARN(TELNET) << "Analizando la subopción \"IAC\""
                                     >> Telnet::getEnumString((Telnet::Option) suboption.at(0));
                    // IAC SB Operation SubOption [...] IAC SE
                    switch (suboption[0]) {
                    case Telnet::Authentication:
                        parseSubAuth(suboption);
                        break;
                    case Telnet::TerminalType:
                        parseSubTT(suboption);
                        break;
                    case Telnet::NAWS:
                        parseSubNAWS(data);
                        break;
                    default:
                        break;
                    }
                    ret = suboption.size() + 4;
                }
                else
                {
                    LOG_ALERT(TELNET) << "Recibida una subopción vacía.";
                }
            }
        }
    }

    return ret;
}

int TelnetPrivate::parsePlaintext(const QByteArray &data)
{
    TRACE(TELNET);
    int consumed = 0;
    int length = data.indexOf('\0');
    if (length == -1) {
        length = data.size();
        consumed = length;
    } else {
        consumed = length + 1; // + 1 for removing '\0'
    }

    QString text = QString::fromLocal8Bit(data.constData(), length);

    LOG_DEBUG(TELNET) << "Recibido el texto" << SAM::JsonUtil::stringify(text);

    if (!nocheckp && nullauth)
    {
        if (!promptp.isEmpty() && promptp.indexIn(text) != -1)
        {
            nocheckp = true;
            text.clear();
            loggedIn();
        }
    }

    if (!nocheckp && nullauth)
    {
        if (!loginp.isEmpty() && loginp.indexIn(text) != -1)
        {
            if (triedlogin || firsttry) {
                emit d->message(text);    // Display the login prompt
                text.clear();
                emit d->loginRequired();  // Get a (new) login
                firsttry = false;
            }
            if (!triedlogin) {
                d->sendData(login);
                triedlogin = true;
            }
        }
        if (!passp.isEmpty() && passp.indexIn(text) != -1)
        {
            if (triedpass || firsttry)
            {
                emit d->message(text);    // Display the password prompt
                text.clear();
                emit d->loginRequired();  // Get a (new) pass
                firsttry = false;
            }
            if (!triedpass)
            {
                d->sendData(pass);
                triedpass = true;
                text.clear();
                // We don't have to store the password anymore
                pass.fill(' ');
                pass.resize(0);
            }
        }
    }

    if (!text.isEmpty())
    {
        emit d->message(text); // Notifica el mensaje recibido en crudo

        // Las respuestas pueden venir troceadas en varios mensajes,
        // por lo que es necesario pegarlos de nuevo antes de procesarlos.
        if(!responseList.isEmpty())
        {
            // Acumula la respuesta en la última cadena recibida
            QString & currentResponse = responseList.last();
            currentResponse.append(text);

            QString trimmedResponse = currentResponse.trimmed();

            int promptIndex = trimmedResponse.size();

            // Comprobar que se ha recibido la respuesta completa
            // verificando que se muestra de nuevo el inductor de órdenes
            if (!promptp.isEmpty()) {
                promptIndex = promptp.indexIn(trimmedResponse);
            }

            if(promptIndex != -1) // Se ha encontrado una respuesta completa
            {
                QString strCommand;
                // La primera línea contiene el eco del comando enviado
                int firstLineIndex = trimmedResponse.indexOf("\n");
                if(firstLineIndex > 0) {
                    strCommand = trimmedResponse.left(firstLineIndex).trimmed();
                } else {
                    firstLineIndex = 0;
                }

                int n = promptIndex - firstLineIndex;
                QString strResponse = trimmedResponse.mid(firstLineIndex, n).trimmed();

                // Se ha recibido correctamente la respuesta al comando enviado
                timerToRetry->stop();
                emit d->commandResponse(strCommand, strResponse);
//                if(!strResponse.isEmpty() || !strCommand.isEmpty()) {
//                    emit d->commandResponse(strCommand, strResponse);
//                }
            }
        }
    }

    return consumed;
}

bool TelnetPrivate::replyNeeded(char operation, char option)
{
    TRACE(TELNET);
    if (operation == Telnet::DO || operation == Telnet::DONT) {
        // RFC854 requires that we don't acknowledge
        // requests to enter a mode we're already in
        if (operation == Telnet::DO && modes[option])
            return false;
        if (operation == Telnet::DONT && !modes[option])
            return false;
    }
    return true;
}

void TelnetPrivate::setMode(char operation, char option)
{
    TRACE(TELNET);
    if (operation != Telnet::DO && operation != Telnet::DONT)
        return;

    modes[option] = (operation == Telnet::DO);
    if (option == Telnet::NAWS && modes[Telnet::NAWS])
        sendWindowSize();
}

void TelnetPrivate::sendWindowSize()
{
    TRACE(TELNET);
    if (!modes[Telnet::NAWS])
        return;
    if (!d->isValidWindowSize())
        return;

    short h = htons(windowSize.height());
    short w = htons(windowSize.width());
    const char c[9] = { Telnet::IAC, Telnet::SB, Telnet::NAWS,
                        char(w & 0x00ff), char(w >> 8), char(h & 0x00ff), char(h >> 8),
                        Telnet::IAC, Telnet::SE };
    sendCommand(c, sizeof(c));
}

void TelnetPrivate::addSent(char operation, char option)
{
    TRACE(TELNET);
    osent.append(QPair<char, char>(operation, option));
}

bool TelnetPrivate::alreadySent(char operation, char option)
{
    TRACE(TELNET);
    QPair<char, char> value(operation, option);
    if (osent.contains(value)) {
        osent.removeAll(value);
        return true;
    }
    return false;
}

void TelnetPrivate::sendString(const QString &str)
{
    TRACE(TELNET);
    if (!isConnected() || str.length() == 0)
        return;

    socket->write(str.toLocal8Bit());
    LOG_DEBUG(TELNET) << "Enviada cadena" >> str;
}

qint64 TelnetPrivate::sendCommand(const QByteArray &command)
{
    TRACE(TELNET);
    if (!isConnected() || command.isEmpty())
        return 0;

    const Telnet::Operation comm = (Telnet::Operation) command.at(0);
    int commandSize = command.size();
    if (commandSize == 3) {
        const Telnet::Operation operation = (Telnet::Operation) command.at(1);
        const Telnet::Option option = (Telnet::Option) command.at(2);
        if (alreadySent(operation, option))
            return 0;
        LOG_DEBUG(TELNET) << "Enviando comando" << Telnet::getEnumString(comm)
                          >> Telnet::getEnumString(operation)
                            << ", option" >> Telnet::getEnumString(option);
        addSent(operation, option);
    }
    else if (commandSize == 2) {
        const Telnet::Operation operation = (Telnet::Operation) command.at(1);
        LOG_DEBUG(TELNET) << "Enviando comando" >> Telnet::getEnumString(comm)
                          >> Telnet::getEnumString(operation);
    } else {
        LOG_DEBUG(TELNET) << "Enviando comando" << Telnet::getEnumString(comm)
                          << "de" << commandSize << "bytes:" << command;
    }

    return socket->write(command);
}

qint64 TelnetPrivate::sendCommand(const char operation, const char option)
{
    TRACE(TELNET);
    const char c[3] = { Telnet::IAC, operation, option };
    return sendCommand(c, 3);
}

qint64 TelnetPrivate::sendCommand(const char *command, int length)
{
    TRACE(TELNET);
    QByteArray a(command, length);
    return sendCommand(a);
}

bool TelnetPrivate::allowOption(int /*oper*/, int opt)
{
    TRACE(TELNET);
    if (opt == Telnet::Authentication ||
        opt == Telnet::SuppressGoAhead ||
        opt == Telnet::LineMode ||
        opt == Telnet::Status ||
        opt == Telnet::Logout ||
        opt == Telnet::TerminalType)
        return true;
    if (opt == Telnet::NAWS && d->isValidWindowSize())
        return true;
    return false;
}

qint64 TelnetPrivate::sendOptions()
{
    TRACE(TELNET);
    qint64 ret = 0;
    ret += sendCommand(Telnet::WILL, Telnet::Authentication);
    ret += sendCommand(Telnet::DO, Telnet::SuppressGoAhead);
    ret += sendCommand(Telnet::WILL, Telnet::LineMode);
    ret += sendCommand(Telnet::DO, Telnet::Status);
    if (d->isValidWindowSize())
        ret += sendCommand(Telnet::WILL, Telnet::NAWS);
    return ret;
}

qint64 TelnetPrivate::socketConnected()
{
    TRACE(TELNET);
    qint64 ret = 0;

    LOG_DEBUG(TELNET) << "Conectado a" >> d->objectName();
    timerToRetry->stop();

    // On Windows Vista and later, the SO_KEEPALIVE socket option can only be set using the
    // setsockopt function when the socket is in a well-known state not a transitional state.
    int fd = socket->socketDescriptor();

    // Comprobar la comunicación con el servidor a los 5 segundos de la última actividad.
    // A partir de ahí, hacer 10 comprobaciones cada 1 segundo. Si no se ha recibido respuesta
    // activar la notificación de conexión rota QAbstractSocket::disconnected().
    keepSocketAlive(fd, 1, 5, 1); // Con inactividad se generará la señal disconnected() en 15s.

    ret = sendOptions();

    return ret;
}

void TelnetPrivate::socketConnectionClosed()
{
    TRACE(TELNET);
    if(logedIn) {
        logedIn = false;
        emit d->loggedOut();
    }

    LOG_DEBUG(TELNET) << "Cerrada la conexión.";

    if(timerToRetry->interval() > 0) {
        timerToRetry->start();
        LOG_DEBUG(TELNET) << "Reiniciado el temporizador con un intervalo de"
                          << timerToRetry->interval() / 1000 << "segundos.";
    }
}

void TelnetPrivate::socketReadyRead()
{
    TRACE(TELNET);

    if(socket->bytesAvailable())
    {
        QByteArray ba(socket->readAll());
        LOG_DEBUG(TELNET) << "Recibidos" << ba.size() << "bytes.";
        buffer.write(ba);
        consume();
    }
}

void TelnetPrivate::socketError(QAbstractSocket::SocketError error)
{
    TRACE(TELNET);

    // Ante una desconexión del socket, reintenta la conexión si se configuró así
    if(!isConnected() && timerToRetry->interval() > 0) {
        LOG_DEBUG(TELNET) << "Programando una reconexión dentro de"
                          << timerToRetry->interval() << "segundos.";
        timerToRetry->start();
    }

    emit d->socketError(error);
}


Telnet::Telnet(QObject *parent)
    : QObject(parent), d(new TelnetPrivate(this))
{
}

Telnet::~Telnet()
{
    delete d;
}

const char * Telnet::getEnumString(Operation e)
{
    static QEnumToString<Telnet> e2s("Operation");
    const char * szName = e2s(e);

    return szName;
}

const char * Telnet::getEnumString(Option e)
{
    static QEnumToString<Telnet> e2s("Option");
    const char * szName = e2s(e);

    return szName;
}

const char * Telnet::getEnumString(Auth e)
{
    static QEnumToString<Telnet> e2s("Auth");
    const char * szName = e2s(e);

    return szName;
}

const char * Telnet::getEnumString(Types e)
{
    static QEnumToString<Telnet> e2s("Types");
    const char * szName = e2s(e);

    return szName;
}

const char * Telnet::getEnumString(Modifiers e)
{
    static QEnumToString<Telnet> e2s("Modifiers");
    const char * szName = e2s(e);

    return szName;
}

const char * Telnet::getEnumString(SRA_TYPES e)
{
    static QEnumToString<Telnet> e2s("SRA_TYPES");
    const char * szName = e2s(e);

    return szName;
}


bool Telnet::isValid() const
{
    return d->socket->isValid();
}

void Telnet::connectToHost(const QString &host, quint16 port)
{
    if (!isUnconnected()) {
        // Sólo intentar la conexión cuando está en estado desconectado
        return;
    }

    if(d->timerToRetry->interval() > 0) {
        d->timerToRetry->start();
        LOG_DEBUG(TELNET) << "Iniciado el temporizador con un intervalo de"
                          << d->timerToRetry->interval() / 1000 << "segundos.";
    }

    clearResponses();
    d->responseList.append("Initializing\n"); // Pseudo comando

    d->hostName = host;
    d->port = port;
    d->socket->connectToHost(host, port);
}

void Telnet::close()
{
    TRACE(TELNET);

    d->osent.clear();
    d->modes.clear();
    d->auths.clear();
    if(d->curauth) {
        delete d->curauth;
        d->curauth = NULL;
    }

    d->nocheckp = false;
    d->nullauth = false;
    d->triedlogin = false;
    d->triedpass = false;
    d->firsttry = true;

    if (!d->isUnconnected())
    {
        d->socket->close();
        if(d->logedIn) {
            d->logedIn = false;
            emit loggedOut();
        }

        LOG_DEBUG(TELNET) << "Se ha cerrado el socket.";
    }
}

void Telnet::sendControl(Operation ctrl)
{
    TRACE(TELNET);
    bool sendsync = false;

    switch (ctrl) {
    case InterruptProcess: // Ctrl-C
    case AbortOutput: // suspend/resume output
    case AreYouThere:
        sendsync = true;
        break;
    case Break:
    case Suspend: // Ctrl-Z
    case EndOfFile:
    case Abort:
    case GoAhead:
    case EraseCharacter:
    case EraseLine:
        break;
    default:
        return;
    }

    const char command[2] = {Telnet::IAC, ctrl};
    d->sendCommand(command, sizeof(command));
    if (sendsync) {
        sendSync();
    }
}

void Telnet::setMaxResponseStrings(int maxStrings)
{
    if(maxStrings > 0) {
        d->maxStrings = maxStrings;
    }
}

int Telnet::getMaxResponseStrings() const
{
    return d->maxStrings;
}

qint64 Telnet::sendCommand(const QString &data)
{
    TRACE(TELNET);
    qint64 ret = 0;
    if (d->isConnected())
    {
        // No permitir que se acumulen más de d->maxStrings respuestas
        while(d->responseList.count() > d->maxStrings) {
            d->responseList.removeFirst();
        }

        // Reserva espacio para una nueva respuesta
        d->responseList.append(QString());

        ret = sendData(data);
    }

    return ret;
}

qint64 Telnet::sendData(const QString &data)
{
    TRACE(TELNET);
    qint64 ret = 0;
    if (d->isConnected())
    {
        QByteArray str = data.toLocal8Bit();
        ret = d->socket->write(str);
        ret += d->socket->write("\r\n\0", 3);

        if(ret > 0) {
            if(d->timerToRetry->interval() > 0) {
                d->timerToRetry->start();
                //LOG_DEBUG(TELNET) << "Reiniciado el temporizador con un intervalo de"
                //                  << d->timerToRetry->interval() / 1000 << "segundos.";
            }
            LOG_DEBUG(TELNET) << "Cadena enviada" >> data;
        } else {
            LOG_WARN(TELNET) << "No se ha podido enviar la cadena" >> data;
        }
    }
    else
    {
        LOG_WARN(TELNET) << "No se puede enviar la cadena" >> data <<
                            "porque el cliente de telnet no se encuentra conectado";
    }

    return ret;
}


QString Telnet::stripCR(const QString &msg)
{
    TRACE(TELNET);
    static QRegExp reg("(?:\r|\033\\[[0-9;]*[A-Za-z])");

    QString nmsg(msg);
    nmsg.remove(reg);
    return nmsg;
}

void Telnet::logout()
{
    TRACE(TELNET);
    d->sendCommand(Telnet::DO, Telnet::Logout);
}

void Telnet::setWindowSize(const QSize &size)
{
    TRACE(TELNET);
    setWindowSize(size.width(), size.height());
}

void Telnet::setWindowSize(int width, int height)
{
    TRACE(TELNET);
    bool wasvalid = isValidWindowSize();

    d->windowSize.setWidth(width);
    d->windowSize.setHeight(height);

    bool isValid = isValidWindowSize();

    if (wasvalid && isValid)
        d->sendWindowSize();
    else if (isValid)
        d->sendCommand(Telnet::WILL, Telnet::NAWS);
    else if (wasvalid)
        d->sendCommand(Telnet::WONT, Telnet::NAWS);
}

QSize Telnet::windowSize() const
{
    return (d->modes[Telnet::NAWS] ? d->windowSize : QSize());
}

bool Telnet::isValidWindowSize() const
{
    return windowSize().isValid();
}

void Telnet::setSocket(QTcpSocket *socket)
{
    d->setSocket(socket);
}

QTcpSocket *Telnet::socket() const
{
    return d->socket;
}

void Telnet::sendSync()
{
    TRACE(TELNET);
    if (d->isConnected())
    {
        d->socket->flush(); // Force the socket to send all the pending data before
        // sending the SYNC sequence.
        int s = d->socket->socketDescriptor();
        char tosend = Telnet::DM;
        ::send(s, &tosend, 1, MSG_OOB); // Send the DATA MARK as out-of-band
    }
}

void Telnet::setPromptPattern(const QRegExp &pattern)
{
    TRACE(TELNET);
    d->promptp = pattern;
}

void Telnet::setPromptString(const QString &pattern)
{
    setPromptPattern(QRegExp(QRegExp::escape(pattern)));
}

void Telnet::setLoginPattern(const QRegExp &pattern)
{
    d->loginp = pattern;
}

void Telnet::setLoginString(const QString &pattern)
{
    setLoginPattern(QRegExp(QRegExp::escape(pattern)));
}

void Telnet::setPasswordPattern(const QRegExp &pattern)
{
    d->passp = pattern;
}

void Telnet::setPasswordString(const QString &pattern)
{
    setPasswordPattern(QRegExp(QRegExp::escape(pattern)));
}

void Telnet::setTimeToRetry(int timeToRetry)
{
    TRACE(TELNET);
    d->timerToRetry->setInterval(timeToRetry * 1000);

    if(timeToRetry < 0) {
        // Con valores negativos se desactiva el mecanismo de notificación
        d->timerToRetry->stop();
    }

    if(!d->isConnected() && timeToRetry > 0) {
        d->timerToRetry->start();
        LOG_DEBUG(TELNET) << "Reiniciado el temporizador con un intervalo de"
                          << d->timerToRetry->interval() / 1000 << "segundos.";
    }
}

void Telnet::login(const QString &username, const QString &password)
{
    TRACE(TELNET);
    d->triedpass = d->triedlogin = false;
    d->login = username;
    d->pass = password;
}

bool Telnet::isConnected() const
{
    TRACE(TELNET);
    return d->isConnected();
}

bool Telnet::isUnconnected() const
{
    TRACE(TELNET);
    return d->isUnconnected();
}

bool Telnet::isLogged() const
{
    TRACE(TELNET);
    return d->logedIn;
}

QStringList Telnet::receivedResponses() const
{
    return d->responseList;
}

void Telnet::clearResponses()
{
    d->responseList.clear();
}

} // namespace SAM

#include "SAM_Telnet.moc"

