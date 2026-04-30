################################################################################
# Copyright (C) 2014-2022 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH  #
#                                                                              #
#              This software is distributed under the terms of the             #
#              GNU Lesser General Public Licence (LGPL) version 3,             #
#                  copied verbatim in the file "LICENSE"                       #
################################################################################

set(target Passive)

set(sources
  PndCave.cxx
  PndGeoCave.cxx
  PndGeoMagnet.cxx
  PndGeoPassivePar.cxx
  PndGeoPipe.cxx
  PndGeoTarget.cxx
  PndMagnet.cxx
  PndPassiveContFact.cxx
  PndPipe.cxx
  PndPassive.cxx
  PndTarget.cxx
)

fair_change_extensions_if_exists(.cxx .h FILES "${sources}" OUTVAR headers)


add_library(${target} SHARED ${sources} ${headers})
add_library(PndRoot::${target} ALIAS ${target})
#set_target_properties(${target} PROPERTIES ${PROJECT_LIBRARY_PROPERTIES})

target_include_directories(${target} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(${target}
  PUBLIC
    FairRoot::Base
    FairRoot::GeoBase
    FairRoot::ParBase

  PRIVATE
    FairLogger::FairLogger
    ${VMCLIB}
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF PassiveLinkDef.h
  EXTRA_INCLUDE_DIRS ${CMAKE_INSTALL_FULL_INCLUDEDIR}
)

install(TARGETS ${target} LIBRARY DESTINATION ${PROJECT_INSTALL_LIBDIR})
install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

