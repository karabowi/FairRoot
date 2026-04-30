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

/////////////////////////////////////////////////////////////
// PndSttMapCreator
//
// Class to create the STT map:
// usage: add to your task Init function:
// PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
// fTubeArray = mapper->FillTubeArray();
// with fSttParameters = PndGeoSttPar from params.root file
//
// authors: Lia Lavezzi - INFN Pavia
//                        [changed to TGeoManager (2013)]
/////////////////////////////////////////////////////////////

#include "PndSttMapCreator.h"
#include "PndSttTubeParameters.h"
#include "PndSttTube.h"
#include "PndGeoSttPar.h"
#include "PndSttGeometryMap.h"
#include "PndGeoHandling.h"

#include "FairGeoNode.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairGeoRotation.h"
#include "FairGeoTube.h"
#include "FairRun.h"
#include "FairLogger.h"

#include "TGeoTube.h"
#include "TVector3.h"
#include "TObjArray.h"
#include "TString.h"
#include "TGeoVolume.h"
#include "TGeoTube.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TGeoVolume.h"
#include "TList.h"

#include <iostream>

using namespace std;

PndSttMapCreator::PndSttMapCreator() : fGeoType(-1), fSttParameters(new PndGeoSttPar()), fTubeInRad(0), fTubeOutRad(0), copy_map(), fSttTube(nullptr), fMap(nullptr)
{
  // copy_map.clear();
}

// to use in PndStt
PndSttMapCreator::PndSttMapCreator(Int_t geoType)
  : fGeoType(geoType), fSttParameters(new PndGeoSttPar()), fTubeInRad(0), fTubeOutRad(0), copy_map(), fSttTube(nullptr), fMap(nullptr)
{
  // copy_map.clear();

  if (fGeoType > 2)
    LOG(error) << " PndSttMapCreator: geometry not supported by map"; // CHECK

  if (!gGeoManager)
    LOG(error) << " PndSttMapCreator: no geo manager "; // CHECK
  // set general par
  SetGeneralParameters();
}

// crete geometry from parameters file
PndSttMapCreator::PndSttMapCreator(PndGeoSttPar *sttPar) : fGeoType(-1), fSttParameters(sttPar), fTubeInRad(0), fTubeOutRad(0), fSttTube(nullptr)
{

  if (!gGeoManager)
    LOG(error) << " PndSttMapCreator: no geo manager "; // CHECK

  // choose geometry type
  fGeoType = sttPar->GetGeometryType(); // classic, optimized, average, detailed, CAD

  if (fGeoType > 2)
    LOG(error) << " PndSttMapCreator: geometry not supported by map"; // CHECK
  // set general par
  SetGeneralParameters();
}

PndSttMapCreator::~PndSttMapCreator()
{
  if (fMap != nullptr)
    delete (fMap);
}

void PndSttMapCreator::SetGeneralParameters()
{ //  CHECK whether it depends on geometry or not
  if (fGeoType == 1) {
    fTubeInRad = 0.5;    // tube inner radius cm
    fTubeOutRad = 0.001; // tube outer radius cm CHECK why not: fTubeInRad + 0.001
  } else if (fGeoType == 2) {
    fTubeInRad = 0.5;
    fTubeOutRad = 0.001; // copy of values from fGeoType 1. I have no clue why fTubeOutRad is 0.001 but it is used nowhere
  } else {
    fTubeInRad = -1;
    fTubeOutRad = -1;
  }
}

// during simulation
Int_t PndSttMapCreator::GetTubeIDFromPath(TString path)
{
  if (fGeoType == 1)
    return GetTubeIDFromPathGeoType1(path);
  return -999;
}

// during parameters reading
Int_t PndSttMapCreator::GetTubeIDFromName(TString name)
{
  if (fGeoType == 1)
    return GetTubeIDFromNameGeoType1(name);
  return -999;
}

// retrieve parameters from tube ID
PndSttTube *PndSttMapCreator::GetTubeFromTubeID(Int_t tubeid)
{
  if (fGeoType == 1)
    return GetTubeFromTubeIDGeoType1(tubeid);
  return nullptr;
}

TString PndSttMapCreator::GetNameFromPath(TString path)
{
  if (fGeoType == 1)
    return GetNameFromPathGeoType1(path);
  return "";
}

TString PndSttMapCreator::GetPathFromTubeID(Int_t tubeid, Bool_t isCopy)
{
  if (fGeoType == 1)
    return GetPathFromTubeIDGeoType1(tubeid, isCopy);
  return "";
}

PndSttTube *PndSttMapCreator::GetTubeFromParametersToFill(PndSttTubeParameters *parms)
{
  if (fGeoType == 1)
    return GetTubeFromParametersToFillGeoType1(parms);
  return nullptr;
}

// fill the tube map at the beginning of the run
TClonesArray *PndSttMapCreator::FillTubeArray()
{
  if (fGeoType == 1)
    return FillTubeArrayGeoType1();
  else if (fGeoType == 2)
    return FillTubeArrayGeoType2();

  return nullptr;
}

// fill the tube map at the beginning of the run
Int_t PndSttMapCreator::FillSttTubeParameters(PndGeoSttPar *par, TList *volList)
{
  if (fGeoType == 1)
    return FillSttTubeParametersGeoType1(par, volList);
  return -1;
}

PndSttTubeParameters *PndSttMapCreator::CreateTubeParameters(FairGeoNode *pnode)
{
  if (fGeoType == 1)
    return CreateTubeParametersGeoType1(pnode);
  else if (fGeoType == 2)
    return CreateTubeParametersGeoType2(pnode);

  return nullptr;
}
// ------------------- simulation ----------------------------------------------------
// during simulation
Int_t PndSttMapCreator::GetTubeIDFromPathGeoType1(TString path)
{
  TString tmpstring = GetNameFromPathGeoType1(path);
  return GetTubeIDFromNameGeoType1(tmpstring);
}

// OK
// name as in geo file: from path "_" to name "#"
TString PndSttMapCreator::GetNameFromPathGeoType1(TString path)
{
  // two possibilities:
  // copy : /cave_1/stt01assembly_0/stt01tube_XXX/stt01gas_1     --> stt01tube#XXX
  // solo : /cave_1/stt01assembly_0/stt01tubeXXX_0/stt01gasXXX_0 --> stt01tubeXXX

  TString tmpstring = path;

  // cut /cave_1/stt01assembly_0/
  TString cavename;
  cavename = "/cave_1/stt01assembly_0/";
  tmpstring = tmpstring(cavename.Sizeof() - 1, tmpstring.Sizeof());

  // cut /stt01gas...
  int index = tmpstring.Index("/");
  tmpstring = tmpstring(0, index);

  // replace _ with #
  if (tmpstring.Contains("_0")) {
    tmpstring.Chop(); // cut "0"
    tmpstring.Chop(); // cut "_"
  } else {
    tmpstring.Replace(9, 1, "#");
  }

  return tmpstring;
}

// OK
Int_t PndSttMapCreator::GetTubeIDFromNameGeoType1(TString name)
{
  // two possibilities: we DON' T CARE and just take the XXX part
  // copy : stt01tube#XXX  --> XXX
  // solo : stt01tubeXXX --> XXX

  TString tmpstring = name;

  if (tmpstring.Contains("#")) {
    int start = tmpstring.Index("#") + 1;
    tmpstring = tmpstring(start, tmpstring.Sizeof());
  } else {
    int start = tmpstring.Index("e") + 1;
    tmpstring = tmpstring(start, tmpstring.Sizeof());
  }
  return tmpstring.Atoi();
}

Int_t PndSttMapCreator::FillSttTubeParametersGeoType1(PndGeoSttPar *par, TList *volList)
{
  fSttParameters = par;

  fSttParameters->SetGeometryType(fGeoType);
  fSttParameters->SetTubeInRad(fTubeInRad);
  fSttParameters->SetTubeOutRad(fTubeOutRad);

  // store geo parameter
  TObjArray *pararray = fSttParameters->GetTubeParameters();

  TListIter iter(volList);

  FairGeoNode *node = nullptr;
  // FairGeoVolume *aVol=nullptr; //[R.K. 01/2017] unused variable?
  int tubecounter = 0;
  PndSttTubeParameters *parms = new PndSttTubeParameters();
  pararray->AddLast(parms); // add this to have correspondence index <-> tubeid
  while ((node = (FairGeoNode *)iter.Next())) {
    if (node->isSensitive())
      continue;
    TString nodename = node->GetName();
    if (!nodename.Contains("stt01tube"))
      continue;
    tubecounter++;

    parms = CreateTubeParameters(node);
    pararray->AddLast(parms);
  }

  return tubecounter;
}

PndSttTubeParameters *PndSttMapCreator::CreateTubeParametersGeoType1(FairGeoNode *pnode)
{
  TString nodename = pnode->getName();
  Int_t tubeID = GetTubeIDFromNameGeoType1(nodename);

  FairGeoTransform *lab = pnode->getLabTransform();
  FairGeoVector tra = lab->getTransVector();
  FairGeoRotation rot = lab->getRotMatrix();
  TGeoVolume *rootvol = pnode->getRootVolume();
  TGeoTube *gtube = (TGeoTube *)rootvol->GetShape();
  Double_t halflength = gtube->GetDz(); // in cm

  PndSttTubeParameters *parms = new PndSttTubeParameters(tubeID, halflength);
  return parms;
}

// --------------------------------------------------------------------------------
TClonesArray *PndSttMapCreator::FillTubeArrayGeoType1()
{
  TObjArray *pararray = fSttParameters->GetTubeParameters();

  fTubeArray = new TClonesArray("PndSttTube");
  // fTubeArray->Delete();

  for (int i = 1; i < pararray->GetEntries(); i++) {
    PndSttTubeParameters *parms = (PndSttTubeParameters *)pararray->At(i);
    int tubeID = parms->GetTubeID();

    fSttTube = GetTubeFromParametersToFillGeoType1(parms);
    if (!fSttTube)
      continue;
    // correspondance position in TCA <-> tubeID
    new ((*fTubeArray)[tubeID]) PndSttTube(*fSttTube);

    delete (fSttTube);
  }

  fMap = new PndSttGeometryMap(fTubeArray, fGeoType);
  fMap->FillGeometryParameters();
  return fTubeArray;
}

PndSttTube *PndSttMapCreator::GetTubeFromParametersToFillGeoType1(PndSttTubeParameters *parms)
{

  Int_t tubeid = parms->GetTubeID();
  if (tubeid == -1) {
    cout << "PndSttMapCreator::GetTubeFromParametersToFillGeoType1 (tubeid): tube " << tubeid << " not found (nor as a copy)" << endl;
    return nullptr;
  }

  gGeoManager->cd("/cave_1/stt01assembly_0");
  TGeoNode *assembly_node = gGeoManager->GetCurrentNode();
  double local[3] = {0., 0., 0.}, master[3];
  assembly_node->LocalToMaster(local, master);
  TVector3 assembly_position(master[0], master[1], master[2]);

  Bool_t isCopy = kTRUE;

  // try as if it was a copy stt01tube#XXX
  TString path = GetPathFromTubeIDGeoType1(tubeid, isCopy);

  bool ispath = gGeoManager->CheckPath(path);
  if (ispath == kFALSE) { // try as if it was a solo stt01tubeXXX
    isCopy = kFALSE;
    path = GetPathFromTubeIDGeoType1(tubeid, isCopy);
    ispath = gGeoManager->CheckPath(path);
  }
  if (ispath == kFALSE) {
    cout << "PndSttMapCreator::GetTubeFromParametersToFillGeoType1 (ispath): tube " << tubeid << " not found (nor as a copy)" << endl;
    return nullptr;
  }

  gGeoManager->cd(path);
  TGeoNode *tube_node = gGeoManager->GetCurrentNode();
  tube_node->LocalToMaster(local, master);
  TVector3 tube_position(master[0], master[1], master[2]);
  tube_position += assembly_position;

  // tube_position.Print();

  // geometrical info
  double x = tube_position.X();
  double y = tube_position.Y();
  double z = tube_position.Z();

  TGeoMatrix *mat = tube_node->GetMatrix();
  Double_t const *rotation = mat->GetRotationMatrix();

  double r[3][3];
  int irot = 0, i = 0, j = 0;
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      r[i][j] = rotation[irot];
      irot++;
    }
  }

  // sets up the correspondence int (tubeID) <--> int (1 = copy/0 = solo)
  //  copy_map[key] = alloc
  copy_map[tubeid] = isCopy;

  return new PndSttTube(parms, (float)x, (float)y, (float)z, r[0][0], r[0][1], r[0][2], r[1][0], r[1][1], r[1][2], r[2][0], r[2][1], r[2][2], fSttParameters->GetTubeInRad(),
                        fSttParameters->GetTubeOutRad());
}

PndSttTube *PndSttMapCreator::GetTubeFromTubeIDGeoType1(Int_t tubeid)
{

  Bool_t isCopy = copy_map[tubeid];

  gGeoManager->cd("/cave_1/stt01assembly_0");
  TGeoNode *assembly_node = gGeoManager->GetCurrentNode();
  double local[3] = {0., 0., 0.}, master[3];
  assembly_node->LocalToMaster(local, master);
  TVector3 assembly_position(master[0], master[1], master[2]);

  // try as if it was a copy stt01tube#XXX
  TString path = GetPathFromTubeIDGeoType1(tubeid, isCopy);

  bool ispath = gGeoManager->CheckPath(path);

  if (ispath == kFALSE) {
    cout << "PndSttMapCreator::GetTubeFromTubeIDGeoType1 (ispath): tube " << tubeid << " not found (nor as a copy)" << endl;
    return nullptr;
  }

  gGeoManager->cd(path);
  TGeoNode *tube_node = gGeoManager->GetCurrentNode();
  tube_node->LocalToMaster(local, master);
  TVector3 tube_position(master[0], master[1], master[2]);
  tube_position += assembly_position;

  // tube_position.Print();

  // geometrical info
  double x = tube_position.X();
  double y = tube_position.Y();
  double z = tube_position.Z();

  TGeoMatrix *mat = tube_node->GetMatrix();
  Double_t const *rotation = mat->GetRotationMatrix();

  double r[3][3];
  int irot = 0, i = 0, j = 0;
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      r[i][j] = rotation[irot];
      irot++;
    }
  }

  TObjArray *parmsarray = fSttParameters->GetTubeParameters();
  PndSttTubeParameters *parms = (PndSttTubeParameters *)parmsarray->At(tubeid); // CHECK

  return new PndSttTube(parms, (float)x, (float)y, (float)z, r[0][0], r[0][1], r[0][2], r[1][0], r[1][1], r[1][2], r[2][0], r[2][1], r[2][2], fSttParameters->GetTubeInRad(),
                        fSttParameters->GetTubeOutRad());
}

// name as in geo file: from path "_" to name "#"
TString PndSttMapCreator::GetPathFromTubeIDGeoType1(Int_t tubeid, Bool_t isCopy)
{

  // two possibilities:
  // stt01tube#XXX  --> copy : /cave_1/stt01assembly_0/stt01tube_XXX/stt01gas_1     --> stt01tube#XXX
  // stt01tubeXXX   --> solo : /cave_1/stt01assembly_0/stt01tubeXXX_0/stt01gasXXX_0 --> stt01tubeXXX

  TString tmpstring;
  tmpstring += tubeid;

  if (isCopy == kTRUE)
    tmpstring.Prepend("_");
  else
    tmpstring.Append("_0");
  tmpstring.Prepend("stt01tube");

  TString cavename;
  cavename = "/cave_1/stt01assembly_0/";
  tmpstring.Prepend(cavename);

  return tmpstring;
}

PndSttTubeParameters *PndSttMapCreator::CreateTubeParametersGeoType2(FairGeoNode *pnode)
{
  TString nodename = pnode->getName();
  Int_t tubeID = PndGeoHandling::Instance()->GetShortID(nodename);

  FairGeoTransform *lab = pnode->getLabTransform();
  FairGeoVector tra = lab->getTransVector();
  FairGeoRotation rot = lab->getRotMatrix();
  TGeoVolume *rootvol = pnode->getRootVolume();
  TGeoTube *gtube = (TGeoTube *)rootvol->GetShape();
  Double_t halflength = gtube->GetDz(); // in cm

  PndSttTubeParameters *parms = new PndSttTubeParameters(tubeID, halflength);
  return parms;
}

Int_t PndSttMapCreator::GetTubeIDFromNameGeoType2(TString name) {}

Int_t PndSttMapCreator::FillSttTubeParametersType2(PndGeoSttPar *par)
{

  fSttParameters = par;

  fSttParameters->SetGeometryType(fGeoType);
  fSttParameters->SetTubeInRad(fTubeInRad);
  fSttParameters->SetTubeOutRad(fTubeOutRad);
  //
  //
  //  // store geo parameter
  TObjArray *sensorNames = PndGeoHandling::Instance()->GetSensorNames();
  TIter iter = sensorNames->MakeIterator();

  TObjArray *pararray = fSttParameters->GetTubeParameters();
  //  std::cout << "SensorNames.GetEntries() " << sensorNames->GetEntries() << std::endl;
  int tubecounter = 0;
  TObjString *sensorName;
  while ((sensorName = (TObjString *)iter.Next())) {
    TString stringName = sensorName->GetString();
    if (stringName.Contains("ArCO2Sensitive")) {
      gGeoManager->cd(stringName.Data());
      TGeoNode *node = gGeoManager->GetCurrentNode();
      TGeoTube *tube = (TGeoTube *)node->GetVolume()->GetShape();
      PndSttTubeParameters *parms = new PndSttTubeParameters(PndGeoHandling::Instance()->GetShortID(stringName), tube->GetDZ());
      //          std::cout << "-I- PndSttMapCreator::FillSttTubeParameeresType2 " << stringName.Data() << " ID: " << PndGeoHandling::Instance()->GetShortID(stringName) << " z/2 "
      //          << tube->GetDZ() << std::endl;
      pararray->AddLast(parms);
      tubecounter++;
    }
  }

  return tubecounter;
}

TClonesArray *PndSttMapCreator::FillTubeArrayGeoType2()
{
  TObjArray *pararray = fSttParameters->GetTubeParameters();

  fTubeArray = new TClonesArray("PndSttTube");
  // fTubeArray->Delete();

  for (int i = 1; i < pararray->GetEntries(); i++) {
    PndSttTubeParameters *parms = (PndSttTubeParameters *)pararray->At(i);
    int tubeID = parms->GetTubeID();

    fSttTube = GetTubeFromParametersToFillGeoType2(parms);
    if (!fSttTube)
      continue;
    // correspondance position in TCA <-> tubeID
    new ((*fTubeArray)[tubeID]) PndSttTube(*fSttTube);

    delete (fSttTube);
  }

  fMap = new PndSttGeometryMap(fTubeArray, fGeoType);
  fMap->FillGeometryParameters();
  return fTubeArray;
}

PndSttTube *PndSttMapCreator::GetTubeFromParametersToFillGeoType2(PndSttTubeParameters *parms)
{

  Int_t tubeid = parms->GetTubeID();
  if (tubeid == -1) {
    cout << "PndSttMapCreator::GetTubeFromParametersToFillGeoType2 (tubeid): tube " << tubeid << " not found (nor as a copy)" << endl;
    return nullptr;
  }
  TVector3 local(0., 0., 0.);
  TVector3 localToMaster = PndGeoHandling::Instance()->LocalToMasterShortId(local, tubeid);
  TGeoHMatrix *mat = PndGeoHandling::Instance()->GetMatrixShortId(tubeid);
  Double_t const *rotation = mat->GetRotationMatrix();

  double r[3][3];
  int irot = 0, i = 0, j = 0;
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      r[i][j] = rotation[irot];
      irot++;
    }
  }

  return new PndSttTube(parms, localToMaster.X(), localToMaster.Y(), localToMaster.Z(), r[0][0], r[0][1], r[0][2], r[1][0], r[1][1], r[1][2], r[2][0], r[2][1], r[2][2],
                        fSttParameters->GetTubeInRad(), fSttParameters->GetTubeOutRad());
}

ClassImp(PndSttMapCreator);
