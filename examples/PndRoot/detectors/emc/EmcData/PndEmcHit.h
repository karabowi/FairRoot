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

// -------------------------------------------------------------------------
// -----                     PndEmcHit header file                     -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------

/**  PndEmcHit.h
 *@author S.Spataro
 **
 ** Hits in EMC
 **/

//#pragma once
#ifndef PNDEMCHIT_H
#define PNDEMCHIT_H

#include "FairHit.h"
#include "TMath.h"
#include <vector>
#include <map>
#include <algorithm>

class PndEmcPoint;

/**
 * @brief represents the deposited energy of one emc crystal from simulation
 * @ingroup PndEmc
 */ 
class PndEmcHit : public FairHit {
 public:
  /** Default constructor **/
  PndEmcHit();

  PndEmcHit(Int_t trackid, Int_t id, Float_t energy, Float_t time, Float_t X, Float_t Y, Float_t Z);

  PndEmcHit(Int_t trackid, Int_t id, Float_t energy, Float_t time, Float_t X, Float_t Y, Float_t Z, std::vector<Int_t> McTruth, FairMultiLinkedData enteringTrack,
            FairMultiLinkedData exitingTrack, std::set<Int_t> clusterList);

  /** Copy **/
  PndEmcHit(const PndEmcHit &);

  /** Destructor **/
  virtual ~PndEmcHit();

  /** Output to screen **/
  void Print(const Option_t *opt = "") const;

  /** Modifiers **/
  void SetEnergy(Double32_t energy) { fEnergy = energy; };
  void SetTime(Double32_t time) { fTimeStamp = time * 1E9; };
  void SetDepositedEnergyMap(const std::map<Int_t, Double_t> &depEnergy) { fDepositedEnergy = depEnergy; }
  void AddDepositedEnergy(Int_t mcId, Double_t depEnergy) { fDepositedEnergy[mcId] += depEnergy; }

  /** Accessors **/
  Double_t GetEnergy() const { return fEnergy; };
  Double_t GetTime() const { return fTimeStamp / 1E9; };
  Float_t GetTheta() const { return fX == 0.0 && fY == 0.0 && fZ == 0.0 ? 0.0 : TMath::ATan2(sqrt(fX * fX + fY * fY), fZ) * TMath::RadToDeg(); };
  Float_t GetPhi() const { return fX == 0.0 && fY == 0.0 ? 0.0 : TMath::ATan2(fY, fX) * TMath::RadToDeg(); };
  Short_t GetModule() const { return (fDetectorID / 100000000); };
  Short_t GetRow() const { return ((fDetectorID / 1000000) % 100); };
  Short_t GetCrystal() const { return (fDetectorID % 10000); };
  Short_t GetCopy() const { return ((fDetectorID / 10000) % 100); };

  Short_t GetXPad() const;
  Short_t GetYPad() const;
  FairMultiLinkedData GetTrackEntering() const { return fTrackEntering; }
  FairMultiLinkedData GetTrackExiting() const { return fTrackExiting; }

  const std::vector<Int_t> &GetMcList() { return fMcList; }
  std::set<Int_t> GetClusterList() { return fClusterList; }

  void AddClusterID(int clusterId) { fClusterList.insert(clusterId); }

  std::map<Int_t, Double_t> GetDepositedEnergyMap() const { return fDepositedEnergy; };

 protected:
  Double32_t fEnergy; // hit amplitude

  std::vector<Int_t> fMcList;                 //  Mc TrackIndex contributed to hit
  FairMultiLinkedData fTrackEntering;         // Links to tracks entering the crystal
  FairMultiLinkedData fTrackExiting;          // Links to tracks exiting the crystal
  std::set<Int_t> fClusterList;               // To which cluster belongs the hit based on MC information (could be more than one)
  std::map<Int_t, Double_t> fDepositedEnergy; // MC deposited energy per (primary) MC particle

  ClassDef(PndEmcHit, 4)
};

#endif // PNDEMCHIT_H
