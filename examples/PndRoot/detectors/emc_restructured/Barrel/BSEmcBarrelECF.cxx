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

#include "BSEmcBarrelECF.h"

#include <stdlib.h>

#include "TString.h"

BSEmcBarrelECF::BSEmcBarrelECF() : BSEmcThetaEnergyECF()
{
  TString path = getenv("VMCWORKDIR");
  path += "/macro/params/";

  TString fileNamePhoton;
  fileNamePhoton.Form("emc_correction_hist_gamma_%i.root", GetVersion());
  fileNamePhoton = path + fileNamePhoton;

  SetupLookUpMap(fileNamePhoton, "hisEnergyRatioBarrel");
}

BSEmcBarrelECF::~BSEmcBarrelECF() {}
