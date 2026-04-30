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

#ifndef PNDMDTIGEOMETRY_H
#define PNDMDTIGEOMETRY_H 1

#include "TVector3.h"
#include "TGeoVolume.h"
#include <vector>
#include "TF1.h"
#include <map>
#include <set>
#include "PndMdtID.h"
#include "TGeoMatrix.h"
#include <iostream>

using std::ostream;
ostream &operator<<(ostream &os, const TVector3 &v3);

class PndMdtIGeometry : public TNamed {

 public:
  static PndMdtIGeometry *Instance();
  /** Default constructor **/
  PndMdtIGeometry();
  /** Destructor **/
  ~PndMdtIGeometry();

  void SetVerbose(Int_t _v) { fVerbose = _v; }
  void AddSensor(TString _v) { fSensorSet.insert(_v); }
  // version and Init() should be invoked first before using access functions
  // now only support ROOT geometry version 0, 1, 2
  Bool_t Init();
  //================================================================
  // Access to Tube Geometery for given parameters
  Bool_t GetTubeCenter(Int_t iDetId, TVector3 &pos) const
  {
    std::map<Int_t, InfoType>::const_iterator it = fGeoMap.find(iDetId);
    if (it == fGeoMap.end())
      return kFALSE;
    pos = it->second.Position;
    return kTRUE;
  }
  //
  Bool_t GetTubeLength(Int_t iDetId, Double_t &len) const
  {
    std::map<Int_t, InfoType>::const_iterator it = fGeoMap.find(iDetId);
    if (it == fGeoMap.end())
      return kFALSE;
    len = it->second.Length;
    return kTRUE;
  }
  //
  Bool_t GetStripLength(Int_t iDetId, Double_t &len) const
  {
    std::map<Int_t, InfoType>::const_iterator it = fGeoMap.find(iDetId);
    if (it == fGeoMap.end())
      return kFALSE;
    len = it->second.Length;
    return kTRUE;
  }
  Bool_t GetStripCenter(Int_t iDetId, TVector3 &pos) const
  {
    std::map<Int_t, InfoType>::const_iterator it = fGeoMap.find(iDetId);
    if (it == fGeoMap.end())
      return kFALSE;
    pos = it->second.Position;
    return kTRUE;
  }
  Bool_t GetLayerBoundary(Int_t iLayer, TVector3 &LBPos, TVector3 &RTPos) const
  {
    std::map<Int_t, LayerBoundary>::const_iterator it = fLayerInfoMap.find(iLayer);
    if (it == fLayerInfoMap.end())
      return kFALSE;
    LBPos = TVector3(it->second.minX, it->second.minY, it->second.minZ);
    RTPos = TVector3(it->second.maxX, it->second.maxY, it->second.maxZ);
    return kTRUE;
  }
  Bool_t MasterToLocal(Int_t iDetId, TVector3 &master, TVector3 &local)
  {
    std::map<Int_t, InfoType>::const_iterator it = fGeoMap.find(iDetId);
    if (it == fGeoMap.end())
      return kFALSE;
    TVector3 center = it->second.Position;
    TGeoMatrix *fMtrx = it->second.Matrix;
    if (!fMtrx)
      return kFALSE;
    // fMtrx->Print();
    // fMtrx->MasterToLocal(&center[0], &local[0]);
    // std::cout<<"local 1 "<<local<<std::endl;
    fMtrx->MasterToLocal(&master[0], &local[0]);
    // std::cout<<"local 2 "<<local<<std::endl;
    return kTRUE;
  }
  Bool_t MapWireToStrip(Int_t iDetId, const TVector3 &fEntryPos, Int_t &fStripDetID)
  {
    static const Double_t cSTRIPWIDTH = 1.;   // cent meter;
    static const Double_t cGAPOFSTRIPS = 0.0; // cent meter;
    Short_t iMod = PndMdtID::Module(iDetId);
    Short_t iSec = PndMdtID::Sector(iDetId);
    Short_t iLayer = PndMdtID::Layer(iDetId);
    Int_t layerID = PndMdtID::LayerID(iMod, iSec, iLayer);
    LayerInfoMapIter it = fLayerInfoMap.find(layerID);
    if (it == fLayerInfoMap.end())
      return kFALSE;
    Int_t fStripNo;
    if (iMod == 1)
      fStripNo = (fEntryPos.Z() - it->second.minZ) / (cSTRIPWIDTH + cGAPOFSTRIPS);
    else if (iMod == 2) // endcap
    {
      if (iLayer < 2)
        fStripNo = (fEntryPos.Y() - it->second.minY) / (cSTRIPWIDTH + cGAPOFSTRIPS);
      else
        fStripNo = (fEntryPos.X() - it->second.minX) / (cSTRIPWIDTH + cGAPOFSTRIPS);
    } else
      fStripNo = (fEntryPos.X() - it->second.minX) / (cSTRIPWIDTH + cGAPOFSTRIPS);
    fStripDetID = PndMdtID::Identifier(iMod, iSec, iLayer, fStripNo);
    return kTRUE;
  }
  void Print() const;

 private:
  std::set<TString> fSensorSet;
  Bool_t FindSensorinPath(TString path)
  {
    Bool_t ret = kFALSE;
    std::set<TString>::iterator it = fSensorSet.begin();
    std::set<TString>::iterator end = fSensorSet.end();
    while (it != end) {
      if (path.Contains(*it)) {
        ret = kTRUE;
        break;
      }
      ++it;
    }
    return ret;
  }

 private:
  Int_t fVerbose;
  Bool_t fGoodGeometry;
  TString fStateTip;

 public:
  struct InfoType {
    TVector3 Position;
    Double_t Length;
    TGeoMatrix *Matrix;
  };

 private:
  std::map<Int_t, InfoType> fGeoMap;
  std::map<Int_t, TGeoMatrix *> fMatrixMap;

 public:
  struct LayerBoundary {
    Double_t minX;
    Double_t minY;
    Double_t minZ;
    Double_t maxX;
    Double_t maxY;
    Double_t maxZ;
  };

 private:
  std::map<Int_t, LayerBoundary> fLayerInfoMap;
  typedef std::map<Int_t, LayerBoundary>::iterator LayerInfoMapIter;
  typedef std::pair<Int_t, LayerBoundary> LayerInfoMapValue;

  Int_t fBarrelVersion;
  Int_t fEndcapVersion;
  Int_t fForwardVersion;
  Int_t fMuonFilterVersion;
  Int_t fNumofMdt;
  Int_t fNumofStrip;
  Bool_t fInitialized;

  void LoadGeometry(TGeoNode *node);
  Bool_t GetStripInfo();
  Bool_t GetGeometryInfoV2(const TString &fFullPath, const TString &Name);
  void InsertLayerInfo(Int_t layerID, const TVector3 &pos, const TVector3 &lwh);

  Bool_t AddTubeInfo(Int_t detID, const TVector3 &center, Double_t wirelen, TGeoMatrix *matrix);
  Bool_t AddStripInfo(Int_t detID, TVector3 stripPos, Double_t stripLen, TGeoMatrix *matrix = 0);
  Bool_t AddMatrix(Int_t layerID, TGeoMatrix *matrix);

  static PndMdtIGeometry *fInstance;

  ClassDef(PndMdtIGeometry, 1);
};

#endif
