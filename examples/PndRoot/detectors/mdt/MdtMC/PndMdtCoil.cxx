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
#include "PndMdtCoil.h"

using namespace std;

void PndMdt::PndMdtCoil()
{
  TString vname = "cave";
  vname = vname.Strip();
  TGeoVolume *vcave = gGeoManager->FindVolumeFast(vname.Data());

  // General definitions
  //  TGeoVolume* mag        = new TGeoVolumeAssembly("PndMdtMagnet");
  TGeoVolume *mdtcoi = new TGeoVolumeAssembly("MdtCoil");

  FairGeoLoader *geoLoad = FairGeoLoader::Instance();
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  FairGeoMedia *Media = geoFace->getMedia();
  FairGeoBuilder *geobuild = geoLoad->getGeoBuilder();
  FairGeoMedium *medmdtiron = Media->getMedium("iron");
  geobuild->createMedium(medmdtiron);

  // MdtMagnetCoil ///////////

  ////////////////////////// SC //////////////////////////////
  /////////// centr NbTi obmotki na radiuse 1075mm
  ////////// 3.2mm NbTi <--> 2.6mm Fe

  //// -90->-13 + 13->47 + 73->150 3 rings //////////////////
  TGeoTube *s_nbti = new TGeoTube("s_nbti", (1075. - 1.3) / 10, (1075.2 + 1.3) / 10, (900 - 130) / 20);
  TGeoVolume *s_nbti_vol = new TGeoVolume("s_nbti_vol", s_nbti, gGeoManager->GetMedium("iron"));
  TGeoTube *s_nbti1 = new TGeoTube("s_nbti1", (1075. - 1.3) / 10, (1075.2 + 1.3) / 10, (470 - 130) / 20);
  TGeoVolume *s_nbti_vol1 = new TGeoVolume("s_nbti_vol1", s_nbti1, gGeoManager->GetMedium("iron"));
  TGeoTube *s_nbti2 = new TGeoTube("s_nbti2", (1075. - 1.3) / 10, (1075.2 + 1.3) / 10, (1500 - 730) / 20);
  TGeoVolume *s_nbti_vol2 = new TGeoVolume("s_nbti_vol2", s_nbti2, gGeoManager->GetMedium("iron"));

  TGeoTube *s_aldown = new TGeoTube("s_aldown", (1057. - 15) / 10, (1057. + 15.) / 10, (900 - 130) / 20);
  //  TGeoVolume* s_aldown_vol     = new TGeoVolume("s_aldown_vol",s_aldown,gGeoManager->GetMedium("aluminium"));
  TGeoVolume *s_aldown_vol = new TGeoVolume("s_aldown_vol", s_aldown, gGeoManager->GetMedium("iron"));
  TGeoTube *s_aldown1 = new TGeoTube("s_aldown1", (1057. - 15) / 10, (1057. + 15.) / 10, (470 - 130) / 20);
  //  TGeoVolume* s_aldown_vol1     = new TGeoVolume("s_aldown_vol1",s_aldown1,gGeoManager->GetMedium("aluminium"));
  TGeoVolume *s_aldown_vol1 = new TGeoVolume("s_aldown_vol1", s_aldown1, gGeoManager->GetMedium("iron"));
  TGeoTube *s_aldown2 = new TGeoTube("s_aldown2", (1057. - 15) / 10, (1057. + 15.) / 10, (1500 - 730) / 20);
  //  TGeoVolume* s_aldown_vol2     = new TGeoVolume("s_aldown_vol2",s_aldown2,gGeoManager->GetMedium("aluminium"));
  TGeoVolume *s_aldown_vol2 = new TGeoVolume("s_aldown_vol2", s_aldown2, gGeoManager->GetMedium("iron"));

  TGeoTube *s_alup = new TGeoTube("s_alup", (1097. - 15.) / 10, (1097. + 15.) / 10, (900 - 130) / 20);
  //  TGeoVolume* s_alup_vol     = new TGeoVolume("s_alup_vol",s_alup,gGeoManager->GetMedium("aluminium"));
  TGeoVolume *s_alup_vol = new TGeoVolume("s_alup_vol", s_alup, gGeoManager->GetMedium("iron"));
  TGeoTube *s_alup1 = new TGeoTube("s_alup1", (1097. - 15.) / 10, (1097. + 15.) / 10, (470 - 130) / 20);
  //  TGeoVolume* s_alup_vol1     = new TGeoVolume("s_alup_vol1",s_alup1,gGeoManager->GetMedium("aluminium"));
  TGeoVolume *s_alup_vol1 = new TGeoVolume("s_alup_vol1", s_alup1, gGeoManager->GetMedium("iron"));
  TGeoTube *s_alup2 = new TGeoTube("s_alup2", (1097. - 15.) / 10, (1097. + 15.) / 10, (1500 - 730) / 20);
  //  TGeoVolume* s_alup_vol2     = new TGeoVolume("s_alup_vol2",s_alup2,gGeoManager->GetMedium("aluminium"));
  TGeoVolume *s_alup_vol2 = new TGeoVolume("s_alup_vol2", s_alup2, gGeoManager->GetMedium("iron"));

  s_nbti_vol->SetLineColor(4);
  s_nbti_vol1->SetLineColor(4);
  s_nbti_vol2->SetLineColor(4);
  s_alup_vol->SetLineColor(2);
  s_alup_vol1->SetLineColor(2);
  s_alup_vol2->SetLineColor(2);
  s_aldown_vol->SetLineColor(2);
  s_aldown_vol1->SetLineColor(2);
  s_aldown_vol2->SetLineColor(2);

  mdtcoi->AddNode(s_nbti_vol, 0, new TGeoTranslation(0.0, 0.0, (-77 - 22.5) / 2));
  mdtcoi->AddNode(s_nbti_vol1, 1, new TGeoTranslation(0.0, 0.0, (47 + 17 / 2) / 2));
  mdtcoi->AddNode(s_nbti_vol2, 2, new TGeoTranslation(0.0, 0.0, (150 + 45 + 77 / 2) / 2));

  mdtcoi->AddNode(s_aldown_vol, 3, new TGeoTranslation(0.0, 0.0, (-77 - 22.5) / 2));
  mdtcoi->AddNode(s_aldown_vol1, 4, new TGeoTranslation(0.0, 0.0, (47 + 17 / 2) / 2));
  mdtcoi->AddNode(s_aldown_vol2, 5, new TGeoTranslation(0.0, 0.0, (150 + 45 + 77 / 2) / 2));

  mdtcoi->AddNode(s_alup_vol, 6, new TGeoTranslation(0.0, 0.0, (-77 - 22.5) / 2));
  mdtcoi->AddNode(s_alup_vol1, 7, new TGeoTranslation(0.0, 0.0, (47 + 17 / 2) / 2));
  mdtcoi->AddNode(s_alup_vol2, 8, new TGeoTranslation(0.0, 0.0, (150 + 45 + 77 / 2) / 2));

  ////////////// end of SC ////////////////////////////////////////

  //////////////  Cryo /////////////////////////////////////////
  TGeoTube *cr_fedown = new TGeoTube("cr_fedown", (960 - 2.) / 10, (960 + 2.) / 10, (1100 + 1790) / 20);
  TGeoVolume *cr_fedown_vol = new TGeoVolume("cr_fedown_vol", cr_fedown, gGeoManager->GetMedium("iron"));

  ///////////// Al(30mm) + Cu(3mm) = Al(30mm) + 3mm*(8.96/2.7) = 40mm
  TGeoTube *cr_aldown = new TGeoTube("cr_aldown", (990 - 20.) / 10, (990 + 20.) / 10, (1100 + 1790) / 20);
  //  TGeoVolume* cr_aldown_vol = new TGeoVolume("cr_aldown_vol",cr_aldown,gGeoManager->GetMedium("aluminium"));
  TGeoVolume *cr_aldown_vol = new TGeoVolume("cr_aldown_vol", cr_aldown, gGeoManager->GetMedium("iron"));

  cr_fedown_vol->SetLineColor(5);
  cr_aldown_vol->SetLineColor(4);

  mdtcoi->AddNode(cr_fedown_vol, 9, new TGeoTranslation(0.0, 0.0, 69. / 2));
  mdtcoi->AddNode(cr_aldown_vol, 10, new TGeoTranslation(0.0, 0.0, 69. / 2));

  ///////////// Al(80mm)//////////////////
  TGeoTube *cr_alup = new TGeoTube("cr_alup", (1180 - 40.) / 10, (1180 + 40.) / 10, (1100 + 1790) / 20);
  //  TGeoVolume* cr_alup_vol = new TGeoVolume("cr_alup_vol",cr_alup,gGeoManager->GetMedium("aluminium"));
  TGeoVolume *cr_alup_vol = new TGeoVolume("cr_alup_vol", cr_alup, gGeoManager->GetMedium("iron"));

  ////////////// Fe(12mm)+Cu(3mm) = Fe(12.4mm) ///////
  TGeoTube *cr_feup = new TGeoTube("cr_feup", (1240 - 6.2) / 10, (1240 + 6.2) / 10, (1100 + 1790) / 20);
  TGeoVolume *cr_feup_vol = new TGeoVolume("cr_feup_vol", cr_feup, gGeoManager->GetMedium("iron"));

  cr_alup_vol->SetLineColor(5);
  cr_alup_vol->SetLineColor(4);

  mdtcoi->AddNode(cr_alup_vol, 11, new TGeoTranslation(0.0, 0.0, 69. / 2));
  mdtcoi->AddNode(cr_feup_vol, 12, new TGeoTranslation(0.0, 0.0, 69. / 2));
  ///////////////////////////////////////////////////////////////
  //  mag->AddNode(magCoil,1);
  //  vcave->AddNode(mag,1);

  vcave->AddNode(mdtcoi, 1);

  return;
}

ClassImp(PndMdt)
