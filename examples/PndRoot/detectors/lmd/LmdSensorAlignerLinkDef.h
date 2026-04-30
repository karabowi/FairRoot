// $Id: LmdLinkDef.h,v 1.0 M. Michel $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// sensor Aligner
#pragma link C++ class PndLmdPairFinderTask + ;
#pragma link C++ class PndLmdHitPair + ;
#pragma link C++ class std::map < std::string, TGeoHMatrix> + ;

#endif
