
# Create a library called "libRadMapTool" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

#if(COMMAND cmake_policy)
#  cmake_policy(SET CMP0003 OLD)
#endif(COMMAND cmake_policy)

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${CLHEP_INCLUDE_DIR}
  ${GEANT4_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES}
  )

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/tools/RadMapTool
  ${CMAKE_SOURCE_DIR}/detectors/passive
  ${CMAKE_SOURCE_DIR}/detectors/stt
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdMC
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcData
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcDigi
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcDigi/PndEmcPSAFPGA
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcMC
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcReco
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcTools
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcProtoTools
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcCorr
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcTimeBased
  ${CMAKE_SOURCE_DIR}/detectors/emc
  ${CMAKE_SOURCE_DIR}/detectors/mdt/MdtMC
  ${CMAKE_SOURCE_DIR}/detectors/sds/SdsMC
  ${CMAKE_SOURCE_DIR}/detectors/gem
  ${CMAKE_SOURCE_DIR}/detectors/dsk/DskMC
  ${CMAKE_SOURCE_DIR}/detectors/drc
  ${CMAKE_SOURCE_DIR}/detectors/fts
  ${CMAKE_SOURCE_DIR}/detectors/scitil/SciTMC
  ${CMAKE_SOURCE_DIR}/detectors/ftof
  ${CMAKE_SOURCE_DIR}/detectors/rich
  ${CMAKE_SOURCE_DIR}/pgenerators
  ${CMAKE_SOURCE_DIR}/pgenerators/Target
  ${CMAKE_SOURCE_DIR}/pgenerators/Direct
  ${CMAKE_SOURCE_DIR}/pgenerators/EvtGen/EvtGenDirect
  ${CMAKE_SOURCE_DIR}/pgenerators/EvtGen/EvtGen/R01-03-00/
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/tools
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/DrcData
  ${CMAKE_SOURCE_DIR}/pnddata/DskData
  ${CMAKE_SOURCE_DIR}/pnddata/DiscData
  ${CMAKE_SOURCE_DIR}/pnddata/FtsData
  ${CMAKE_SOURCE_DIR}/pnddata/GemData
  ${CMAKE_SOURCE_DIR}/pnddata/HypData
  ${CMAKE_SOURCE_DIR}/pnddata/HypGData
  ${CMAKE_SOURCE_DIR}/pnddata/MdtData
  ${CMAKE_SOURCE_DIR}/pnddata/MvdData
  ${CMAKE_SOURCE_DIR}/pnddata/PidData
  ${CMAKE_SOURCE_DIR}/pnddata/RichData
  ${CMAKE_SOURCE_DIR}/pnddata/SciTData
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/scitil
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib
${CLHEP_LIBRARY_DIR}
#${GEANT4_LIBRARY_DIR}
${PYTHIA8_LIB_DIR}
${CMAKE_CURRENT_BINARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
 #Mesh.cpp
 PndRadMapPlane.cxx
 PndRadMapBoxMesh.cxx
)
set(LINKDEF RadMapLinkDef.h)
set(LIBRARY_NAME RadMapTool)

set(DEPENDENCIES Base GeoBase ParBase MathCore MathMore Matrix EG Hist Physics Core)
# PndData generalTools Geane Sds TrkBase )

PANDA_GENERATE_LIBRARY()


set(MYG4LIBS G4materials G4particles G4processes ${CLHEP_LIB} G4global G4geometry G4physicslists G4track EG )
set(MYPNDLIBS FtfEvtGen DpmEvtGen FtfGenDirect RadMapTool Base ParBase  Field  Emc Stt Mdt Gem Dsk Drc Fts Sds Lmd Mvd SciT Ftof Rich Gen PGen generalTools PndData Passive)

add_executable(RadMapToolSim_exe sim_radmap.C)
add_executable(RadMapToolPlot_exe plot_radmap.C)

target_link_libraries(RadMapToolSim_exe ${ROOT_LIBRARIES} ${MYG4LIBS} ${MYPNDLIBS})
target_link_libraries(RadMapToolPlot_exe ${ROOT_LIBRARIES} ${MYG4LIBS} ${MYPNDLIBS})

set_target_properties(RadMapToolSim_exe PROPERTIES OUTPUT_NAME sim_radmap)
set_target_properties(RadMapToolPlot_exe PROPERTIES OUTPUT_NAME plot_radmap)

################# install ###################
install(TARGETS RadMapToolSim_exe DESTINATION bin)
install(TARGETS RadMapToolPlot_exe DESTINATION bin)
