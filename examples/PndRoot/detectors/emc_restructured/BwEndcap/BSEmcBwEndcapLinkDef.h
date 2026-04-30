#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class BSEmcBwEndcap2DMapper + ;
#pragma link C++ class BSEmcBwEndcapECF + ;
#pragma link C++ class BSEmcBwEndcapSensorNameIdMap + ;
#pragma link C++ class BSEmcBwEndcapDigi + ;
#pragma link C++ class BSEmcBwEndcapDigiPar + ;
#pragma link C++ class BSEmcBwEndcapDigitizer + ;
#pragma link C++ class BSEmcBwEndcapNoiseAdder + ;
#pragma link C++ class BSEmcBwEndcapPulseAmplifier + ;
#pragma link C++ class BSEmcBwEndcapTimebasedWaveforms + ;
#pragma link C++ class BSEmcBwPulseshape + ;

#pragma link C++ class BSEmcMCDepositToTimebasedWaveforms < BSEmcBwEndcapDigiPar> + ;
#pragma link C++ class BSEmcExtractDigisFromWaveforms < BSEmcBwEndcapDigiPar> + ;
#pragma link C++ class BSEmcBwEndcapContFact + ;
#pragma link C++ class BSEmcMultiPSA + ;
#pragma link C++ class BSEmcPSACombinator + ;
#pragma link C++ class BSEmcPSAOverflowCombinator + ;
#pragma link C++ class BSEmcPSATmaxAnalyser + ;
#pragma link C++ class BSEmcBwEndcapOnlineFeatureExtraction + ;
#endif
