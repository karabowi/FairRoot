#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// Reconstruction
#pragma link C++ class BSEmcCreateRecoHitProcess + ;
#pragma link C++ class BSEmcOutOfClusterCorrectionProcess + ;
#pragma link C++ class BSEmcPhiRecoHitSplitter + ;
#pragma link C++ class BSEmcFinishRecoHitProcess + ;

#pragma link C++ class BSEmcCorrection + ;
#pragma link C++ class BSEmcThetaEnergyECF + ;
#pragma link C++ class BSEmcErrorCalculationPar + ;
#pragma link C++ class BSEmcErrorMatrixCalculation + ;

#pragma link C++ class BSEmcRecoContFact + ;

#pragma link C++ class BSEmcSplitOffPar + ;
#pragma link C++ class BSEmcFlagSplitOffs + ;
#pragma link C++ class BSEmcMergeRecoHits + ;

#endif
