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

/** PndTrackingQualityMCInfo
 **
 ** Data container for the result of the ideal track finder and the information of the associated MCTrack
 **
 ** @author Lia Lavezzi with modifications by Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 **/

#ifndef PNDTRACKINGQUALITYMCINFO_H
#define PNDTRACKINGQUALITYMCINFO_H

// #include "PndTrack.h"
#include "TVector3.h"
#include "TArrayI.h"

class PndTrackingQualityMCInfo : public TObject {

 public:
  PndTrackingQualityMCInfo();
  PndTrackingQualityMCInfo(int nofmvdpix, int nofmvdstr, int nofsttparal, int nofsttskew, int nofgem, int noffts);
  PndTrackingQualityMCInfo(const PndTrackingQualityMCInfo &info);
  ~PndTrackingQualityMCInfo();

  PndTrackingQualityMCInfo &operator=(const PndTrackingQualityMCInfo &info);

  void SetPositionFirst(TVector3 pos) { fPosFirst = pos; }
  void SetMomentumFirst(TVector3 mom) { fMomFirst = mom; }
  void SetPositionLast(TVector3 pos) { fPosLast = pos; }
  void SetMomentumLast(TVector3 mom) { fMomLast = mom; }

  void SetCharge(int ch) { fCharge = ch; }
  void SetIsPrimary(Bool_t val) { fIsPrimary = val; }
  void SetVertex(TVector3 val) { fVertex = val; }
  void SetMomentum(TVector3 val) { fMomentum = val; }

  Int_t GetNofMvdPixelPoints() { return fNofMvdPixelPoints; }
  Int_t GetNofMvdStripPoints() { return fNofMvdStripPoints; }
  Int_t GetNofMvdPoints() { return fNofMvdPixelPoints + fNofMvdStripPoints; }
  Int_t GetNofSttParalPoints() { return fNofSttParalPoints; }
  Int_t GetNofSttSkewPoints() { return fNofSttSkewPoints; }
  Int_t GetNofSttPoints() { return fNofSttParalPoints + fNofSttSkewPoints; }
  Int_t GetNofGemPoints() { return fNofGemPoints; }
  Int_t GetNofFtsPoints() { return fNofFtsPoints; }
  Int_t GetNofMCPoints() { return fNofMvdPixelPoints + fNofMvdStripPoints + fNofSttParalPoints + fNofSttSkewPoints + fNofGemPoints + fNofFtsPoints; }

  Int_t GetMCTrackID() { return fMCTrackID; }                           //< Returns the corresponding MCTrack to an ideal track
  Short_t GetNofRecoTracks(void) const { return fRecoTrackIDs.size(); } //< Returns the number of reco tracks associated to the ideal track
  Int_t GetRecoTrackID(Int_t i = 0) const
  { //< Returns the id of the ith reco track associated to the ideal track
    if (i < GetNofRecoTracks())
      return fRecoTrackIDs[i];
    return -1;
  }
  Int_t GetAssoRecoTrackID() const { return fAssoRecoTrackID; }

  TVector3 GetPositionFirst() { return fPosFirst; }
  TVector3 GetMomentumFirst() { return fMomFirst; }
  TVector3 GetPositionLast() { return fPosLast; }
  TVector3 GetMomentumLast() { return fMomLast; }

  Int_t GetCharge() { return fCharge; }
  Bool_t GetIsPrimary() { return fIsPrimary; }
  TVector3 GetVertex() { return fVertex; }
  TVector3 GetStartMomentum() { return fMomentum; }

  void SetPDGCode(int pdg) { fPDGCode = pdg; }
  Int_t GetPDGCode() { return fPDGCode; }

  void SetReconstructabilityStatus(Int_t reco) { fReconstructabilityStatus = reco; }
  Int_t GetReconstructabilityStatus() { return fReconstructabilityStatus; }

  void SetMCTrackID(Int_t mctrackid) { fMCTrackID = mctrackid; }
  void SetRecoTrackIDs(const std::vector<int> recotrkids) { fRecoTrackIDs = recotrkids; }
  void SetRecoTrackID(int recotrkid) { fRecoTrackIDs.push_back(recotrkid); }
  void SetAssoRecoTrackID(int asso) { fAssoRecoTrackID = asso; }
  Bool_t IsReconstructed() { return GetNofRecoTracks() > 0; }

  void SetQuality(int quality) { fQuality = quality; }
  Int_t GetQuality() { return fQuality; }

  void SetMCQuality(int mcquality) { fMCQuality = mcquality; }
  Int_t GetMCQuality() { return fMCQuality; }

 protected:
  // PndTrack* fAssociatedIdealTrack; // This pointer is used in the time based simulation in order to connect the MC trackInfo to the ideal track

  Int_t fNofMvdPixelPoints, fNofMvdStripPoints, fNofSttParalPoints, fNofSttSkewPoints, fNofGemPoints, fNofFtsPoints;

  Bool_t fReconstructabilityStatus;
  Int_t fMCTrackID;
  std::vector<int> fRecoTrackIDs;
  TVector3 fPosFirst, fMomFirst;
  TVector3 fPosLast, fMomLast;

  Int_t fCharge; //< Charge from MCTrack
  Bool_t fIsPrimary;
  TVector3 fVertex;   //< Vertex position from MCTrack
  TVector3 fMomentum; //< Momentum at vertex position from MCTrack
  Int_t fPDGCode;     //< PDG code from MCTrack

  Int_t fQuality;
  Int_t fMCQuality;
  Int_t fAssoRecoTrackID; //< Id of the reco track with the highest efficiency (or highest purity if two tracks with identical eff. exist)

  ClassDef(PndTrackingQualityMCInfo, 3);
};

#endif
