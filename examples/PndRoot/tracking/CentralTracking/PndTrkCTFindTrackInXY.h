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

#ifndef PndTrkCTFindTrackInXY_H
#define PndTrkCTFindTrackInXY_H 1

// Root includes
#include "TROOT.h"
struct FindTrackInXYProjection_InputData {
  Double_t *ALFA;
  Double_t apotemamaxskewstraw;
  Double_t *BETA;
  Short_t *Charge;
  Double_t deltanr;
  Double_t dimensionscitil;
  Short_t *FiConformalIndex;
  Double_t *Fi_final_helix_referenceframe;
  Double_t *Fi_initial_helix_referenceframe;
  Double_t *Fi_low_limit;
  Double_t *Fi_up_limit;
  Double_t *GAMMA;
  Short_t *HitsinBoxConf;
  int icounter; // counter useful for debugging;
  Short_t iHit; // seed hit; it is negative for SciTil Hits.
  bool *InclusionListStt;
  bool *InclusionListSciTil;
  Double_t (*info)[7];
  Double_t (*infoparalConformal)[5];
  Short_t *ListHitsinTrack;
  Short_t *ListSciTilHitsinTrack;
  Short_t *ListSttParHits;
  Short_t maxhitsinfit;
  Short_t maxscitilhitsintrack;
  Short_t maxstthits;
  Short_t maxstthitsintrack;
  Short_t minimumhitspertrack;
  Short_t minouterhitspertrack;
  Short_t *nBoxConf;
  Short_t nFicell; // Fi cell of the seed hit;
  Short_t nfidivconformal;
  Short_t *nHitsinTrack;
  Short_t nRcell; // R cell of the seed hit;
  Short_t nrdivconformal;
  Short_t nSciTilHits;
  Short_t *nSciTilHitsinTrack;
  Int_t nsttparhit;
  Double_t *Oxx;
  Double_t *Oyy;
  void *posizSciT;
  Double_t posizSciTilx;
  Double_t posizSciTily;
  Double_t *radiaConf;
  Short_t *RConformalIndex;
  Double_t *Rr;
  Double_t rstrawdetectormax;
  Double_t rstrawdetectormin;
  Double_t strawradius;
  Double_t *S_SciTilHitsinTrack;
  Double_t *trajectory_vertex;
  bool *TypeConf;
  Double_t *U;
  Double_t *V;
  bool YesSciTil;
};

class PndTrkCTFindTrackInXY : public TObject {

 public:
  /** Default constructor **/
  PndTrkCTFindTrackInXY(){};

  /** Destructor **/
  ~PndTrkCTFindTrackInXY(){};

  //----------------

  bool AcceptHitsConformal(Double_t distance,
                           Double_t DriftConfR, // drift radius in conformal space
                           Double_t StrawConfR  // straw radius in conformal space
  );

  Short_t AssociateSciTilHit(Double_t dimensionscitil,
                             Double_t *esse, // output, list of  S of the SciTil hits associated.
                             bool *InclusionListSciTil,
                             Short_t *List, // output, list of SciTil hits associated (max. 2);
                             Short_t maxscitilhitsintrack, Short_t nSciTilHits, Double_t Oxx, Double_t Oyy, Double_t posizSciTil[][3], Double_t Rr);

  void FindCharge(Double_t oX, Double_t oY, Short_t nHits, Double_t *X, Double_t *Y, Short_t *Charge);

  bool FindTrackInXYProjection(struct FindTrackInXYProjection_InputData *inputdata);

  Short_t FindTrackPatterninBoxConformal(Short_t *FiConformalIndex, Short_t *HitsinBoxConformal,
                                         Short_t ihit, // seed hit;
                                         bool *InclusionListStt, Double_t info[][7], Short_t *ListHitsinTrack, Short_t *ListSttParHits, Short_t maxstthitsintracks,
                                         Short_t maxstthits, Short_t minimumhitspertrack, Short_t *nBoxConformal, Short_t nfidivconformal,
                                         Short_t nFicell, // Fi cell of the seed hit;
                                         Short_t NFiCELLDISTANCE, Short_t Nparal,
                                         Short_t nRcell, // R cell of the seed hit;
                                         // can be negative beacuse of SciTil hits;
                                         Short_t NRCELLDISTANCE, Short_t *RConformalIndex, Short_t nrdivconformal);

  Short_t FindTrackPatterninBoxConformalSpecial(Short_t *FiConformalIndex, Short_t *HitsinBoxConformal, bool *InclusionListStt, Double_t info[][7], Short_t iSeed,
                                                Short_t *ListHitsinTrackinWhichToSearch, Short_t *ListSttParHits, Short_t maxstthits, Short_t minimumhitspertrack,
                                                Short_t *nBoxConformal, Short_t NFiCELLDISTANCE, Short_t nfidivconformal, Short_t Nparal, Short_t NparallelToSearch,
                                                Short_t NRCELLDISTANCE, Short_t nrdivconformal, Short_t *OutputListHitsinTrack, Short_t *RConformalIndex

  );

  Short_t FindTrackStrictCollection(Short_t *FiConformalIndex, bool *InclusionListStt,
                                    //  seed track (original notation) as far as the Fi angle is concerned
                                    Short_t iSeed, Short_t *ListHitsinTrackinWhichToSearch, Short_t MAXSTTHITS, Short_t NFiCELLDISTANCE, Short_t nfidivconformal,
                                    //  n. of hits to search in ListHitsinTrackinWhichToSearch
                                    Short_t NParallelToSearch, Short_t *OutputListHitsinTrack);

  void OrderingParallel(Short_t Charge, Double_t *Fi_initial_helix_referenceframe, Double_t *Fi_final_helix_referenceframe, Double_t info[][7], Short_t *ListParallelHits,
                        Short_t nParallelHits, Double_t oX, Double_t oY, Double_t *U, Double_t *V);

  Short_t TrkAssociatedParallelHitsToHelixQuater(Short_t *auxListHitsinTrack, Double_t deltanr, Short_t *FiConformalIndex, Short_t *HitsinBoxConformal, bool *InclusionListStt,
                                                 Double_t info[][7], Double_t infoparalConformal[][5], Short_t *ListHitsinTrack, Double_t m, Short_t MAXSTTHITS,
                                                 Short_t *nBoxConformal, Short_t nfidivconformal, Short_t nHitsinTrack, Int_t NhitsParallel, Short_t nrdivconformal, Double_t Oxx,
                                                 Double_t Oyy, Double_t q, Double_t *radiaConf, Short_t *RConformalIndex, Double_t Rr, Double_t rstrawdetectormin, Short_t Status,
                                                 Double_t strawradius);

  Short_t TrkAssociatedParallelHitsToHelix5(Short_t *auxListHitsinTrack, bool *InclusionListStt, Double_t Fi_low, Double_t Fi_up, Double_t info[][7], Short_t *ListSttParHits,
                                            Int_t NhitsParallel, Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t strawradius);

  ClassDef(PndTrkCTFindTrackInXY, 1);
};

#endif
