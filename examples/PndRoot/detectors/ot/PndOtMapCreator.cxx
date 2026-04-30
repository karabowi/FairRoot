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

////////////////////////////////////////////////////////////////////////////
// PndOtMapCreator source file
//
// Mapper for OT
//
// authors: Radoslaw Karabowicz, GSI, 2024
//
// modified from PndGeoFts by Nafija Ibrišimović in 2023
////////////////////////////////////////////////////////////////////////////

// from PandaRoot, this library
#include "PndOtMapCreator.h"
#include "PndOtTube.h"
#include "PndGeoOtPar.h"
// from FairRoot
#include <FairGeoNode.h>
#include <FairGeoVolume.h>
#include <FairGeoTransform.h>
#include <FairGeoVector.h>
#include <FairGeoRotation.h>
#include <FairGeoTube.h>
// from ROOT
#include <TGeoTube.h>
#include <TVector3.h>
#include <TObjArray.h>
#include <TString.h>
#include <TGeoVolume.h>
#include <TGeoTube.h>
#include <TClonesArray.h>
#include <FairRootManager.h>
// standard
#include <iostream>
#include <fstream>

using namespace std;

PndOtMapCreator::PndOtMapCreator() : fGeoType(0), fTubeInRad(0), fTubeOutRad(0), fOtParameters(new PndGeoOtPar()) {}

// PndOtMapCreator::PndOtMapCreator(){}
// to use in PndOt
PndOtMapCreator::PndOtMapCreator(Int_t geoType) : fGeoType(geoType), fTubeInRad(0), fTubeOutRad(0), fOtParameters(new PndGeoOtPar("PndGeoOtPar", "OT Geometry Parameters"))
{
  if (fGeoType != 1)
    LOG(fatal) << "[OT] PndOtMapCreator. Geometry " << fGeoType << " not supported by map";
  // cout << "Creating mapper with geoType = " << fGeoType << endl;
  // cout << "fOtParameters name is " << fOtParameters->GetName() << endl;
}

PndOtMapCreator::PndOtMapCreator(PndGeoOtPar *otPar) : fOtParameters(otPar)
{
  fTubeInRad = fOtParameters->GetTubeInRad();   // tube inner radius
  fTubeOutRad = fOtParameters->GetTubeOutRad(); // tube outer radius
  // choose geometry type
  fGeoType = otPar->GetGeometryType(); // classic, optimized, average, detailed, CAD
  //  cout << "Creating map with geoType = " << fGeoType << ", radius = " << fTubeInRad << " / " << fTubeOutRad << endl;
}

PndOtMapCreator::~PndOtMapCreator() {}

void PndOtMapCreator::SetGeneralParameters()
{                                               //  CHECK whether it depends on geometry or not
  fTubeInRad = fOtParameters->GetTubeInRad();   // tube inner radius
  fTubeOutRad = fOtParameters->GetTubeOutRad(); // tube outer radius
}

Int_t PndOtMapCreator::GetTubeIDFromPath(TString path)
{
  TString tmpstring = GetNameFromPath(path);
  return GetTubeIDFromName(tmpstring);
}

// ===== GEO TYPE 1 =====
// sensitive volume is gas
// when reading the parameters we use ot01tube to retrieve geometrical information
// OK
// during simulation

Int_t PndOtMapCreator::GetTubeIDFromName(TString name)
{

  TString tmpstring = name;
  tmpstring.ReplaceAll('_', '#');

  if (tmpstring.Contains("#")) {
    int start = tmpstring.Index("#") + 1;
    tmpstring = tmpstring(start, tmpstring.Sizeof());
  } else {
    int start = tmpstring.Index("e") + 1;
    tmpstring = tmpstring(start, tmpstring.Sizeof());
  }

  //  cout << "GetTubeIDFromName(" << name.Data() << ") = " << tmpstring.Data() << " = " << tmpstring.Atoi() << endl;

  return tmpstring.Atoi();
}

// name as in geo file: from path "_" to name "#"
TString PndOtMapCreator::GetNameFromPath(TString path)
{
  TString tmpstring = path;
  tmpstring.Remove(tmpstring.Last('/'), tmpstring.Length());

  tmpstring.Remove(0, tmpstring.Last('/'));

  return tmpstring;
}

int PndOtMapCreator::GetTypeNumTube(TString path, int &module_type, int &module_number, int &tube_in_module)
{

  if (path.Contains("cave") && path.Contains("assembly"))
    path.Replace(0, 34, "");
  path.Replace(0, path.First("M") - 1, "");
  module_type = path.Atoi();
  path.Replace(0, path.First("M") + 1, "");
  module_number = path.Atoi();
  path.Replace(0, path.First("M") + 1, "");
  path.Replace(0, path.First("_") + 1, "");
  tube_in_module = path.Atoi();
  //  cout << "GetTypeNumTube will return " << module_type << ", " << module_number << ", " << tube_in_module << endl;
  return 0;
}

Int_t PndOtMapCreator::GetLayerID(Int_t chamberid, TString path)
{

  int module_type{0};
  int module_number{0};
  int tube_in_module{0};
  GetTypeNumTube(path, module_type, module_number, tube_in_module);
  //  cout << "Point detected in module S" << module_type << " number " << module_number << " in the tube " << tube_in_module << " of the module." << endl;

  int layerid = 0;
  if (chamberid % 2 == 1) {
    if (module_type == 1) {
      if (tube_in_module <= 64 && module_number <= 5)
        layerid = 1;
      else if (64 < tube_in_module && tube_in_module <= 2 * 64 && module_number <= 5)
        layerid = 2;
      else if (tube_in_module <= 64 && 5 < module_number && module_number <= 10)
        layerid = 3;
      else if (64 < tube_in_module && tube_in_module <= 2 * 64 && 5 < module_number && module_number <= 10)
        layerid = 4;
      else if (tube_in_module <= 64 && 10 < module_number && module_number <= 15)
        layerid = 5;
      else if (64 < tube_in_module && tube_in_module <= 2 * 64 && 10 < module_number && module_number <= 15)
        layerid = 6;
      else if (tube_in_module <= 64 && 15 < module_number && module_number <= 20)
        layerid = 7;
      else if (64 < tube_in_module && tube_in_module <= 2 * 64 && 15 < module_number && module_number <= 20)
        layerid = 8;
    }
    if (module_type == 2) {
      if (tube_in_module <= 64 && module_number <= 4)
        layerid = 1;
      else if (64 < tube_in_module && tube_in_module <= 2 * 64 && module_number <= 4)
        layerid = 2;
      else if (tube_in_module <= 64 && 4 < module_number && module_number <= 8)
        layerid = 3;
      else if (64 < tube_in_module && tube_in_module <= 2 * 64 && 4 < module_number && module_number <= 8)
        layerid = 4;
      else if (tube_in_module <= 64 && 8 < module_number && module_number <= 12)
        layerid = 5;
      else if (64 < tube_in_module && tube_in_module <= 2 * 64 && 8 < module_number && module_number <= 12)
        layerid = 6;
      else if (tube_in_module <= 64 && 12 < module_number && module_number <= 16)
        layerid = 7;
      else if (64 < tube_in_module && tube_in_module <= 2 * 64 && 12 < module_number && module_number <= 16)
        layerid = 8;
    }
    if (module_type == 3) {
      if (tube_in_module <= 32 && module_number <= 3)
        layerid = 1;
      else if (32 < tube_in_module && tube_in_module <= 2 * 32 && module_number <= 3)
        layerid = 2;
      else if (tube_in_module <= 32 && 3 < module_number && module_number <= 6)
        layerid = 3;
      else if (32 < tube_in_module && tube_in_module <= 2 * 32 && 3 < module_number && module_number <= 6)
        layerid = 4;
      else if (tube_in_module <= 32 && 6 < module_number && module_number <= 9)
        layerid = 5;
      else if (32 < tube_in_module && tube_in_module <= 2 * 32 && 6 < module_number && module_number <= 9)
        layerid = 6;
      else if (tube_in_module <= 32 && 9 < module_number && module_number <= 12)
        layerid = 7;
      else if (32 < tube_in_module && tube_in_module <= 2 * 32 && 9 < module_number && module_number <= 12)
        layerid = 8;
    }
  } else if (chamberid % 2 == 0) {
    if (module_type == 1) {
      if (tube_in_module <= 64 && module_number <= 7)
        layerid = 1;
      else if (64 < tube_in_module && tube_in_module <= 2 * 64 && module_number <= 7)
        layerid = 2;
      else if (tube_in_module <= 64 && 7 < module_number && module_number <= 14)
        layerid = 3;
      else if (64 < tube_in_module && tube_in_module <= 2 * 64 && 7 < module_number && module_number <= 14)
        layerid = 4;
      else if (tube_in_module <= 64 && 14 < module_number && module_number <= 21)
        layerid = 5;
      else if (64 < tube_in_module && tube_in_module <= 2 * 64 && 14 < module_number && module_number <= 21)
        layerid = 6;
      else if (tube_in_module <= 64 && 21 < module_number && module_number <= 28)
        layerid = 7;
      else if (64 < tube_in_module && tube_in_module <= 2 * 64 && 21 < module_number && module_number <= 28)
        layerid = 8;
    }
    if (module_type == 2) {
      if (tube_in_module <= 64 && module_number <= 2)
        layerid = 1;
      else if (64 < tube_in_module && tube_in_module <= 2 * 64 && module_number <= 2)
        layerid = 2;
      else if (tube_in_module <= 64 && 2 < module_number && module_number <= 4)
        layerid = 3;
      else if (64 < tube_in_module && tube_in_module <= 2 * 64 && 2 < module_number && module_number <= 4)
        layerid = 4;
      else if (tube_in_module <= 64 && 4 < module_number && module_number <= 6)
        layerid = 5;
      else if (64 < tube_in_module && tube_in_module <= 2 * 64 && 4 < module_number && module_number <= 6)
        layerid = 6;
      else if (tube_in_module <= 64 && 6 < module_number && module_number <= 8)
        layerid = 7;
      else if (64 < tube_in_module && tube_in_module <= 2 * 64 && 6 < module_number && module_number <= 8)
        layerid = 8;
    }
    if (module_type == 3) {
      if (tube_in_module <= 32 && module_number <= 3)
        layerid = 1;
      else if (32 < tube_in_module && tube_in_module <= 2 * 32 && module_number <= 3)
        layerid = 2;
      else if (tube_in_module <= 32 && 3 < module_number && module_number <= 6)
        layerid = 3;
      else if (32 < tube_in_module && tube_in_module <= 2 * 32 && 3 < module_number && module_number <= 6)
        layerid = 4;
      else if (tube_in_module <= 32 && 6 < module_number && module_number <= 9)
        layerid = 5;
      else if (32 < tube_in_module && tube_in_module <= 2 * 32 && 6 < module_number && module_number <= 9)
        layerid = 6;
      else if (tube_in_module <= 32 && 9 < module_number && module_number <= 12)
        layerid = 7;
      else if (32 < tube_in_module && tube_in_module <= 2 * 32 && 9 < module_number && module_number <= 12)
        layerid = 8;
    }
    layerid = layerid + 8;
  }
  //    cout << "will return layer id = " << layerid << endl;
  return layerid;
}

Int_t PndOtMapCreator::IsSkew(Int_t layerid)
{
  Int_t skew = 0;
  // inclination of +5deg: skew=+1
  if (layerid == 3 || layerid == 4 || layerid == 11 || layerid == 12 || layerid == 19 || layerid == 20 || layerid == 27 || layerid == 28 || layerid == 35 || layerid == 36 ||
      layerid == 43 || layerid == 44) {
    skew = 1;
  }
  // inclination of -5deg: skew=-1
  if (layerid == 5 || layerid == 6 || layerid == 13 || layerid == 14 || layerid == 21 || layerid == 22 || layerid == 29 || layerid == 30 || layerid == 37 || layerid == 38 ||
      layerid == 45 || layerid == 46) {
    skew = -1;
  }

  return skew;
}

Int_t PndOtMapCreator::GetOTTubeIDTot(Int_t chamberid, Int_t layerid, Int_t module_type, Int_t module_number, Int_t tube_in_module, Int_t strawCh12)
{
  int tubeidtot = 0;
  int modStraws[3] = {64, 64, 32};
  if (chamberid % 2 == 1) {
    tubeidtot = tube_in_module + fModShiftOT1[module_type - 1][module_number - 1] + (layerid % 2 == 1 ? 0 : strawCh12 - modStraws[module_type - 1]);
  } else if (chamberid % 2 == 0) {
    tubeidtot = tube_in_module + fModShiftOT2[module_type - 1][module_number - 1] + (layerid % 2 == 1 ? 0 : strawCh12 - modStraws[module_type - 1]);
  }
  return tubeidtot;
}

Int_t PndOtMapCreator::GetTubeIDTot(Int_t chamberid, Int_t layerid, TString path)
{

  TString tmpstring = path;
  Int_t tube = 0;
  Int_t totTubeID = 0;
  Int_t strawCh12 = 672;
  int module_type{0};
  int module_number{0};
  int tube_in_module{0};
  GetTypeNumTube(path, module_type, module_number, tube_in_module);
  tube = GetOTTubeIDTot(chamberid, layerid, module_type, module_number, tube_in_module, strawCh12);
  //  cout << path << " : " << chamberid << " / " << layerid << " / " << module_type << " / " << module_number << " / " << tube_in_module << " -> " << tube << endl;
  if (chamberid % 2 == 1) {
    totTubeID = tube;
  }
  if (chamberid % 2 == 0) {
    totTubeID = tube + strawCh12 * 8;
  }
  return totTubeID;
}

/////////////////////////////////////////////////////////////
/// this function will be used in PndOtHitProducesRealFast
////////////////////////////////////////////////////////////

PndOtTube *PndOtMapCreator::GetTubeFromNameToFill(TString tubename, Int_t, Int_t)
{ // tubeid layerid //[R.K.03/2017] unused variable(s)

  TObjArray *geoPassNodes = fOtParameters->GetGeoPassiveNodes();
  // Bool_t isCopy = kTRUE; //[R.K. 01/2017] unused variable
  // try as if it was a copy stt01tube#XXX

  FairGeoNode *pnode = (FairGeoNode *)geoPassNodes->FindObject(tubename);
  if (!pnode) { // try as if it was a solo stt01tubeXXX
    // isCopy = kFALSE; //[R.K. 01/2017] unused variable
    // tubename = GetNameFromTubeID(tubeid, isCopy);
    // pnode = (FairGeoNode*) geoPassNodes->FindObject(tubename);
  }

  if (!pnode) {
    cout << "PndOtMapCreator::GetTubeFromNameToFill: tube " << tubename << " not found (nor as a copy)" << endl;
    return nullptr;
  }

  FairGeoTransform *lab = pnode->getLabTransform();
  FairGeoVector tra = lab->getTransVector();
  FairGeoRotation rot = lab->getRotMatrix();

  // geometrical info
  double x = tra.getX() / 10.; // in cm
  double y = tra.getY() / 10.; // in cm
  double z = tra.getZ() / 10.; // in cm

  // std::cout<<"tubename="<<tubename<<" tubeid="<<tubeid<<" x="<<x<<" z="<<z<<std::endl;
  // Int_t skew=IsSkew(layerid); //[R.K. 01/2017] unused variable?
  // double angle=5*3.14159/180;
  // x=(x+skew*(tra.getY()*sin(angle))/10.1);
  // std::cout<<"tubename="<<tubename<<" tubeid="<<tubeid<<" skew="<<skew<<" newx="<<x<<std::endl;

  double r[3][3];
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      r[i][j] = rot.getElement(i, j);

  TGeoVolume *rootvol = pnode->getRootVolume();
  TGeoTube *tube = (TGeoTube *)rootvol->GetShape();
  Double_t halflength = tube->GetDz(); // in cm
  // sets up the correspondence int (tubeID) <--> int (1 = copy/0 = solo)
  //  copy_map[key] = alloc
  // fCopy_Map[tubeid] = isCopy;

  return new PndOtTube((float)x, (float)y, (float)z, r[0][0], r[0][1], r[0][2], r[1][0], r[1][1], r[1][2], r[2][0], r[2][1], r[2][2], fTubeInRad, fTubeOutRad, halflength);
}

TClonesArray *PndOtMapCreator::FillTubeArray()
{
  // ofstream myfile("testNumber.txt");
  TObjArray *geoPassNodes = fOtParameters->GetGeoPassiveNodes();
  TClonesArray *tubeArray = new TClonesArray("PndOtTube");
  //  tubeArray->Delete();
  // int mytest=0; //[R.K. 01/2017] unused variable?
  //  cout << "TA: entries: " <<  geoPassNodes->GetEntriesFast() << endl;
  for (int i = 0; i < geoPassNodes->GetEntriesFast(); i++) {
    FairGeoNode *pnode = (FairGeoNode *)geoPassNodes->At(i);
    if (!pnode) {
      cout << "PndOtMapCreator::FillTubeArray : tubename=" << pnode->GetName() << " not existing!!!" << endl;
      continue;
    }
    TString tubename = pnode->GetName();

    // cout<<"PndOtMapCreator::FillTubeArray : tubename="<<tubename<< endl;
    if ((!tubename.Contains("tube")) || (!tubename.Contains("ot"))) {
      // myfile<<"PndOtMapCreator::FillTubeArray : skipping tubename="<<tubename<<endl;
      continue;
    }

    tubename.ReplaceAll("#", "_");
    Int_t tempChamber = GetChamberIDFromName(tubename);
    Int_t tempLayer = GetLayerID(tempChamber, tubename);
    Int_t totTubeID = GetTubeIDTot(tempChamber, tempLayer, tubename);

    // cout<<"PndOtMapCreator::FillTubeArray : totTubeID="<<totTubeID<< endl;
    tubename = pnode->GetName();

    //    cout << "TUBE " << tempChamber << " / " << tubeID << " / " << tempLayer << " / " << totTubeID << endl;

    PndOtTube *ottube = GetTubeFromNameToFill(tubename, totTubeID, tempLayer);
    new ((*tubeArray)[totTubeID]) PndOtTube(*ottube);

    delete (ottube);
    // myfile <<  tubename << " " << totTubeID << " "<<tempLayer<<endl;
  }
  //  cout << "end: " << tubeArray->GetEntriesFast() << endl;
  return tubeArray;
}

Int_t PndOtMapCreator::GetChamberIDFromName(TString name)
{
  TString pathstring = name;
  if (pathstring.Contains("ot01")) {
    return 1;
  }
  if (pathstring.Contains("ot02")) {
    return 2;
  }
  LOG(fatal) << "[OT] GetChamberIDFromName failed (" << name.Data() << ")";
  return -1;
}

Int_t PndOtMapCreator::GetChamberIDFromPath(TString path)
{
  TString pathstring = path;
  if (pathstring.Contains("ot01tube")) {
    return 1;
  }
  if (pathstring.Contains("ot02tube")) {
    return 2;
  }
  LOG(fatal) << "[OT] GetChamberIDFromPath failed (" << path.Data() << ")";
  return -1;
}

ClassImp(PndOtMapCreator)
