#
# Created by Bartosz Sobol on 17.11.2021.
#


set(INCLUDE_DIRECTORIES
        ${CMAKE_SOURCE_DIR}/tracking/PndFtsTrackFinder
        ${CMAKE_SOURCE_DIR}/tracking/PndFtsTrackFinder/tool
        ${CMAKE_SOURCE_DIR}/tracking/PndFtsTrackFinder/core
        ${CMAKE_SOURCE_DIR}/pnddata
        ${CMAKE_SOURCE_DIR}/pnddata/FtsData
        ${CMAKE_SOURCE_DIR}/pnddata/TrackData
        ${CMAKE_SOURCE_DIR}/detectors/fts
        )

set(SYSTEM_INCLUDE_DIRECTORIES
        ${SYSTEM_INCLUDE_DIRECTORIES}
        ${BASE_INCLUDE_DIRECTORIES}
        ${VMC_INCLUDE_DIRS}
        )

include_directories(${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

link_directories(
        ${ROOT_LIBRARY_DIR}
        ${FAIRROOT_LIBRARY_DIR}
        ${SIMPATH}/lib
)

set(SRCS
        PndFtsTrackFinderTask.cxx
        PndFtsTrackFinderTask.h
        PndFtsTrackFinderPndTrackProducer.cxx
        PndFtsTrackFinderPndTrackProducer.h
        tool/PndFtsSimpleHit.h
        tool/PndFtsContext.h
        tool/PndFtsGeom.h
        tool/PndFtsGeomUtil.cxx
        tool/PndFtsGeomUtil.h
        tool/PndFtsMath.h
        tool/PndFtsHelpers.h
        tool/PndFtsStation.h
        tool/PndFtsCircleHitSet.cxx
        tool/PndFtsCircleHitSet.h
        tool/PndFtsLineHitSet.cxx
        tool/PndFtsLineHitSet.h
        tool/PndFtsConstants.h
        tool/PndFtsStraw.h
        tool/PndFtsTrack.h
        tool/PndFtsUtility.h
        core/PndFtsReconCircle.h
        core/PndFtsReconCircle.cxx
        core/PndFtsReconCircleZOX.cxx
        core/PndFtsReconCircleZOX.h
        core/PndFtsReconCircleZOY.cxx
        core/PndFtsReconCircleZOY.h
        core/PndFtsReconLine.h
        core/PndFtsReconLine.cxx
        core/PndFtsReconLineZOX.cxx
        core/PndFtsReconLineZOX.h
        core/PndFtsReconLineZOY.cxx
        core/PndFtsReconLineZOY.h
        core/PndFtsReconMerge.cxx
        core/PndFtsReconMerge.h
        core/PndFtsReconRunner.cxx
        core/PndFtsReconRunner.h
        )

set(LINKDEF PndFtsTrackFinderLinkDef.h)
set(LIBRARY_NAME FtsTrackFinder)

set(DEPENDENCIES Base RIO Tree PndData TrkBase)

PANDA_GENERATE_LIBRARY()
