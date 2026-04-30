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
// -----                     BSEmcMCDeposit header file -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------

#ifndef BSEMCMCDEPOSIT_HH
#define BSEMCMCDEPOSIT_HH

#include <map> // for map
#include <set>
#include <string> // for string
#include <vector>

#include "Rtypes.h"     // for BSEmcMCDeposit::Class, BSEmcMCDeposit::Stre...
#include "RtypesCore.h" // for Int_t, Double_t, Double32_t, Option_t

#include "FairHit.h"             // for FairHit
#include "FairMultiLinkedData.h" // for FairMultiLinkedData

#include "BSEmcDataBranchNames.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief represents the deposited energy of one emc crystal from simulation
 * @author S.Spataro
 * @ingroup EmcData
 */
// class BSEmcMCDeposit : public FairTimeStamp
class BSEmcMCDeposit : public FairHit {
 public:
  /** Default constructor **/
  BSEmcMCDeposit();

  BSEmcMCDeposit(Int_t t_id, Float_t t_time, std::vector<Int_t> t_mcTruth, FairMultiLinkedData t_enteringTrack, FairMultiLinkedData t_exitingTrack);

  /** Copy **/
  BSEmcMCDeposit(const BSEmcMCDeposit &);

  /** Destructor **/
  virtual ~BSEmcMCDeposit();

  /** Output to screen **/
  virtual void Print(const Option_t *t_opt = "") const;

  /** Modifiers **/
  // virtual void SetEnergy(Double32_t energy) { fEnergy = energy ;};
  virtual void SetTime(Double32_t t_time) { fTimeStamp = t_time * 1E9; };

  /** Accessors **/
  virtual Double_t GetEnergy() const;
  virtual Double_t GetTime() const { return fTimeStamp / 1E9; };

  FairMultiLinkedData GetTrackEntering() const { return fTrackEntering; }
  FairMultiLinkedData GetTrackExiting() const { return fTrackExiting; }

  const std::vector<Int_t> &GetMcList() { return fMcList; }

  Int_t GetDetectorID() const { return fDetectorID; }
  void SetDetectorID(Int_t t_detectorId) { fDetectorID = t_detectorId; }

  Int_t GetDetectorId() const { return fDetectorID; }
  void SetDetectorId(Int_t t_detectorId) { fDetectorID = t_detectorId; }

  void SetDepositedEnergyMap(const std::map<Int_t, Double_t> &t_depEnergy) { fDepositedEnergy = t_depEnergy; }
  void AddDepositedEnergy(Int_t t_mcId, Double_t t_depEnergy) { fDepositedEnergy[t_mcId] += t_depEnergy; }

  std::set<Int_t> GetClusterList() const;
  const std::map<Int_t, Double_t> &GetDepositedEnergyMap() const { return fDepositedEnergy; }

 protected:
  std::map<Int_t, Double_t> fDepositedEnergy{}; // MC deposited energy per (primary) MC particle
  std::vector<Int_t> fMcList{};                 //  Mc TrackIndex contributed to hit (including low level tracks (aka not just cluster causing tracks))
  FairMultiLinkedData fTrackEntering{};         // Links to tracks entering the crystal (including low level tracks (aka not just cluster causing tracks))
  FairMultiLinkedData fTrackExiting{};          // Links to tracks exiting the crystal (including low level tracks (aka not just cluster causing tracks))

  Int_t fDetectorID{-1};

  ClassDef(BSEmcMCDeposit, 3)
};

#endif /*BSEMCMCDEPOSIT_HH*/
