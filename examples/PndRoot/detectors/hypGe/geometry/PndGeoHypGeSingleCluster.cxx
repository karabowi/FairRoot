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

// ******** Implementation of class to build HypGe Double Cluster Geometry *************
//					by steinen@kph.uni-mainz.de
#include "PndGeoHypGeSingleCluster.h"
#include "PndGeoHypGeCrystal.h"
#include <iostream>

ClassImp(PndGeoHypGeSingleCluster)

  using namespace std;
// double Pi = TMath::Pi();
// -----   Default constructor   -------------------------------------------
PndGeoHypGeSingleCluster::PndGeoHypGeSingleCluster()
{
  // Constructor
}
// -----    constructor to use   -------------------------------------------
PndGeoHypGeSingleCluster::PndGeoHypGeSingleCluster(TGeoMedium *ExtGe, TGeoMedium *ExtAl, TGeoMedium *ExtCu, Int_t ExtClusterNumber)
{
  MakeTop = true;

  Ge = ExtGe;
  Al = ExtAl;
  Cu = ExtCu;
  ClusterNumber = ExtClusterNumber;
  StartClusterNumber = ExtClusterNumber;
  NumberOfCryostatEdges = 10;
  NumberOfDifferentZValues = 5;
  NumberOfXYInput = (NumberOfDifferentZValues * 2 - 1) * 2;
  NumberOfFrontArb8 = 4;
  NumberOfTopArb8 = NumberOfFrontArb8;
  char *InputTitleBuffer;
  ZValuesInputArray = new Double_t[NumberOfDifferentZValues];
  CoordinatesInputArray = new Double_t[NumberOfCryostatEdges * NumberOfXYInput];

  CryostatFrontThickness = 0.1; // ZValuesInputArray[1]-ZValuesInputArray[0];
  CryostatSideHeight = 12;
  CryostatBack1Height = 1;
  CryostatBack2Height = 20;
  ColdFinger1Height = 10;
  ColdFinger1Height = 10;
}

// ------- destructor --------------
PndGeoHypGeSingleCluster::~PndGeoHypGeSingleCluster()
{
  cout << "Destrucor called" << endl;
};

// --------------- build the crystals ---------------------------

void PndGeoHypGeSingleCluster::BuildCrystals(Int_t *CrystalNumber)
{
  // create 2 crystals. The copy number of a crystal is composed of the number of the cluster and a running number for every placed crystal like e.g.: cluster 4, crystal 7: 407 ;
  // cluster 11, crystal 21: 1121
  Crystal1Number = *CrystalNumber;
  *CrystalNumber += 1;
  cout << "Cluster:" << ClusterNumber << "\tCrystal1Number: " << Crystal1Number << endl;

  Crystal1 = new PndGeoHypGeCrystal(Ge, Al, 100 * ClusterNumber + Crystal1Number);

  // set positions of the 2 crystals

  SingleCrystalZOffset = 0.3; // measured in CAD

  Crystal1Rotation = new TGeoRotation("Crystal1Rotation", 0, 0, 0); // angles measured in CAD drawing
  Crystal1Rotation->RegisterYourself();

  // maybe some signs needs adaption
  Crystal1Position = new TGeoCombiTrans(0, 0, -SingleCrystalZOffset, Crystal1Rotation);
  Crystal1Position->RegisterYourself();

  Crystal1->PlaceCrystal(ClusterAssembly, Crystal1Position);
};

// ---------------	build the cryostat --------------------------
void PndGeoHypGeSingleCluster::BuildCryostat()
{
  // the cryostat is a combination front, side and top parts. The single crystal detector is easy and consists of only 2 +x parts

  string CompositeShapeExpression;

  // cout << CompositeShapeExpression << endl;

  CryostatFront = new TGeoTube("CryostatFront", 0, 6, CryostatFrontThickness / 2);
  CryostatSide = new TGeoTube("CryostatSide", 6, 6.2, CryostatSideHeight / 2);
  CryostatBack1 = new TGeoTube("CryostatBack1", 4.2, 6, CryostatBack1Height / 2);
  CryostatBack2 = new TGeoTube("CryostatBack2", 4, 4.2, CryostatBack2Height / 2);

  CryostatFrontTranslation = new TGeoTranslation("CryostatFrontTranslation", 0, 0, CryostatFrontThickness / 2);
  CryostatFrontTranslation->RegisterYourself();
  CryostatSideTranslation = new TGeoTranslation("CryostatSideTranslation", 0, 0, CryostatSideHeight / 2);
  CryostatSideTranslation->RegisterYourself();

  CryostatBack1Translation = new TGeoTranslation("CryostatBack1Translation", 0, 0, CryostatSideHeight + CryostatBack1Height / 2);
  CryostatBack1Translation->RegisterYourself();
  CryostatBack2Translation = new TGeoTranslation("CryostatBack2Translation", 0, 0, CryostatSideHeight + CryostatBack2Height / 2);
  CryostatBack2Translation->RegisterYourself();

  CompositeShapeExpression =
    "CryostatFront : CryostatFrontTranslation + CryostatSide : CryostatSideTranslation + CryostatBack1 : CryostatBack1Translation + CryostatBack2 : CryostatBack2Translation";

  // cout << CompositeShapeExpression << endl;
  // combine the cryostat
  CryostatShape = new TGeoCompositeShape("CryostatShape", CompositeShapeExpression.data());
  Cryostat = new TGeoVolume("Cryostat", CryostatShape, Al);
  Cryostat->SetLineColor(kGreen);
  Cryostat->SetFillColor(kGreen);

  // add the coldfinger
  ColdFinger1 = new TGeoTube("ColdFinger1", 0, 1.5, ColdFinger1Height / 2);
  ColdFinger2 = new TGeoTube("ColdFinger2", 0, 1.5, ColdFinger2Height / 2);

  ColdFinger1Translation = new TGeoTranslation("ColdFinger1Translation", 0, 0, -SingleCrystalZOffset + 10 + ColdFinger1Height / 2);
  ColdFinger1Translation->RegisterYourself();
  ColdFinger2Translation = new TGeoTranslation("ColdFinger2Translation", 0, 0, -SingleCrystalZOffset + 10 + ColdFinger1Height + ColdFinger2Height / 2);
  ColdFinger2Translation->RegisterYourself();

  string ColdFingerShapeExpression = "ColdFinger1 : ColdFinger1Translation + ColdFinger2 : ColdFinger2Translation"; // problem here? no trans if only one part?
  ColdFingerShape = new TGeoCompositeShape("ColdFingerShape", ColdFingerShapeExpression.data());
  ColdFinger = new TGeoVolume("ColdFinger", ColdFingerShape, Cu);
  // bring the Cryostat in the right position
  CryostatCombiTrans = new TGeoCombiTrans("CryostatCombiTrans", 0, 0, 0, new TGeoRotation("CryostatRotation", 0, 180, 0));
  CryostatCombiTrans->RegisterYourself();

  ClusterAssembly->AddNode(Cryostat, ClusterNumber, CryostatCombiTrans);
  ClusterAssembly->AddNode(ColdFinger, ClusterNumber, CryostatCombiTrans);
};

// ---------------  Prints the nodes of a cluster -----------------
void PndGeoHypGeSingleCluster::PrintNodes(Int_t nLevels)
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
  }
}
