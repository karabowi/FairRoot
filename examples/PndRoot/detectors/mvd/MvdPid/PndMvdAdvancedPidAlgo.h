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

#ifndef PndMvdAdvancedPidAlgo_h
#define PndMvdAdvancedPidAlgo_h

#include <math.h>
#include <map>
#include "Rtypes.h"

class PndMvdPidCand;

class PndMvdAdvancedPidAlgo {
 public:
  // Local pid type (BABAR)
  enum { kNPidType = 5 };
  enum PidType { kElectron = 0, kMuon, kPion, kKaon, kProton };

  // Write likelihoods to PndMvdPidCand
  static void CalcLikelihood(PndMvdPidCand *cand);

  // Same as CalcLikelihood but create random energyloss first
  static void CalcLikelihood(PidType particle, double momentum, PndMvdPidCand *cand);

  // Same as CalcLikelihood but write lhs to array instead
  static void CalcLikelihood(PidType part, double momentum, double *lh);

  // Same with a pdtid switch
  static void CalcLikelihood(int lundId, double momentum, double *lh);

  // Read last used energy loss and momentum
  static double GetMomentum() { return fmomentum; };
  static double GetEnergyLoss() { return fenergyloss; };

 private:
  // private contructor/destructor to prohibit instantiation
  PndMvdAdvancedPidAlgo(){};
  virtual ~PndMvdAdvancedPidAlgo(){};

  static void CalcLikelihood(double *lh);

  // Expected energy loss, as given by Bethe-Bloch formula
  static double MeanEnergyLoss(PidType particle);

  // Integral algorithm that computes convolution of landau and gaus
  // distribution; s_mpv is the difference between energy loss and
  // most probable value of the landau distribution, width1 is its
  // scaling, widht2 is the gaussian width.
  static double LandauGaus(double s_mpv, double width1, double width2);

  // Obtained by TrkFitter
  static double fmomentum;
  static double fenergyloss;

  // Distribution parameters depending on track momentum and particle
  static double mpv(PidType particle);
  static double width1(PidType particle);
  static double width2(PidType particle);

  // public:
  ClassDef(PndMvdAdvancedPidAlgo, 1);
};

#endif
