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

#ifndef PndTrkChi2Fits_H
#define PndTrkChi2Fits_H 1

#include "PndTrkVectors.h"
// Root includes
#include "TROOT.h"

class PndTrkChi2Fits : public TObject {

 private:
  int fIcounter;

 public:
  /** Default constructor **/
  PndTrkChi2Fits();

  /** Destructor **/
  ~PndTrkChi2Fits(){};

  void Calculations_Mvd(bool *InclusionMvd,          // input;
                        Double_t *Mvd_DipVar_DipVar, // input;
                        Double_t *Mvd_IndVar_DipVar, // input;
                        Double_t *Mvd_IndVar_IndVar, // input;
                        Short_t nMvdHits,            // input;

                        Double_t &Mvd_DipVar_DipVar_Sum, // output;
                        Double_t &Mvd_IndVar_DipVar_Sum, // output;
                        Double_t &Mvd_IndVar_IndVar_Sum  // output;
  );

  void Calculations_SkewStt_AllLeftRightCombinations(Short_t nSttHits,              // input;
                                                     Double_t *Stt_DriftRad_DipVar, // input;
                                                     Double_t *Stt_DriftRad_IndVar, // input;

                                                     Double_t *Stt_DriftRad_DipVar_Sum, // output;
                                                     Double_t *Stt_DriftRad_IndVar_Sum  // output;
  );

  Short_t FitHelixCylinder(Short_t nHitsinTrack, Double_t *Xconformal, Double_t *Yconformal, Double_t *DriftRadiusconformal, Double_t *ErrorDriftRadiusconformal,
                           Double_t rotationangle, Double_t trajectory_vertex[2], Short_t NMAX, Double_t *m, Double_t *q, Double_t *pAlfa, Double_t *pBeta, Double_t *pGamma,
                           bool *Type, int istampa, int IVOLTE);

  Short_t FitSZspace(Short_t nHitsinTrack, Double_t *S, Double_t *Z, Double_t *DriftRadius, Double_t *ErrorDriftRadius, Double_t FInot, Short_t NMAX, Double_t *emme, int IVOLTE);

  Short_t FitSZspace_Chi2_AnnealingtheMvdOnly(Short_t nHitsinTrack, Double_t *S, Double_t *Z, Double_t *DriftRadius, Double_t *ErrorDriftRadius, Double_t FInot, Short_t NMAX,
                                              Double_t *emme, int IVOLTE);

  void GSumCalculation(

    Double_t *S,     // input; the independent-like variable;
    Double_t *Z1,    // input, first possibility of Z; the dependent-like variable;
    Double_t *Z2,    // input; second possibility of Z; the dependent-like variable;
    Double_t *Sigma, // input; the errors on Z;
    Double_t FInot,  // input fixed parameter;
    int nHits,
    Double_t *outSum // the output; this must be an array of 2**nSttHits elements;

  );

  void UinvSumCalculation(

    Double_t *S,     // input; the independent-like variable;
    Double_t *Sigma, // input; the errors on Z;
    Double_t FInot,  // input fixed parameter;
    int nHits,
    Double_t *outSum // the output; this must be an array of 2**nSttHits elements;

  );

  void ZqSumCalculation(

    Double_t *Z1,    // input, first possibility of Z; the dependent-like variable;
    Double_t *Z2,    // input; second possibility of Z; the dependent-like variable;
    Double_t *Sigma, // input; the errors on Z;
    Double_t FInot,  // input fixed parameter;
    int nHits,
    Double_t *outSum // the output; this must be an array of 2**nSttHits elements;

  );

  ClassDef(PndTrkChi2Fits, 1);
};

#endif
