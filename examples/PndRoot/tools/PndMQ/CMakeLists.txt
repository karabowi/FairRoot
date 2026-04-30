 ################################################################################
 #    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    #
 #                                                                              #
 #              This software is distributed under the terms of the             #
 #         GNU Lesser General Public Licence version 3 (LGPL) version 3,        #
 #                  copied verbatim in the file "LICENSE"                       #
 ################################################################################

Set(INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES}
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
    
  ${CMAKE_SOURCE_DIR}/fairmq/zeromq
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/MvdData
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/GemData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/detectors/passive
  ${CMAKE_SOURCE_DIR}/tools
  ${CMAKE_SOURCE_DIR}/tools/PndMQ
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

  "PndMQGapEventBuilder.cxx"
  "PndMapSorter.cxx"
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
  FairTestDetector
  RIO
  Tree
)

set(LIBRARY_NAME pndMQ)

PANDA_GENERATE_LIBRARY()

#set(Exe_Names
##  PndMQFileSampler
#)




#set(Exe_Source
##  runFileSampler.cxx
#)

#list(LENGTH Exe_Names _length)
#math(EXPR _length ${_length}-1)

#ForEach(_file RANGE 0 ${_length})
#  list(GET Exe_Names ${_file} _name)
#  list(GET Exe_Source ${_file} _src)
#  set(EXE_NAME ${_name})
#  set(SRCS ${_src})
#  set(DEPENDENCIES FairMQ testMQ ${FAIRMQ_DEPENDENCIES})
#  GENERATE_EXECUTABLE()
#EndForEach(_file RANGE 0 ${_length})
