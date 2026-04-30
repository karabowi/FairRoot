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

#include "PndTrkChi2Fits.h"
#include "PndTrkVectors.h"
#include <cmath>
#include <iostream>
// Root includes
#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

using namespace std;

#define PI 3.141592654

/** Default constructor **/
PndTrkChi2Fits::PndTrkChi2Fits(){};

//----------begin of function PndTrkChi2Fits::Calculations_Mvd

void PndTrkChi2Fits::Calculations_Mvd(bool *InclusionMvd,          // input;
                                      Double_t *Mvd_DipVar_DipVar, // input;
                                      Double_t *Mvd_IndVar_DipVar, // input;
                                      Double_t *Mvd_IndVar_IndVar, // input;
                                      Short_t nMvdHits,            // input;

                                      Double_t &Mvd_DipVar_DipVar_Sum, // output;
                                      Double_t &Mvd_IndVar_DipVar_Sum, // output;
                                      Double_t &Mvd_IndVar_IndVar_Sum  // output;
)
{
  Short_t i;
  // j; //[R.K. 01/2017] unused variable?

  Mvd_DipVar_DipVar_Sum = 0.;
  Mvd_IndVar_DipVar_Sum = 0.;
  Mvd_IndVar_IndVar_Sum = 0.;

  for (i = 0; i < nMvdHits; i++) {
    // if this Mvd hit has been annihiled, skip;
    if (!InclusionMvd[i])
      continue;

    Mvd_DipVar_DipVar_Sum += Mvd_DipVar_DipVar[i];
    Mvd_IndVar_DipVar_Sum += Mvd_IndVar_DipVar[i];
    Mvd_IndVar_IndVar_Sum += Mvd_IndVar_IndVar[i];

  } // end of for(i=0;i<nMvdHits; i++)
}

//----------begin of function PndTrkChi2Fits::Calculations_Mvd

//----------begin of function PndTrkChi2Fits::Calculations_SkewStt_AllLeftRightCombinations

void PndTrkChi2Fits::Calculations_SkewStt_AllLeftRightCombinations(Short_t nSttHits,              // input;
                                                                   Double_t *Stt_DriftRad_DipVar, // input;
                                                                   Double_t *Stt_DriftRad_IndVar, // input;

                                                                   Double_t *Stt_DriftRad_DipVar_Sum, // output;
                                                                   Double_t *Stt_DriftRad_IndVar_Sum  // output;
)
{

  Short_t current_number, i, j;

  Double_t aux;

  current_number = 1;
  Stt_DriftRad_IndVar_Sum[0] = 0.;
  Stt_DriftRad_DipVar_Sum[0] = 0.;

  // calculate the quantities for the chi**2 minimization for all the other combinations;
  // this part was written previously in an iterative way;
  for (i = 0; i < nSttHits; i++) {
    for (j = 0; j < current_number; j++) { // current_number --> number of combinations formed so far;

      aux = Stt_DriftRad_IndVar_Sum[i];
      Stt_DriftRad_IndVar_Sum[j] += Stt_DriftRad_IndVar[i];
      Stt_DriftRad_IndVar_Sum[current_number + j] = aux - Stt_DriftRad_IndVar[i];

      aux = Stt_DriftRad_DipVar_Sum[i];
      Stt_DriftRad_DipVar_Sum[j] += Stt_DriftRad_DipVar[i];
      Stt_DriftRad_DipVar_Sum[current_number + j] = aux - Stt_DriftRad_DipVar[i];

    } //  end of for(j=0; j<current_number; j++)

    current_number *= 2;

  } // end of  for(i=0; i<nHitsinTrack; i++)
}

//----------end of function PndTrkChi2Fits::Calculations_SkewStt_AllLeftRightCombinations

//----------begin of function PndTrkChi2Fits::FitHelixCylinder

Short_t PndTrkChi2Fits::FitHelixCylinder(Short_t nHitsinTrack, Double_t *Xconformal, Double_t *Yconformal, Double_t *DriftRadiusconformal, Double_t *ErrorDriftRadiusconformal,
                                         Double_t rotationangle, Double_t trajectory_vertex[2], Short_t /*NMAX*/, //[R.K. 9/2018] unused
                                         Double_t *emme, Double_t *qu,
                                         Double_t *pAlfa,             // this is input and output;
                                         Double_t *pBeta,             // this is input and output;
                                         Double_t *pGamma,            // this is input and output;
                                         bool *Type, int /*istampa*/, //[R.K. 9/2018] unused
                                         int /*IVOLTE*/               //[R.K. 9/2018] unused
)
{

  int i;

  Double_t Alfa, alfetta, Beta, cose = cos(rotationangle), dete, mm, sigma2, sine = sin(rotationangle), Su, Suu, Suv, Sv, S1, ui, u1, u2, vi, v1, v2;

  // the parameters of the track trajectory in conformal space is a straight line;
  // here it is parametrized as :  (*pBeta) * V + (*pAlfa)*U +1 =0
  // where U, V are the conformal coordinates :  U = X/( X**2 + Y**2 );
  // In this method *pAlfa, *pBeta are inputs from the fit to the candidate track
  // performed earlier; also, assume *pGamma = 0.

  // perform the required translation of coordinates in new center trajectory_vertex[2]
  // in XY space !!
  // The arrays Xconformal and Yconformal are SUPPOSED TO BE CALCULATED ALREADY TAKING
  // INTO ACCOUNT THE TRANSLATION, therefore no need to modify them here for that. However
  // they still need to be rotated.

  // modify the input values for the translation :

  Alfa = (*pAlfa) + 2. * trajectory_vertex[0];
  Beta = (*pBeta) + 2. * trajectory_vertex[1];

  // now take into account the rotation by rotationangle;

  alfetta = Alfa;
  Alfa = Alfa * cose + Beta * sine;
  Beta = -alfetta * sine + Beta * cose;

  Double_t Xp[nHitsinTrack], Yp[nHitsinTrack];

  // rotate the points;
  for (i = 0; i < nHitsinTrack; i++) {
    Xp[i] = Xconformal[i] * cose + Yconformal[i] * sine;
    Yp[i] = -Xconformal[i] * sine + Yconformal[i] * cose;
  }
  //--------------------

  // starts fitting procedure by finding the POCA first and then doing the Chi**2 minimization;
  // use the starting value of Alfa and Beta for finding the POCA

  mm = sqrt(Alfa * Alfa + Beta * Beta);

  Su = 0.;
  Sv = 0.;
  Suv = 0.;
  Suu = 0.;
  S1 = 0.;
  for (i = 0; i < nHitsinTrack; i++) {

    if (DriftRadiusconformal[i] < 0) { // this is a Mvd hit;
      ui = Xp[i];                      // U
      vi = Yp[i];                      // V
    } else {                           // this is a Stt axial;
                                       // find the POCA of the Stt hit to the original trajectory in conformal space;
      u1 = Xp[i] - DriftRadiusconformal[i] * Alfa / mm;
      v1 = Yp[i] - DriftRadiusconformal[i] * Beta / mm;
      u2 = Xp[i] + DriftRadiusconformal[i] * Alfa / mm;
      v2 = Yp[i] + DriftRadiusconformal[i] * Beta / mm;
      // poca is the point closest to the straight line;
      if (fabs(Beta * v1 + Alfa * u1 + 1) < fabs(Beta * v2 + Alfa * u2 + 1)) {
        ui = u1;
        vi = v1;
      } else {
        ui = u2;
        vi = v2;
      }
    }

    // now the minimization with the chi2 formula;
    sigma2 = ErrorDriftRadiusconformal[i] * ErrorDriftRadiusconformal[i];
    Su += ui / sigma2;
    Sv += vi / sigma2;

    Suv += ui * vi / sigma2;
    Suu += ui * ui / sigma2;

    S1 += 1. / sigma2;

  } // end of for(i=0; i<nHitsinTrack; i++)

  dete = Suu * S1 - Su * Su;
  if (fabs(dete) < 1e-10) {
    *Type = false;
    return -5;
  } // fit fails;

  *emme = (Suv * S1 - Su * Sv) / dete;
  *qu = (Suu * Sv - Su * Suv) / dete;
  // protect the extreme case of q=0 (in principle not possible, it would
  // correspond to a straight line trajectory  y = emme*x  in the XY plane;
  if (fabs(*qu) < 1.e-9) {
    //		if ((*qu) > 0. ) *qu = 1.e-9 ; else *qu = -1.e-9;
    *Type = false;
    return -4; // fit fails;
  } else {
    *Type = true; // normal case;
  }

  // calculate the output coefficients of the circumference in XY plane;

  Alfa = (*emme) / (*qu);
  Beta = -1. / (*qu);

  //  now take into account the rotation and correct back; the affected quantities are ALFA and BETA,
  //  (*emme and *qu also but later);
  alfetta = Alfa;
  Alfa = Alfa * cose - Beta * sine;
  Beta = alfetta * sine + Beta * cose;

  // calculate the coefficients taking into account the translation;
  // now take into account the displacement and calculate Gamma;

  *pGamma = trajectory_vertex[0] * trajectory_vertex[0] + // *pGamma is assumed to be 0 at the beginning;
            trajectory_vertex[1] * trajectory_vertex[1] - Alfa * trajectory_vertex[0] - Beta * trajectory_vertex[1];

  // calculate the coefficients taking into account the translation;
  Alfa -= 2. * trajectory_vertex[0];
  Beta -= 2. * trajectory_vertex[1];

  // calculate  *emme and *qu using the newly calculated *pAlfa, *pBeta, *pGamma of the
  // circular trajectory in XY. Assuming also that *pGamma ~ 0, that is, the circumference
  // goes thru the origin.

  if (abs(Beta) > 1e-9) {
    // normal case of a straight line in UV that can be put in the    V = m*U +q  form;
    *emme = -Alfa / Beta;
    *qu = -1. / Beta;
    *pAlfa = Alfa;
    *pBeta = Beta;
    return 1;
  } else {
    *emme = -Alfa / 1e-9;
    *qu = -1. / 1e-9;
    *pAlfa = Alfa;
    *pBeta = Beta;
    return 99;
  }
}
//----------end of function PndTrkChi2Fits::FitHelixCylinder

//----------begin of function PndTrkChi2Fits::FitSZspace

Short_t PndTrkChi2Fits::FitSZspace(Short_t nHitsinTrack, Double_t *S, Double_t *Z, Double_t *DriftRadius, Double_t *ErrorDriftRadius, Double_t FInot, Short_t NMAX, Double_t *emme,
                                   int /*IVOLTE*/ //[R.K. 9/2018] unused
)
{

  if (nHitsinTrack < 0) {
    cout << "from PndTrkChi2Fits::FitSZspace  error, n hits to fit = 0, exit(-1).";
    exit(-1);
  }

  int Combinations, i, icomb, nSttHits;

  Double_t e2, mvdGSum, mvdUinvSum, mvdZqSum,
    sttS[nHitsinTrack],     // S of only the STT hits;
                            // this is a little overdimensioned, but easy to do;
    sttSigma[nHitsinTrack], // as above; first possibility of Z;
    sttZ1[nHitsinTrack],    // as above; first possibility of Z;
    sttZ2[nHitsinTrack]     // as above; second possibility of Z;
    ;

  nSttHits = 0;
  mvdGSum = 0.;
  mvdUinvSum = 0.;
  mvdZqSum = 0.;

  for (i = 0; i < nHitsinTrack; i++) {
    if (DriftRadius[i] < 0.) {
      // Mvd hit;
      // relevant quantities for the chi2 calculation and minimization;
      e2 = ErrorDriftRadius[i] * ErrorDriftRadius[i];
      mvdGSum += Z[i] * (S[i] - FInot) / e2;
      mvdUinvSum += (S[i] - FInot) * (S[i] - FInot) / e2;
      mvdZqSum += Z[i] * Z[i] / e2;

    } else {
      // Stt hit;
      sttS[nSttHits] = S[i];
      sttSigma[nSttHits] = ErrorDriftRadius[i];
      sttZ1[nSttHits] = Z[i] + DriftRadius[i];
      sttZ2[nSttHits] = Z[i] - DriftRadius[i];
      nSttHits++;
    }
  } // end of  for(i=0; i<nHitsinTrack; i++)

  // limit the number of Skew hits in fit, otherwise the number of combinations
  // becomes too large;
  if (nSttHits > NMAX)
    nSttHits = NMAX;

  //---- begin the fit procedure. Here S is the INDEPENDENT-like variable and  Z the
  //     dependent-like one.
  // For each Stt (Skew) hit the two possibilities are taken into account; therefore the
  // number f chi**2 are  2**nSttHits  ; at the end of the procedure the Stt (Skew) hit pattern
  // is chosen with the least chi**2;
  // use a recursive algorithm to calculate all the chi**2 and minimize them because the
  // a priori is NOT know how may Stt (Skew) hits are there in the track candidate.

  Combinations = (int)(pow(2., nSttHits) + 0.1); // +0.1 only for beeing absolutely sure agains rounding errors;

  Double_t GSum[Combinations], UinvSum[Combinations], ZqSum[Combinations];

  Double_t chi2, Chi2min, Kappa, TotalGSum, TotalUinvSum, TotalZqSum;

  if (nSttHits > 0) {
    //  the following is a recursive function;

    GSumCalculation(sttS,     // input; the independent-like variable;
                    sttZ1,    // input; the dependent-like variable;
                    sttZ2,    // input; the dependent-like variable;
                    sttSigma, // input; the errors on sttZ;
                    FInot, nSttHits,
                    GSum // the output; this must be an array of 2**nSttHits elements;
    );

    //  the following is a recursive function;

    UinvSumCalculation(sttS,     // input; the independent-like variable;
                       sttSigma, // input; the errors on sttZ;
                       FInot, nSttHits,
                       UinvSum // the output; this must be an array of 2**nSttHits elements;
    );

    //  the following is a recursive function;

    ZqSumCalculation(sttZ1,    // input; the dependent-like variable;
                     sttZ2,    // input; the dependent-like variable;
                     sttSigma, // input; the errors on sttZ;
                     FInot, nSttHits,
                     ZqSum // the output; this must be an array of 2**nSttHits elements;
    );

    //  calculation of the Kappa parameter (the parametrization is : Z = (S-FInot)/Kappa );
    //  and the all chi**2;

    // the first combination;

    TotalGSum = mvdGSum + GSum[0];
    TotalUinvSum = mvdUinvSum + UinvSum[0];
    TotalZqSum = mvdZqSum + ZqSum[0];
    // calculation of the fit parameter obtained by minimization of the chi**2;
    if (fabs(TotalGSum) > 1e-9) {
      Kappa = TotalUinvSum / TotalGSum;
    } else {
      Kappa = 1e9;
    }
    // calculation of the corresponding  chi**2;
    Chi2min = TotalZqSum + TotalUinvSum / Kappa - 2. * TotalGSum / Kappa;
    // for the moment this is the solution;
    *emme = Kappa;

    // all the other combinations;
    for (icomb = 1; icomb < Combinations; icomb++) {
      TotalGSum = mvdGSum + GSum[icomb];
      TotalUinvSum = mvdUinvSum + UinvSum[icomb];
      TotalZqSum = mvdZqSum + ZqSum[icomb];
      // calculation of the fit parameter obtained by minimization of the chi**2;
      if (fabs(TotalGSum) > 1.e-8) {
        Kappa = TotalUinvSum / TotalGSum;
        // calculation of the corresponding  chi**2;
        chi2 = TotalZqSum + TotalUinvSum / Kappa - 2. * TotalGSum / Kappa;
        // choose the solution with best chi**2;
        if (chi2 < Chi2min)
          *emme = Kappa;
      } else {
        if (TotalGSum > 0.)
          Kappa = 1.e8;
        else
          Kappa = -1.e8;
      } // end of if(fabs(TotalGSum) > 1.e-8)

    } // end of for(icomb=1; icomb<Combinations; i++)

  } else { // continuation of   if(nSttHits>0)
    // here nSttHits is 0;
    TotalGSum = mvdGSum;
    TotalUinvSum = mvdUinvSum;
    TotalZqSum = mvdZqSum;

    if (fabs(TotalGSum) > 1.e-8) {
      Kappa = TotalUinvSum / TotalGSum;
      // choose the solution with best chi**2;
      *emme = Kappa;
    } else {
      if (TotalGSum > 0.)
        Kappa = 1.e8;
      else
        Kappa = -1.e8;
    } // end of if(fabs(TotalGSum) > 1.e-8)

  } // end of  if(nSttHits>0)

  return 1;
}

//----------end of function PndTrkChi2Fits::FitSZspace

//----------begin of function PndTrkChi2Fits::FitSZspace_Chi2_AnnealingtheMvdOnly

Short_t PndTrkChi2Fits::FitSZspace_Chi2_AnnealingtheMvdOnly(Short_t nHitsinTrack, Double_t *S, Double_t *Z, Double_t *DriftRadius, Double_t *ErrorDriftRadius, Double_t FInot,
                                                            Short_t NMAX, Double_t *emme, int /*IVOLTE*/ //[R.K. 9/2018] unused
)
{
  bool InclusionMvd[nHitsinTrack];

  Short_t i, j,
    // nHitsinFit, //[R.K. 01/2017] unused variable?
    nMvdHits, nSttHits, status;

  Int_t Combinations;

  Double_t A, chi2, chi2_fixed, chi2_best, e2, M,

    Mvd_DipVar_DipVar[nHitsinTrack], Mvd_DipVar_DipVar_Sum, Mvd_IndVar_DipVar[nHitsinTrack], Mvd_IndVar_DipVar_Sum, Mvd_IndVar_IndVar[nHitsinTrack], Mvd_IndVar_IndVar_Sum,
    // Mvd_invError2[nHitsinTrack], //[R.K.02/2017] Unused variable?

    Penalty,

    // Stt_DipVar_Sum, //[R.K. 01/2017] unused variable?
    Stt_DipVar_DipVar_Sum, Stt_DriftRad_DipVar[nHitsinTrack], Stt_DriftRad_DriftRad_Sum, Stt_DriftRad_IndVar[nHitsinTrack],
    // Stt_IndVar_Sum, //[R.K. 01/2017] unused variable?
    Stt_IndVar_DipVar_Sum, Stt_IndVar_IndVar_Sum;

  //	nHitsinTrack = n. hits in this track candidate;
  //   *S  =  array of the independent variable of the fit, namely the R*fi variable on
  // 		the side surface of the Helix cylinder;
  //   *Z  =  array of the dependent variable of the fit; namely the Z position of the hit
  //		projected on the side surface of the Helix cylinder; (Z coordinate for a
  //		Mvd hit, intersection between wire and Helix cylinder for Skew Stt);
  //   *DriftRadius  = array of Radius of drift for the Skew Sraw PROJECTED onto the lateral surface
  //			of the Helix, namely the major axis of the ellipsis projection
  //			of the Skew Straw;  for the Mvd hits it is < 0;
  //   *ErrorDriftRadius = array of Errors associated to DriftRadius; presently (8 Aug 2013) they
  //			are set to 0.5 cm for the Mvd hits, and to 2 times DriftRadius for the
  //			Skew Straws;
  //   FInot = this is a fixed input from the previous fits in XY; it is NOT a output variable
  //			in this fit;
  //   NMAX = maximum number of Skew Straws allowed to partecipate in this fit;
  //  *emme = output of the fit;
  //  IVOLTE = service variable indicating the current event;

  // This method calculates the Chi2 for :
  // 1)  all Mvd hits plus all left/right combinations of the Skew Stt;
  // 2)  like the above but excluding one Mvd hit and replacing it with a penalty term;
  // at the end the minimum Chi2 among all these is chosen and the fit parameter accordingly.

  //   Step 1 : calculation of all the Mvd hits plus all the combinations left/right of the Skew Stt;
  //   here Mvd hits means Mvd (these have DriftRadius = -1) or SciTil (these have DriftRadius = -2);
  nMvdHits = 0;
  nSttHits = 0;
  Stt_DipVar_DipVar_Sum = 0.;
  Stt_DriftRad_DriftRad_Sum = 0.;
  Stt_IndVar_IndVar_Sum = 0.;
  Stt_IndVar_DipVar_Sum = 0.;

  for (i = 0; i < nHitsinTrack; i++) {

    e2 = 1. / (ErrorDriftRadius[i] * ErrorDriftRadius[i]);

    if (DriftRadius[i] < 0.) {
      // Mvd hit;
      // relevant quantities for the chi2 calculation and minimization;
      // Here S is the independent variable (== Mvd_IndVar), Z is the
      // dependent variable (== Mvd_DepVar);
      // the fit function is :  Z = (1/Kappa)*(S-FInot);
      Mvd_DipVar_DipVar[nMvdHits] = Z[i] * Z[i] * e2;
      Mvd_IndVar_IndVar[nMvdHits] = (S[i] - FInot) * (S[i] - FInot) * e2;
      Mvd_IndVar_DipVar[nMvdHits] = (S[i] - FInot) * Z[i] * e2;
      // the following is useful only for adding the penalty term later;
      // Mvd_invError2[nMvdHits] = e2; //[R.K.02/2017] Unused variable?
      nMvdHits++;
    } else {
      // Skew Straw hit;
      // relevant quantities for the chi2 calculation and minimization;
      // Here S is the independent variable (== Stt_IndVar), Z is the
      // dependent variable (== Stt_DepVar);
      // the fit function is :  Z = (1/Kappa)*(S-FInot);

      if (nSttHits < NMAX) {

        Stt_DipVar_DipVar_Sum += Z[i] * Z[i] * e2;
        Stt_DriftRad_DriftRad_Sum += DriftRadius[i] * DriftRadius[i] * e2;
        Stt_DriftRad_IndVar[nSttHits] = DriftRadius[i] * (S[i] - FInot) * e2;
        Stt_DriftRad_DipVar[nSttHits] = DriftRadius[i] * Z[i] * e2;

        Stt_IndVar_IndVar_Sum += (S[i] - FInot) * (S[i] - FInot) * e2;
        Stt_IndVar_DipVar_Sum += (S[i] - FInot) * Z[i] * e2;
      }
      nSttHits++;
    }
  } // end of  for(i=0; i<nHitsinTrack; i++)

  // limit the number of Skew hits in fit, otherwise the number of combinations
  // becomes too large;
  if (nSttHits > NMAX)
    nSttHits = NMAX;

  //---- begin the fit procedure. Here S is the INDEPENDENT-like variable and  Z the
  //     dependent-like one.

  status = 0; // failure in fitting (just initializing!);

  // For the Skew Stt hit calculate the contribution of all possible left/right combinations once and for all
  // since no 'annealing' mechanism is used for them in this algorithm;
  // only the terms containing DriftRadius (linearly) change depending on the combinations;

  //  Combinations == number of all possible left/right combinations given the Skew Stt in the track;
  Combinations = (Int_t)(pow(2., (double)nSttHits) + 0.1); // +0.1 only for being absolutely
                                                           //  sure agains rounding errors;

  Double_t Stt_DriftRad_DipVar_Sum[Combinations], Stt_DriftRad_IndVar_Sum[Combinations];

  Calculations_SkewStt_AllLeftRightCombinations(nSttHits,            // input;
                                                Stt_DriftRad_DipVar, // input;
                                                Stt_DriftRad_IndVar, // input;

                                                Stt_DriftRad_DipVar_Sum, // output;
                                                Stt_DriftRad_IndVar_Sum  // output;
  );

  // For the Mvd hits, calculate the contribution of all Mvd, and all Mvd except 1 (a penalty term instead)

  // all Mvd contribution;
  memset(InclusionMvd, true, sizeof(InclusionMvd));
  Calculations_Mvd(InclusionMvd,      // input;
                   Mvd_DipVar_DipVar, // input;
                   Mvd_IndVar_DipVar, // input;
                   Mvd_IndVar_IndVar, // input;
                   nMvdHits,          // input;

                   Mvd_DipVar_DipVar_Sum, // output;
                   Mvd_IndVar_DipVar_Sum, // output;
                   Mvd_IndVar_IndVar_Sum  // output;
  );

  // calculation with all the Mvd hits considered;

  // formula used here :  emme * (Sum_i (x_i **2 / sigma_i**2)) = Sum_i ( x_i*y_i/sigma_i**2) - qu *(Sum_i x_i/sigma_i**2);
  chi2_best = 9999999.;
  A = Mvd_IndVar_DipVar_Sum + Stt_IndVar_DipVar_Sum;
  chi2_fixed = Mvd_DipVar_DipVar_Sum + Stt_DipVar_DipVar_Sum + Stt_DriftRad_DriftRad_Sum;

  for (i = 0; i < Combinations; i++) {
    M = (A + Stt_DriftRad_IndVar_Sum[i]) / (Stt_IndVar_IndVar_Sum + Mvd_IndVar_IndVar_Sum);
    chi2 = chi2_fixed + M * M * (Mvd_IndVar_IndVar_Sum + Stt_IndVar_IndVar_Sum) + 2. * Stt_DriftRad_DipVar_Sum[i] - 2. * M * (Mvd_IndVar_DipVar_Sum + Stt_IndVar_DipVar_Sum) -
           2. * Stt_DriftRad_IndVar_Sum[i] * M;
    if (chi2 < chi2_best) {
      chi2_best = chi2;
      *emme = M;
      status = 1; // success in fitting;
    }
  } // end of for(i=0;i<nCombinations; i++)

  // calculate the chi2/degrees of freedom; in case there is only 1 hit than just keep the chi2_best as it is;
  if (nSttHits + nMvdHits > 1) {
    chi2_best = chi2_best / (nSttHits + nMvdHits - 1);
  }

  if (nSttHits + nMvdHits > 1) {

    // exclude 1 Mvd hit, add penalty term to chi2; DON'T DO IT in the situation when
    // there is only one hit associated to this track (because when such a hit is a Mvd hit,
    // the program crashes; in fact the track remains without hits and the chi**2 cannot be
    // calculated);

    // //	Penalty = 9.*Mvd_invError2[j];
    //	Penalty = 36.;
    Penalty = 0.;
    for (j = 0; j < nMvdHits; j++) {
      InclusionMvd[j] = false;
      // the following is necessary to include again the Mvd hit excluded in the previous loop;
      if (j > 0)
        InclusionMvd[j - 1] = true;

      // calculate the new Mvd contributions with 1 hit excluded;
      Calculations_Mvd(InclusionMvd,      // input;
                       Mvd_DipVar_DipVar, // input;
                       Mvd_IndVar_DipVar, // input;
                       Mvd_IndVar_IndVar, // input;
                       nMvdHits,          // input;

                       Mvd_DipVar_DipVar_Sum, // output;
                       Mvd_IndVar_DipVar_Sum, // output;
                       Mvd_IndVar_IndVar_Sum  // output;
      );

      // redo the chi2 minimization and best chi2 choice;
      A = Mvd_IndVar_DipVar_Sum + Stt_IndVar_DipVar_Sum;
      chi2_fixed = Penalty + Mvd_DipVar_DipVar_Sum + Stt_DipVar_DipVar_Sum + Stt_DriftRad_DriftRad_Sum;
      for (i = 0; i < Combinations; i++) {
        M = (A + Stt_DriftRad_IndVar_Sum[i]) / (Stt_IndVar_IndVar_Sum + Mvd_IndVar_IndVar_Sum);
        chi2 = chi2_fixed + M * M * (Mvd_IndVar_IndVar_Sum + Stt_IndVar_IndVar_Sum) + 2. * Stt_DriftRad_DipVar_Sum[i] - 2. * M * (Mvd_IndVar_DipVar_Sum + Stt_IndVar_DipVar_Sum) -
               2. * Stt_DriftRad_IndVar_Sum[i] * M;

        // comparison now has to be done between chi2/degrees of freedom; in the case
        // when  nSttHits+nMvdHits = 2, just leave the chi2 as it is;

        if (nSttHits + nMvdHits - 2 > 0)
          chi2 = chi2 / (nSttHits + nMvdHits - 2);

        if (chi2 < chi2_best) {
          chi2_best = chi2;
          *emme = M;
          status = 1; // success in fitting;
        }
      } // end of for(i=0;i<nCombinations; i++)

    } //  end of  for( j =0; j<nMvdHits;j++)

  } // end of if(nSttHits+nMvdHits > 1)

  // at this moment  *emme  corresponds to 1/KAPPA so now it is inverted;
  if (status > 0) {
    if (fabs(*emme) > 1.e-10) {
      *emme = 1. / (*emme);
    } else {
      *emme = 1.e10;
    }
  }

  return status;
}

//----------end of function PndTrkChi2Fits::FitSZspace_Chi2_AnnealingtheMvdOnly

//----------begin of function PndTrkChi2Fits::GSumCalculation

void PndTrkChi2Fits::GSumCalculation(Double_t *S,     // input; the independent-like variable;
                                     Double_t *Z1,    // input, first possibility of Z; the dependent-like variable;
                                     Double_t *Z2,    // input; second possibility of Z; the dependent-like variable;
                                     Double_t *Sigma, // input; the errors on Z;
                                     Double_t FInot,  // input fixed parameter;
                                     int nHits,
                                     Double_t *outSum // the output; this must be an array of 2**nSttHits elements;
)
{

  int i, Combinations, Combinations2;

  double e2;

  // this method calculates the sum calleg   g   in the PDG minimization formula;
  // it is a recursive method;

  Combinations = (int)(pow(2., nHits) + 0.1); // +0.1 to be absolutely sure agains rounding errors;
  Combinations2 = (int)(pow(2., nHits - 1) + 0.1);

  if (nHits == 1) {
    // two possible combinations;
    outSum[0] = Z1[0] * (S[0] - FInot) / (Sigma[0] * Sigma[0]);
    outSum[1] = Z2[0] * (S[0] - FInot) / (Sigma[0] * Sigma[0]);
    return;
  }

  Double_t aux[Combinations];

  // the following is the recursion;

  GSumCalculation(S,     // input; the independent-like variable;
                  Z1,    // input; the dependent-like variable;
                  Z2,    // input; the dependent-like variable;
                  Sigma, // input; the errors on sttZ;
                  FInot, nHits - 1,
                  outSum // the output; this must be an array of 2**nSttHits elements;
  );

  // for each of the   Combinations2 = 2**(nHits-1) already calculated combinations
  // of Stt Skew hits, calculate two combinations by using the last SttSkew hit
  // (its Z1, Z2, S are Z1[nHits-1], Z2[nHits-1] and so on);
  // in this way a total of    Combinations = 2**nHits  combinations exist now.

  for (i = 0; i < Combinations2; i++) {
    e2 = Sigma[nHits - 1] * Sigma[nHits - 1];
    aux[i] = outSum[i] + Z1[nHits - 1] * (S[nHits - 1] - FInot) / e2;
    aux[i + Combinations2] = outSum[i] + Z2[nHits - 1] * (S[nHits - 1] - FInot) / e2;
  } // end of for(i=0; i<Combinations; i++)

  // reload the output array (that has now more elements);
  for (i = 0; i < Combinations; i++) {
    outSum[i] = aux[i];
  }

  return;
}
//----------end of function PndTrkChi2Fits::GSumCalculation

//----------begin of function PndTrkChi2Fits::UinvSumCalculation

void PndTrkChi2Fits::UinvSumCalculation(Double_t *S,     // input; the independent-like variable;
                                        Double_t *Sigma, // input; the errors on Z;
                                        Double_t FInot,  // input fixed parameter;
                                        int nHits,
                                        Double_t *outSum // the output; this must be an array of 2**nSttHits elements;
)
{

  int i, Combinations, Combinations2;

  double e2;

  // this method calculates the sum calleg   g   in the PDG minimization formula;
  // it is a recursive method;

  Combinations = (int)(pow(2., nHits) + 0.1); // +0.1 to be absolutely sure agains rounding errors;
  Combinations2 = (int)(pow(2., nHits - 1) + 0.1);

  if (nHits == 1) {
    // two possible combinations; they have the same Uinv;
    outSum[0] = (S[0] - FInot) * (S[0] - FInot) / (Sigma[0] * Sigma[0]);
    outSum[1] = outSum[0];
    return;
  }

  Double_t aux[Combinations];

  // the following is the recursion;

  UinvSumCalculation(S,     // input; the independent-like variable;
                     Sigma, // input; the errors on sttZ;
                     FInot, nHits - 1,
                     outSum // the output; this must be an array of 2**nSttHits elements;
  );

  // for each of the   Combinations2 = 2**(nHits-1) already calculated combinations
  // of Stt Skew hits, calculate two combinations by using the last SttSkew hit
  // (its Z1, Z2, S are Z1[nHits-1], Z2[nHits-1] and so on);
  // in this way a total of    Combinations = 2**nHits  combinations exist now.

  for (i = 0; i < Combinations2; i++) {
    e2 = Sigma[nHits - 1] * Sigma[nHits - 1];
    // two combinations but with the same U inv;
    aux[i] = outSum[i] + (S[nHits - 1] - FInot) * (S[nHits - 1] - FInot) / e2;
    aux[i + Combinations2] = aux[i];
  } // end of for(i=0; i<Combinations; i++)

  // reload the output array (that has now more elements);
  for (i = 0; i < Combinations; i++) {
    outSum[i] = aux[i];
  }

  return;
}
//----------end of function PndTrkChi2Fits::UinvSumCalculation

//----------begin of function PndTrkChi2Fits::ZqSumCalculation

void PndTrkChi2Fits::ZqSumCalculation(Double_t *Z1,    // input, first possibility of Z; the dependent-like variable;
                                      Double_t *Z2,    // input; second possibility of Z; the dependent-like variable;
                                      Double_t *Sigma, // input; the errors on Z;
                                      Double_t FInot,  // input fixed parameter;
                                      int nHits,
                                      Double_t *outSum // the output; this must be an array of 2**nSttHits elements;
)
{

  int i, Combinations, Combinations2;

  double e2;

  // this method calculates the sum calleg   g   in the PDG minimization formula;
  // it is a recursive method;

  Combinations = pow(2., nHits) + 0.1; // +0.1 to be absolutely sure against rounding errors;
  Combinations2 = pow(2., nHits - 1) + 0.1;

  if (nHits == 1) {
    // two possible combinations;
    outSum[0] = Z1[0] * Z1[0] / (Sigma[0] * Sigma[0]);
    outSum[1] = Z2[0] * Z2[0] / (Sigma[0] * Sigma[0]);
    return;
  }

  Double_t aux[Combinations];

  // the following is the recursion;

  ZqSumCalculation(Z1,    // input; the dependent-like variable;
                   Z2,    // input; the dependent-like variable;
                   Sigma, // input; the errors on sttZ;
                   FInot, nHits - 1,
                   outSum // the output; this must be an array of 2**nSttHits elements;
  );

  // for each of the   Combinations2 = 2**(nHits-1) already calculated combinations
  // of Stt Skew hits, calculate two combinations by using the last SttSkew hit
  // (its Z1, Z2, S are Z1[nHits-1], Z2[nHits-1] and so on);
  // in this way a total of    Combinations = 2**nHits  combinations exist now.

  for (i = 0; i < Combinations2; i++) {
    e2 = Sigma[nHits - 1] * Sigma[nHits - 1];
    aux[i] = outSum[i] + Z1[nHits - 1] * Z1[nHits - 1] / e2;
    aux[i + Combinations2] = outSum[i] + Z2[nHits - 1] * Z2[nHits - 1] / e2;
  } // end of for(i=0; i<Combinations; i++)

  // reload the output array (that has now more elements);
  for (i = 0; i < Combinations; i++) {
    outSum[i] = aux[i];
  }

  return;
}
//----------end of function PndTrkChi2Fits::ZqSumCalculation

ClassImp(PndTrkChi2Fits)
