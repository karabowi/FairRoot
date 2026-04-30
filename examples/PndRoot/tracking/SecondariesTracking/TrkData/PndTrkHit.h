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

/** PndTrkHit
 ** Class for pattern recognition hit
 ** @author Lia Lavezzi
 **
 ** fSensorID: identifies the specific sensor of the MVD,
               the specific tube of the STT
 **/

#ifndef PNDTRKHIT_H
#define PNDTRKHIT_H 1

#include "FairTimeStamp.h"
#include "TVector3.h"
#include "PndTrkParameters.h"
/* #include "TObject.h" */

class PndTrkHit : public FairTimeStamp {

 public:
  PndTrkHit();
  PndTrkHit(Int_t hitID, Int_t detID, Bool_t used, Int_t iregion, Int_t sensorID, TVector3 &pos, Double_t isochrone, Double_t sortvar);
  //  PndTrkHit(Int_t hitID, Int_t detID, Bool_t used, Int_t iregion, Int_t sensorID, TVector3& pos);
  PndTrkHit(const PndTrkHit &hit);

  ~PndTrkHit();

  virtual Bool_t IsEqual(const TObject *obj) const { return (((PndTrkHit *)obj)->fDetectorID == fDetectorID) && (((PndTrkHit *)obj)->fHitID == fHitID); }

  virtual Bool_t IsSortable() { return !(fSortVariable == -1); }
  Int_t Compare(const TObject *hit) const;

  Bool_t operator==(const PndTrkHit &hit1);
  Bool_t operator<(const PndTrkHit &hit1);
  /*  friend Bool_t operator==(const PndTrkHit & hit1, const PndTrkHit & hit2); */
  //  friend Bool_t operator<(const PndTrkHit &hit1, const PndTrkHit &hit2) ;

  // CHECK add other set
  void SetIsochrone(Double_t isochrone) { fIsochrone = isochrone; }
  void SetSortVariable(Double_t sortvar) { fSortVariable = sortvar; }

  void SetPosition(TVector3 pos) { fPosition = pos; }
  void SetPhi(Double_t phi) { fPhi = phi; }

  void SetUsedFlag(Bool_t used) { fUsed = used; }
  void SetRefHitFlag(Bool_t used) { fUsedAsRefHit = used; }

  void SetRegion(Int_t iregion) { fIRegion = iregion; }

  // inline get
  inline Int_t GetHitID() { return fHitID; }
  inline Int_t GetDetectorID() { return fDetectorID; }
  inline Bool_t IsUsed() { return fUsed; }
  inline Bool_t WasRefHit() { return fUsedAsRefHit; }
  inline Int_t GetSensorID() { return fSensorID; }
  inline Int_t GetTubeID() { return fSensorID; }
  inline TVector3 GetPosition() { return fPosition; }
  inline Double_t GetIsochrone() { return fIsochrone; }
  inline Int_t GetIRegion() { return fIRegion; }
  inline Bool_t IsRegion(Int_t iregion) { return iregion == fIRegion; }
  inline Double_t GetSortVariable() { return fSortVariable; }
  inline Double_t GetPhi() { return fPhi; }

  // is stt
  inline Bool_t IsSttParallel() { return fIRegion == INNER_LEFT || fIRegion == OUTER_LEFT || fIRegion == INNER_RIGHT || fIRegion == OUTER_RIGHT; }
  inline Bool_t IsSttSkew() { return fIRegion == SKEW_LEFT || fIRegion == SKEW_RIGHT; }
  inline Bool_t IsStt() { return IsSttParallel() || IsSttSkew(); }

  // is mvd
  inline Bool_t IsMvdPixel() { return fIRegion == MVDPIXEL; }
  inline Bool_t IsMvdStrip() { return fIRegion == MVDSTRIP; }
  inline Bool_t IsMvd() { return IsMvdStrip() || IsMvdPixel(); }

  // is gem
  inline Bool_t IsGem() { return fIRegion == GEM; }

  // is scitil
  inline Bool_t IsSciTil() { return fIRegion == SCIT; }

  Double_t GetDistance(PndTrkHit *fromhit);
  Double_t GetDistance(TVector3 frompoint);
  Double_t GetXYDistance(PndTrkHit *fromhit);
  Double_t GetXYDistance(TVector3 frompoint);
  Double_t GetXYDistanceFromTrack(double x0, double y0, double R);
  // info
  void Draw(Color_t color);
  void DrawTube(Color_t color);
  void Print();

  int GetSector() { return fSector; }

  // variables
  Int_t fHitID, fDetectorID, fIRegion, fSensorID;
  Bool_t fUsed;
  Bool_t fUsedAsRefHit;
  TVector3 fPosition;
  Double_t fIsochrone;
  Double_t fSortVariable; // variable for sorting
  Double_t fPhi;
  Double_t fSector;
  ClassDef(PndTrkHit, 1);
};

#endif
