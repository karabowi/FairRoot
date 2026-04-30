SET(EMC_BASE_INCLUDE_DIRS
${CMAKE_SOURCE_DIR}/tools
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/EmcData
${CMAKE_SOURCE_DIR}/ProcessTask
${CMAKE_SOURCE_DIR}/PndContainer

${CMAKE_SOURCE_DIR}/detectors/emc_restructured
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcPlotProcesses
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcTools
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcClustering
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcClustering/ClusterProperties
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcClustering/Algorithms
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcClustering/Processes
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcMC
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcMC/DepositCreation
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcMC/DepositCreation/Processes
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcMC/DigiCreation
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcMC/DigiCreation/Processes
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcTasks
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcReco
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcReco/Processes
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/Timebased
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/Timebased/WaveformGeneration
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/Timebased/WaveformFeatureExtraction
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/Timebased/TimeSorting
${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcGeoParts
)

add_subdirectory(Base)
add_subdirectory(FwEndcap)
add_subdirectory(BwEndcap)
add_subdirectory(Barrel)
add_subdirectory(Shashlik)
add_subdirectory(Utility)
add_subdirectory(CompactEmc)

# install(EXPORT PandaEmcExports DESTINATION lib)
