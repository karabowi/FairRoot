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

#ifndef PndTrkCTFindTrackInXY2_H
#define PndTrkCTFindTrackInXY2_H 1

#include "PndTrkVectors.h"
// Root includes
#include "TROOT.h"

struct FindTrackInXYProjection2_InputOutputData {
  // inputs;
  Double_t apotemastrawdetectormin;
  Double_t *Cosine; // for the Legiandre fit;
  Short_t legiandre_nthetadiv;
  Short_t legiandre_nradiusdiv;
  bool *InclusionListStt;
  Double_t (*info)[7];
  Short_t maxhitsinfit;
  Short_t maxmvdpixelhitsintrack;
  Short_t maxmvdstriphitsintrack;
  bool *Mvdhits;
  Short_t nMvdPixelHit;
  Short_t nMvdStripHit;
  Short_t number_straws; // this is the number of all straws in the Stt detector;
  Short_t *nParContiguous;
  Short_t (*ListParContiguous)[6]; // this is the list of contiguous TubeID ;
  Double_t r_stt_inner_par_max;    // r_stt_inner_par_max ==> radius of the circumscribed
                                   // circumference to the
                                   //; outer hexagon defining THE INNER axial Stt straw region;
  Double_t *Sinus;                 // for the Legiandre fit;
  Short_t *StrawCode;
  Short_t *StrawCode2;
  Short_t *TubeID;     // TubeID[i] =  Tube ID corresponding to i-th Stt hit;
  Short_t *SttStrawOn; //  SttStrawOn[i] >= 0 --> it is the Stt hit number corresponding to Stt
                       // i-th Tube ID; tSttStrawOn[i] == -1 --> i-th Stt straw NOT hit;
  Double_t thetamax;   // input, maximum of Theta range (usually 2PI radians);
  Double_t thetamin;   // input, minimum of Theta range (usually 0 radians);
  Double_t *XMvdPixel;
  Double_t *XMvdStrip;
  Double_t *YMvdPixel;
  Double_t *YMvdStrip;

  // outputs;
  Double_t *ALFA;
  Double_t *BETA;
  Short_t *Charge;
  Double_t *Fi_initial_helix_referenceframe;
  Double_t *Fi_low_limit;
  Double_t *Fi_up_limit;
  Double_t *GAMMA;
  Short_t *ListHitsinTrack;
  Short_t *ListMvdPixelHitsinTrack;
  Short_t *ListMvdStripHitsinTrack;
  Short_t *nMvdPixelHitsinTrack;
  Short_t *nMvdStripHitsinTrack;
  Short_t maxstthitsintrack;
  Short_t minimumhitspertrack;
  Short_t *nHitsinTrack;
  Double_t *Oxx;
  Double_t *Oyy;
  Double_t *Rr;
  Double_t *xTube;
  Double_t *yTube;
  Double_t *zTube;
  Double_t *xxyyTube;

  //---

  Double_t apotemamaxskewstraw;
  Double_t deltanr;
  Double_t dimensionscitil;
  Short_t *FiConformalIndex;
  Double_t *Fi_final_helix_referenceframe;
  Short_t *HitsinBoxConf;
  int icounter; // counter useful for debugging;
  bool *InclusionListSciTil;
  Double_t (*infoparalConformal)[5];
  Short_t *ListSciTilHitsinTrack;
  Short_t *ListSttParHits;
  Short_t maxscitilhitsintrack;
  Short_t maxstthits;
  Short_t minouterhitspertrack;
  Short_t *nBoxConf;
  Short_t nFicell; // Fi cell of the seed hit;
  Short_t nfidivconformal;
  Short_t nRcell; // R cell of the seed hit;
  Short_t nrdivconformal;
  Short_t nSciTilHits;
  Short_t *nSciTilHitsinTrack;
  Int_t nsttparhit;
  void *posizSciT;
  Double_t posizSciTilx;
  Double_t posizSciTily;
  Double_t *radiaConf;
  Short_t *RConformalIndex;
  Double_t rstrawdetectormax;
  Double_t strawradius;
  Double_t *S_SciTilHitsinTrack;
  Double_t *trajectory_vertex;
  bool *TypeConf;
  Double_t *U;
  Double_t *V;
  bool YesSciTil;
};

class PndTrkCTFindTrackInXY2 : public TObject {

 public:
  /** Default constructor **/
  PndTrkCTFindTrackInXY2(){};

  /** Destructor **/
  ~PndTrkCTFindTrackInXY2(){};

  //----------------

  void AddMvdHitsToSttTracks(Double_t delta,                 // input;
                             Double_t highqualitycut,        // input;
                             Double_t FiRangeMvdLow,         // input;
                             Double_t FiRangeMvdUp,          // input;
                             Short_t maxmvdpixelhitsintrack, // input;
                             Short_t maxmvdstriphitsintrack, // input;
                             Short_t nMvdPixelHit,           // input;
                             Short_t nMvdStripHit,           // input;
                             Double_t Ox,                    // input;
                             Double_t Oy,                    // input;
                             Double_t R,                     // input;
                             Double_t *XMvdPixel,            // input;
                             Double_t *XMvdStrip,            // input;
                             Double_t *YMvdPixel,            // input;
                             Double_t *YMvdStrip,            // input;

                             Short_t &nPixelHitsinTrack,    // output
                             Short_t *ListPixelHitsinTrack, // output; dimensionality : [MAXMVDSTRIPHITSINTRACK];
                             Short_t &nStripHitsinTrack,    // output
                             Short_t *ListStripHitsinTrack  // output; dimensionality : [MAXMVDSTRIPHITSINTRACK];
  );

  Short_t AssociateSciTilHit(Double_t dimensionscitil,
                             Double_t *esse, // output, list of  S of the SciTil hits associated.
                             bool *InclusionListSciTil,
                             Short_t *List, // output, list of SciTil hits associated (max. 2);
                             Short_t maxscitilhitsintrack, Short_t nSciTilHits, Double_t Oxx, Double_t Oyy, Double_t posizSciTil[][3], Double_t Rr);

  void DecideWhichAngularRangeAndCharge(Double_t fiCenter,        // input, fi of the (0,0) in the Helix reference frame;
                                        Double_t fi_low_limit[2], // input; fi low limit of the Stt detector in the Helix frame;
                                        Double_t fi_up_limit[2],  // input; fi up limit of the Stt detector in the Helix frame;
                                        Double_t (*info)[7],      // input
                                        Short_t *ListHitsinTrack, // input
                                        Short_t nHitsinTrack,     // input
                                        Double_t Oxx,             // input
                                        Double_t Oyy,             // input

                                        Short_t &charge,         // output; charge of the particle;
                                        Double_t &FiRangeMvdLow, // output; Fi range of possible Mvd hits; FiRangeMvdLow always
                                                                 // between 0 and 2PI; and always  FiRangeMvdLow<FiRangeMvdUp;
                                        Double_t &FiRangeMvdUp,  // output; Fi range of possible Mvd hits;
                                        Double_t &Fi_low_limit,  // output;
                                        Double_t &Fi_up_limit    // output;

  );

  bool FindTrackInXYProjection(struct FindTrackInXYProjection2_InputOutputData *InOut, int istampa, int IVOLTE);

  void OrderingUsingConformal(Short_t Charge,     // input;
                              Double_t info[][7], // input;
                              Int_t nHits,        // input;
                              Double_t oX,        // input;
                              Double_t oY,        // input;
                              Short_t *ListHits   // input and output (ordered);
  );

  void OrderingUsingFi(Short_t Charge,     // input;
                       Double_t info[][7], // input;
                       Int_t nHits,        // input;
                       Double_t oX,        // input;
                       Double_t oY,        // input;
                       Short_t *ListHits   // input and output (ordered);
  );

  void OrderingUsingR(Double_t info[][7], // input;
                      Int_t nHits,        // input;
                      Short_t *ListHits   // input and output (ordered);
  );

  Short_t TrkAssociatedParallelHitsToHelix5(Short_t *auxListHitsinTrack, bool *InclusionListStt, Double_t Fi_low, Double_t Fi_up, Double_t info[][7], Short_t *ListSttParHits,
                                            Int_t NhitsParallel, Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t strawradius);

  Short_t TrkAssociatedParallelHitsToHelix6(Short_t *auxListHitsinTrack, bool *InclusionListStt, Double_t Fi_low, Double_t Fi_up, Double_t info[][7], Short_t *ListSttParHits,
                                            Int_t NhitsParallel, Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t maximum_distance);
  ClassDef(PndTrkCTFindTrackInXY2, 1);
};

#endif
