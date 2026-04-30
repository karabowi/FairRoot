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

/** PndTrackingQualityRecoInfo
 **
 ** @author Lia Lavezzi
 **/

#ifndef PNDTRACKINGQUALITYRECOINFO_H
#define PNDTRACKINGQUALITYRECOINFO_H 1

#include "PndTrackingQualityMCInfo.h"

#include "TVector3.h"

class PndTrackingQualityRecoInfo : public TObject {

 public:
  PndTrackingQualityRecoInfo();
  PndTrackingQualityRecoInfo(int recotrackid);
  ~PndTrackingQualityRecoInfo();

  void SetMCTrackInfo(PndTrackingQualityMCInfo *info)
  {
    fMCTrackInfo = *info;
    fMCMomFirst = fMCTrackInfo.GetMomentumFirst();
    fMCMomLast = fMCTrackInfo.GetMomentumLast();
    fMCVertex = fMCTrackInfo.GetVertex();
    fMCCharge = fMCTrackInfo.GetCharge();
    fIsPrimary = fMCTrackInfo.GetIsPrimary();
    fMCPdg = fMCTrackInfo.GetPDGCode();
    fQuality = fMCTrackInfo.GetQuality();
    fMCQuality = fMCTrackInfo.GetMCQuality();
  }
  void SetNofMCTracks(Int_t nofmctracks) { fNofMCTracks = nofmctracks; }
  Int_t GetNofMCTracks() { return fNofMCTracks; }

  double GetPurity();
  /*  double GetSttParalPurity() { return (double) GetNofSttParalTrueHits()/GetNofSttParalHits(); } */
  /*  double GetSttSkewPurity() { return (double) GetNofSttSkewTrueHits()/GetNofSttSkewHits(); } */
  double GetSttPurity();
  double GetMvdPixelPurity();
  double GetMvdStripPurity();
  double GetMvdPurity();
  double GetGemPurity();
  double GetFtsPurity();
  /*  double GetSciTilPurity() { return (double) GetNofSciTilTrueHits()/GetNofSciTilHits(); } */

  double GetContamination();
  /*  double GetSttParalContamination() { return (double) GetNofSttParalFakeHits()/GetNofSttParalHits(); } */
  /*  double GetSttSkewContamination() { return (double) GetNofSttSkewFakeHits()/GetNofSttSkewHits(); } */
  double GetSttContamination();
  double GetMvdPixelContamination();
  double GetMvdStripContamination();
  double GetMvdContamination();
  double GetGemContamination();
  double GetFtsContamination();
  /*  double GetSciTilContamination() { return (double) GetNofSciTilFakeHits()/GetNofSciTilHits(); } */

  double GetEfficiency();
  double GetMvdPixelEfficiency();
  double GetMvdStripEfficiency();
  double GetMvdEfficiency();
  /*  double GetSttParalEfficiency(); */
  /*  double GetSttSkewEfficiency(); */
  double GetSttEfficiency();
  double GetGemEfficiency();
  double GetFtsEfficiency();
  /*  double GetSciTilEfficiency(); */

  double GetInefficiency();
  double GetMvdPixelInefficiency();
  double GetMvdStripInefficiency();
  double GetMvdInefficiency();
  /*  double GetSttParalInefficiency(); */
  /*  double GetSttSkewInefficiency(); */
  double GetSttInefficiency();
  double GetGemInefficiency();
  double GetFtsInefficiency();
  /*  double GetSciTilInefficiency(); */

  int GetQuality() { return fQuality; }
  int GetMCQuality() { return fMCQuality; }

  void SetMCVertex(TVector3 pos) { fMCVertex = pos; }
  void SetMCMomentum(TVector3 mom) { fMCMomentum = mom; }

  void SetPositionFirst(TVector3 pos) { fPosFirst = pos; }
  void SetMomentumFirst(TVector3 mom) { fMomFirst = mom; }
  void SetPositionLast(TVector3 pos) { fPosLast = pos; }
  void SetMomentumLast(TVector3 mom) { fMomLast = mom; }

  TVector3 GetPositionFirst() { return fPosFirst; }
  TVector3 GetMomentumFirst() { return fMomFirst; }
  TVector3 GetPositionLast() { return fPosLast; }
  TVector3 GetMomentumLast() { return fMomLast; }

  void SetCharge(int ch) { fCharge = ch; }

  void SetNofMvdPixelTrueHits(int nofpix) { fNofMvdPixelTrueHits = nofpix; }
  void SetNofMvdStripTrueHits(int nofstr) { fNofMvdStripTrueHits = nofstr; }
  /*  void SetNofSttParalTrueHits(int nofsttparal) { fNofSttParalTrueHits = nofsttparal; } */
  /*  void SetNofSttSkewTrueHits(int nofsttskew) { fNofSttSkewTrueHits = nofsttskew; } */
  void SetNofSttTrueHits(int nofstt) { fNofSttTrueHits = nofstt; }
  void SetNofGemTrueHits(int nofgem) { fNofGemTrueHits = nofgem; }
  void SetNofFtsTrueHits(int noffts) { fNofFtsTrueHits = noffts; }
  /*  void SetNofSciTilTrueHits(int nofscitil) { fNofSciTilTrueHits = nofscitil; } */

  Int_t GetNofMvdPixelTrueHits() { return fNofMvdPixelTrueHits; }
  Int_t GetNofMvdStripTrueHits() { return fNofMvdStripTrueHits; }
  Int_t GetNofMvdTrueHits() { return fNofMvdPixelTrueHits + fNofMvdStripTrueHits; }
  /*  Int_t GetNofSttParalTrueHits() { return fNofSttParalTrueHits; } */
  /*  Int_t GetNofSttSkewTrueHits() { return fNofSttSkewTrueHits; } */
  Int_t GetNofSttTrueHits() { return fNofSttTrueHits; } // fNofParalSttTrueHits + fNofSttSkewTrueHits ;}
  Int_t GetNofGemTrueHits() { return fNofGemTrueHits; }
  Int_t GetNofFtsTrueHits() { return fNofFtsTrueHits; }
  /*  Int_t GetNofSciTilTrueHits() {return fNofSciTilTrueHits; } */
  Int_t GetNofRecoTrueHits()
  {
    return fNofMvdPixelTrueHits + fNofMvdStripTrueHits + fNofSttTrueHits + fNofGemTrueHits + fNofFtsTrueHits;
  } // + fNofSttParalTrueHits + fNofSttSkewTrueHit + fNofSciTilTrueHits; }

  void SetNofMvdPixelFakeHits(int nofpix) { fNofMvdPixelFakeHits = nofpix; }
  void SetNofMvdStripFakeHits(int nofstr) { fNofMvdStripFakeHits = nofstr; }
  /*  void SetNofSttParalFakeHits(int nofsttparal) { fNofSttParalFakeHits = nofsttparal; } */
  /*  void SetNofSttSkewFakeHits(int nofsttskew) { fNofSttSkewFakeHits = nofsttskew; } */
  void SetNofSttFakeHits(int nofstt) { fNofSttFakeHits = nofstt; }
  void SetNofGemFakeHits(int nofgem) { fNofGemFakeHits = nofgem; }
  void SetNofFtsFakeHits(int noffts) { fNofFtsFakeHits = noffts; }
  /*  void SetNofSciTilFakeHits(int nofscitil) { fNofSciTilFakeHits = nofscitil; } */

  Int_t GetNofMvdPixelFakeHits() { return fNofMvdPixelFakeHits; }
  Int_t GetNofMvdStripFakeHits() { return fNofMvdStripFakeHits; }
  Int_t GetNofMvdFakeHits() { return fNofMvdPixelFakeHits + fNofMvdStripFakeHits; }
  /*  Int_t GetNofSttParalFakeHits() { return fNofSttParalFakeHits; } */
  /*  Int_t GetNofSttSkewFakeHits() { return fNofSttSkewFakeHits; } */
  Int_t GetNofSttFakeHits() { return fNofSttFakeHits; } // fNofSttParalFakeHits + fNofSttSkewFakeHits ;}
  Int_t GetNofGemFakeHits() { return fNofGemFakeHits; }
  Int_t GetNofFtsFakeHits() { return fNofFtsFakeHits; }
  /*  Int_t GetNofSciTilFakeHits() {return fNofSciTilFakeHits; } */
  Int_t GetNofRecoFakeHits()
  {
    return fNofMvdPixelFakeHits + fNofMvdStripFakeHits + fNofSttFakeHits + fNofGemFakeHits;
  } //  + fNofSttParalFakeHits + fNofSttSkewFakeHits + fNofSciTilFakeHits; }

  Int_t GetNofMvdPixelMissingHits() { return fNofMvdPixelMissingHits; }
  Int_t GetNofMvdStripMissingHits() { return fNofMvdStripMissingHits; }
  Int_t GetNofMvdMissingHits() { return fNofMvdPixelMissingHits + fNofMvdStripMissingHits; }
  /*  Int_t GetNofSttParalMissingHits() { return fNofSttParalMissingHits; } */
  /*  Int_t GetNofSttSkewMissingHits() { return fNofSttSkewMissingHits; } */
  Int_t GetNofSttMissingHits() { return fNofSttMissingHits; } // fNofSttParalMissingHits + fNofSttSkewMissingHits ;}
  Int_t GetNofGemMissingHits() { return fNofGemMissingHits; }
  Int_t GetNofFtsMissingHits() { return fNofFtsMissingHits; }
  /*  Int_t GetNofSciTilMissingHits() {return fNofSciTilMissingHits; } */
  Int_t GetNofMissingHits()
  {
    return fNofMvdPixelMissingHits + fNofMvdStripMissingHits + fNofSttMissingHits + fNofGemMissingHits;
  } // + fNofSttParalMissingHits + fNofSttSkewMissingHits + fNofSciTilMissingHits; }

  void SetNofMvdPixelMissingHits(int nofpix) { fNofMvdPixelMissingHits = nofpix; }
  void SetNofMvdStripMissingHits(int nofstr) { fNofMvdStripMissingHits = nofstr; }
  /*  void SetNofSttParalMissingHits(int nofsttparal) { fNofSttParalMissingHits = nofsttparal; } */
  /*  void SetNofSttSkewMissingHits(int nofsttskew) { fNofSttSkewMissingHits = nofsttskew; } */
  void SetNofSttMissingHits(int nofstt) { fNofSttMissingHits = nofstt; }
  void SetNofGemMissingHits(int nofgem) { fNofGemMissingHits = nofgem; }
  void SetNofFtsMissingHits(int nofgem) { fNofFtsMissingHits = nofgem; }
  /*  void SetNofSciTilMissingHits(int nofscitil) { fNofSciTilMissingHits = nofscitil; } */

  Int_t GetNofTrueHits()
  {
    return fNofMvdPixelTrueHits + fNofMvdStripTrueHits + fNofSttTrueHits + fNofGemTrueHits;
  } // + fNofSttParalTrueHits + fNofSttSkewTrueHits + fNofSciTilTrueHits; }
  Int_t GetNofFakeHits()
  {
    return fNofMvdPixelFakeHits + fNofMvdStripFakeHits + fNofSttFakeHits + fNofGemFakeHits;
  } // + fNofSttParalFakeHits + fNofSttSkewFakeHits + fNofSciTilFakeHits; }
  Int_t GetNofRecoHits() { return GetNofRecoTrueHits() + GetNofRecoFakeHits(); }

  Int_t GetNofMvdPixelHits() { return GetNofMvdPixelTrueHits() + GetNofMvdPixelFakeHits(); }
  Int_t GetNofMvdStripHits() { return GetNofMvdStripTrueHits() + GetNofMvdStripFakeHits(); }
  Int_t GetNofMvdHits() { return GetNofMvdPixelHits() + GetNofMvdStripHits(); }
  /*  Int_t GetNofSttParalHits() { return GetNofSttParalTrueHits() + GetNofSttParalFakeHits(); } */
  /*  Int_t GetNofSttSkewHits() { return GetNofSttSkewTrueHits() + GetNofSttSkewFakeHits(); } */
  Int_t GetNofSttHits() { return GetNofSttTrueHits() + GetNofSttFakeHits(); } // GetNofSttParalHits() + GetNofSttSkewHits(); }
  Int_t GetNofGemHits() { return GetNofGemTrueHits() + GetNofGemFakeHits(); }
  Int_t GetNofFtsHits() { return GetNofFtsTrueHits() + GetNofFtsFakeHits(); }
  /*  Int_t GetNofSciTilHits() { return GetNofSciTilTrueHits() + GetNofSciTilFakeHits(); } */

  Bool_t IsFull() { return GetEfficiency() > 0.9999; }
  Bool_t IsAlmostFull() { return GetEfficiency() < 0.9999 && GetEfficiency() > 0.90; }
  Bool_t IsPartial() { return GetEfficiency() < 0.90 && GetEfficiency() > 0.60; }
  Bool_t IsScarce() { return GetEfficiency() < 0.60 && GetEfficiency() > 0.; }

  Bool_t IsClean() { return GetContamination() < 0.0001; }
  Bool_t IsAlmostClean() { return GetContamination() > 0.0001 && GetContamination() < 0.1; }
  Bool_t IsNoisy() { return GetContamination() > 0.1; }

  Int_t GetRecoTrackID() { return fRecoTrackID; }

  Int_t GetCharge() { return fCharge; }

  PndTrackingQualityMCInfo GetMCTrackInfo() { return fMCTrackInfo; }
  void SetMCTrackID(int mctrackid) { fMCTrackID = mctrackid; }
  Int_t GetMCTrackID() { return fMCTrackID; }

  void SetTrue() { fTrue = 1; }
  Bool_t IsTrue() { return fTrue == 1; }

  void SetClone() { fTrue = 0; }
  Bool_t IsClone() { return fTrue == 0; }

  void SetFlag(int flag) { fFlag = flag; }
  Int_t GetFlag() { return fFlag; }

  void SetIdealTrackId(int idealid) { fIdealTrackId = idealid; }
  Int_t GetIdealTrackId() { return fIdealTrackId; }

 protected:
  Int_t fNofMvdPixelTrueHits, fNofMvdStripTrueHits, fNofSttTrueHits, fNofGemTrueHits, fNofFtsTrueHits;
  Int_t fNofMvdPixelFakeHits, fNofMvdStripFakeHits, fNofSttFakeHits, fNofGemFakeHits, fNofFtsFakeHits;
  Int_t fNofMvdPixelMissingHits, fNofMvdStripMissingHits, fNofSttMissingHits, fNofGemMissingHits, fNofFtsMissingHits;

  /*  Int_t fNofSttParalTrueHits, fNofSttSkewTrueHits, fNofSciTilTrueHits; */
  /*  Int_t fNofSttParalFakeHits, fNofSttSkewFakeHits, fNofSciTilFakeHits; */
  /*  Int_t fNofSttParalMissingHits, fNofSttSkewMissingHits, fNofSciTilMissingHits; */

  Int_t fRecoTrackID;
  TVector3 fPosFirst, fMomFirst;
  TVector3 fPosLast, fMomLast;

  PndTrackingQualityMCInfo fMCTrackInfo;
  Int_t fFlag;
  Int_t fNofMCTracks;
  Int_t fIdealTrackId;

  TVector3 fMCMomFirst;
  TVector3 fMCMomLast;
  TVector3 fMCVertex;
  TVector3 fMCMomentum;
  Int_t fMCCharge;
  Int_t fMCPdg;

  Bool_t fIsPrimary;
  Int_t fQuality;
  Int_t fMCQuality;
  Int_t fCharge;
  Int_t fMCTrackID;
  // true = 0, clone = 1
  Bool_t fTrue;

  ClassDef(PndTrackingQualityRecoInfo, 2);
};

#endif
