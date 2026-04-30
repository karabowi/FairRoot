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

#include "PndMvdDetector.h"

#include "TClonesArray.h"

// -----   Default constructor   -------------------------------------------
PndMvdDetector::PndMvdDetector() : PndSdsDetector()
{
  SetDetectorID(PndDetectorId::kMVD);
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndMvdDetector::PndMvdDetector(const char *name, Bool_t active) : PndSdsDetector(name, active)
{
  SetDetectorID(PndDetectorId::kMVD);
}
// -------------------------------------------------------------------------

PndMvdDetector::PndMvdDetector(const PndMvdDetector& rhs)
    : PndSdsDetector(rhs)
{
    SetDefaultSensorNames();
}

void PndMvdDetector::SetDefaultSensorNames()
{
  fListOfSensitives.push_back("Disk-Sensor");   // Root_Test.root
  fListOfSensitives.push_back("Barrel-Sensor"); // Root_Test.root
  fListOfSensitives.push_back("PixelSensorActive");
  fListOfSensitives.push_back("StripSensorActive");
  fListOfSensitives.push_back("StripSensor");      // MVD14.root
  fListOfSensitives.push_back("SensorActiveArea"); // MVD14.root
  fListOfSensitives.push_back("StripActive");      // MVD_v1.0.root
  fListOfSensitives.push_back("PixelActive");      // MVD_v1.0.root
  fListOfSensitives.push_back("TrapAddDisks");
  fListOfSensitives.push_back("TestHCal");                   // tracking station
  fListOfSensitives.push_back("StripSensorActiveSmallTrap"); // LambdaDisks
  fListOfSensitives.push_back("StripSensorActiveLargeTrap"); // LambdaDisks
  fListOfSensitives.push_back("EpiSensorVolume");            // MVD TB

  if (fVerboseLevel > 0) {
    std::cout << "- I - PndMvdDetector: fListOfSensitives contains:";
    for (UInt_t k = 0; k < fListOfSensitives.size(); k++)
      std::cout << "\n\t" << fListOfSensitives[k];
    std::cout << std::endl;
  }
}

// -----   Destructor   ----------------------------------------------------
PndMvdDetector::~PndMvdDetector() {}

// -------------------------------------------------------------------------
void PndMvdDetector::SetBranchNames(TString outBranchname, TString folderName)
{
  fOutBranchName = outBranchname;
  fFolderName = folderName;
}

// -------------------------------------------------------------------------
void PndMvdDetector::SetBranchNames()
{
  fOutBranchName = "MVDPoint";
  fFolderName = "PndMvd";
}

// -------------------------------------------------------------------------
// overwrite virtual method of FairDetector
void PndMvdDetector::SetSpecialPhysicsCuts()
{
  // Switched off, default of the MC engine is used
  return;

  // for using G4 user limits call gMC->SetUserParameters(true); and set the properties in the Mediafile

  // TODO: Study the effects of cuts.
  //  FairRun* fRun = FairRun::Instance();
  //
  //  //check for GEANT3, else abort
  //  if (strcmp(fRun->GetName(),"TGeant3") == 0) {
  //
  //    //get material ID for customs settings
  //    int matIdVMC = gGeoManager->GetMedium("silicon")->GetId();
  //
  //    //double cut_el = 1.0E-5;   // (GeV)
  //    //double cut_had = 1.0E-3;  // (GeV)
  //    double tofmax = 1.E10;    // (s)
  //
  //    // Set new properties, physics cuts etc. for the TPCmixture
  //    gMC->Gstpar(matIdVMC,"PAIR",1); /** pair production*/
  //    gMC->Gstpar(matIdVMC,"COMP",1); /**Compton scattering*/
  //    gMC->Gstpar(matIdVMC,"PHOT",1); /** photo electric effect */
  //    gMC->Gstpar(matIdVMC,"PFIS",0); /**photofission*/
  //    gMC->Gstpar(matIdVMC,"DRAY",1); /**delta-ray*/
  //    gMC->Gstpar(matIdVMC,"ANNI",1); /**annihilation*/
  //    gMC->Gstpar(matIdVMC,"BREM",1); /**bremsstrahlung*/
  //    gMC->Gstpar(matIdVMC,"HADR",1); /**hadronic process*/
  //    gMC->Gstpar(matIdVMC,"MUNU",1); /**muon nuclear interaction*/
  //    gMC->Gstpar(matIdVMC,"DCAY",1); /**decay*/
  //    gMC->Gstpar(matIdVMC,"LOSS",1); /**energy loss*/
  //    gMC->Gstpar(matIdVMC,"MULS",1); /**multiple scattering*/
  //    gMC->Gstpar(matIdVMC,"STRA",0);
  //    gMC->Gstpar(matIdVMC,"RAYL",1);
  //
  //    gMC->Gstpar(matIdVMC,"CUTGAM",fCut_el); /** gammas (GeV)*/
  //    gMC->Gstpar(matIdVMC,"CUTELE",fCut_el); /** electrons (GeV)*/
  //    gMC->Gstpar(matIdVMC,"CUTNEU",fCut_had); /** neutral hadrons (GeV)*/
  //    gMC->Gstpar(matIdVMC,"CUTHAD",fCut_had); /** charged hadrons (GeV)*/
  //    gMC->Gstpar(matIdVMC,"CUTMUO",fCut_el); /** muons (GeV)*/
  //    gMC->Gstpar(matIdVMC,"BCUTE",fCut_el);  /** electron bremsstrahlung (GeV)*/
  //    gMC->Gstpar(matIdVMC,"BCUTM",fCut_el);  /** muon and hadron bremsstrahlung(GeV)*/
  //    gMC->Gstpar(matIdVMC,"DCUTE",fCut_el);  /** delta-rays by electrons (GeV)*/
  //    gMC->Gstpar(matIdVMC,"DCUTM",fCut_el);  /** delta-rays by muons (GeV)*/
  //    gMC->Gstpar(matIdVMC,"PPCUTM",fCut_el); /** direct pair production by muons (GeV)*/
  //
  //    gMC->SetMaxNStep(1E6);
  //
  //    Info("SetSpecialPhysicsCuts()","Using special physics cuts in MVD Sensors.");
  //  }
}


FairModule* PndMvdDetector::CloneModule() const
{
    return new PndMvdDetector(*this);
}

ClassImp(PndMvdDetector);
