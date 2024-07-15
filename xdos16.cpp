/* Copyright (c) 2022-2024 hors<horsicq@gmail.com>
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
#include "xdos16.h"

XDOS16::XDOS16(QIODevice *pDevice) : XArchive(pDevice)
{
}

bool XDOS16::isValid(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    bool bResult = false;

    if (getSize() > 1024) {
        if (read_uint16(0) == XMSDOS_DEF::S_IMAGE_DOS_SIGNATURE_MZ) {
            bResult = (getFileType() != FT_UNKNOWN);
        }
    }

    return bResult;
}

bool XDOS16::isValid(QIODevice *pDevice)
{
    XDOS16 xdos16(pDevice);

    return xdos16.isValid();
}

quint64 XDOS16::getNumberOfRecords(PDSTRUCT *pPdStruct)
{
    quint64 nResult=0;

    quint16 nCP = read_uint16(offsetof(XMSDOS_DEF::IMAGE_DOS_HEADER, e_cp));
    quint16 nCblp = read_uint16(offsetof(XMSDOS_DEF::IMAGE_DOS_HEADER, e_cblp));
    qint64 nSize = getSize();

    if (nCP > 0) {
        nResult = 1;
        qint64 nSignatureOffset = (nCP - 1) * 512 + nCblp;
        if (nSize - nSignatureOffset) {
            while (true) {
                quint16 nSignature = read_uint16(nSignatureOffset);

                if (nSignature == 0x5742) {     // BW
                    nSignatureOffset = read_uint32(nSignatureOffset + offsetof(XMSDOS_DEF::dos16m_exe_header, next_header_pos));
                    nResult++;
                } else if (nSignature == 0x5A4D) { // MZ
                    nResult++;
                    break;
                } else {
                    break;
                }
            }
        }
    }

    return nResult;
}

QList<XArchive::RECORD> XDOS16::getRecords(qint32 nLimit, PDSTRUCT *pPdStruct)
{
    QList<XArchive::RECORD> listResult;

    quint16 nCP = read_uint16(offsetof(XMSDOS_DEF::IMAGE_DOS_HEADER, e_cp));
    quint16 nCblp = read_uint16(offsetof(XMSDOS_DEF::IMAGE_DOS_HEADER, e_cblp));
    qint64 nSize = getSize();

    if (nCP > 0) {
        qint64 nSignatureOffset = (nCP - 1) * 512 + nCblp;
        {
            RECORD record = {};

            record.sFileName = tr("Loader");
            record.nDataOffset = 0;
            record.nCompressedSize = nSignatureOffset;
            record.nUncompressedSize = nSignatureOffset;
            record.compressMethod = COMPRESS_METHOD_STORE;

            listResult.append(record);
        }

        if (nSize - nSignatureOffset) {
            while (true) {
                quint16 nSignature = read_uint16(nSignatureOffset);

                if (nSignature == 0x5742) {     // BW
                    qint64 nNewSignatureOffset = read_uint32(nSignatureOffset + offsetof(XMSDOS_DEF::dos16m_exe_header, next_header_pos));
                    QString sName = read_ansiString(nSignatureOffset + offsetof(XMSDOS_DEF::dos16m_exe_header, EXP_path));

                    nNewSignatureOffset = qMin(nNewSignatureOffset, nSize);

                    RECORD record = {};

                    record.sFileName = sName;
                    record.nDataOffset = nSignatureOffset;
                    record.nCompressedSize = nNewSignatureOffset - nSignatureOffset;
                    record.nUncompressedSize = nNewSignatureOffset - nSignatureOffset;
                    record.compressMethod = COMPRESS_METHOD_STORE;

                    listResult.append(record);

                    nSignatureOffset = nNewSignatureOffset;
                } else if (nSignature == 0x5A4D) { // MZ
                    RECORD record = {};

                    record.sFileName = tr("Data");
                    record.nDataOffset = nSignatureOffset;
                    record.nCompressedSize = nSize - nSignatureOffset;
                    record.nUncompressedSize = nSize - nSignatureOffset;
                    record.compressMethod = COMPRESS_METHOD_STORE;

                    listResult.append(record);

                    break;
                } else {
                    break;
                }
            }
        }
    }

    return listResult;
}

XBinary::FT XDOS16::getFileType()
{
    XBinary::FT result = FT_UNKNOWN;

    quint16 nCP = read_uint16(offsetof(XMSDOS_DEF::IMAGE_DOS_HEADER, e_cp));
    quint16 nCblp = read_uint16(offsetof(XMSDOS_DEF::IMAGE_DOS_HEADER, e_cblp));
    qint64 nSize = getSize();

    if (nCP > 0) {
        qint64 nSignatureOffset = (nCP - 1) * 512 + nCblp;
        if (nSize - nSignatureOffset) {
            while (true) {
                quint16 nSignature = read_uint16(nSignatureOffset);

                if (nSignature == 0x5742) {     // BW
                    result = FT_DOS16M;
                    nSignatureOffset = read_uint32(nSignatureOffset + offsetof(XMSDOS_DEF::dos16m_exe_header, next_header_pos));
                } else if (nSignature == 0x5A4D) { // MZ
                    qint64 nSignatureOffsetOpt = read_uint32(nSignatureOffset + offsetof(XMSDOS_DEF::IMAGE_DOS_HEADEREX, e_lfanew));
                    quint16 nSignatureOpt = read_uint16(nSignatureOffsetOpt + nSignatureOffset);

                    if (nSignatureOpt == 0x454E) { // NE
                        result = FT_DOS16M;
                    } else if (nSignatureOpt == 0x454C) { // LE
                        result = FT_DOS4G;
                    } else if (nSignatureOpt == 0x584C) { // LX
                        result = FT_DOS4G;
                    }
                    break;
                } else {
                    break;
                }
            }
        }
    }

    return result;
}

XBinary::_MEMORY_MAP XDOS16::getMemoryMap(XBinary::MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)
    Q_UNUSED(pPdStruct)

    _MEMORY_MAP result = {};

    qint32 nIndex = 0;

    result.fileType = getFileType();

    result.sArch = QString("386");;
    result.sType = typeIdToString(getType());;
    result.mode = getMode();
    result.nBinarySize = getSize();
    result.nImageSize = getImageSize();
    result.nModuleAddress = getModuleAddress();
    result.endian = getEndian();

    quint16 nCP = read_uint16(offsetof(XMSDOS_DEF::IMAGE_DOS_HEADER, e_cp));
    quint16 nCblp = read_uint16(offsetof(XMSDOS_DEF::IMAGE_DOS_HEADER, e_cblp));
    qint64 nSize = getSize();

    if (nCP > 0) {
        qint64 nSignatureOffset = (nCP - 1) * 512 + nCblp;

        {
            _MEMORY_RECORD record = {};
            record.nSize = nSignatureOffset;
            record.nOffset = 0;
            record.nAddress = -1;
            record.segment = ADDRESS_SEGMENT_UNKNOWN;
            record.type = MMT_FILESEGMENT;
            record.sName = tr("Loader");
            record.nIndex = nIndex++;

            result.listRecords.append(record);
        }

        if (nSize - nSignatureOffset) {
            while (true) {
                quint16 nSignature = read_uint16(nSignatureOffset);

                if (nSignature == 0x5742) {     // BW
                    qint64 nNewSignatureOffset = read_uint32(nSignatureOffset + offsetof(XMSDOS_DEF::dos16m_exe_header, next_header_pos));
                    QString sName = read_ansiString(nSignatureOffset + offsetof(XMSDOS_DEF::dos16m_exe_header, EXP_path));

                    nNewSignatureOffset = qMin(nNewSignatureOffset, nSize);

                    _MEMORY_RECORD record = {};
                    record.nSize = nNewSignatureOffset - nSignatureOffset;
                    record.nOffset = nSignatureOffset;
                    record.nAddress = -1;
                    record.segment = ADDRESS_SEGMENT_UNKNOWN;
                    record.type = MMT_FILESEGMENT;
                    record.sName = sName;
                    record.nIndex = nIndex++;

                    result.listRecords.append(record);

                    nSignatureOffset = nNewSignatureOffset;
                } else if (nSignature == 0x5A4D) { // MZ
                    _MEMORY_RECORD record = {};
                    record.nSize = nSize - nSignatureOffset;
                    record.nOffset = nSignatureOffset;
                    record.nAddress = -1;
                    record.segment = ADDRESS_SEGMENT_UNKNOWN;
                    record.type = MMT_FILESEGMENT;
                    record.sName = tr("Data");
                    record.nIndex = nIndex++;

                    result.listRecords.append(record);

                    break;
                } else {
                    break;
                }
            }
        }
    }

    return result;
}

XBinary::OSINFO XDOS16::getOsInfo()
{
    OSINFO result = {};

    result.osName = OSNAME_MSDOS;
    result.sOsVersion = "3.0";
    result.sArch = getArch();
    result.mode = getMode();
    result.sType = typeIdToString(getType());
    result.endian = getEndian();

    return result;
}

XBinary::MODE XDOS16::getMode()
{
    return MODE_16;
}

QString XDOS16::getArch()
{
    return "386";
}

XBinary::ENDIAN XDOS16::getEndian()
{
    return ENDIAN_LITTLE;
}

qint32 XDOS16::getType()
{
    return TYPE_DOSEXTENDER;
}

QString XDOS16::getFileFormatString()
{
    QString sResult;

    FT fileType = getFileType();

    if (fileType == FT_DOS16M) {
        sResult = "DOS16M";
    } else if (fileType == FT_DOS4G) {
        sResult = "DOS4G";
    }

    return sResult;
}

QString XDOS16::getFileFormatExt()
{
    return "exe";
}
