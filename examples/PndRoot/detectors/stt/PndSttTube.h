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

#ifndef PNDSTTTUBE_H
#define PNDSTTTUBE_H 1

#include <iostream>

#include "TVector3.h"
#include "TMatrixT.h"
#include "TObject.h"
#include "TArrayI.h"
#include "TString.h"
#include "PndSttTubeParameters.h"

class PndSttTubeParameters;
class PndSttTube : public TObject {

 public:
  /** Default constructor **/
  PndSttTube();
  PndSttTube(PndSttTube &tube);
  PndSttTube(PndSttTubeParameters *parms, Double_t x, Double_t y, Double_t z, Double_t r11, Double_t r12, Double_t r13, Double_t r21, Double_t r22, Double_t r23, Double_t r31,
             Double_t r32, Double_t r33, Double_t radin, Double_t radout);
  PndSttTube(int id, TVector3 cnt, TVector3 wup, TVector3 wdown, const Double_t *rotmat, Double_t halflen, Double_t rin, Double_t rout);

  ~PndSttTube() {};

  // assignment operator
  PndSttTube &operator=(const PndSttTube &o)
  {
    fTubeParms = o.fTubeParms;
    fTubeID = o.fTubeID;
    fHalfLen = o.fHalfLen;
    fSkewAng = o.fSkewAng;
    fCenPos = o.fCenPos;
    fWireUpStr = o.fWireUpStr;
    fWireDownStr = o.fWireDownStr;
    fTubeType = o.fTubeType;
    fConnected = o.fConnected;
    fRotMat = o.fRotMat;
    fRadIn = o.fRadIn;
    fRadOut = o.fRadOut;
    fSectorID = o.fSectorID;
    fLayerID = o.fLayerID;
    fNeighborings = o.fNeighborings;
    fLayerLimit = o.fLayerLimit;
    fSectorLimit = o.fSectorLimit;
    fShortTube = o.fShortTube;
    //fCenPosition = o.fCenPosition;
    //fRotationMatrix = o.fRotationMatrix;
    //fRadIn = o.fRadIn;
    //fRadOut = o.fRadOut;
    //fSectorID = o.fSectorID;
    //fLayerID = o.fLayerID;
    //fNeighborings = o.fNeighborings;
    //fLayerLimit = o.fLayerLimit;
    //fSectorLimit = o.fSectorLimit;
    return *this;
  };

  // accessors
  TVector3 GetPosition()       { return fCenPos;}
  TVector3 GetWireUpStr()      { return fWireUpStr;}
  TVector3 GetWireDownStr()    { return fWireDownStr;}
  Double_t GetRadIn()          { return fRadIn; }
  Double_t GetRadOut()         { return fRadOut; }
  Double_t GetHalfLength()     { return fHalfLen; } 
  Double_t GetSkewAngle()      { return fSkewAng; } 
  
  Int_t GetTubeID()            { return fTubeID; }  
  Int_t GetLayerID()           { return fLayerID; }
  Int_t GetSectorID()          { return fSectorID; }
  Int_t GetIndex()             { return fIndex; }
  Int_t GetTubeType()          { return fTubeType; }
  Int_t GetConnected()         { return fConnected; }
  
  TArrayI GetNeighborings()    { return fNeighborings; }
  Int_t GetNeighboring(int i)  { return fNeighborings.At(i); }
  bool IsLayerLimit()          { return fLayerLimit; }
  int IsSectorLimit()          { return fSectorLimit; }
  int GetSectorLimit()         { return fSectorLimit; }
  bool IsShortTube()           { return fShortTube; }

  TVector3 GetWireDirection()  { return TVector3(fRotMat[0][2], fRotMat[1][2], fRotMat[2][2]); }
  bool IsSkew()                { return (fRotMat[2][2] < 1); }
  bool IsParallel()            { return !IsSkew(); }
  TMatrixT<Double_t> GetRotationMatrix()    { return fRotMat; }

  TVector3 GetPositionAtZ(double z);      // compute position of wire at certain z 
  Double_t GetDistance(PndSttTube *tube); // get distance to another tube
  Bool_t IsNeighboring(int tubeID);

  PndSttTubeParameters *GetTubeParameters() { return fTubeParms; }

  //PndStt2GeoHandler* GetSttGeoHandler() { return fSttGeoH; }      // get pointer to geohandler 

  // modifiers
  void SetSectorID(int id)        { fSectorID = id; }
  void SetLayerID(int id)         { fLayerID = id; }
  void SetIndex(int idx)          { fIndex = idx; }
  void SetTubeType(int ttype)     { fTubeType = ttype; }
  void SetConnected(int conn)     { fConnected = conn; }
  void SetShortTube(bool st=true) { fShortTube = st; }
  void SetSkewAngle(double ang)   { fSkewAng = ang; }
  
  void SetNeighborings(TArrayI neighborings) { fNeighborings = neighborings; }
  void SetSectorLimitFlag(Int_t flag) { fSectorLimit = flag; }
  void SetLayerLimitFlag(Bool_t flag = kTRUE) { fLayerLimit = flag; }
  
  //void SetSttGeoHandler(PndStt2GeoHandler *gh) { fSttGeoH = gh; } // set pointer to geohandler 

  // methods
  TString InfoStr();
  void Print() { std::cout<<InfoStr()<<std::endl; }

  //TVector3 GetPosition();
  //TMatrixT<Double_t> GetRotationMatrix();
  //Double_t GetRadIn();
  //Double_t GetRadOut();
  //Double_t GetHalfLength();
  //TVector3 GetWireDirection();
  //PndSttTubeParameters *GetTubeParameters();
  //Int_t GetTubeID();
  //void SetSectorID(int id);
  //void SetLayerID(int id);
  //int GetLayerID();
  //int GetSectorID();
  //void SetNeighborings(TArrayI neighborings);
  //TArrayI GetNeighborings();
  //Int_t GetNeighboring(int i);
  //Double_t GetDistance(PndSttTube *tube);
  //Bool_t IsNeighboring(int tubeID);

  //void SetSectorLimitFlag(Int_t flag) { fSectorLimit = flag; }
  //void SetLayerLimitFlag(Bool_t flag = kTRUE) { fLayerLimit = flag; }
  //bool IsLayerLimit() { return fLayerLimit; }
  //int IsSectorLimit() { return fSectorLimit; }

  //bool IsParallel() { return (GetWireDirection().Z() == 1); }

  //bool IsSkew() { return !(GetWireDirection().Z() == 1); }

 private:
  PndSttTubeParameters *fTubeParms;  // tube parameters (tube ID, tube half length)
  Int_t fTubeID;                     // tube ID
  Double_t fHalfLen;                 // half length of tube
  Double_t fSkewAng;                 // stereo angle
  TVector3 fCenPos;                  // center position (global coord)
  TVector3 fWireUpStr;               // wire coordinate up stream
  TVector3 fWireDownStr;             // wire coordinate down stream
  Int_t fTubeType;                   // tube type: right = -1, axial = 0, left = +1
  Int_t fConnected;                  // connected tube (only for short tubes)
  TMatrixT<double> fRotMat;          // rotation matrix
  Double_t fRadIn, fRadOut;          // inner/outer radius of tube
  Int_t fSectorID, fLayerID, fIndex; // sector ID, layer ID, index of tube in layer
  TArrayI fNeighborings;             // tube IDs of all neighbours
  Bool_t fLayerLimit;                // ??
  Int_t fSectorLimit;                // ??
  Bool_t fShortTube;                 // is this a short straw

  //TVector3 fCenPosition;
  //TMatrixT<double> fRotationMatrix;
  //Double_t fRadIn, fRadOut;
  //Int_t fSectorID, fLayerID;
  //TArrayI fNeighborings;
  //Bool_t fLayerLimit;
  //Int_t fSectorLimit;

  ClassDef(PndSttTube, 1);
};

#endif
