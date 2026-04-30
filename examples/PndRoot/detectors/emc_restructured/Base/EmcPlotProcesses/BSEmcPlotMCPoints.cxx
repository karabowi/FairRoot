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

#include "BSEmcPlotMCPoints.h"

#include <vector>

#include "RtypesCore.h"
#include "TVector3.h"

#include "PndContainerI.h"

#include "BSEmcCrystalPositionPar.h"
#include "BSEmcHistogrammer.h"
#include "BSEmcMCPoint.h"

void BSEmcPlotMCPoints::RequestDataContainer(PndContainerRegister *t_register)
{
  // Lets use the defaults set by the task
  t_register->Request({"", "BSEmcMCPoint", kFALSE, kFALSE});
}

void BSEmcPlotMCPoints::GetDataContainer(PndContainerRegister *t_register)
{
  fMCPointArray = t_register->GetDefaultBranch<BSEmcMCPoint>();
}

void BSEmcPlotMCPoints::InitHistos(const TString &t_folderprefix)
{
  SetFolderPrefix(t_folderprefix);
  TString foldername = t_folderprefix + "/ControlHistos/MCPoints";
  fHistogrammer->Create1DHist("MCPointNumber", foldername, {5000, -0.5, 4999.5, "Number of reco points per Event"});
  fHistogrammer->Create1DHist("MCPointTime", foldername, {500, 0, 50, "Time / [ns]"});
  fHistogrammer->Create1DHist("MCPointEnergy", foldername, {1600, 0, 16, "Energy / [GeV]"});
  fHistogrammer->Create1DHist("TotalMCPointEnergy", foldername, {1600, 0, 16, "Energy / [GeV]"});

  fHistogrammer->Create2DHist("MCPointPosition", foldername, {360, 0, 180, "Theta / [Deg]"}, {720, 0, 360, "Phi / [Deg]"});
}

void BSEmcPlotMCPoints::Process()
{
  fHistogrammer->Fill("MCPointNumber", fMCPointArray->GetSize(), 1);
  Double_t totalenergy = 0;
  for (const BSEmcMCPoint *point : fMCPointArray->GetVectorOfPtrToConst()) {
    totalenergy += point->GetEnergyLoss();

    fHistogrammer->Fill("MCPointTime", point->GetTime() * 1e9, 1);
    fHistogrammer->Fill("MCPointEnergy", point->GetEnergyLoss(), 1);

    TVector3 pos = fPositionPar->GetPosition(point->GetDetectorID());
    Double_t theta = GetThetaOfInDeg(pos);
    Double_t phi = GetPhiOfInDeg(pos);
    fHistogrammer->Fill("MCPointPosition", theta, phi, 1);
  }
  fHistogrammer->Fill("TotalMCPointEnergy", totalenergy, 1);
}
