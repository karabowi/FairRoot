// $Id: LmdTrkLinkDef.h,v 1.0 M. Michel$

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// tracking
//#pragma link C++ class PndLmdMCPointTrackFinderTask+;
#pragma link C++ class PndLmdTrackFinderTask + ;
#pragma link C++ class PndLmdTrackFinderCATask + ;
#pragma link C++ class PndLmdLinFitTask + ;
#pragma link C++ class PndLmdLineTask + ;
#pragma link C++ class PndLmdIdealFitTask + ;
#pragma link C++ class PndLinTrack + ;
//#pragma link C++ class PndLmdMCPointLinFitTask+;
//#pragma link C++ class PndLmdTrackCandToGFTrackCandConvertorTask+;
#pragma link C++ class PndLmdGeaneTask + ;
#pragma link C++ class PndLmdSigCleanTask + ;
// PndLmdKalmanTask and PndLmdBPRungeKuttaTask have dependences from genfit -> switched off until movement to gentfit2
//#pragma link C++ class PndLmdKalmanTask+;
//#pragma link C++ class PndLmdBPRungeKuttaTask+;
#pragma link C++ class PndLmdTrksFilterTask + ;
#pragma link C++ class PndLmdNoiseTrkSuppressionTask + ;

#endif
