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

#ifndef PNDSTT2DIGITIZER_H
#define PNDSTT2DIGITIZER_H

#include <vector>
#include <string>
#include "pndtools.h"

using std::vector;
using std::string;

class TF1;

class PndStt2Digitizer {
public:
  PndStt2Digitizer() : fFcnT2Rmean(0), fFcnT2RsigL(0), fFcnT2RsigH(0), fFcnR2Tmean(0), fFcnR2TsigL(0), fFcnR2TsigH(0) {};
  PndStt2Digitizer(vector<string> vfcn);
  ~PndStt2Digitizer();
  
  TF1 *GetFcnT2Rmean() { return fFcnT2Rmean;}
  TF1 *GetFcnT2RsigL() { return fFcnT2RsigL;}
  TF1 *GetFcnT2RsigH() { return fFcnT2RsigH;}
  TF1 *GetFcnR2Tmean() { return fFcnR2Tmean;}
  TF1 *GetFcnR2TsigL() { return fFcnR2TsigL;}
  TF1 *GetFcnR2TsigH() { return fFcnR2TsigH;}
  
  double GetRndBiGaus(double mu, double sigl, double sigh);
  double DriftTime(double dist, double resfact=1.0);
  vector<double> IsochroneRadius(double tdrift, double resfact=1.0);
  
private:
  TF1 *fFcnT2Rmean;
  TF1 *fFcnT2RsigL;
  TF1 *fFcnT2RsigH;
  TF1 *fFcnR2Tmean;
  TF1 *fFcnR2TsigL;
  TF1 *fFcnR2TsigH;
};

#endif
