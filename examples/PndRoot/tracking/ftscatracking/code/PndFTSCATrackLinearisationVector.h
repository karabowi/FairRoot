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

// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef PNDFTSCATRACKLINEARISATIONVECTOR_H
#define PNDFTSCATRACKLINEARISATIONVECTOR_H

#include "PndFTSCATrackParamVector.h"
#include "PndFTSVector.h"

/**
 * @class PndFTSCATrackLinearisationVector
 *
 * PndFTSCATrackLinearisationVector class describes the parameters which are used
 * to linearise the transport equations for the track trajectory.
 *
 * The class is used during track (re)fit, when the PndFTSTrackParam track is only
 * partially fitted, and there is some apriory knowledge about trajectory.
 * This apriory knowledge is used to linearise the transport equations.
 *
 * In case the track is fully fitted, the best linearisation point is
 * the track trajectory itself (PndFTSCATrackLinearisationVector = PndFTSTrackParamVector ).
 *
 */
class PndFTSCATrackLinearisationVector {
 public:
  PndFTSCATrackLinearisationVector() : fSinPhi(Vc::Zero), fCosPhi(1), fDzDs(Vc::Zero), fQPt(Vc::Zero) {}

  PndFTSCATrackLinearisationVector(float_v SinPhi1, float_v CosPhi1, float_v DzDs1, float_v QPt1) : fSinPhi(SinPhi1), fCosPhi(CosPhi1), fDzDs(DzDs1), fQPt(QPt1) {}

  PndFTSCATrackLinearisationVector(const PndFTSCATrackParamVector &t);

  void Set(float_v SinPhi1, float_v CosPhi1, float_v DzDs1, float_v QPt1);

  float_v SinPhi() const { return fSinPhi; }
  float_v CosPhi() const { return fCosPhi; }
  float_v DzDs() const { return fDzDs; }
  float_v QPt() const { return fQPt; }

  float_v GetSinPhi() const { return fSinPhi; }
  float_v GetCosPhi() const { return fCosPhi; }
  float_v GetDzDs() const { return fDzDs; }
  float_v GetQPt() const { return fQPt; }

  void SetSinPhi(float_v v) { fSinPhi = v; }
  void SetCosPhi(float_v v) { fCosPhi = v; }
  void SetDzDs(float_v v) { fDzDs = v; }
  void SetQPt(float_v v) { fQPt = v; }

 private:
  float_v fSinPhi; // SinPhi
  float_v fCosPhi; // CosPhi
  float_v fDzDs;   // DzDs
  float_v fQPt;    // QPt
};

inline PndFTSCATrackLinearisationVector::PndFTSCATrackLinearisationVector(const PndFTSCATrackParamVector &t)
  : fSinPhi(t.SinPhi()), fCosPhi(Vc::Zero), fDzDs(t.DzDs()), fQPt(t.QPt())
{
  fSinPhi = CAMath::Max(CAMath::Min(fSinPhi, float_v(.999f)), float_v(-.999f));
  fCosPhi = t.SignCosPhi() * CAMath::Sqrt(float_v(Vc::One) - fSinPhi * fSinPhi);
}

inline void PndFTSCATrackLinearisationVector::Set(float_v SinPhi1, float_v CosPhi1, float_v DzDs1, float_v QPt1)
{
  SetSinPhi(SinPhi1);
  SetCosPhi(CosPhi1);
  SetDzDs(DzDs1);
  SetQPt(QPt1);
}

#endif
