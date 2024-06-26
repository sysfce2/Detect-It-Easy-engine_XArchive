/* Copyright (c) 2017-2024 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef XRAR_H
#define XRAR_H

#include "xarchive.h"

class XRar : public XArchive {
    Q_OBJECT

    struct GENERICHEADER5 {
        qint64 nSize;
        quint32 nCRC32;
        quint32 nHeaderSize;
        quint32 nType;
        quint32 nFlags;
        quint32 nExtraAreaSize;
        quint64 nDataSize;
    };

    enum HEADERTYPE {
        HEADERTYPE_UNKNOWN = 0,
        HEADERTYPE_MAIN = 1,
        HEADERTYPE_FILE = 2,
        HEADERTYPE_SERVICE = 3,
        HEADERTYPE_ENCRYPTION = 4,
        HEADERTYPE_ENDARC = 5,
    };

public:
    explicit XRar(QIODevice *pDevice = nullptr);

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr);
    static bool isValid(QIODevice *pDevice);
    virtual QString getVersion();
    virtual quint64 getNumberOfRecords(PDSTRUCT *pPdStruct);
    virtual QList<RECORD> getRecords(qint32 nLimit, PDSTRUCT *pPdStruct);

    virtual QString getFileFormatExt();
    virtual qint64 getFileFormatSize(PDSTRUCT *pPdStruct);
    virtual QString getFileFormatString();

    static QList<MAPMODE> getMapModesList();
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr);
    virtual FT getFileType();

private:
    GENERICHEADER5 readGenericHeader5(qint64 nOffset);
};

#endif  // XRAR_H
