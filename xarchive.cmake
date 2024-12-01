include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${CMAKE_CURRENT_LIST_DIR}/3rdparty/bzip2/src/)
include_directories(${CMAKE_CURRENT_LIST_DIR}/3rdparty/lzma/src/)
include_directories(${CMAKE_CURRENT_LIST_DIR}/3rdparty/zlib/src/)

if (NOT DEFINED XBINARY_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../Formats/xbinary.cmake)
    set(XARCHIVE_SOURCES ${XARCHIVE_SOURCES} ${XBINARY_SOURCES})
endif()

#include(${CMAKE_CURRENT_LIST_DIR}/../Formats/exec/xmach.cmake)

set(XARCHIVE_SOURCES
    ${XARCHIVE_SOURCES}
    ${XBINARY_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/x_ar.cpp
    ${CMAKE_CURRENT_LIST_DIR}/x_ar.h
    ${CMAKE_CURRENT_LIST_DIR}/xapk.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xapk.h
    ${CMAKE_CURRENT_LIST_DIR}/xapks.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xapks.h
    ${CMAKE_CURRENT_LIST_DIR}/xarchive.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xarchive.h
    ${CMAKE_CURRENT_LIST_DIR}/xcab.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xcab.h
    ${CMAKE_CURRENT_LIST_DIR}/xcompress.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xcompress.h
    ${CMAKE_CURRENT_LIST_DIR}/xcompresseddevice.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xcompresseddevice.h
    ${CMAKE_CURRENT_LIST_DIR}/xdeb.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xdeb.h
    ${CMAKE_CURRENT_LIST_DIR}/xgzip.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xgzip.h
    ${CMAKE_CURRENT_LIST_DIR}/xipa.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xipa.h
    ${CMAKE_CURRENT_LIST_DIR}/xjar.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xjar.h
    ${CMAKE_CURRENT_LIST_DIR}/xlha.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xlha.h
    ${CMAKE_CURRENT_LIST_DIR}/xmachofat.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xmachofat.h
    ${CMAKE_CURRENT_LIST_DIR}/xrar.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xrar.h
    ${CMAKE_CURRENT_LIST_DIR}/xsevenzip.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xsevenzip.h
    ${CMAKE_CURRENT_LIST_DIR}/xtar.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xtar.h
    ${CMAKE_CURRENT_LIST_DIR}/xtgz.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xtgz.h
    ${CMAKE_CURRENT_LIST_DIR}/xzip.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xzip.h
    ${CMAKE_CURRENT_LIST_DIR}/xzlib.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xzlib.h
    ${CMAKE_CURRENT_LIST_DIR}/xnpm.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xnpm.h
    ${CMAKE_CURRENT_LIST_DIR}/xdos16.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xdos16.h
)
