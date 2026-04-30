 ################################################################################
 #    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    #
 #                                                                              #
 #              This software is distributed under the terms of the             # 
 #         GNU Lesser General Public Licence version 3 (LGPL) version 3,        #  
 #                  copied verbatim in the file "LICENSE"                       #
 ################################################################################
# Create a library called "libPixel" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(MVDMQ_FILE_LOCATION ${CMAKE_SOURCE_DIR})
set(MVDMQ_BIN_LOCATION  ${CMAKE_BINARY_DIR}/bin)
set(EXAMPLE9_BIN_LOCATION ${FAIRROOTPATH}/bin)

configure_file(${CMAKE_SOURCE_DIR}/detectors/mvd/MvdMQ/run/scripts/startMvdClusterTask.sh.in ${CMAKE_BINARY_DIR}/bin/startMvdClusterTask.sh)
configure_file(${CMAKE_SOURCE_DIR}/detectors/mvd/MvdMQ/run/scripts/startMvdFileSamplerBursts.sh.in ${CMAKE_BINARY_DIR}/bin/startMvdFileSamplerBursts.sh)
configure_file(${CMAKE_SOURCE_DIR}/detectors/mvd/MvdMQ/run/scripts/startMvdStripClusterTask.sh.in ${CMAKE_BINARY_DIR}/bin/startMvdStripClusterTask.sh)
configure_file(${CMAKE_SOURCE_DIR}/detectors/mvd/MvdMQ/run/scripts/startCombinedClusterTask.sh.in ${CMAKE_BINARY_DIR}/bin/startCombinedClusterTask.sh)
configure_file(${CMAKE_SOURCE_DIR}/detectors/mvd/MvdMQ/run/scripts/startMvdPixelProcessor.sh.in ${CMAKE_BINARY_DIR}/bin/startMvdPixelProcessor.sh)

set(INCLUDE_DIRECTORIES
    ${BASE_INCLUDE_DIRECTORIES}
    ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdMQ
    ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdMQ/src
    ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdMQ/src/devices
    ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdReco
    ${CMAKE_SOURCE_DIR}/detectors/sds
    ${CMAKE_SOURCE_DIR}/detectors/sds/SdsReco
    ${CMAKE_SOURCE_DIR}/detectors/sds/SdsFEModel
    ${CMAKE_SOURCE_DIR}/detectors/sds/SdsTools
    ${CMAKE_SOURCE_DIR}/pnddata/SdsData
    ${CMAKE_SOURCE_DIR}/tools/MQ
    ${CMAKE_CURRENT_BINARY_DIR}
    ${FAIRROOT_INCLUDE_DIR}
)


Set(SYSTEM_INCLUDE_DIRECTORIES
    ${Boost_INCLUDE_DIR}
    ${SYSTEM_INCLUDE_DIRECTORIES}
    ${ZMQ_INCLUDE_DIR}
)

  Set(INCLUDE_DIRECTORIES
    ${INCLUDE_DIRECTORIES}
    ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/MvdData
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/GemData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/detectors/sds/SdsDigi
  ${CMAKE_SOURCE_DIR}/detectors/passive
  ${CMAKE_SOURCE_DIR}/tools
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdTools
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdReco
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdMQ/src/algorithms

  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdOfflineTBAnalysis_Topix4

    ${CMAKE_SOURCE_DIR}/fairmq/zeromq
  )

Include_Directories( ${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
	${ROOT_LIBRARY_DIR}
	${FAIRROOT_LIBRARY_DIR}
	${Boost_LIBRARY_DIRS}
) 

Link_Directories( ${LINK_DIRECTORIES})

Set(SRCS
	src/devices/PndMvdMQFileSampler.cxx
	src/devices/PndMvdMQFileSamplerBursts.cxx
	src/devices/PndMvdMQFileSink.cxx
	src/devices/PndMQFileSinkBursts.cxx
	src/devices/PndMQBurstProcessor.cxx
	src/devices/PndMQMerger.cxx
	src/devices/PndMQMergerTest.cxx
	src/devices/PndMQMvdPixelDigiProcessorBursts.cxx
	src/algorithms/PndMvdPixelClusterFinder.cxx
	src/algorithms/PndMQSdsPixelBackMapping.cxx
	src/algorithms/PndMQSdsChargeWeightedPixelMapping.cxx
	src/algorithms/PndMQMvdChargeWeightedPixelMapping.cxx
	
)

Set(HEADER
	src/devices/PndMvdMQTaskProcessor.h
	src/devices/PndMvdMQTaskProcessor.tpl
)

Set(DEPENDENCIES
    Base
    BaseMQ
    Core
    Net
    fairmq_logger
    FairTools
    Mvd
    MvdReco
    RIO
    Tree
    GeoBase
    ParBase
    MCStack
    FairMQ 
    Gen
    Passive
    ${Boost_THREAD_LIBRARY} 
    ${Boost_SYSTEM_LIBRARY} 
    ${Boost_SERIALIZATION_LIBRARY} 
   ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

Set(LIBRARY_NAME MvdMQ)

PANDA_GENERATE_LIBRARY()

set(Exe_Names
  PndMQFileSampler
  PndMQFileSamplerBursts
  PndMQFileSink
  PndMQFileSinkBursts
  PndMQTaskProcessor
  PndMQPixelProcessor
  PndMQMergerTest
)

set(Exe_Source
  run/runFileSampler.cxx
  run/runFileSamplerBursts.cxx
  run/runFileSink.cxx
  run/runFileSinkBursts.cxx
  run/runTaskProcessor.cxx
  run/runPixelDigisProcessor.cxx
  run/runPndMQMergerTest.cxx
)

list(LENGTH Exe_Names _length)
math(EXPR _length ${_length}-1)

ForEach(_file RANGE 0 ${_length})
  list(GET Exe_Names ${_file} _name)
  list(GET Exe_Source ${_file} _src)
  set(EXE_NAME ${_name})
  set(SRCS ${_src})
  set(DEPENDENCIES FairMQ MvdMQ Mvd Sds SdsReco PndData ${FAIRMQ_DEPENDENCIES})
  GENERATE_EXECUTABLE()
EndForEach(_file RANGE 0 ${_length})
