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

//-------------------------------------------------------------------------
// Author:      Oliver Merle (Oliver.Merle@exp2.physik.uni-giessen.de)
// Changes:     Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Date:        30.11.2015
// Description: Information of charged particle tracks
//-------------------------------------------------------------------------

#ifndef PNDDISCPARTICLEMCPOINT_HH
#define PNDDISCPARTICLEMCPOINT_HH

#include "PndMCPoint.h"

class PndDiscParticleMCPoint : public PndMCPoint {
 public:
  PndDiscParticleMCPoint();
  // trackID, detID, posin, momin, tof, length, eLoss
  PndDiscParticleMCPoint(Int_t track_id,        // base
                                                // Int_t event_id,      // in FairMCEventHeader
                         Int_t det_id,          // base
                         Int_t volume_id,       // -- use some kind of volume identifier here (radiator, lightguide etc) (PndGeoHandling short id??)
                         TVector3 pos_in,       // base
                         TVector3 mom_in,       // base
                         TVector3 pos_out,      // base
                         TVector3 mom_out,      // base
                         Double_t total_tof,    // base
                         Double_t total_length, // base
                         Double_t energy_loss, Double_t charge, Double_t mass, Int_t pdgcode,
                         Bool_t pos_in_inside, // whether the particle was entering the volume at pos_in
                         Bool_t is_primary);

  virtual ~PndDiscParticleMCPoint();

  virtual void Print(const Option_t *opt = nullptr) const; /// FairMCPoint forces the implementation

  void SetVolumeID(Int_t val) { fVolume_id = val; }
  void SetCharge(Double_t val) { fCharge = val; }
  void SetMass(Double_t val) { fMass = val; }
  void SetPdgCode(Int_t val) { fPdgCode = val; }
  void SetPosInInside(Bool_t val) { fPos_in_inside = val; }
  void SetIsPrimary(Bool_t val) { fIs_primary = val; }
  void SetMyEnergyLoss(Double_t val) { fMy_energy_loss = val; }

  Int_t GetVolumeID() const { return fVolume_id; }
  Double_t GetCharge() const { return fCharge; }
  Double_t GetMass() const { return fMass; }
  Int_t GetPdgCode() const { return fPdgCode; }
  Bool_t GetPosInInside() const { return fPos_in_inside; }
  Bool_t GetIsPrimary() const { return fIs_primary; }
  Double_t GetMyEnergyLoss() const { return fMy_energy_loss; }

 protected:
  Int_t fVolume_id = 0;
  Double_t fCharge = 0.; // todo: this doubles the information of the particel and the MC track. Should be removed
  Double_t fMass = 0.;
  Int_t fPdgCode = 0;
  Bool_t fPos_in_inside = false;
  Bool_t fIs_primary = false;

  Double_t fMy_energy_loss = 0.;

  ClassDef(PndDiscParticleMCPoint, 3)
};

#endif // PNDDISCPARTICLEMCPOINT_HH
