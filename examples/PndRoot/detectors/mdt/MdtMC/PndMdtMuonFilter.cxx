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
#include "PndMdtMuonFilter.h"

using namespace std;

void PndMdt::PndMdtMuonFilter()
{
  TString vname = "Mdt";
  vname = vname.Strip();
  TGeoVolume *vcave = gGeoManager->FindVolumeFast(vname.Data());

  // General definitions
  TGeoVolume *mdtmf = new TGeoVolumeAssembly("MdtMuonFilter");
  TGeoVolume *mdtmfl00 = new TGeoVolumeAssembly("MdtMuonFilterLayer00");
  TGeoVolume *mdtmfl01 = new TGeoVolumeAssembly("MdtMuonFilterLayer01");
  TGeoVolume *mdtmfl02 = new TGeoVolumeAssembly("MdtMuonFilterLayer02");
  TGeoVolume *mdtmfl03 = new TGeoVolumeAssembly("MdtMuonFilterLayer03");
  TGeoVolume *mdtmfl04 = new TGeoVolumeAssembly("MdtMuonFilterLayer04");
  TGeoVolume *mdtmfl05 = new TGeoVolumeAssembly("MdtMuonFilterLayer05");

  // Additional definitions
  Text_t buffer[30];
  Text_t longbuffer[250];
  // Double_t mpx, mpy, mpz; //[R.K. 01/2017] unused variable?
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
  FairGeoMedium *medmdtArCO2 = Media->getMedium("MDTMixture");
  geobuild->createMedium(medmdtArCO2);

  // creating the holes
  new TGeoBBox("mfhbox2", ((Double_t)PndMdt_H07_H) / 10.0, 10.0 + ((Double_t)PndMdt_SVThickness) / 10.0, ((Double_t)PndMdt_H07_V) / 10.0, 0);
  new TGeoBBox("mfhbox3", ((Double_t)PndMdt_H08_H) / 10.0, 10.0 + ((Double_t)PndMdt_SVThickness) / 10.0, ((Double_t)PndMdt_H08_V) / 10.0, 0);
  new TGeoBBox("mfhbox4", ((Double_t)PndMdt_H09_H) / 10.0, 10.0 + ((Double_t)PndMdt_SVThickness) / 10.0, ((Double_t)PndMdt_H09_V) / 10.0, 0);
  new TGeoBBox("mfhbox5", ((Double_t)PndMdt_H10_H) / 10.0, 10.0 + ((Double_t)PndMdt_SVThickness) / 10.0, ((Double_t)PndMdt_H10_V) / 10.0, 0);
  new TGeoBBox("mfhbox6", ((Double_t)PndMdt_H11_H) / 10.0, 10.0 + ((Double_t)PndMdt_SVThickness) / 10.0, ((Double_t)PndMdt_H11_V) / 10.0, 0);
  new TGeoBBox("mfhbox7", ((Double_t)PndMdt_H12_H) / 10.0, 10.0 + ((Double_t)PndMdt_SVThickness) / 10.0, ((Double_t)PndMdt_H12_V) / 10.0, 0);
  new TGeoBBox("mfhboxpmp2", ((Double_t)PndMdt_PUMP_H) / 10.0, 10.0 + ((Double_t)PndMdt_SVThickness) / 10.0, ((Double_t)PndMdt_PUMP_V) / 10.0, 0);
  new TGeoBBox("mfhboxpmp3", ((Double_t)PndMdt_PUMP_H) / 10.0, 10.0 + ((Double_t)PndMdt_SVThickness) / 10.0, ((Double_t)PndMdt_PUMP_V) / 10.0, 0);
  new TGeoBBox("mfhboxpmp4", ((Double_t)PndMdt_PUMP_H) / 10.0, 10.0 + ((Double_t)PndMdt_SVThickness) / 10.0, ((Double_t)PndMdt_PUMP_V) / 10.0, 0);
  new TGeoBBox("mfhboxpmp5", ((Double_t)PndMdt_PUMP_H) / 10.0, 10.0 + ((Double_t)PndMdt_SVThickness) / 10.0, ((Double_t)PndMdt_PUMP_V) / 10.0, 0);
  new TGeoBBox("mfhboxpmp6", ((Double_t)PndMdt_PUMP_H) / 10.0, 10.0 + ((Double_t)PndMdt_SVThickness) / 10.0, ((Double_t)PndMdt_PUMP_V) / 10.0, 0);
  new TGeoBBox("mfhboxpmp7", ((Double_t)PndMdt_PUMP_H) / 10.0, 10.0 + ((Double_t)PndMdt_SVThickness) / 10.0, ((Double_t)PndMdt_PUMP_V) / 10.0, 0);

  // MdtMF
  tRot.RotateX(90.0);
  TGeoRotation mftgrl;
  mftgrl.RotateY(0.0);
  TGeoRotation *mftgr0 = new TGeoRotation(mftgrl);
  mftgr0->SetName("mftgr0");
  mftgr0->RegisterYourself();
  mftgrl.RotateY(45.0);
  TGeoRotation *mftgr1 = new TGeoRotation(mftgrl);
  mftgr1->SetName("mftgr1");
  mftgr1->RegisterYourself();
  mftgrl.RotateY(45.0);
  TGeoRotation *mftgr2 = new TGeoRotation(mftgrl);
  mftgr2->SetName("mftgr2");
  mftgr2->RegisterYourself();
  mftgrl.RotateY(45.0);
  TGeoRotation *mftgr3 = new TGeoRotation(mftgrl);
  mftgr3->SetName("mftgr3");
  mftgr3->RegisterYourself();
  mftgrl.RotateY(45.0);
  TGeoRotation *mftgr4 = new TGeoRotation(mftgrl);
  mftgr4->SetName("mftgr4");
  mftgr4->RegisterYourself();
  mftgrl.RotateY(45.0);
  TGeoRotation *mftgr5 = new TGeoRotation(mftgrl);
  mftgr5->SetName("mftgr5");
  mftgr5->RegisterYourself();
  mftgrl.RotateY(45.0);
  TGeoRotation *mftgr6 = new TGeoRotation(mftgrl);
  mftgr6->SetName("mftgr6");
  mftgr6->RegisterYourself();
  mftgrl.RotateY(45.0);
  TGeoRotation *mftgr7 = new TGeoRotation(mftgrl);
  mftgr7->SetName("mftgr7");
  mftgr7->RegisterYourself();

  dx2 = 0.0;
  dy1 = (Double_t)PndMdt_SVThickness;
  dy2 = (Double_t)PndMdt_SVThickness;
  dx1 = ((Double_t)PndMdt_MF_Height) * (TMath::Tan(TMath::ACos(-1.0) / 8.0));
  my = ((Double_t)PndMdt_MF_Height) / 2.0;
  dz = ((Double_t)PndMdt_MF_Height) / 2.0;
  TGeoTranslation *mftgt = new TGeoTranslation(0.0, 0.0, my / (-10.0));
  mftgt->SetName("mftgt");
  mftgt->RegisterYourself();
  new TGeoTrd2("mftrd", dx1 / 10.0, dx2 / 10.0, dy1 / 10.0, dy2 / 10.0, dz / 10.0);

  for (int i = 0; i < 6; i++) {
    switch (i) {
    case 0: mz0 = (Double_t)PndMdt_SV300; break;
    case 1: mz0 = (Double_t)PndMdt_SV301; break;
    case 2: mz0 = (Double_t)PndMdt_SV302; break;
    case 3: mz0 = (Double_t)PndMdt_SV303; break;
    case 4: mz0 = (Double_t)PndMdt_SV304; break;
    case 5: mz0 = (Double_t)PndMdt_SV305; break;
    };
    for (int j = 0; j < 8; j++) {
      sprintf(buffer, "mftgcs%i", (8 * i) + j);
      sprintf(longbuffer, "mftrd:mftgt-mfhbox%i:mftgr%i-mfhboxpmp%i:mftgr%i", i + 2, j, i + 2, j);
      TGeoCompositeShape *mftgcs = new TGeoCompositeShape(buffer, longbuffer);
      // sprintf(buffer,"muon%i",300+8*i+j);
      sprintf(buffer, "MDT%is%il%ib%iw%i", 3, j, i, 0, 0);
      TGeoVolume *volume = new TGeoVolume(buffer, mftgcs, gGeoManager->GetMedium("MDTMixture"));
      AddSensitiveVolume(volume);
      volume->SetLineColor(1);
      //	    mdtmf->AddNode(volume,300+8*i+j,new TGeoCombiTrans(0.0,0.0,mz0/10.0,new TGeoRotation(tRot)));
      switch (i) {
      case 0: mdtmfl00->AddNode(volume, 300 + 8 * i + j, new TGeoCombiTrans(0.0, 0.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
      case 1: mdtmfl01->AddNode(volume, 300 + 8 * i + j, new TGeoCombiTrans(0.0, 0.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
      case 2: mdtmfl02->AddNode(volume, 300 + 8 * i + j, new TGeoCombiTrans(0.0, 0.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
      case 3: mdtmfl03->AddNode(volume, 300 + 8 * i + j, new TGeoCombiTrans(0.0, 0.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
      case 4: mdtmfl04->AddNode(volume, 300 + 8 * i + j, new TGeoCombiTrans(0.0, 0.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
      case 5: mdtmfl05->AddNode(volume, 300 + 8 * i + j, new TGeoCombiTrans(0.0, 0.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
      };
      tRot.RotateZ(-45.0);
    };
  };

  mdtmf->AddNode(mdtmfl00, 1);
  mdtmf->AddNode(mdtmfl01, 1);
  mdtmf->AddNode(mdtmfl02, 1);
  mdtmf->AddNode(mdtmfl03, 1);
  mdtmf->AddNode(mdtmfl04, 1);
  mdtmf->AddNode(mdtmfl05, 1);
  vcave->AddNode(mdtmf, 1);

  return;
}

ClassImp(PndMdt)
