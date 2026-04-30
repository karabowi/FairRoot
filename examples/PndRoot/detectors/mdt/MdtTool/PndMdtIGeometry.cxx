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

// -------------------------------------------------------------------------
// -----                PndMdtIGeometry source file                  -----
// author, Jifeng Hu, hu@to.infn.it, Torino University
// -------------------------------------------------------------------------

#include "PndMdtIGeometry.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "TMath.h"
#include <assert.h>
#include <iostream>
#include <math.h>
#include <list>
#include <algorithm>
#include "PndMdtID.h"
#include <fstream>

using std::cerr;
using std::cout;
using std::endl;
ostream &operator<<(ostream &os, const TVector3 &v3)
{
  os << '(' << v3.X() << ", " << v3.Y() << ", " << v3.Z() << ')';
  return os;
}

PndMdtIGeometry *PndMdtIGeometry::fInstance = nullptr;
PndMdtIGeometry *PndMdtIGeometry::Instance()
{
  if (!fInstance)
    fInstance = new PndMdtIGeometry();
  return fInstance;
}
// -----   Default constructor   -------------------------------------------
PndMdtIGeometry::PndMdtIGeometry() : TNamed("PndMdtIGeometry", "parameterized simulation of MDT")
{
  fNumofMdt = 0;
  fNumofStrip = 0;
  fVerbose = 0;
  fGoodGeometry = kFALSE;
  fStateTip = "Success";
  fBarrelVersion = fEndcapVersion = fMuonFilterVersion = fForwardVersion = -1;
  fInitialized = kFALSE;
}
PndMdtIGeometry::~PndMdtIGeometry()
{
  // fWirePosMap.clear();
  // fWireLengthMap.clear();
  // fStripPosMap.clear();
  // fStripLengthMap.clear();
  fGeoMap.clear();
  fLayerInfoMap.clear();
}
// -------------------------------------------------------------------------
Bool_t PndMdtIGeometry::Init()
{
  if (!fInitialized) {
    fInitialized = kTRUE;
    TGeoVolume *fTopVol = gGeoManager->GetTopVolume();
    TGeoNode *fTopNode = gGeoManager->GetTopNode();
    fGoodGeometry = kFALSE;
    LoadGeometry(fTopNode);
    if (fGoodGeometry) {
      fGoodGeometry = GetStripInfo();
    }
    if (fVerbose > 0) {
      cout << "=========================================================================" << endl;
      cout << "PndMdtIGeometry::Init(), information of MDT geometry version, \n"
           << "\tBarrel #" << fBarrelVersion << endl
           << "\tEndcap #" << fEndcapVersion << endl
           << "\tForward #" << fForwardVersion << endl
           << "\tFilter #" << fMuonFilterVersion << endl;
      cout << "ROOT Geometry, top volume name#" << fTopVol->GetName() << ", top node name#" << fTopNode->GetName() << endl;
      cout << "State :" << fStateTip << endl;
      cout << (fGoodGeometry ? "Succeed to" : "Fail to ") << " load in " << fNumofMdt << " tubes, " << fNumofStrip << " strips." << endl;
      cout << "=========================================================================" << endl;
    }
  }
  return fGoodGeometry;
}
namespace shadow {
struct FullPathConverter {
  FullPathConverter(TString &str) : fullPath(str) {}
  void operator()(TString aStr)
  {
    fullPath.Append(aStr);
    fullPath.Append("/");
  }

 private:
  TString &fullPath;
};
} // namespace shadow
void PndMdtIGeometry::LoadGeometry(TGeoNode *node)
{
  static std::list<TString> stackPath;
  TString name(node->GetName());
  stackPath.push_back(name);
  fGoodGeometry = kTRUE;
  if (node->GetNdaughters() > 0) {
    Int_t nd = node->GetNdaughters();
    for (Int_t i = 0; i < nd; i++) {
      TGeoNode *theNode = node->GetDaughter(i);
      if (fGoodGeometry)
        LoadGeometry(theNode);
    }
  } else {
    // if(name.Contains("MDT"))
    if (FindSensorinPath(name)) {
      ++fNumofMdt;
      TString fFullPath;
      for_each(stackPath.begin(), stackPath.end(), shadow::FullPathConverter(fFullPath));
      fFullPath.Remove(fFullPath.Length() - 1);
      fGoodGeometry = GetGeometryInfoV2(fFullPath, stackPath.back());
    }
  }
  stackPath.pop_back();
}
// analyis the bound of one layer
void PndMdtIGeometry::InsertLayerInfo(Int_t layerID, const TVector3 &pos, const TVector3 &lwh)
{
  LayerInfoMapIter it = fLayerInfoMap.find(layerID);
  TVector3 pos1 = pos - lwh;
  TVector3 pos2 = pos + lwh;
  if (it == fLayerInfoMap.end()) {
    LayerBoundary aLayerBound;
    aLayerBound.minX = pos1.X();
    aLayerBound.minY = pos1.Y();
    aLayerBound.minZ = pos1.Z();
    aLayerBound.maxX = pos2.X();
    aLayerBound.maxY = pos2.Y();
    aLayerBound.maxZ = pos2.Z();
    fLayerInfoMap.insert(LayerInfoMapValue(layerID, aLayerBound));
  } else {
    LayerBoundary &aLayerBound = it->second;
    if (pos1.X() < aLayerBound.minX)
      aLayerBound.minX = pos1.X();
    if (pos1.Y() < aLayerBound.minY)
      aLayerBound.minY = pos1.Y();
    if (pos1.Z() < aLayerBound.minZ)
      aLayerBound.minZ = pos1.Z();
    if (pos2.X() > aLayerBound.maxX)
      aLayerBound.maxX = pos2.X();
    if (pos2.Y() > aLayerBound.maxY)
      aLayerBound.maxY = pos2.Y();
    if (pos2.Z() > aLayerBound.maxZ)
      aLayerBound.maxZ = pos2.Z();
  }
}
Bool_t PndMdtIGeometry::GetStripInfo()
{
  static const Double_t cGAPOFBOXANDSTRIP = 0.1; // cent meter
  static const Double_t cSTRIPWIDTH = 1.;        // cent meter;
  static const Double_t cSTRIPTHICKNESS = 0.20;  // cent menter
  static const Double_t cGAPOFSTRIPS = 0.0;      // cent meter;

  fStateTip = " GetStripInfo:: place strips on ";
  fStateTip += fLayerInfoMap.size();
  fStateTip += " layers.";
  LayerInfoMapIter it = fLayerInfoMap.begin();
  LayerInfoMapIter end = fLayerInfoMap.end();
  Double_t mStripZPos;
  Double_t mStripXPos;
  Double_t mStripLen;
  Double_t mDiffX;
  Double_t mDiffY;
  Int_t stripNO = 0;
  Bool_t isSucc = kTRUE;
  for (; it != end; ++it) {
    stripNO = 0;
    Int_t layerID = it->first;
    LayerBoundary &theBoundary = it->second;
    Int_t mod = PndMdtID::Module(layerID);
    Int_t sec = PndMdtID::Sector(layerID);
    Int_t layer = PndMdtID::Layer(layerID);
    // put strip
    if (mod == 1) { // barrel, left angle = 0
      Double_t zPos = theBoundary.minZ;
      while (zPos < theBoundary.maxZ) {
        mStripZPos = zPos + 0.5 * cSTRIPWIDTH;
        zPos += (cSTRIPWIDTH + cGAPOFSTRIPS);
        mDiffY = theBoundary.maxY - theBoundary.minY;
        mDiffX = theBoundary.maxX - theBoundary.minX;
        Double_t cx = 0.5 * (theBoundary.minX + theBoundary.maxX);
        Double_t cy = 0.5 * (theBoundary.minY + theBoundary.maxY);
        mStripLen = TMath::Abs(mDiffX * TMath::Cos(sec * TMath::Pi() / 4.)) + TMath::Abs(mDiffY * TMath::Sin(sec * TMath::Pi() / 4.));
        Double_t dx = TMath::Abs((cGAPOFBOXANDSTRIP + 0.5 * cSTRIPTHICKNESS + 0.5) * TMath::Cos(sec * TMath::Pi() / 4.));
        Double_t dy = TMath::Abs((cGAPOFBOXANDSTRIP + 0.5 * cSTRIPTHICKNESS + 0.5) * TMath::Sin(sec * TMath::Pi() / 4.));
        isSucc = AddStripInfo(PndMdtID::Identifier(mod, sec, layer, stripNO), TVector3(cx + dx, cy + dy, mStripZPos), mStripLen);
        if (!isSucc)
          return kFALSE;
        ++stripNO;
      }
    } else if (mod == 2) // endcap
    {
      if (layer < 3) { // layer start from 1
        Double_t xPos = theBoundary.minY;
        while (xPos < theBoundary.maxY) {
          mStripXPos = xPos + 0.5 * cSTRIPWIDTH;
          xPos += (cSTRIPWIDTH + cGAPOFSTRIPS);
          mStripLen = theBoundary.maxX - theBoundary.minX;
          Double_t cx = 0.5 * (theBoundary.minX + theBoundary.maxX);
          Double_t cz = 0.5 * (theBoundary.minZ + theBoundary.maxZ);
          Double_t dz = (cGAPOFBOXANDSTRIP + 0.5 * cSTRIPTHICKNESS + 0.5);
          isSucc = AddStripInfo(PndMdtID::Identifier(mod, sec, layer, stripNO), TVector3(cx, mStripXPos, cz + dz), mStripLen);
          if (!isSucc)
            return kFALSE;
          ++stripNO;
        }
      } else {
        Double_t xPos = theBoundary.minX;
        while (xPos < theBoundary.maxX) {
          mStripXPos = xPos + 0.5 * cSTRIPWIDTH;
          xPos += (cSTRIPWIDTH + cGAPOFSTRIPS);
          mStripLen = theBoundary.maxY - theBoundary.minY;
          Double_t cy = 0.5 * (theBoundary.minY + theBoundary.maxY);
          Double_t cz = 0.5 * (theBoundary.minZ + theBoundary.maxZ);
          Double_t dz = (cGAPOFBOXANDSTRIP + 0.5 * cSTRIPTHICKNESS + 0.5);
          isSucc = AddStripInfo(PndMdtID::Identifier(mod, sec, layer, stripNO), TVector3(mStripXPos, cy, cz + dz), mStripLen);
          if (!isSucc)
            return kFALSE;
          ++stripNO;
        }
      }
    } else {
      Double_t xPos = theBoundary.minX;
      while (xPos < theBoundary.maxX) {
        mStripXPos = xPos + 0.5 * cSTRIPWIDTH;
        xPos += (cSTRIPWIDTH + cGAPOFSTRIPS);
        mStripLen = theBoundary.maxY - theBoundary.minY;
        Double_t cy = 0.5 * (theBoundary.minY + theBoundary.maxY);
        Double_t cz = 0.5 * (theBoundary.minZ + theBoundary.maxZ);
        Double_t dz = (cGAPOFBOXANDSTRIP + 0.5 * cSTRIPTHICKNESS + 0.5);
        isSucc = AddStripInfo(PndMdtID::Identifier(mod, sec, layer, stripNO), TVector3(mStripXPos, cy, cz + dz), mStripLen);
        if (!isSucc)
          return kFALSE;
        ++stripNO;
      }
    }
    if (fVerbose > 2)
      cout << "layer ID #" << layerID << ", mod#" << mod << ", sec#" << sec << ", layer#" << layer << ", X#" << theBoundary.minX << ", " << theBoundary.maxX << ", Y#"
           << theBoundary.minY << ", " << theBoundary.maxY << ", Z#" << theBoundary.minZ << ", " << theBoundary.maxZ << ", " << stripNO << " strips." << endl;
    fNumofStrip += stripNO;
  }
  return kTRUE;
}
Bool_t PndMdtIGeometry::GetGeometryInfoV2(const TString &fFullPath, const TString &Name)
{
  Int_t iMod(-1);
  Int_t iSec(-1);
  Int_t iSec1(-1); // for barrel special section
  Int_t iLayer(-1);
  Int_t iLayer1(-1);
  Int_t iBox(-1);
  Int_t iWire(-1);
  Bool_t isSucc;
  // read this tube information
  isSucc = gGeoManager->cd(fFullPath.Data());
  if (isSucc) {
    Int_t iholder;
    if (fFullPath.BeginsWith("cave_1/Mdt_1/")) {
      fBarrelVersion = fEndcapVersion = fMuonFilterVersion = fForwardVersion = 0;
      sscanf(Name.Data(), "MDT%is%il%ib%iw%i_%i", &iMod, &iSec, &iLayer, &iBox, &iWire, &iWire);
    } else if (fFullPath.BeginsWith("cave_1/Mdt")) {
      if (fFullPath.Contains("MdtBarrel")) { //
        fBarrelVersion = 1;
        sscanf(fFullPath.Data(), "cave_1/MdtBarrel_0/MdtBarrelOct%i_%i/MdtBarrelOct%iLayer%i_%i/BP%is%il%ib%iw%i_%i/BA%i_%i/MDT%is%il%ib%iw%i_%i", &iSec1, &iSec, &iholder, &iLayer,
               &iLayer1, &iMod, &iholder, &iholder, &iBox, &iholder, &iholder, &iholder, &iholder, &iholder, &iholder, &iholder, &iholder, &iholder, &iWire);
      } else if (fFullPath.Contains("MdtEndcap")) {
        fEndcapVersion = 1;
        sscanf(fFullPath.Data(), "cave_1/MdtEndcap_0/MdtEndcapLayer%i_%i/BP%is%il%ib%iw%i_%i/BA%i_%i/MDT%is%il%ib%iw%i_%i", &iLayer1, &iLayer // MdtEndcapLayer ...
               ,
               &iMod, &iSec, &iholder, &iholder, &iholder, &iBox // BP...
               ,
               &iholder, &iholder // BA ...
               ,
               &iholder, &iholder, &iholder, &iholder, &iholder, &iWire); // MDT ...
      } else if (fFullPath.Contains("MdtMF")) {
        fMuonFilterVersion = 1;
        sscanf(fFullPath.Data(), "cave_1/MdtMF_0/MdtMFLayer%i_%i/BP%is%il%ib%iw%i_%i/BA%i_%i/MDT%is%il%ib%iw%i_%i", &iLayer1, &iLayer, &iMod, &iSec, &iholder, &iBox, &iholder,
               &iholder, &iholder, &iholder, &iholder, &iholder, &iholder, &iholder, &iholder, &iWire);
      } else if (fFullPath.Contains("MdtForward")) {
        fForwardVersion = 1;
        sscanf(fFullPath.Data(), "cave_1/MdtForward_0/MdtForwardLayer%i_%i/BP%is%il%ib%iw%i_%i/BA%i_%i/MDT%is%il%ib%iw%i_%i", &iLayer1, &iLayer, &iMod, &iSec, &iholder, &iBox,
               &iholder, &iholder, &iholder, &iholder, &iholder, &iholder, &iholder, &iholder, &iholder, &iWire);
      } else {
        cout << "PndMdtIGeometry::GetGeometryInfoV2, Unknown geometry pattern." << endl;
      }
    } else if (fFullPath.BeginsWith("cave_1/Mdt")) {
      fBarrelVersion = fEndcapVersion = fMuonFilterVersion = fForwardVersion = 0;
      sscanf(Name.Data(), "MDT%is%il%ib%iw%i_%i", &iMod, &iSec, &iLayer, &iBox, &iWire, &iWire);
    } else {
      cout << "PndMdtIGeometry::GetGeometryInfoV2, Unknown geometry pattern." << endl;
    }
    TVector3 itsPos(0, 0, 0);
    TGeoBBox *theShape = (TGeoBBox *)gGeoManager->GetCurrentVolume()->GetShape();
    TVector3 itsOrigin(theShape->GetOrigin());
    gGeoManager->LocalToMaster(&itsOrigin[0], &itsPos[0]);
    TVector3 itsLWH(theShape->GetDX(), theShape->GetDY(), theShape->GetDZ());
    Double_t wirelen;
    if (iMod == 1)
      wirelen = 2. * itsLWH.Z();
    else if (iMod == 2 && iLayer < 2)
      wirelen = 2. * itsLWH.X();
    else
      wirelen = 2. * itsLWH.Y();

    if (fVerbose > 2) {
      cout << fFullPath << endl;
      cout << "(" << iMod << ", " << iSec << ", " << iLayer << ", " << iBox << ", " << iWire << ")" << itsPos << ", " << itsLWH << endl;
    }

    TGeoMatrix *matrix = gGeoManager->GetCurrentMatrix()->MakeClone();
    Int_t detID = PndMdtID::Identifier(iMod, iSec, iLayer, iBox, iWire);
    isSucc = AddTubeInfo(detID, itsPos, wirelen, matrix);
    //
    Int_t layerID = PndMdtID::LayerID(iMod, iSec, iLayer);
    InsertLayerInfo(layerID, itsPos, itsLWH);

    AddMatrix(layerID, matrix);
  } else {
    fStateTip = "gGeoManager:: nonexisted path";
    cerr << "PndMdtIGeometry::GetGeometryInfoV2, " << fStateTip << endl;
    ;
  }
  return isSucc;
}
Bool_t PndMdtIGeometry::AddMatrix(Int_t layerID, TGeoMatrix *matrix)
{
  if (fMatrixMap.find(layerID) == fMatrixMap.end()) {
    if (fVerbose > 1)
      cout << "PndMdtIGeometry::AddMatrix(), MDT layer ID#" << layerID << " insert" << endl;
    fMatrixMap.insert(std::pair<Int_t, TGeoMatrix *>(layerID, matrix));
  }
  return kTRUE;
}
Bool_t PndMdtIGeometry::AddTubeInfo(Int_t detID, const TVector3 &center, Double_t wirelen, TGeoMatrix *matrix)
{
  if (fGeoMap.find(detID) != fGeoMap.end()) {
    fStateTip = "AddTubeInfo:: detID not unique";
    cout << "PndMdtIGeometry::" << fStateTip << ", " << detID << endl;
    ;
    return kFALSE;
  } else {
    InfoType _info;
    _info.Position = center;
    if (TMath::Abs(_info.Position.X()) < 1.e-8)
      _info.Position.SetX(0.);
    if (TMath::Abs(_info.Position.Y()) < 1.e-8)
      _info.Position.SetY(0.);
    if (TMath::Abs(_info.Position.Z()) < 1.e-8)
      _info.Position.SetZ(0.);
    _info.Length = wirelen;
    _info.Matrix = matrix;
    if (fVerbose > 1)
      cout << "PndMdtIGeometry::AddTubeInfo(), MDT detector ID#" << detID << " insert" << endl;
    fGeoMap.insert(std::pair<Int_t, InfoType>(detID, _info));
  }
  return kTRUE;
}
Bool_t PndMdtIGeometry::AddStripInfo(Int_t detID, TVector3 stripPos, Double_t stripLen, TGeoMatrix *matrix)
{
  if (fGeoMap.find(detID) != fGeoMap.end()) {
    fStateTip = "AddStripInfo:: detID not unique";
    cout << "PndMdtIGeometry::" << fStateTip << ", " << detID << endl;
    ;
    return kFALSE;
  } else {
    InfoType _info;
    _info.Position = stripPos;
    _info.Length = stripLen;
    _info.Matrix = matrix;
    if (fVerbose > 1)
      cout << "PndMdtIGeometry::AddStripInfo(), MDT detector ID#" << detID << " insert" << endl;
    fGeoMap.insert(std::pair<Int_t, InfoType>(detID, _info));
  }
  return kTRUE;
}
void PndMdtIGeometry::Print() const
{
  std::map<Int_t, InfoType>::const_iterator cit = fGeoMap.begin();
  ;
  std::map<Int_t, InfoType>::const_iterator cend = fGeoMap.end();
  ;

  std::ofstream ofs1("Tube.map", std::ios::out);
  ofs1 << "============================================================" << endl;
  ofs1 << "==>Tube Placement" << endl;
  ofs1 << "============================================================" << endl;
  ofs1 << "Mod\t"
       << "Sec\t"
       << "Layer\t"
       << "Box\t"
       << "NO.\t"
       << "X\t"
       << "Y\t"
       << "Z\t" << endl;
  for (; cit != cend; ++cit) {
    if (PndMdtID::isWire(cit->first)) {
      ofs1 << PndMdtID::Module(cit->first) << "\t" << PndMdtID::Sector(cit->first) << "\t" << PndMdtID::Layer(cit->first) << "\t" << PndMdtID::Box(cit->first) << "\t"
           << PndMdtID::Wire(cit->first) << "\t" << cit->second.Position.X() << "\t" << cit->second.Position.Y() << "\t" << cit->second.Position.Z() << "\t" << endl;
    }
  }
  ofs1.close();
  std::ofstream ofs2("Strip.map", std::ios::out);
  ofs2 << "============================================================" << endl;
  ofs2 << "==>Strip Placement" << endl;
  ofs2 << "============================================================" << endl;
  ofs2 << "Mod\t"
       << "Sec\t"
       << "Layer\t"
       << "NO.\t"
       << "X\t"
       << "Y\t"
       << "Z\t" << endl;
  cit = fGeoMap.begin();
  ;
  for (; cit != cend; ++cit) {
    if (PndMdtID::isStrip(cit->first)) {
      ofs2 << PndMdtID::Module(cit->first) << "\t" << PndMdtID::Sector(cit->first) << "\t" << PndMdtID::Layer(cit->first) << "\t" << PndMdtID::Strip(cit->first) << "\t"
           << cit->second.Position.X() << "\t" << cit->second.Position.Y() << "\t" << cit->second.Position.Z() << "\t" << endl;
    }
  }
  ofs2.close();
}

ClassImp(PndMdtIGeometry)
