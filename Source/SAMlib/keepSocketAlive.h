#ifndef KEEPSOCKETALIVE_H
#define KEEPSOCKETALIVE_H

/**
 * @brief Mantiene el socket vivo.
 *  Configura el socket para que envíe una serie de mensajes de control que comprueban
 *  que la conexión sigue existiendo.
 *  En caso de que se rompa el socket generará la señal de que se ha desconectado:
 *  QAbstractSocket::disconnected().
 *
 * @param fd Descriptor del socket tal como lo devuelve la función QAbstractSocket::socketDescriptor().
 * @param enableKeepAlive Configura el que se mantenga vivo o no el socket.
 * @param maxIdle Tiempo máximo en segundos que transcurrirá desde la última actividad
 *  detectada hasta el inicio de la petición de mensajes de ACK de control.
 * @param interval Tiempo en segundos que transcurrirá entre peticiones de ACKs.
 * @param count Cuenta el número de ACKs que se enviarán hasta que se dé por caída la conexión.
 *  Éste parámetro no se usa en Windows que a partir de Vista usa un valor por defecto de 10
 *  y no se puede cambiar.
 * @return bool Devuelve true si se ha podido configurar la detección de desconexión.
 *
 * @note On Windows Vista and later, this socket option can only be set
 *  when the socket is in a well-known state not a transitional state.
 */
bool keepSocketAlive(int fd, int enableKeepAlive = 1, int maxIdle = 5, int interval = 1, int count = 10);

#endif // KEEPSOCKETALIVE_H
