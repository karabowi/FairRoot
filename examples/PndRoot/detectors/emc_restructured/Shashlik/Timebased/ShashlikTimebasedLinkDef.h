#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class BSEmcPSAOptimalFilterAnalyser + ;
#pragma link C++ class BSEmcShashlykDigiPar + ;
#pragma link C++ class BSEmcExtractDigisFromWaveforms < BSEmcShashlykDigiPar> + ;

#pragma link C++ class BSEmcShashlykDigi + ;

#pragma link C++ class BSEmcShashlykNoiseAdder + ;
#pragma link C++ class BSEmcShashlykPulseshape + ;

#pragma link C++ class BSEmcMCDepositToTimebasedWaveforms < BSEmcShashlykDigiPar> + ;
#pragma link C++ class BSEmcShashlykTimebasedWaveforms + ;

#pragma link C++ class BSEmcShashlykDigiContFact + ;

#endif
