// $Id: DrcLinkDef.h,v 1.13 2006/02/23 15:08:24 cecchi Exp $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class PndDrc + ;
#pragma link C++ class PndGeoDrc + ;
#pragma link C++ class PndDrcContFact + ;
#pragma link C++ class PndGeoDrcPar + ;
#pragma link C++ class PndDrcDigiRingSorter + ;
#pragma link C++ class PndDrcDigiSorterTask + ;
#pragma link C++ class PndDrcDigiWriteoutBuffer + ;
#pragma link C++ class PndDrcHitProducerIdeal + ;
#pragma link C++ class PndDrcHitProducerReal + ;

#pragma link C++ class DrawHits + ;
#pragma link C++ class PndDrcDigiPar + ;
#pragma link C++ class PndDrcDigiTask + ;
#pragma link C++ class PndDrcTimeDigiTask + ;
#pragma link C++ class PndDrcHitFinder + ;

#pragma link C++ class PndDrcLutFill + ;
#pragma link C++ class PndDrcLutReco + ;
#pragma link C++ class PndDrcReco + ;

#endif
