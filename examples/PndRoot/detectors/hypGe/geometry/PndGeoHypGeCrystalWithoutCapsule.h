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

// ******** Header of class to build HypGe encapsulated HPGe Crystal needed to build cluster detectors *************
//					by steinen@kph.uni-mainz.de

#ifndef PNDGEOHYPGECRYSTALWITHOUTCAPSULE_H
#define PNDGEOHYPGECRYSTALWITHOUTCAPSULE_H

#include "TGeoMedium.h"
#include "TGeoTube.h"
#include "TGeoPgon.h"
#include "TGeoTorus.h"
#include "TGeoVolume.h"
//#include "TGeoVolumeAssembly.h"		// does not exist, class in inside TGeoVolume.h
#include "TGeoMatrix.h" // all the translations, rotations ..
#include "TGeoCompositeShape.h"
#include "TMath.h"
#include <stdio.h>
#include "PndGeoHypGeCrystal.h"

class PndGeoHypGeCrystalWithoutCapsule : public PndGeoHypGeCrystal {
 protected:
  Double_t Pi;
  // Media
  // TGeoMedium					*Ge;
  // TGeoMedium					*Al;

  ////objects to build the encapsulation of the crystal
  // Double_t 						CapsuleThickness;
  // Double_t 						InnerScale;
  // Double_t						OuterPgonInnerRadius;
  // Double_t						CapsuleLength;
  // char								*CapsuleName;

  // TGeoTube						*CapsuleOuterTube;
  // TGeoTube						*CapsuleInnerTube;
  // TGeoPgon						*CapsuleOuterPgon;
  // TGeoPgon						*CapsuleInnerPgon;
  // TGeoTube						*CapsuleTopCover;
  // TGeoTranslation			*CapsuleTopZTranslation;
  // TGeoTranslation			*CapsuleInnerZTranslation;
  // TGeoCompositeShape	*CapsuleOuterShape;
  // TGeoCompositeShape	*CapsuleInnerShape;
  // TGeoCompositeShape	*CapsuleCompleteShape;	//OuterShape - InnerShape
  // TGeoVolume					*Capsule;

  ////objects to build the crystal

  // Double_t 						VacuumThickness;
  // Double_t						CrystalLength;
  // Double_t						CrystalFrontPgonInnerRadius;
  // Double_t						CrystalInnerPgonRadius;
  // char								*CrystalName;

  ////objects to build the main crystal shape
  // TGeoTube 						*CrystalTube;
  // TGeoPgon 						*CrystalPgon;
  ////objects to build the inner crystal hole
  // TGeoTube						*CrystalInnerTube;
  // TGeoTranslation			*CrystalInnerTubeZTranslation;
  // TGeoTorus						*CrystalInnerTubeTorus;
  // TGeoTranslation			*CrystalInnerTubeTorusZTranslation;
  // TGeoTube						*CrystalInnerTubeMiniExtraTube;
  // TGeoTranslation			*CrystalInnerTubeMiniExtraTubeZTranslation;
  ////objects to get the top rounding of the crystal
  // TGeoTube						*CrystalTopRoundingTube;
  // TGeoTranslation			*CrystalTopRoundingTubeZTranslation;
  // TGeoTorus						*CrystalTopRoundingTorus;
  // TGeoCompositeShape	*CrystalTopRoundingTooling;
  // TGeoTranslation			*CrystalTopRoundingToolingZTranslation;
  ////objects to build the guard ring of the crystal
  // TGeoTube						*CrystalGuardRingTube;
  // TGeoTranslation			*CrystalGuardRingTubeZTranslation;
  // TGeoTorus						*CrystalGuardRingTorus;
  // TGeoTorus						*CrystalGuardRingTorusInner;
  // TGeoTranslation			*CrystalGuardRingTorusZTranslation;
  // TGeoTube						*CrystalGuardRingMiniExtraTube;
  // TGeoTranslation			*CrystalGuardRingMiniExtraTubeZTranslation;
  ////building the whole crystal
  // TGeoCompositeShape *CrystalShape;
  // TGeoVolume *Crystal;

  ////objects to combine crystal and capsule
  // TGeoVolumeAssembly 	*CrystalPlusCapsule;
  // TGeoCombiTrans			*CapsuleCombiTrans;
  // TGeoCombiTrans			*CrystalInsideCapsuleCombiTrans;
  // Int_t								CrystalNumber;

 public:
  PndGeoHypGeCrystalWithoutCapsule();
  PndGeoHypGeCrystalWithoutCapsule(TGeoMedium *ExtGe, TGeoMedium *ExtAl, Int_t ExtCrystalNumber);
  ~PndGeoHypGeCrystalWithoutCapsule(){};

  Int_t GetCrystalNumber();
  Int_t GetNdaughters();
  void PrintNodes();

  void PlaceCrystal(TGeoVolume *top, TGeoMatrix *CrystalPlaceAndDirectionTranslation); //		places the encapsulated crystal

  ClassDef(PndGeoHypGeCrystalWithoutCapsule, 0); // Class for PndGeoHypGeCrystal
};

#endif /* !PNDGEOHYPGECRYSTAL_H */
