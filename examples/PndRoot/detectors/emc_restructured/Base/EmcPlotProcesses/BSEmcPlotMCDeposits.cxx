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

#include "BSEmcPlotMCDeposits.h"

#include <vector>

#include "TVector3.h"

#include "PndContainerI.h"

#include "BSEmcCrystalPositionPar.h"
#include "BSEmcHistogrammer.h"
#include "BSEmcMCDeposit.h"

void BSEmcPlotMCDeposits::RequestDataContainer(PndContainerRegister *t_register)
{
  // Lets use the defaults set by the task
  t_register->Request({"", "BSEmcMCDeposit", kFALSE, kFALSE});
}

void BSEmcPlotMCDeposits::GetDataContainer(PndContainerRegister *t_register)
{
  fMCDepositArray = t_register->GetDefaultBranch<BSEmcMCDeposit>();
}

void BSEmcPlotMCDeposits::InitHistos(const TString &t_folderprefix)
{
  SetFolderPrefix(t_folderprefix);
  TString foldername = t_folderprefix + "/ControlHistos/MCDeposits";
  fHistogrammer->Create1DHist("MCDepositNumber", foldername, {1000, -0.5, 999.5, "Number of deposits per Event"});
  fHistogrammer->Create1DHist("MCDepositTime", foldername, {500, 0, 50, "Time / [ns]"});
  fHistogrammer->Create1DHist("MCDepositEnergy", foldername, {1600, 0, 16, "Energy / [GeV]"});
  fHistogrammer->Create1DHist("TotalMCDepositEnergy", foldername, {1600, 0, 16, "Energy / [GeV]"});

  fHistogrammer->Create2DHist("MCDepositPosition", foldername, {360, 0, 180, "Theta / [Deg]"}, {720, 0, 360, "Phi / [Deg]"});
}

void BSEmcPlotMCDeposits::Process()
{
  fHistogrammer->Fill("MCDepositNumber", fMCDepositArray->GetSize(), 1);

  Double_t totalenergy = 0;
  for (const BSEmcMCDeposit *deposit : fMCDepositArray->GetVectorOfPtrToConst()) {
    totalenergy += deposit->GetEnergy();

    fHistogrammer->Fill("MCDepositTime", deposit->GetTimeStamp(), 1);
    fHistogrammer->Fill("MCDepositEnergy", deposit->GetEnergy(), 1);

    TVector3 pos = fPositionPar->GetPosition(deposit->GetDetectorID());
    Double_t theta = GetThetaOfInDeg(pos);
    Double_t phi = GetPhiOfInDeg(pos);
    fHistogrammer->Fill("MCDepositPosition", theta, phi, 1);
  }
  fHistogrammer->Fill("TotalMCDepositEnergy", totalenergy, 1);
  ++fEvent;
}
