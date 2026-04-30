// $Id: MvdLinkDef.h,v 1.0 Ralf Kliemt 08.Jan.2007 $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// data

// reco
#pragma link C++ class PndMvdIdealRecoTask + ;
//#pragma link C++ class PndMvdRecoTask+;
#pragma link C++ class PndMvdPixelClusterTask + ;
//#pragma link C++ class PndMvdPixelClusterFinder+;
#pragma link C++ class PndMvdSimplePixelClusterFinder + ;
#pragma link C++ class PndMvdClusterTask + ;
#pragma link C++ class PndMvdIdealClusterTask + ;
#pragma link C++ class PndMvdStripClusterTask + ;
#pragma link C++ class PndMvdChargeWeightedPixelMapping + ;
//#pragma link C++ class PndMvdStripClusterBuilder+;
//#pragma link C++ class PndMvdSimpleStripClusterFinder+;
//#pragma link C++ class PndMvdStripClusterFinder+;
//#pragma link C++ class PndMvdChargeWeightingAlgorithms+;

#pragma link C++ class PndMvdTimeWalkCorrTask + ;

// tools
#pragma link C++ class PndMvdRadDamList + ;
#pragma link C++ class PndMvdRadDamTask + ;
#pragma link C++ class PndMvdAnaRadDam + ;
#pragma link C++ class PndMvdRadDamIonizingTask + ;

#pragma link C++ class PndMvdToPix4ClusterTask;

// QA
#pragma link C++ class PndMvdQATask;

#endif
