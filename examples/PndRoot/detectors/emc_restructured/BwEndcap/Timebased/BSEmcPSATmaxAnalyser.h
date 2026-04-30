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

//-----------------------------------------------------------
// Description:
//     	Class of TMAX filter for feature extraction
//      Guang Zhao
//-----------------------------------------------------------

#ifndef BSEMCPSATMAXANALYSER_HH
#define BSEMCPSATMAXANALYSER_HH

#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"

#include "BSEmcAbsPSA.h"

class BSEmcWaveform;
class TBuffer;
class TClass;
class TMemberInspector;

class BSEmcPSATmaxAnalyser : public BSEmcAbsPSA {

 public:
  BSEmcPSATmaxAnalyser(const Double_t *t_coeff, Int_t t_taps, Int_t t_gap, Int_t t_samplingrate, Double_t t_hit_threshold, Double_t t_tut_peak, Double_t t_hit_val,
                       Int_t t_verbose);
  virtual ~BSEmcPSATmaxAnalyser();

  virtual Int_t Process(const BSEmcWaveform *t_waveform) /*override*/;

  virtual void GetHit(Int_t t_idx, Double_t &t_energy, Double_t &t_time) /*override*/;

  virtual void Reset() /*override*/;

 private:
  Double_t *fir(Double_t *, Int_t);
  Double_t hit_det(Int_t t_tut_int);

  Int_t fTaps;
  Int_t fGap;
  Double_t fHitThr;
  Double_t fTutPeak;
  Double_t fHitVal;
  const Double_t *fFIRCoeff;
  Double_t fTimeStep;

  std::vector<Double_t> fEnergyList;
  std::vector<Double_t> fTimeList;

  Int_t fVerbose;

  ClassDef(BSEmcPSATmaxAnalyser, 1)
};

#endif /*BSEMCPSATMAXANALYSER_HH*/
