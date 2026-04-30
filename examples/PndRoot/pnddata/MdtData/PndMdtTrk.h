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

#ifndef PNDMDTTRK_HH
#define PNDMDTTRK_HH

#include "FairHit.h"
#include "FairMultiLinkedData_Interface.h"
#include "PndTrackCand.h"
#include "PndDetectorList.h"
#include "TVector3.h"

#include <iostream>

using std::cout;
using std::endl;

class PndMdtTrk : public FairMultiLinkedData_Interface {

 public:
  PndMdtTrk();

  virtual ~PndMdtTrk();

  /** Output to screen (not yet implemented) **/
  virtual void Print(const Option_t *opt = nullptr) const
  {
    std::cout << " opt = " << opt << std::endl;
    return;
  }

  /** Public method Clear
   ** Resets the flag to -1
   **/
  void Clear();

  /** Accessors **/
  void GetHitList(Int_t *hit)
  {
    for (Int_t ii = 0; ii < 25; ii++)
      hit[ii] = fHitList[ii];
  };
  void GetHitMult(Int_t *hit)
  {
    for (Int_t ii = 0; ii < 25; ii++)
      hit[ii] = fHitMult[ii];
  };
  void GetHitDist(Float_t *hit)
  {
    for (Int_t ii = 0; ii < 25; ii++)
      hit[ii] = fHitDist[ii];
  };
  void GetHitDeltaAngle(Float_t *hit)
  {
    for (Int_t ii = 0; ii < 25; ii++)
      hit[ii] = fHitDeltaAngle[ii];
  };
  Int_t GetHitIndex(Int_t lay) const { return fHitList[lay]; };
  Int_t GetHitMult(Int_t lay) const { return fHitMult[lay]; };
  Float_t GetHitDist(Int_t lay) const { return fHitDist[lay]; };
  Float_t GetHitDeltaAngle(Int_t lay) const { return fHitDeltaAngle[lay]; };
  Float_t GetLayerDist(Int_t lay) const { return fLayDist[lay]; };
  Float_t GetIronDist() const { return fIronDist; };
  Int_t GetHitCount() const { return fHitCount; };
  Int_t GetLayerCount() const { return fLayerCount; };
  Int_t GetMaxLayer() const { return fMaxLayer; };
  Int_t GetHitBit() const { return fHitBit; };
  Int_t GetHitBit(Int_t lay) const { return (fHitBit & (1 << lay)); };
  Int_t GetModule() const { return fModule; };
  Float_t GetChi2() const { return fChi2; };

  PndTrackCand *AddTrackCand(const PndTrackCand *inTrackCand = new PndTrackCand());

  /** Modifiers **/
  void SetHitIndex(Int_t lay, Int_t trackId);
  void SetHitMult(Int_t lay, Int_t mult);
  void SetHitDist(Int_t lay, Float_t dist);
  void SetHitDeltaAngle(Int_t lay, Float_t angle);
  void SetLayerDist(Int_t lay, Float_t dist);
  void SetBit(Int_t lay) { fHitBit = fHitBit | (1 << lay); };

  void SetIronDist(Float_t dist) { fIronDist = dist; };
  void SetHitCount(Int_t hit) { fHitCount = hit; };
  void SetLayerCount(Int_t lay) { fLayerCount = lay; };
  void SetMaxLayer(Int_t lay) { fMaxLayer = lay; };
  void SetHitBit(Int_t bit) { fHitBit = bit; };
  void SetModule(Int_t mod) { fModule = mod; };
  void SetChi2(Float_t chi2) { fChi2 = chi2; };

 private:
  Int_t fHitList[25];   // List of indexes of MdtHit
  Int_t fHitMult[25];   // Number of MdtHits inside the correlation
  Float_t fHitDist[25]; // Distance of the closest point to the previous layer hit
  Float_t fHitDeltaAngle[25];
  Float_t fLayDist[25]; // Distance of the actual layer from the previous one
  Float_t fIronDist;    // Amount of crossed iron [cm]
  Int_t fHitCount;      // Number of hits inside correlation
  Int_t fLayerCount;    // Number of fired layers
  Int_t fMaxLayer;      // Lat layer fired
  Int_t fHitBit;        // Layer Bit
  Int_t fModule;        // Module number of the first hit
  Float_t fChi2;        // global chi2 of the MDT correlation

  ClassDef(PndMdtTrk, 1);
};

inline void PndMdtTrk::SetHitIndex(Int_t lay, Int_t trackId)
{
  if (lay > 25) {
    cout << " -E- PndMdtTrk::SetHitIndex: Layer > 25 !!!!!" << endl;
  } else {
    fHitList[lay] = trackId;
    SetBit(lay);
    AddLink(FairLink("MdtHit", trackId)); // 21.09.10 Stefano: fix for link
  }
}

inline void PndMdtTrk::SetHitMult(Int_t lay, Int_t mult)
{
  if (lay > 25) {
    cout << " -E- PndMdtTrk::SetHitMult: Layer > 25 !!!!!" << endl;
  } else {
    fHitMult[lay] = mult;
  }
}

inline void PndMdtTrk::SetHitDist(Int_t lay, Float_t dist)
{
  if (lay > 25) {
    cout << " -E- PndMdtTrk::SetHitDist: Layer > 25 !!!!!" << endl;
  } else {
    fHitDist[lay] = dist;
  }
}

inline void PndMdtTrk::SetHitDeltaAngle(Int_t lay, Float_t dist)
{
  if (lay > 25) {
    cout << " -E- PndMdtTrk::SetHitDeltaAngle: Layer > 25 !!!!!" << endl;
  } else {
    fHitDeltaAngle[lay] = dist;
  }
}

inline void PndMdtTrk::SetLayerDist(Int_t lay, Float_t dist)
{
  if (lay > 25) {
    cout << " -E- PndMdtTrk::SetLayerDist: Layer > 25 !!!!!" << endl;
  } else {
    fLayDist[lay] = dist;
  }
}

#endif
