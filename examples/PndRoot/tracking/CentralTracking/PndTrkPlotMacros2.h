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

#ifndef PndTrkPlotMacros2_H
#define PndTrkPlotMacros2_H 1

#include "PndTrkVectors.h"

// Root includes
#include "TClonesArray.h"
#include "TROOT.h"

struct PndTrkPlotMacros2_InputData {
  Double_t apotemastrawdetectormin;
  Double_t apotemamaxinnerparstraw;
  Double_t apotemamaxskewstraw;
  Double_t apotemaminouterparstraw;
  Double_t apotemaminskewstraw;
  Double_t bfield;
  Short_t *Charge;
  Double_t cvel;
  Short_t *daTrackFoundaTrackMC;
  Double_t dimensionscitil;
  bool doMcComparison;
  Double_t *FI0;
  TClonesArray *fMCTrackArray;
  TClonesArray *fSttPointArray;
  Double_t *info;
  int IVOLTE;
  Double_t *KAPPA;
  bool *keepit;
  bool *InclusionListSciTil;
  bool *InclusionListStt;
  int istampa;
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
  int MAXMVDSTRIPHITS;
  int MAXMVDSTRIPHITSINTRACK;
  int MAXSCITILHITSINTRACK;
  int MAXSCITILHITS;
  int MAXSTTHITS;
  int maxstthitsintrack;
  int MAXTRACKSPEREVENT;
  Short_t *MCMvdPixelAloneList;
  Short_t *MCMvdStripAloneList;
  Short_t *MCParalAloneList;

  Short_t *MCSciTilAloneList; // equivalent to a matrix
                              //  [nTotalCandidates][nSciTilHits]

  Short_t *MCSkewAloneList;
  Double_t *MCSkewAloneX;
  Double_t *MCSkewAloneY;
  Short_t *MvdPixelCommonList;
  Short_t *MvdPixelSpuriList;
  Short_t *MvdStripCommonList;
  Short_t *MvdStripSpuriList;
  Short_t NFIDIVCONFORMAL;
  Short_t *nMCMvdPixelAlone;
  Short_t *nMCMvdStripAlone;
  Short_t *nMCParalAlone;

  Short_t *nMCSciTilAlone;

  Short_t *nMCSkewAlone;
  Short_t nMCTracks;
  Short_t *nMvdPixelCommon;
  Short_t nMvdPixelHit;
  Short_t *nMvdPixelHitsinTrack;
  Short_t *nMvdPixelSpuriinTrack;
  Short_t *nMvdStripCommon;
  Short_t nMvdStripHit;
  Short_t *nMvdStripHitsinTrack;
  Short_t *nMvdStripSpuriinTrack;
  Short_t *nParalCommon;
  Short_t NRDIVCONFORMAL;
  Short_t *nSciTilCommon;

  Short_t nSciTilHits;
  Short_t *nSciTilHitsinTrack;

  Short_t *nSciTilSpuriinTrack;

  Short_t *nSkewCommon;
  Short_t *nSpuriParinTrack;
  Int_t nSttHit;
  Int_t nSttParHit;
  Short_t *nSttParHitsinTrack;
  Int_t nSttSkewHit;
  Short_t *nSttSkewHitsinTrack;
  Short_t nTotalCandidates;
  Short_t *nTrackCandHit;
  Short_t number_straws;
  Double_t *Ox;
  Double_t *Oy;
  Short_t *ParalCommonList;
  Short_t *ParSpuriList;
  Double_t *posizSciTil;
  Double_t *R;
  Double_t *radiaConf;
  Double_t rstrawdetectormax;
  Double_t *SchosenSkew;

  Short_t *SciTilCommonList; // equivalent to a matrix
                             // [nTotalCandidates][MAXSCITILHITSINTRACK]
  Short_t *SciTilSpuriList;  // equivalent to a matrix
                             // [nTotalCandidates][MAXSCITILHITSINTRACK]

  Double_t *sigmaXMvdPixel;
  Double_t *sigmaXMvdStrip;
  Double_t *sigmaYMvdPixel;
  Double_t *sigmaYMvdStrip;
  Short_t *SkewCommonList;
  TClonesArray *SttTubeArray;
  Short_t *StrawCode;
  Short_t *StrawCode2;
  Double_t verticalgap;
  Double_t *XMvdPixel;
  Double_t *XMvdStrip;
  Double_t *YMvdPixel;
  Double_t *YMvdStrip;
  Double_t *WDX;
  Double_t *WDY;
  Double_t *WDZ;
  Double_t *ZMvdPixel;
  Double_t *ZMvdStrip;
};

// inizio cambio_in_perl

class PndTrkPlotMacros2 : public TObject {

 public:
  /** Default constructor **/
  PndTrkPlotMacros2(){};
  /** Destructor **/
  ~PndTrkPlotMacros2(){};

  void disegnaAssiXY(FILE *MACRO, double xmin, double xmax, double ymin, double ymax);

  void disegnaSciTilHit(int colorcode, // goes in the SetColor function of root;
                        Double_t DIMENSIONSCITIL, FILE *MACRO, double posx, double posy, int ScitilHit,
                        int tipo // if 0 then SciTil draw in XY; if 1 then SciTil draw in SZ; else
                                 // SciTil draw in UV.
  );

  void DrawBiHexagonInMacro(Double_t vgap, FILE *MACRO, Double_t Ami, Double_t Ama, Short_t color, char *name);

  void DrawHexagonCircleInMacro(Double_t GAP, FILE *MACRO, Double_t ApotemaMin, Double_t Rma, Short_t color, char *name);

  void SttInfoXYZParal(Vec<Double_t> *info, Short_t infopar, Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t KAPPA, Double_t FI0, Short_t Charge, Double_t *Posiz);

  void WriteAllMacros(PndTrkPlotMacros2_InputData In_Put);

  void WriteMacroAllHitsRestanti(Double_t APOTEMAMAXINNERPARSTRAW, Double_t APOTEMAMAXSKEWSTRAW, Double_t APOTEMAMINOUTERPARSTRAW, Double_t APOTEMAMINSKEWSTRAW,
                                 Vec<bool> *InclusionListSciTil, Vec<bool> *InclusionListStt, Vec<Double_t> *info, int IVOLTE, Vec<bool> *keepit, Vec<Short_t> *ListTrackCandHit,
                                 Vec<Short_t> *ListTrackCandHitType, int MAXMVDPIXELHITS, int MAXMVDPIXELHITSINTRACK, int MAXMVDSTRIPHITS, int MAXMVDSTRIPHITSINTRACK,
                                 int MAXSCITILHITSINTRACK, int MAXSTTHITS, int MAXSTTHITSINTRACK, int MAXTRACKSPEREVENT, Short_t nMvdPixelHit, Short_t nMvdStripHit,
                                 Short_t nSciTilHit, Short_t nSttHit, Short_t nSttParHit, Short_t nSttSkewHit, Short_t nSttTrackCand, Vec<Short_t> *nTrackCandHit,
                                 Vec<Double_t> *posizSciTil, Double_t RSTRAWDETECTORMAX, Double_t APOTEMASTRAWDETECTORMIN, Double_t VERTICALGAP, Vec<Double_t> *XMvdPixel,
                                 Vec<Double_t> *XMvdStrip, Vec<Double_t> *YMvdPixel, Vec<Double_t> *YMvdStrip

  );

  void WriteMacroParallelHitsGeneral(Double_t APOTEMAMAXINNERPARSTRAW, Double_t APOTEMAMAXSKEWSTRAW, Double_t APOTEMAMINOUTERPARSTRAW, Double_t APOTEMAMINSKEWSTRAW,
                                     Double_t BFIELD, Double_t CVEL, Double_t DIMENSIONSCITIL, bool doMcComparison, TClonesArray *fMCTrackArray, Int_t Nhits, Vec<Double_t> *info,
                                     PndTrkPlotMacros2_InputData In_Put, int IVOLTE, Short_t nMCTracks, Short_t nMvdPixelHit, Short_t nMvdStripHit, Short_t nSciTilHits,
                                     Short_t nTracksFoundSoFar, Vec<bool> *keepit, Vec<Double_t> *FI0, Vec<Double_t> *Ox, Vec<Double_t> *Oy, Vec<Double_t> *posizSciTil,
                                     Vec<Double_t> *primoangolo, Vec<Double_t> *R, Double_t RSTRAWDETECTORMAX, Double_t APOTEMASTRAWDETECTORMIN, Vec<Double_t> *sigmaXMvdPixel,
                                     Vec<Double_t> *sigmaXMvdStrip, Vec<Double_t> *sigmaYMvdPixel, Vec<Double_t> *sigmaYMvdStrip, Vec<Double_t> *ultimoangolo, Double_t VERTICALGAP,
                                     Vec<Double_t> *XMvdPixel, Vec<Double_t> *XMvdStrip, Vec<Double_t> *YMvdPixel, Vec<Double_t> *YMvdStrip);

  void WriteMacroParallelHitsGeneralConformalwithMC(Double_t APOTEMAMAXINNERPARSTRAW, Double_t APOTEMAMAXSKEWSTRAW, Double_t APOTEMAMINOUTERPARSTRAW, Double_t APOTEMAMINSKEWSTRAW,
                                                    Double_t BFIELD, Double_t CVEL, Double_t DIMENSIONSCITIL, bool doMcComparison, TClonesArray *fMCTrackArray, Int_t Nhits,
                                                    Vec<Double_t> *info, PndTrkPlotMacros2_InputData In_Put, int IVOLTE, Short_t nMCTracks, Short_t nMvdPixelHit,
                                                    Short_t nMvdStripHit, Short_t nSciTilHits, Short_t nTracksFoundSoFar, Vec<bool> *keepit, Vec<Double_t> *FI0, Vec<Double_t> *Ox,
                                                    Vec<Double_t> *Oy, Vec<Double_t> *posizSciTil, Vec<Double_t> *primoangolo, Vec<Double_t> *R, Double_t RSTRAWDETECTORMAX,
                                                    Double_t APOTEMASTRAWDETECTORMIN, Vec<Double_t> *sigmaXMvdPixel, Vec<Double_t> *sigmaXMvdStrip, Vec<Double_t> *sigmaYMvdPixel,
                                                    Vec<Double_t> *sigmaYMvdStrip, Vec<Double_t> *ultimoangolo, Double_t VERTICALGAP, Vec<Double_t> *XMvdPixel,
                                                    Vec<Double_t> *XMvdStrip, Vec<Double_t> *YMvdPixel, Vec<Double_t> *YMvdStrip);

  void WriteMacroParallel_MvdHitsGeneralConformalwithMC(Double_t APOTEMAMAXINNERPARSTRAW, Double_t APOTEMAMAXSKEWSTRAW, Double_t APOTEMAMINOUTERPARSTRAW,
                                                        Double_t APOTEMAMINSKEWSTRAW, Double_t BFIELD, Double_t CVEL, Double_t DIMENSIONSCITIL, bool doMcComparison,
                                                        TClonesArray *fMCTrackArray, Int_t Nhits, Vec<Double_t> *info, PndTrkPlotMacros2_InputData In_Put, int IVOLTE,
                                                        Short_t nMCTracks, Short_t nMvdPixelHit, Short_t nMvdStripHit, Short_t nSciTilHits, Short_t nTracksFoundSoFar,
                                                        Vec<bool> *keepit, Vec<Double_t> *FI0, Vec<Double_t> *Ox, Vec<Double_t> *Oy, Vec<Double_t> *posizSciTil,
                                                        Vec<Double_t> *primoangolo, Vec<Double_t> *R, Double_t RSTRAWDETECTORMAX, Double_t APOTEMASTRAWDETECTORMIN,
                                                        Vec<Double_t> *sigmaXMvdPixel, Vec<Double_t> *sigmaXMvdStrip, Vec<Double_t> *sigmaYMvdPixel, Vec<Double_t> *sigmaYMvdStrip,
                                                        Vec<Double_t> *ultimoangolo, Double_t VERTICALGAP, Vec<Double_t> *XMvdPixel, Vec<Double_t> *XMvdStrip,
                                                        Vec<Double_t> *YMvdPixel, Vec<Double_t> *YMvdStrip);

  void WriteMacroSkewAssociatedHitswithMC(Double_t *ESSE, Double_t *ESSEalone, PndTrkPlotMacros2_InputData In_Put,
                                          int iNome, // questo e' per il nome delle Macro solamente.
                                          int iTrack);

  void WriteMacroSkewAssociatedHitswithMC_Degree(Double_t *ESSE, Double_t *ESSEalone, PndTrkPlotMacros2_InputData In_Put,
                                                 int iNome, // questo e' per il nome delle Macro solamente.
                                                 int iTrack);

  void WriteMacroSttParallelAssociatedHitsandMvdwithMC(PndTrkPlotMacros2_InputData In_Put, Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t primoangolo, Double_t ultimoangolo,
                                                       Short_t Nhits, int iTrack, int iNome, Short_t daSttTrackaMCTrack, Short_t nMvdPixelHitsAssociatedToSttTra,
                                                       Short_t nMvdStripHitsAssociatedToSttTra, Short_t nSkewHitsinTrack);
  // fine cambio_in_perl  ;

  void WriteMacroSttParallel(PndTrkPlotMacros2_InputData In_Put);

  void WriteMacroSttParallelExternal(PndTrkPlotMacros2_InputData In_Put);

  ClassDef(PndTrkPlotMacros2, 1);
};

#endif
