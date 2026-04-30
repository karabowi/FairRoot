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

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	EMC Digi.
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Xiaorong Shi            Lawrence Livermore National Lab
//	Steve Playfer           University of Edinburgh
//	Stephen Gowdy           University of Edinburgh
// Dima Melnichuk - adaption for PANDA
//
// Copyright Information:
//	Copyright (C) 1994	Lawrence Livermore National Lab
//
///////////////////////////////////////////////////////////////
#ifndef BSEMCDIGI_HH
#define BSEMCDIGI_HH

#include <iosfwd> // for ostream
#include <ostream>
#include <string> // for string

#include "Rtypes.h"     // for BSEmcDigi::Class, BSEmcDigi::Streamer
#include "RtypesCore.h" // for Double_t, Int_t, Bool_t, Float_t, Option_t

#include "FairTimeStamp.h" // for FairTimeStamp

#include "BSEmcDataBranchNames.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief represents the reconstructed hit of one emc crystal
 * @author Xiaorong Shi            Lawrence Livermore National Lab
 * @author Steve Playfer           University of Edinburgh
 * @author Stephen Gowdy           University of Edinburgh
 * @author Dima Melnichuk - adaption for PANDA
 * @ingroup EmcData
 */
class BSEmcDigi : public FairTimeStamp {

  friend std::ostream &operator<<(std::ostream &t_out, const BSEmcDigi &t_digi)
  {
    t_out << "BSEmc Digi in crystal: " << t_digi.GetDetectorId() << " energy: " << t_digi.GetEnergy() << " timestamp: " << t_digi.GetTimeStamp();
    return t_out;
  }

 public:
  enum class eGAIN : Int_t { kNONE, kHIGH, kLOW };

  /** Default constructor **/
  BSEmcDigi();
  BSEmcDigi(Int_t t_id, Double_t t_energy, Float_t t_time);
  BSEmcDigi(const BSEmcDigi &);

  /** Destructor **/
  virtual ~BSEmcDigi();
  // Operators

  virtual Bool_t operator==(const BSEmcDigi &t_otherDigi) const;
  virtual Bool_t operator!=(const BSEmcDigi &t_otherDigi) const;
  virtual Bool_t operator<(const BSEmcDigi &t_otherDigi) const;
  virtual Bool_t equal(FairTimeStamp *t_data);

  /** Output to screen **/
  virtual void Print(const Option_t *t_opt = "") const;

  void SetEnergy(Double_t t_energy) { fEnergy = t_energy; };
  virtual Double_t GetEnergy() const { return fEnergy; };

  void SetRawEnergy(Double_t t_energy) { fRawEnergyBin = t_energy; };
  Double_t GetRawEnergy() const { return fRawEnergyBin; }

  void SetDetectorId(Int_t t_detectorId) { fDetectorId = t_detectorId; }
  Int_t GetDetectorId() const { return fDetectorId; };

  void SetGainType(eGAIN t_type) { fGain = t_type; }
  eGAIN GetGainType() const { return fGain; }

  void SetDCNumber(Int_t t_dcId) { fDCNumber = t_dcId; }
  Int_t GetDCNumber() const { return fDCNumber; };

 protected:
  Double_t fRawEnergyBin{-1};
  Double_t fEnergy{-1}; // digi amplitude
  Int_t fDetectorId{-1};
  eGAIN fGain{eGAIN::kNONE};
  Int_t fDCNumber{-1};
  ClassDef(BSEmcDigi, 5);
};

#endif /*BSEMCDIGI_HH*/
