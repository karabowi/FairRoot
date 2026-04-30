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

#include "PndFtsMapCreator.h"
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

PndFtsMapCreator::PndFtsMapCreator() : fGeoType(0), fFtsParameters(new PndGeoFtsPar()), fTubeInRad(0), fTubeOutRad(0) //, fCopy_Map()
{
  // fCopy_Map.clear();
  // Geometry loading
  // FairRootManager* ioman = FairRootManager::Instance(); //[R.K. 01/2017] unused variable?
  // TFile *infile = ioman->GetInFile(); //[R.K. 01/2017] unused variable?
}

// PndFtsMapCreator::PndFtsMapCreator(){}
// to use in PndFts
PndFtsMapCreator::PndFtsMapCreator(Int_t geoType) : fGeoType(geoType), fFtsParameters(new PndGeoFtsPar()), fTubeInRad(0), fTubeOutRad(0) //, fCopy_Map()
{
  // fCopy_Map.clear();
  if (fGeoType != 1)
    Info("PndFtsMapCreator", "Geometry %i not supported by map", fGeoType); // CHECK
}

// crete geometry from parameters file
PndFtsMapCreator::PndFtsMapCreator(PndGeoFtsPar *ftsPar) : fGeoType(0), fFtsParameters(ftsPar), fTubeInRad(0), fTubeOutRad(0) //, fCopy_Map()
{
  // fCopy_Map.clear();
  // set general par
  SetGeneralParameters();
  // choose geometry type
  fGeoType = ftsPar->GetGeometryType(); // classic, optimized, average, detailed, CAD

  if (fGeoType != 1)
    Info("PndFtsMapCreator", "Geometry %i not supported by map", fGeoType); // CHECK
}

PndFtsMapCreator::~PndFtsMapCreator() {}

void PndFtsMapCreator::SetGeneralParameters()
{                                                //  CHECK whether it depends on geometry or not
  fTubeInRad = fFtsParameters->GetTubeInRad();   // tube inner radius
  fTubeOutRad = fFtsParameters->GetTubeOutRad(); // tube outer radius
}

// during simulation (for tubeID)
Int_t PndFtsMapCreator::GetTubeIDFromPath(TString path)
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

Int_t PndFtsMapCreator::GetTubeIDFromPathGeoType1(TString path)
{
  TString tmpstring = GetNameFromPathGeoType1(path);
  return GetTubeIDFromNameGeoType1(tmpstring);
}

PndFtsTube *PndFtsMapCreator::GetTubeFromTubeID(Int_t tubeid)
{
  if (fGeoType == 1)
    return GetTubeFromTubeIDToFillGeoType1(tubeid);
  return nullptr;
}

Int_t PndFtsMapCreator::GetChamberIDFromName(TString name)
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
  return -1;
}

Int_t PndFtsMapCreator::GetChamberIDFromPath(TString path)
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
  return -1;
}

Int_t PndFtsMapCreator::GetTubeIDFromNameGeoType1(TString name)
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
TString PndFtsMapCreator::GetNameFromPathGeoType1(TString path)
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

Int_t PndFtsMapCreator::GetLayerID(Int_t chamberid, Int_t tudeid, TString path)
{

  Int_t tube = tudeid;
  // Int_t chamber=chamberid; //[R.K. 01/2017] unused variable?
  TString tmpstring = path;

  Int_t i = 1;
  if (chamberid == 1 || chamberid == 2) {
    if (chamberid == 2) {
      i = i + 8;
    }
    while (tube > 132) {
      tube = tube - 132;
      i++;
    }
  }
  if (chamberid == 3) {
    i = i + 16;
    if (tmpstring.Contains("fts32")) {
      tube = tube + 352;
    }
    if (tmpstring.Contains("fts33")) {
      tube = tube + 704;
    }
    if (tmpstring.Contains("fts34")) {
      tube = tube + 1056;
    }
    while (tube > 176) {
      tube = tube - 176;
      i++;
    }
  }
  if (chamberid == 4) {
    i = i + 24;
    if (tmpstring.Contains("fts36")) {
      tube = tube + 416;
    }
    if (tmpstring.Contains("fts37")) {
      tube = tube + 832;
    }
    if (tmpstring.Contains("fts38")) {
      tube = tube + 1248;
    }
    while (tube > 208) {
      tube = tube - 208;
      i++;
    }
  }
  if (chamberid == 5) {
    i = i + 32;
    while (tube > 388) {
      tube = tube - 388;
      i++;
    }
  }
  if (chamberid == 6) {
    i = i + 40;
    while (tube > 388) {
      tube = tube - 388;
      i++;
    }
  }
  return i;
}

Int_t PndFtsMapCreator::IsSkew(Int_t layerid)
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

Int_t PndFtsMapCreator::GetTubeIDTot(Int_t chamberid, Int_t layerid, Int_t tubeid, TString path)
{
  Int_t chamber = chamberid;
  Int_t layer = layerid;
  Int_t tubeID = tubeid;
  TString tmpstring = path;
  Int_t tube = 0;
  Int_t totTubeID = 0;
  Int_t strawCh12 = 132, strawCh3 = 176, strawCh4 = 208, strawCh56 = 388;

  if (chamberid == 1) {
    tube = tubeID;
  }
  if (chamberid == 2) {
    tube = tubeID + (8 * strawCh12);
  }
  if (chamberid == 3) {
    if (layer == 17 || layer == 18) {
      tube = tubeID + (strawCh12 * (16));
    }
    if (layer == 19 || layer == 20) {
      tube = tubeID + (strawCh12 * 16) + (strawCh3 * 2) + (16 * 2);
    } //
    if (layer == 21 || layer == 22) {
      tube = tubeID + (strawCh12 * 16) + (strawCh3 * 4) + (16 * 4);
    } //
    if (layer == 23 || layer == 24) {
      tube = tubeID + (strawCh12 * 16) + (strawCh3 * 6) + (16 * 6);
    } //
  }
  if (chamberid == 4) {
    if (layer == 25 || layer == 26) {
      tube = tubeID + (strawCh12 * (16) + (strawCh3 * 8));
    } //
    if (layer == 27 || layer == 28) {
      tube = tubeID + (strawCh12 * (16) + (strawCh3 * 8) + (strawCh4 * 2)) + 16 * 2;
    } //
    if (layer == 29 || layer == 30) {
      tube = tubeID + (strawCh12 * (16) + (strawCh3 * 8) + (strawCh4 * 4)) + 16 * 4;
    } //
    if (layer == 31 || layer == 32) {
      tube = tubeID + (strawCh12 * (16) + (strawCh3 * 8) + (strawCh4 * 6)) + 16 * 6;
    } //
  }
  if (chamberid == 5) {
    tube = tubeID + (strawCh12 * 16) + (strawCh3 * 8) + (strawCh4 * 8);
  }
  if (chamberid == 6) {
    tube = tubeID + (strawCh12 * 16) + (strawCh3 * 8) + (strawCh4 * 8) + (strawCh56 * 8);
  }

  // up and down short tubes have different name but the same ID number
  // I shift the tube ID to give a different ID number to each tube
  // tubeid chamber 1,2: between 1-143*8, hole:12 tubes
  // tubeid chamber 3,4: between 1-ch3(4)*2 for each double layer, hole: 16 tubes
  // tubeid chamber 5: between 1-388*8, hole: 24 tubes
  // tubeid chamber 6: between 1-388*8, hole: 24 tubes

  // chamber 1 and chamber 2
  if (chamber == 1) {
    int shift = 0;
    if (!tmpstring.Contains("down") && !tmpstring.Contains("up")) {
      if (tubeid <= 61) {
        totTubeID = tube + shift;
      } else {
        if (abs(tubeid - (strawCh12 * (layer))) <= 61) {
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
      if (tubeid <= 61) {
        totTubeID = tube + shift;
      } else {
        if (abs(tubeid - (strawCh12 * (layer - 8))) <= 61) {
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
  if (chamber == 3) {
    int shift = 12 * (2 * 8);
    if (!tmpstring.Contains("down") && !tmpstring.Contains("up")) {
      if (tubeid <= 81) {
        totTubeID = tube + shift;
      } else {
        if (layer % 2 != 0) { // odd layers
          if (abs(tubeid - (strawCh3)) <= 81) {
            totTubeID = tube + shift + 16;
          } else {
            totTubeID = tube + shift;
          }
        } else {
          if (abs(tubeid - (strawCh3 * 2)) <= 81) {
            totTubeID = tube + shift + 16 * 2;
          } else {
            totTubeID = tube + shift + 16;
          }
        }
      }
    }
    if (tmpstring.Contains("down")) {
      if (layer % 2 != 0) {
        totTubeID = tube + 16 + shift;
      } // odd layer
      else {
        totTubeID = tube + 16 * 2 + shift;
      }
    }
    if (tmpstring.Contains("up")) {
      if (layer % 2 != 0) {
        totTubeID = tube + shift;
      } // odd layer
      else {
        totTubeID = tube + 16 + shift;
      }
    }
  }

  // chamber 4
  if (chamber == 4) {
    int shift = 12 * (2 * 8) + 16 * 8;
    if (!tmpstring.Contains("down") && !tmpstring.Contains("up")) {
      if (tubeid <= 97) {
        totTubeID = tube + shift;
      } else {
        if (layer % 2 != 0) {                    // odd layers
          if (abs(tubeid - (strawCh4)) <= 103) { //
            totTubeID = tube + shift + 16;
          } else {
            totTubeID = tube + shift;
          }
        } else {
          if (abs(tubeid - (strawCh4 * 2)) <= 103) {
            totTubeID = tube + shift + 16 * 2;
          } else {
            totTubeID = tube + shift + 16;
          }
        }
      }
    }
    if (tmpstring.Contains("down")) {
      if (layer % 2 != 0) {
        totTubeID = tube + 16 + shift;
      } // odd layer
      else {
        totTubeID = tube + 16 * 2 + shift;
      }
    }
    if (tmpstring.Contains("up")) {
      if (layer % 2 != 0) {
        totTubeID = tube + shift;
      } // odd layer
      else {
        totTubeID = tube + 16 + shift;
      }
    }
  }

  // chamber 5
  // 12*16+16*16 are the tubes shifted in the previous chambers
  if (chamber == 5) {
    int shift = 12 * 16 + 16 * 16;
    if (!tmpstring.Contains("down") && !tmpstring.Contains("up")) {
      if (tubeid <= 177) {
        totTubeID = tube + shift;
      } else {
        if (abs(tubeid - (strawCh56 * (layer - 32))) <= 194) {
          totTubeID = tube + 24 * (layer - 32) + shift;
        } else {
          totTubeID = tube + 24 * (layer - 33) + shift;
        }
      }
    }
    if (tmpstring.Contains("up")) {
      totTubeID = tube + 24 * (layer - 33) + shift;
    }
    if (tmpstring.Contains("down")) {
      totTubeID = tube + 24 * (layer - 32) + shift;
    }
  }

  // chamber 6
  if (chamber == 6) {
    int shift = 12 * 16 + 16 * 16 + 24 * 8;
    if (!tmpstring.Contains("down") && !tmpstring.Contains("up")) {
      if (tubeid <= 177) {
        totTubeID = tube + shift;
      } else {
        if (abs(tubeid - (strawCh56 * (layer - 40))) <= 194) {
          totTubeID = tube + 24 * (layer - 40) + shift;
        } else {
          totTubeID = tube + 24 * (layer - 41) + shift;
        }
      }
    }
    if (tmpstring.Contains("up")) {
      totTubeID = tube + 24 * (layer - 41) + shift;
    }
    if (tmpstring.Contains("down")) {
      totTubeID = tube + 24 * (layer - 40) + shift;
    }
  }

  return totTubeID;
}

/////////////////////////////////////////////////////////////
/// this function will be used in PndFtsHitProducesRealFast
////////////////////////////////////////////////////////////

// fill the tube map at the beginning of the run
TClonesArray *PndFtsMapCreator::FillTubeArray()
{

  std::cout << "##### PndFtsMapCreator::FillTubeArray() ######" << std::endl;
  std::cout << "fGeoType=" << fGeoType << std::endl;
  if (fGeoType == 1)
    return FillTubeArrayGeoType1();
  return nullptr;
}

TString PndFtsMapCreator::GetNameFromTubeIDGeoType1(Int_t tubeid, Bool_t isCopy)
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

PndFtsTube *PndFtsMapCreator::GetTubeFromTubeIDToFillGeoType1(Int_t tubeid)
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
    // cout << "PndFtsMapCreator::GetTubeFromTubeIDToFillGeoType1: tube " << tubename << " not f\ound (nor as a copy)" << endl;
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
  // fCopy_Map[tubeid] = isCopy;

  return new PndFtsTube((float)x, (float)y, (float)z, r[0][0], r[0][1], r[0][2], r[1][0], r[1][1], r[1][2], r[2][0], r[2][1], r[2][2], fTubeInRad, fTubeOutRad, halflength);
}

PndFtsTube *PndFtsMapCreator::GetTubeFromNameToFillGeoType1(TString tubename, Int_t, Int_t)
{ // tubeid layerid //[R.K.03/2017] unused variable(s)

  TObjArray *geoPassNodes = fFtsParameters->GetGeoPassiveNodes();
  // Bool_t isCopy = kTRUE; //[R.K. 01/2017] unused variable
  // try as if it was a copy stt01tube#XXX

  FairGeoNode *pnode = (FairGeoNode *)geoPassNodes->FindObject(tubename);
  if (!pnode) { // try as if it was a solo stt01tubeXXX
    // isCopy = kFALSE; //[R.K. 01/2017] unused variable
    // tubename = GetNameFromTubeIDGeoType1(tubeid, isCopy);
    // pnode = (FairGeoNode*) geoPassNodes->FindObject(tubename);
  }

  if (!pnode) {
    cout << "PndFtsMapCreator::GetTubeFromNameToFillGeoType1: tube " << tubename << " not found (nor as a copy)" << endl;
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

  return new PndFtsTube((float)x, (float)y, (float)z, r[0][0], r[0][1], r[0][2], r[1][0], r[1][1], r[1][2], r[2][0], r[2][1], r[2][2], fTubeInRad, fTubeOutRad, halflength);
}

TClonesArray *PndFtsMapCreator::FillTubeArrayGeoType1()
{

  // ofstream myfile("testNumber.txt");
  TObjArray *geoPassNodes = fFtsParameters->GetGeoPassiveNodes();
  TClonesArray *tubeArray = new TClonesArray("PndFtsTube");
  //  tubeArray->Delete();
  // int mytest=0; //[R.K. 01/2017] unused variable?
  // std::cout << "TA: entries: " <<  geoPassNodes->GetEntriesFast() << std::endl;
  for (int i = 0; i < geoPassNodes->GetEntriesFast(); i++) {
    FairGeoNode *pnode = (FairGeoNode *)geoPassNodes->At(i);
    if (!pnode) {
      std::cout << "PndFtsMapCreator::FillTubeArrayGeoType1 : tubename=" << pnode->GetName() << " not existing!!!" << std::endl;
      continue;
    }
    TString tubename = pnode->GetName();

    // std::cout<<"PndFtsMapCreator::FillTubeArrayGeoType1 : tubename="<<tubename<< std::endl;
    if ((!tubename.Contains("tube")) || (!tubename.Contains("fts"))) {
      // myfile<<"PndFtsMapCreator::FillTubeArrayGeoType1 : skipping tubename="<<tubename<<endl;
      continue;
    }

    Int_t tempChamber = GetChamberIDFromName(tubename);
    Int_t tubeID = GetTubeIDFromNameGeoType1(tubename);
    Int_t tempLayer = GetLayerID(tempChamber, tubeID, tubename);
    Int_t totTubeID = GetTubeIDTot(tempChamber, tempLayer, tubeID, tubename);
    PndFtsTube *ftstube = GetTubeFromNameToFillGeoType1(tubename, totTubeID, tempLayer);
    new ((*tubeArray)[totTubeID]) PndFtsTube(*ftstube);

    delete (ftstube);
    // myfile <<  tubename << " " << totTubeID << " "<<tempLayer<<endl;
  }
  // std:cout << "end: " << tubeArray->GetEntriesFast() << std::endl;
  return tubeArray;
}

ClassImp(PndFtsMapCreator)
