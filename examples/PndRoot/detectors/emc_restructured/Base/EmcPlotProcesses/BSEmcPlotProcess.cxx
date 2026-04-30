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


#include "BSEmcPlotProcess.h"

#include <vector>

#include "TMath.h"

#include "FairParSet.h"

#include "PndParameterRegister.h"

#include "BSEmcCrystalPositionPar.h"
#include "BSEmcGeoNeighbouringRelationPar.h"

BSEmcPlotProcess::BSEmcPlotProcess()
  : PndProcess{"BSEmcPlotProcess"}, fPositionParName(BSEmcCrystalPositionPar::fgParameterName), fNeighbouringRelationParName(BSEmcGeoNeighbouringRelationPar::fgParameterName)
{
}

BSEmcPlotProcess::BSEmcPlotProcess(const std::string &t_processname)
  : PndProcess{t_processname}, fPositionParName(BSEmcCrystalPositionPar::fgParameterName), fNeighbouringRelationParName(BSEmcGeoNeighbouringRelationPar::fgParameterName)
{
}

BSEmcPlotProcess::~BSEmcPlotProcess() {}

void BSEmcPlotProcess::SetDetectorName(const std::string &t_detectorName)
{
  fDetectorName = t_detectorName;
  fPositionParName += fDetectorName;
  fNeighbouringRelationParName += fDetectorName;

  fParameterList.push_back(fPositionParName);
  fParameterList.push_back(fNeighbouringRelationParName);
}

void BSEmcPlotProcess::SetupParameters(const PndParameterRegister *t_parameterRegister)
{
  fNeighbouringRelationPar = dynamic_cast<BSEmcGeoNeighbouringRelationPar *>(t_parameterRegister->GetParameter(fNeighbouringRelationParName));

  fPositionPar = dynamic_cast<BSEmcCrystalPositionPar *>(t_parameterRegister->GetParameter(fPositionParName));
}

Double_t BSEmcPlotProcess::GetPhiOfInDeg(const TVector3 &t_pos) const
{
  Double_t phi = t_pos.Phi();
  if (phi < 0) {
    phi += 2 * TMath::Pi();
  }
  phi *= TMath::RadToDeg();
  return phi;
}

Double_t BSEmcPlotProcess::GetThetaOfInDeg(const TVector3 &t_pos) const
{
  return t_pos.Theta() * TMath::RadToDeg();
}
