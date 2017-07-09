#ifndef BASENAME_H
#define BASENAME_H

namespace SAM
{
    /**
     * @brief Implementación GNU de la orden `basename()` de POSIX.
     *  Se incluye aquí ya que Windows no la incorpora.
     *
     * @param name Ruta que puede incluir nombres de directorios.
     * @return const char * Nombre base de `name`.
     */
    const char * basename(const char *name);
}

#endif // BASENAME_H
