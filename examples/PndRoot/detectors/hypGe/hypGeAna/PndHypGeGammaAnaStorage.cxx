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

#include "PndHypGeGammaAnaStorage.h"

PndHypGeGammaAnaStorage::PndHypGeGammaAnaStorage() {}

PndHypGeGammaAnaStorage::~PndHypGeGammaAnaStorage() {}

void PndHypGeGammaAnaStorage::SetAllEntries(Int_t Entries)
{
  AllEntries = Entries;
}

Int_t PndHypGeGammaAnaStorage::GetAllEntries()
{
  return AllEntries;
}

void PndHypGeGammaAnaStorage::SetSumPeak(Int_t SumPeak_Ext)
{
  SumPeak = SumPeak_Ext;
}

Int_t PndHypGeGammaAnaStorage::GetSumPeak()
{
  return SumPeak;
}

void PndHypGeGammaAnaStorage::SetInFile(TString InFile_Ext)
{
  InFile = InFile_Ext;
}

TString PndHypGeGammaAnaStorage::GetInFile()
{
  return InFile;
}

ClassImp(PndHypGeGammaAnaStorage)
