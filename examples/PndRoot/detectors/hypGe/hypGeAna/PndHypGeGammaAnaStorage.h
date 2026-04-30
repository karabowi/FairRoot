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

/******************************************************

Storage of Data collected during HypGe Gamma Analysis Task created by M.Steinen steinen@kph.uni-mainz.de
Storage of Analysis of Gamma Simulation with hypGe detectors
*******************************************************/

#ifndef PNDHYPGEGAMMAANASTORAGE_H
#define PNDHYPGEGAMMAANASTORAGE_H

#include <TNamed.h>

class PndHypGeGammaAnaStorage : public TNamed {
 public:
  PndHypGeGammaAnaStorage();
  PndHypGeGammaAnaStorage(const TString &name, const TString &title) : TNamed(name, title){};
  virtual ~PndHypGeGammaAnaStorage();

  void SetAllEntries(Int_t Entries);
  Int_t GetAllEntries();

  void SetSumPeak(Int_t SumPeak_Ext);
  Int_t GetSumPeak();

  void SetInFile(TString inFile_Ext);
  TString GetInFile();

 protected:
  Int_t AllEntries;
  Int_t SumPeak;
  TString InFile;

  ClassDef(PndHypGeGammaAnaStorage, 1);
};

#endif /* PNDHYPGEGAMMAANASTORAGE_H */
