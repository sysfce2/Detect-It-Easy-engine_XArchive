/* Copyright (c) 2023 hors<horsicq@gmail.com>
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
#ifndef XZLIB_H
#define XZLIB_H

#include "xarchive.h"

class XZlib : public XArchive {
    Q_OBJECT
public:
    explicit XZlib(QIODevice *pDevice = nullptr);

    virtual bool isValid();
    static bool isValid(QIODevice *pDevice);
    virtual quint64 getNumberOfRecords(PDSTRUCT *pPdStruct);
    virtual QList<RECORD> getRecords(qint32 nLimit, PDSTRUCT *pPdStruct);
    virtual qint64 getFileFormatSize();
    virtual _MEMORY_MAP getMemoryMap(PDSTRUCT *pPdStruct = nullptr);
    virtual FT getFileType();
    virtual QString getFileFormatString();
    virtual QString getFileFormatExt();
};

#endif  // XZLIB_H
