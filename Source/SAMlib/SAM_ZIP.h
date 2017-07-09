#ifndef SAM_ZIP_H
#define SAM_ZIP_H

//#include "zlib.h"
//#include "unzip.h"

class QDataStream;

namespace SAM
{
    typedef unsigned int   uInt;  /* 16 bits or more */
    typedef unsigned long  uLong; /* 32 bits or more */


    /** tm_unz contain date/time info */
    struct tm_unz
    {
        uInt tm_sec;            /**< seconds after the minute - [0,59] */
        uInt tm_min;            /**< minutes after the hour - [0,59] */
        uInt tm_hour;           /**< hours since midnight - [0,23] */
        uInt tm_mday;           /**< day of the month - [1,31] */
        uInt tm_mon;            /**< months since January - [0,11] */
        uInt tm_year;           /**< years - [1980..2044] */
    };

    /** unz_file_info contain information about a file in the zipfile */
    struct unz_file_info
    {
        uLong version;              /**< version made by                 2 bytes */
        uLong version_needed;       /**< version needed to extract       2 bytes */
        uLong flag;                 /**< general purpose bit flag        2 bytes */
        uLong compression_method;   /**< compression method              2 bytes */
        uLong dosDate;              /**< last mod file date in Dos fmt   4 bytes */
        uLong crc;                  /**< crc-32                          4 bytes */
        uLong compressed_size;      /**< compressed size                 4 bytes */
        uLong uncompressed_size;    /**< uncompressed size               4 bytes */
        uLong size_filename;        /**< filename length                 2 bytes */
        uLong size_file_extra;      /**< extra field length              2 bytes */
        uLong size_file_comment;    /**< file comment length             2 bytes */

        uLong disk_num_start;       /**< disk number start               2 bytes */
        uLong internal_fa;          /**< internal file attributes        2 bytes */
        uLong external_fa;          /**< external file attributes        4 bytes */

        tm_unz tmu_date;
    };

    struct UnzipFileInfo : public unz_file_info
    {
        char szFileName[260];
        char szExtraField[256];
        char szComment[256];
        unz_file_info zFileInfo;

        UnzipFileInfo();
    };

    /**
     * @brief
     *
     * @param szFileName
     * @param decompressedFile
     * @param pFileInfo
     * @return int El número total de bytes leidos
     */
    int unzipFirstFile(
            const char * szFileName,
            QDataStream & decompressedFile,
            UnzipFileInfo * pFileInfo = NULL);
}

#endif // SAM_ZIP_H
