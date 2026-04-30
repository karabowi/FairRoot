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

/**
 * @class PndTrackingQARecoInfo
 *
 * @date 20.06.2023
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 */

#pragma once

#include "PndTrackingQABranchEnum.h"

#include "TVector3.h"
#include "TString.h"
#include "TObject.h"

#include "FairLink.h"

#include <map>
#include <array>
#include <iostream>
#include <functional>

class PndTrackingQAMCInfo;

class PndTrackingQARecoInfo : public TObject {
 public:
  PndTrackingQARecoInfo();
  PndTrackingQARecoInfo(FairLink recoTrackID) : fRecoTrackID(recoTrackID) {}

  virtual ~PndTrackingQARecoInfo();

  void SetMCTrackInfo(PndTrackingQAMCInfo *info);

  Int_t GetTrueHits();
  Int_t GetTrueHits(TString branch) { return GetTrueHits(TrackingQA::StringToBranchEnum(branch)); }
  Int_t GetTrueHits(TrackingQA::branchEnum branch)
  {
    if (branch == TrackingQA::branchEnum::STTHit) {
      return (fTrueHitsCollection[TrackingQA::branchEnum::STTHitSkew] + fTrueHitsCollection[TrackingQA::branchEnum::STTHitParal]);
    } else {
      return fTrueHitsCollection[branch];
    }
  }
  Int_t GetFalseHits();
  Int_t GetFalseHits(TString branch) { return GetFalseHits(TrackingQA::StringToBranchEnum(branch)); }
  Int_t GetFalseHits(TrackingQA::branchEnum branch)
  {
    if (branch == TrackingQA::branchEnum::STTHit) {
      return (fFalseHitsCollection[TrackingQA::branchEnum::STTHitSkew] + fFalseHitsCollection[TrackingQA::branchEnum::STTHitParal]);
    } else {
      return fFalseHitsCollection[branch];
    }
  }
  Int_t GetMissingHits();
  Int_t GetMissingHits(TString branch) { return GetMCHits(branch) - GetTrueHits(branch); }
  Int_t GetMissingHits(TrackingQA::branchEnum branch);
  Int_t GetMCHits();
  Int_t GetMCHits(TString branch)
  {
    if (branch == "STTHit") {
      return fMCHitsCollection[TrackingQA::branchEnum::STTHitParal] + fMCHitsCollection[TrackingQA::branchEnum::STTHitSkew];
    }
    return fMCHitsCollection[TrackingQA::StringToBranchEnum(branch)];
  }

  Int_t GetMCHits(TrackingQA::branchEnum branch);

  Int_t GetRecoHits() { return GetTrueHits() + GetFalseHits(); }
  Int_t GetRecoHits(TString branch) { return GetTrueHits(branch) + GetFalseHits(branch); }

  double GetEfficiency()
  {
    if (GetMCHits() > 0) {
      return (double)GetTrueHits() / GetMCHits();
    } else
      return -1.;
  }

  double GetEfficiency(TString branch)
  {
    if (GetMCHits(branch) > 0) {
      return (double)GetTrueHits(branch) / GetMCHits(branch);
    } else
      return -1.;
  }

  double GetEfficiency(TrackingQA::branchEnum branch);

  double GetPurity()
  {
    if (GetRecoHits() > 0) {
      return (double)GetTrueHits() / GetRecoHits();
    } else
      return -1.;
  }

  double GetPurity(TString branch)
  {
    if (GetRecoHits(branch) > 0) {
      return (double)GetTrueHits(branch) / GetRecoHits(branch);
    } else
      return -1.;
  }

  double GetPurity(TrackingQA::branchEnum branch);

  void SetQuality(int val) { fQuality = val; }
  int GetQuality() const { return fQuality; }
  int GetMCQuality() const { return fMCQuality; }

  void SetMCVertex(TVector3 pos) { fMCVertex = pos; }
  void SetMCMomentum(TVector3 mom) { fMCMomentum = mom; }

  void SetPositionFirst(TVector3 pos) { fPosFirst = pos; }
  void SetMomentumFirst(TVector3 mom) { fMomFirst = mom; }
  void SetPositionLast(TVector3 pos) { fPosLast = pos; }
  void SetMomentumLast(TVector3 mom) { fMomLast = mom; }

  TVector3 GetPositionFirst() const { return fPosFirst; }
  TVector3 GetMomentumFirst() const { return fMomFirst; }
  TVector3 GetPositionLast() const { return fPosLast; }
  TVector3 GetMomentumLast() const { return fMomLast; }

  TVector3 GetMCPositionFirst() const { return fMCPosFirst; }
  TVector3 GetMCMomentumFirst() const { return fMCMomFirst; }
  TVector3 GetMCPositionLast() const { return fMCPosLast; }
  TVector3 GetMCMomentumLast() const { return fMCMomLast; }

  void SetCharge(int ch) { fCharge = ch; }
  Int_t GetCharge() const { return fCharge; }

  void SetMCCharge(int ch) { fMCCharge = ch; }
  Int_t GetMCCharge() const { return fMCCharge; }

  void SetTrueHits(TString branch, int hits) { fTrueHitsCollection[TrackingQA::StringToBranchEnum(branch)] = hits; }
  void SetFalseHits(TString branch, int hits) { fFalseHitsCollection[TrackingQA::StringToBranchEnum(branch)] = hits; }
  void SetMCTrueHits(TString branch, int hits) { fMCHitsCollection[TrackingQA::StringToBranchEnum(branch)] = hits; }

  void SetIdealTrackId(FairLink idealid) { fIdealTrackId = idealid; }
  FairLink GetIdealTrackId() const { return fIdealTrackId; }

  void SetMCTrackID(FairLink mctrackid) { fMCTrackID = mctrackid; }
  FairLink GetMCTrackID() const { return fMCTrackID; }

  void SetFitStatusFlag(int flag) { fFitStatusFlag = flag; }
  Int_t GetFitStatusFlag() const { return fFitStatusFlag; }

  void SetRecoTrackID(FairLink link) { fRecoTrackID = link; }
  FairLink GetRecoTrackID() const { return fRecoTrackID; }

  void SetIsClone(Bool_t val) { fIsClone = val; }
  Bool_t IsClone() { return fIsClone; }

  void SetNofMCTracks(Int_t val) { fNofMCTracks = val; }
  Int_t GetNofMCTracks() const { return fNofMCTracks; }

  friend std::ostream &operator<<(std::ostream &os, PndTrackingQARecoInfo &recoInfo)
  {
    os << "RecoTrack: " << recoInfo.GetRecoTrackID() << " RecoQuality " << recoInfo.GetQuality() << " MCTrackID " << recoInfo.GetMCTrackID()
       << " MCQuality: " << recoInfo.GetMCQuality() << std::endl;
    os << "Purity: " << recoInfo.GetPurity() << " Efficiency: " << recoInfo.GetEfficiency() << std::endl;
    std::cout << "BranchName : trueHits/falseHits/mcHits/missingHits : Efficiency/Purity" << std::endl;
    for (auto tHits : recoInfo.fTrueHitsCollection) {
      os << TrackingQA::BranchEnumToString(tHits.first) << " : " << tHits.second << "/" << recoInfo.GetFalseHits(tHits.first) << "/" << recoInfo.GetMCHits(tHits.first) << "/"
         << recoInfo.GetMissingHits(tHits.first) << " ";
      os << recoInfo.GetEfficiency(tHits.first) << "/" << recoInfo.GetPurity(tHits.first) << std::endl;
    }
    return os;
  }

 private:
  std::map<TrackingQA::branchEnum, Int_t> fTrueHitsCollection;
  std::map<TrackingQA::branchEnum, Int_t> fFalseHitsCollection;
  std::map<TrackingQA::branchEnum, Int_t> fMCHitsCollection;

  FairLink fRecoTrackID;
  TVector3 fPosFirst, fMomFirst;
  TVector3 fPosLast, fMomLast;

  Bool_t fIsClone = kFALSE;

  Int_t fNofMCTracks = 0;
  FairLink fIdealTrackId;

  TVector3 fMCMomFirst;
  TVector3 fMCMomLast;
  TVector3 fMCPosFirst;
  TVector3 fMCPosLast;
  TVector3 fMCVertex;
  TVector3 fMCMomentum;
  Int_t fMCCharge = -100;
  Int_t fMCPdg = 0;

  Bool_t fIsPrimary = kFALSE;
  Int_t fQuality = -100;
  Int_t fMCQuality = -100;
  Int_t fCharge = -100;
  FairLink fMCTrackID;

  Int_t fFitStatusFlag = 0;

  ClassDef(PndTrackingQARecoInfo, 7);
};
