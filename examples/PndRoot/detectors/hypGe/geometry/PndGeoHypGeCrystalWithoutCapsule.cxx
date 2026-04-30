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

// ******** Implementation of class to build HypGe encapsulated HPGe Crystal needed to build cluster detectors *************
//					by steinen@kph.uni-mainz.de

#include "PndGeoHypGeCrystalWithoutCapsule.h"

ClassImp(PndGeoHypGeCrystalWithoutCapsule);

// -----   Default constructor   -------------------------------------------
PndGeoHypGeCrystalWithoutCapsule::PndGeoHypGeCrystalWithoutCapsule(){};

// ----  constructor to use ----------------------------------
PndGeoHypGeCrystalWithoutCapsule::PndGeoHypGeCrystalWithoutCapsule(TGeoMedium *ExtGe, TGeoMedium *ExtAl, Int_t ExtCrystalNumber)
{
  Pi = TMath::Pi();
  Ge = ExtGe;
  Al = ExtAl;
  CrystalNumber = ExtCrystalNumber;

  // building a capsule
  OuterPgonInnerRadius = 3.0571;
  CapsuleThickness = 0.08;
  InnerScale = (OuterPgonInnerRadius - CapsuleThickness) / OuterPgonInnerRadius;
  CapsuleLength = 10;

  // building parts of capsule shape
  CapsuleOuterTube = new TGeoTube("CapsuleOuterTube", 0., 3.75, CapsuleLength / 2);
  CapsuleOuterPgon = new TGeoPgon("CapsuleOuterPgon", 0., 360., 6, 2);
  ;
  CapsuleOuterPgon->DefineSection(0, -CapsuleLength / 2, OuterPgonInnerRadius, 3.8);
  CapsuleOuterPgon->DefineSection(1, 4.9094, 3.75, 3.8);
  CapsuleInnerTube = new TGeoTube("CapsuleInnerTube", 0, 3.75 * InnerScale, 5);
  ;
  CapsuleInnerPgon = new TGeoPgon("CapsuleInnerPgon", 0., 360., 6, 2);
  CapsuleInnerPgon->DefineSection(0, -CapsuleLength / 2, OuterPgonInnerRadius - CapsuleThickness, 3.8);
  CapsuleInnerPgon->DefineSection(1, 4.9094, 3.75 - CapsuleThickness, 3.8);
  CapsuleTopCover = new TGeoTube("CapsuleTopCover", 0, 3.75, CapsuleThickness / 2);
  CapsuleTopZTranslation = new TGeoTranslation("CapsuleTopZTranslation", 0, 0, 5 - CapsuleThickness / 2);
  CapsuleTopZTranslation->RegisterYourself();

  // combining simple shapes
  CapsuleOuterShape = new TGeoCompositeShape("CapsuleOuterShape", "(CapsuleOuterTube-CapsuleOuterPgon)");
  CapsuleInnerShape = new TGeoCompositeShape("CapsuleInnerShape", "(CapsuleInnerTube-CapsuleInnerPgon)");
  CapsuleInnerZTranslation = new TGeoTranslation("CapsuleInnerZTranslation", 0, 0, CapsuleThickness);
  CapsuleInnerZTranslation->RegisterYourself();
  CapsuleCompleteShape = new TGeoCompositeShape("CapsuleCompleteShape", "CapsuleOuterShape-CapsuleInnerShape:CapsuleInnerZTranslation+CapsuleTopCover:CapsuleTopZTranslation");

  // building capsule volume
  CapsuleName = new char[11];
  sprintf(CapsuleName, "Capsule", (int)CrystalNumber);
  Capsule = new TGeoVolume(CapsuleName, CapsuleCompleteShape, Al);
  Capsule->SetTransparency(50);
  Capsule->SetLineColor(19);

  // building a crystal

  CapsuleThickness = 0.08;
  VacuumThickness = 0.08;
  CrystalLength = 7.85;
  CrystalFrontPgonInnerRadius = 2.9485;

  // building the main shape of the crystal
  CrystalTube = new TGeoTube("CrystalTube", 0, 3.5, CrystalLength / 2);
  CrystalPgon = new TGeoPgon("CrystalPgon", 0., 360., 6, 2);
  CrystalPgon->DefineSection(0, -CrystalLength / 2, CrystalFrontPgonInnerRadius, 3.8);
  CrystalPgon->DefineSection(1, 8 - CrystalLength / 2, CrystalFrontPgonInnerRadius + 8 * TMath::Tan(Pi / 180 * 4.12), 3.8);

  // building the inner hole of the crystal
  CrystalInnerTube = new TGeoTube("CrystalInnerTube", 0, 0.6, 3.05);
  CrystalInnerTubeZTranslation = new TGeoTranslation("CrystalInnerTubeZTranslation", 0, 0, CrystalLength / 2 - 3.05);
  CrystalInnerTubeZTranslation->RegisterYourself();
  CrystalInnerTubeTorus = new TGeoTorus("CrystalInnerTubeTorus", 0.2, 0, 0.4, 0, 360);
  CrystalInnerTubeTorusZTranslation = new TGeoTranslation("CrystalInnerTubeTorusZTranslation", 0, 0, CrystalLength / 2 - 3.05 * 2);
  CrystalInnerTubeTorusZTranslation->RegisterYourself();
  CrystalInnerTubeMiniExtraTube = new TGeoTube("CrystalInnerTubeMiniExtraTube", 0, 0.2, 0.2);
  CrystalInnerTubeMiniExtraTubeZTranslation = new TGeoTranslation("CrystalInnerTubeMiniExtraTubeZTranslation", 0, 0, CrystalLength / 2 - 3.05 * 2 - 0.2);
  CrystalInnerTubeMiniExtraTubeZTranslation->RegisterYourself();

  // building the top rounding of the crystal
  CrystalTopRoundingTube = new TGeoTube("CrystalTopRoundingTube", 2.9, 3.5, 0.3);
  CrystalTopRoundingTubeZTranslation = new TGeoTranslation("CrystalTopRoundingTubeZTranslation", 0, 0, 0.3);
  CrystalTopRoundingTubeZTranslation->RegisterYourself();
  CrystalTopRoundingTorus = new TGeoTorus("CrystalTopRoundingTorus", 2.9, 0, 0.6, 0, 360);
  CrystalTopRoundingTooling = new TGeoCompositeShape("CrystalTopRoundingTooling", "CrystalTopRoundingTube:CrystalTopRoundingTubeZTranslation-CrystalTopRoundingTorus");
  CrystalTopRoundingToolingZTranslation = new TGeoTranslation("CrystalTopRoundingToolingZTranslation", 0, 0, CrystalLength / 2 - 0.6);
  CrystalTopRoundingToolingZTranslation->RegisterYourself();

  // building the guard ring of the crystal
  CrystalGuardRingTube = new TGeoTube("CrystalGuardRingTube", 0.9, 1.4, 0.15);
  CrystalGuardRingTubeZTranslation = new TGeoTranslation("CrystalGuardRingTubeZTranslation", 0, 0, CrystalLength / 2 - 0.15);
  CrystalGuardRingTubeZTranslation->RegisterYourself();
  CrystalGuardRingTorus = new TGeoTorus("CrystalGuardRingTorus", 1.2, 0, 0.2, 0, 360);
  CrystalGuardRingTorusInner = new TGeoTorus("CrystalGuardRingTorusInner", 1.1, 0, 0.2, 0, 360);
  CrystalGuardRingTorusZTranslation = new TGeoTranslation("CrystalGuardRingTorusZTranslation", 0, 0, CrystalLength / 2 - 0.15 * 2);
  CrystalGuardRingTorusZTranslation->RegisterYourself();
  CrystalGuardRingMiniExtraTube = new TGeoTube("CrystalGuardRingMiniExtraTube", 1.1, 1.2, 0.2);
  CrystalGuardRingMiniExtraTubeZTranslation = new TGeoTranslation("CrystalGuardRingMiniExtraTubeZTranslation", 0, 0, CrystalLength / 2 - 0.15 * 2);
  CrystalGuardRingMiniExtraTubeZTranslation->RegisterYourself();

  // combining the final shape of the crystal
  CrystalShape = new TGeoCompositeShape(
    "CrystalShape", "(CrystalTube-CrystalPgon)-CrystalTopRoundingTooling:CrystalTopRoundingToolingZTranslation-(CrystalInnerTube:CrystalInnerTubeZTranslation+"
                    "CrystalInnerTubeTorus:CrystalInnerTubeTorusZTranslation+CrystalInnerTubeMiniExtraTube:CrystalInnerTubeMiniExtraTubeZTranslation)-(CrystalGuardRingTube:"
                    "CrystalGuardRingTubeZTranslation+CrystalGuardRingTorus:CrystalGuardRingTorusZTranslation+CrystalGuardRingTorusInner:CrystalGuardRingTorusZTranslation+"
                    "CrystalGuardRingMiniExtraTube:CrystalGuardRingMiniExtraTubeZTranslation)"); //	(main shape) - (inner tube) - (top rounding) - (guard ring)

  CrystalName = new char[11];
  sprintf(CrystalName, "Crystal", CrystalNumber);
  Crystal = new TGeoVolume(CrystalName, CrystalShape, Ge);
  Crystal->SetLineColor(kBlue);
  Crystal->SetFillColor(kBlue);

  // combining capsule and crystal

  CrystalPlusCapsule = new TGeoVolumeAssembly("CrystalPlusCapsule");
  CapsuleCombiTrans = new TGeoCombiTrans(0, 0, -CapsuleLength / 2, new TGeoRotation("CapsuleRotation", 0, 180, 0));
  CapsuleCombiTrans->RegisterYourself();
  CrystalInsideCapsuleCombiTrans = new TGeoCombiTrans(0, 0, -(CrystalLength / 2 + CapsuleThickness + VacuumThickness), new TGeoRotation("CrystalRotation", 0, 180, 0));
  CrystalInsideCapsuleCombiTrans->RegisterYourself();

  // CrystalPlusCapsule->AddNode(Capsule, CrystalNumber, CapsuleCombiTrans);
  CrystalPlusCapsule->AddNode(Crystal, CrystalNumber, CrystalInsideCapsuleCombiTrans);
};

Int_t PndGeoHypGeCrystalWithoutCapsule::GetCrystalNumber()
{
  return CrystalNumber;
}
Int_t PndGeoHypGeCrystalWithoutCapsule::GetNdaughters()
{
  CrystalPlusCapsule->GetNdaughters();
};

void PndGeoHypGeCrystalWithoutCapsule::PrintNodes()
{
  CrystalPlusCapsule->PrintNodes();
}

void PndGeoHypGeCrystalWithoutCapsule::PlaceCrystal(TGeoVolume *top, TGeoMatrix *CrystalPlaceAndDirectionTranslation)
{
  top->AddNode(CrystalPlusCapsule, CrystalNumber, CrystalPlaceAndDirectionTranslation);
};
