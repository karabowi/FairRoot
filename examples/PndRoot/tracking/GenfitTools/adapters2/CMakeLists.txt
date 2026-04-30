# Create a library called "libgenfitAdapter" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${CLHEP_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES}
  ${GEANT3_INCLUDE_DIR}
  ${VMC_INCLUDE_DIRS}
)

set(INCLUDE_DIRECTORIES
  ${SIMPATH}/include
  ${ROOT_INCLUDE_DIR} 
  ${CLHEP_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES}
  ${CMAKE_SOURCE_DIR}/geane
  ${CMAKE_SOURCE_DIR}/genfit2/
  ${CMAKE_SOURCE_DIR}/genfit2/core/include
  ${CMAKE_SOURCE_DIR}/genfit2/trackReps/include
  #${CMAKE_SOURCE_DIR}/genfit/measurements/include
  #${CMAKE_SOURCE_DIR}/genfit/fitters/include
  #${CMAKE_SOURCE_DIR}/genfit/fields/include
  #${CMAKE_SOURCE_DIR}/genfit/finitePlanes/include
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
  ${ROOT_LIBRARY_DIR}
  ${FAIR_ROOT_LIBRARY_DIR}
  ${FAIRROOT_LIBRARY_DIR}
  ${SIMPATH}/lib
)

link_directories( ${LINK_DIRECTORIES})


set(SRCS 
  PndGenfitAdapters2.cxx
  PndGenfitField2.cxx
)
Set(HEADERS )

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${GFADAPTER_SRCS}" "${INCLUDE_DIRECTORIES}" GFADAPTER_RULES)
endif(RULE_CHECKER_FOUND)

set(LIBRARY_NAME genfitAdapters2)
        
set(DEPENDENCIES Base genfit2 PndData)
        
PANDA_GENERATE_LIBRARY()
