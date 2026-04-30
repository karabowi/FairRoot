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

#ifndef PndMvdSttGemRiemannTrackFinder_H_
#define PndMvdSttGemRiemannTrackFinder_H_

#include "TClonesArray.h"
#include "PndRiemannTrackFinder.h"
#include "PndGeoHandling.h"
#include "PndSttStrawMap.h"

#include "TH2F.h"
#include <vector>

class PndSttHit;

class PndMvdSttGemRiemannTrackFinder : public PndRiemannTrackFinder {
 public:
  PndMvdSttGemRiemannTrackFinder();
  virtual ~PndMvdSttGemRiemannTrackFinder();
  PndMvdSttGemRiemannTrackFinder(const PndMvdSttGemRiemannTrackFinder &) = delete;
  PndMvdSttGemRiemannTrackFinder &operator=(const PndMvdSttGemRiemannTrackFinder &) = delete;

  void FindTracks(); ///< Main function to start the riemann track finding
  void AddHits(TClonesArray *hits, Int_t branchId);

  // void SetGeoH(PndGeoHandling geoH){fGeoH=geoH;};
  void SetCutDistH(TH2F *hist) { fCutDistH = hist; }
  void SetCutChi2H(TH2F *hist) { fCutChi2H = hist; }

  void SetSttTubeArray(TClonesArray *SttTubeArray) { fStrawMap.GenerateStrawMap(SttTubeArray); }

 private:
  PndGeoHandling *fGeoH;

  std::vector<std::set<Int_t>> GetStartTracks();
  bool CheckSZ(PndRiemannTrack aTrack); ///< Tests the results of the sz fit
  bool CheckRiemannHit(PndRiemannTrack *track, PndRiemannHit *hit, FairHit *fairHit);
  bool CheckRiemannHitMvd(PndRiemannTrack *track, PndRiemannHit *hit, FairHit *fairHit);
  bool CheckRiemannHitGem(PndRiemannTrack *track, PndRiemannHit *hit, FairHit *fairHit);
  bool CheckRiemannHitStt(PndRiemannTrack *track, PndRiemannHit *hit, FairHit *fairHit);
  bool CheckRiemannHitSkewedStt(PndRiemannTrack *track, PndRiemannHit *hit, FairHit *fairHit);

  //	bool CheckTrackStt(PndRiemannTrack* track);

  void AssignSttHits(PndRiemannTrack &actTrack, std::set<Int_t> &startTrack);
  void AssignSkewedSttHits(PndRiemannTrack &actTrack, std::set<Int_t> &startTrack);
  void AssignGemHits(PndRiemannTrack &actTrack, std::set<Int_t> &startTrack);

  bool CheckBoarderHitsStt(PndTrackCand *track);
  int GetStrawSector(PndRiemannTrack &track);

  int GetLayerGem(FairHit *hit);
  int GetLayerMvd(FairHit *hit);
  int GetLayer(TString identifier);
  void InitLayerMap();
  void InitLayerMapMvd();
  void InitLayerMapGem();

  TH2F *fCutDistH; ///< cutting histograms
  TH2F *fCutChi2H;

  TClonesArray *fSttHits;
  TClonesArray *fSkewedSttHits;
  TClonesArray *fGemHits;
  PndSttStrawMap fStrawMap;
  std::vector<std::vector<PndSttHit *>> fSttHitsInSectors;
  std::vector<std::vector<FairHit *>> fSttSkewedHitsInSectors;

  double fZClosePar; ///< parameter to separate  forward and backward tracks

  std::vector<std::vector<int>> fLayers; ///< contains layer information of hits
  std::map<TString, int> fLayerMap;      ///< identifier string, assigned layer id
  int fLastLayerId;                      ///< last layer Id assigned
  /*       b - barrel layer;   d - disk layer
  0 - zero position
  1 - 1b
  2 - 1d
  3 - 2d
  4 - 2b
  5 - 3d
  6 - 4d
  7 - 3b
  8 - 5d
  9 - 4b
   10 - 6d
*/
  int fNLayers;                                                         ///< number of Layers
  double GetMaxPlaneDist(double radius, double dip, bool sign);         // getting cut distance
  double GetMaxSZChi2(double radius, double dip, bool sign);            // getting cut Chi2
  std::set<int> GetTooCloseHitsInLayer(int LayerNumber, int HitNumber); // searching for too close hits to HitNumber-Hit in layer LayerNumber

 public:
  ClassDef(PndMvdSttGemRiemannTrackFinder, 1)
};

#endif /*PndMvdSttGemRiemannTrackFinder_H_*/
