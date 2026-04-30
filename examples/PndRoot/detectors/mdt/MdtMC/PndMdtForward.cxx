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
#include "PndMdtForward.h"

using namespace std;

void PndMdt::PndMdtForward()
{
  TString vname = "Mdt";
  vname = vname.Strip();
  TGeoVolume *vcave = gGeoManager->FindVolumeFast(vname.Data());

  // General definitions
  TGeoVolume *mdtfwd = new TGeoVolumeAssembly("MdtForward");

  FairGeoLoader *geoLoad = FairGeoLoader::Instance();
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  FairGeoMedia *Media = geoFace->getMedia();
  FairGeoBuilder *geobuild = geoLoad->getGeoBuilder();
  FairGeoMedium *medmdtArCO2 = Media->getMedium("MDTMixture");
  geobuild->createMedium(medmdtArCO2);
  FairGeoMedium *medmdtFe = Media->getMedium("iron");
  geobuild->createMedium(medmdtFe);
  FairGeoMedium *medmdtVac = Media->getMedium("vacuum");
  geobuild->createMedium(medmdtVac);

  // char fe[12], hl[12], ffe[12], gas[12], hgas[12], ggas[12]; //[R.K. 01/2017] unused variable?
  Char_t buffer[255];
  TGeoTranslation *frotfe[16];
  TGeoTranslation *frotgas[17];
  TGeoVolume *FeForward[16];
  TGeoVolume *GasForward[17];

  Double_t fwd_dx = (Double_t)PndMdt_FWD_DX;
  Double_t fwd_dy = (Double_t)PndMdt_FWD_DY;
  Double_t fwd_fedz = (Double_t)PndMdt_FWD_Fe_DZ;
  Double_t fwd_holedx = (Double_t)PndMdt_FWD_Hole_DX;
  Double_t fwd_holedy = (Double_t)PndMdt_FWD_Hole_DY;
  Double_t fwd_thick = (Double_t)PndMdt_SVThickness;

  gGeoManager->MakeBox("fe", gGeoManager->GetMedium("iron"), fwd_dx / 20., fwd_dy / 20., fwd_fedz / 20.);
  gGeoManager->MakeBox("hl", gGeoManager->GetMedium("vacuum"), fwd_holedx / 20., fwd_holedy / 20., fwd_fedz / 20. + 1.);

  gGeoManager->MakeBox("gas", gGeoManager->GetMedium("MDTMixture"), fwd_dx / 20., fwd_dy / 20., fwd_thick / 20.);
  gGeoManager->MakeBox("hgas", gGeoManager->GetMedium("vacuum"), fwd_holedx / 20., fwd_holedy / 20., fwd_thick / 20. + 1.);

  TGeoTranslation *vv = new TGeoTranslation((15 + 1.8), 0, 0);
  vv->SetName("vv");
  vv->RegisterYourself();
  TGeoCompositeShape *csfe = new TGeoCompositeShape("ffe", "(fe - (hl:vv))");
  TGeoCompositeShape *csgas = new TGeoCompositeShape("ggas", "(gas - (hgas:vv))");

  // Fe layers
  int yt = 0, kk = 0;
  for (int nFeLayer = 0; nFeLayer < 16; nFeLayer++) {
    yt = (nFeLayer % 4);
    if (yt == 0)
      kk++;
    frotfe[nFeLayer] = new TGeoTranslation(kk * 1.45, 0, 873 + 9 * nFeLayer);
    FeForward[nFeLayer] = new TGeoVolume(Form("FeForwardLayer%d", nFeLayer), csfe, gGeoManager->GetMedium("iron"));
    frotfe[nFeLayer]->SetName(Form("frotfe%d", nFeLayer));
    frotfe[nFeLayer]->RegisterYourself();

    mdtfwd->AddNode(FeForward[nFeLayer], nFeLayer, frotfe[nFeLayer]);
  }
  // end of Fe layers

  // // MDT gas layers
  yt = 0;
  kk = 0;
  for (int nLayer = 0; nLayer < 17; nLayer++) {
    sprintf(buffer, "MDT%is%il%ib%iw%i", 4, 0, nLayer, 0, 0);
    yt = (nLayer % 4);
    if (yt == 0)
      kk++;
    if (nLayer != 0) {
      frotgas[nLayer] = new TGeoTranslation(kk * 1.45, 0, 876.1 + fwd_thick / 20. + 9 * (nLayer - 1));
      GasForward[nLayer] = new TGeoVolume(buffer, csgas, gGeoManager->GetMedium("MDTMixture"));
    } else {
      frotgas[nLayer] = new TGeoTranslation(kk * 1.45, 0, 869.9 - fwd_thick / 20.);
      GasForward[nLayer] = new TGeoVolume(buffer, csgas, gGeoManager->GetMedium("MDTMixture"));
    }
    frotgas[nLayer]->SetName(Form("frotgas%d", nLayer));
    frotgas[nLayer]->RegisterYourself();
    AddSensitiveVolume(GasForward[nLayer]);
    mdtfwd->AddNode(GasForward[nLayer], nLayer, frotgas[nLayer]);
  }
  // end of MDT gas layers

  vcave->AddNode(mdtfwd, 1);

  return;
}

ClassImp(PndMdt)
