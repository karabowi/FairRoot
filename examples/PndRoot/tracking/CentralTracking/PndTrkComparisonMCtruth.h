//****************************************************************************
//*                   This file is part of PandaRoot.                        *
//*                                                                          *
//*            PandaRoot is distributed under the terms of the               *
//*              GNU General Public License (GPL) version 3,                 *
//*                 copied verbatim in the file "LICENSE".                   *
//*                                                                          *
//*  Copyright (C) 2006 - 2024 FAIR GmbH and copyright holders of PandaRoot  *
//*     The copyright holders are listed in the file "COPYRIGHTHOLDERS".     *
//*               The authors are listed in the file "AUTHORS".              *
//****************************************************************************

#ifndef PndTrkComparisonMCtruth_H
#define PndTrkComparisonMCtruth_H 1

// #include "FairRootManager.h"
#include "PndTrkVectors.h"
#include "PndMCTrack.h"
// Root includes
#include "TROOT.h"
#include "TClonesArray.h"

struct PndTrkComparisonMCtruth_io_Data {
  Double_t Bfield;
  Short_t *Charge;
  Double_t Cvel;
  Short_t *daTrackFoundaTrackMC;
  Double_t DIMENSIONSciTil;
  Double_t Errorsqpixel;
  Double_t Errorsqstrip;
  Double_t *FI0;
  TClonesArray *fMCTrackArray;
  TClonesArray *fMvdMCPointArray;
  Short_t fSciTilMaxNumber;
  TClonesArray *fSciTHitArray;
  TClonesArray *fSciTPointArray;
  TClonesArray *fSttPointArray;
  FILE *HANDLE;
  FILE *HANDLE2;
  Double_t *info;
  int istampa;
  int IVOLTE;
  Double_t *KAPPA;
  bool *keepit;
  bool *InclusionListStt;
  Short_t *ListMvdPixelHitsinTrack;
  Short_t *ListMvdStripHitsinTrack;
  Short_t *ListSciTilHitsinTrack;
  Short_t *ListSttParHitsinTrack;
  Short_t *ListSttSkewHitsinTrack;
  Short_t *ListTrackCandHit;
  Short_t *ListTrackCandHitType;
  int MAXMCTRACKS;
  int MAXMVDPIXELHITS;
  int MAXMVDPIXELHITSINTRACK;
  int Maxmvdmcpoints;
  int MAXMVDSTRIPHITS;
  int MAXMVDSTRIPHITSINTRACK;
  int MAXSCITILHITS;
  int MAXSCITILHITSINTRACK;
  int MAXSTTHITS;
  int maxstthitsintrack;
  int MAXTRACKSPEREVENT;
  Short_t *MCMvdPixelAloneList;
  Short_t *MCMvdStripAloneList;
  Short_t *MCParalAloneList;
  Short_t *MCSkewAloneList;
  Double_t *MCSkewAloneX; // dimension : [MAXSTTHITS]
  Double_t *MCSkewAloneY; // dimension : [MAXSTTHITS]
  Short_t *MvdPixelCommonList;
  Short_t *MvdPixelSpuriList;
  Short_t *MCSciTilAloneList; // equivalent to a matrix
                              //  [nTotalCandidates][nSciTilHits];
  Short_t *MvdStripCommonList;
  Short_t *MvdStripSpuriList;
  Short_t *nHitsInMCTrack;
  Short_t *nHitsInSciTile;
  Short_t *nMCMvdPixelAlone;
  Short_t *nMCMvdStripAlone;
  Short_t *nMCParalAlone;
  Short_t *nMCSciTilAlone;
  Short_t *nMCSkewAlone;
  Short_t *nMvdPixelCommon;
  Short_t *nMvdPixelHitsinTrack;
  Short_t *nMvdStripHitsinTrack;
  Short_t nMvdPixelHit;
  Short_t *nMvdPixelSpuriinTrack;
  Short_t *nMvdStripCommon;
  Short_t nMvdStripHit;
  Short_t *nMvdStripSpuriinTrack;
  Short_t *nParalCommon;
  Short_t *nSciTilCommon;
  Short_t nSciTilHits;
  Short_t *nSciTilHitsinTrack;
  Short_t *nSciTilSpuriinTrack;
  Short_t *nSkewCommon;
  Short_t *nSkewHitsInMCTrack;
  Short_t *nSpuriParinTrack;
  Short_t *nSpuriSkewinTrack;
  Int_t nSttHit;
  Short_t *nSttParHitsinTrack;
  Short_t *nSttSkewHitsinTrack;
  Short_t nTotalCandidates;
  Short_t *OriginalSciTilList;
  Double_t *Ox;
  Double_t *Oy;
  Short_t *ParalCommonList;
  Short_t *ParSpuriList;
  Double_t *R;
  Double_t *refindexMvdPixel;
  Double_t *refindexMvdStrip;
  Short_t *resultFitSZagain;
  Short_t *SciTilCommonList; // equivalent to a matrix
                             // [nTotalCandidates][MAXSCITILHITSINTRACK];
  Short_t *SciTilSpuriList;  // equivalent to a matrix
                             //  [nTotalCandidates][MAXSCITILHITSINTRACK];
  Short_t *SkewCommonList;
  Short_t *SkewSpuriList;
  bool *SttSZfit;
  Double_t *XMvdPixel;
  Double_t *XMvdStrip;
  Double_t *XSciTilCenter;
  Double_t *YMvdPixel;
  Double_t *YMvdStrip;
  Double_t *YSciTilCenter;
  Double_t *ZMvdPixel;
  Double_t *ZMvdStrip;
  Double_t *ZSciTilCenter;
};

// inizio cambio_in_perl

class PndTrkComparisonMCtruth {

 public:
  /** Default constructor **/
  PndTrkComparisonMCtruth(){};
  /** Destructor **/
  virtual ~PndTrkComparisonMCtruth(){};

  void AssociateFoundTrackstoMCquater(Double_t BFIELD, Double_t CVEL, Vec<Short_t> *daTrackFoundaTrackMC, TClonesArray *fMCTrackArray, Vec<Int_t> *FromPixeltoMCTrack,
                                      Vec<Int_t> *FromStriptoMCTrack, Vec<int> *FromSciTiltoMCTrackList,
                                      //  dimension   nSciTilHits*nMCTracks, equivalent to a matrix
                                      //  of dimension  [nSciTilHits][nMCTracks].
                                      Vec<bool> *keepit, Vec<Double_t> *info, Vec<Short_t> *ListSttParHitsinTrack, Vec<Short_t> *ListMvdPixelHitsinTrack,
                                      Vec<Short_t> *ListSciTilHitsinTrack, Vec<Short_t> *ListSttSkewHitsinTrack, Vec<Short_t> *ListMvdStripHitsinTrack, int MAXMVDPIXELHITSINTRACK,
                                      int MAXMVDSTRIPHITSINTRACK, int MAXSCITILHITSINTRACK, int MAXSTTHITSINTRACK, Vec<int> *nFromSciTiltoMCTrack, Vec<Short_t> *nSttParHitsinTrack,
                                      int nMCTracks, Vec<Short_t> *nMvdPixelHitsinTrack, Short_t nSciTilHits, Vec<Short_t> *nSciTilHitsinTrack,
                                      // dimension [MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK]
                                      Vec<Short_t> *nSttSkewHitsinTrack, Vec<Short_t> *nMvdStripHitsinTrack, Short_t nTracksFoundSoFar, Vec<Double_t> *Ox, Vec<Double_t> *Oy,
                                      Vec<Double_t> *R, Vec<Double_t> *X1, Vec<Double_t> *Y1, Vec<Double_t> *X2, Vec<Double_t> *Y2, Vec<Double_t> *X3, Vec<Double_t> *Y3, int IVOLTE

  );

  int ComparisonwithMC(PndTrkComparisonMCtruth_io_Data ioData);

  Double_t FindDistance(Double_t Oxx, //  center from wich distance is calculated
                        Double_t Oyy, //  center from wich distance is calculated
                        Double_t Rr, Double_t tanlow, Double_t tanmid, Double_t tanup,
                        Double_t alfa, //  intersection circumference parameter
                        Double_t beta, //  intersection circumference parameter
                        Double_t gamma //  intersection circumference parameter
  );
  void getMCInfo(Double_t BFIELD, Double_t CVEL, Double_t *Cx, Double_t *Cy, TClonesArray *fMCTrackArray, Int_t MCTrack, Double_t *Rr);

  void MvdMatchedSpurioustoTrackCand(Vec<Short_t> *daTrackFoundaTrackMC, Vec<Int_t> *FromPixeltoMCTrack, Vec<Int_t> *FromStriptoMCTrack, Vec<bool> *keepit,
                                     Vec<Short_t> *ListMvdPixelHitsinTrack, Vec<Short_t> *ListMvdStripHitsinTrack, int MAXMVDPIXELHITSINTRACK, int MAXMVDSTRIPHITSINTRACK,
                                     Short_t nMvdPixelHit, Short_t nMvdStripHit, Vec<Short_t> *nMvdPixelHitsinTrack, Vec<Short_t> *nMvdStripHitsinTrack,
                                     Short_t nSttTrackCand, // input

                                     Vec<Short_t> *nMvdPixelCommon, Vec<Short_t> *MvdPixelCommonList, Vec<Short_t> *nMvdPixelSpuriinTrack, Vec<Short_t> *MvdPixelSpuriList,
                                     Vec<Short_t> *nMCMvdPixelAlone, Vec<Short_t> *MCMvdPixelAloneList,

                                     Vec<Short_t> *nMvdStripCommon, Vec<Short_t> *MvdStripCommonList, Vec<Short_t> *nMvdStripSpuriinTrack, Vec<Short_t> *MvdStripSpuriList,
                                     Vec<Short_t> *nMCMvdStripAlone, Vec<Short_t> *MCMvdStripAloneList);

  void MvdMatchtoMC(Double_t ERRORSQPIXEL, Double_t ERRORSQSTRIP, TClonesArray *fMvdMCPointArray, Short_t nMvdMCPoint, int istampa, int IVOLTE, Short_t nMvdPixelHit,
                    Short_t nMvdStripHit, Vec<Double_t> *refindexMvdPixel, Vec<Double_t> *refindexMvdStrip, Vec<Double_t> *XMvdPixel, Vec<Double_t> *XMvdStrip,
                    Vec<Double_t> *YMvdPixel, Vec<Double_t> *YMvdStrip, Vec<Double_t> *ZMvdPixel, Vec<Double_t> *ZMvdStrip, Vec<Int_t> *FromPixeltoMCTrack,
                    Vec<Int_t> *FromStriptoMCTrack);

  void SciTilMatchtoMC(Double_t BFIELD, Double_t CVEL, Double_t DIMENSIONSCITIL, TClonesArray *fMCTrackArray, Vec<int> *FromSciTiltoMCTrackList, TClonesArray *fSciTHitArray,
                       Short_t fSciTilMaxNumber, TClonesArray *fSciTPointArray, Vec<int> *nFromSciTiltoMCTrack, Short_t *nHitsInSciTile, int nMCTracks, Short_t nSciTilHits,
                       Short_t *OriginalSciTilList, Vec<Double_t> *XSciTilCenter, Vec<Double_t> *YSciTilCenter, Vec<Double_t> *ZSciTilCenter

  );

  void SciTilMatchedSpurioustoTrackCand(Vec<Short_t> *daTrackFoundaTrackMC, Vec<int> *FromSciTiltoMCTrackList,
                                        //  of dimension  [nSciTilHits][nMCTracks]
                                        Vec<bool> *keepit, Vec<Short_t> *ListSciTilHitsinTrack,
                                        // [MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK]
                                        int MAXSCITILHITSINTRACK,   // input
                                        Short_t *MCSciTilAloneList, // output; equivalent to a matrix of dimension
                                                                    // [MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK]
                                        Vec<int> *nFromSciTiltoMCTrack,
                                        Short_t *nMCSciTilAlone, // output
                                        int nMCTracks,           // input
                                        Short_t nSciTilHits,     // input
                                        Vec<Short_t> *nSciTilHitsinTrack,
                                        Short_t *nSciTilCommon,       // output
                                        Short_t *nSciTilSpuriinTrack, // output
                                        Short_t nSttTrackCand,        // input
                                        Short_t *SciTilCommonList,    // output; equivalent to a matrix of dimension
                                                                      // [MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK]
                                        Short_t *SciTilSpuriList      // output; equivalent to a matrix of dimension
                                                                      // [MAXTRACKSPEREVENT][MAXSCITILHITSINTRACK].
  );

  void stampaMCTracks(Double_t BFIELD, Double_t CVEL, TClonesArray *fMCTrackArray, int nMCTracks);

  void SttMatchedSpurious(Vec<Short_t> *daTrackFoundaTrackMC, Vec<bool> *InclusionListStt, Vec<Double_t> *info, Vec<bool> *keepit, int MAXSTTHITS, int MAXSTTHITSINTRACK,
                          int MAXTRACKSPEREVENT, Vec<Short_t> *ListSttParHitsinTrack, Vec<Short_t> *ListSttSkewHitsinTrack, Vec<Short_t> *MCParalAloneList,
                          Vec<Short_t> *MCSkewAloneList, Vec<Short_t> *nHitsInMCTrack, Vec<Short_t> *nSttParHitsinTrack, Vec<Short_t> *nMCParalAlone, Vec<Short_t> *nMCSkewAlone,
                          Vec<Short_t> *nParalCommon, Vec<Short_t> *nSkewCommon, Vec<Short_t> *nSkewHitsInMCTrack, Vec<Short_t> *nSttSkewHitsinTrack,
                          Vec<Short_t> *nSpuriParinTrack, Vec<Short_t> *nSpuriSkewinTrack, Short_t nSttHits,
                          Short_t nTracksFoundSoFar, // those found by PR
                          Vec<Short_t> *ParalCommonList, Vec<Short_t> *ParSpuriList, Vec<Short_t> *SkewCommonList, Vec<Short_t> *SkewSpuriList

  );

  // fine cambio_in_perl

  ClassDef(PndTrkComparisonMCtruth, 1);
};

#endif
