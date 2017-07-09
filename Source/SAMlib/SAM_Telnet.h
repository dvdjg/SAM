/** @file SAM_Telnet.h
 *  @brief Este archivo y su correspondiente implementación SAM_Telnet.cpp son
 *  modificaciones de los originales descargados de la Web de Nokia en donde se
 *  mostraba un ejemplo de un cliente Telnet realizado a bajo nivel mediante sockets.
 *  Los archivos resultantes están fuertemente modificados sobre los originales.
 *
 *  @legalese Se reproduce el mensaje de licencia original:
 *
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
 */


#ifndef QTTELNET_H
#define QTTELNET_H

#include <QString>
#include <QSize>
#include <QStringList>
#include <QAbstractSocket>

class QTcpSocket;

#if defined(Q_OS_WIN)
#  if !defined(QT_QTTELNET_EXPORT) && !defined(QT_QTTELNET_IMPORT)
#    define QT_QTTELNET_EXPORT
#  elif defined(QT_QTTELNET_IMPORT)
#    if defined(QT_QTTELNET_EXPORT)
#      undef QT_QTTELNET_EXPORT
#    endif
#    define QT_QTTELNET_EXPORT __declspec(dllimport)
#  elif defined(QT_QTTELNET_EXPORT)
#    undef QT_QTTELNET_EXPORT
#    define QT_QTTELNET_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTTELNET_EXPORT
#endif

namespace SAM
{
class TelnetPrivate;

/*!
    \class Telnet
    \brief The Telnet class proveds an API to connect to Telnet servers,
    issue commands and receive replies.

    When a Telnet object has been created, you need to call
    connectToHost() to establish a connection with a Telnet server.
    When the connection is established the connected() signal is
    emitted. At this point you should call login(). The
    Telnet object will emit socketError() if the connection
    fails, and authenticationFailed() if the login() failed.

    Once the connection has been successfully established and
    you've logged in you can send control messages using sendControl()
    and data using sendData(). Connect to the message() signal to
    receive data from the Telnet server. The connection is closed with
    close().

    You can use your own socket if you call setSocket() before
    connecting. The socket used by Telnet is available from
    socket().
    @note Esta clase ha sido modificada respecto a la que se encuentra en
    el repositorio de Qt.
*/
class QT_QTTELNET_EXPORT Telnet : public QObject
{
    Q_OBJECT

    Q_ENUMS(Operation Option Auth Types Modifiers SRA_TYPES)
    Q_PROPERTY( bool isValid READ isValid )
    Q_PROPERTY( bool isConnected READ isConnected )
    Q_PROPERTY( bool isLogged READ isLogged )
    Q_PROPERTY( QStringList receivedResponses READ receivedResponses )

    Q_DISABLE_COPY(Telnet)

    friend class TelnetPrivate;

public:
    // Telnet Commands
    static const char CEOF  = (char) 236;
    static const char SUSP  = (char) 237;
    static const char ABORT = (char) 238;
    static const char SE    = (char) 240;
    static const char NOP   = (char) 241;
    static const char DM    = (char) 242;
    static const char BRK   = (char) 243;
    static const char IP    = (char) 244;
    static const char AO    = (char) 245;
    static const char AYT   = (char) 246;
    static const char EC    = (char) 247;
    static const char EL    = (char) 248;
    static const char GA    = (char) 249;
    static const char SB    = (char) 250;
    static const char WILL  = (char) 251;
    static const char WONT  = (char) 252;
    static const char DO    = (char) 253;
    static const char DONT  = (char) 254;
    static const char IAC   = (char) 255;

    /**
     * @brief Mensajes que se puede enviar al servidor de Telnet usando `sendControl()`
     */
    enum Operation
    {
        EndOfFile = CEOF, ///< Envía el mensaje de control End Of File.
        Suspend = SUSP, ///< Ctrl+Z. Suspende el proceso en ejecución en el servirdor.
        Abort = ABORT, ///< Envía el mensaje de control ABORT.
        SubnegotiationEnds = SE, ///< Final de los parámetros de la subnegociación Tipo de Terminal
        NoOperation = NOP, ///< Sin operación.
        DataMark = DM, ///< Indica la posición de un evento de sincronización dentro de la secuencia de datos. Esto siempre debe acompañarse de una notificación urgente de TCP.
        Break = BRK, ///< Envía el mensaje de control BREAK. Indica que la tecla "pausa" o "atención" era alta.
        InterruptProcess = IP, ///< Ctrl+C. Suspender. Interrumpir o cancelar el proceso al que está conectado el NVT (Network Virtual Terminal).
        AbortOutput = AO, ///< Anular/Reanudar salida. Esto se utiliza para suprimir la transmisión de salida del proceso remoto. Un carácter AO (238) se incluye en la secuencia de datos.
        AreYouThere = AYT, ///< Comprobar que la conexión sigue viva. Devolver a la NVT alguna evidencia visible que se ha recibido el AYT.
        EraseCharacter = EC, ///< Borrar el último carácter introducido. El receptor debe eliminar el último carácter no eliminado anterior de la secuencia de datos.
        EraseLine = EL, ///< Borrar la última línea. Eliminar caracteres de la secuencia de datos a pero sin incluir el carácter CRLF anterior.
        GoAhead = GA, ///< Siga adelante. En determinadas circunstancias que se usa para indicar que el otro extremo que se puede transmitir.
        SubnegotiationBegins = SB, ///< Subnegociación Tipo de Terminal de la opción indicada.
        Will = WILL, ///< Indica el deseo de comenzar a realizar, o la confirmación de que va a realizar, la opción indicada.
        WillNot = WONT, ///< Indica la negativa a realizar o continuar realizando la opción indicada.
        Do = DO, ///< 	Indica que la solicitud que llevan a cabo la otra parte, o confirmación de que se espera la otra parte para llevar a cabo, la opción indicada.
        DoNot = DONT, ///< Indica la demanda que dejar de realizar la otra parte, o confirmación de que ya no se espera la otra parte para llevar a cabo, la opción indicada.
        InterpretAsCommand = IAC ///< Se interpreta como un comando
    };

    enum Option
    {
        IS    = 0,
        SEND  = 1,

        Authentication = 37, ///< RFC1416,  implemented to always return NULL
        SuppressGoAhead = 3, ///< RFC858

        Echo = 1, ///< RFC857, not implemented (returns WONT/DONT)
        LineMode = 34, ///< RFC1184, implemented
        LineModeEOF = 236, ///< RFC1184, not implemented
        LineModeSUSP = 237,
        LineModeABORT = 238,
        Status = 5, ///< RFC859, should be implemented!
        Logout = 18, ///< RFC727, implemented
        TerminalType = 24, ///< RFC1091,  implemented to always return UNKNOWN
        NAWS = 31, ///< RFC1073, implemented
        TerminalSpeed = 32, ///< RFC1079, not implemented
        FlowControl = 33, ///< RFC1372, should be implemented?
        XDisplayLocation = 35, ///< RFC1096, not implemented
        EnvironmentOld = 36, ///< RFC1408, should not be implemented!
        Environment = 39, ///< RFC1572, should be implemented
        Encrypt = 38 ///< RFC2946, not implemented
    };

    enum Auth
    {
        REPLY = 2,
        NAME
    };

    enum Types
    {
        AUTHNULL, // Can't have enum values named NULL :/
        KERBEROS_V4,
        KERBEROS_V5,
        SPX,
        SRA = 6,
        LOKI = 10
    };

    enum Modifiers
    {
        AUTH_WHO_MASK = 1,
        AUTH_CLIENT_TO_SERVER = 0,
        AUTH_SERVER_TO_CLIENT = 1,
        AUTH_HOW_MASK = 2,
        AUTH_HOW_ONE_WAY = 0,
        AUTH_HOW_MUTUAL = 2
    };

    enum SRA_TYPES
    {
        SRA_KEY = 0,
        SRA_USER = 1,
        SRA_CONTINUE = 2,
        SRA_PASSWORD = 3,
        SRA_ACCEPT = 4,
        SRA_REJECT = 5
    };

public:
    Telnet(QObject *parent = 0);
    /*!
        Destroys the Telnet object. This will also close
        the connection to the server.

        \sa logout()
    */
    ~Telnet();

    /**
     * @brief Devuelve una representación en forma de texto del tipo enumerado `e`.
     * @param e
     * @return Cadena de texto correspondiente al tipo enumerado.
     */
    static const char * getEnumString(Operation e);
    /**
     * @brief Devuelve una representación en forma de texto del tipo enumerado `e`.
     * @param e
     * @return Cadena de texto correspondiente al tipo enumerado.
     */
    static const char * getEnumString(Option e);
    /**
     * @brief Devuelve una representación en forma de texto del tipo enumerado `e`.
     * @param e
     * @return Cadena de texto correspondiente al tipo enumerado.
     */
    static const char * getEnumString(Auth e);
    /**
     * @brief Devuelve una representación en forma de texto del tipo enumerado `e`.
     * @param e
     * @return Cadena de texto correspondiente al tipo enumerado.
     */
    static const char * getEnumString(Types e);
    /**
     * @brief Devuelve una representación en forma de texto del tipo enumerado `e`.
     * @param e
     * @return Cadena de texto correspondiente al tipo enumerado.
     */
    static const char * getEnumString(Modifiers e);
    /**
     * @brief Devuelve una representación en forma de texto del tipo enumerado `e`.
     * @param e
     * @return Cadena de texto correspondiente al tipo enumerado.
     */
    static const char * getEnumString(SRA_TYPES e);

    /*!
        Calling this function will make the Telnet object attempt to
        connect to a Telnet server specified by the given \a host and \a
        port.

        The connected() signal is emitted if the connection
        succeeds, and the connectionError() signal is emitted if the
        connection fails. Once the connection is establishe you must call
        login().

        \sa close()
    */
    void connectToHost(const QString &host, quint16 port = 23);

    /**
     * @brief Informa si el socket es válido y está preparado para ser usado,
     *  independientemente de que se encuentre en estado conectado o no.
     *
     * @return bool
     */
    bool isValid() const;

    /**
     * @brief Establece el tiempo en segundos que transcurrirá hasta que se reintente la comunicación
     *  con el servidor de telnet si el socket actual no está respondiendo.
     *  Usar un valor negativo para desactivar el timer.
     *
     * @param timeToRetry
     */
    void setTimeToRetry(int timeToRetry = 20);

    /*!
        Sets the \a username and \a password to be used when logging in to
        the server.

        \sa setLoginPattern(), setPasswordPattern()
    */
    void login(const QString &user, const QString &pass);

    /*!
        Sets the client window size to \a size.

        The width and height are given in number of characters.
        Non-visible clients should pass an invalid size (i.e. QSize()).

        \sa isValidWindowSize()
    */
    void setWindowSize(const QSize &size);
    /*!
        Sets the client window size.

        The \a width and \a height are given in number of characters.

        \overload
    */
    void setWindowSize(int width, int height); // In number of characters

    /*!
        Returns the window's size. This will be an invalid size
        if the Telnet server is not using the NAWS option (RFC1073).

        \sa isValidWindowSize()
    */
    QSize windowSize() const;

    /*!
        Returns true if the window size is valid, i.e.
        windowSize().isValid() returns true; otherwise returns false.

        \sa setWindowSize()
    */
    bool isValidWindowSize() const;

    /*!
        Set the \a socket to be used in the communication.

        This function allows you to use your own QSocket subclass. You
        should call this function before calling connectToHost(); if you
        call it after a connection has been established the connection
        will be closed, so in all cases you will need to call
        connectToHost() after calling this function.

        \sa socket(), connectToHost(), logout()
    */
    void setSocket(QTcpSocket *socket);

    /*!
        Returns the QTcpSocket instance used by this telnet object.

        \sa setSocket()
    */
    QTcpSocket *socket() const;

    /*!
        Sets the expected shell prompt pattern.

        The \a pattern is used to automatically recognize when the client
        has successfully logged in. When a line is read that matches the
        \a pattern, the loggedIn() signal will be emitted.

        \sa login(), loggedIn()
    */
    void setPromptPattern(const QRegExp &pattern);

    /*!
        \fn void Telnet::setPromptString(const QString &pattern)

        Sets the expected shell prompt to \a pattern.

        \overload
    */
    void setPromptString(const QString &pattern);

    /**
     * @brief Establece el número máximo de respuestas que se van a mantener en el historial.
     * @param maxStrings
     */
    void setMaxResponseStrings(int maxStrings);

    /**
     * @brief Devuelve el número máximo de respuestas que se mantienen en el historial.
     * @param maxStrings
     */
    int getMaxResponseStrings() const;

    /**
     * @brief stripCR Borra los códigos de control del terminal y el retorno de carro.
     * @param msg
     */
    static QString stripCR(const QString &msg);

public Q_SLOTS:
    /*!
        Cierra la conexión al servidor Telnet.

        \sa connectToHost() login()
    */
    void close();

    /*!
        This function will log you out of the Telnet server.
        You cannot send any other data after sending this command.

        \sa login() sendData() sendControl()
    */
    void logout();

    /*!
        Sends the control message \a ctrl to the Telnet server the
        Telnet object is connected to.

        \sa Control sendData() sendSync()
    */
    void sendControl(Operation ctrl);

    /*!
        Sends the string \a data to the Telnet server. This is often a
        command the Telnet server will execute.

        \sa sendControl()
    */
    qint64 sendData(const QString &data);
    qint64 sendCommand(const QString &data);

    /*!
        Sends the Telnet \c SYNC sequence, meaning that the Telnet server
        should discard any data waiting to be processed once the \c SYNC
        sequence has been received. This is sent using a TCP urgent
        notification.

        \sa sendControl()
    */
    void sendSync();

Q_SIGNALS:
    /*!
        \fn void Telnet::loginRequired()

        This signal is emitted when the Telnet class sees
        that the Telnet server expects authentication and you
        have not already called login().

        As a reply to this signal you should either call
        login() or logout()

        \sa login(), logout()
    */
    void loginRequired();

    /*!
        \fn void Telnet::loginFailed()

        This signal is emitted when the login has failed.
        Do note that you might in certain cases see several
        loginRequired() signals being emitted but no
        loginFailed() signals. This is due to the Telnet
        specification not requiring the Telnet server to
        support reliable authentication methods.

        \sa login(), loginRequired()
    */
    void loginFailed();

    /*!
        \fn void Telnet::loggedIn()

        This signal is emitted when you have been logged in
        to the server as a result of the login() command
        being called. Do note that you might never see this
        signal even if you have been logged in, due to the
        Telnet specification not requiring Telnet servers
        to notify clients when users are logged in.

        \sa login(), setPromptPattern()
    */
    void loggedIn();

    /*!
        \fn void Telnet::loggedOut()

        This signal is emitted when you have called logout()
        and the Telnet server has actually logged you out.

        \sa logout(), login()
    */
    void loggedOut();

    /*!
        \fn void Telnet::connectionError(QAbstractSocket::SocketError error)

        This signal is emitted if the underlying socket
        implementation receives an error. The \a error
        argument is the same as being used in
        QSocket::error()
    */
    void socketError(QAbstractSocket::SocketError error);

    /*!
        \fn void Telnet::message(const QString &data)

        This signal is emitted when the Telnet object
        receives more \a data from the Telnet server.

        \sa sendData()
    */
    void message(const QString &data);

    /**
     * @brief commandResponse Se ha recibido la respuesta al último comando solicitado.
     * @param command Eco del comando que se envió al SAGE
     * @param response Respuesta del SAGE
     */
    void commandResponse(QString command, QString response);

    void responseTimeout();

public:
    /*!
        Sets the expected login pattern.

        The \a pattern is used to automatically recognize when the server
        asks for a username. If no username has been set, the
        loginRequired() signal will be emitted.

        \sa login()
    */
    void setLoginPattern(const QRegExp &pattern);

    /*!
        \fn void Telnet::setLoginString(const QString &login)

        Sets the expected login string to \a login.

        \overload
    */
    void setLoginString(const QString &pattern);

    /*!
        Sets the expected password prompt pattern.

        The \a pattern is used to automatically recognize when the server
        asks for a password. If no password has been set, the loginRequired()
        signal will be emitted.

        \sa login()
    */
    void setPasswordPattern(const QRegExp &pattern);
    /*!
        \fn void Telnet::setPasswordString(const QString &pattern)

        Sets the expected password prompt to \a pattern.

        \overload
    */
    void setPasswordString(const QString &pattern);

    /**
     * @brief Determina si el socket está actualmente conectado.
     *  El socket puede estar en proceso de conexión si `!isConnected() && !isUnconnected()`.
     */
    bool isConnected() const;

    /**
     * @brief Determina si el socket está actualmente desconectado.
     *  El socket puede estar en proceso de conexión si `!isConnected() && !isUnconnected()`.
     */
    bool isUnconnected() const;

    /**
     * @brief Historial de cadenas recibidas. La primera línea de cada QString
     *  suele ser el eco de comandos introducidos.
     * @return
     */
    QStringList receivedResponses() const;

    /**
     * @brief Limpia el historial de respuestas recibidas.
     */
    void clearResponses();

    bool isLogged() const;
private:
    TelnetPrivate *d;
};
}
#endif
