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

//-*- Mode: C++ -*-
// *****************************************************************************
//                                                                             *
// @Autors: I.Kulakov; M.Zyzak; I.Kisel                                        *
// @e-mail: I.Kulakov@gsi.de; M.Zyzak@gsi.de; I.Kisel@compeng.uni-frankfurt.de *
//                                                                             *
// *****************************************************************************

#include "PndCAFunctionality.h"
#include "PndCAHitsV.h"
#include "PndCATarget.h"

float_m PndCAFunctionality::FitIteration(const PndCAParam &caParam, const PndCAHits &hits, PndCATrackParamVector &param, const vector<PndCATESV> &iHits, const PndCATarget &target,
                                         bool dir, bool useParam, const float_m &mask)
{
  const unsigned int NTHits = iHits.size();
  float_m active = mask;
  if (active.isEmpty())
    return active;
  // get hits
  vector<PndCAHitV> thits(NTHits);
  for (unsigned int ihit = 0; ihit < NTHits; ihit++) {
    PndCATESV index = iHits[ihit];
    if (!dir)
      index = iHits[NTHits - 1 - ihit];

    PndCAHit hs[float_v::Size];
    foreach_bit(unsigned int iV, active) { hs[iV] = hits[index.s[iV]][index.e[iV]]; }

    thits[ihit] = PndCAHitV(hs, active);
  }

  const PndCAHitV &hit0 = thits[0];

  if (!useParam) {
    float_v qMom = param.QMomentum();
    param.InitByTarget(target);
    param.SetQMomentum(qMom);
    param.InitDirection(hit0.X0(), hit0.X1(), hit0.X2());
    param.SetAngle(hit0.Angle());
  } else {
    param.InitCovMatrix(target.Err2QMom());
  }

  for (unsigned int ihit = 0; ihit < NTHits; ihit++) {
    if (active.isEmpty())
      break;
    const PndCAHitV &hit = thits[ihit];
    active &= param.Transport(hit, caParam, active);
    if (active.isEmpty())
      break;
    active &= param.Filter(hit, caParam, active);
  }

  return active;
}

float_m PndCAFunctionality::Fit(const PndCAParam &caParam, const PndCAHits &hits, PndCATrackParamVector &param, const vector<PndCATESV> &iHits, const PndCATarget &target, bool dir,
                                const float_m &mask)
{
  float_m active = mask;
  int i = 0;

  float_v qMom = float_v(10e10f);

#if 1
  while ((++i < 10) && (abs((param.QMomentum() - qMom) / qMom) > 0.005f)) { // SG!!! 10 iterations !!!
    qMom = param.QMomentum();
    if (active.isEmpty())
      break;
    active &= FitIteration(caParam, hits, param, iHits, target, dir, false, active);
    break; // SG!!
  }
#else // for Panda it is and slower, since doesn't use target z-coor?
  active &= FitIteration(caParam, hits, param, iHits, target, dir, false, active);
  while ((++i < 5) && (abs((param.QMomentum() - qMom) / qMom) > 0.005f)) {
    qMom = param.QMomentum();
    active &= FitIteration(caParam, hits, param, iHits, target, !dir, true, active);
    active &= FitIteration(caParam, hits, param, iHits, target, dir, true, active);
  }
#endif
  return active;
}
