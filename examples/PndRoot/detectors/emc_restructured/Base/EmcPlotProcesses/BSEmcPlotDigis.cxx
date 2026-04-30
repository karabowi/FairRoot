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

#include "BSEmcPlotDigis.h"

#include <string>
#include <vector>

#include "TVector3.h"

#include "fairlogger/Logger.h"

#include "PndContainerI.h"

#include "BSEmcCrystalPositionPar.h"
#include "BSEmcDigi.h"
#include "BSEmcHistogrammer.h"

void BSEmcPlotDigis::RequestDataContainer(PndContainerRegister *t_register)
{
  // Lets use the defaults set by the task
  t_register->Request({"", "BSEmcDigi", kFALSE, kFALSE});
}

void BSEmcPlotDigis::GetDataContainer(PndContainerRegister *t_register)
{
  fDigiArray = t_register->GetDefaultBranch<BSEmcDigi>();
}

void BSEmcPlotDigis::InitHistos(const TString &t_folderprefix)
{
  SetFolderPrefix(t_folderprefix);
  TString foldername = t_folderprefix + "/ControlHistos/Digis";
  fHistogrammer->Create1DHist("DigiNumber", foldername, {1000, -0.5, 999.5, "Number of digis per Event"});
  fHistogrammer->Create1DHist("DigiTime", foldername, {1000, 0, 50000, "Time / [ns]"});
  fHistogrammer->Create1DHist("DigiEnergy", foldername, {1600, 0, 16, "Energy / [GeV]"});
  fHistogrammer->Create1DHist("DigiRawEnergy", foldername, {16385, -0.5, 16384.5, "Energy / [ADC Channel]"});
  fHistogrammer->Create1DHist("TotalDigiEnergy", foldername, {1600, 0, 16, "Energy / [GeV]"});
  fHistogrammer->Create1DHist("TotalDigiRawEnergy", foldername, {2000, 0, 1000000, "Total Event Energy / [ADC Channel]"});

  fHistogrammer->Create2DHist("DigiPosition", foldername, {360, 0, 180, "Theta / [Deg]"}, {720, 0, 360, "Phi / [Deg]"});
}

void BSEmcPlotDigis::Process()
{
  if (fDigiArray == nullptr) {
    LOG(info) << "BSEmcPlotDigis::Process() - fDigiArray not set";
  }
  fHistogrammer->Fill("DigiNumber", fDigiArray->GetSize(), 1);
  TString histname = "DigiPosition_Event" + std::to_string(fEvent);

  Double_t totalenergy = 0;
  Double_t totalrawenergy = 0;
  for (const BSEmcDigi *digi : fDigiArray->GetVectorOfPtrToConst()) {
    totalenergy += digi->GetEnergy();
    totalrawenergy += digi->GetRawEnergy();

    fHistogrammer->Fill("DigiTime", digi->GetTimeStamp(), 1);
    fHistogrammer->Fill("DigiEnergy", digi->GetEnergy(), 1);
    fHistogrammer->Fill("DigiRawEnergy", digi->GetRawEnergy(), 1);

    TVector3 pos = fPositionPar->GetPosition(digi->GetDetectorId());
    Double_t theta = GetThetaOfInDeg(pos);
    Double_t phi = GetPhiOfInDeg(pos);
    fHistogrammer->Fill("DigiPosition", theta, phi, 1);
  }
  fHistogrammer->Fill("TotalDigiEnergy", totalenergy, 1);
  fHistogrammer->Fill("TotalDigiRawEnergy", totalrawenergy, 1);
  ++fEvent;
}
