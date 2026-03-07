# Install script for directory: /home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/LASlib" TYPE FILE FILES
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lascopc.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasdefinitions.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasfilter.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasignore.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/laskdtree.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasprogress.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasreader.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasreader_asc.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasreader_bil.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasreader_bin.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasreader_dtm.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasreader_las.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasreader_ply.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasreader_qfit.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasreader_shp.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasreader_txt.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasreaderbuffered.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasreadermerged.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasreaderpipeon.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasreaderstored.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lastransform.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasutility.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasvlr.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/lasvlrpayload.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/laswaveform13reader.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/laswaveform13writer.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/laswriter.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/laswriter_bin.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/laswriter_las.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/laswriter_qfit.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/laswriter_txt.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/laswriter_wrl.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/inc/laswritercompatible.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/include/laszip/laszip_api_version.h"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/include/laszip/laszip_common.h"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/arithmeticdecoder.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/arithmeticencoder.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/arithmeticmodel.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/bytestreamin.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/bytestreamin_array.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/bytestreamin_file.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/bytestreamin_istream.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/bytestreaminout.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/bytestreaminout_file.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/bytestreamout.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/bytestreamout_array.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/bytestreamout_file.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/bytestreamout_nil.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/bytestreamout_ostream.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/endian.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/integercompressor.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/lasattributer.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/lasindex.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/lasinterval.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/lasmessage.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/laspoint.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/lasquadtree.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/lasquantizer.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/lasreaditem.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/lasreaditemcompressed_v1.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/lasreaditemcompressed_v2.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/lasreaditemcompressed_v3.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/lasreaditemcompressed_v4.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/lasreaditemraw.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/lasreadpoint.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/lasunzipper.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/laswriteitem.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/laswriteitemcompressed_v1.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/laswriteitemcompressed_v2.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/laswriteitemcompressed_v3.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/laswriteitemcompressed_v4.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/laswriteitemraw.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/laswritepoint.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/laszip.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/laszip_common_v1.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/laszip_common_v2.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/laszip_common_v3.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/laszip_decompress_selective_v3.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/laszipper.hpp"
    "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASzip/src/mydefs.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/LASlib" TYPE STATIC_LIBRARY FILES "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/lib/libLASlib.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/LASlib/laslib-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/LASlib/laslib-targets.cmake"
         "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/src/CMakeFiles/Export/c55326e5cb745217e14af8383b523273/laslib-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/LASlib/laslib-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/LASlib/laslib-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/LASlib" TYPE FILE FILES "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/src/CMakeFiles/Export/c55326e5cb745217e14af8383b523273/laslib-targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/LASlib" TYPE FILE FILES "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/src/CMakeFiles/Export/c55326e5cb745217e14af8383b523273/laslib-targets-noconfig.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/LASlib" TYPE FILE FILES "/home/abel/Universidad/4/TFG/Code/src/LAStools/LASlib/src/laslib-config.cmake")
endif()

