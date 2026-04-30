// $Id: GemLinkDef.h,v 1.0 Ralf Kliemt 08.Jan.2007 $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// data

#pragma link C++ class PndGemContFact + ;
#pragma link C++ class PndGemDetector + ;
#pragma link C++ class PndGemDigiAna + ;
#pragma link C++ class PndGemDigiPar + ;
#pragma link C++ class PndGemDigiRingSorter + ;
#pragma link C++ class PndGemDigiSorterTask + ;
#pragma link C++ class PndGemDigiWriteoutBuffer + ;
#pragma link C++ class PndGemDigitize + ;
#pragma link C++ class PndGemEmpty + ;
//#pragma link C++ class PndGemEventBuilderOnTracks+;
#pragma link C++ class PndGemFindClusters + ;
#pragma link C++ class PndGemFindClustersTB + ;
#pragma link C++ class PndGemFindHits + ;
#pragma link C++ class PndGemFindHitsQA + ;
#pragma link C++ class PndGemFindHitsAna + ;
#pragma link C++ class PndGemFindTracks + ;
#pragma link C++ class PndGemGeo + ;
#pragma link C++ class PndGemGeoPar + ;
#pragma link C++ class PndGemIdealHitProducer + ;
#pragma link C++ class PndGemIdealPatternRecoTask + ;
#pragma link C++ class PndGemMagneticFieldVsTrackParameters + ;
#pragma link C++ class PndGemMatchHits + ;
#pragma link C++ class PndGemMonitor + ;
#pragma link C++ class PndGemSensor + ;
#pragma link C++ class PndGemSensorMonitor + ;
#pragma link C++ class PndGemSmearingTask + ;
#pragma link C++ class PndGemStation + ;
#pragma link C++ class PndGemTrackFinder + ;
#pragma link C++ class PndGemTrackFinderIdeal + ;
#pragma link C++ class PndGemTrackFinderOnHits + ;
#pragma link C++ class PndGemTrackFinderOnHitsTB + ;
#pragma link C++ class PndGemTrackFinderQA + ;
//#pragma link C++ class PndGemTrackWriteoutBuffer+;

#endif
