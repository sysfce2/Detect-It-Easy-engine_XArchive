include_directories(${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED XARCHIVE_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/xarchive.cmake)
    set(XARCHIVES_SOURCES ${XARCHIVES_SOURCES} ${XARCHIVE_SOURCES})
endif()

set(XARCHIVES_SOURCES
    ${XARCHIVES_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/xarchives.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xarchives.h
)
