/* Copyright (c) 2023-2025 hors<horsicq@gmail.com>
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
#ifndef XDECOMPRESS_H
#define XDECOMPRESS_H

#include "xformats.h"
#include "xthreadobject.h"

class XDecompress : public XThreadObject
{
    Q_OBJECT

    struct STATE {
        QIODevice *pDeviceInput;
        QIODevice *pDeviceOutput;
        qint64 nInputOffset;
        qint64 nInputLimit;
        qint64 nDecompressedOffset;
        qint64 nDecompressedLimit;
        XBinary::COMPRESS_METHOD compressMethod;
        bool bReadError;
        bool bWriteError;
        qint64 nCountInput;
        qint64 nCountOutput;
    };

    qint32 _readDevice(char *pBuffer, qint32 nBufferSize, STATE *pState);
    qint32 _writeDevice(char *pBuffer, qint32 nBufferSize, STATE *pState);

public:
    enum MODE {
        MODE_UNKNOWN = 0,
        MODE_UNPACKDEVICETOFOLDER,
    };

    explicit XDecompress(QObject *parent = nullptr);
    bool decompressFPART(const XBinary::FPART &fpart, QIODevice *pDeviceInput, QIODevice *pDeviceOutput, qint64 nDecompressedOffset,
                    qint64 nDecompressedLimit, XBinary::PDSTRUCT *pPdStruct);
    bool decompress(STATE *pState, XBinary::PDSTRUCT *pPdStruct);
    bool unpackDeviceToFolder(XBinary::FT fileFormat, QIODevice *pDevice, QString sFolderName, XBinary::PDSTRUCT *pPdStruct);

    void setData(MODE mode, XBinary::FT fileFormat, QIODevice *pDevice, QString sFolderName, XBinary::PDSTRUCT *pPdStruct);

    virtual void process();

private:
    MODE g_mode;
    XBinary::FT g_fileFormat;
    QIODevice *g_pDevice;
    QString g_sFolderName;
    XBinary::PDSTRUCT *g_pPdStruct;
};

#endif // XDECOMPRESS_H
