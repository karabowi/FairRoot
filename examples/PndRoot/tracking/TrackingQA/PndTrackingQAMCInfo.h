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
 * @class PndTrackingQAMCInfo
 *
 * @date 20.06.2023
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 */

#pragma once

#include "PndTrackingQABranchEnum.h"
#include "TVector3.h"
#include "TString.h"
#include "TObject.h"

#include "TClonesArray.h"

#include "FairLink.h"

#include <map>
#include <functional>

class PndTrackingQAMCInfo : public TObject {
 public:
  PndTrackingQAMCInfo();
  virtual ~PndTrackingQAMCInfo();

  void SetdPCA2d(Double_t val) { fdPCA2d = val; }
  void SetCharge(int ch) { fCharge = ch; }
  void SetIsPrimary(Bool_t val) { fIsPrimary = val; }
  void SetVertex(TVector3 val) { fVertex = val; }
  void SetMomentum(TVector3 val) { fMomentum = val; }
  void SetPositionFirst(TVector3 pos) { fPosFirst = pos; }
  void SetMomentumFirst(TVector3 mom) { fMomFirst = mom; }
  void SetPositionLast(TVector3 pos) { fPosLast = pos; }
  void SetMomentumLast(TVector3 mom) { fMomLast = mom; }
  void SetPDGCode(int pdg) { fPDGCode = pdg; }
  void SetMCTrackID(FairLink mctrackid) { fMCTrackID = mctrackid; }
  void SetRecoTrackIDs(std::vector<FairLink> recotrkids) { fRecoTrackIDs = recotrkids; }
  void AddRecoTrackID(FairLink recotrkid) { fRecoTrackIDs.push_back(recotrkid); }
  void SetAssoRecoTrackID(FairLink asso) { fAssoRecoTrackID = asso; }
  void SetQuality(int quality) { fQuality = quality; }
  void SetMCQuality(int mcquality) { fMCQuality = mcquality; }
  void SetIdealTrackLink(FairLink link) { fIdealTrackLink = link; }
  void SetNofPoints(TString branchName, int points) { fMapBranchNameMCHits[TrackingQA::StringToBranchEnum(branchName.Data())] = points; }

  FairLink GetAssoRecoTrackID() const { return fAssoRecoTrackID; }
  FairLink GetMCTrackID() const { return fMCTrackID; }                  //< Returns the corresponding MCTrack to an ideal track
  Short_t GetNofRecoTracks(void) const { return fRecoTrackIDs.size(); } //< Returns the number of reco tracks associated to the ideal track
  FairLink GetRecoTrackID(Int_t i = 0) const
  { //< Returns the id of the ith reco track associated to the ideal track
    if (i < GetNofRecoTracks())
      return fRecoTrackIDs[i];
    return FairLink();
  }

  Int_t GetNofSttPoints() { return GetNofSttParalPoints() + GetNofSttSkewPoints(); }

  Int_t GetNofSttParalPoints() { return fMapBranchNameMCHits[TrackingQA::branchEnum::STTHitParal]; }
  Int_t GetNofSttSkewPoints() { return fMapBranchNameMCHits[TrackingQA::branchEnum::STTHitSkew]; }

  Int_t GetNofMCPoints(TString branchName) { return GetNofMCPoints(TrackingQA::StringToBranchEnum(branchName)); }

  Int_t GetNofMCPoints(TrackingQA::branchEnum branch)
  {
    if (branch == TrackingQA::branchEnum::STTHit) {
      return GetNofSttPoints();
    } else {
      if (fMapBranchNameMCHits.count(branch) > 0)
        return fMapBranchNameMCHits[branch];
      else
        return 0;
    }
  }

  Int_t GetNofMCPoints() const
  {
    int nPoints = 0;
    for (auto val : fMapBranchNameMCHits) {
      nPoints += val.second;
    }
    return nPoints;
  }

  TVector3 GetPositionFirst() const { return fPosFirst; }
  TVector3 GetMomentumFirst() const { return fMomFirst; }
  TVector3 GetPositionLast() const { return fPosLast; }
  TVector3 GetMomentumLast() const { return fMomLast; }
  Double_t GetdPCA2d() const { return fdPCA2d; }

  Int_t GetCharge() const { return fCharge; }
  Bool_t GetIsPrimary() const { return fIsPrimary; }
  TVector3 GetVertex() const { return fVertex; }
  TVector3 GetStartMomentum() const { return fMomentum; }
  Int_t GetPDGCode() const { return fPDGCode; }
  Int_t GetQuality() const { return fQuality; }
  Int_t GetMCQuality() const { return fMCQuality; }
  FairLink GetIdealTrackLink() const { return fIdealTrackLink; }
  std::map<TrackingQA::branchEnum, Int_t> GetMCHits() const { return fMapBranchNameMCHits; }

  void FindBestRecoTrack(TClonesArray *recoInfo);
  void FindBestRecoTrack(TClonesArray *recoInfo, std::function<bool(FairLink &, FairLink &)> function);

  friend std::ostream &operator<<(std::ostream &os, const PndTrackingQAMCInfo &mcInfo)
  {
    os << "MCTrack: " << mcInfo.GetMCTrackID() << " MCQuality: " << mcInfo.GetMCQuality() << std::endl;
    for (auto hits : mcInfo.fMapBranchNameMCHits) {
      os << "MCInfo " << TrackingQA::BranchEnumToString(hits.first) << " : " << hits.second << std::endl;
    }
    os << "RecoTrackIDs: ";
    for (auto trackID : mcInfo.fRecoTrackIDs)
      os << trackID << " ";
    os << std::endl;
    os << "AssociatedTrackID: " << mcInfo.fAssoRecoTrackID << " Quality: " << mcInfo.GetQuality() << std::endl;
    return os;
  }

 protected:
  void SetBestRecoTrack(TClonesArray *recoInfo);
  void SetRecoTrackIsClone(TClonesArray *recoInfo);

 private:
  std::map<TrackingQA::branchEnum, Int_t> fMapBranchNameMCHits; // branchID, MCHits

  FairLink fMCTrackID;
  std::vector<FairLink> fRecoTrackIDs;
  TVector3 fPosFirst, fMomFirst;
  TVector3 fPosLast, fMomLast;

  Int_t fCharge = -10; //< Charge from MCTrack
  Bool_t fIsPrimary = kFALSE;
  TVector3 fVertex = {-10000., -10000., -10000.}; //< Vertex position from MCTrack
  TVector3 fMomentum;                             //< Momentum at vertex position from MCTrack
  Int_t fPDGCode = 0;                             //< PDG code from MCTrack
  Double_t fdPCA2d = 0.;                          //< Distance	of closest approach of track circle to primary vertex in xy plane

  Int_t fQuality = 0;
  Int_t fMCQuality = 0;
  FairLink fAssoRecoTrackID; //< Id of the reco track with the highest efficiency (or highest purity if two tracks with identical eff. exist)
  FairLink fIdealTrackLink;
  ClassDef(PndTrackingQAMCInfo, 3);
};
