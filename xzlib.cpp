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
#include "xzlib.h"

XZlib::XZlib(QIODevice *pDevice) : XArchive(pDevice)
{
}

bool XZlib::isValid(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    bool bResult = false;

    if (getSize() >= 6) {
        quint16 nHeader = read_uint16(0);
        // 0x0178 no compression
        if (nHeader == 0x5E78) {
            bResult = true;
        } else if (nHeader == 0x9C78) {
            bResult = true;
        } else if (nHeader == 0xDA78) {
            bResult = true;
        }

        if (bResult) {
            bResult = false;

            if (getSize() >= 10) {
                SubDevice sd(getDevice(), 2, -1);

                if (sd.open(QIODevice::ReadOnly)) {
                    XArchive::DECOMPRESSSTRUCT decompressStruct = {};
                    decompressStruct.spInfo.compressMethod = COMPRESS_METHOD_DEFLATE;
                    decompressStruct.pSourceDevice = &sd;
                    decompressStruct.pDestDevice = 0;
                    decompressStruct.nDecompressedOffset = 0;
                    decompressStruct.nDecompressedLimit = 0x200;

                    COMPRESS_RESULT cr = _decompress(&decompressStruct, pPdStruct);

                    if (decompressStruct.nInSize > 0) {
                        if (!decompressStruct.bLimit) {
                            QBuffer buffer;
                            if (buffer.open(QIODevice::ReadWrite)) {
                                sd.reset();
                                XArchive::DECOMPRESSSTRUCT _decompressStruct = {};
                                _decompressStruct.spInfo.compressMethod = COMPRESS_METHOD_DEFLATE;
                                _decompressStruct.pSourceDevice = &sd;
                                _decompressStruct.pDestDevice = &buffer;

                                cr = _decompress(&_decompressStruct, pPdStruct);

                                if (cr == COMPRESS_RESULT_OK) {
                                    quint32 nAdler = read_uint32(2 + _decompressStruct.nInSize, true);

                                    bResult = (nAdler == XBinary::getAdler32(&buffer, pPdStruct));
                                }

                                buffer.close();
                            }
                        } else {
                            bResult = true;
                        }
                    }
                }
            }
        }
    }

    return bResult;
}

bool XZlib::isValid(QIODevice *pDevice)
{
    XZlib xzlib(pDevice);

    return xzlib.isValid();
}

quint64 XZlib::getNumberOfRecords(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    return 1;  // Always 1
}

QList<XArchive::RECORD> XZlib::getRecords(qint32 nLimit, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nLimit)  // Always 1

    QList<RECORD> listResult;

    RECORD record = {};

    qint64 nOffset = 0;

    record.spInfo.compressMethod = COMPRESS_METHOD_DEFLATE;

    nOffset += 2;

    SubDevice sd(getDevice(), nOffset, -1);

    if (sd.open(QIODevice::ReadOnly)) {
        DECOMPRESSSTRUCT decompressStruct = {};
        decompressStruct.spInfo.compressMethod = record.spInfo.compressMethod;
        decompressStruct.pSourceDevice = &sd;
        decompressStruct.pDestDevice = 0;

        COMPRESS_RESULT cr = _decompress(&decompressStruct, pPdStruct);

        Q_UNUSED(cr)

        record.nHeaderOffset = 0;
        record.nHeaderSize = nOffset;
        record.nDataOffset = nOffset;
        record.nDataSize = decompressStruct.nInSize;
        record.spInfo.nUncompressedSize = decompressStruct.nOutSize;
        record.spInfo.sRecordName = XBinary::getDeviceFileBaseName(getDevice());

        sd.close();
    }

    // TODO

    listResult.append(record);

    return listResult;
}

qint64 XZlib::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    return _calculateRawSize(pPdStruct);
}

QList<XBinary::MAPMODE> XZlib::getMapModesList()
{
    QList<MAPMODE> listResult;

    listResult.append(MAPMODE_REGIONS);

    return listResult;
}

XBinary::_MEMORY_MAP XZlib::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)

    XBinary::PDSTRUCT pdStructEmpty = {};

    if (!pPdStruct) {
        pdStructEmpty = XBinary::createPdStruct();
        pPdStruct = &pdStructEmpty;
    }

    _MEMORY_MAP result = {};
    result.fileType = getFileType();
    result.mode = getMode();
    result.endian = getEndian();
    result.sType = typeIdToString(getType());
    result.sArch = getArch();
    result.nBinarySize = getSize();

    qint32 nIndex = 0;

    _MEMORY_RECORD memoryRecordHeader = {};
    _MEMORY_RECORD memoryRecord = {};
    _MEMORY_RECORD memoryRecordFooter = {};

    qint64 nOffset = 0;

    COMPRESS_METHOD cm = COMPRESS_METHOD_DEFLATE;

    nOffset += 2;  // TODO consts

    memoryRecordHeader.nOffset = 0;
    memoryRecordHeader.nAddress = -1;
    memoryRecordHeader.nSize = nOffset;
    memoryRecordHeader.sName = tr("Header");
    memoryRecordHeader.filePart = FILEPART_HEADER;
    memoryRecordHeader.nIndex = nIndex++;

    result.listRecords.append(memoryRecordHeader);

    SubDevice sd(getDevice(), nOffset, -1);

    if (sd.open(QIODevice::ReadOnly)) {
        DECOMPRESSSTRUCT decompressStruct = {};
        decompressStruct.spInfo.compressMethod = cm;
        decompressStruct.pSourceDevice = &sd;
        decompressStruct.pDestDevice = 0;

        COMPRESS_RESULT cr = _decompress(&decompressStruct, pPdStruct);

        Q_UNUSED(cr)

        memoryRecord.nOffset = nOffset;
        memoryRecord.nAddress = -1;
        memoryRecord.nSize = decompressStruct.nInSize;
        memoryRecord.filePart = FILEPART_REGION;
        memoryRecord.sName = tr("Data");
        memoryRecord.nIndex = nIndex++;

        sd.close();
    }

    // TODO

    result.listRecords.append(memoryRecord);

    memoryRecordFooter.nOffset = memoryRecord.nOffset + memoryRecord.nSize;
    memoryRecordFooter.nAddress = -1;
    memoryRecordFooter.nSize = 4;
    memoryRecordFooter.sName = tr("Footer");
    memoryRecordFooter.filePart = FILEPART_FOOTER;
    memoryRecordFooter.nIndex = nIndex++;

    result.listRecords.append(memoryRecordFooter);

    _handleOverlay(&result);

    return result;
}

XBinary::FT XZlib::getFileType()
{
    return FT_ZLIB;
}

QString XZlib::getVersion()
{
    QString sResult;

    quint16 nHeader = read_uint16(0);
    // 0x0178 no compression
    if (nHeader == 0x5E78) {
        sResult = "fast";
    } else if (nHeader == 0x9C78) {
        sResult = "default";
    } else if (nHeader == 0xDA78) {
        sResult = "best";
    }

    return sResult;
}

QString XZlib::getFileFormatExt()
{
    return "zlib";
}

QString XZlib::getFileFormatExtsString()
{
    return "Zlib (*.zlib)";
}

QString XZlib::getMIMEString()
{
    return "application/x-zlib";
}
