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

#ifndef PndMvdSimplePidAlgo_h
#define PndMvdSimplePidAlgo_h

#include <math.h>
#include <map>
#include "Rtypes.h"

class PndMvdPidCand;

class PndMvdSimplePidAlgo {
 public:
  // Write likelihoods to PndMvdPidCand
  static void CalcLikelihood(PndMvdPidCand *cand);

 private:
  // Lower boundary of energy loss distribution, as given by Bethe-Bloch formula
  static double LowerBoundary(double p, double m);

  // These functions represent the lower boundary of the energy loss distribution
  // at a given momentum. The offset numbers in both momentum and energy loss
  // axis are adjusted that at least 99% of all events have are gathered.
  static double LowerProtonBoundary(double momentum);

  static double LowerKaonBoundary(double momentum);

  static double LowerMuonBoundary(double momentum);

  // Naive approximation of the Landau distribution, integrated from -inf to x
  static double LandauIntegral(double x);

  // Constants
  static float fpiMass;
  static float fkMass;
  static float fpMass;
  static float fmuMass;
  static float feMass;
  static float fc;
  static float feb;

  // Landau distribution parameters
  static double fkShift;
  static double fkScale;

  static double fpiShift;
  static double fpiScale;

  static double feShift;
  static double feScale;
};

#endif
