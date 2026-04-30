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

#include "PndRichResolution.h"
#include "PndPidCandidate.h"

//___________________________________________________________
PndRichResolution::~PndRichResolution() {}

// -----   Default constructor   -------------------------------------------
PndRichResolution::PndRichResolution()
{
  caldb = new PndRichCalDb("rich_caldb_v313", "R");
  caldb->Init();
  caldb->Print();
}

//______________________________________________________
Double_t PndRichResolution::Sigma(dbpoint pnt)
{
  Double_t sigmu = caldb->GetBetaSig(pnt);
  // temporary: multi scattering correction
  Double_t k = 0.41e-3; // 0.462e-3;
  Double_t beta2 = pnt.beta * pnt.beta;
  k *= sqrt(1 - beta2) / beta2;
  Double_t sigMultScattM = k / pnt.mass;
  Double_t sigMultScattMu = k / 0.1056583715;
  Double_t sigmar = sigmu * sigmu - sigMultScattMu * sigMultScattMu + sigMultScattM * sigMultScattM;
  sigmu = sigmar > 0 ? sqrt(sigmar) : sigmu;
  return sigmu;
}

//______________________________________________________
Double_t PndRichResolution::Shift(dbpoint pnt)
{
  return caldb->GetBetaMean(pnt);
}

//______________________________________________________
Double_t PndRichResolution::Efficiency(dbpoint pnt)
{
  return caldb->GetBetaEff(pnt);
}

// -------------------------------------------------------------------------
