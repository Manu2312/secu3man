# SECU-3  - An open source, free engine control unit
# Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev
# http://secu-3.org

cmake_minimum_required(VERSION 2.6)

ADD_DEFINITIONS(-D_AFXDLL)
ADD_DEFINITIONS(-D_AFXEXT)
ADD_DEFINITIONS(-D_WINDLL)

set(CMAKE_MFC_FLAG 2)
set(PROJECT fwimpexp)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ../../libraries)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ../../libraries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ../../libraries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ../../libraries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ../../binaries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ../../binaries)

include_directories(../)
project(${PROJECT})

set(HEADERS
  EEPROMImpExpController.h
  MapImpExpDlg.h
  MPSZFileDataIO.h
  MPSZImpExpController.h
  Resource.h
  S3FImpExpController.h
  S3FFileDataIO.h
  SECUImpExpController.h
  StdAfx.h
  )

set(SOURCES
  EEPROMImpExpController.cpp
  FwImpExp.cpp
  MapImpExpDlg.cpp
  MPSZFileDataIO.cpp
  MPSZImpExpController.cpp
  S3FImpExpController.cpp
  S3FFileDataIO.cpp
  SECUImpExpController.cpp
  StdAfx.cpp
  FwImpExp.rc
  )

add_library (${PROJECT} SHARED ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} io-core ui-core)
