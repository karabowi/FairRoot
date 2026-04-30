# Create a library called "Master" which includes the source files given in
# the array .
# The header extension is already found. Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${BASE_INCLUDE_DIRECTORIES}
  ${VMC_INCLUDE_DIRS} 
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/tools/MasterTasks
  ${CMAKE_SOURCE_DIR}/tools
  ${CMAKE_SOURCE_DIR}/tools/JSONReaders
  ${CMAKE_SOURCE_DIR}/pgenerators
  ${CMAKE_SOURCE_DIR}/pgenerators/Target
  ${CMAKE_SOURCE_DIR}/pgenerators/Direct
  ${CMAKE_SOURCE_DIR}/pgenerators/FileReaders
  ${CMAKE_SOURCE_DIR}/pgenerators/particleguns
  ${CMAKE_SOURCE_DIR}/pgenerators/FtfEvtGen
  ${CMAKE_SOURCE_DIR}/pgenerators/FtfEvtGen/include
  ${CMAKE_SOURCE_DIR}/pgenerators/Pythia
  ${CMAKE_SOURCE_DIR}/pgenerators/eventFilter
  ${CMAKE_SOURCE_DIR}/pgenerators/EvtGen/EvtGen/R01-03-00
  ${CMAKE_SOURCE_DIR}/pgenerators/EvtGen/EvtGenDirect
  ${CMAKE_SOURCE_DIR}/pgenerators/Direct
  ${CMAKE_SOURCE_DIR}/pgenerators/EMFFgenerators
  ${CMAKE_SOURCE_DIR}/tracking/TrackingQA
  ${CMAKE_SOURCE_DIR}/tracking/TrackingQANew
  ${CMAKE_SOURCE_DIR}/tracking/PndIdealTrackFinder
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/detectors/stt
  ${CMAKE_SOURCE_DIR}/analysis/rho/RhoHistogram
  ${CMAKE_SOURCE_DIR}/tests/baseClasses
  ${CMAKE_SOURCE_DIR}/tests/tracking/baseClasses
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${BASE_LINK_DIRECTORIES}
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${FORTRAN_LIBDIR}
${SIMPATH}/lib
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndJSONReader.cxx
PndMasterRunSimJSONReader.cxx
PndMasterRunAnaJSONReader.cxx
PndBoxGeneratorJSONReader.cxx
PndFixStepParticleGunJSONReader.cxx
PndTaskJSONReader.cxx
PndMultipleTasksJSONReader.cxx
PndTrackingQAJSONReader.cxx
PndIdealTrackingJSONReader.cxx
PndMeasurementBoundariesJSONReader.cxx
PndTrackingQACDashJSONReader.cxx
PndHistoCombinerJSONReader.cxx
)

set(LINKDEF  JSONReaderLinkDef.h)
set(LIBRARY_NAME JSONReader)

set(DEPENDENCIES Base Gen generalTools Master PGen EvtGenDirect FtfGenDirect TrackingQA TestsTrackingQACDash PandaTestBase)


PANDA_GENERATE_LIBRARY()
