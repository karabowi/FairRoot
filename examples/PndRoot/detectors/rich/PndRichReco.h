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

#ifndef PNDRICHRECO_H
#define PNDRICHRECO_H

#include "FairGeoSet.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "PndRichGeo.h"
#include "PndRichPhoton.h"
#include "PndRichBarPoint.h"
#include "PndRichMirrorSegment.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include <map>
#include <TVectorT.h>
#include <TMatrixT.h>

class TClonesArray;

using std::map;
using std::pair;
using std::vector;

class PndRichReco //: public FairGeoSet
{

 private:
  // removed from PndPidCorrelator.h
  TClonesArray *fRichPDHit; //! PndRichPDHit TCA

  PndRichGeo *fGeo;
  UInt_t fEvent;
  UInt_t fGeoVersion;
  UInt_t fGeoVersionMirr;
  UInt_t fParticleID;
  Double_t fMirrorLength;
  TVector3 fTrackPosition;
  TVector3 fTrackDirection;
  // flat mirror parametrs
  std::vector<PndRichMirrorSegment> fMirrSegs;

  Double_t fPhDetAngle;
  Double_t fZamid;
  Double_t fTrackTime;
  TVectorT<double> gResVect;
  TMatrixT<double> gRotMatr;

 protected:
 public:
  PndRichReco();
  PndRichReco(UInt_t version);
  virtual ~PndRichReco();

  void Init();
  void Register();

  void SetGeoVersion(UInt_t version) { fGeoVersion = version; };
  void RichFullReconstruction(TVector3 pos, TVector3 dir, Float_t ts, Float_t &chi2, Float_t &chTh, Float_t &dChTh, Int_t &nph);
  std::vector<double> GetPhis();
  std::vector<double> GetThetas();
  std::vector<double> GetDThetas();
  std::vector<PndRichPhoton> CherenkovPhotonListFlat(PndRichBarPoint *track);
  double BetaPeakFinding(std::vector<PndRichPhoton> photons, Double_t nopt, Double_t nnz);
  void HitSelection(std::vector<size_t> &it, std::vector<double> &ph, std::vector<double> &th, std::vector<PndRichPhoton> photons, Double_t beta, Double_t nopt, Double_t nnz,
                    Double_t dthc);
  void AppendFlatMirrorReflections(std::vector<PndRichPhoton> &ph, TVector3 hit, Double_t hitTime, PndRichBarPoint *track);

  ClassDef(PndRichReco, 1);
};

#endif
