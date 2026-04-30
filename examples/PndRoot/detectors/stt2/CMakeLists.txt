################################################################################
# Copyright (C) 2014-2022 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH  #
#                                                                              #
#              This software is distributed under the terms of the             #
#              GNU Lesser General Public Licence (LGPL) version 3,             #
#                  copied verbatim in the file "LICENSE"                       #
################################################################################

set(target PndStt2)

set(sources
#Put here your sourcefiles
PndStt2.cxx
PndStt2ContFact.cxx
PndStt2DigiPar.cxx
PndStt2Digitizer.cxx
PndStt2DigiTask.cxx
PndStt2HitProducerReal.cxx
PndStt2HitProducerIdeal.cxx
PndStt2GeoHandler.cxx
)


fair_change_extensions_if_exists(.cxx .h FILES "${sources}" OUTVAR headers)


add_library(${target} SHARED ${sources} ${headers})
add_library(PndRoot::${target} ALIAS ${target})
#set_target_properties(${target} PROPERTIES ${PROJECT_LIBRARY_PROPERTIES})

target_include_directories(${target} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  ${CMAKE_SOURCE_DIR}/examples/PndRoot/trackbase
  ${CMAKE_SOURCE_DIR}/examples/PndRoot/pnddata
  ${CMAKE_SOURCE_DIR}/examples/PndRoot/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/examples/PndRoot/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/examples/PndRoot/detectors/stt2
  ${CMAKE_SOURCE_DIR}/examples/PndRoot/detectors/stt
  ${CMAKE_SOURCE_DIR}/examples/PndRoot/tools
)

target_link_libraries(${target}
  PUBLIC
    ROOT::MathCore
    ROOT::RIO
    ROOT::Hist

    FairRoot::Base
    FairRoot::GeoBase
    FairRoot::ParBase
    FairRoot::TrackBase
    FairRoot::Tools     # FairTools

    PndRoot::PndData
    PndRoot::generalTools
    PndRoot::Stt

  PRIVATE
    FairLogger::FairLogger
    ${VMCLIB}

)

fairroot_target_root_dictionary(${target}
  HEADERS ${headers}
  LINKDEF PndStt2LinkDef.h
  EXTRA_INCLUDE_DIRS ${CMAKE_INSTALL_FULL_INCLUDEDIR}
)

install(TARGETS ${target} LIBRARY DESTINATION ${PROJECT_INSTALL_LIBDIR})
install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

