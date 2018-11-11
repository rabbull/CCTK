cmake_minimum_required(VERSION 3.12)
project(cctk)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CBLAS REQUIRED openblas)
pkg_check_modules(MAGICK REQUIRED "Magick++")

SET(TEMP_PKG_NAME CBLAS)
if (${${TEMP_PKG_NAME}_FOUND})
    message(STATUS "${TEMP_PKG_NAME} found")
    message(STATUS "   include dirctories: " ${${TEMP_PKG_NAME}_INCLUDE_DIRS})
    message(STATUS "   libraries include directores: " ${${TEMP_PKG_NAME}_LIBRARY_DIRS})
    message(STATUS "   libraries: " ${${TEMP_PKG_NAME}_LIBRARIES})
    message(STATUS "   cflags: " ${${TEMP_PKG_NAME}_CFLAGS})
endif()

SET(TEMP_PKG_NAME MAGICK)
if (${${TEMP_PKG_NAME}_FOUND})
    message(STATUS "${TEMP_PKG_NAME} found")
    message(STATUS "   include dirctories: ${${TEMP_PKG_NAME}_INCLUDE_DIRS}")
    message(STATUS "   libraries include directores: ${${TEMP_PKG_NAME}_LIBRARY_DIRS}")
    message(STATUS "   libraries: ${${TEMP_PKG_NAME}_LIBRARIES}")
    message(STATUS "   cflags: ${${TEMP_PKG_NAME}_CFLAGS}")
endif()

SET(${CMAKE_C_FLAGS} "${CMAKE_C_FLAGS} ${CBLAS_CFLAGS} -I/usr/include/ImageMagick-7 -fopenmp -DMAGICKCORE_HDRI_ENABLE=1 -DMAGICKCORE_QUANTUM_DEPTH=16")
SET(${CMAKE_CXX_FLAGS} "${CMAKE_CXX_FLAGS} -I/usr/include/ImageMagick-7 -fopenmp -DMAGICKCORE_HDRI_ENABLE=1 -DMAGICKCORE_QUANTUM_DEPTH=16")

include_directories(${CBLAS_INCLUDE_DIRS})
include_directories(${MAGICK_INCLUDE_DIRS})

find_library(CBLAS_LIBRARIES_ABS ${CBLAS_LIBRARIES} ${CBLAS_LIBRARY_DIRS})
find_library(MAGICK_LIBRARIES_ABS ${MAGICK_LIBRARIES} ${MAGICK_LIBRARY_DIRS})

set(CMAKE_CXX_STANDARD 17)
add_executable(cctk main.cpp blas_utils.cpp blas_utils.h traditional_fuctions.cpp traditional_fuctions.h types.h const.h const.cpp DOCC.cpp DOCC.h)

target_link_libraries(cctk ${CBLAS_LIBRARIES_ABS} ${MAGICK_LIBRARIES})