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

#ifndef PNDDISCSENSORMCPOINT_HH
#define PNDDISCSENSORMCPOINT_HH

// Fairroot / PROOT headers
#include "FairMCPoint.h"

// cpp headers
#include <math.h>

class PndDiscSensorMCPoint : public FairMCPoint {
 public:
  PndDiscSensorMCPoint();

  PndDiscSensorMCPoint(Int_t track_id,                            // base
                                                                  // Int_t event_id,      // in FairMCEventHeader
                       Int_t det_id,                              // base
                       Int_t volume_id,                           // -- use some kind of volume identifier here (or just sensor copy number?)
                       Double_t const &internal_reflection_angle, // may be 0.0 if none registered (e.g. not created in radiator)
                       TVector3 const &pos_in,                    // base
                       TVector3 const &mom_in,                    // base
                       Double_t const &total_tof,                 // base
                       Double_t const &total_length,              // base
                       Double_t const &energy_loss,               // base
                       Double_t const &track_start_time           // global time of track creation
  );

  virtual ~PndDiscSensorMCPoint();

  virtual void Print(const Option_t *opt = nullptr) const; /// FairMCPoint forces the implementation
  const Double_t &GetTotalReflectionAngle() { return internal_reflection_angle; }
  Double_t ComputeWavelenght() { return 1.239841939E-6 / sqrt(fPx * fPx + fPy * fPy + fPz * fPz); }
  Double_t ComputeTrackTof() { return fTime - track_start_time; }

  Int_t volume_id;
  Double_t internal_reflection_angle;
  Double_t track_start_time;
  Int_t my_track_id;

  TVector3 photon_entering_pos;      // position of photon entering optics
  TVector3 photon_entering_momentum; // momentum vector of photon entering optics

  ClassDef(PndDiscSensorMCPoint, 4)
};

#endif // PNDDISCSENSORMCPOINT_HH
