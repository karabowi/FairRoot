#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class PndHyp + ;
#pragma link C++ class PndGeoHyp + ;
#pragma link C++ class PndGeoHypPar + ;
#pragma link C++ class PndHypContFact + ;
#pragma link C++ class PndHypHitProducerIdeal + ;
#pragma link C++ class THParticle + ;
//#pragma link C++ class  PndHypDecayer+;
//#pragma link C++ class  PndHypDecayReader+;
//#pragma link C++ class  PndHypAnaIdeal+;
#pragma link C++ class HypStatDecay + ;
//#pragma link C++ class  PndHypDigi+;

// data
/*#pragma link C++ class PndHypCluster+;
//#pragma link C++ class PndHypDigiPixel+;
//#pragma link C++ class PndHypPixel+;
//#pragma link C++ class PndHypDigiStrip+;
//#pragma link C++ class PndHypStrip+;
//#pragma link C++ class PndHypGeoMappingPar+;
//#pragma link C++ class PndHypCluster+;
*/

//#pragma link C++ class PndHypRecoHit+;
//#pragma link C++ class PndHypRecoSPHit+;

//#pragma link C++ class PndHypStripCluster+;

// digi

#pragma link C++ class PndHypDigiPar + ;
#pragma link C++ class PndHypStripDigiPar + ;
#pragma link C++ class PndHypCalcStrip;
#pragma link C++ class PndHypStripHitProducer + ;
//#pragma link C++ class PndHypPixelHitProducer+;

// reco
#pragma link C++ class PndHypIdealRecoTask + ;
//#pragma link C++ class PndHypRecoTask+;
//#pragma link C++ class PndHypHitMaker+;
//#pragma link C++ class PndHypHitMakerFactory+;
//#pragma link C++ class PndHypSimpleHitMaker+;
#pragma link C++ class PndHypStripClusterBuilder + ;
#pragma link C++ class PndHypStripClusterTask + ;

// fullana
//#pragma link C++ class PndHypFullAna+;
//#pragma link C++ class PndHypSimpleAna+;
//#pragma link C++ class PndHypFullIdealAna+;
//#pragma link C++ class PndHypMicroWriter+;
//#pragma link C++ class PndHypMicroIdealWriter+;

// tools
#pragma link C++ class PndHypGeoHandling + ;
//#pragma link C++ class PndHypMSAnaTask+;
#pragma link C++ class PndStringVector + ;

// tracking
/*#pragma link C++ class PndHypIdealTrackingTask+;
#pragma link C++ class PndHypIdealTrackFinderTask+;
//#pragma link C++ class PndHypRiemannTrackFinderTask+;
#pragma link C++ class PndHypKalmanTask+;
#pragma link C++ class PndHypDKalmanTask+;
#pragma link C++ class PndHypDPatternRecoTask+;
#pragma link C++ class PndHypIdealPRTask+;
#pragma link C++ class PndFieldAdaptor+;
*/
#pragma link C++ class PndHypAdvancedPidAlgo + ;
#pragma link C++ class PndHypPidIdealTask + ;

#endif
