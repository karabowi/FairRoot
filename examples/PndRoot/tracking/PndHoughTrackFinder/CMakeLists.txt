If (DO_THE_CUDA_DANCE)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cuda)
  add_definitions(-DTHERE_IS_CUDA)  # so we can actually do different things in the .cpp files depending if there is CUDA or not
  message(STATUS "Gentlemen, we are running with CUDA")
  # message(STATUS ${DO_THE_CUDA_DANCE})
  # message(STATUS DO_THE_CUDA_DANCE)
EndIf (DO_THE_CUDA_DANCE)


set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${SIMPATH}/include
${VMC_INCLUDE_DIRS}
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/field 
${CMAKE_SOURCE_DIR}/detectors/passive
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/detectors/stt
${CMAKE_SOURCE_DIR}/detectors/stt2
${CMAKE_SOURCE_DIR}/tracking/sttmvdtracking
${CMAKE_SOURCE_DIR}/tracking/SttCellTrackFinder
${CMAKE_SOURCE_DIR}/tracking/SttCA
${CMAKE_SOURCE_DIR}/tracking/riemannfit
${CMAKE_SOURCE_DIR}/tracking/PndIdealTrackFinder
${CMAKE_SOURCE_DIR}/tracking/PndHoughTrackFinder
${CMAKE_SOURCE_DIR}/tracking/PndApolloniusTripletTrackFinder
${CMAKE_SOURCE_DIR}/tracking/TrackingQA
${CMAKE_SOURCE_DIR}/analysis/rho  
${CMAKE_SOURCE_DIR}/analysis/rho/RhoBase
${CMAKE_SOURCE_DIR}/tracking/PndTrackingTools

)

If (DO_THE_CUDA_DANCE)
        LIST(APPEND INCLUDE_DIRECTORIES ${CUDA_INCLUDE})
EndIf (DO_THE_CUDA_DANCE)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndHoughApollonius.cxx
PndHoughUtilities.cxx
PndHoughMultipletCreator.cxx
PndHoughTrackFinderTask.cxx
PndHoughTrackFinder.cxx
PndHoughData.cxx
PndHoughTransformation.cxx
PndHoughSegment.cxx
PndHoughSpace.cxx
PndHoughMerge.cxx
PndHoughPreprocessing.cxx
PndHoughTrackCorrection.cxx
)


set(LINKDEF  PndHoughTrackFinderLinkDef.h)
set(LIBRARY_NAME Hough)


set(DEPENDENCIES Base PndData TrkBase Stt PndStt2 SttCA riemann TrackingQA TrackingTools)
if (DO_THE_CUDA_DANCE)
        LIST(APPEND DEPENDENCIES ${PNDHOUGHTRACKFINDERCUDA})
endif(DO_THE_CUDA_DANCE)

PANDA_GENERATE_LIBRARY()
