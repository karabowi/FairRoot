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

#include "PndSttTube.h"

#include <iostream>
#include <math.h>
#include <vector>

#include "TVector3.h"

using std::min;
using std::vector;

PndSttTube::PndSttTube()
  : fTubeParms(new PndSttTubeParameters()), 
  fTubeID(0), fHalfLen(0.0), fSkewAng(0.0), fTubeType(0), fConnected(0), fRotMat(TMatrixT<double>(3, 3)), fRadIn(-1), fRadOut(-1), fSectorID(-1), fLayerID(-1), fIndex(-1), fLayerLimit(false), fSectorLimit(0), fShortTube(false)
  //fCenPosition(TVector3(0, 0, 0)), fRotationMatrix(TMatrixT<double>(3, 3)), fRadIn(-1), fRadOut(-1), fSectorID(-1), fLayerID(-1),
    //fLayerLimit(kFALSE), fSectorLimit(kFALSE)
{
  fRotMat.ResizeTo(3, 3);
  fRotMat[0][0] = -1.;
  fRotMat[0][1] = -1.;
  fRotMat[0][2] = -1.;
  
  fRotMat[1][0] = -1.;
  fRotMat[1][1] = -1.;
  fRotMat[1][2] = -1.;
  
  fRotMat[2][0] = -1.;
  fRotMat[2][1] = -1.;
  fRotMat[2][2] = -1.;
}

//-------------------------------------------
// copy constructor
//-------------------------------------------
PndSttTube::PndSttTube(PndSttTube &tube)
  : TObject(tube), fTubeParms(tube.GetTubeParameters()), fTubeID(tube.GetTubeID()), fHalfLen(tube.GetHalfLength()), fSkewAng(tube.GetSkewAngle()), fCenPos(tube.GetPosition()), fWireUpStr(tube.GetWireUpStr()), 
    fWireDownStr(tube.GetWireDownStr()), fTubeType(tube.GetTubeType()), fConnected(tube.GetConnected()), fRotMat(TMatrixT<double>(3, 3)), fRadIn(tube.GetRadIn()), fRadOut(tube.GetRadOut()), 
    fSectorID(tube.GetSectorID()), fLayerID(tube.GetLayerID()), fIndex(tube.GetIndex()), fLayerLimit(tube.IsLayerLimit()), fSectorLimit(tube.IsSectorLimit()), fShortTube(tube.IsShortTube())
  //: TObject(tube), fTubeParms(tube.GetTubeParameters()), fCenPosition(tube.GetPosition()), fRotationMatrix(TMatrixT<double>(3, 3)), fRadIn(tube.GetRadIn()),
    //fRadOut(tube.GetRadOut()), fSectorID(tube.GetSectorID()), fLayerID(tube.GetLayerID()), fLayerLimit(tube.IsLayerLimit()), fSectorLimit(tube.IsSectorLimit())
{
  fRotMat.ResizeTo(3, 3);
  fRotMat  = tube.GetRotationMatrix();
}

//-------------------------------------------
// standard constructor (STT1)
//-------------------------------------------
PndSttTube::PndSttTube(PndSttTubeParameters *parms, Double_t x, Double_t y, Double_t z, Double_t r11, Double_t r12, Double_t r13, Double_t r21, Double_t r22, Double_t r23,
                       Double_t r31, Double_t r32, Double_t r33, Double_t radin, Double_t radout) :
    //fTubeParms(parms), fCenPos(TVector3(x, y, z)), fRotationMatrix(TMatrixT<double>(3, 3)), fRadIn(radin), fRadOut(radout), fSectorID(-1), fLayerID(-1), fLayerLimit(kFALSE),
    //fSectorLimit(kFALSE)
  fTubeParms(parms), fTubeID(0), fHalfLen(0.0), fSkewAng(0.0), fCenPos(TVector3(x, y, z)), fWireUpStr(), fWireDownStr(), fTubeType(0), fConnected(0), fRadIn(radin), fRadOut(radout),
  fSectorID(-1), fLayerID(-1), fIndex(-1), fLayerLimit(kFALSE), fSectorLimit(0), fShortTube(false)
{
  fTubeID  = fTubeParms->GetTubeID();
  fHalfLen = fTubeParms->GetHalfLength();

  fRotMat.ResizeTo(3, 3);
  fRotMat[0][0] = r11;
  fRotMat[0][1] = r12;
  fRotMat[0][2] = r13;

  fRotMat[1][0] = r21;
  fRotMat[1][1] = r22;
  fRotMat[1][2] = r23;

  fRotMat[2][0] = r31;
  fRotMat[2][1] = r32;
  fRotMat[2][2] = r33;
  
  fWireUpStr   = fCenPos - fHalfLen * TVector3(r13, r23, r33);
  fWireDownStr = fCenPos + fHalfLen * TVector3(r13, r23, r33);
}

//-------------------------------------------
// standard constructor (STT2)
//-------------------------------------------
PndSttTube::PndSttTube(int id, TVector3 cnt, TVector3 wup, TVector3 wdown, const Double_t *rotmat, Double_t halflen, Double_t rin, Double_t rout) :
  fTubeParms(new PndSttTubeParameters(id, halflen)), fTubeID(id), fHalfLen(halflen), fSkewAng(0.0), fCenPos(cnt), fWireUpStr(wup), fWireDownStr(wdown), fTubeType(0), fConnected(0), fRadIn(rin), fRadOut(rout),
  fSectorID(-1), fLayerID(-1), fIndex(-1), fLayerLimit(kFALSE), fSectorLimit(0), fShortTube(false)
{
  fRotMat.ResizeTo(3, 3);
  fRotMat[0][0] = rotmat[0];
  fRotMat[0][1] = rotmat[1];
  fRotMat[0][2] = rotmat[2];

  fRotMat[1][0] = rotmat[3];
  fRotMat[1][1] = rotmat[4];
  fRotMat[1][2] = rotmat[5];

  fRotMat[2][0] = rotmat[6];
  fRotMat[2][1] = rotmat[7];
  fRotMat[2][2] = rotmat[8];
}


//-------------------------------------------
// check if neighbour
//-------------------------------------------
Bool_t PndSttTube::IsNeighboring(int tubeID)
{
  for (int itube = 0; itube < fNeighborings.GetSize(); itube++) {
    if (GetNeighboring(itube) == tubeID)
      return kTRUE;
  }
  return kFALSE;
}


//-------------------------------------------
// compute distance to other tube
//-------------------------------------------
Double_t PndSttTube::GetDistance(PndSttTube *tube)
{
  if (tube==0) return -1.0;
  
  // ##### wire end points upstream (p0, p1) and downstream (p0e, p1e) and direction vectors (d0, d1)
  TVector3 p0 = fWireUpStr,           p0e = fWireDownStr,           d0 = p0e - p0; // wire 0
  TVector3 p1 = tube->GetWireUpStr(), p1e = tube->GetWireDownStr(), d1 = p1e - p1; // wire 1
  
  double dist = -1.0;
  
  // ---------------------
  // ##### parallel wires
  // ---------------------
  // ##### |d0 x d1| = 0
  if (d0.Cross(d1).Mag()<0.1) {
    // ##### determine line parameter (p1e -> p0 + s0*d0 or p0e -> p1 + s1*d1) of projection of downstream ends to other wire
    double s0 = d1*(p0e-p1)/d1.Mag2();
    double s1 = d0*(p1e-p0)/d0.Mag2();
    
    // ##### if either 0 < s_0,1 < 1 (wires 'overlap'), use distance formular for parallel lines: |(p0-p1) x d1| / |d1|
    if ((s0>=0 && s0<=1) || (s1>=0 && s1<=1)) 
      dist = ((p0-p1).Cross(d1)).Mag()/d1.Mag();
    // ##### else take minimum of distances |p0 - p1e| and |p1 - p0e| as distance
    else 
      dist = min((p0-p1e).Mag(), (p1-p0e).Mag());
  }
  // -------------------------
  // ##### non-parallel wires 
  // -------------------------
  else
  { 
    // ##### determine parameters s,t for perpendicular intersecting line between wire 0 and wire 1
    // ##### formulas based on equations (p0 + s*d0 - p1 -t*d1)*d0 = 0 and (p0 + s*d0 - p1 -t*d1)*d1 = 0 (conn. perpendicular to both lines)
    double s = (p0-p1)*((d0*d1)*d1-d1.Mag2()*d0)/(d0.Mag2()*d1.Mag2()-pow(d0*d1,2));
    double t = (s*d0*d1+(p0-p1)*d1)/d1.Mag2();
    
    // ##### dx intersects both wires (i.e.: 0 < s < 1 AND 0 < t < 1)
    if (s>=0 && s<=1 && t>=0 && t<=1) {   
      TVector3 xp0 = p0 + s * d0; // intersection point along wire 0
      TVector3 xp1 = p1 + t * d1; // intersection point along wire 1
      TVector3 dx  = xp1 - xp0;   // connection line between these points
      
      dist = dx.Mag();            // distance is length of connection
    }
    // ##### dx does not intersect both wires 
    // ##### => compute all projection of the end points to the respective other wire
    // #####    and the distances of the end points themselves and take the minimum
    else {
      // ##### distances of the end points
      vector<double> vdist = { (p0-p1).Mag(), (p0-p1e).Mag(), (p1-p0e).Mag(), (p0e-p1e).Mag() };

      double s0 = d0*(p1e-p0)/d0.Mag2();  // projection of p1e to wire 0
      double s1 = d0*(p1-p0)/d0.Mag2();   // projection of p1  to wire 0
      double t0 = d1*(p0e-p1)/d1.Mag2();  // projection of p0e to wire 1
      double t1 = d1*(p0-p1)/d1.Mag2();   // projection of p0  to wire 1
      
      // ##### only consider projections inside wire range
      if (s0>=0 && s0<=1) 
        vdist.push_back(((p1e-p0).Cross(d0)).Mag()/d0.Mag());
      if (s1>=0 && s1<=1) 
        vdist.push_back(((p1-p0).Cross(d0)).Mag()/d0.Mag());
      if (t0>=0 && t0<=1) 
        vdist.push_back(((p0e-p1).Cross(d1)).Mag()/d1.Mag());
      if (t1>=0 && t1<=1) 
        vdist.push_back(((p0-p1).Cross(d1)).Mag()/d1.Mag());
      
      dist = TMath::MinElement(vdist.size(), &vdist[0]);
    }
  }
  
  return dist;
}
//// CHECK this can be written in a better way
//Double_t PndSttTube::GetDistance(PndSttTube *tube)
//{

  //TVector3 extr1 = fCenPosition - GetHalfLength() * TVector3(fRotationMatrix[0][2], fRotationMatrix[1][2], fRotationMatrix[2][2]);
  //TVector3 extr2 = fCenPosition + GetHalfLength() * TVector3(fRotationMatrix[0][2], fRotationMatrix[1][2], fRotationMatrix[2][2]);
  //TVector3 tubeextr1 = tube->GetPosition() - tube->GetHalfLength() * tube->GetWireDirection();
  //TVector3 tubeextr2 = tube->GetPosition() + tube->GetHalfLength() * tube->GetWireDirection();

  //double distance = -1;
  //if (TVector3(fRotationMatrix[0][2], fRotationMatrix[1][2], fRotationMatrix[2][2]).Cross(tube->GetWireDirection()).Mag() == 0) {
    //distance = ((fCenPosition - tubeextr1).Cross(fCenPosition - tubeextr2)).Mag() / (tubeextr2 - tubeextr1).Mag();
    //return distance;
  //} else {
    //distance = fabs((tubeextr1 - extr1).Dot((extr2 - extr1).Cross(tubeextr2 - tubeextr1))) / ((extr2 - extr1).Cross(tubeextr2 - tubeextr1)).Mag();

    //double s = (((extr2 - extr1).Dot(tubeextr2 - tubeextr1)) * ((tubeextr2 - tubeextr1).Dot(extr1 - tubeextr1)) -
                //((tubeextr2 - tubeextr1).Dot(tubeextr2 - tubeextr1)) * ((extr2 - extr1).Dot(extr1 - tubeextr1))) /
               //(((extr2 - extr1).Dot(extr2 - extr1)) * ((tubeextr2 - tubeextr1).Dot(tubeextr2 - tubeextr1)) -
                //((extr2 - extr1).Dot(tubeextr2 - tubeextr1)) * ((extr2 - extr1).Dot(tubeextr2 - tubeextr1)));
    //double t = (((extr2 - extr1).Dot(extr2 - extr1)) * ((tubeextr2 - tubeextr1).Dot(extr1 - tubeextr1)) -
                //((extr2 - extr1).Dot(tubeextr2 - tubeextr1)) * ((extr2 - extr1).Dot(extr1 - tubeextr1))) /
               //(((extr2 - extr1).Dot(extr2 - extr1)) * ((tubeextr2 - tubeextr1).Dot(tubeextr2 - tubeextr1)) -
                //((extr2 - extr1).Dot(tubeextr2 - tubeextr1)) * ((extr2 - extr1).Dot(tubeextr2 - tubeextr1)));

    //TVector3 point1 = extr1 + s * (extr2 - extr1);
    //TVector3 point2 = tubeextr1 + t * (tubeextr2 - tubeextr1);

    ////    std::cout << "GET DISTANCE " << distance << " " << (point1 - point2).Mag() << std::endl;
    ////     point1.Print();
    ////     point2.Print();

    //if (point1.Z() < extr1.Z() || point1.Z() > extr2.Z() || point2.Z() < tubeextr1.Z() || point2.Z() > tubeextr2.Z()) {
      ////   std::cout << "point outside limits" << std::endl;
      //distance = (extr1 - tubeextr1).Mag() < (extr2 - tubeextr2).Mag() ? (extr1 - tubeextr1).Mag() : (extr2 - tubeextr2).Mag();
    //}
    ////  std::cout << "DISTANCE " << distance << std::endl;
    //return distance;
  //}
//}



//-------------------------------------------
// Generated Info String
//-------------------------------------------
TString PndSttTube::InfoStr()
{
  TVector3 p1 = fWireUpStr, p2 = fWireDownStr;
  TString info = Form("id=%4d : segment=%d : layer=%2d : index=%2d : p1=%10.5f,%10.5f,%10.5f : p2=%10.5f,%10.5f,%10.5f : type=%+1d : connect=%4d : neighb(%2d)=",
          fTubeID, fSectorID, fLayerID, fIndex, p1.X(), p1.Y(), p1.Z(), p2.X(), p2.Y(), p2.Z(), fTubeType, fConnected, fNeighborings.GetSize());
  
  for (int inei=0; inei<fNeighborings.GetSize(); ++inei) 
    info += Form("%d ", fNeighborings[inei]);
    
  return info;
}

////-------------------------------------------
////  Compute DOCA of two tube wires
////-------------------------------------------
//Double_t PndSttTube::GetDistance(PndStt2Tube *tube)
//{
  //if (nullptr!=fSttGeoH)
    //return fSttGeoH->GetDistance(fTubeID, tube->GetTubeID());
  
  //return -1.0;
//}

//-------------------------------------------
//  Compute position of wire at certain z 
//-------------------------------------------
TVector3 PndSttTube::GetPositionAtZ(double z)
{
  // ##### compute fraction of wire at z
  double s = (z - fWireUpStr.Z())/(2*fHalfLen);
  
  // ##### z not on wire
  if (s<0 || s>1) 
    return TVector3(0,0,0);
  else
   return fWireUpStr + s*(fWireDownStr-fWireUpStr);
}

ClassImp(PndSttTube)
