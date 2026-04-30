#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// BSEmcMC
#pragma link C++ class BSEmcDetector + ;

// DepositCreation

#pragma link C++ class BSEmcMCDepositProducerTask + ;
#pragma link C++ class BSEmcSortMCTruthAlgo + ;
#pragma link C++ class BSEmcMCDepositProductionProcess + ;
#pragma link C++ class BSEmcVolume + ;
#pragma link C++ class BSEmcVolumeDisk + ;
#pragma link C++ class BSEmcVolumeBox + ;
#pragma link C++ class BSEmcMCPar + ;

#pragma link C++ class BSEmcDigitizerTask + ;
#pragma link C++ class BSEmcDigiPar + ;
#pragma link C++ class BSEmcIdealDigitizationProcess + ;
#pragma link C++ class BSEmcDataConcentratorLookUp + ;

// Calibration
#pragma link C++ class BSEmcCalibrationPar + ;
#pragma link C++ class BSEmcApplyCalibrationProcess + ;
#pragma link C++ class BSEmcDigiDCSetterProcess + ;

#pragma link C++ class BSEmcMCContFact + ;

#endif
