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

//---------------------------------------------------------
// Description:
// 	File Calibrator for Emc Crystals.
//
// 	Christian Hammann
// 	31.7.2012

//---------------------------------------------------------

#include "PndEmcFileCrystalCalibrator.h"
#include <fstream>
#include <iostream>
#include "TObjArray.h"

using std::endl;
using std::ifstream;
using std::map;

PndEmcFileCrystalCalibrator::PndEmcFileCrystalCalibrator() : PndEmcAbsCrystalCalibrator(), fCalFileName(""){};

PndEmcFileCrystalCalibrator::PndEmcFileCrystalCalibrator(const char *CalFileName) : PndEmcAbsCrystalCalibrator(), fCalFileName(CalFileName){};

void PndEmcFileCrystalCalibrator::SetCalibrationFile(const char *CalFileName)
{
  fCalFileName = CalFileName;
};

Bool_t PndEmcFileCrystalCalibrator::Init()
{
  ifstream in(fCalFileName, ifstream::in);
  //    in.open(fCalibrationFileName);
  if (!in.good()) {
    std::cerr << "Cannot open calibration file!" << endl;
    return kFALSE;
  }

  char buf[255];
  while (in.getline(buf, 255)) {
    TString tmp = buf;
    TObjArray *tokens = tmp.Tokenize(" \t;");
    if (tokens->GetEntries() < 2) {
      continue;
    }
    tmp = tokens->UncheckedAt(0)->GetName();
    TString tmp2 = tokens->UncheckedAt(1)->GetName();
    if (tmp.IsDigit() && tmp2.IsFloat()) {
      fCalibrationMap.insert(std::pair<Long_t, Double_t>(tmp.Atoi(), tmp2.Atof()));
    }
    if (tokens->GetEntries() < 3) {
      continue;
    }
    tmp2 = tokens->UncheckedAt(2)->GetName();
    if (tmp.IsDigit() && tmp2.IsFloat()) {
      fGainMap.insert(std::pair<Long_t, Double_t>(tmp.Atoi(), tmp2.Atof()));
    }
    if (tokens->GetEntries() < 4) {
      continue;
    }
    tmp2 = tokens->UncheckedAt(3)->GetName();
    if (tmp.IsDigit() && tmp2.IsFloat()) {
      fOverflowMap.insert(std::pair<Long_t, Double_t>(tmp.Atoi(), tmp2.Atof()));
    }
    delete tokens;
  }
  if (fVerbose > 1) {
    std::cout << "calibration values:" << std::endl;
    map<Long_t, Double_t>::iterator it;
    map<Long_t, Double_t>::iterator it2;
    Long_t detid;
    for (it = fCalibrationMap.begin(); it != fCalibrationMap.end(); it++) {
      detid = (*it).first;
      std::cout << detid << "\t" << (*it).second << "\t";
      it2 = fGainMap.find(detid);
      if (it2 != fGainMap.end()) {
        std::cout << (*it2).second << "\t";
      }
      it2 = fOverflowMap.find(detid);
      if (it2 != fOverflowMap.end()) {
        std::cout << (*it2).second;
      }
      std::cout << endl;
    }
  }
  return kTRUE;
};

PndEmcAbsCrystalCalibrator::CalibrationStatus_t PndEmcFileCrystalCalibrator::Calibrate(Double_t &Energy, Long_t detId, Int_t SignalNr)
{
  map<Long_t, Double_t>::iterator it;
  if (SignalNr == 0) {
    it = fOverflowMap.find(detId);
    if (it != fOverflowMap.end()) {
      if (Energy > it->second) {
        return kCALOVERFLOW;
      }
    }
  }

  it = fCalibrationMap.find(detId);
  if (it != fCalibrationMap.end()) {
    Energy *= it->second;
  } else {
    return kCALMISSING;
  }

  if (SignalNr == 1) {
    it = fGainMap.find(detId);
    if (it != fGainMap.end()) {
      Energy *= it->second;
    }
  }

  return kCALOK;
};

ClassImp(PndEmcFileCrystalCalibrator);
