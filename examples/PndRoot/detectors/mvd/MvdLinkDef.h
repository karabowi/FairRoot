// $Id: MvdLinkDef.h,v 1.0 Ralf Kliemt 08.Jan.2007 $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// data
// #pragma link C++ class PndSdsMCPoint+;
// #pragma link C++ class PndMvdDigi+;
// #pragma link C++ class PndMvdDigiPixel+;
// #pragma link C++ class PndMvdPixel+;
// #pragma link C++ class PndMvdStrip+;
// #pragma link C++ class PndMvdApvHit+;

// mc
//#pragma link C++ class PndMvdGeo+;
//#pragma link C++ class PndMvdGeoPar+;
#pragma link C++ class PndMvdContFact + ;
#pragma link C++ class PndMvdDetector + ;

// digi
#pragma link C++ class PndMvdHybridHitProducer + ;
#pragma link C++ class PndMvdStripHitProducer + ;
//#pragma link C++ class PndMvdCalcPixel;
//#pragma link C++ class PndMvdCalcFePixel;
//#pragma link C++ class PndMvdCalcStrip;
#pragma link C++ class PndMvdDigiTask + ;
//#pragma link C++ class PndMvdPixelDigiPar+;
//#pragma link C++ class PndMvdStripDigiPar+;
//#pragma link C++ class PndMvdDigiAna+;
#pragma link C++ class PndMvdHitProducerIdeal + ;
#pragma link C++ class PndMvdNoiseProducer + ;
//#pragma link C++ class PndMvdCalcFePixel+;

//#pragma link C++ class PndMvdMCEventAna+;
//#pragma link C++ class PndMvdEventAna+;
//#pragma link C++ class PndMvdDigiEventAna+;
//#pragma link C++ class PndMvdEventAna+;
//#pragma link C++ class PndMvdMCEventAna+;
//#pragma link C++ class PndMvdDigiEventAna+;
//#pragma link C++ class PndMvdAllDataEventAna+;
#pragma link C++ class PndMvdAccessRTDBTask + ;

#pragma link C++ class PndGeoHitList + ;
#pragma link C++ class PndEventDisplay + ;
//#pragma link C++ class PndMvdEventAnaTask+;
//#pragma link C++ class PndMvdEventMerger+;
//#pragma link C++ class PndMvdEventMergerTask+;
#pragma link C++ class PndMvdMSAnaTask + ;

#pragma link C++ class PndMvdSiHit + ;
#pragma link C++ class PndMvdTdcData + ;
#pragma link C++ class PndMvdQdcData + ;
#pragma link C++ class PndMvdTsEvent + ;

#pragma link C++ class PndMvdConvertApv + ;
#pragma link C++ class PndMvdBoxMap;
#pragma link C++ class PndMvdConvertApvTask + ;
#pragma link C++ class PndMvdCreateDefaultApvMap;

//#pragma link C++ class PndHammingDecoder;
#pragma link C++ class PndCRCCalculator;

//#pragma link C++ class PndMvdReadInTBData;
//#pragma link C++ class PndMvdReadInTBDataTask;
//#pragma link C++ class PndMapSorterTask;
//#pragma link C++ class PndMapSorter;
//#pragma link C++ class PndMvdTopix4RingSorterTask;
//#pragma link C++ class PndMvdTopix4RingSorter;
//#pragma link C++ class PndTimeOffsetFinderTask;
//#pragma link C++ class PndTSCorrectorTask;
//#pragma link C++ class PndStraightLineTrackFinderTask;
//#pragma link C++ class PndXYResidualTask;
//#pragma link C++ class PndPosCorrectorTask;

#endif
