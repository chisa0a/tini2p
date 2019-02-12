# Copyright (c) 2013-2017, The Kovri I2P Router Project
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this list of
#    conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this list
#    of conditions and the following disclaimer in the documentation and/or other
#    materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors may be
#    used to endorse or promote products derived from this software without specific
#    prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
# THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
# THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

if(CryptoPP_INCLUDE_DIR AND CryptoPP_LIBRARIES)
   set(CryptoPP_FOUND TRUE)

else(CryptoPP_INCLUDE_DIR AND CryptoPP_LIBRARIES)

  find_path(CryptoPP_INCLUDE_DIR
    name cryptlib.h
    PATHS ${PROJECT_SOURCE_DIR}/deps/cryptopp /usr/include /usr/local/include
    NO_DEFAULT_PATH)

  # Commented to prevent confusion if submodule was not built correctly.
  # Uncomment when we stop using a submodule.
  #find_path(CryptoPP_INCLUDE_DIR cryptopp/cryptlib.h
  #/usr/include
  #/usr/local/include
  #$ENV{SystemDrive}/Crypto++/include
  #$ENV{CRYPTOPP}
  #$ENV{CRYPTOPP}/..
  #$ENV{CRYPTOPP}/include
  #${PROJECT_SOURCE_DIR}/../..)

  find_library(CryptoPP_LIBRARIES
    NAMES cryptopp
    PATHS ${PROJECT_SOURCE_DIR}/deps/cryptopp /usr/lib /usr/local/lib
    NO_DEFAULT_PATH)

  # Commented to prevent confusion if submodule was not built correctly.
  # Uncomment when we stop using a submodule.
  #find_library(CryptoPP_LIBRARIES NAMES cryptopp PATHS
  #/usr/lib
  #/usr/local/lib
  #/opt/local/lib
  #$ENV{SystemDrive}/Crypto++/lib
  #$ENV{CRYPTOPP}/lib)

  if(MSVC AND NOT CryptoPP_LIBRARIES) # Give a chance for MSVC multiconfig
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(PLATFORM x64)
    else()
      set(PLATFORM Win32)
    endif()
    find_library(CryptoPP_LIBRARIES_RELEASE NAMES cryptlib cryptopp
      HINTS ${PROJECT_SOURCE_DIR}/deps/cryptopp
      ENV CRYPTOPP
      PATH_SUFFIXES ${PLATFORM}/Output/Release)
    find_library(CryptoPP_LIBRARIES_DEBUG NAMES cryptlib cryptopp
      HINTS ${PROJECT_SOURCE_DIR}/deps/cryptopp
      ENV CRYPTOPP
      PATH_SUFFIXES ${PLATFORM}/Output/Debug)
    set(CryptoPP_LIBRARIES
      debug ${CryptoPP_LIBRARIES_DEBUG}
      optimized ${CryptoPP_LIBRARIES_RELEASE}
      CACHE PATH "Path to Crypto++ library" FORCE)
  endif()

  if(CryptoPP_INCLUDE_DIR AND CryptoPP_LIBRARIES)
    set(CryptoPP_FOUND TRUE)
  else(CryptoPP_INCLUDE_DIR AND CryptoPP_LIBRARIES)
    set(CryptoPP_FOUND FALSE)
  endif(CryptoPP_INCLUDE_DIR AND CryptoPP_LIBRARIES)

  mark_as_advanced(CryptoPP_INCLUDE_DIR CryptoPP_LIBRARIES)

endif(CryptoPP_INCLUDE_DIR AND CryptoPP_LIBRARIES)

if (CryptoPP_FOUND AND NOT TARGET CryptoPP::CryptoPP)

  set(library_type SHARED)
  if (MSVC)
    set(library_type STATIC)
  elseif(MINGW)
    set(library_type STATIC)
  else()
    if (WITH_STATIC_DEPS OR CMAKE_BUILD_TYPE STREQUAL Release)
      set(library_type STATIC)
    endif()
  endif()

  add_library(CryptoPP::CryptoPP ${library_type} IMPORTED)
  set_target_properties(CryptoPP::CryptoPP PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CryptoPP_INCLUDE_DIR};${CryptoPP_INCLUDE_DIR}/..")
  if (EXISTS "${CryptoPP_LIBRARIES}")
    set_target_properties(CryptoPP::CryptoPP PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
      IMPORTED_LOCATION "${CryptoPP_LIBRARIES}")
  endif()
  if (EXISTS "${CryptoPP_LIBRARIES_RELEASE}")
    set_property(TARGET CryptoPP::CryptoPP APPEND PROPERTY
      IMPORTED_CONFIGURATIONS RELEASE)
    set_target_properties(CryptoPP::CryptoPP PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
      IMPORTED_LOCATION_RELEASE "${CryptoPP_LIBRARIES_RELEASE}")
  endif()
  if (EXISTS "${CryptoPP_LIBRARIES_DEBUG}")
    set_property(TARGET CryptoPP::CryptoPP APPEND PROPERTY
      IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(CryptoPP::CryptoPP PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
      IMPORTED_LOCATION_DEBUG "${CryptoPP_LIBRARIES_DEBUG}")
  endif()
endif()
