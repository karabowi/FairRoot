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

//////////////////////////////////////////////////////////
// Mapper for TFT
// chamberID layerID tubeIDtot
//
// author: Isabella Garzia
//////////////////////////////////////////////////////////

#include "PndFtsMapCreator2.h"
#include "PndFtsTube.h"
#include "PndGeoFtsPar.h"

#include "FairGeoNode.h"
#include "FairGeoVolume.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairGeoRotation.h"
#include "FairGeoTube.h"

#include "TGeoTube.h"
#include "TVector3.h"
#include "TObjArray.h"
#include "TString.h"
#include "TGeoVolume.h"
#include "TGeoTube.h"
#include "TClonesArray.h"
#include "FairRootManager.h"

#include <iostream>
#include <fstream>

using namespace std;

PndFtsMapCreator2::PndFtsMapCreator2() : fGeoType(0), fFtsParameters(new PndGeoFtsPar()), fTubeInRad(0), fTubeOutRad(0), copy_map()
{
  copy_map.clear();
  // Geometry loading
  FairRootManager *ioman = FairRootManager::Instance();
  TFile *infile = ioman->GetInFile();
}

// PndFtsMapCreator2::PndFtsMapCreator2(){}
// to use in PndFts
PndFtsMapCreator2::PndFtsMapCreator2(Int_t geoType) : fGeoType(geoType), fFtsParameters(new PndGeoFtsPar()), fTubeInRad(0), fTubeOutRad(0), copy_map()
{
  copy_map.clear();
  if (fGeoType != 1)
    Info("PndFtsMapCreator2", "Geometry %i not supported by map", fGeoType); // CHECK
}

// crete geometry from parameters file
PndFtsMapCreator2::PndFtsMapCreator2(PndGeoFtsPar *ftsPar) : fGeoType(0), fFtsParameters(ftsPar), fTubeInRad(0), fTubeOutRad(0), copy_map()
{
  copy_map.clear();
  // set general par
  SetGeneralParameters();
  // choose geometry type
  fGeoType = ftsPar->GetGeometryType(); // classic, optimized, average, detailed, CAD

  if (fGeoType != 1)
    Info("PndFtsMapCreator2", "Geometry %i not supported by map", fGeoType); // CHECK
}

PndFtsMapCreator2::~PndFtsMapCreator2() {}

void PndFtsMapCreator2::SetGeneralParameters()
{                                                //  CHECK whether it depends on geometry or not
  fTubeInRad = fFtsParameters->GetTubeInRad();   // tube inner radius
  fTubeOutRad = fFtsParameters->GetTubeOutRad(); // tube outer radius
}

// during simulation (for tubeID)
Int_t PndFtsMapCreator2::GetTubeIDFromPath(TString path)
{
  if (fGeoType == 1)
    return GetTubeIDFromPathGeoType1(path);
  return -999;
}

// ===== GEO TYPE 1 =====
// sensitive volume is gas
// when reading the parameters we use fts01tube to retrieve geometrical information
// OK
// during simulation

Int_t PndFtsMapCreator2::GetTubeIDFromPathGeoType1(TString path)
{
  TString tmpstring = GetNameFromPathGeoType1(path);
  return GetTubeIDFromNameGeoType1(tmpstring);
}

PndFtsTube *PndFtsMapCreator2::GetTubeFromTubeID(Int_t tubeid)
{
  if (fGeoType == 1)
    return GetTubeFromTubeIDToFillGeoType1(tubeid);
  return nullptr;
}

Int_t PndFtsMapCreator2::GetChamberIDFromName(TString name)
{
  TString pathstring = name;
  if (pathstring.Contains("fts01")) {
    // std::cout<<"chamber 1"<<std::endl;
    return 1;
  }
  if (pathstring.Contains("fts02")) {
    // std::cout<<"chamber 2"<<std::endl;
    return 2;
  }
  if (pathstring.Contains("fts31") || pathstring.Contains("fts32") || pathstring.Contains("fts33") || pathstring.Contains("fts34")) {
    // std::cout<<"chamber 3"<<std::endl;
    return 3;
  }
  if (pathstring.Contains("fts35") || pathstring.Contains("fts36") || pathstring.Contains("fts37") || pathstring.Contains("fts38")) {
    // std::cout<<"chamber 4"<<std::endl;
    return 4;
  }
  if (pathstring.Contains("fts05tube")) {
    // std::cout<<"chamber 5"<<std::endl;
    return 5;
  }
  if (pathstring.Contains("fts06tube")) {
    // std::cout<<"chamber 6"<<std::endl;
    return 6;
  }
}

Int_t PndFtsMapCreator2::GetChamberIDFromPath(TString path)
{
  TString pathstring = path;
  if (pathstring.Contains("fts01tube")) {
    // std::cout<<"chamber 1"<<std::endl;
    return 1;
  }
  if (pathstring.Contains("fts02tube")) {
    // std::cout<<"chamber 2"<<std::endl;
    return 2;
  }
  if (pathstring.Contains("fts31tube") || pathstring.Contains("fts32tube") || pathstring.Contains("fts33tube") || pathstring.Contains("fts34tube")) {
    // std::cout<<"chamber 3"<<std::endl;
    return 3;
  }
  if (pathstring.Contains("fts35tube") || pathstring.Contains("fts36tube") || pathstring.Contains("fts37tube") || pathstring.Contains("fts38tube")) {
    // std::cout<<"chamber 4"<<std::endl;
    return 4;
  }
  if (pathstring.Contains("fts05tube")) {
    // std::cout<<"chamber 5"<<std::endl;
    return 5;
  }
  if (pathstring.Contains("fts06tube")) {
    // std::cout<<"chamber 6"<<std::endl;
    return 6;
  }
}

Int_t PndFtsMapCreator2::GetTubeIDFromNameGeoType1(TString name)
{

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

// name as in geo file: from path "_" to name "#"
TString PndFtsMapCreator2::GetNameFromPathGeoType1(TString path)
{
  TString tmpstring = path;

  // std::cout<<"tmpstring original###### = "<<tmpstring<<std::endl;

  // cut /cave_1/fts01assembly_0/
  TString cavename;
  cavename = "/cave_1/ftsXXassembly_X/";
  tmpstring = tmpstring(cavename.Sizeof() - 1, tmpstring.Sizeof());

  // cut /fts01gas...
  int index = tmpstring.Index("/");
  tmpstring = tmpstring(0, index);

  // replace _ with #
  //  if(tmpstring.Contains("_0")) {
  //    tmpstring.Chop(); // cut "0"
  //   tmpstring.Chop(); // cut "_"
  // }

  if (tmpstring.Contains("down")) {
    tmpstring.Replace(13, 1, "#");
    // std::cout<<"tmpstring = "<<tmpstring<<std::endl;
  }
  if (tmpstring.Contains("up")) {
    tmpstring.Replace(11, 1, "#");
    // std::cout<<"tmpstring = "<<tmpstring<<std::endl;
  }
  if (!tmpstring.Contains("up") && !tmpstring.Contains("down")) {
    tmpstring.Replace(9, 1, "#");
    // std::cout<<"tmpstring = "<<tmpstring<<std::endl;
  }

  return tmpstring;
}

Int_t PndFtsMapCreator2::GetLayerID(Int_t chamberid, Int_t tudeid, TString path)
{

  Int_t tube = tudeid;
  Int_t chamber = chamberid;
  TString tmpstring = path;

  Int_t i = 1;
  if (chamberid == 1 || chamberid == 2) {
    if (chamberid == 2) {
      i = i + 8;
    }
    while (tube > 128) {
      tube = tube - 128;
      i++;
    }
  }
  if (chamberid == 3 || chamberid == 4) {
    if (chamberid == 3) {
      i = i + 16;
    } else {
      i = i + 24;
    }
    if (tmpstring.Contains("fts32")) {
      tube = tube + 384;
    }
    if (tmpstring.Contains("fts33")) {
      tube = tube + 768;
    }
    if (tmpstring.Contains("fts34")) {
      tube = tube + 1152;
    }
    // if(tmpstring.Contains("fts35")){tube=tube+}
    if (tmpstring.Contains("fts36")) {
      tube = tube + 384;
    }
    if (tmpstring.Contains("fts37")) {
      tube = tube + 768;
    }
    if (tmpstring.Contains("fts38")) {
      tube = tube + 1152;
    }
    while (tube > 192) {
      tube = tube - 192;
      i++;
    }
  }
  if (chamberid == 5) {
    i = i + 32;
    while (tube > 400) {
      tube = tube - 400;
      i++;
    }
  }
  if (chamberid == 6) {
    i = i + 40;
    while (tube > 592) {
      tube = tube - 592;
      i++;
    }
  }
  return i;
}

Int_t PndFtsMapCreator2::GetTubeIDTot(Int_t chamberid, Int_t layerid, Int_t tubeid, TString path)
{
  Int_t chamber = chamberid;
  Int_t layer = layerid;
  Int_t tubeID = tubeid;
  TString tmpstring = path;
  Int_t tube = 0;
  Int_t totTubeID = 0;

  if (chamberid == 1) {
    tube = tubeID;
  }
  if (chamberid == 2) {
    tube = tubeID + 1024;
  }
  if (chamberid == 3) {
    if (layer == 17 || layer == 18) {
      tube = tubeID + (128 * (16));
    }
    if (layer == 19 || layer == 20) {
      tube = tubeID + (128 * 16) + (192 * 2) + (12 * 2);
    } // 2648
    if (layer == 21 || layer == 22) {
      tube = tubeID + (128 * 16) + (192 * 4) + (12 * 4);
    } // 3056
    if (layer == 23 || layer == 24) {
      tube = tubeID + (128 * 16) + (192 * 6) + (12 * 6);
    } // 3464
  }
  if (chamberid == 4) {
    if (layer == 25 || layer == 26) {
      tube = tubeID + (128 * (16) + 192 * 8);
    } // 3872
    if (layer == 27 || layer == 28) {
      tube = tubeID + (128 * (16) + 192 * 10) + 12 * 2;
    } // 4280
    if (layer == 29 || layer == 30) {
      tube = tubeID + (128 * (16) + 192 * 12) + 12 * 4;
    } // 4688
    if (layer == 31 || layer == 32) {
      tube = tubeID + (128 * (16) + 192 * 14) + 12 * 6;
    } // 5096
  }
  if (chamberid == 5) {
    tube = tubeID + (128 * 16) + (192 * 16);
  }
  if (chamberid == 6) {
    tube = tubeID + (128 * 16) + (192 * 16) + (400 * 8);
  }

  // up and down short tubes have different name but the same ID number
  // I shift the tube ID to give a different ID number to each tube
  // tubeid chamber 1,2: between 1-128*8, hole:12 tubes
  // tubeid chamber 3,4: between 1-192*2 for each double layer, hole: 12 tubes
  // tubeid chamber 5: between 1-400*8, hole: 18 tubes
  // tubeid chamber 6: between 1-592*8, hole: 18 tubes

  // chamber 1 and chamber 2
  if (chamber == 1) {
    int shift = 0;
    if (!tmpstring.Contains("down") && !tmpstring.Contains("up")) {
      if (tubeid <= 58) {
        totTubeID = tube + shift;
      } else {
        if (abs(tubeid - (128 * (layer))) <= 58) {
          totTubeID = tube + 12 * (layer) + shift;
        } else {
          totTubeID = tube + 12 * (layer - 1) + shift;
        }
      }
    }
    if (tmpstring.Contains("up")) {
      totTubeID = tube + 12 * (layer - 1) + shift;
    }
    if (tmpstring.Contains("down")) {
      totTubeID = tube + 12 * (layer) + shift;
    }
  }
  // chamber 2
  if (chamber == 2) {
    int shift = 12 * 8;
    if (!tmpstring.Contains("down") && !tmpstring.Contains("up")) {
      if (tubeid <= 58) {
        totTubeID = tube + shift;
      } else {
        if (abs(tubeid - (128 * (layer - 8))) <= 58) {
          totTubeID = tube + 12 * (layer - 8) + shift;
        } else {
          totTubeID = tube + 12 * (layer - 9) + shift;
        }
      }
    }
    if (tmpstring.Contains("up")) {
      totTubeID = tube + 12 * (layer - 9) + shift;
    }
    if (tmpstring.Contains("down")) {
      totTubeID = tube + 12 * (layer - 8) + shift;
    }
  }
  // chamber 3
  if (chamber == 3 || chamber == 4) {
    int shift = 0;
    if (chamber == 3) {
      shift = 12 * (2 * 8);
      int numTL = 192;
    } else {
      shift = 12 * (3 * 8);
    }
    if (!tmpstring.Contains("down") && !tmpstring.Contains("up")) {
      if (tubeid <= 90) {
        totTubeID = tube + shift;
      } else {
        if (layer % 2 != 0) { // odd layers
          if (abs(tubeid - (192)) <= 90) {
            totTubeID = tube + shift + 12;
          } else {
            totTubeID = tube + shift;
          }
        } else {
          if (abs(tubeid - (192 * 2)) <= 90) {
            totTubeID = tube + shift + 12 * 2;
          } else {
            totTubeID = tube + shift + 12;
          }
        }
      }
    }
    if (tmpstring.Contains("down")) {
      if (layer % 2 != 0) {
        totTubeID = tube + 12 + shift;
      } // odd layer
      else {
        totTubeID = tube + 12 * 2 + shift;
      }
    }
    if (tmpstring.Contains("up")) {
      if (layer % 2 != 0) {
        totTubeID = tube + shift;
      } // odd layer
      else {
        totTubeID = tube + 12 + shift;
      }
    }
  }
  // chamber 5
  // 12*32 are the tubes shifted in the previous chambers
  if (chamber == 5) {
    int shift = 12 * 32;
    if (!tmpstring.Contains("down") && !tmpstring.Contains("up")) {
      if (tubeid <= 191) {
        totTubeID = tube + shift;
      } else {
        if (abs(tubeid - (400 * (layer - 32))) <= 191) {
          totTubeID = tube + 18 * (layer - 32) + shift;
        } else {
          totTubeID = tube + 18 * (layer - 33) + shift;
        }
      }
    }
    if (tmpstring.Contains("up")) {
      totTubeID = tube + 18 * (layer - 33) + shift;
    }
    if (tmpstring.Contains("down")) {
      totTubeID = tube + 18 * (layer - 32) + shift;
    }
  }

  // chamber 6
  if (chamber == 6) {
    int shift = 12 * 32 + 18 * 8;
    if (!tmpstring.Contains("down") && !tmpstring.Contains("up")) {
      if (tubeid <= 287) {
        totTubeID = tube + shift;
      } else {
        if (abs(tubeid - (592 * (layer - 40))) <= 287) {
          totTubeID = tube + 18 * (layer - 40) + shift;
        } else {
          totTubeID = tube + 18 * (layer - 41) + shift;
        }
      }
    }
    if (tmpstring.Contains("up")) {
      totTubeID = tube + 18 * (layer - 41) + shift;
    }
    if (tmpstring.Contains("down")) {
      totTubeID = tube + 18 * (layer - 40) + shift;
    }
  }

  return totTubeID;
}

/////////////////////////////////////////////////////////////
/// this function will be used in PndFtsHitProducesRealFast
////////////////////////////////////////////////////////////

// fill the tube map at the beginning of the run
TClonesArray *PndFtsMapCreator2::FillTubeArray()
{

  std::cout << "#####Mapper:filltubearray######" << std::endl;
  std::cout << "fGeoType=" << fGeoType << std::endl;
  if (fGeoType == 1)
    return FillTubeArrayGeoType1();
  return nullptr;
}

TString PndFtsMapCreator2::GetNameFromTubeIDGeoType1(Int_t tubeid, Bool_t isCopy)
{

  // two possibilities:
  // copy : XXX --> fts01tube#XXX
  // solo : XXX --> fts01tubeXXX

  TString tmpstring;
  TString chstring;

  tmpstring += tubeid;

  // to be virfied
  if (isCopy == kTRUE) {
    tmpstring.Prepend("ftsXXtube#");
    // tmpstring.Prepend("fts01tube#");
  } else {
    tmpstring.Prepend("ftsXXtube");
  }
  return tmpstring;
}

PndFtsTube *PndFtsMapCreator2::GetTubeFromTubeIDToFillGeoType1(Int_t tubeid)
{

  TObjArray *geoPassNodes = fFtsParameters->GetGeoPassiveNodes();
  Bool_t isCopy = kTRUE;
  // try as if it was a copy stt01tube#XXX
  TString tubename = GetNameFromTubeIDGeoType1(tubeid, isCopy);
  FairGeoNode *pnode = (FairGeoNode *)geoPassNodes->FindObject(tubename);
  if (!pnode) { // try as if it was a solo stt01tubeXXX
    isCopy = kFALSE;
    tubename = GetNameFromTubeIDGeoType1(tubeid, isCopy);
    pnode = (FairGeoNode *)geoPassNodes->FindObject(tubename);
  }

  if (!pnode) {
    // cout << "PndFtsMapCreator2::GetTubeFromTubeIDToFillGeoType1: tube " << tubename << " not f\ound (nor as a copy)" << endl;
    return nullptr;
  }

  FairGeoTransform *lab = pnode->getLabTransform();
  FairGeoVector tra = lab->getTransVector();
  FairGeoRotation rot = lab->getRotMatrix();

  // geometrical info
  double x = tra.getX() / 10.; // in cm
  double y = tra.getY() / 10.; // in cm
  double z = tra.getZ() / 10.; // in cm
  double r[3][3];
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      r[i][j] = rot.getElement(i, j);

  TGeoVolume *rootvol = pnode->getRootVolume();
  TGeoTube *tube = (TGeoTube *)rootvol->GetShape();
  Double_t halflength = tube->GetDz(); // in cm
  // sets up the correspondence int (tubeID) <--> int (1 = copy/0 = solo)
  //  copy_map[key] = alloc
  copy_map[tubeid] = isCopy;

  return new PndFtsTube((float)x, (float)y, (float)z, r[0][0], r[0][1], r[0][2], r[1][0], r[1][1], r[1][2], r[2][0], r[2][1], r[2][2], fTubeInRad, fTubeOutRad, halflength);
}

PndFtsTube *PndFtsMapCreator2::GetTubeFromNameToFillGeoType1(TString tubename)
{

  Int_t tubeid = GetTubeIDFromNameGeoType1(tubename);

  TObjArray *geoPassNodes = fFtsParameters->GetGeoPassiveNodes();
  Bool_t isCopy = kTRUE;
  // try as if it was a copy stt01tube#XXX

  FairGeoNode *pnode = (FairGeoNode *)geoPassNodes->FindObject(tubename);
  if (!pnode) { // try as if it was a solo stt01tubeXXX
    isCopy = kFALSE;
    // tubename = GetNameFromTubeIDGeoType1(tubeid, isCopy);
    // pnode = (FairGeoNode*) geoPassNodes->FindObject(tubename);
  }

  if (!pnode) {
    cout << "PndFtsMapCreator2::GetTubeFromNameToFillGeoType1: tube " << tubename << " not found (nor as a copy)" << endl;
    return nullptr;
  }

  FairGeoTransform *lab = pnode->getLabTransform();
  FairGeoVector tra = lab->getTransVector();
  FairGeoRotation rot = lab->getRotMatrix();

  // geometrical info
  double x = tra.getX() / 10.; // in cm
  double y = tra.getY() / 10.; // in cm
  double z = tra.getZ() / 10.; // in cm
  double r[3][3];
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      r[i][j] = rot.getElement(i, j);

  TGeoVolume *rootvol = pnode->getRootVolume();
  TGeoTube *tube = (TGeoTube *)rootvol->GetShape();
  Double_t halflength = tube->GetDz(); // in cm
  // sets up the correspondence int (tubeID) <--> int (1 = copy/0 = solo)
  //  copy_map[key] = alloc
  copy_map[tubeid] = isCopy;

  return new PndFtsTube((float)x, (float)y, (float)z, r[0][0], r[0][1], r[0][2], r[1][0], r[1][1], r[1][2], r[2][0], r[2][1], r[2][2], fTubeInRad, fTubeOutRad, halflength);
}

TClonesArray *PndFtsMapCreator2::FillTubeArrayGeoType1()
{

  TObjArray *geoPassNodes = fFtsParameters->GetGeoPassiveNodes();
  TClonesArray *tubeArray = new TClonesArray("PndFtsTube");
  tubeArray->Delete();
  int mytest = 0;
  // std::cout << "TA: entries: " <<  geoPassNodes->GetEntriesFast() << std::endl;
  for (int i = 0; i < geoPassNodes->GetEntriesFast(); i++) {
    FairGeoNode *pnode = (FairGeoNode *)geoPassNodes->At(i);
    if (!pnode) {
      std::cout << "PndFtsMapCreator2::FillTubeArrayGeoType1 : tubename=" << pnode->GetName() << " not existing!!!" << std::endl;
      continue;
    }
    TString tubename = pnode->GetName();

    // std::cout<<"PndFtsMapCreator2::FillTubeArrayGeoType1 : tubename="<<tubename<< std::endl;
    if ((!tubename.Contains("tube")) || (!tubename.Contains("fts"))) {
      // myfile<<"PndFtsMapCreator2::FillTubeArrayGeoType1 : skipping tubename="<<tubename<<endl;
      continue;
    }

    Int_t tempChamber = GetChamberIDFromName(tubename);
    Int_t tubeID = GetTubeIDFromNameGeoType1(tubename);
    Int_t tempLayer = GetLayerID(tempChamber, tubeID, tubename);
    Int_t totTubeID = GetTubeIDTot(tempChamber, tempLayer, tubeID, tubename);
    // PndFtsTube *ftstube = GetTubeFromTubeIDToFillGeoType1(tubeID);
    PndFtsTube *ftstube = GetTubeFromNameToFillGeoType1(tubename);
    new ((*tubeArray)[totTubeID]) PndFtsTube(*ftstube);
  }
  // std:cout << "end: " << tubeArray->GetEntriesFast() << std::endl;
  return tubeArray;
}

ClassImp(PndFtsMapCreator2)
