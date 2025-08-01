/* Copyright (c) 2024-2025 hors<horsicq@gmail.com>
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
#ifndef XCOMPRESSEDDEVICE_H
#define XCOMPRESSEDDEVICE_H

#include "xiodevice.h"
#include "subdevice.h"
#include "xgzip.h"
#include "xdecompress.h"

class XCompressedDevice : public XIODevice {
    Q_OBJECT

public:
    explicit XCompressedDevice(QObject *pParent = nullptr);
    ~XCompressedDevice();

    bool setData(QIODevice *pDevice, const XBinary::FPART &fPart, XBinary::PDSTRUCT *pPdStruct);
    virtual bool open(OpenMode mode);

    QIODevice *getOrigDevice();

    virtual qint64 size() const;
    virtual bool seek(qint64 nPos);
    virtual qint64 pos() const;

protected:
    virtual qint64 readData(char *pData, qint64 nMaxSize);
    virtual qint64 writeData(const char *pData, qint64 nMaxSize);

private:
    QIODevice *g_pOrigDevice;
    SubDevice *g_pSubDevice;
    QBuffer *g_pBufferDevice;
    QTemporaryFile *g_pTempFile;
    QIODevice *g_pCurrentDevice;
    bool g_bIsValid;
    qint32 g_nBufferSize;
    char *g_pBuffer;
};

#endif  // XCOMPRESSEDDEVICE_H
