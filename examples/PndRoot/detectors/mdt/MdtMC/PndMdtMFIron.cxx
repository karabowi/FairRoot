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


#include <iostream>

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TParticle.h"
#include "TGeoArb8.h"
#include "TGeoTrd2.h"
#include "TGeoCompositeShape.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TVirtualMC.h"

#include "FairVolume.h"
#include "FairGeoMedia.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoBuilder.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairModule.h"

#include "PndDetectorList.h"
#include "PndStack.h"
#include "PndMdt.h"
#include "PndMdtMFIron.h"

using namespace std;

void PndMdt::PndMdtMFIron()
{
  TString vname = "cave";
  vname = vname.Strip();
  TGeoVolume *vcave = gGeoManager->FindVolumeFast(vname.Data());

  // General definitions
  TGeoVolume *mdtmfi = new TGeoVolumeAssembly("MdtMFIron");

  // Additional definitions
  Text_t buffer[30];
  Text_t longbuffer[250];
  // Double_t mpx, mpy, mpz;
  // Double_t mx0, my0,  //[R.K. 01/2017] unused variable?
  Double_t mz0;
  Double_t my;
  // int k; //[R.K. 01/2017] unused variable?
  Double_t dx1, dx2, dy1, dy2, dz;
  TGeoRotation tRot, tRotSlice;
  // Double_t angle; //[R.K. 01/2017] unused variable?

  FairGeoLoader *geoLoad = FairGeoLoader::Instance();
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  FairGeoMedia *Media = geoFace->getMedia();
  FairGeoBuilder *geobuild = geoLoad->getGeoBuilder();
  FairGeoMedium *medIron = Media->getMedium("iron");
  geobuild->createMedium(medIron);

  // creating the holes
  new TGeoBBox("mfihbox2", ((Double_t)PndMdtMFIron_H02_H) / 10.0, 10.0 + ((Double_t)PndMdtMFIron_Th2) / 10.0, ((Double_t)PndMdtMFIron_H02_V) / 10.0, 0);
  new TGeoBBox("mfihbox3", ((Double_t)PndMdtMFIron_H03_H) / 10.0, 10.0 + ((Double_t)PndMdtMFIron_Th2) / 10.0, ((Double_t)PndMdtMFIron_H03_V) / 10.0, 0);
  new TGeoBBox("mfihbox4", ((Double_t)PndMdtMFIron_H04_H) / 10.0, 10.0 + ((Double_t)PndMdtMFIron_Th2) / 10.0, ((Double_t)PndMdtMFIron_H04_V) / 10.0, 0);
  new TGeoBBox("mfihbox5", ((Double_t)PndMdtMFIron_H05_H) / 10.0, 10.0 + ((Double_t)PndMdtMFIron_Th2) / 10.0, ((Double_t)PndMdtMFIron_H05_V) / 10.0, 0);
  new TGeoBBox("mfihbox6", ((Double_t)PndMdtMFIron_H06_H) / 10.0, 10.0 + ((Double_t)PndMdtMFIron_Th2) / 10.0, ((Double_t)PndMdtMFIron_H06_V) / 10.0, 0);
  new TGeoBBox("mfihboxpmp2", ((Double_t)PndMdtMFIron_PUMP_H) / 10.0, 10.0 + ((Double_t)PndMdtMFIron_Th2) / 10.0, ((Double_t)PndMdtMFIron_PUMP_V) / 10.0, 0);
  new TGeoBBox("mfihboxpmp3", ((Double_t)PndMdtMFIron_PUMP_H) / 10.0, 10.0 + ((Double_t)PndMdtMFIron_Th2) / 10.0, ((Double_t)PndMdtMFIron_PUMP_V) / 10.0, 0);
  new TGeoBBox("mfihboxpmp4", ((Double_t)PndMdtMFIron_PUMP_H) / 10.0, 10.0 + ((Double_t)PndMdtMFIron_Th2) / 10.0, ((Double_t)PndMdtMFIron_PUMP_V) / 10.0, 0);
  new TGeoBBox("mfihboxpmp5", ((Double_t)PndMdtMFIron_PUMP_H) / 10.0, 10.0 + ((Double_t)PndMdtMFIron_Th2) / 10.0, ((Double_t)PndMdtMFIron_PUMP_V) / 10.0, 0);
  new TGeoBBox("mfihboxpmp6", ((Double_t)PndMdtMFIron_PUMP_H) / 10.0, 10.0 + ((Double_t)PndMdtMFIron_Th2) / 10.0, ((Double_t)PndMdtMFIron_PUMP_V) / 10.0, 0);

  // MdtMF
  tRot.RotateX(90.0);
  TGeoRotation mfitgrl;
  mfitgrl.RotateY(0.0);
  TGeoRotation *mfitgr0 = new TGeoRotation(mfitgrl);
  mfitgr0->SetName("mfitgr0");
  mfitgr0->RegisterYourself();
  mfitgrl.RotateY(45.0);
  TGeoRotation *mfitgr1 = new TGeoRotation(mfitgrl);
  mfitgr1->SetName("mfitgr1");
  mfitgr1->RegisterYourself();
  mfitgrl.RotateY(45.0);
  TGeoRotation *mfitgr2 = new TGeoRotation(mfitgrl);
  mfitgr2->SetName("mfitgr2");
  mfitgr2->RegisterYourself();
  mfitgrl.RotateY(45.0);
  TGeoRotation *mfitgr3 = new TGeoRotation(mfitgrl);
  mfitgr3->SetName("mfitgr3");
  mfitgr3->RegisterYourself();
  mfitgrl.RotateY(45.0);
  TGeoRotation *mfitgr4 = new TGeoRotation(mfitgrl);
  mfitgr4->SetName("mfitgr4");
  mfitgr4->RegisterYourself();
  mfitgrl.RotateY(45.0);
  TGeoRotation *mfitgr5 = new TGeoRotation(mfitgrl);
  mfitgr5->SetName("mfitgr5");
  mfitgr5->RegisterYourself();
  mfitgrl.RotateY(45.0);
  TGeoRotation *mfitgr6 = new TGeoRotation(mfitgrl);
  mfitgr6->SetName("mfitgr6");
  mfitgr6->RegisterYourself();
  mfitgrl.RotateY(45.0);
  TGeoRotation *mfitgr7 = new TGeoRotation(mfitgrl);
  mfitgr7->SetName("mfitgr7");
  mfitgr7->RegisterYourself();

  dx2 = 0.0;
  dy1 = (Double_t)PndMdtMFIron_Th2;
  dy2 = (Double_t)PndMdtMFIron_Th2;
  dx1 = ((Double_t)PndMdtMFIron_Height) * (TMath::Tan(TMath::ACos(-1.0) / 8.0));
  my = ((Double_t)PndMdtMFIron_Height) / 2.0;
  dz = ((Double_t)PndMdtMFIron_Height) / 2.0;
  TGeoTranslation *mfitgt = new TGeoTranslation(0.0, 0.0, my / (-10.0));
  mfitgt->SetName("mfitgt");
  mfitgt->RegisterYourself();
  new TGeoTrd2("mfitrd", dx1 / 10.0, dx2 / 10.0, dy1 / 10.0, dy2 / 10.0, dz / 10.0);

  for (int i = 0; i < 5; i++) {
    switch (i) {
    case 0: mz0 = (Double_t)PndMdtMFIron_L200; break;
    case 1: mz0 = (Double_t)PndMdtMFIron_L201; break;
    case 2: mz0 = (Double_t)PndMdtMFIron_L202; break;
    case 3: mz0 = (Double_t)PndMdtMFIron_L203; break;
    case 4: mz0 = (Double_t)PndMdtMFIron_L204; break;
    };
    for (int j = 0; j < 8; j++) {
      sprintf(buffer, "mfitgcs%i", (8 * i) + j);
      sprintf(longbuffer, "mfitrd:mfitgt-mfihbox%i:mfitgr%i-mfihboxpmp%i:mfitgr%i", i + 2, j, i + 2, j);
      TGeoCompositeShape *mfitgcs = new TGeoCompositeShape(buffer, longbuffer);
      sprintf(buffer, "mag%i", 600 + 8 * i + j);
      TGeoVolume *volume = new TGeoVolume(buffer, mfitgcs, gGeoManager->GetMedium("iron"));
      volume->SetLineColor(3);
      mdtmfi->AddNode(volume, 600 + 8 * i + j, new TGeoCombiTrans(0.0, 0.0, mz0 / 10.0, new TGeoRotation(tRot)));
      tRot.RotateZ(-45.0);
    };
  };

  vcave->AddNode(mdtmfi, 1);

  return;
}

ClassImp(PndMdt)
