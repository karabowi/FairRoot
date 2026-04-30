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
// Description: Information of photons
//-------------------------------------------------------------------------

#include "PndDiscSensorMCPoint.h"

ClassImp(PndDiscSensorMCPoint)

  PndDiscSensorMCPoint::PndDiscSensorMCPoint()
  : FairMCPoint(), volume_id(0), internal_reflection_angle(0.), track_start_time(0.), my_track_id(0), photon_entering_pos(0., 0., 0.), photon_entering_momentum(0., 0., 0.)
{
}

PndDiscSensorMCPoint::PndDiscSensorMCPoint(Int_t track_id_, Int_t det_id_, Int_t volume_id_, Double_t const &internal_reflection_angle_, TVector3 const &pos_in_,
                                           TVector3 const &mom_in_, Double_t const &total_tof_, Double_t const &total_length_, Double_t const &energy_loss_,
                                           Double_t const &track_start_time_)

  : FairMCPoint(track_id_, det_id_, pos_in_, mom_in_, total_tof_, total_length_, energy_loss_), volume_id(volume_id_), internal_reflection_angle(internal_reflection_angle_),
    track_start_time(track_start_time_), my_track_id(track_id_), photon_entering_pos(0., 0., 0.), photon_entering_momentum(0., 0., 0.)
{
}

PndDiscSensorMCPoint::~PndDiscSensorMCPoint() {}

void PndDiscSensorMCPoint::Print(const Option_t *opt) const
{
  // Logger or cout ? There should be only one target !
  std::cout << "-I- PndDiscSensorMCPoint"
            << "  fTrackID      " << fTrackID << std::endl
            << "  fDetectorID   " << fDetectorID << std::endl
            << "  Position      " << fX << "; " << fY << "; " << fZ << std::endl
            << "  Momentum      " << fPx << "; " << fPy << "; " << fPz << std::endl
            << "  fTime         " << fTime << std::endl
            << "  Length        " << fLength << std::endl
            << "  fELoss        " << fELoss << std::endl
            << "  volume_id     " << volume_id << std::endl
            << "  irefl. angle  " << internal_reflection_angle << std::endl
            << "  Option is     " << opt << std::endl;
}
