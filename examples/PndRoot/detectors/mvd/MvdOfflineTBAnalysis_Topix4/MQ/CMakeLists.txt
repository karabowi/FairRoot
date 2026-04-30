 ################################################################################
 #    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    #
 #                                                                              #
 #              This software is distributed under the terms of the             #
 #         GNU Lesser General Public Licence version 3 (LGPL) version 3,        #
 #                  copied verbatim in the file "LICENSE"                       #
 ################################################################################

configure_file(${CMAKE_SOURCE_DIR}/detectors/mvd/MvdOfflineTBAnalysis_Topix4/MQ/TopixChain.json ${CMAKE_BINARY_DIR}/bin/config/TopixChain.json)
configure_file(${CMAKE_SOURCE_DIR}/detectors/mvd/MvdOfflineTBAnalysis_Topix4/MQ/CombineChain.json ${CMAKE_BINARY_DIR}/bin/config/CombineChain.json)

configure_file(${CMAKE_SOURCE_DIR}/detectors/mvd/MvdOfflineTBAnalysis_Topix4/macro/startAll.sh.in ${CMAKE_BINARY_DIR}/bin/startAll.sh)
configure_file(${CMAKE_SOURCE_DIR}/detectors/mvd/MvdOfflineTBAnalysis_Topix4/macro/startAll_FE2.sh.in ${CMAKE_BINARY_DIR}/bin/startAll_FE2.sh)
configure_file(${CMAKE_SOURCE_DIR}/detectors/mvd/MvdOfflineTBAnalysis_Topix4/macro/startAll_FE3.sh.in ${CMAKE_BINARY_DIR}/bin/startAll_FE3.sh)
configure_file(${CMAKE_SOURCE_DIR}/detectors/mvd/MvdOfflineTBAnalysis_Topix4/macro/startAll_FE4.sh.in ${CMAKE_BINARY_DIR}/bin/startAll_FE4.sh)
#configure_file(${CMAKE_SOURCE_DIR}/detectors/mvd/MvdOfflineTBAnalysis_Topix4/macro/startMvdClusterTask.sh.in ${CMAKE_BINARY_DIR}/bin/startMvdClusterTask.sh)



configure_file(${CMAKE_SOURCE_DIR}/detectors/mvd/MvdOfflineTBAnalysis_Topix4/macro/combineAll.sh.in ${CMAKE_BINARY_DIR}/bin/combineAll.sh)
configure_file(${CMAKE_SOURCE_DIR}/detectors/mvd/MvdOfflineTBAnalysis_Topix4/macro/combineAllRamDisk.sh.in ${CMAKE_BINARY_DIR}/bin/combineAllRamDisk.sh)

Set(INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES}
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdOfflineTBAnalysis_Topix4
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdOfflineTBAnalysis_Topix4/MQ
  ${CMAKE_CURRENT_BINARY_DIR}
)

Message(STATUS ${BASE_INCLUDE_DIRECTORIES} " base include dir")
Message(STATUS ${CMAKE_CURRENT_BINARY_DIR} " binary dir")

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${Boost_INCLUDE_DIR}
  ${SYSTEM_INCLUDE_DIRECTORIES}

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

  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdOfflineTBAnalysis_Topix4

    ${CMAKE_SOURCE_DIR}/fairmq/zeromq
  )
  
  Set(SYSTEM_INCLUDE_DIRECTORIES
    ${SYSTEM_INCLUDE_DIRECTORIES}
    ${ZMQ_INCLUDE_DIR}
  )

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

Set(LINK_DIRECTORIES
  ${ROOT_LIBRARY_DIR}
  ${FAIRROOT_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS}
)

Link_Directories(${LINK_DIRECTORIES})

set(SRCS

  "PndMQExample1Sampler.cxx"
  "PndMQExample1Sink.cxx"
  "PndMQTopix4Sampler.cxx"
  "PndMQTopix4ProcessorTask.cxx"
  "PndMQTopix4Processor.cxx"
  "PndMQTopix4Sink.cxx"
  "PndMQSorterDistributor.cxx"
  "PndMQTopix4Sorter.cxx"
  "PndMQSorterMerger.cxx"
  "PndMQFileSink.cxx"
  "PndMQFileSinkHits.cxx"
  "PndMQDataDuplicator.cxx"
  "PndMQTopix4OnlineHisto.cxx"
  "PndMQGapEventBuilderToPix.cxx"
  "PndMQTopix4DigiToHit.cxx"
  "PndMvdTopixClusterFinder.cxx"
  "PndMvdTopixHitProducer.cxx"
  "PndMQTopixHitProducer.cxx"
  "PndMQSdsHitLoader.cxx"
  "PndMQHitsEventBuilder.cxx"
  "PndMQHitEventDevice.cxx"
  "PndMQFileSamplerHits.cxx"
#  "PndMvdMQFileSampler.cxx"
)


# to copy src that are header-only files (e.g. c++ template) for FairRoot external installation
# manual install (globbing add not recommended)
#Set(FAIRMQHEADERS
#  tools/FairMQTools.h
#)
#install(FILES ${FAIRMQHEADERS} DESTINATION ${PANDA_INCLUDE})

set(DEPENDENCIES
  ${DEPENDENCIES}
  Base
  BaseMQ
  Core
  boost_thread
  fairmq_logger
  boost_log
  boost_log_setup
  boost_timer
  boost_system
  boost_filesystem
  boost_program_options
  boost_random
  boost_chrono
  boost_exception
  Mvd
  FairTestDetector
  RIO
  Tree
)

set(LIBRARY_NAME testMQ)

PANDA_GENERATE_LIBRARY()

set(Exe_Names
  Topix4-sampler
  Topix4-processor
  Topix4-sink
  Topix4-distributor
  Topix4-sorter
  Topix4-merger
  Topix4-filesink
  Topix4-filesinkhits
  Topix4-duplicator
  Topix4-onlineHisto
  Topix4-digitohit
  Topix4-hitsampler
  Topix4-hiteventbuilder
  Topix4-hitsampler2
#  PndMQFileSampler
)




set(Exe_Source
  runPndTopixSampler.cxx
  runPndTopixProcessor.cxx
  runPndTopixSink.cxx
  runPndTopixSorterDistributor.cxx
  runPndTopixSorter.cxx
  runPndTopixSorterMerger.cxx
  runPndTopixFileSink.cxx
  runPndTopixFileSinkHits.cxx
  runPndTopixDuplicator.cxx
  runPndTopixOnlineHisto.cxx
  runPndTopixDigiToHit.cxx
  runHitSampler.cxx
  runPndHitEventBuilder.cxx
  runPndTopixHitSampler.cxx
#  runFileSampler.cxx
)

list(LENGTH Exe_Names _length)
math(EXPR _length ${_length}-1)

ForEach(_file RANGE 0 ${_length})
  list(GET Exe_Names ${_file} _name)
  list(GET Exe_Source ${_file} _src)
  set(EXE_NAME ${_name})
  set(SRCS ${_src})
  set(DEPENDENCIES FairMQ testMQ ${FAIRMQ_DEPENDENCIES})
  GENERATE_EXECUTABLE()
EndForEach(_file RANGE 0 ${_length})
