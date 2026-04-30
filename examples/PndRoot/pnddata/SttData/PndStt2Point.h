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
 
#ifndef PNDSTT2POINT_H
#define PNDSTT2POINT_H

#include "TObject.h"
#include "TVector3.h"
#include "PndMCPoint.h"

class PndStt2Point : public PndMCPoint {

 public:
  /** Default constructor **/
  PndStt2Point();

  /** Constructor with arguments
   *@param trackID       Index of MCTrack
   *@param tubeID        Number of tube
   *@param posIn         Coordinates at entrance to active volume [cm]
   *@param posOut        Coordinates at exit of active volume [cm]
   *@param momIn         Momentum of track at entrance [GeV]
   *@param momOut        Momentum of track at exit [GeV]
   *@param tof           Time since event start [ns]
   *@param length        Track length since creation [cm]
   *@param eLoss         Energy deposit [GeV]
   **/
  PndStt2Point(Int_t trackID, Int_t detID, Int_t tubeID, TVector3 pos, TVector3 posOut, TVector3 momIn, TVector3 momOut, Double_t tof, Double_t length, Double_t eLoss);

  /** Copy constructor **/
  PndStt2Point(const PndStt2Point &point) : PndMCPoint(point), fTubeID(point.fTubeID) { *this = point; };

  /** Destructor **/
  virtual ~PndStt2Point();

  /** Accessors **/
  Int_t GetTubeID() { return fTubeID; }

  /** Modifiers **/
  void SetTubeID(Int_t tubeid) { fTubeID = tubeid; }
  virtual void SetTrackID(Int_t id) { FairMCPoint::SetTrackID(id); }

  /** Output to screen **/
  virtual void Print(const Option_t *opt) const;
  
 protected:
  Int_t    fTubeID; 

  ClassDef(PndStt2Point, 1)
};


#endif
