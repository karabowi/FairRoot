# Create a library called "Master" which includes the source files given in
# the array .
# The header extension is already found. Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${GEANT3_INCLUDE_DIR}
  ${GEANT4_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES}
  ${CLHEP_INCLUDE_DIR}
  ${VMC_INCLUDE_DIRS}
  ${PYTHIA8_INCLUDE_DIR}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/tools/MasterTasks
  ${CMAKE_SOURCE_DIR}/tools
  ${CMAKE_SOURCE_DIR}/tools/ProcessTask
  ${CMAKE_SOURCE_DIR}/tracking/PndTrackingTools
  ${CMAKE_SOURCE_DIR}/tracking/PndIdealTrackFinder
  ${CMAKE_SOURCE_DIR}/tracking/PndBarrelTrackFinder
  ${CMAKE_SOURCE_DIR}/tracking/CentralTracking
  ${CMAKE_SOURCE_DIR}/tracking/sttmvdtracking
  ${CMAKE_SOURCE_DIR}/tracking/ftscatracking
  ${CMAKE_SOURCE_DIR}/tracking/ftscatracking/code
  ${CMAKE_SOURCE_DIR}/tracking/ftscatracking/code/Performance
  ${CMAKE_SOURCE_DIR}/tracking/ftscatracking/code/KFParticle
  ${CMAKE_SOURCE_DIR}/tracking/PndFtsTrackFinder
  ${CMAKE_SOURCE_DIR}/tracking/PndFtsTrackFinder/tool
  ${CMAKE_SOURCE_DIR}/tracking/PndFtsTrackFinder/core
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
  ${CMAKE_SOURCE_DIR}/detectors/stt
  ${CMAKE_SOURCE_DIR}/detectors/stt2
  ${CMAKE_SOURCE_DIR}/detectors/sds/SdsMC
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdMC
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdDigi
  ${CMAKE_SOURCE_DIR}/detectors/mvd/MvdReco
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcMC
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcDigi
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcData
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcTools
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcReco
  ${CMAKE_SOURCE_DIR}/detectors/emc/EmcCorr
  ${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcMC
  ${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcTools
  ${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcGeoParts
  ${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcMC/DepositCreation
  ${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Base/EmcMC/DepositCreation/Processes
  ${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Utility
  ${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Barrel
  ${CMAKE_SOURCE_DIR}/detectors/emc_restructured/BwEndcap
  ${CMAKE_SOURCE_DIR}/detectors/emc_restructured/Shashlik
  ${CMAKE_SOURCE_DIR}/detectors/emc_restructured/FwEndcap/Tools
  ${CMAKE_SOURCE_DIR}/detectors/emc_restructured/CompactEmc/BarrelSonderslice
  ${CMAKE_SOURCE_DIR}/detectors/scitil/SciTMC
  ${CMAKE_SOURCE_DIR}/detectors/scitil/SciTDigi
  ${CMAKE_SOURCE_DIR}/detectors/mdt/MdtMC
  ${CMAKE_SOURCE_DIR}/detectors/mdt/MdtDigi
  ${CMAKE_SOURCE_DIR}/detectors/mdt/MdtReco
  ${CMAKE_SOURCE_DIR}/detectors/drc
  ${CMAKE_SOURCE_DIR}/detectors/dsk/DskMC
  ${CMAKE_SOURCE_DIR}/detectors/gem
  ${CMAKE_SOURCE_DIR}/detectors/fts
  ${CMAKE_SOURCE_DIR}/detectors/ftof
  ${CMAKE_SOURCE_DIR}/detectors/rich
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/detectors/passive
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/SciTData
  ${CMAKE_SOURCE_DIR}/pnddata/MdtData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/pnddata/DrcData
  ${CMAKE_SOURCE_DIR}/pnddata/DskData
  ${CMAKE_SOURCE_DIR}/pnddata/GemData
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/FtsData
  ${CMAKE_SOURCE_DIR}/pnddata/PidData
  ${CMAKE_SOURCE_DIR}/pnddata/RichData
  ${CMAKE_SOURCE_DIR}/pnddata/EmcData
  ${CMAKE_SOURCE_DIR}/detectors/fts/FtsTracking
  ${CMAKE_SOURCE_DIR}/tools/PndMCMatchNewLinks
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/recotasks
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/recotasks2
  ${CMAKE_SOURCE_DIR}/tracking/GenfitTools/gfreco
  ${CMAKE_SOURCE_DIR}/tracking/PndPropagator
  ${CMAKE_SOURCE_DIR}/tracking/PndGeanePropagator
  ${CMAKE_SOURCE_DIR}/tracking/PndHelixPropagator
  ${CMAKE_SOURCE_DIR}/genfit
  ${CMAKE_SOURCE_DIR}/genfit2
  ${CMAKE_SOURCE_DIR}/genfit2/core/include
  ${CMAKE_SOURCE_DIR}/genfit2/measurements/include
  ${CMAKE_SOURCE_DIR}/genfit2/fitters/include
  ${CMAKE_SOURCE_DIR}/genfit2/finitePlanes/include
  ${CMAKE_SOURCE_DIR}/genfit2/trackReps/include
  ${CMAKE_SOURCE_DIR}/genfit2/fields/include
  ${CMAKE_SOURCE_DIR}/pid/PidCorr
  ${CMAKE_SOURCE_DIR}/pid/PidCorr/PidDetectors
  ${CMAKE_SOURCE_DIR}/pid/PidCorr/TrackingDetectors
  ${CMAKE_SOURCE_DIR}/pid/PidClassifier
  ${CMAKE_SOURCE_DIR}/pid/MatchBarrelForward
  ${CMAKE_SOURCE_DIR}/pid/HadronicSplitOffCut

)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${BASE_LINK_DIRECTORIES}
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib
${FORTRAN_LIBDIR}
${PYTHIA8_LIB_DIR}
${HEPMC_LIB_DIR}
${SIMPATH}/lib
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndMasterTask.cxx
PndMasterSimTask.cxx
PndMasterDigiTask.cxx
PndMasterDigiOnlyTask.cxx
PndMasterRecoTask.cxx
PndMasterLocalRecoTask.cxx
PndMasterRecoIdealTask.cxx
#PndMasterPidTask.cxx
PndMasterMultiPidTask.cxx
PndMasterRunSim.cxx
PndMasterRunAna.cxx
PndMasterRunCompactSim.cxx
PndMasterRunDay1Sim.cxx
PndMasterRunPhase1Sim.cxx
)

set(LINKDEF  MasterLinkDef.h)
set(LIBRARY_NAME Master)

set(DEPENDENCIES Base Gen generalTools MvdReco CentralTracking TrackingTools IdealTrackFinder SttMvdTracking
    BarrelTrackFinder MCMatchNewLinks RecoTasks RecoTasks2 Pid Field Mvd Stt PndStt2 SciT Drc Dsk Mdt Rich PGen
    EvtGenDirect FtfGenDirect pndpropagator geanepropagator helixpropagator EMFFGEN GFReco FtsTrackFinder EmcMC
    NewEmcBarrel NewEmcBwEndcap FwEndcapTools NewShashlik CmpPndBarrelSonderslice)

If(VCSUPPORT)
  set(DEPENDENCIES ${DEPENDENCIES} CAFtsTracking)
EndIf()

PANDA_GENERATE_LIBRARY()
