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
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TRotation.h"

#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoArb8.h"
#include "TGeoMedium.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "PndGeoHypGe.h"
#include "FairGeoRootBuilder.h"
#include "TGeoMatrix.h"
#include "PndStack.h"
#include "PndHypGe.h"
#include "PndHypGeReader.h"
#include "PndHypGePoint.h"
#include "FairGeoMedium.h"
#include "FairRootManager.h"
#include "FairModule.h"
#include "FairVolume.h"
#include "GeCluster.h"
// add on for debug
//#include "FairGeoG3Builder.h"
#include "FairRuntimeDb.h"
#include "TObjArray.h"
#include "FairRun.h"

ClassImp(GeCluster)

  GeCluster::GeCluster()
{

  SenVolList = new TList();
}
GeCluster::~GeCluster()
{
  delete SenVolList;
}
//

TList *GeCluster::CreateCluster(TGeoPgon *logicCrystal_test, TGeoPgon *lay_sci, TGeoCompositeShape *lay_cap, TGeoVolume &sphere, Double_t distance, Double_t theta, Double_t phi,
                                Double_t depth_first_interaction, int id_start)

//#### real geometry(Euroball)
// TList *GeCluster::CreateCluster(TGeoCompositeShape* logicCrystal_test,
// 			      TGeoVolume& sphere,
// 			      Double_t distance,
// 			      Double_t theta,
// 			      Double_t phi,
// 			      Double_t depth_first_interaction,
// 			      int id_start)

// void GeCluster::CreateCluster(TGeoCompositeShape* logicCrystal_test,
// 			      TGeoVolume& sphere,
// 			      Double_t distance,
// 			      Double_t theta,
// 			      Double_t phi,
// 			      Double_t depth_first_interaction,
// 			      int id_start)

{

  SenVolList->Clear();

  // G4SDManager* SDman = G4SDManager::GetSDMpointer();

  char cluster_name[10];
  if (id_start == 800)
    sprintf(cluster_name, "J");
  if (id_start == 700)
    sprintf(cluster_name, "G");
  if (id_start == 100)
    sprintf(cluster_name, "A");
  if (id_start == 200)
    sprintf(cluster_name, "B");
  if (id_start == 900)
    sprintf(cluster_name, "K");

  if (id_start == 1100)
    sprintf(cluster_name, "M");
  if (id_start == 300)
    sprintf(cluster_name, "C");
  if (id_start == 500)
    sprintf(cluster_name, "E");
  if (id_start == 1500)
    sprintf(cluster_name, "R");
  if (id_start == 1300)
    sprintf(cluster_name, "P");

  if (id_start == 400)
    sprintf(cluster_name, "D");
  if (id_start == 600)
    sprintf(cluster_name, "F");
  if (id_start == 1000)
    sprintf(cluster_name, "L");
  if (id_start == 1200)
    sprintf(cluster_name, "N");

  if (id_start == 1400)
    sprintf(cluster_name, "Q");

  Double_t deg = TMath::Pi() / 180.;
  Double_t phi1;

  fnum_crystal = 0; // each cluster consists of several crystals
  fnum_cluster = 0; // cluster number

  distance = distance + 2.5;

  phi1 = phi * deg;

  if (phi1 > 360.0 * deg)
    phi1 = phi1 - 360.0 * deg;
  if (phi1 >= 0.0 && phi1 <= 180.0 * deg)
    phi1 = 180.0 * deg - phi1;
  if (phi1 > 180.0 * deg && phi1 < 360.0 * deg)
    phi1 = 540.0 * deg - phi1;
  phi1 = phi1 + 90.0 * deg;

  if (phi > 360.0)
    phi = phi - 360.0;
  if (phi >= 0.0 && phi <= 180.0)
    phi = 180.0 - phi;
  if (phi > 180.0 && phi < 360.0)
    phi = 540.0 - phi;
  phi = phi + 90.0;

  TString pclus, psci;
  pclus = fpath;
  psci = fpath;
  pclus += "/hypGe/GeGlusterPos3Clus.dat";
  psci += "/hypGe/GeClusterPosSci31.dat";

  PndHypGeReader *read;
  read = new PndHypGeReader(pclus.Data());
  PndHypGeReader *readsci;
  readsci = new PndHypGeReader(psci.Data());

  TGeoRotation rot3D;
  TRotation rho;
  TRotation rho2;
  TVector3 rotatedPos, PosOrb;

  // TGeoCombiTrans *c1;

  TGeoRotation rot3D2;
  TVector3 rotatedPos2;
  // TGeoCombiTrans *c2;
  rot3D2.SetAngles(0, 0, 0);
  rot3D2.RotateX(theta);
  rot3D2.RotateZ(phi);

  rho2(0, 0);
  rho2.RotateX(theta * deg);
  rho2.RotateZ(phi1);

  rot3D.SetAngles(0, 0, 0);
  rot3D.RotateX(theta);
  rot3D.RotateZ(phi);

  rho(0, 0);
  rho.RotateX(theta * deg);
  rho.RotateZ(phi1);

  TGeoMedium *Ge = gGeoManager->GetMedium("germanium");
  // scintillator plate
  TGeoMedium *sci = gGeoManager->GetMedium("polypropylene");
  // capsule
  TGeoMedium *cap = gGeoManager->GetMedium("HYPaluminium");

  // Int_t geid = Ge->GetId();
  // cout<<" media "<<geid<<endl;

  // TGeoMedium *Si= gGeoManager->GetMedium("air");
  // TGeoMedium *Ge= gGeoManager->Medium("germanium",700,1,1,30.,10.0, 0.1, 0.1, 0.1, 0.1);
  // Germanium
  //  a =  72.61*g/mole;
  // density = 5.323*g/cm3;
  // G4Material* Ge = new G4Material(name="Germanium"  , z=32., a, density);
  // Definition of materials

  // cout<<"media "<<medPWO<<endl;

  char name_crystal[13];
  sprintf(name_crystal, "GeCrystal%02d", id_start + 6);

  char name_lay[13];
  char name_cap[13];
  // scint.
  sprintf(name_lay, "laySci%02d", id_start + 6);
  // capsula
  sprintf(name_cap, "Cap%02d", id_start + 6);

  crystal_centre[fnum_crystal] = new TGeoVolume(name_crystal, (const TGeoShape *)logicCrystal_test, (const TGeoMedium *)Ge);

  laySci[fnum_crystal] = new TGeoVolume(name_lay, (const TGeoShape *)lay_sci, (const TGeoMedium *)sci);

  Cap[fnum_crystal] = new TGeoVolume(name_cap, (const TGeoShape *)lay_cap, (const TGeoMedium *)cap);

  DataG4 data;
  data = read->GetData(id_start + 6);

  //  sphere.AddNode(crystal_centre[fnum_crystal],id_start+6,
  // 		  new TGeoCombiTrans(rotatedPos.X(),
  // 				     rotatedPos.Y(),
  // 				     rotatedPos.Z(),
  // 				     new TGeoRotation (rot3D)));

  sphere.AddNode(crystal_centre[fnum_crystal], id_start + 6, new TGeoCombiTrans(data.posX / 10., data.posY / 10., data.posZ / 10., new TGeoRotation(rot3D)));

  sphere.AddNode(Cap[fnum_crystal], id_start + 6, new TGeoCombiTrans(data.posX / 10., data.posY / 10., (data.posZ / 10.), new TGeoRotation(rot3D)));

  // scint
  DataG4 data2;
  data2 = readsci->GetData(id_start + 6);
  ////cout<<id_start+6<<" x "<<data2.posX<<" y "<<data2.posY<<" z "<<data2.posZ<<endl;

  sphere.AddNode(laySci[fnum_crystal], id_start + 6, new TGeoCombiTrans(data2.posX / 10., data2.posY / 10., (data2.posZ / 10.), new TGeoRotation(rot3D)));

  //------------------------------------------------
  // Sensitive detectors
  //------------------------------------------------

  // AddSensitiveVolume(crystal_centre[fnum_crystal]);
  SenVolList->AddLast(crystal_centre[fnum_crystal]);
  SenVolList->AddLast(laySci[fnum_crystal]);
  SenVolList->AddLast(Cap[fnum_crystal]);

  //  cout  << id_start+6 << " " << rotatedPos.X() << " "
  //        << rotatedPos.Y() << " "
  // 	<< rotatedPos.Z() << std::endl;

  //------------------------------------------------
  // Crystals surrounding the central one.
  //------------------------------------------------

  fnum_crystal = fnum_crystal + 1;

  // Float_t v1[6],v2[6],v3[6];

  TRotation a;
  TVector3 b;
  a.RotateZ(90. * deg);
  b = TVector3(0, 1., 0).Transform(a);

  char tempname[100];
  char periname[13];
  char cappname[13];
  for (int iii = 0; iii <= 1; iii++) {

    rot3D.SetAngles(0., 0., 0.);
    rot3D.RotateX(-6.66);
    rot3D.RotateZ((-1 * iii + 1) * 60.0);
    rot3D.RotateX(theta);
    rot3D.RotateZ(phi);

    // rho(0,0,0);
    rho.RotateX(0. * deg);
    rho.RotateY(0. * deg);
    rho.RotateZ(0. * deg);

    rho.RotateX(-6.66 * deg);
    rho.RotateZ((-1 * iii + 1) * 60.0 * deg);
    rho.RotateX(theta * deg);
    rho.RotateZ(phi1);

    rotatedPos.SetXYZ(0., 0., 0.);
    rotatedPos2.SetXYZ(0., 0., 0.);

    sprintf(tempname, "GeCrystal%04d", id_start + iii);
    // TGeoVolume *crystal[fnum_crystal];

    // scint
    sprintf(periname, "laySci%04d", id_start + iii);
    // capsula
    sprintf(cappname, "Cap%04d", id_start + iii);

    crystal[fnum_crystal] = new TGeoVolume(tempname, (const TGeoShape *)logicCrystal_test, (const TGeoMedium *)Ge);

    laySci_peri[fnum_crystal] = new TGeoVolume(periname, (const TGeoShape *)lay_sci, (const TGeoMedium *)sci);

    Cap_peri[fnum_crystal] = new TGeoVolume(cappname, (const TGeoShape *)lay_cap, (const TGeoMedium *)cap);

    DataG4 data3;
    data3 = read->GetData(id_start + iii);

    // sphere.AddNode(crystal[fnum_crystal],id_start+iii,
    // 		       new TGeoCombiTrans(PosOrb.X(),PosOrb.Y(),
    // 					  PosOrb.Z(), new TGeoRotation (rot3D)));

    sphere.AddNode(crystal[fnum_crystal], id_start + iii, new TGeoCombiTrans(data3.posX / 10., data3.posY / 10., data3.posZ / 10., new TGeoRotation(rot3D)));

    // capsula
    sphere.AddNode(Cap_peri[fnum_crystal], id_start + iii, new TGeoCombiTrans(data3.posX / 10., data3.posY / 10., (data3.posZ / 10.), new TGeoRotation(rot3D)));

    DataG4 data4;
    data4 = readsci->GetData(id_start + iii);

    sphere.AddNode(laySci_peri[fnum_crystal], id_start + iii, new TGeoCombiTrans(data4.posX / 10., data4.posY / 10., (data4.posZ / 10.), new TGeoRotation(rot3D)));

    //------------------------------------------------
    // Sensitive detectors
    //------------------------------------------------

    // AddSensitiveVolume(crystal[fnum_crystal]);

    SenVolList->AddLast(crystal[fnum_crystal]);
    SenVolList->AddLast(laySci_peri[fnum_crystal]);
    SenVolList->AddLast(Cap_peri[fnum_crystal]);

    cout << id_start + iii << " " << data.posX << " " << data.posY << " " << data.posZ << " " << std::endl;

    fnum_crystal = fnum_crystal + 1;
  }

  fnum_cluster = fnum_cluster + 1;

  return SenVolList;
}
