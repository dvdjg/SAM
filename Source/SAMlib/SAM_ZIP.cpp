#include <QDataStream>
#include <cstring>

#include "zlib.h"
#include "unzip.h"

#include "SAM_ZIP.h"

namespace SAM
{

    UnzipFileInfo::UnzipFileInfo()
    {
        std::memset(this, 0, sizeof(UnzipFileInfo));
    }

    int unzipFirstFile(const char * szFileName,
                       QDataStream & decompressedFile,
                       UnzipFileInfo * pFileInfo)
    {
        int nTotalRead = 0;

        unzFile zipFile = unzOpen(szFileName);
        if(!zipFile)
            return nTotalRead;

        int zFile = unzOpenCurrentFile(zipFile);
        if(zFile == UNZ_OK)
        {
            UnzipFileInfo fileInfo;
            ::unz_file_info zFileInfo;

            unzGoToFirstFile(zipFile);
            if (unzGetCurrentFileInfo(zipFile, &zFileInfo,
                                      fileInfo.szFileName, sizeof(fileInfo.szFileName)-1,
                                      fileInfo.szExtraField, sizeof(fileInfo.szExtraField)-1,
                                      fileInfo.szComment, sizeof(fileInfo.szComment)-1) == UNZ_OK)
            {
                std::memcpy(&fileInfo.zFileInfo, &zFileInfo, sizeof(::unz_file_info));
                fileInfo.szFileName[sizeof(fileInfo.szFileName)-1] = 0;
                fileInfo.szExtraField[sizeof(fileInfo.szExtraField)-1] = 0;
                fileInfo.szComment[sizeof(fileInfo.szComment)-1] = 0;

                if(pFileInfo)
                    *pFileInfo = fileInfo;
            }

            char buff[512];
            int nRead = 0;
            while((nRead = unzReadCurrentFile(zipFile, buff, sizeof(buff))) > 0)
            {
                decompressedFile.writeRawData(buff, nRead);
                nTotalRead += nRead;
            }

            unzCloseCurrentFile(zipFile);
        }

        unzClose(zipFile);

        return nTotalRead;
    }

}

