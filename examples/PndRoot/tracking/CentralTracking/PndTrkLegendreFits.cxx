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

#include "PndTrkLegendreFits.h"
#include "PndTrkTracking2.h"
#include "PndTrkMergeSort.h"
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
PndTrkLegendreFits::PndTrkLegendreFits()
{
  fNThetaDiv = 360, // this is one of the dimension of the accumulation Matrix;
    fNRDiv = 100;   // this is the other dimension;
  fRMin = 0.;
  fThetaMax = 2. * PI, fThetaMin = 0.;
  fDeltaT = (fThetaMax - fThetaMin) / fNThetaDiv;

  fIcounter = 0;
};

//----------begin of function PndTrkLegendreFits::FindMaximumInMatrix

int PndTrkLegendreFits::FindMaximumInMatrix(int nRDiv,     // input, n. division in the R axis;
                                            int nThetaD,   // input  n. division in the Theta axis;
                                            UShort_t *Mat, // input Matrix of dimensions [fNRDiv][fNThetaDiv] of
                                                           // which the Maximum has to be searched;

                                            int *iRMax, // output, bin in R of the location of the Maximum;
                                            int *iTMax  // output, bin in Theta of the location of the Maximum;
)
{

  int i, j;

  // initializations;
  int max = -99999;

  for (i = 0; i < nRDiv; i++) {
    for (j = 0; j < nThetaD; j++) {
      if (Mat[i * nThetaD + j] > max) {
        max = Mat[i * nThetaD + j];
        *iRMax = i;
        *iTMax = j;
      } // end of if(Max[i*nThetaD+j]>max)
    }
  } // end of for(i=0;i<nRDiv;i++)

  return max;
}
//----------end of function PndTrkLegendreFits::FindMaximumInMatrix

//----------begin of function PndTrkLegendreFits::FitHelixCylinder

Short_t PndTrkLegendreFits::FitHelixCylinder(Short_t nHitsinTrack, Double_t *Xconformal, Double_t *Yconformal, Double_t *DriftRadiusconformal, Double_t *ErrorDriftRadiusconformal,
                                             Double_t /*rotationangle*/,                                                                                    //[R.K. 9/2018] unused
                                             Double_t trajectory_vertex[2], Short_t /*NMAX*/,                                                               //[R.K. 9/2018] unused
                                             Double_t *emme, Double_t *qu, Double_t *pAlfa, Double_t *pBeta, Double_t *pGamma, bool *Type, int /*istampa*/, //[R.K. 9/2018] unused
                                             int IVOLTE)
{

  int result;

  Double_t cosT, sinT,
    R,     // output parameter of the straight line; Xcos(Theta)+Y*sin(Theta)=R;
    Theta; // output parameter of the straight line; Xcos(Theta)+Y*sin(Theta)=R;

  fIcounter = IVOLTE;
  result = LoadMatrix_FindMaximum(nHitsinTrack,              // input
                                  Xconformal,                // X position (in conformal or SZ or whatever);
                                  Yconformal,                // Y position (in conformal or SZ or whatever);
                                  DriftRadiusconformal,      // negative if Mvd hit or similar;
                                  ErrorDriftRadiusconformal, // for the Mvd this is the Radius of the circumference
                                                             // translated with the Conformal transformation,
                                                             // which is, in the XY space : a) centered on the Pixel
                                                             // or Strip; with radius = 0.01 cm --> therefore encompassing
                                                             // completely the Pixel or Stip hit.

                                  &R,    // output parameter of the straight line; Xcos(Theta)+Y*sin(Theta)=R;
                                  &Theta // output parameter of the straight line; Xcos(Theta)+Y*sin(Theta)=R;
  );

  if (result < 0) {
    *Type = false;
    return result;
  }

  cosT = cos(Theta);
  sinT = sin(Theta);

  //------------------------- final summary of the fit results; load output variables;

  *pGamma = 0.;

  // R != 0 --> normal case : the trajectory is a circle in XY passing for the origin;
  if (fabs(R) > 1.e-10) {
    *pAlfa = -cosT / R;
    *pBeta = -sinT / R;
    *Type = true;
  } else {
    // in this case the trajectory is a straight line :     Y*sin(Theta) + X*cos(Theta) = 0
    // in XY;
    *Type = false;
    return 1; // the fit did not fail anyway, so return value>0 ;
  }

  // now take into account the displacement and correct
  *pGamma += (trajectory_vertex[0] * trajectory_vertex[0] + trajectory_vertex[1] * trajectory_vertex[1] - *pAlfa * trajectory_vertex[0] - *pBeta * trajectory_vertex[1]);
  *pAlfa -= 2. * trajectory_vertex[0];
  *pBeta -= 2. * trajectory_vertex[1];

  // calculate  *emme and *qu using the newly calculated *pAlfa, *pBeta, *pGamma of the
  // circular trajectory in XY. Assuming also that *pGamma ~ 0, that is the circumference
  // goes thru the origin.

  if (abs(*pBeta) > 1e-10) {
    // normal case of a straight line in UV that can be put in the    V = m*U +q  form;
    *emme = -(*pAlfa) / (*pBeta);
    *qu = -1. / (*pBeta);
    return 1;
  } else {
    *emme = -(*pAlfa) / 1e-10;
    *qu = -1. / 1e-10;
    return 99;
  }
}
//----------end of function PndTrkLegendreFits::FitHelixCylinder

//----------begin of function PndTrkLegendreFits::FitHelixCylinder2

Short_t PndTrkLegendreFits::FitHelixCylinder2(Double_t *Cosine,
                                              Short_t LEGIANDRE_NTHETADIV,  // input, the theta divisions;
                                              Short_t LEGIANDRE_NRADIUSDIV, // input, the theta divisions;
                                              Short_t nHitsinTrack, Double_t *Xconformal, Double_t *Yconformal, Double_t *DriftRadiusconformal, Double_t *ErrorDriftRadiusconformal,
                                              Double_t /*rotationangle*/,                                                                                    //[R.K. 9/2018] unused
                                              Double_t *Sinus, Double_t THETAMAX, Double_t THETAMIN, Double_t trajectory_vertex[2], Short_t /*NMAX*/,        //[R.K. 9/2018] unused
                                              Double_t *emme, Double_t *qu, Double_t *pAlfa, Double_t *pBeta, Double_t *pGamma, bool *Type, int /*istampa*/, //[R.K. 9/2018] unused
                                              int IVOLTE)
{

  Double_t cosT, sinT,
    R,     // output parameter of the straight line; Xcos(Theta)+Y*sin(Theta)=R;
    Theta; // output parameter of the straight line; Xcos(Theta)+Y*sin(Theta)=R;

  fIcounter = IVOLTE;

  /*
   LoadMatrix_FindMaximum(
      nHitsinTrack,			// input
      Xconformal,			// X position (in conformal or SZ or whatever);
      Yconformal,			// Y position (in conformal or SZ or whatever);
      DriftRadiusconformal,		// negative if Mvd hit or similar;
      ErrorDriftRadiusconformal,	// for the Mvd this is the Radius of the circumference
              // translated with the Conformal transformation,
              // which is, in the XY space : a) centered on the Pixel
              // or Strip; with radius = 0.01 cm --> therefore encompassing
              // completely the Pixel or Stip hit.

      &R,	// output parameter of the straight line; Xcos(Theta)+Y*sin(Theta)=R;
      &Theta  // output parameter of the straight line; Xcos(Theta)+Y*sin(Theta)=R;
        );


  */
  LoadMatrix_FindMaximum2(Cosine,               // input,  the precalculated values of cosine;
                          LEGIANDRE_NTHETADIV,  // input, the theta divisions;
                          LEGIANDRE_NRADIUSDIV, // input, the radius divisions;
                          nHitsinTrack,         // input
                          Sinus,                // input, the precalculated values of sinus;
                          THETAMAX, THETAMIN,
                          Xconformal,                // X position (in conformal or SZ or whatever);
                          Yconformal,                // Y position (in conformal or SZ or whatever);
                          DriftRadiusconformal,      // negative if Mvd hit or similar;
                          ErrorDriftRadiusconformal, // for the Mvd this is the Radius of the circumference
                                                     // translated with the Conformal transformation,
                                                     // which is, in the XY space : a) centered on the Pixel
                                                     // or Strip; with radius = 0.01 cm --> therefore encompassing
                                                     // completely the Pixel or Stip hit.

                          &R,    // output parameter of the straight line; X*cos(Theta)+Y*sin(Theta)=R;
                          &Theta // output parameter of the straight line; X*cos(Theta)+Y*sin(Theta)=R;
  );

  cosT = cos(Theta);
  sinT = sin(Theta);

  //------------------------- final summary of the fit results; load output variables;

  *pGamma = 0.;

  // R != 0 --> normal case : the trajectory is a circle in XY passing for the origin;
  if (fabs(R) > 1.e-10) {
    *pAlfa = -cosT / R;
    *pBeta = -sinT / R;
    *Type = true;
  } else {
    // in this case the trajectory is a straight line :     Y*sin(Theta) + X*cos(Theta) = 0
    // in XY;
    *Type = false;
    return 1; // the fit did not fail anyway, so return value>0 ;
  }

  // now take into account the displacement and correct
  *pGamma += (trajectory_vertex[0] * trajectory_vertex[0] + trajectory_vertex[1] * trajectory_vertex[1] - *pAlfa * trajectory_vertex[0] - *pBeta * trajectory_vertex[1]);
  *pAlfa -= 2. * trajectory_vertex[0];
  *pBeta -= 2. * trajectory_vertex[1];

  // calculate  *emme and *qu using the newly calculated *pAlfa, *pBeta, *pGamma of the
  // circular trajectory in XY. Assuming also that *pGamma ~ 0, that is the circumference
  // goes thru the origin.

  if (abs(*pBeta) > 1e-10) {
    // normal case of a straight line in UV that can be put in the    V = m*U +q  form;
    *emme = -(*pAlfa) / (*pBeta);
    *qu = -1. / (*pBeta);
    return 1;
  } else {
    *emme = -(*pAlfa) / 1e-10;
    *qu = -1. / 1e-10;
    return 99;
  }
}
//----------end of function PndTrkLegendreFits::FitHelixCylinder2

//----------begin of function PndTrkLegendreFits::FitSZspace

Short_t PndTrkLegendreFits::FitSZspace(Short_t nHitsinTrack, Double_t *S,
                                       Double_t *Z,                                                              //
                                       Double_t *DriftRadiusProjected, Double_t * /*ErrorDriftRadiusProjected*/, // //[R.K. 9/2018] unused
                                       Double_t FInot, Short_t /*NMAX*/,                                         //[R.K. 9/2018] unused
                                       Double_t *emme, int PlotNumber)
{

  const int ThetaDiv = 20;

  int i, icount, CellMax, ncell, nMvdHits, nSttHits;
  // result; //[R.K. 01/2017] unused variable?

  UShort_t Matrix[ThetaDiv];

  Double_t Amax, Amin, delta, max, ThetaMax;

  if (nHitsinTrack < 1)
    return -1;

  // this fit in SZ is a fit with only 1 variable :  S = kappa * Z + fi0   with kappa
  // the variable, and  fi0 = FInot  is fixed (obtained by the previous XY fit);
  // therefore here it is calculated an 'accumulation' plot in only 1 dimension.
  // The variable chosen is  Theta --> tan(Theta) = kappa; in this way Theta is a
  // variable bound between 0 and PI;   the equation is then
  // S = tan(Theta) * Z + fi0.
  // For each Stt Skew hit I calculate 2 entries (only 2  are the possible tangents to
  // drift elipsoid, having a fixed fi0) in the accumulation plot and onl;y 1 entry for
  // the Mvd and SciTil hits.

  // reset the Matrix;
  size_t len;
  len = sizeof(Matrix);
  memset(Matrix, 0, len);

  // count the number of Stt hits (2 entries in the accumulation plot each)
  // and the Mvd/SciTil number of hits (1 entry only);
  nMvdHits = 0;
  nSttHits = 0;
  for (i = 0; i < nHitsinTrack; i++) {
    if (DriftRadiusProjected[i] <= 0.)
      nMvdHits++;
    else
      nSttHits++;
  }

  // now the array containing the list of Angles can be declared;

  Double_t AngleArray[nMvdHits + 2 * nSttHits];

  // filling the AngleArray and finding the range of extension of the Angles;

  for (i = 0, icount = 0, Amax = -99999., Amin = 9999999.; i < nHitsinTrack; i++) {
    if (DriftRadiusProjected[i] > 0.) {
      // Stt hit : 2 possible tangent;

      // calculation of the first possible tangent;
      if (abs(Z[i] + DriftRadiusProjected[i]) > 1.e-10) {
        AngleArray[icount] = atan((S[i] - FInot) / (Z[i] + DriftRadiusProjected[i]));
      } else {
        AngleArray[icount] = PI / 2.;
      }
      if (AngleArray[icount] < Amin)
        Amin = AngleArray[icount];
      if (AngleArray[icount] > Amax)
        Amax = AngleArray[icount];
      icount++;
      // calculation of the second possible tangent;
      if (abs(Z[i] - DriftRadiusProjected[i]) > 1.e-10) {
        AngleArray[icount] = atan((S[i] - FInot) / (Z[i] - DriftRadiusProjected[i]));
      } else {
        AngleArray[icount] = PI / 2.;
      }
      if (AngleArray[icount] < Amin)
        Amin = AngleArray[icount];
      if (AngleArray[icount] > Amax)
        Amax = AngleArray[icount];
      icount++;

    } else { // continuation of if(DriftRadiusProjected[i]>0.)

      // Mvd or SciTil hit : only 1 tangent is possible;
      // calculation of the only possible tangent;
      if (abs(Z[i]) > 1.e-10) {
        AngleArray[icount] = atan((S[i] - FInot) / Z[i]);
      } else {
        AngleArray[icount] = PI / 2.;
      }
      if (AngleArray[icount] < Amin)
        Amin = AngleArray[icount];
      if (AngleArray[icount] > Amax)
        Amax = AngleArray[icount];
      icount++;

    } // end of   if(DriftRadiusProjected[i]>0.)

  } // end of    for(i=0, icount=0, Amax ....

  //  fix the case when Amin=Amax (for instance when there is only one hit to fit);
  if (Amin >= Amax)
    Amin = Amax - 0.1 * fabs(Amax);
  // add a 10% slac to the range of the Angles;
  delta = 0.1 * (Amax - Amin);
  Amax += delta;
  Amin -= delta;
  delta = (Amax - Amin) / ThetaDiv;

  char titolo[100];
  sprintf(titolo, "KAPPA%d", PlotNumber);
  TH1F *hKAPPAPlot = new TH1F(titolo, "", ThetaDiv, Amin, Amax);

  // filling the Matrix;

  for (i = 0; i < icount; i++) {

    ncell = (int)((AngleArray[i] - Amin) / delta);
    //	if(ncell <0) { ncell=0; } else if (ncell>=ThetaDiv) {ncell = ThetaDiv-1;}
    Matrix[ncell]++;

    hKAPPAPlot->Fill(AngleArray[i]);

  } // end of for(i=0 ;i<icount;i++)

  hKAPPAPlot->Write();
  delete hKAPPAPlot;

  // find the cell of the maximum in the Matrix;
  max = Matrix[0];
  CellMax = 0;
  for (i = 1; i < ThetaDiv; i++) {
    if (max < Matrix[i]) {
      max = Matrix[i];
      CellMax = i;
    }
  } // end of  for(i=0;i<ThetaDiv;i++)

  //------------------------- final summary of the fit results; load output variables;

  ThetaMax = Amin + (CellMax + 0.5) * delta;

  if (abs(PI / 2. - ThetaMax) < 1.e-5) {
    (*emme) = 9999999.;
  } else {
    (*emme) = tan(ThetaMax);
  }

  return 1;
}

//----------end of function PndTrkLegendreFits::FitSZspace

//----------begin of function PndTrkLegendreFits::LoadMatrix_FindMaximum
int PndTrkLegendreFits::LoadMatrix_FindMaximum(Short_t nHitsinTrack,                // input
                                               Double_t *X,                         // X position (in conformal or SZ or whatever);
                                               Double_t *Y,                         // Y position (in conformal or SZ or whatever);
                                               Double_t *DriftRadius,               // negative if Mvd hit or similar;
                                               Double_t *ErrorDriftRadiusconformal, // for the Mvd this is the Radius of the circumference
                                                                                    // translated with the Conformal transformation,
                                                                                    // which is, in the XY space : a) centered on the Pixel
                                                                                    // or Strip; with radius = 0.01 cm --> therefore encompassing
                                                                                    // completely the Pixel or Stip hit.

                                               Double_t *Rout,    // output parameter of the straight line; Xcos(Theta)+Y*sin(Theta)=R;
                                               Double_t *Thetaout // output parameter of the straight line; Xcos(Theta)+Y*sin(Theta)=R;
)
{
  const int MAXCONTENT = 30000;

  int IndexR, IndexT, i, iRMax, iTMax, j, maxval;

  UShort_t Matrix[fNRDiv][fNThetaDiv];

  Double_t DeltaR, Drift[nHitsinTrack], HistoRmax, R, Theta;

  //  initialization of the Matrix that will be loaded with points;

  size_t len;
  len = sizeof(Matrix);
  memset(Matrix, 0, len);
  // the following HistoRmax corresponds to 1/R**2 in XY plane with R=40 (approximately
  // the radius of a outer axial Stt hit).
  HistoRmax = 0.000625;

  // find the maximum R of the 'histogram';
  for (i = 0; i < nHitsinTrack; i++) {
    R = sqrt(X[i] * X[i] + Y[i] * Y[i]);

    if (DriftRadius[i] < 0.) { // this is a Mvd point; the Pixel/Strip
      // in the Conformal plane is approximately contained in
      // a circle centered in X[i], Y[i] of radius ErrorDriftRadiusconformal[i];
      if (HistoRmax < R) {
        HistoRmax = R + ErrorDriftRadiusconformal[i];
      }
      Drift[i] = 0;
    } else { // this is a Stt axial hit;
      // take into consideration also the drift radius;
      R += DriftRadius[i];
      if (HistoRmax < R) {
        HistoRmax = R;
      }
      Drift[i] = DriftRadius[i];
    }
  } // end of for (i=0; nHitsinTrack; i++)
  DeltaR = (HistoRmax - fRMin) / fNRDiv;

  // fill the Matrix;

  //  char titolo[100];
  //  sprintf(titolo,"Legendre%d",fIcounter);
  //  TH2F * hMatrixPlot = new TH2F(titolo, "",fNRDiv, 0. , HistoRmax
  //  		, fNThetaDiv, fThetaMin, fThetaMax);

  for (i = 0; i < nHitsinTrack; i++) {
    // for now the number of Theta generated are one per Theta bin;
    for (j = 0; j < fNThetaDiv; j++) {
      // generate the Theta in the middle of the bin;
      Theta = fThetaMin + (j + 0.5) * fDeltaT;
      R = X[i] * cos(Theta) + Y[i] * sin(Theta) + Drift[i];
      IndexR = (int)((fabs(R) - fRMin) / DeltaR);
      if (IndexR >= fNRDiv)
        IndexR = fNRDiv - 1;
      // the following is an essential calculation for Theta,
      // because it changes by 180 degrees when
      //  X[i]*cos(Theta) + Y[i]*sin(Theta)+DriftRadius[i]<0;
      if (R < 0.) {
        Theta += PI;
        if (Theta > 2. * PI) {
          Theta -= 2. * PI;
        }
        IndexT = (int)((Theta - fThetaMin) / fDeltaT);
        if (IndexT >= fNThetaDiv)
          IndexT = fNThetaDiv - 1;
      } else {
        IndexT = j;
      }
      if (Matrix[IndexR][IndexT] < MAXCONTENT) {
        Matrix[IndexR][IndexT]++;
      }
      if (Theta == fThetaMax)
        Theta -= 1e-10;
      //		hMatrixPlot->Fill( fabs(R),Theta);
    }
  } // end of for (i=0; nHitsinTrack; i++)

  // hMatrixPlot->Write();
  // delete hMatrixPlot;

  //  find maximum in the Matrix;
  maxval = FindMaximumInMatrix(fNRDiv,        // input
                               fNThetaDiv,    // input
                               &Matrix[0][0], // input

                               &iRMax, // output
                               &iTMax  // output
  );

  //  fit failed
  if (maxval < 0)
    return -5;
  // the found parameters;
  *Rout = (iRMax + 0.5) * DeltaR + fRMin;
  *Thetaout = (iTMax + 0.5) * fDeltaT + fThetaMin;

  return 1;
}
//----------end of function PndTrkLegendreFits::LoadMatrix_FindMaximum

//----------end of function PndTrkLegendreFits::LoadMatrix_FindMaximum2
void PndTrkLegendreFits::LoadMatrix_FindMaximum2(Double_t *Cosine,
                                                 Short_t LEGIANDRE_NTHETADIV,         // input, the theta divisions; Theta goes from 0 to 2*PI;
                                                 Short_t LEGIANDRE_NRADIUSDIV,        // input, the R divisions;
                                                 Short_t nHitsinTrack,                // input
                                                 Double_t *Sinus,                     // input, the precalculated values of sinus;
                                                 Double_t THETAMAX,                   // input, maximum of Theta range (usually 2PI radians);
                                                 Double_t THETAMIN,                   // input, minimum of Theta range (usually 0 radians);
                                                 Double_t *X,                         // X position (in conformal or SZ or whatever);
                                                 Double_t *Y,                         // Y position (in conformal or SZ or whatever);
                                                 Double_t *DriftRadius,               // negative if Mvd hit or similar;
                                                 Double_t *ErrorDriftRadiusconformal, // for the Mvd this is the Radius of the circumference
                                                                                      // translated with the Conformal transformation,
                                                                                      // which is, in the XY space : a) centered on the Pixel
                                                                                      // or Strip; with radius = 0.01 cm --> therefore encompassing
                                                                                      // completely the Pixel or Stip hit.

                                                 Double_t *Rout,    // output parameter of the straight line; Xcos(Theta)+Y*sin(Theta)=R;
                                                 Double_t *Thetaout // output parameter of the straight line; Xcos(Theta)+Y*sin(Theta)=R;
)
{

  // const int MAXCONTENT= 30000; //[R.K. 01/2017] unused variable?

  bool previously_filled[LEGIANDRE_NRADIUSDIV][LEGIANDRE_NTHETADIV];
  memset(previously_filled, false, sizeof(previously_filled));

  Short_t i, IndexR, IndexT, iRMax, iTMax, j, List_filled_cells_IndexR[nHitsinTrack * LEGIANDRE_NTHETADIV], List_filled_cells_IndexT[nHitsinTrack * LEGIANDRE_NTHETADIV],
    // maxval, //[R.K. 01/2017] unused variable?
    n_filled_cells;

  Int_t Matrix[LEGIANDRE_NRADIUSDIV][LEGIANDRE_NTHETADIV];

  Double_t DeltaR, Drift[nHitsinTrack], HistoRmax, R;
  // Theta; //[R.K. 01/2017] unused variable?

  PndTrkMergeSort MergerSorter;

  //  initialization of the Matrix that will be loaded with points;

  size_t len;
  len = sizeof(Matrix);
  memset(Matrix, 0, len);
  // the following HistoRmax corresponds to 1/R**2 in XY plane with R=40 (approximately
  // the radius of a outer axial Stt hit).
  HistoRmax = 0.000625;

  // find the maximum R of the 'histogram';
  for (i = 0; i < nHitsinTrack; i++) {
    R = sqrt(X[i] * X[i] + Y[i] * Y[i]);

    if (DriftRadius[i] < 0.) { // this is a Mvd point; the Pixel/Strip
      // in the Conformal plane is approximately contained in
      // a circle centered in X[i], Y[i] of radius ErrorDriftRadiusconformal[i];
      if (HistoRmax < R) {
        HistoRmax = R + ErrorDriftRadiusconformal[i];
      }
      Drift[i] = 0;
    } else { // this is a Stt axial hit;
      // take into consideration also the drift radius;
      R += DriftRadius[i];
      if (HistoRmax < R) {
        HistoRmax = R;
      }
      Drift[i] = DriftRadius[i];
    }
  } // end of for (i=0; nHitsinTrack; i++)
  DeltaR = (HistoRmax - fRMin) / LEGIANDRE_NRADIUSDIV;

  // fill the Matrix;

  // the Theta (angle with respect to the center of the straw tube center)
  // is the angle generated uniformly between 0 1nd 360;

  // LEGIANDRE_NTHETADIV is a Short_t const MULTIPLE OF 2, defined in PndTrkTracking2.h; it is the divisions of
  // the full 360 degrees angle;

  n_filled_cells = 0;
  for (i = 0; i < nHitsinTrack; i++) {
    // for now the number of Theta generated are one per Theta bin;
    for (j = 0; j < LEGIANDRE_NTHETADIV; j++) {
      // generate the Theta in the middle of the bin; from 0. to 360 degrees;
      R = X[i] * Cosine[j] + Y[i] * Sinus[j] + Drift[i];
      IndexR = (Short_t)((fabs(R) - fRMin) / DeltaR);
      if (IndexR >= LEGIANDRE_NRADIUSDIV)
        IndexR = LEGIANDRE_NRADIUSDIV - 1;
      // the following is an essential calculation for Theta,
      // because it changes by 180 degrees when
      //  X[i]*Cosine[j] + Y[i]*sin(Theta)+DriftRadius[i]<0;
      if (R < 0.) {
        IndexT = j + LEGIANDRE_NTHETADIV / 2;
        if (IndexT >= LEGIANDRE_NTHETADIV)
          IndexT -= LEGIANDRE_NTHETADIV;
        if (IndexT >= LEGIANDRE_NTHETADIV)
          IndexT = LEGIANDRE_NTHETADIV;
      } else {
        IndexT = j;
      }

      if (!previously_filled[IndexR][IndexT]) {
        previously_filled[IndexR][IndexT] = true;
        n_filled_cells++;
        List_filled_cells_IndexR[n_filled_cells - 1] = IndexR;
        List_filled_cells_IndexT[n_filled_cells - 1] = IndexT;
        Matrix[IndexR][IndexT] = 0;
      }

      Matrix[IndexR][IndexT]++;
    }

  } // end of for (i=0; nHitsinTrack; i++)

  Short_t index_array[n_filled_cells];
  Int_t input_array[n_filled_cells];

  for (i = 0; i < n_filled_cells; i++) {
    IndexR = List_filled_cells_IndexR[i];
    IndexT = List_filled_cells_IndexT[i];
    input_array[i] = Matrix[IndexR][IndexT];
    index_array[i] = i;
  } // end of for(i=0;i<n_filled_cells;i++)
  // find maximum in the Matrix by sorting first (from lower to bigger) and then taking the last element;

  MergerSorter.Merge_Sort3(n_filled_cells, input_array, index_array);

  iRMax = List_filled_cells_IndexR[index_array[n_filled_cells - 1]];
  iTMax = List_filled_cells_IndexT[index_array[n_filled_cells - 1]];

  // the found parameters;
  *Rout = (iRMax + 0.5) * DeltaR + fRMin;
  // to be coherent with the formula adopted for the straight line :  X*cos(theta) + Y*sin(theta) = R
  // it is necessary to take the complementary angle (consequently the PI/4 -    );
  *Thetaout = (iTMax + 0.5) * (THETAMAX - THETAMIN) / LEGIANDRE_NTHETADIV + THETAMIN;

  return;
}
//----------end of function PndTrkLegendreFits::LoadMatrix_FindMaximum2

ClassImp(PndTrkLegendreFits)
