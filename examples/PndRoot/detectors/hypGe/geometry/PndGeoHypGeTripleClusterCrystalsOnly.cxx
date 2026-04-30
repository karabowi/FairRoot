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

// ******** Implementation of class to build HypGe Triple Cluster Geometry *************
//					by steinen@kph.uni-mainz.de
#include "PndGeoHypGeTripleClusterCrystalsOnly.h"
//#include "PndGeoHypGeCrystal.h"
#include <iostream>

using namespace std;

ClassImp(PndGeoHypGeTripleClusterCrystalsOnly)

  // -----   Default constructor   -------------------------------------------
  PndGeoHypGeTripleClusterCrystalsOnly::PndGeoHypGeTripleClusterCrystalsOnly()
{
  // Constructor
}

PndGeoHypGeTripleClusterCrystalsOnly::PndGeoHypGeTripleClusterCrystalsOnly(TGeoMedium *ExtGe, TGeoMedium *ExtAl, Int_t ExtClusterNumber)
{
  MakeTop = true;

  Ge = ExtGe;
  Al = ExtAl;
  ClusterNumber = ExtClusterNumber;
  StartClusterNumber = ExtClusterNumber;

  NumberOfCryostatEdges = 12;
  NumberOfDifferentZValues = 4;
  NumberOfXYInput = (NumberOfDifferentZValues * 2 - 1) * 2;
  NumberOfFrontArb8 = 5;
  NumberOfTopArb8 = NumberOfFrontArb8;

  // read coordinates from file
  char *InputTitleBuffer;
  ZValuesInputArray = new Double_t[NumberOfDifferentZValues];
  CoordinatesInputArray = new Double_t[NumberOfCryostatEdges * NumberOfXYInput];
  string Filename = getenv("VMCWORKDIR");
  Filename += "/hypGe/geometry/PndGeoHypGeTripleClusterParfile.txt";

  ParametersInputFile.open(Filename.data());
  FillZValues(NumberOfDifferentZValues);
  FillInputArray(NumberOfCryostatEdges, NumberOfXYInput);
  ParametersInputFile.close();

  CryostatFrontThickness = ZValuesInputArray[1] - ZValuesInputArray[0];
  CryostatLowerArb8Heigt = ZValuesInputArray[2] - ZValuesInputArray[1];
  CryostatMiddleArb8Heigt = ZValuesInputArray[3] - ZValuesInputArray[2];
  // CryostatUpperArb8Heigt= ZValuesInputArray[4]-ZValuesInputArray[3];
  CryostatTopThickness = 0.5;
}

// ------- destructor --------------
PndGeoHypGeTripleClusterCrystalsOnly::~PndGeoHypGeTripleClusterCrystalsOnly()
{
  cout << "Destrucor called" << endl;
  delete Crystal1;
  delete Crystal2;
  delete Crystal3;
  delete Crystal1Rotation;
  delete Crystal2Rotation;
  delete Crystal3Rotation;
};

// --------------- build the crystals --------------------------
void PndGeoHypGeTripleClusterCrystalsOnly::BuildCrystals(Int_t *CrystalNumber)
{
  // create 3 crystal objects. The copy number of a crystal is composed of the number of the cluster and a running number for every placed crystal like e.g.: cluster 4, crystal 9:
  // 409 ; cluster 11, crystal 32: 1132
  Crystal1Number = *CrystalNumber;
  *CrystalNumber += 1;
  Crystal2Number = *CrystalNumber;
  *CrystalNumber += 1;
  Crystal3Number = *CrystalNumber;
  *CrystalNumber += 1;
  Crystal1 = new PndGeoHypGeCrystalWithoutCapsule(Ge, Al, 100 * ClusterNumber + Crystal1Number);
  Crystal2 = new PndGeoHypGeCrystalWithoutCapsule(Ge, Al, 100 * ClusterNumber + Crystal2Number);
  Crystal3 = new PndGeoHypGeCrystalWithoutCapsule(Ge, Al, 100 * ClusterNumber + Crystal3Number);

  // set positions of the 3 crystals

  TripleCrystalXOffset = 3.5441;
  TripleCrystalYOffset = 3.0694;
  TripleCrystalZOffset = 0.7343;

  Crystal1Rotation = new TGeoRotation("Crystal1Rotation", -90, 4.62, 90); // angles measured in CAD drawing
  Crystal1Rotation->RegisterYourself();
  Crystal2Rotation = new TGeoRotation("Crystal2Rotation", 30, 4.62, -30);
  Crystal2Rotation->RegisterYourself();
  Crystal3Rotation = new TGeoRotation("Crystal2Rotation", 150, +4.62, -30);
  Crystal3Rotation->RegisterYourself();

  Crystal1Position = new TGeoCombiTrans(TripleCrystalXOffset, 0, -TripleCrystalZOffset, Crystal1Rotation);
  Crystal1Position->RegisterYourself();
  Crystal2Position = new TGeoCombiTrans(-TripleCrystalXOffset / 2, TripleCrystalYOffset, -TripleCrystalZOffset, Crystal2Rotation);
  Crystal2Position->RegisterYourself();
  Crystal3Position = new TGeoCombiTrans(-TripleCrystalXOffset / 2, -TripleCrystalYOffset, -TripleCrystalZOffset, Crystal3Rotation);
  Crystal3Position->RegisterYourself();

  Crystal1->PlaceCrystal(ClusterAssembly, Crystal1Position);
  Crystal2->PlaceCrystal(ClusterAssembly, Crystal2Position);
  Crystal3->PlaceCrystal(ClusterAssembly, Crystal3Position);
}
// ---------------	build the cryostat --------------------------
void PndGeoHypGeTripleClusterCrystalsOnly::BuildCryostat()
{
  // the cryostat is a combination front, side and top parts. The front and the top is build of 5 TGeoArb8. The side parts are split into 12 lower and 12 middle. All are TGeoArb8.
  // This parts are all combined to a TGeoCompositeShape

  string CompositeShapeExpression;
  // the front of the cryostat

  CryostatFrontArb8XY_1 = new Double_t[16];
  BuildTArb8FrontArray(CryostatFrontArb8XY_1, 2, 3, 4, 1, NumberOfXYInput);
  CryostatFrontArb8XY_2 = new Double_t[16];
  BuildTArb8FrontArray(CryostatFrontArb8XY_2, 5, 6, 7, 4, NumberOfXYInput);
  CryostatFrontArb8XY_3 = new Double_t[16];
  BuildTArb8FrontArray(CryostatFrontArb8XY_3, 4, 7, 8, 1, NumberOfXYInput);
  CryostatFrontArb8XY_4 = new Double_t[16];
  BuildTArb8FrontArray(CryostatFrontArb8XY_4, 8, 9, 12, 1, NumberOfXYInput);
  CryostatFrontArb8XY_5 = new Double_t[16];
  BuildTArb8FrontArray(CryostatFrontArb8XY_5, 9, 10, 11, 12, NumberOfXYInput);

  CryostatFrontArb8_1 = new TGeoArb8("CryostatTripleFrontArb8_1", CryostatFrontThickness / 2, CryostatFrontArb8XY_1);
  CryostatFrontArb8_2 = new TGeoArb8("CryostatTripleFrontArb8_2", CryostatFrontThickness / 2, CryostatFrontArb8XY_2);
  CryostatFrontArb8_3 = new TGeoArb8("CryostatTripleFrontArb8_3", CryostatFrontThickness / 2, CryostatFrontArb8XY_3);
  CryostatFrontArb8_4 = new TGeoArb8("CryostatTripleFrontArb8_4", CryostatFrontThickness / 2, CryostatFrontArb8XY_4);
  CryostatFrontArb8_5 = new TGeoArb8("CryostatTripleFrontArb8_5", CryostatFrontThickness / 2, CryostatFrontArb8XY_5);

  CryostatFrontTranslation = new TGeoTranslation("CryostatTripleFrontTranslation", 0, 0, CryostatFrontThickness / 2);
  CryostatFrontTranslation->RegisterYourself();

  for (Int_t i = 1; i < NumberOfFrontArb8 + 1; i++) {
    char buffer[100];
    sprintf(buffer, "CryostatTripleFrontArb8_%d : CryostatTripleFrontTranslation", i);
    CompositeShapeExpression += buffer;
    // if (i<NumberOfFrontArb8)	//uncomment if last part of composite shape
    {
      CompositeShapeExpression += "+";
    }
  }

  // the lower part of the cryostat

  CryostatLowerArb8XY_1 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_1, 1, 2, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_2 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_2, 2, 3, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_3 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_3, 3, 4, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_4 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_4, 4, 5, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_5 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_5, 5, 6, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_6 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_6, 6, 7, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_7 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_7, 7, 8, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_8 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_8, 8, 9, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_9 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_9, 9, 10, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_10 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_10, 10, 11, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_11 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_11, 11, 12, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_12 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_12, 12, 1, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);

  CryostatLowerArb8_1 = new TGeoArb8("CryostatTripleLowerArb8_1", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_1);
  CryostatLowerArb8_2 = new TGeoArb8("CryostatTripleLowerArb8_2", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_2);
  CryostatLowerArb8_3 = new TGeoArb8("CryostatTripleLowerArb8_3", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_3);
  CryostatLowerArb8_4 = new TGeoArb8("CryostatTripleLowerArb8_4", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_4);
  CryostatLowerArb8_5 = new TGeoArb8("CryostatTripleLowerArb8_5", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_5);
  CryostatLowerArb8_6 = new TGeoArb8("CryostatTripleLowerArb8_6", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_6);
  CryostatLowerArb8_7 = new TGeoArb8("CryostatTripleLowerArb8_7", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_7);
  CryostatLowerArb8_8 = new TGeoArb8("CryostatTripleLowerArb8_8", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_8);
  CryostatLowerArb8_9 = new TGeoArb8("CryostatTripleLowerArb8_9", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_9);
  CryostatLowerArb8_10 = new TGeoArb8("CryostatTripleLowerArb8_10", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_10);
  CryostatLowerArb8_11 = new TGeoArb8("CryostatTripleLowerArb8_11", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_11);
  CryostatLowerArb8_12 = new TGeoArb8("CryostatTripleLowerArb8_12", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_12);

  CryostatLowerTranslation = new TGeoTranslation("CryostatTripleLowerTranslation", 0, 0, CryostatFrontThickness + CryostatLowerArb8Heigt / 2);
  CryostatLowerTranslation->RegisterYourself();

  for (Int_t i = 1; i < NumberOfCryostatEdges + 1; i++) {
    char buffer[100];
    sprintf(buffer, "CryostatTripleLowerArb8_%d : CryostatTripleLowerTranslation", i);
    CompositeShapeExpression += buffer;
    // if (i<NumberOfCryostatEdges)		//uncomment if last part of composite shape
    {
      CompositeShapeExpression += "+";
    }
  }

  // the middle part of the cryostat

  CryostatMiddleArb8XY_1 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_1, 1, 2, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatMiddleArb8XY_2 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_2, 2, 3, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatMiddleArb8XY_3 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_3, 3, 4, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatMiddleArb8XY_4 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_4, 4, 5, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatMiddleArb8XY_5 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_5, 5, 6, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatMiddleArb8XY_6 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_6, 6, 7, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatMiddleArb8XY_7 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_7, 7, 8, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatMiddleArb8XY_8 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_8, 8, 9, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatMiddleArb8XY_9 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_9, 9, 10, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatMiddleArb8XY_10 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_10, 10, 11, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatMiddleArb8XY_11 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_11, 11, 12, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatMiddleArb8XY_12 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_12, 12, 1, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);

  CryostatMiddleArb8_1 = new TGeoArb8("CryostatTripleMiddleArb8_1", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_1);
  CryostatMiddleArb8_2 = new TGeoArb8("CryostatTripleMiddleArb8_2", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_2);
  CryostatMiddleArb8_3 = new TGeoArb8("CryostatTripleMiddleArb8_3", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_3);
  CryostatMiddleArb8_4 = new TGeoArb8("CryostatTripleMiddleArb8_4", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_4);
  CryostatMiddleArb8_5 = new TGeoArb8("CryostatTripleMiddleArb8_5", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_5);
  CryostatMiddleArb8_6 = new TGeoArb8("CryostatTripleMiddleArb8_6", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_6);
  CryostatMiddleArb8_7 = new TGeoArb8("CryostatTripleMiddleArb8_7", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_7);
  CryostatMiddleArb8_8 = new TGeoArb8("CryostatTripleMiddleArb8_8", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_8);
  CryostatMiddleArb8_9 = new TGeoArb8("CryostatTripleMiddleArb8_9", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_9);
  CryostatMiddleArb8_10 = new TGeoArb8("CryostatTripleMiddleArb8_10", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_10);
  CryostatMiddleArb8_11 = new TGeoArb8("CryostatTripleMiddleArb8_11", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_11);
  CryostatMiddleArb8_12 = new TGeoArb8("CryostatTripleMiddleArb8_12", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_12);

  CryostatMiddleTranslation = new TGeoTranslation("CryostatTripleMiddleTranslation", 0, 0, CryostatFrontThickness + CryostatLowerArb8Heigt + CryostatMiddleArb8Heigt / 2);
  CryostatMiddleTranslation->RegisterYourself();

  for (Int_t i = 1; i < NumberOfCryostatEdges + 1; i++) {
    char buffer[100];
    sprintf(buffer, "CryostatTripleMiddleArb8_%d : CryostatTripleMiddleTranslation", i);
    CompositeShapeExpression += buffer;
    if (i < NumberOfCryostatEdges) {
      CompositeShapeExpression += "+";
    }
    if (i == NumberOfCryostatEdges && MakeTop) {
      CompositeShapeExpression += "+";
    }
  }

  // the top of the cryostat

  CryostatTopArb8XY_1 = new Double_t[16];
  BuildTArb8TopArray(CryostatTopArb8XY_1, 2, 3, 4, 1, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatTopArb8XY_2 = new Double_t[16];
  BuildTArb8TopArray(CryostatTopArb8XY_2, 5, 6, 7, 4, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatTopArb8XY_3 = new Double_t[16];
  BuildTArb8TopArray(CryostatTopArb8XY_3, 4, 7, 8, 1, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatTopArb8XY_4 = new Double_t[16];
  BuildTArb8TopArray(CryostatTopArb8XY_4, 8, 9, 12, 1, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatTopArb8XY_5 = new Double_t[16];
  BuildTArb8TopArray(CryostatTopArb8XY_5, 9, 10, 11, 12, NumberOfDifferentZValues - 1, NumberOfXYInput);

  CryostatTopArb8_1 = new TGeoArb8("CryostatTripleTopArb8_1", CryostatTopThickness / 2, CryostatTopArb8XY_1);
  CryostatTopArb8_2 = new TGeoArb8("CryostatTripleTopArb8_2", CryostatTopThickness / 2, CryostatTopArb8XY_2);
  CryostatTopArb8_3 = new TGeoArb8("CryostatTripleTopArb8_3", CryostatTopThickness / 2, CryostatTopArb8XY_3);
  CryostatTopArb8_4 = new TGeoArb8("CryostatTripleTopArb8_4", CryostatTopThickness / 2, CryostatTopArb8XY_4);
  CryostatTopArb8_5 = new TGeoArb8("CryostatTripleTopArb8_5", CryostatTopThickness / 2, CryostatTopArb8XY_5);

  CryostatTopTranslation = new TGeoTranslation("CryostatTripleTopTranslation", 0, 0,
                                               CryostatFrontThickness + CryostatLowerArb8Heigt + CryostatMiddleArb8Heigt + CryostatUpperArb8Heigt + CryostatTopThickness / 2);
  CryostatTopTranslation->RegisterYourself();

  if (MakeTop) {
    for (Int_t i = 1; i < NumberOfTopArb8 + 1; i++) {
      char buffer[100];
      sprintf(buffer, "CryostatTripleTopArb8_%d : CryostatTripleTopTranslation", i);
      CompositeShapeExpression += buffer;
      if (i < NumberOfTopArb8) // uncomment if  last part of composite shape
      {
        CompositeShapeExpression += "+";
      }
    }
  }

  // cout << CompositeShapeExpression << endl;

  // combine the cryostat
  CryostatShape = new TGeoCompositeShape("CryostatShape", CompositeShapeExpression.data());
  Cryostat = new TGeoVolume("Cryostat", CryostatShape, Al);
  Cryostat->SetLineColor(kGreen);
  Cryostat->SetFillColor(kGreen);
  // bring the Cryostat in the right position
  CryostatCombiTrans = new TGeoCombiTrans("CryostatCombiTrans", 0, 0, 0, new TGeoRotation("CryostatRotation", 0, 180, 0));
  CryostatCombiTrans->RegisterYourself();

  // ClusterAssembly->AddNode(Cryostat,ClusterNumber,CryostatCombiTrans);
};

// ---------------  Prints the nodes of a cluster -----------------
void PndGeoHypGeTripleClusterCrystalsOnly::PrintNodes(Int_t nLevels)
{
  if (nLevels == 0) {
    cout << "Nothing to Print, use higher number to go deeper in the tree." << endl;
  }
  if (nLevels > 0) {
    cout << "Printing the content of the cluster assembly." << endl;
    cout << endl << "Number of Nodes in cluster assembly: " << ClusterAssembly->GetNdaughters() << endl;
    ClusterAssembly->PrintNodes();
  }
  if (nLevels > 1) {
    cout << "Printing the content of the crystal assemblies." << endl;
    cout << endl << "Number of Nodes in crystal assembly 1: " << Crystal1->GetNdaughters() << endl;
    Crystal1->PrintNodes();
    cout << endl << "Number of Nodes in crystal assembly 2: " << Crystal2->GetNdaughters() << endl;
    Crystal2->PrintNodes();
    cout << endl << "Number of Nodes in crystal assembly 3: " << Crystal3->GetNdaughters() << endl;
    Crystal3->PrintNodes();
  }
}
