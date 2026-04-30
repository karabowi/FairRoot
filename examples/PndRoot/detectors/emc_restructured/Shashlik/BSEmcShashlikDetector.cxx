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

#include "BSEmcShashlikDetector.h"

#include <stddef.h>

#include "TVirtualMC.h"

#include "PndSensorNameIdMap.h"

Int_t BSEmcShashlikDetector::GetDetectorId()
{
  TString tmp{gMC->CurrentVolPath()};
  return fIdMap->GetId(tmp);
  // return fGeoH->GetShortID(ProcessSensitiveName(gMC->CurrentVolPath()));
}

TString BSEmcShashlikDetector::ProcessSensitiveName(TString t_sensName) const
{
  TString result{""};
  size_t remove_position = t_sensName.Index("/FscLayerHolderVolume");
  result = t_sensName.Remove(remove_position);
  return result;
}

void BSEmcShashlikDetector::SetSpecialPhysicsCuts()
{
  FairRun *fRun = FairRun::Instance();

  // check for GEANT3, else abort
  if (strcmp(fRun->GetName(), "TGeant3") == 0) {

    // get material ID for customs settings

    std::string sMedium[3] = {"FscScint", "FscFiber", "lead"};
    int matIdVMC;
    for (Int_t i = 0; i < 3; i++) {
      TGeoMedium *medium = gGeoManager->GetMedium(sMedium[i].c_str());
      if (medium == 0)
        continue;
      matIdVMC = medium->GetId();
      double cut_el = 1.0E-4;  // 100 KeV
      double cut_had = 1.0E-4; // 100 KeV
      // double tofmax = 1.E10;    // (s) //[R.K. 01/2017] unused variable?

      // Set new properties, physics cuts etc. for the FSC
      //		gMC->Gstpar(matIdVMC,"PAIR",1); /** pair production*/
      //		gMC->Gstpar(matIdVMC,"COMP",1); /**Compton scattering*/
      //		gMC->Gstpar(matIdVMC,"PHOT",1); /** photo electric effect */
      //		gMC->Gstpar(matIdVMC,"PFIS",0); /**photofission*/
      //		gMC->Gstpar(matIdVMC,"DRAY",1); /**delta-ray*/
      //		gMC->Gstpar(matIdVMC,"ANNI",1); /**annihilation*/
      //		gMC->Gstpar(matIdVMC,"BREM",1); /**bremsstrahlung*/
      //		gMC->Gstpar(matIdVMC,"HADR",1); /**hadronic process*/
      //		gMC->Gstpar(matIdVMC,"MUNU",1); /**muon nuclear interaction*/
      //		gMC->Gstpar(matIdVMC,"DCAY",1); /**decay*/
      //		gMC->Gstpar(matIdVMC,"LOSS",1); /**energy loss*/
      //		gMC->Gstpar(matIdVMC,"MULS",1); /**multiple scattering*/
      //		gMC->Gstpar(matIdVMC,"STRA",1); // collision sampling method to simulate energy loss in thin materials, particularly gases
      //		gMC->Gstpar(matIdVMC,"RAYL",1); // Rayleigh scattering

      gMC->Gstpar(matIdVMC, "CUTGAM", cut_el);  /** gammas (GeV)*/
      gMC->Gstpar(matIdVMC, "CUTELE", cut_el);  /** electrons (GeV)*/
      gMC->Gstpar(matIdVMC, "CUTNEU", cut_had); /** neutral hadrons (GeV)*/
      gMC->Gstpar(matIdVMC, "CUTHAD", cut_had); /** charged hadrons (GeV)*/
      gMC->Gstpar(matIdVMC, "CUTMUO", cut_el);  /** muons (GeV)*/
      gMC->Gstpar(matIdVMC, "BCUTE", cut_el);   /** electron bremsstrahlung (GeV)*/
      gMC->Gstpar(matIdVMC, "BCUTM", cut_el);   /** muon and hadron bremsstrahlung(GeV)*/
      gMC->Gstpar(matIdVMC, "DCUTE", cut_el);   /** delta-rays by electrons (GeV)*/
      gMC->Gstpar(matIdVMC, "DCUTM", cut_el);   /** delta-rays by muons (GeV)*/
      gMC->Gstpar(matIdVMC, "PPCUTM", cut_el);  /** direct pair production by muons (GeV)*/
    }
    gMC->SetMaxNStep((int)1E6);

    std::cout << "\n************************************************************\n"
              << "PndEmc::SetSpecialPhysicsCuts():\n"
              << "   using special physics cuts ...\n";
    std::cout << "************************************************************" << std::endl;
  }
}
// ----