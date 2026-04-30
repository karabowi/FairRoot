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

#include "PndDiscParticleMCPoint.h"

#include <iostream>

ClassImp(PndDiscParticleMCPoint)

  PndDiscParticleMCPoint::PndDiscParticleMCPoint()
  : PndMCPoint()
{
}

PndDiscParticleMCPoint::PndDiscParticleMCPoint(Int_t track_id_, Int_t det_id_, Int_t volume_id_, TVector3 pos_in_, TVector3 mom_in_, TVector3 pos_out_, TVector3 mom_out_,
                                               Double_t total_tof_, Double_t total_length_, Double_t energy_loss_, Double_t charge_, Double_t mass_, Int_t pdgCode_,
                                               Bool_t pos_in_inside_, Bool_t is_primary_)

  : PndMCPoint(track_id_, det_id_, pos_in_, pos_out_, mom_in_, mom_out_, total_tof_, total_length_, energy_loss_), fVolume_id(volume_id_), fCharge(charge_), fMass(mass_),
    fPdgCode(pdgCode_), fPos_in_inside(pos_in_inside_), fIs_primary(is_primary_), fMy_energy_loss(energy_loss_)
{
}

PndDiscParticleMCPoint::~PndDiscParticleMCPoint() {}

void PndDiscParticleMCPoint::Print(const Option_t *opt) const
{
  std::cout << "-I- PndDiscParticleMCPoint"
            << "  fTrackID      " << fTrackID << std::endl
            << "  fDetectorID   " << fDetectorID << std::endl
            << "  Position in   " << fX << "; " << fY << "; " << fZ << std::endl
            << "  Momentum in   " << fPx << "; " << fPy << "; " << fPz << std::endl
            << "  Position out  " << fXOut << "; " << fYOut << "; " << fZOut << std::endl
            << "  Momentum out  " << fPxOut << "; " << fPyOut << "; " << fPzOut << std::endl
            << "  fTime         " << fTime << std::endl
            << "  Length        " << fLength << std::endl
            << "  fELoss        " << fELoss << std::endl
            << "  volume_id     " << fVolume_id << std::endl
            << "  charge        " << fCharge << std::endl
            << "  mass          " << fMass << std::endl
            << "  pdgCode       " << fPdgCode << std::endl
            << "  Pos in inside " << fPos_in_inside << std::endl
            << "  is primary    " << fIs_primary << std::endl
            << "  Option is     " << opt << std::endl;
}
