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

/*
 *      Author: stockman
 * PndDrcDigiSorterTask.cxx
 *
 */
// -------------------------------------------------------------------------
// -----                   PndDrcDigiSorterTask source file            -----
// -----         HARPHOOL KUMAWAT h.kumawat@gsi.de                     -----
// -----                                                               -----
// -------------------------------------------------------------------------

#include "PndDrcDigiSorterTask.h"

#include "PndDrcDigi.h"
#include "PndDrcDigiRingSorter.h"

#include "FairRootManager.h"

ClassImp(PndDrcDigiSorterTask);

PndDrcDigiSorterTask::PndDrcDigiSorterTask()
{
  // TODO Auto-generated constructor stub
}

PndDrcDigiSorterTask::~PndDrcDigiSorterTask()
{
  // TODO Auto-generated destructor stub
}

void PndDrcDigiSorterTask::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fOutputBranch);
  if (fVerbose > 1) {
    std::cout << "-I- PndDrcDigiSorterTask::AddNewDataToTClonesArray Data: ";
    std::cout << *(PndDrcDigi *)(data) << std::endl;
  }
  new ((*myArray)[myArray->GetEntries()]) PndDrcDigi(*(PndDrcDigi *)(data));
}

FairRingSorter *PndDrcDigiSorterTask::InitSorter(Int_t numberOfCells, Double_t widthOfCells) const
{
  return new PndDrcDigiRingSorter(numberOfCells, widthOfCells);
}
