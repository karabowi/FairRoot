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
#include "PndMdtGeoConstructorFast.h"

using namespace std;

// -----   Public method ConstructGeometryFast   -----------------------------
void PndMdt::ConstructGeometryFast()
{
  TString vname = "cave";
  vname = vname.Strip();
  TGeoVolume *vcave = gGeoManager->FindVolumeFast(vname.Data());

  // General definitions
  TGeoVolume *mdt = new TGeoVolumeAssembly("Mdt");
  new TGeoVolumeAssembly("MdtMag"); // TGeoVolume* mdtMag     = // FIXME [R.K. 03/2017] unused variable?
  TGeoVolume *mdtBarrel = new TGeoVolumeAssembly("MdtBarrel");
  TGeoVolume *mdtBL00 = new TGeoVolumeAssembly("MdtBarrelLayer00");
  TGeoVolume *mdtBL01 = new TGeoVolumeAssembly("MdtBarrelLayer01");
  TGeoVolume *mdtBL02 = new TGeoVolumeAssembly("MdtBarrelLayer02");
  TGeoVolume *mdtBL03 = new TGeoVolumeAssembly("MdtBarrelLayer03");
  TGeoVolume *mdtBL04 = new TGeoVolumeAssembly("MdtBarrelLayer04");
  TGeoVolume *mdtBL05 = new TGeoVolumeAssembly("MdtBarrelLayer05");
  TGeoVolume *mdtBL06 = new TGeoVolumeAssembly("MdtBarrelLayer06");
  TGeoVolume *mdtBL07 = new TGeoVolumeAssembly("MdtBarrelLayer07");
  TGeoVolume *mdtBL08 = new TGeoVolumeAssembly("MdtBarrelLayer08");
  TGeoVolume *mdtBL09 = new TGeoVolumeAssembly("MdtBarrelLayer09");
  TGeoVolume *mdtBL10 = new TGeoVolumeAssembly("MdtBarrelLayer10");
  TGeoVolume *mdtBL11 = new TGeoVolumeAssembly("MdtBarrelLayer11");
  TGeoVolume *mdtBL12 = new TGeoVolumeAssembly("MdtBarrelLayer12");
  TGeoVolume *mdtEndcap = new TGeoVolumeAssembly("MdtEndcap");
  TGeoVolume *mdtEL00 = new TGeoVolumeAssembly("MdtEndcapLayer00");
  TGeoVolume *mdtEL01 = new TGeoVolumeAssembly("MdtEndcapLayer01");
  TGeoVolume *mdtEL02 = new TGeoVolumeAssembly("MdtEndcapLayer02");
  TGeoVolume *mdtEL03 = new TGeoVolumeAssembly("MdtEndcapLayer03");
  TGeoVolume *mdtEL04 = new TGeoVolumeAssembly("MdtEndcapLayer04");
  new TGeoVolumeAssembly("MdtEndcapLayer05"); // TGeoVolume* mdtEL05    = //FIXME [R.K. 03/2017] unused variable?

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
  FairGeoMedium *medmdtArCO2 = Media->getMedium("MDTMixture");
  geobuild->createMedium(medmdtArCO2); // Int_t  kMedmdtArCO2= //[R.K.03/2017] unused variable

  // creating the holes
  new TGeoBBox("hbox1", ((Double_t)PndMdt_H01_LengthX) / 10.0, ((Double_t)PndMdt_H01_LengthZ) / 10.0, ((Double_t)PndMdt_SVThickness + 1.) / 10.0, 0);
  new TGeoBBox("hbox2", ((Double_t)PndMdt_H02_H) / 10.0, 10.0 + ((Double_t)PndMdt_SVThickness) / 10.0, ((Double_t)PndMdt_H02_V) / 10.0, 0);
  new TGeoBBox("hbox3", ((Double_t)PndMdt_H03_H) / 10.0, 10.0 + ((Double_t)PndMdt_SVThickness) / 10.0, ((Double_t)PndMdt_H03_V) / 10.0, 0);
  new TGeoBBox("hbox4", ((Double_t)PndMdt_H04_H) / 10.0, 10.0 + ((Double_t)PndMdt_SVThickness) / 10.0, ((Double_t)PndMdt_H04_V) / 10.0, 0);
  new TGeoBBox("hbox5", ((Double_t)PndMdt_H05_H) / 10.0, 10.0 + ((Double_t)PndMdt_SVThickness) / 10.0, ((Double_t)PndMdt_H05_V) / 10.0, 0);
  new TGeoBBox("hbox6", ((Double_t)PndMdt_H06_H) / 10.0, 10.0 + ((Double_t)PndMdt_SVThickness) / 10.0, ((Double_t)PndMdt_H06_V) / 10.0, 0);

  // MdtBarrel
  tRot.RotateX(90.0);
  mpy = ((Double_t)PndMdt_Barrel_Length) / 2.0; // subtract dead space in yoke
  mpz = (Double_t)PndMdt_SVThickness;
  mz0 = (Double_t)PndMdt_Barrel_Displacement;
  for (int i = 0; i < 13; i++) {
    switch (i) {
    case 0: my0 = (Double_t)PndMdt_SV100; break;
    case 1: my0 = (Double_t)PndMdt_SV101; break;
    case 2: my0 = (Double_t)PndMdt_SV102; break;
    case 3: my0 = (Double_t)PndMdt_SV103; break;
    case 4: my0 = (Double_t)PndMdt_SV104; break;
    case 5: my0 = (Double_t)PndMdt_SV105; break;
    case 6: my0 = (Double_t)PndMdt_SV106; break;
    case 7: my0 = (Double_t)PndMdt_SV107; break;
    case 8: my0 = (Double_t)PndMdt_SV108; break;
    case 9: my0 = (Double_t)PndMdt_SV109; break;
    case 10: my0 = (Double_t)PndMdt_SV110; break;
    case 11: my0 = (Double_t)PndMdt_SV111; break;
    case 12: my0 = (Double_t)PndMdt_SV112; break;
    };
    my = my0;
    mpx = (my - mpz) * (TMath::Tan(TMath::ACos(-1.0) / 8.0));
    if (!mdtMagnet)
      mpx = mpx - 55.; // STE: If realistic magnet, planes are a bit smaller because of the yoke structure (edges)
    sprintf(buffer, "box%i", i);
    TGeoBBox *box = new TGeoBBox(buffer, mpx / 10.0, mpy / 10.0, mpz / 10.0, 0);
    TGeoTranslation *tgt = new TGeoTranslation(0.0, mz0 / 10.0, 0.0);
    tgt->SetName("tgt");
    tgt->RegisterYourself();
    sprintf(buffer, "box%i:tgt-hbox1", i);
    TGeoCompositeShape *tgcs = new TGeoCompositeShape("tgcs", buffer);
    for (int j = 0; j < 8; j++) {
      if (j == 0 || j == 4) {
        k = j + 2;
        mx0 = my * TMath::Cos(((Double_t)k) * TMath::ACos(-1.0) / 4.0);
        my0 = my * TMath::Sin(((Double_t)k) * TMath::ACos(-1.0) / 4.0);
        // sprintf(buffer,"muon%i",8*i+j);
        sprintf(buffer, "MDT%is%il%ib%iw%i", 1, j, i, 0, 0);
        TGeoVolume *volume = new TGeoVolume(buffer, tgcs, gGeoManager->GetMedium("MDTMixture"));
        AddSensitiveVolume(volume);
        volume->SetLineColor(1);
        //		mdtBarrel->AddNode(volume,8*i+j,new TGeoCombiTrans(mx0/10.0,my0/10.0,0.0,new TGeoRotation(tRot)));
        switch (i) {
        case 0: mdtBL00->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, 0.0, new TGeoRotation(tRot))); break;
        case 1: mdtBL01->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, 0.0, new TGeoRotation(tRot))); break;
        case 2: mdtBL02->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, 0.0, new TGeoRotation(tRot))); break;
        case 3: mdtBL03->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, 0.0, new TGeoRotation(tRot))); break;
        case 4: mdtBL04->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, 0.0, new TGeoRotation(tRot))); break;
        case 5: mdtBL05->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, 0.0, new TGeoRotation(tRot))); break;
        case 6: mdtBL06->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, 0.0, new TGeoRotation(tRot))); break;
        case 7: mdtBL07->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, 0.0, new TGeoRotation(tRot))); break;
        case 8: mdtBL08->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, 0.0, new TGeoRotation(tRot))); break;
        case 9: mdtBL09->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, 0.0, new TGeoRotation(tRot))); break;
        case 10: mdtBL10->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, 0.0, new TGeoRotation(tRot))); break;
        case 11: mdtBL11->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, 0.0, new TGeoRotation(tRot))); break;
        case 12: mdtBL12->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, 0.0, new TGeoRotation(tRot))); break;
        };
      } else {
        if (j == 2 || j == 6)
          k = j - 2;
        else
          k = j;
        mx0 = my * TMath::Cos(((Double_t)k) * TMath::ACos(-1.0) / 4.0);
        my0 = my * TMath::Sin(((Double_t)k) * TMath::ACos(-1.0) / 4.0);
        // sprintf(buffer,"muon%i",8*i+j);
        sprintf(buffer, "MDT%is%il%ib%iw%i", 1, j, i, 0, 0);
        TGeoVolume *volume = new TGeoVolume(buffer, box, gGeoManager->GetMedium("MDTMixture"));
        AddSensitiveVolume(volume);
        volume->SetLineColor(1);
        //		mdtBarrel->AddNode(volume,8*i+j,new TGeoCombiTrans(mx0/10.0,my0/10.0,mz0/10.0,new TGeoRotation(tRot)));
        switch (i) {
        case 0: mdtBL00->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
        case 1: mdtBL01->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
        case 2: mdtBL02->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
        case 3: mdtBL03->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
        case 4: mdtBL04->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
        case 5: mdtBL05->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
        case 6: mdtBL06->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
        case 7: mdtBL07->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
        case 8: mdtBL08->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
        case 9: mdtBL09->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
        case 10: mdtBL10->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
        case 11: mdtBL11->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
        case 12: mdtBL12->AddNode(volume, 8 * i + j, new TGeoCombiTrans(mx0 / 10.0, my0 / 10.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
        };
      };
      tRot.RotateZ(-45.0);
    };
  };

  mdtBarrel->AddNode(mdtBL00, 1);
  mdtBarrel->AddNode(mdtBL01, 1);
  mdtBarrel->AddNode(mdtBL02, 1);
  mdtBarrel->AddNode(mdtBL03, 1);
  mdtBarrel->AddNode(mdtBL04, 1);
  mdtBarrel->AddNode(mdtBL05, 1);
  mdtBarrel->AddNode(mdtBL06, 1);
  mdtBarrel->AddNode(mdtBL07, 1);
  mdtBarrel->AddNode(mdtBL08, 1);
  mdtBarrel->AddNode(mdtBL09, 1);
  mdtBarrel->AddNode(mdtBL10, 1);
  mdtBarrel->AddNode(mdtBL11, 1);
  mdtBarrel->AddNode(mdtBL12, 1);
  mdt->AddNode(mdtBarrel, 1);

  // MdtEndcap
  if (fEndcap == "Fast" || fEndcap == "fast") {
    TGeoRotation tgrl;
    tgrl.RotateY(0.0);
    TGeoRotation *tgr0 = new TGeoRotation(tgrl);
    tgr0->SetName("tgr0");
    tgr0->RegisterYourself();
    tgrl.RotateY(45.0);
    TGeoRotation *tgr1 = new TGeoRotation(tgrl);
    tgr1->SetName("tgr1");
    tgr1->RegisterYourself();
    tgrl.RotateY(45.0);
    TGeoRotation *tgr2 = new TGeoRotation(tgrl);
    tgr2->SetName("tgr2");
    tgr2->RegisterYourself();
    tgrl.RotateY(45.0);
    TGeoRotation *tgr3 = new TGeoRotation(tgrl);
    tgr3->SetName("tgr3");
    tgr3->RegisterYourself();
    tgrl.RotateY(45.0);
    TGeoRotation *tgr4 = new TGeoRotation(tgrl);
    tgr4->SetName("tgr4");
    tgr4->RegisterYourself();
    tgrl.RotateY(45.0);
    TGeoRotation *tgr5 = new TGeoRotation(tgrl);
    tgr5->SetName("tgr5");
    tgr5->RegisterYourself();
    tgrl.RotateY(45.0);
    TGeoRotation *tgr6 = new TGeoRotation(tgrl);
    tgr6->SetName("tgr6");
    tgr6->RegisterYourself();
    tgrl.RotateY(45.0);
    TGeoRotation *tgr7 = new TGeoRotation(tgrl);
    tgr7->SetName("tgr7");
    tgr7->RegisterYourself();

    dx2 = 0.0;
    dy1 = (Double_t)PndMdt_SVThickness;
    dy2 = (Double_t)PndMdt_SVThickness;

    for (int i = 0; i < 5; i++) {
      switch (i) {
      case 0: mz0 = (Double_t)PndMdt_SV200; break;
      case 1: mz0 = (Double_t)PndMdt_SV201; break;
      case 2: mz0 = (Double_t)PndMdt_SV202; break;
      case 3: mz0 = (Double_t)PndMdt_SV203; break;
      case 4: mz0 = (Double_t)PndMdt_SV204; break;
      };
      if (i == 0) {
        dx1 = ((Double_t)(PndMdt_SV100 - 15)) * (TMath::Tan(TMath::ACos(-1.0) / 8.0));
        my = ((Double_t)(PndMdt_SV100 - 15)) / 2.0;
        dz = ((Double_t)(PndMdt_SV100 - 15)) / 2.0;
        TGeoTranslation *tgt1 = new TGeoTranslation(0.0, 0.0, my / (-10.0));
        tgt1->SetName("tgt1");
        tgt1->RegisterYourself();
        new TGeoTrd2("trd1", dx1 / 10.0, dx2 / 10.0, dy1 / 10.0, dy2 / 10.0, dz / 10.0);
      } else {
        dx1 = ((Double_t)PndMdt_Endcap_Height) * (TMath::Tan(TMath::ACos(-1.0) / 8.0));
        my = ((Double_t)PndMdt_Endcap_Height) / 2.0;
        dz = ((Double_t)PndMdt_Endcap_Height) / 2.0;
        TGeoTranslation *tgt2 = new TGeoTranslation(0.0, 0.0, my / (-10.0));
        tgt2->SetName("tgt2");
        tgt2->RegisterYourself();
        new TGeoTrd2("trd2", dx1 / 10.0, dx2 / 10.0, dy1 / 10.0, dy2 / 10.0, dz / 10.0);
      };
      for (int j = 0; j < 8; j++) {
        sprintf(buffer, "tgcs%i", (8 * i) + j);
        if (i == 0)
          sprintf(longbuffer, "trd1:tgt1-hbox%i:tgr%i", i + 2, j);
        else
          sprintf(longbuffer, "trd2:tgt2-hbox%i:tgr%i", i + 2, j);
        TGeoCompositeShape *tgcs = new TGeoCompositeShape(buffer, longbuffer);
        // sprintf(buffer,"muon%i",200+8*i+j);
        sprintf(buffer, "MDT%is%il%ib%iw%i", 2, j, i, 0, 0);
        TGeoVolume *volume = new TGeoVolume(buffer, tgcs, gGeoManager->GetMedium("MDTMixture"));
        AddSensitiveVolume(volume);
        volume->SetLineColor(1);
        //	    mdtEndcap->AddNode(volume,200+8*i+j,new TGeoCombiTrans(0.0,0.0,mz0/10.0,new TGeoRotation(tRot)));
        switch (i) {
        case 0: mdtEL00->AddNode(volume, 200 + 8 * i + j, new TGeoCombiTrans(0.0, 0.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
        case 1: mdtEL01->AddNode(volume, 200 + 8 * i + j, new TGeoCombiTrans(0.0, 0.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
        case 2: mdtEL02->AddNode(volume, 200 + 8 * i + j, new TGeoCombiTrans(0.0, 0.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
        case 3: mdtEL03->AddNode(volume, 200 + 8 * i + j, new TGeoCombiTrans(0.0, 0.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
        case 4: mdtEL04->AddNode(volume, 200 + 8 * i + j, new TGeoCombiTrans(0.0, 0.0, mz0 / 10.0, new TGeoRotation(tRot))); break;
        };
        tRot.RotateZ(-45.0);
      };
    };

    mdtEndcap->AddNode(mdtEL00, 1);
    mdtEndcap->AddNode(mdtEL01, 1);
    mdtEndcap->AddNode(mdtEL02, 1);
    mdtEndcap->AddNode(mdtEL03, 1);
    mdtEndcap->AddNode(mdtEL04, 1);
    mdt->AddNode(mdtEndcap, 1);
  }

  vcave->AddNode(mdt, 1);

  //  if(mdtMF) PndMdtMuonFilter();

  return;
}
// ----------------------------------------------------------------------------

ClassImp(PndMdt)
