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

/** PndTrkTrack
 **
 ** @author Lia Lavezzi
 **/

#ifndef PNDTRKTRACK_H
#define PNDTRKTRACK_H 1

#include "TVector3.h"
#include "TVector2.h"
#include "PndTrkCluster.h"
#include "PndTrackCand.h"
#include "PndTrack.h"
#include "PndTrkHit.h"
#include "PndTrkTools.h"

class PndTrkTrack : public TObject {

 public:
  PndTrkTrack();
  PndTrkTrack(PndTrkCluster *cluster);
  PndTrkTrack(PndTrkCluster *cluster, double x, double y, double radius);
  PndTrkTrack(PndTrkHit *hit, PndTrkCluster *cluster, double x, double y, double radius);
  PndTrkTrack(double x, double y, double radius);
  PndTrkTrack(PndTrack *trk);
  PndTrkTrack(const PndTrkTrack &track);

  ~PndTrkTrack();

  PndTrkTrack &operator=(const PndTrkTrack &track);

  Bool_t operator==(PndTrkTrack track); // CHECK this needs to be changed

  void SetRadius(double radius) { fRadius = radius; }
  void SetCenter(double x, double y)
  {
    fCenterX = x;
    fCenterY = y;
  }
  void SetCenter(TVector3 center)
  {
    fCenterX = center.X();
    fCenterY = center.Y();
  }
  void SetTanL(double tanl) { fTanL = tanl; }
  void SetZ0(double z0) { fZ0 = z0; }
  void SetCluster(PndTrkCluster *cluster) { fCluster = *cluster; }
  void SetRefHit(PndTrkHit *hit) { fRefHit = hit; }
  Double_t GetRadius() { return fRadius; }
  TVector2 GetCenter() { return TVector2(fCenterX, fCenterY); }
  Double_t GetTanL() { return fTanL; }
  Double_t GetZ0() { return fZ0; }

  PndTrkCluster GetCluster() { return fCluster; }

  Double_t ComputePhi(TVector3 hit);
  Double_t ComputePhiFrom(TVector3 hit, TVector3 from);

  TVector3 ComputeMomentumAtPosition(TVector3 position, TVector3 &newposition);
  void ComputeCharge();
  PndTrackCand ConvertToPndTrackCand();
  PndTrack ConvertToPndTrack();
  Int_t GetCharge() { return fCharge; }

  void Draw(Color_t color = kBlack);
  void LightUp();

  void Clear(Option_t *opt = "");

 protected:
  PndTrkCluster fCluster;                         //!
  PndTrkHit *fRefHit;                             //!
  double fCenterX, fCenterY, fRadius, fTanL, fZ0; //!
  int fCharge;                                    //!
  double fPhiMin;                                 //!
  double fPhiMax;                                 //!
  // parameters:
  // pT = p cos(lam)
  // pL = p sin(lam)
  // ==> pL = pT * tan(lam)
  // z = z0 + s sin(lam) = z0 + tan(lam) [s cos(lam)]
  // with s cos(lam) = proj track length = -q phi_rad* R
  // ==> z = z0 - q tanl phi_rad R
  // If I have z = z0 + k phi_deg ==> k 180/pi = -q tanl R ==> tanl = -q k (180/pi) /R

  ClassDef(PndTrkTrack, 1);
};

#endif
