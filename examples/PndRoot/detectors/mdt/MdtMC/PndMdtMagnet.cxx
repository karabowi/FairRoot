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
#include "TGeoTube.h"
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
#include "PndMdtMagnet.h"

using namespace std;

void PndMdt::PndMdtMagnet()
{
  TString vname = "cave";
  vname = vname.Strip();
  TGeoVolume *vcave = gGeoManager->FindVolumeFast(vname.Data());

  // General definitions
  TGeoVolume *mag = new TGeoVolumeAssembly("PndMdtMagnet");
  TGeoVolume *magBarrel = new TGeoVolumeAssembly("PndMdtMagnetBarrel");
  TGeoVolume *magEndcap = new TGeoVolumeAssembly("PndMdtMagnetEndcap");

  // Additional definitions
  Text_t buffer[30];
  Text_t longbuffer[250];
  Double_t mpx, mpy, mpz;
  Double_t mx0, my0, mz0;
  Double_t my;
  int k;
  Double_t dx1, dx2, dy1, dy2, dz;
  TGeoRotation tRot, tRotSlice;
  // Double_t angle; //[R.K. 01/2017] unused variable?

  FairGeoLoader *geoLoad = FairGeoLoader::Instance();
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  FairGeoMedia *Media = geoFace->getMedia();
  FairGeoBuilder *geobuild = geoLoad->getGeoBuilder();
  FairGeoMedium *medmdtiron = Media->getMedium("iron");
  geobuild->createMedium(medmdtiron);

  // creating the holes
  new TGeoBBox("mhbox1", ((Double_t)PndMdtMagnet_H01_Length) / 10.0, ((Double_t)PndMdtMagnet_H01_Length) / 10.0, ((Double_t)PndMdtMagnet_L201) / 10.0, 0);
  new TGeoBBox("mhbox2", ((Double_t)PndMdtMagnet_H02_H) / 10.0, 0.5 + ((Double_t)PndMdtMagnet_Th2) / 10.0, ((Double_t)PndMdtMagnet_H02_V) / 10.0, 0);
  new TGeoBBox("mhbox3", ((Double_t)PndMdtMagnet_H03_H) / 10.0, 0.5 + ((Double_t)PndMdtMagnet_Th2) / 10.0, ((Double_t)PndMdtMagnet_H03_V) / 10.0, 0);
  new TGeoBBox("mhbox4", ((Double_t)PndMdtMagnet_H04_H) / 10.0, 0.5 + ((Double_t)PndMdtMagnet_Th2) / 10.0, ((Double_t)PndMdtMagnet_H04_V) / 10.0, 0);
  new TGeoBBox("mhbox5", ((Double_t)PndMdtMagnet_H05_H) / 10.0, 0.5 + ((Double_t)PndMdtMagnet_Th2) / 10.0, ((Double_t)PndMdtMagnet_H05_V) / 10.0, 0);
  new TGeoBBox("mhbox6", ((Double_t)PndMdtMagnet_H06_H) / 10.0, 0.5 + ((Double_t)PndMdtMagnet_Th2) / 10.0, ((Double_t)PndMdtMagnet_H06_V) / 10.0, 0);

  new TGeoBBox("mhupbox", ((Double_t)PndMdtMagnet_H_U_X) / 10.0, ((Double_t)PndMdtMagnet_H_U_Z) / 10.0, ((Double_t)PndMdtMagnet_L201) / 10.0, 0);
  new TGeoTube("mhuptub", 0., ((Double_t)PndMdtMagnet_H_U_R) / 10.0, ((Double_t)PndMdtMagnet_L201) / 10.0);
  new TGeoTube("mhdotub1", 0., ((Double_t)PndMdtMagnet_H_D_R1) / 10.0, ((Double_t)PndMdtMagnet_L201) / 10.0);
  new TGeoTube("mhdotub2", 0., ((Double_t)PndMdtMagnet_H_D_R2) / 10.0, ((Double_t)PndMdtMagnet_L201) / 10.0);

  // MdtMagnetBarrel
  tRot.RotateX(90.0);
  mpy = ((Double_t)PndMdtMagnet_Barrel_Length) / 2.0;
  mz0 = (Double_t)PndMdtMagnet_Barrel_Displacement;
  for (int i = 0; i < 13; i++) {
    switch (i) {
    case 0:
      my0 = (Double_t)PndMdtMagnet_L100;
      mpz = (Double_t)PndMdtMagnet_Th2;
      break;
    case 1:
      my0 = (Double_t)PndMdtMagnet_L101;
      mpz = (Double_t)PndMdtMagnet_Th1;
      break;
    case 2:
      my0 = (Double_t)PndMdtMagnet_L102;
      mpz = (Double_t)PndMdtMagnet_Th1;
      break;
    case 3:
      my0 = (Double_t)PndMdtMagnet_L103;
      mpz = (Double_t)PndMdtMagnet_Th1;
      break;
    case 4:
      my0 = (Double_t)PndMdtMagnet_L104;
      mpz = (Double_t)PndMdtMagnet_Th1;
      break;
    case 5:
      my0 = (Double_t)PndMdtMagnet_L105;
      mpz = (Double_t)PndMdtMagnet_Th1;
      break;
    case 6:
      my0 = (Double_t)PndMdtMagnet_L106;
      mpz = (Double_t)PndMdtMagnet_Th1;
      break;
    case 7:
      my0 = (Double_t)PndMdtMagnet_L107;
      mpz = (Double_t)PndMdtMagnet_Th1;
      break;
    case 8:
      my0 = (Double_t)PndMdtMagnet_L108;
      mpz = (Double_t)PndMdtMagnet_Th1;
      break;
    case 9:
      my0 = (Double_t)PndMdtMagnet_L109;
      mpz = (Double_t)PndMdtMagnet_Th1;
      break;
    case 10:
      my0 = (Double_t)PndMdtMagnet_L110;
      mpz = (Double_t)PndMdtMagnet_Th1;
      break;
    case 11:
      my0 = (Double_t)PndMdtMagnet_L111;
      mpz = (Double_t)PndMdtMagnet_Th1;
      break;
    case 12:
      my0 = (Double_t)PndMdtMagnet_L112;
      mpz = (Double_t)PndMdtMagnet_Th2;
      break;
    };
    my = my0;
    mpx = (my - mpz) * (TMath::Tan(TMath::ACos(-1.0) / 8.0));
    sprintf(buffer, "mbox%i", i);
    TGeoBBox *mbox = new TGeoBBox(buffer, mpx / 10.0, mpy / 10.0, mpz / 10.0, 0);
    TGeoTranslation *mtgt = new TGeoTranslation(0.0, mz0 / 10.0, 0.0);
    mtgt->SetName("mtgt");
    mtgt->RegisterYourself();

    //  sprintf(buffer,"mbox%i:mtgt-mhbox1",i);
    //       TGeoCompositeShape* mtgcs = new TGeoCompositeShape("mtgcs",buffer);
    for (int j = 0; j < 8; j++) {
      if (j == 0 || j == 4) {
        if (i < 3) {
          sprintf(buffer, "mbox%i:mtgt-mhuptub", i);
        } else {
          if (j == 0)
            sprintf(buffer, "mbox%i:mtgt-mhupbox", i);
          if (j == 4)
            sprintf(buffer, "mbox%i:mtgt-mhdotub2", i);
        }
        // sprintf(buffer,"mbox%i:mtgt-mhbox1",i);
        TGeoCompositeShape *mtgcs = new TGeoCompositeShape("mtgcs", buffer);

        k = j + 2;
        mx0 = my * TMath::Cos(((Double_t)k) * TMath::ACos(-1.0) / 4.0);
        my0 = my * TMath::Sin(((Double_t)k) * TMath::ACos(-1.0) / 4.0);
        sprintf(buffer, "magnet%i", 300 + 8 * i + j);
        TGeoVolume *volume = new TGeoVolume(buffer, mtgcs, gGeoManager->GetMedium("iron"));
        volume->SetLineColor(3);
        magBarrel->AddNode(volume, 300 + 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, 0.0, new TGeoRotation(tRot)));
      } else {
        if (j == 2 || j == 6)
          k = j - 2;
        else
          k = j;
        mx0 = my * TMath::Cos(((Double_t)k) * TMath::ACos(-1.0) / 4.0);
        my0 = my * TMath::Sin(((Double_t)k) * TMath::ACos(-1.0) / 4.0);
        sprintf(buffer, "magnet%i", 300 + 8 * i + j);
        TGeoVolume *volume = new TGeoVolume(buffer, mbox, gGeoManager->GetMedium("iron"));
        volume->SetLineColor(3);
        magBarrel->AddNode(volume, 300 + 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, mz0 / 10.0, new TGeoRotation(tRot)));
      };
      tRot.RotateZ(-45.0);
    };
  };

  // MdtMagnetEndcap

  TGeoRotation magtgrl;
  magtgrl.RotateY(0.0);
  TGeoRotation *magtgr0 = new TGeoRotation(magtgrl);
  magtgr0->SetName("magtgr0");
  magtgr0->RegisterYourself();
  magtgrl.RotateY(45.0);
  TGeoRotation *magtgr1 = new TGeoRotation(magtgrl);
  magtgr1->SetName("magtgr1");
  magtgr1->RegisterYourself();
  magtgrl.RotateY(45.0);
  TGeoRotation *magtgr2 = new TGeoRotation(magtgrl);
  magtgr2->SetName("magtgr2");
  magtgr2->RegisterYourself();
  magtgrl.RotateY(45.0);
  TGeoRotation *magtgr3 = new TGeoRotation(magtgrl);
  magtgr3->SetName("magtgr3");
  magtgr3->RegisterYourself();
  magtgrl.RotateY(45.0);
  TGeoRotation *magtgr4 = new TGeoRotation(magtgrl);
  magtgr4->SetName("magtgr4");
  magtgr4->RegisterYourself();
  magtgrl.RotateY(45.0);
  TGeoRotation *magtgr5 = new TGeoRotation(magtgrl);
  magtgr5->SetName("magtgr5");
  magtgr5->RegisterYourself();
  magtgrl.RotateY(45.0);
  TGeoRotation *magtgr6 = new TGeoRotation(magtgrl);
  magtgr6->SetName("magtgr6");
  magtgr6->RegisterYourself();
  magtgrl.RotateY(45.0);
  TGeoRotation *magtgr7 = new TGeoRotation(magtgrl);
  magtgr7->SetName("magtgr7");
  magtgr7->RegisterYourself();

  dx2 = 0.0;
  dy1 = (Double_t)PndMdtMagnet_Th2;
  dy2 = (Double_t)PndMdtMagnet_Th2;
  dx1 = ((Double_t)PndMdtMagnet_Endcap_Height) * (TMath::Tan(TMath::ACos(-1.0) / 8.0));
  my = ((Double_t)PndMdtMagnet_Endcap_Height) / 2.0;
  dz = ((Double_t)PndMdtMagnet_Endcap_Height) / 2.0;
  TGeoTranslation *magtgt = new TGeoTranslation(0.0, 0.0, my / (-10.0));
  magtgt->SetName("magtgt");
  magtgt->RegisterYourself();
  new TGeoTrd2("magtrd", dx1 / 10.0, dx2 / 10.0, dy1 / 10.0, dy2 / 10.0, dz / 10.0);

  for (int i = 0; i < 5; i++) {
    switch (i) {
    case 0: mz0 = (Double_t)PndMdtMagnet_L200; break;
    case 1: mz0 = (Double_t)PndMdtMagnet_L201; break;
    case 2: mz0 = (Double_t)PndMdtMagnet_L202; break;
    case 3: mz0 = (Double_t)PndMdtMagnet_L203; break;
    case 4: mz0 = (Double_t)PndMdtMagnet_L204; break;
    };
    for (int j = 0; j < 8; j++) {
      sprintf(buffer, "magtgcs%i", (8 * i) + j);
      sprintf(longbuffer, "magtrd:magtgt-mhbox%i:magtgr%i", i + 2, j);
      TGeoCompositeShape *magtgcs = new TGeoCompositeShape(buffer, longbuffer);
      sprintf(buffer, "magnet%i", 500 + 8 * i + j);
      TGeoVolume *volume = new TGeoVolume(buffer, magtgcs, gGeoManager->GetMedium("iron"));
      volume->SetLineColor(3);
      magEndcap->AddNode(volume, 500 + 8 * i + j, new TGeoCombiTrans(0.0, 0.0, mz0 / 10.0, new TGeoRotation(tRot)));
      tRot.RotateZ(-45.0);
    };
  };

  mag->AddNode(magBarrel, 1);
  mag->AddNode(magEndcap, 1);
  vcave->AddNode(mag, 1);

  return;
}

ClassImp(PndMdt)
