################################################################################
# Copyright (C) 2014-2022 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH  #
#                                                                              #
#              This software is distributed under the terms of the             #
#              GNU Lesser General Public Licence (LGPL) version 3,             #
#                  copied verbatim in the file "LICENSE"                       #
################################################################################

set(target generalTools)

set(sources
PndFileNameCreator.cxx
PndStringSeparator.cxx
PndGeoHandling.cxx
PndMcCloner.cxx
PndMcClonerMulti.cxx
PndSensorNameContFact.cxx
PndSensorNameIdMap.cxx
PndSensorNamePar.cxx
PndEventCounterTask.cxx
PndHistoCombiner.cxx
PndMultiCanvasCreator.cxx
PndVersionInfo.cxx
pndtools.cxx
HepParMap.cxx
#PndTrackArrayMerger.cxx
)

fair_change_extensions_if_exists(.cxx .h FILES "${sources}" OUTVAR headers)


add_library(${target} SHARED ${sources} ${headers})
add_library(PndRoot::${target} ALIAS ${target})
#set_target_properties(${target} PROPERTIES ${PROJECT_LIBRARY_PROPERTIES})

target_include_directories(${target} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  ${CMAKE_SOURCE_DIR}/pnddata 
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/pnddata/PidData
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/tools

)

target_link_libraries(${target}
  PUBLIC
    ROOT::RIO
    ROOT::Gpad
    ROOT::Graf
    ROOT::Postscript
    
    FairRoot::Base
    FairRoot::ParBase
    FairRoot::Tools     # FairTools
    
    PndRoot::PndData

  PRIVATE
    ${VMCLIB}
    FairLogger::FairLogger
)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF generalToolsLinkDef.h
  EXTRA_INCLUDE_DIRS ${CMAKE_INSTALL_FULL_INCLUDEDIR}
)

install(TARGETS ${target} LIBRARY DESTINATION ${PROJECT_INSTALL_LIBDIR})
install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

