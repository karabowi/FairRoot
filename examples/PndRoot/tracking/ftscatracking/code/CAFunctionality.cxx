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
// @Autors: I.Kulakov; M.Pugach; M.Zyzak; I.Kisel                                        *
// @e-mail: I.Kulakov@gsi.de; M.Pugach@gsi.de; M.Zyzak@gsi.de; I.Kisel@compeng.uni-frankfurt.de *
//                                                                             *
// *****************************************************************************

#include "CAFunctionality.h"
#include "FTSCAHitsV.h"
#include "FTSCATarget.h"
#include "iostream"

float_m CAFunctionality::FitIteration(const PndFTSCAParam &caParam, const FTSCAHits &hits, PndFTSCATrackParamVector &param, const vector<TESV> &iHits, const FTSCATarget &target,
                                      bool dir, bool useParam, const float_m &mask)
{
  const int NTHits = iHits.size();
  float_m active = mask;

  // get hits
  vector<FTSCAHitV> thits(NTHits);
  for (unsigned short ihit = 0; ihit < NTHits; ihit++) {
    TESV index = iHits[ihit];
    if (!dir)
      index = iHits[NTHits - 1 - ihit];

    FTSCAHit hs[float_v::Size];
    foreach_bit(unsigned short iV, active) { hs[iV] = hits[index.s[iV]][index.e[iV]]; }
    thits[ihit] = FTSCAHitV(hs, active);
  }

  const FTSCAHitV &hit0 = thits[NTHits - 1];
  /*
  cout<<"active "<<active<<endl;
  cout<<"hit0.X0() "<<hit0.X0()<<endl;
  cout<<"hit0.X1() "<<hit0.X1()<<endl;
  cout<<"hit0.X2() "<<hit0.X2()<<endl;
  if (!useParam) {
    //float_v qMom = param.QMomentum();
    param.InitByTarget(target);
    //param.SetQMomentum(qMom, active);
    param.InitDirection( hit0.X0(), hit0.X1(), hit0.X2() );
    param.SetAngle( hit0.Angle() );
  }
  else {
    param.InitCovMatrix( target.Err2QMom() );
  }
  */
  if (useParam) {
    // NTHits-=1; DON'T FORGET TO DO THIS IF USEPARAM
    float_v qMom = param.QMomentum();
    /*float_v x = param.X();
    float_v y = param.Y();
    float_v z = param.Z();
    float_v tx = param.Tx();
    float_v ty = param.Ty();*/
    /*float_v c00 = param.Cov(0);
    float_v c11 = param.Cov(2);
    float_v c22 = param.Cov(5);
    float_v c33 = param.Cov(9);
    float_v c44 = param.Cov(14);*/
    /*float_v c[15];
    for (int i=0; i<15; i++)
    {
      c[i] = param.Cov(i);
    }*/
    param.InitByTarget(target);
    /*param.SetX(x, active);
    param.SetY(y, active);
    param.SetZ(z, active);
    param.SetTx(tx, active);
    param.SetTy(ty, active);*/
    param.SetQMomentum(qMom, active);
    cout << "param.QP() " << param.QP() << endl;
    /*param.SetCov(0,c00);
    param.SetCov(2,c11);
    param.SetCov(5,c22);
    param.SetCov(9,c33);
    param.SetCov(14,c44);*/
    /*for (int i=0; i<15; i++)
    {
      param.SetCov(i,c[i]);
    }*/
    // param.InitDirection( hit0.X0(), hit0.X1(), hit0.X2() );
    param.SetAngle(hit0.Angle());
    // cout<<param<<endl;
  } else {
    param.InitByTarget(target);
    param.InitDirection(hit0.X0(), hit0.X1(), hit0.X2());
    param.SetAngle(hit0.Angle());
  }
  /*cout<<"Before Fit "<<endl;
  cout<<param<<endl;
  param.PrintCovMat();
  */
  // fit bckwrd
  for (int ihit = NTHits - 1; ihit >= 0; ihit--) {
    const FTSCAHitV &hit = thits[ihit];
    active &= param.Transport(hit, caParam, active);
    active &= param.Filter(hit, caParam, active);
    /*if (useParam)
    {cout<<"1.after filter Z "<<param.Z()<<endl;
    cout<<param<<endl;
    param.PrintCovMat();
    }*/
    // cout<<"2.after filter NDF "<<param.NDF()<<" Chi2 "<<param.Chi2()<<endl;
  }
  // param.PrintCovMat();
  // param.InitCovMatrix(target.Err2QMom());
  /*int_v ndf;
  ndf(static_cast<int_m>(active)) = -4;
  param.SetNDF(ndf);
  param.SetChi2(0.f);
  //cout<<"2. fit bckwrd NDF "<<param.NDF()<<" Chi2 "<<param.Chi2()<<endl;
  for ( int ihit = 1; ihit < NTHits; ihit++ )
  {
    const FTSCAHitV& hit = thits[ihit];
    active &= param.Transport( hit, caParam, active );
    active &= param.Filter( hit, caParam, active );
    cout<<"2.after filter Z "<<param.Z()<<endl;
    cout<<param<<endl;
    //cout<<"1.after filter NDF "<<param.NDF()<<" Chi2 "<<param.Chi2()<<endl;
  }
  //param.PrintCovMat();
  //param.InitCovMatrix(target.Err2QMom());
  param.SetNDF(ndf);
  param.SetChi2(0.f);
  //cout<<"2. fit bckwrd NDF "<<param.NDF()<<" Chi2 "<<param.Chi2()<<endl;
  //fit bckwd
  for ( int ihit = NTHits-2; ihit >= 0; ihit-- )
  {
    const FTSCAHitV& hit = thits[ihit];
    active &= param.Transport( hit, caParam, active );
    active &= param.Filter( hit, caParam, active );
    cout<<"3.after filter Z "<<param.Z()<<endl;
    cout<<param<<endl;
    //cout<<"2.after filter NDF "<<param.NDF()<<" Chi2 "<<param.Chi2()<<endl;
  }

  param.SetNDF(ndf);
  param.SetChi2(0.f);
  //cout<<"2. fit bckwrd NDF "<<param.NDF()<<" Chi2 "<<param.Chi2()<<endl;
  for ( int ihit = 1; ihit < NTHits; ihit++ )
  {
    const FTSCAHitV& hit = thits[ihit];
    active &= param.Transport( hit, caParam, active );
    active &= param.Filter( hit, caParam, active );
    cout<<"4.after filter Z "<<param.Z()<<endl;
    cout<<param<<endl;
    //cout<<"1.after filter NDF "<<param.NDF()<<" Chi2 "<<param.Chi2()<<endl;
  }*/

  /* for ( unsigned short ihit = 0; ihit < NTHits; ihit++ ) {
     const FTSCAHitV& hit = thits[ihit];
     active &= param.Transport( hit, caParam, active );
     cout<<"transport \n";
     cout<<param<<endl;
     active &= param.Filter( hit, caParam, active );
     cout<<"filter \n";
     cout<<param<<endl;
   }
   */
  /*cout<<"After Fit "<<endl;
  cout<<param<<endl;
  param.PrintCovMat();
  std::cin.get();*/
  return active;
}

float_m CAFunctionality::Fit(const PndFTSCAParam &caParam, const FTSCAHits &hits, PndFTSCATrackParamVector &param, const vector<TESV> &iHits, const FTSCATarget &target, bool dir,
                             const float_m &mask)
{
  float_m active1 = mask;
  int_v maskvar(1);
  foreach_bit(unsigned short iV, active1)
  {
    if (iV != 0) {
      maskvar[iV] = 0;
    }
  }

  float_m active = static_cast<float_m>(maskvar != 0);
  // int i = 0; //[R.K. 9/2018] unused

  float_v qMom = float_v(10e10f);

#if 1
  qMom = param.QMomentum();
  // cout<<"FitIteration "<<i<<endl;
  active &= FitIteration(caParam, hits, param, iHits, target, dir, false, active);
  /*
  while ( (++i < 10) && ( abs( (param.QMomentum() - qMom)/qMom ) > 0.005f ) ) {
    qMom = param.QMomentum();
    cout<<"FitIteration "<<i<<endl;
    active &= FitIteration( caParam, hits, param, iHits, target, dir, false, active );
  }
  */
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

float_m CAFunctionality::FitUseParam(const PndFTSCAParam &caParam, const FTSCAHits &hits, PndFTSCATrackParamVector &param, const vector<TESV> &iHits, const FTSCATarget &target,
                                     bool dir, bool usePar, const float_m &mask)
{
  float_m active1 = mask;
  int_v maskvar(1);
  foreach_bit(unsigned short iV, active1)
  {
    if (iV != 0) {
      maskvar[iV] = 0;
    }
  }

  float_m active = static_cast<float_m>(maskvar != 0);
  active &= FitIteration(caParam, hits, param, iHits, target, dir, usePar, active);
  return active;
}
