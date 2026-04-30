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

#ifndef PndTrkLegendreFits_H
#define PndTrkLegendreFits_H 1

// Root includes
#include "TROOT.h"

class PndTrkLegendreFits : public TObject {

 private:
  int fIcounter;

  int fNThetaDiv, fNRDiv;

  Double_t fDeltaT, fRMin, fThetaMax, fThetaMin;

 public:
  /** Default constructor **/
  PndTrkLegendreFits();

  /** Destructor **/
  ~PndTrkLegendreFits(){};

  int FindMaximumInMatrix(int nRDiv,     // input, n. division in the R axis;
                          int nThetaD,   // input  n. division in the Theta axis;
                          UShort_t *Mat, // input Matrix of dimensions [fNRDiv][fNThetaDiv] of
                                         // which the Maximum has to be searched;

                          int *iRMax, // output, bin in R of the location of the Maximum;
                          int *iTMax  // output, bin in Theta of the location of the Maximum;
  );

  Short_t FitHelixCylinder(Short_t nHitsinTrack, Double_t *Xconformal, Double_t *Yconformal, Double_t *DriftRadiusconformal, Double_t *ErrorDriftRadiusconformal,
                           Double_t rotationangle, Double_t trajectory_vertex[2], Short_t NMAX, Double_t *m, Double_t *q, Double_t *pAlfa, Double_t *pBeta, Double_t *pGamma,
                           bool *Type, int istampa, int IVOLTE);

  Short_t FitHelixCylinder2(Double_t *Cosine,
                            Short_t LEGIANDRE_NTHETADIV,  // input, the theta divisions;
                            Short_t LEGIANDRE_NRADIUSDIV, // input, the theta divisions;
                            Short_t nHitsinTrack, Double_t *Xconformal, Double_t *Yconformal, Double_t *DriftRadiusconformal, Double_t *ErrorDriftRadiusconformal,
                            Double_t rotationangle, Double_t *Sinus, Double_t THETAMAX, Double_t THETAMIN, Double_t trajectory_vertex[2], Short_t NMAX, Double_t *m, Double_t *q,
                            Double_t *pAlfa, Double_t *pBeta, Double_t *pGamma, bool *Type, int istampa, int IVOLTE);

  Short_t
  FitSZspace(Short_t nHitsinTrack, Double_t *S, Double_t *Z, Double_t *DriftRadius, Double_t *ErrorDriftRadius, Double_t FInot, Short_t NMAX, Double_t *emme, int PlotNumber);

  int LoadMatrix_FindMaximum(Short_t nHitsinTrack,                // input
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
  );

  void LoadMatrix_FindMaximum2(Double_t *Cosine,                    // input, the precalculated values of cosine;
                               Short_t LEGIANDRE_NTHETADIV,         // input, the theta divisions;
                               Short_t LEGIANDRE_NRADIUSDIV,        // input, the theta divisions;
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
  );

  ClassDef(PndTrkLegendreFits, 1);
};

#endif
