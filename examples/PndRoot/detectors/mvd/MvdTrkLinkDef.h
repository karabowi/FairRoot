// $Id: MvdLinkDef.h,v 1.0 Ralf Kliemt 08.Jan.2007 $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// data
// #pragma link C++ class PndMvdPidCand+;

// pid
#pragma link C++ class PndMvdPidIdealTask + ;
#pragma link C++ class PndMvdIdealPidAlgo + ;
#pragma link C++ class PndMvdSimplePidAlgo + ;
#pragma link C++ class PndMvdAdvancedPidAlgo + ;

// tracking
// #pragma link C++ class PndMvdIdealTrackingTask+;
#pragma link C++ class PndMvdIdealTrackFinderTask + ;
//#pragma link C++ class PndMvdKalmanTask+;
//#pragma link C++ class PndMvdTrackFinderAnaTask+;

//#pragma link C++ class PndMvdTrackFinderAnaTask+;

//#pragma link C++ class PndMvdRiemannTrackFinder+;
//#pragma link C++ class PndMvdGemRiemannTrackFinder+;
#pragma link C++ class PndMvdSttGemRiemannTrackFinderTask + ;
#pragma link C++ class PndMvdRiemannTrackFinderTask + ;
//#pragma link C++ class PndGemRiemannTrackFinderTask+;

//#pragma link C++ class PndMvdTPCRiemannTrackFinderTask+;
//#pragma link C++ class PndMvdTPCRiemannTrackFinderTaskCutPar+;
//#pragma link C++ class PndMvdTPCRiemannTrackFinderTaskEff+;

//#pragma link C++ class PndTpcClustPlusRTFTask;
//#pragma link C++ class PndTpcClustPlusRTFTaskCutPar;

#pragma link C++ class PndMvdRiemannTrackFinderTaskCutPar + ; ///////<----
#pragma link C++ class PndMvdRiemannTrackFinderTaskEff + ;    ///////<----

#pragma link C++ class PndMvdRiemannVertexFinderTask + ;

//#pragma link C++ class PndMvdTpcRiemannCorrelatorTask+;

#pragma link C++ class TtFitRes + ;
#pragma link C++ class TtAliTask + ;
#pragma link C++ class TtCracowTask + ;

#endif
