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

//======================================================================
// Description:
//       Class PndEmcStructure
//
//======================================================================

//-----------------------
// This Class's Header --
//-----------------------
#include "PndEmcStructure.h"
#include "PndEmcMapper.h"
#include "PndEmcTwoCoordIndex.h"
#include "PndEmcXtal.h"

#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TGeoArb8.h"
#include "TVector3.h"
#include "TRotation.h"
#include "TMath.h"
#include "TPRegexp.h"
#include "TRegexp.h"
#include "TString.h"
#include "TObjString.h"

//---------------
// C++ Headers --
//---------------
#include "stdlib.h"
#include <iostream>
#include <fstream>
//#include <string>
#include <cassert>
#include "math.h"
using namespace std;

using std::cout;
using std::endl;

template <typename T>
int getsign(const T &a)
{
  return (a > 0 ? 1 : a < 0 ? -1 : 0);
}

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
PndEmcStructure *PndEmcStructure::_instance = nullptr;

PndEmcStructure::~PndEmcStructure() {}

PndEmcStructure *PndEmcStructure::Instance(TGeoManager *geoMan)
{
  if (_instance == nullptr) {
    if (geoMan == nullptr) {
      cout << "Empty geometry passed to PndEmcStructure" << endl;
      abort();
    } else
      _instance = new PndEmcStructure(geoMan);
  }
  return _instance;
}

// If PndEmcStructure is called without input parameters
// it means that it should have been instantiated with TGeoManager before or use gGeoManager

PndEmcStructure *PndEmcStructure::Instance()
{
  if (_instance == nullptr) {
    TGeoManager *geoMan = gGeoManager;
    if (geoMan == nullptr) {
      cout << "gGeoManager does not exist" << endl;
      abort();
    } else
      _instance = new PndEmcStructure(geoMan);
  }
  return _instance;
}

PndEmcStructure::PndEmcStructure(TGeoManager *geoMan) : emcX(), emcY(), emcZ(), fTciXtalMap()
{

  // Instantiate mapper to convert from detId to TCI
  // PndEmcMapper should be initiated before
  PndEmcMapper *fEmcMap = PndEmcMapper::Instance();

  int module, copy, row, crystal;
  TString node_path;
  const char *crystal_name;
  const TGeoMatrix *crystal_matrix;
  const double *trans;
  TGeoRotation geoRot;
  TRotation rot;
  Double_t phi, theta, psi;
  int detId;

  TGeoIterator next(geoMan->GetTopVolume());
  TGeoNode *node;
  while ((node = next())) {
    next.GetPath(node_path);

    bool isEmcModule = crystal_name_analysis(node_path, module, copy, row, crystal);
    if (!isEmcModule)
      continue;

    detId = module * 100000000 + row * 1000000 + copy * 10000 + crystal;

    PndEmcTwoCoordIndex *tci = fEmcMap->GetTCI(detId);

    if (tci == 0) {
      cout << "Not found tci for index = " << detId << " in PndEmcStructure" << endl;
      abort();
    }

    // Obtaine TGeoMatrix matrix for the current node, later it is factorised to translation and rotation
    crystal_matrix = next.GetCurrentMatrix();

    trans = crystal_matrix->GetTranslation();
    TVector3 pos(trans[0], trans[1], trans[2]);
    emcX[detId] = pos.X();
    emcY[detId] = pos.Y();
    emcZ[detId] = pos.Z();

    //		cout<<"emcX["<<detId<<"]="<< emcX[detId] <<endl;
    //		cout<<"emcY["<<detId<<"]="<< emcY[detId] <<endl;
    //		cout<<"emcZ["<<detId<<"]="<< emcZ[detId] <<endl;
    geoRot.SetMatrix(crystal_matrix->GetRotationMatrix());

    PndEmcXtal *xtal;

    // Check the geometry type and convert to TGeoTrap.

    TString shapeType = node->GetVolume()->GetShape()->ClassName();
    if (shapeType == "TGeoTrap") {
      TGeoTrap *trap = dynamic_cast<TGeoTrap *>(node->GetVolume()->GetShape());

      xtal = new PndEmcXtal(tci, *trap, pos, geoRot);
    } else if (shapeType == "TGeoArb8") {
      TGeoArb8 *arb8 = dynamic_cast<TGeoArb8 *>(node->GetVolume()->GetShape());

      // Approximate the shape with a TGeoTrap.

      Double_t *verts = arb8->GetVertices();

      Double_t dz = arb8->GetDz();
      Double_t tx =
        (verts[4 * 2 + 0] + verts[5 * 2 + 0] + verts[6 * 2 + 0] + verts[7 * 2 + 0] - verts[0 * 2 + 0] - verts[1 * 2 + 0] - verts[2 * 2 + 0] - verts[3 * 2 + 0]) / (2 * 4 * dz);
      Double_t ty =
        (verts[4 * 2 + 1] + verts[5 * 2 + 1] + verts[6 * 2 + 1] + verts[7 * 2 + 1] - verts[0 * 2 + 1] - verts[1 * 2 + 1] - verts[2 * 2 + 1] - verts[3 * 2 + 1]) / (2 * 4 * dz);

      Double_t thetac = TMath::ATan(TMath::Sqrt(tx * tx + ty * ty)) * TMath::RadToDeg();
      Double_t phic = TMath::ATan2(ty, tx) * TMath::RadToDeg();

      Double_t h1 = (verts[1 * 2 + 1] + verts[2 * 2 + 1] - verts[0 * 2 + 1] - verts[3 * 2 + 1]) / (2 * 2);
      Double_t bl1 = (verts[3 * 2 + 0] - verts[0 * 2 + 0]) / 2;
      Double_t tl1 = (verts[2 * 2 + 0] - verts[1 * 2 + 0]) / 2;
      Double_t alpha1 = TMath::ATan((verts[1 * 2 + 0] + verts[2 * 2 + 0] - verts[0 * 2 + 0] - verts[3 * 2 + 0]) / (2 * 2 * h1)) * TMath::RadToDeg();

      Double_t h2 = (verts[5 * 2 + 1] + verts[6 * 2 + 1] - verts[4 * 2 + 1] - verts[7 * 2 + 1]) / (2 * 2);
      Double_t bl2 = (verts[7 * 2 + 0] - verts[4 * 2 + 0]) / 2;
      Double_t tl2 = (verts[6 * 2 + 0] - verts[5 * 2 + 0]) / 2;
      Double_t alpha2 = TMath::ATan((verts[5 * 2 + 0] + verts[6 * 2 + 0] - verts[4 * 2 + 0] - verts[7 * 2 + 0]) / (2 * 2 * h2)) * TMath::RadToDeg();

      TGeoTrap *crystal_shape = new TGeoTrap(dz, thetac, phic, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2);

      // Check the conversion.
      // Double_t *verts2 = crystal_shape.GetVertices();
      // for (size_t i = 0; i < 8; ++i) {
      //    if (TMath::Abs(verts[i*2+0] - verts2[i*2+0]) > 1e-8
      //        || TMath::Abs(verts[i*2+1] - verts2[i*2+1]) > 1e-8) {
      //        cout << "TGeoArb8 conversion bad in module " << module << "  " << "   x " << verts[i*2+0] << "->" << verts2[i*2+0] << "     y "  << verts[i*2+1] << "->" <<
      //        verts2[i*2+1] << endl;
      //    }
      //}

      xtal = new PndEmcXtal(tci, *crystal_shape, pos, geoRot);
    } else if (shapeType == "TGeoBBox" || shapeType == "TGeoScaledShape") {
      TGeoBBox const *box = dynamic_cast<TGeoBBox const *>(node->GetVolume()->GetShape());

      // Convert to TGeoTrap.
      TGeoTrap *crystal_shape = new TGeoTrap(box->GetDZ(), 0, 0, box->GetDY(), box->GetDX(), box->GetDX(), 0, box->GetDY(), box->GetDX(), box->GetDX(), 0);

      xtal = new PndEmcXtal(tci, *crystal_shape, pos, geoRot);
    } else {
      cout << "Unknown geometry type " << shapeType << " in module " << module << endl;
      abort();
    }

    fTciXtalMap[tci] = xtal;
  }
}

bool PndEmcStructure::crystal_name_analysis(TString node_path, int &module, int &copy, int &row, int &crystal)
{
  // The following code extracts information about module, copy, row and crystal number from the path name of the TGeoNode which corresponds to emc crystal
  // Name convention is according to PndEmc.cxx

  if (!(node_path.Contains("emc") || node_path.Contains("Crystal") || node_path.Contains("FscModuleVolume")))
    return false;

  ///////////////////////////////////////////////////////////////////////////
  // Case of old Fsc
  ///////////////////////////////////////////////////////////////////////////
  if (node_path.Contains("Fsc_")) {
    // at the moment all the layers of module in Fsc are not taken into account, only the whole block is selected
    if (node_path.Contains("FscLayer"))
      return false;
    sscanf(node_path.Data(), "cave/Fsc_%d/emc%dr%dc%d_0", &copy, &module, &row, &crystal);
    return true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Case of new Fsc
  ///////////////////////////////////////////////////////////////////////////
  if (node_path.Contains("FscModuleVolume")) {
    // at the moment all the layers of module in Fsc are not taken into account, only the whole block is selected
    //		if (node_path.Contains("FscLayer") || node_path.Contains("FscTyvek") || node_path.Contains("FscFibHole"))
    if (node_path.Contains("FscLayer") || node_path.Contains("FscFibHole"))
      return false;

    //		cout << "node_path= " << node_path << endl;
    int SupModCopy = 0;
    int LocCopy = 0;
    int dummyTyv = 0;
    Int_t nSupCol = 0;
    Int_t nSupRow = 0;
    Int_t nModCol = 0;
    Int_t nModRow = 0;

    //		sscanf(node_path.Data(),"cave/Emc%d_%d/FscModuleVolume_%d",&module,&copy,&ModCopy);
    sscanf(node_path.Data(), "cave/Emc%d_%d/FscSuperModuleVolume_%d/FscTyvekVolume_%d/FscModuleVolume_%d", &module, &copy, &SupModCopy, &dummyTyv, &LocCopy);
    copy += 1;

    nSupCol = SupModCopy / 100;
    nSupRow = SupModCopy % 100;

    nModCol = LocCopy % 2;
    nModRow = LocCopy / 2;

    crystal = (nSupCol - 1) * 2 + nModCol + 1;
    row = (nSupRow - 1) * 2 + nModRow + 1;

    return true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Case of barrel section with hole
  ///////////////////////////////////////////////////////////////////////////
  if (node_path.Contains("Emc12Hole")) {
    int tmp1, tmp2;
    if (node_path.Contains("EmcLayer2Hole")) {
      sscanf(node_path.Data(), "cave/Emc12Hole_%d/EmcLayer2Hole_0/emc%dr%dc%d_0$", &copy, &module, &row, &crystal);
    } else {
      sscanf(node_path.Data(), "cave/Emc12Hole_%d/EmcLayer1_0/emc%dr%dc%d_0$", &copy, &module, &row, &crystal);
    }
    return true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Case of barrel
  ///////////////////////////////////////////////////////////////////////////
  if (node_path.Contains("EmcLayer")) {
    int tmp1, tmp2;
    sscanf(node_path.Data(), "cave/Emc%d_%d/EmcLayer%d_0/emc%dr%dc%d_0$", &tmp1, &copy, &tmp2, &module, &row, &crystal);
  }

  ///////////////////////////////////////////////////////////////////////////
  // Case of new version of barrel section with hole
  ///////////////////////////////////////////////////////////////////////////
  if (node_path.Contains("Slice_target")) {
    int iSM, copySM, iMod, copyMod, iAlv, iCry, copyCry;
    char sgnMod, sgnCry, typeCry;
    sscanf(node_path.Data(), "cave/BarrelEMC_0/Slice_target_%d/SuperModule%d_Target_%d/Module%d%c_%d/Crystal-%d%c-%c%d_%d", &copy, &iSM, &copySM, &iMod, &sgnMod, &copyMod, &iAlv,
           &sgnCry, &typeCry, &iCry, &copyCry);

    // nMod
    if (sgnCry == 'p')
      module = 1;
    else if (sgnCry == 'm')
      module = 2;
    else
      return false;

    // nRow
    row = (iAlv - 1) * 4 + iCry;

    // nCrys
    if (module == 1) {
      if (typeCry == 'L')
        crystal = (copyCry - 1) * 2 + 1;
      if (typeCry == 'R')
        crystal = (copyCry - 1) * 2 + 2;
    } else if (module == 2) {
      if (typeCry == 'R')
        crystal = (copyCry - 1) * 2 + 1;
      if (typeCry == 'L')
        crystal = (copyCry - 1) * 2 + 2;
    } else {
      std::cout << "Error!!!" << std::endl;
      return false;
    }
    return true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Case of new version of barrel
  ///////////////////////////////////////////////////////////////////////////
  if (node_path.Contains("Slice")) {
    int iSM, copySM, iMod, copyMod, iAlv, iCry, copyCry;
    char sgnMod, sgnCry, typeCry;
    sscanf(node_path.Data(), "cave/BarrelEMC_0/Slice_%d/SuperModule%d_%d/Module%d%c_%d/Crystal-%d%c-%c%d_%d", &copy, &iSM, &copySM, &iMod, &sgnMod, &copyMod, &iAlv, &sgnCry,
           &typeCry, &iCry, &copyCry);

    // nMod
    if (sgnCry == 'p')
      module = 1;
    else if (sgnCry == 'm')
      module = 2;
    else
      return false;

    // nRow
    row = (iAlv - 1) * 4 + iCry;

    // nCrys
    if (module == 1) {
      if (typeCry == 'L')
        crystal = (copyCry - 1) * 2 + 1;
      if (typeCry == 'R')
        crystal = (copyCry - 1) * 2 + 2;
    } else if (module == 2) {
      if (typeCry == 'R')
        crystal = (copyCry - 1) * 2 + 1;
      if (typeCry == 'L')
        crystal = (copyCry - 1) * 2 + 2;
    } else {
      std::cout << "Error!!!" << std::endl;
      return false;
    }
    return true;
  }
  ///////////////////////////////////////////////////////////////////////////
  // case of test calorimeter
  ///////////////////////////////////////////////////////////////////////////
  else if (node_path.Contains("EmcTest")) {
    sscanf(node_path.Data(), "cave/EmcTest_%d/emc%dr%dc%d_0$", &copy, &module, &row, &crystal);
  }

  ///////////////////////////////////////////////////////////////////////////
  // case new version of forward end-cap (from "emc_module3_2011_new.root" file)
  ///////////////////////////////////////////////////////////////////////////
  else if (node_path.Contains("SubunitVolFwEndCap")) {
    Int_t copyNoSub, copyNoBox, copyNoCrys;

    if (node_path.Contains("HalfSubunitVolFwEndCap")) // reading the HalfSubunits of FwEndCap
      sscanf(node_path.Data(), "cave/Emc3_0/HalfSubunitVolFwEndCap_%d/BoxVol_%d/CrystalVol_%d", &copyNoSub, &copyNoBox, &copyNoCrys);
    else // reading the Subunits of FwEndCap
      sscanf(node_path.Data(), "cave/Emc3_0/SubunitVolFwEndCap_%d/BoxVol_%d/CrystalVol_%d", &copyNoSub, &copyNoBox, &copyNoCrys);

    // cout << "copyNoSub = "<< copyNoSub<<" ,copyNoBox = "<< copyNoBox<<" ,copyNoCrys = "<< copyNoCrys<<"\n";

    // some presetting of parameters:
    Int_t SubunitCol = -100, SubunitRow = -100, CrystalCol = -100, CrystalRow = -100;
    Int_t RestOfHalfSubunitRowNo[6] = {5, 6, 7, 8, 9, 9}; // row number of 6 peripheral half Subunits
    Int_t RestOfHalfSubunitColNo[6] = {8, 7, 6, 5, 3, 2}; // column number of 6 peripheral half Subunits
    // determination of SubunitRow and SubunitCol:
    if (copyNoSub >= 1 && copyNoSub <= 10) { // the middle row of 10 HalfSubunits
      SubunitRow = 0;
      SubunitCol = pow(-1., 1 + (copyNoSub - 1) / 5) * (4 + (copyNoSub - 1) % 5);
    } else if (copyNoSub > 11 && copyNoSub < 24) { // the upper and lower pipe-region row of 10 HalfSubunits
      SubunitRow = pow(-1., (copyNoSub - 11) / 7) * 2;
      SubunitCol = ((copyNoSub - 11) % 7) - 3;
    } else if ((copyNoSub >= 25 && copyNoSub <= 27) || copyNoSub == 57 || copyNoSub == 58) { // the outermost left column of 5 HalfSubunits
      if (copyNoSub >= 25 && copyNoSub <= 27)
        SubunitRow = copyNoSub - 25;
      else
        SubunitRow = copyNoSub - 59;
      SubunitCol = -9;
    } else if (copyNoSub >= 40 && copyNoSub <= 44) { // the outermost right column of 5 HalfSubunits
      SubunitRow = 42 - copyNoSub;
      SubunitCol = 9;
    } else if (copyNoSub == 28 || copyNoSub == 39 || copyNoSub == 45 || copyNoSub == 56) { // 4 single peripheral half Subunits
      if (copyNoSub <= 39)
        SubunitRow = RestOfHalfSubunitRowNo[0];
      else
        SubunitRow = -RestOfHalfSubunitRowNo[0];
      SubunitCol = pow(-1., copyNoSub - 1) * RestOfHalfSubunitColNo[0];
    } else if (copyNoSub == 29 || copyNoSub == 38 || copyNoSub == 46 || copyNoSub == 55) { // 4 single peripheral half Subunits
      if (copyNoSub <= 38)
        SubunitRow = RestOfHalfSubunitRowNo[1];
      else
        SubunitRow = -RestOfHalfSubunitRowNo[1];
      SubunitCol = pow(-1., copyNoSub) * RestOfHalfSubunitColNo[1];
    } else if (copyNoSub == 30 || copyNoSub == 37 || copyNoSub == 47 || copyNoSub == 54) { // 4 single peripheral half Subunits
      if (copyNoSub <= 37)
        SubunitRow = RestOfHalfSubunitRowNo[2];
      else
        SubunitRow = -RestOfHalfSubunitRowNo[2];
      SubunitCol = pow(-1., copyNoSub - 1) * RestOfHalfSubunitColNo[2];
    } else if (copyNoSub == 31 || copyNoSub == 36 || copyNoSub == 48 || copyNoSub == 53) { // 4 single peripheral half Subunits
      if (copyNoSub <= 36)
        SubunitRow = RestOfHalfSubunitRowNo[3];
      else
        SubunitRow = -RestOfHalfSubunitRowNo[3];
      SubunitCol = pow(-1., copyNoSub) * RestOfHalfSubunitColNo[3];
    } else if (copyNoSub == 32 || copyNoSub == 35 || copyNoSub == 49 || copyNoSub == 52) { // 4 single peripheral half Subunits
      if (copyNoSub <= 35)
        SubunitRow = RestOfHalfSubunitRowNo[4];
      else
        SubunitRow = -RestOfHalfSubunitRowNo[4];
      SubunitCol = pow(-1., copyNoSub - 1) * RestOfHalfSubunitColNo[4];
    } else if (copyNoSub == 33 || copyNoSub == 34 || copyNoSub == 50 || copyNoSub == 51) { // 4 single peripheral half Subunits
      if (copyNoSub <= 34)
        SubunitRow = RestOfHalfSubunitRowNo[5];
      else
        SubunitRow = -RestOfHalfSubunitRowNo[5];
      SubunitCol = pow(-1., copyNoSub) * RestOfHalfSubunitColNo[5];
    } else if (copyNoSub >= 61 && copyNoSub <= 74) { // the middle column of full Subunits
      SubunitRow = pow(-1., (copyNoSub - 61) / 7) * ((copyNoSub - 61) % 7 + 3);
      SubunitCol = 0;
    } else if (copyNoSub % 100 >= 1 && copyNoSub % 100 <= 5) { // rest of the full Subunits
      if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
        SubunitRow = 1;
        SubunitCol = pow(-1., 1 + copyNoSub / 100) * (3 + copyNoSub % 100);
      } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
        SubunitRow = -1;
        SubunitCol = pow(-1., copyNoSub / 100) * (3 + copyNoSub % 100);
      }
    } else if (copyNoSub % 100 >= 6 && copyNoSub % 100 <= 11) { // rest of the full Subunits
      if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
        SubunitRow = 2;
        SubunitCol = pow(-1., 1 + copyNoSub / 100) * (2 + copyNoSub % 100 - 5);
      } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
        SubunitRow = -2;
        SubunitCol = pow(-1., copyNoSub / 100) * (2 + copyNoSub % 100 - 5);
      }
    } else if (copyNoSub % 100 >= 12 && copyNoSub % 100 <= 19) { // rest of the full Subunits
      if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
        SubunitRow = 3;
        SubunitCol = pow(-1., 1 + copyNoSub / 100) * (copyNoSub % 100 - 11);
      } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
        SubunitRow = -3;
        SubunitCol = pow(-1., copyNoSub / 100) * (copyNoSub % 100 - 11);
      }
    } else if (copyNoSub % 100 >= 20 && copyNoSub % 100 <= 27) { // rest of the full Subunits
      if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
        SubunitRow = 4;
        SubunitCol = pow(-1., 1 + copyNoSub / 100) * (copyNoSub % 100 - 19);
      } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
        SubunitRow = -4;
        SubunitCol = pow(-1., copyNoSub / 100) * (copyNoSub % 100 - 19);
      }
    } else if (copyNoSub % 100 >= 28 && copyNoSub % 100 <= 34) { // rest of the full Subunits
      if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
        SubunitRow = 5;
        SubunitCol = pow(-1., 1 + copyNoSub / 100) * (copyNoSub % 100 - 27);
      } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
        SubunitRow = -5;
        SubunitCol = pow(-1., copyNoSub / 100) * (copyNoSub % 100 - 27);
      }
    } else if (copyNoSub % 100 >= 35 && copyNoSub % 100 <= 40) { // rest of the full Subunits
      if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
        SubunitRow = 6;
        SubunitCol = pow(-1., 1 + copyNoSub / 100) * (copyNoSub % 100 - 34);
      } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
        SubunitRow = -6;
        SubunitCol = pow(-1., copyNoSub / 100) * (copyNoSub % 100 - 34);
      }
    } else if (copyNoSub % 100 >= 41 && copyNoSub % 100 <= 45) { // rest of the full Subunits
      if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
        SubunitRow = 7;
        SubunitCol = pow(-1., 1 + copyNoSub / 100) * (copyNoSub % 100 - 40);
      } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
        SubunitRow = -7;
        SubunitCol = pow(-1., copyNoSub / 100) * (copyNoSub % 100 - 40);
      }
    } else if (copyNoSub % 100 >= 46 && copyNoSub % 100 <= 49) { // rest of the full Subunits
      if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
        SubunitRow = 8;
        SubunitCol = pow(-1., 1 + copyNoSub / 100) * (copyNoSub % 100 - 45);
      } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
        SubunitRow = -8;
        SubunitCol = pow(-1., copyNoSub / 100) * (copyNoSub % 100 - 45);
      }
    } else if (copyNoSub % 100 == 50) { // rest of the full Subunits
      if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
        SubunitRow = 9;
        SubunitCol = pow(-1., 1 + copyNoSub / 100);
      } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
        SubunitRow = -9;
        SubunitCol = pow(-1., copyNoSub / 100);
      }
    }

    Int_t flag = 1; // this flag introducing is not necessary in this class but is also harmless
    if (fabs(SubunitRow) <= 1 && fabs(SubunitCol) <= 3)
      flag = 0; // empty copyNoSub in the beam-pipe area
    else if (fabs(SubunitCol) == 9 && fabs(SubunitRow) >= 3)
      flag = 0; // empty copyNoSub in the residual area
    else if (fabs(SubunitCol) == 8 && fabs(SubunitRow) >= 6)
      flag = 0; //  -||-
    else if (fabs(SubunitCol) == 7 && fabs(SubunitRow) >= 7)
      flag = 0; //  -||-
    else if (fabs(SubunitCol) == 6 && fabs(SubunitRow) >= 8)
      flag = 0; //  -||-
    else if (fabs(SubunitCol) >= 4 && fabs(SubunitRow) == 9)
      flag = 0; //  -||-

    // determination of CrystalRow and CrystalCol:
    if (flag && copyNoSub >= 61) { // determination of CrystalRow and CrystalCol for the 214 full Subunits
      if (copyNoBox == 1 || copyNoBox == 4) {
        if (copyNoCrys == 1 || copyNoCrys == 3)
          CrystalRow = SubunitRow * 4 + (3 - getsign(SubunitRow)) / 2;
        else // here necessarily: copyNoCrys==2 || copyNoCrys==4
          CrystalRow = SubunitRow * 4 + (1 - getsign(SubunitRow)) / 2;
      } else if (copyNoBox == 2 || copyNoBox == 3) {
        if (copyNoCrys == 1 || copyNoCrys == 3)
          CrystalRow = SubunitRow * 4 - (1 + getsign(SubunitRow)) / 2;
        else // here necessarily: copyNoCrys==2 || copyNoCrys==4
          CrystalRow = SubunitRow * 4 - (3 + getsign(SubunitRow)) / 2;
      }

      if (copyNoSub >= 61 && copyNoSub <= 74) { // determination of CrystalCol for the 14 middle column full Subunits
        if (copyNoBox == 1 || copyNoBox == 3) {
          if (copyNoCrys == 1 || copyNoCrys == 4)
            CrystalCol = 2;
          else // here necessarily: copyNoCrys==3 || copyNoCrys==2
            CrystalCol = 1;
        } else if (copyNoBox == 4 || copyNoBox == 2) {
          if (copyNoCrys == 1 || copyNoCrys == 4)
            CrystalCol = -1;
          else // here necessarily: copyNoCrys==3 || copyNoCrys==2
            CrystalCol = -2;
        }
      } else { // determination of CrystalCol for the other 4*50=200 full Subunits
        if (copyNoBox == 1 || copyNoBox == 3) {
          if (copyNoCrys == 1 || copyNoCrys == 4)
            CrystalCol = SubunitCol * 4 + (3 + getsign(SubunitCol)) / 2;
          else // here necessarily: copyNoCrys==3 || copyNoCrys==2
            CrystalCol = SubunitCol * 4 + (1 + getsign(SubunitCol)) / 2;
        } else if (copyNoBox == 4 || copyNoBox == 2) {
          if (copyNoCrys == 1 || copyNoCrys == 4)
            CrystalCol = SubunitCol * 4 - (1 - getsign(SubunitCol)) / 2;
          else // here necessarily: copyNoCrys==3 || copyNoCrys==2
            CrystalCol = SubunitCol * 4 - (3 - getsign(SubunitCol)) / 2;
        }
      }
    }

    else if (flag && copyNoSub <= 10) { // determination of CrystalRow and CrystalCol for the 10 middle-row half Subunits
      if (copyNoCrys == 1 || copyNoCrys == 3)
        CrystalRow = 1;
      else // here necessarily: copyNoCrys==2 || copyNoCrys==4
        CrystalRow = -1;

      if (copyNoBox == 1) {
        if (copyNoCrys == 1 || copyNoCrys == 4)
          CrystalCol = SubunitCol * 4 + (3 + getsign(SubunitCol)) / 2;
        else // here necessarily: copyNoCrys==3 || copyNoCrys==2
          CrystalCol = SubunitCol * 4 + (1 + getsign(SubunitCol)) / 2;
      } else { // here necessarily: copyNoBox==2
        if (copyNoCrys == 1 || copyNoCrys == 4)
          CrystalCol = SubunitCol * 4 - (1 - getsign(SubunitCol)) / 2;
        else // here necessarily: copyNoCrys==3 || copyNoCrys==2
          CrystalCol = SubunitCol * 4 - (3 - getsign(SubunitCol)) / 2;
      }
    } else if (flag && copyNoSub > 11 && copyNoSub < 24) { // determination of CrystalRow and CrystalCol for the 10 upper and lower pipe-region row of half subunits
      if (copyNoCrys == 1 || copyNoCrys == 3)
        CrystalRow = (1 + 17 * getsign(SubunitRow)) / 2;
      else // here necessarily: copyNoCrys==2 || copyNoCrys==4
        CrystalRow = (17 * getsign(SubunitRow) - 1) / 2;

      if (SubunitCol == 0 && copyNoBox == 1) {
        if (copyNoCrys == 1 || copyNoCrys == 4)
          CrystalCol = 2;
        else
          CrystalCol = 1;
      } else if (SubunitCol == 0) { // here necessarily: copyNoBox==2
        if (copyNoCrys == 1 || copyNoCrys == 4)
          CrystalCol = -1;
        else
          CrystalCol = -2;
      } else if (copyNoBox == 1) {
        if (copyNoCrys == 1 || copyNoCrys == 4)
          CrystalCol = SubunitCol * 4 + (3 + getsign(SubunitCol)) / 2;
        else // here necessarily: copyNoCrys==3 || copyNoCrys==2
          CrystalCol = SubunitCol * 4 + (1 + getsign(SubunitCol)) / 2;
      } else { // here necessarily: copyNoBox==2
        if (copyNoCrys == 1 || copyNoCrys == 4)
          CrystalCol = SubunitCol * 4 - (1 - getsign(SubunitCol)) / 2;
        else // here necessarily: copyNoCrys==3 || copyNoCrys==2
          CrystalCol = SubunitCol * 4 - (3 - getsign(SubunitCol)) / 2;
      }
    } else if (flag && ((copyNoSub >= 25 && copyNoSub <= 27) || copyNoSub == 57 || copyNoSub == 58 ||
                        (copyNoSub >= 40 && copyNoSub <= 44))) { // determination of CrystalRow and CrystalCol for the 2 outermost left and right columns of half Subunits
      if (copyNoBox == 1 && (copyNoCrys == 1 || copyNoCrys == 4))
        if (copyNoSub == 25 || copyNoSub == 42)
          CrystalRow = 2;
        else
          CrystalRow = SubunitRow * 4 + (3 + getsign(SubunitRow)) / 2;
      else if (copyNoBox == 1) // here necessarily: copyNoCrys==3 || copyNoCrys==2
        if (copyNoSub == 25 || copyNoSub == 42)
          CrystalRow = 1;
        else
          CrystalRow = SubunitRow * 4 + (1 + getsign(SubunitRow)) / 2;

      else if (copyNoBox == 2 && (copyNoCrys == 1 || copyNoCrys == 4))
        if (copyNoSub == 25 || copyNoSub == 42)
          CrystalRow = -1;
        else
          CrystalRow = SubunitRow * 4 - (1 - getsign(SubunitRow)) / 2;
      else if (copyNoBox == 2) // here necessarily: copyNoCrys==3 || copyNoCrys==2
        if (copyNoSub == 25 || copyNoSub == 42)
          CrystalRow = -2;
        else
          CrystalRow = SubunitRow * 4 - (3 - getsign(SubunitRow)) / 2;

      if (copyNoCrys == 1 || copyNoCrys == 3)
        if (!(copyNoSub >= 40 && copyNoSub <= 44))
          CrystalCol = -36;
        else
          CrystalCol = 35;
      else // here necessarily: copyNoCrys==4 || copyNoCrys==2
        if (!(copyNoSub >= 40 && copyNoSub <= 44))
        CrystalCol = -35;
      else
        CrystalCol = 36;
    } else if (flag && (copyNoSub == 28 || copyNoSub == 29 || copyNoSub == 38 || copyNoSub == 39 || copyNoSub == 45 || copyNoSub == 46 || copyNoSub == 55 ||
                        copyNoSub == 56)) { // determination of CrystalRow and CrystalCol for the 8 single peripheral half Subunits
      if (copyNoBox == 1 && (copyNoCrys == 1 || copyNoCrys == 4))
        CrystalRow = SubunitRow * 4 + (3 - getsign(SubunitRow)) / 2;
      else if (copyNoBox == 1) // here necessarily: copyNoCrys==3 || copyNoCrys==2
        CrystalRow = SubunitRow * 4 + (1 - getsign(SubunitRow)) / 2;
      else if (copyNoCrys == 1 || copyNoCrys == 4) // here necessarily: copyNoBox==2
        CrystalRow = SubunitRow * 4 - (1 + getsign(SubunitRow)) / 2;
      else // here necessarily: copyNoBox==2 && (copyNoCrys==3 || copyNoCrys==2)
        CrystalRow = SubunitRow * 4 - (3 + getsign(SubunitRow)) / 2;

      if (copyNoCrys == 4 || copyNoCrys == 2)
        CrystalCol = SubunitCol * 4 + (1 - getsign(SubunitCol)) / 2;
      else // here necessarily: copyNoCrys==3 || copyNoCrys==1
        CrystalCol = SubunitCol * 4 - (1 + getsign(SubunitCol)) / 2;
    } else if (flag && ((copyNoSub >= 30 && copyNoSub <= 37) ||
                        (copyNoSub >= 47 && copyNoSub <= 54))) { // determination of CrystalRow and CrystalCol for the 16 single peripheral half Subunits
      if (copyNoCrys == 1 || copyNoCrys == 3)
        CrystalRow = SubunitRow * 4 + (1 - 3 * getsign(SubunitRow)) / 2;
      else // here necessarily: copyNoCrys==4 || copyNoCrys==2
        CrystalRow = SubunitRow * 4 - (1 + 3 * getsign(SubunitRow)) / 2;

      if (copyNoBox == 1 && (copyNoCrys == 1 || copyNoCrys == 4))
        CrystalCol = SubunitCol * 4 + (3 + getsign(SubunitCol)) / 2;
      else if (copyNoBox == 1) // here necessarily: copyNoCrys==3 || copyNoCrys==2
        CrystalCol = SubunitCol * 4 + (1 + getsign(SubunitCol)) / 2;
      else if (copyNoCrys == 1 || copyNoCrys == 4) // here necessarily: copyNoBox==2
        CrystalCol = SubunitCol * 4 - (1 - getsign(SubunitCol)) / 2;
      else // here necessarily: copyNoBox==2 && (copyNoCrys==3 || copyNoCrys==2)
        CrystalCol = SubunitCol * 4 - (3 - getsign(SubunitCol)) / 2;
    }

    module = 3;
    crystal = CrystalCol + 36; // to avoid negative values for "crystal", since it's gonna be defatorized from detID in an easy way (see PndEmcPoint.cxx)
    row = CrystalRow + 37;     // to avoid negative values for "row"
    copy = 0;

  }

  /*else if (node_path.Contains("CrystalVol_block")) {
          module = 3;
    copy = 999;
    crystal = 999;
          row = 999;
  }*/

  ///////////////////////////////////////////////////////////////////////////
  // case new version of backward end-cap (from "emc_module4new.root" file) - 9.10.2008
  ///////////////////////////////////////////////////////////////////////////
  else if (node_path.Contains("Quarter4Vol")) {

    int copyNoSub, copyNoBox, copyNoCrys;
    int tmp1, tmp2;

    if (node_path.Contains("SubunitVol_")) {
      sscanf(node_path.Data(), "cave/Emc4_0/Quarter4Vol_%d/SubunitVol_%d/BoxVol_%d/CrystalVol_%d", &copy, &copyNoSub, &copyNoBox, &copyNoCrys);
      copyNoSub -= 1;
    } else {
      if (node_path.Contains("BoxVol_")) {
        sscanf(node_path.Data(), "cave/Emc4_0/Quarter4Vol_%d/SubunitVol%d_%d/BoxVol_%d/CrystalVol_%d", &copy, &tmp1, &copyNoSub, &copyNoBox, &copyNoCrys);
        copyNoSub -= 1;
      } else {
        sscanf(node_path.Data(), "cave/Emc4_0/Quarter4Vol_%d/SubunitVol%d_%d/BoxVol%d_%d/CrystalVol_%d", &copy, &tmp1, &copyNoSub, &tmp2, &copyNoBox, &copyNoCrys);
        copyNoSub -= 1;
      }
    }

    Int_t col = 0, k1 = 0, nRow = -1, nCrys = -1;
    Int_t subrow = 4; // 4 crystals in each subvolume
    Int_t next = 0;   // starts (from the middle) next column

    // Below: for BwEndCap we have 13 subunits
    //  Now, 26.02.2009, 3 crystals in the middle's subunit are added =>
    // => number of Subunits for BwEncCap & straight geometry is the same
    if ((copyNoSub >= 0) && (copyNoSub <= 3)) {
      next = copyNoSub;
      col = 0;
    } else if ((copyNoSub >= 4) && (copyNoSub <= 7)) {
      next = (copyNoSub - 4);
      col = 1;
    } else if ((copyNoSub >= 8) && (copyNoSub <= 10)) {
      next = (copyNoSub - 8);
      col = 2;
    } else if ((copyNoSub >= 11) && (copyNoSub <= 12)) {
      next = (copyNoSub - 11);
      col = 3;
    }

    Int_t flag4 = 1;
    // 26.02.2009
    // "next" means "row", "col" means "col"

    if (next > 1 && col > 2)
      flag4 = 0; // corner's subunit + one below
    if (next > 2 && col > 1)
      flag4 = 0; // + one from the corner to the left

    if (flag4 != 0) {
      if ((copyNoBox == 0) || (copyNoBox == 3)) {
        if (copyNoCrys == 1 || copyNoCrys == 3) {
          nCrys = next * 4 + 3;
        } else if (copyNoCrys == 0 || copyNoCrys == 2) {
          nCrys = next * 4 + 4;
        }
      } else if ((copyNoBox == 1) || (copyNoBox == 2)) {
        if (copyNoCrys == 0 || copyNoCrys == 2) {
          nCrys = next * 4 + 2;
        } else if (copyNoCrys == 1 || copyNoCrys == 3) {
          nCrys = next * 4 + 1;
        }
      }
      if ((copyNoBox == 0) || (copyNoBox == 2)) {
        if (copyNoCrys == 0 || copyNoCrys == 3) {
          nRow = subrow * col + 4;
        } else if (copyNoCrys == 1 || copyNoCrys == 2) {
          nRow = subrow * col + 3;
        }
      } else if ((copyNoBox == 1) || (copyNoBox == 3)) {
        if (copyNoCrys == 0 || copyNoCrys == 3) {
          nRow = subrow * col + 2;
        } else if (copyNoCrys == 1 || copyNoCrys == 2) {
          nRow = subrow * col + 1;
        }
      }
    }
    module = 4;
    row = nRow;
    crystal = nCrys;

  }

  ///////////////////////////////////////////////////////////////////////////
  // backward endcap - emc_module4_StraightGeo24.4.root
  ///////////////////////////////////////////////////////////////////////////
  else if (node_path.Contains("QuarterNewVol")) {
    module = 4;
    int copyNoSub, copyNoBox, copyNoCrys;
    int tmp1, tmp2;

    if (node_path.Contains("SubunitVol_")) {
      sscanf(node_path.Data(), "cave/Emc4_0/QuarterNewVol_%d/SubunitVol_%d/BoxVol_%d/CrystalVol_%d", &copy, &copyNoSub, &copyNoBox, &copyNoCrys);
      copyNoSub -= 1;
    } else {
      if (node_path.Contains("BoxVol_")) {
        sscanf(node_path.Data(), "cave/Emc4_0/QuarterNewVol_%d/SubunitVol%d_%d/BoxVol_%d/CrystalVol_%d", &copy, &tmp1, &copyNoSub, &copyNoBox, &copyNoCrys);
        copyNoSub -= 1;
      } else {
        sscanf(node_path.Data(), "cave/Emc4_0/QuarterNewVol_%d/SubunitVol%d_%d/BoxVol%d_%d/CrystalVol_%d", &copy, &tmp1, &copyNoSub, &tmp2, &copyNoBox, &copyNoCrys);
        copyNoSub -= 1;
      }
    }

    Int_t col = 0, nRow = -1, nCrys = -1;
    Int_t next = 0; // starts (from the middle) next column

    // Below: for BwEndCap we have 13 subunits
    //  Now, 26.02.2009, 3 crystals in the middle's subunit are added =>
    // => number of Subunits for BwEncCap & straight geometry is the same
    if ((copyNoSub >= 0) && (copyNoSub <= 2)) {
      next = copyNoSub + 1;
      col = 0;
    } else if ((copyNoSub >= 3) && (copyNoSub <= 6)) {
      next = (copyNoSub - 3);
      col = 1;
    } else if ((copyNoSub >= 7) && (copyNoSub <= 10)) {
      next = (copyNoSub - 7);
      col = 2;
    } else if ((copyNoSub >= 11) && (copyNoSub <= 13)) {
      next = (copyNoSub - 11);
      col = 3;
    }
    //       cout << "copyNoSub= " << copyNoSub << " copyNoBoxs= " << copyNoBox << " copyNoCrys= " << copyNoCrys << endl;

    Int_t flag4 = 1;
    // 26.02.2009
    // "next" means "row", "col" means "col"

    if (next == 3 && col == 3)
      flag4 = 0; // corner's subunit + one below
    if (next == 0 && col == 0)
      flag4 = 0; // + one from the corner to the left
                 //       cout << "next= " << next << " col= " << col << endl;
    if (flag4 != 0) {
      if ((copyNoBox == 0) || (copyNoBox == 3)) {
        if (copyNoCrys == 1 || copyNoCrys == 3) {
          nCrys = next * 4 + 3;
        } else if (copyNoCrys == 0 || copyNoCrys == 2) {
          nCrys = next * 4 + 4;
        }
      } else if ((copyNoBox == 1) || (copyNoBox == 2)) {
        if (copyNoCrys == 0 || copyNoCrys == 2) {
          nCrys = next * 4 + 2;
        } else if (copyNoCrys == 1 || copyNoCrys == 3) {
          nCrys = next * 4 + 1;
        }
      }
      if ((copyNoBox == 0) || (copyNoBox == 2)) {
        if (copyNoCrys == 0 || copyNoCrys == 3) {
          nRow = col * 4 + 4;
        } else if (copyNoCrys == 1 || copyNoCrys == 2) {
          nRow = col * 4 + 3;
        }
      } else if ((copyNoBox == 1) || (copyNoBox == 3)) {
        if (copyNoCrys == 0 || copyNoCrys == 3) {
          nRow = col * 4 + 2;
        } else if (copyNoCrys == 1 || copyNoCrys == 2) {
          nRow = col * 4 + 1;
        }
      }
    }
    module = 4;
    row = nRow;
    crystal = nCrys;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Bwd Endcap geometry - 2017 version
  // copy: quarter no. (0-4), row: submodule no. (0-9), crystal: cryst. no. (0-15)
  ///////////////////////////////////////////////////////////////////////////
  else if (node_path.Contains("BWECinnerVol")) {
    int submodType = -1;
    const char pathform[] = "cave/Emc4_0/BWECouterVol_0/BWECinnerVol_0/BWECquarter_%d/BWECsubmodule%d_%d/PWOCrystal_%d";
    sscanf(node_path.Data(), pathform, &copy, &submodType, &row, &crystal);
    module = 4;
  }

  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  else if (node_path.Contains("EmcProto")) {
    module = 7;
    copy = 1;
    sscanf(node_path.Data(), "cave/EmcProto_0/emc07r%dc%d_0", &row, &crystal);
  }

  ///////////////////////////////////////////////////////////////////////////
  // Proto60
  ///////////////////////////////////////////////////////////////////////////
  else if (node_path.Contains("Proto60")) {
    if (node_path.Contains("Passive") || node_path.Contains(TRegexp(".*PartAss_[0-9]*$"))) {
      return kFALSE;
    }
    module = 7;
    copy = 1;
    Int_t type = 1;

    if (node_path.Contains("CrystalType6aoPartAss")) {
      sscanf(node_path.Data(), "cave/Proto60_0/Active_1/Row%d_1/CrystalType6aoPartAss_%d/CrystalType6a_1", &row, &crystal);
    } else if (node_path.Contains("CrystalType6boPartAss")) {
      sscanf(node_path.Data(), "cave/Proto60_0/Active_1/Row%d_1/CrystalType6boPartAss_%d/CrystalType6b_1", &row, &crystal);
      type = 2;
    } else {
      cout << "crystal name in  Emc Proto: " << node_path << " missmatch pattern" << endl;
      return false;
    }
    crystal = ((crystal - 1) % 5) * 2 + type;

    //        printf("found node: %s\n Rowstring: %s crytalstring:%s\nrow: %d crystal: %d\n",node_path.Data(),((TObjString *)subStrL->At(1))->GetName(),((TObjString
    //        *)subStrL->At(2))->GetName(),row,crystal);
  }

  ///////////////////////////////////////////////////////////////////////////
  // case of endcups of forward calorimeter from .dat file
  ///////////////////////////////////////////////////////////////////////////
  else {
    int tmp1;
    sscanf(node_path.Data(), "cave/Emc%d_%d/emc%dr%dc%d_0", &tmp1, &copy, &module, &row, &crystal);
  }
  return true;
}

PndEmcTwoCoordIndex *PndEmcStructure::locateIndex(double theta, double phi) const
{
  // Find crystal with minimal angular difference with given direction
  TVector3 vec(0, 0, 10);
  vec.SetTheta(theta);
  vec.SetPhi(phi);
  PndEmcTwoCoordIndex *tci;
  double diff = 1000;

  // 	std::map <PndEmcTwoCoordIndex*, PndEmcXtal*>::const_iterator iter=fTciXtalMap.begin();
  PndEmcTciXtalMap::const_iterator iter = fTciXtalMap.begin();
  while (iter != (fTciXtalMap).end()) {
    TVector3 vec2 = ((*iter).second)->frontCentre();
    double tmpDiff = vec2.Angle(vec);

    if (tmpDiff < diff) {
      tci = (*iter).first;
      diff = tmpDiff;
    }

    iter++;
  }

  return tci;
}

// Print information on angular position for all the crystall into the file
void PndEmcStructure::Print(string filename, Int_t option) const
{
  ofstream f(filename.c_str());
  // different option corresponds to different details level of output
  if (option == 1) {
    f << "detID"
      << "\t"
      << "ThetaInd"
      << "\t"
      << "PhiInd"
      << "\t"
      << "theta_centre"
      << "\t"
      << "theta_frontface"
      << "\t"
      << "dTheta"
      << "\t"
      << "phi_centre"
      << "\t"
      << "phi_frontface"
      << "\t"
      << "dPhi" << endl;
    TVector3 centre, front_centre;
    double theta_c, theta_f, phi_c, phi_f;
    double dTheta, dPhi;
    // 		std::map <PndEmcTwoCoordIndex*, PndEmcXtal*>::const_iterator iter=fTciXtalMap.begin();
    PndEmcTciXtalMap::const_iterator iter = fTciXtalMap.begin();
    PndEmcTwoCoordIndex *tci;
    while (iter != (fTciXtalMap).end()) {
      tci = (*iter).first;
      centre = ((*iter).second)->centre();
      front_centre = ((*iter).second)->frontCentre();
      theta_c = centre.Theta() * TMath::RadToDeg();
      phi_c = centre.Phi() * TMath::RadToDeg();
      theta_f = front_centre.Theta() * TMath::RadToDeg();
      phi_f = front_centre.Phi() * TMath::RadToDeg();

      dTheta = theta_c - theta_f;
      dPhi = phi_c - phi_f;

      f << tci->Index() << "\t" << tci->XCoord() << "\t" << tci->YCoord() << "\t" << theta_c << "\t" << theta_f << "\t" << dTheta << "\t" << phi_c << "\t" << phi_f << "\t" << dPhi
        << endl;
      iter++;
    }
  } else if (option == 2) {
    f << "detID"
      << "\t"
      << "ThetaInd"
      << "\t"
      << "PhiInd"
      << "\t"
      << "X"
      << "\t"
      << "Y"
      << "\t"
      << "Z" << endl;
    TVector3 front_centre;
    TVector3 centre;
    double x, y, z;
    // 		std::map <PndEmcTwoCoordIndex*, PndEmcXtal*>::const_iterator iter=fTciXtalMap.begin();
    PndEmcTciXtalMap::const_iterator iter = fTciXtalMap.begin();
    PndEmcTwoCoordIndex *tci;
    while (iter != (fTciXtalMap).end()) {
      tci = (*iter).first;
      front_centre = ((*iter).second)->frontCentre();
      centre = ((*iter).second)->centre();
      // x=front_centre.X();y=front_centre.Y();z=front_centre.Z();
      x = centre.X();
      y = centre.Y();
      z = centre.Z();

      f << tci->Index() << "\t" << tci->XCoord() << "\t" << tci->YCoord() << "\t" << x << "\t" << y << "\t" << z << endl;
      iter++;
    }
  }

  f.close();
  cout << "write emc structure to log file" << endl;
}

ClassImp(PndEmcStructure)
