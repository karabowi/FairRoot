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

/**  PndSdsMCPoint.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 *
 * Interception of MC track with a SSD detetcor. Holds in addition
 * to the base class the coordinates and momentum at the exit from
 * the active volume.
 **/

#ifndef PNDSDSMCPOINT_H
#define PNDSDSMCPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "PndMCPoint.h"
#include "TString.h"
#include "PndDetectorList.h"

#include <stdio.h>
#include <iostream>

using namespace std;

class PndSdsMCPoint : public PndMCPoint {
  friend std::ostream &operator<<(std::ostream &out, const PndSdsMCPoint &point)
  {
    out << "-I- PndSdsMCPoint: PndSds Point for track " << point.GetTrackID() << " in detector " << point.GetDetectorID() << " with sensor " << point.GetSensorID() << std::endl;
    out << "    Position in  (" << point.GetX() << ", " << point.GetY() << ", " << point.GetZ() << ") cm" << std::endl;
    out << "    Position out (" << point.GetXOut() << ", " << point.GetYOut() << ", " << point.GetZOut() << ") cm" << std::endl;
    out << "    Momentum (" << point.GetPx() << ", " << point.GetPy() << ", " << point.GetPz() << ") GeV" << std::endl;
    out << "    Time " << point.GetTime() << " ns,  Length " << point.GetLength() << " cm,  Energy loss " << point.GetEnergyLoss() * 1.0e06 << " keV" << std::endl;
    return out;
  }

 public:
  /** Default constructor **/
  PndSdsMCPoint();

  /** Constructor with arguments
   *@param trackID       Index of MCTrack
   *@param detID         Detector ID
   *@param sensorID		   Full nameId of the detector, see PndSdsGeoHandling
   *@param posIn         Coordinates of ingoing point[cm]
   *@param posOut	 Coordinates of outgoing point[cm]
   *@param momIn         Momentum of track at entrance [GeV]
   *@param momOut        Momentum of track at exit [GeV]
   *@param tof           Time since event start [ns]
   *@param length        Track length since creation [cm]
   *@param eLoss         Energy deposit [GeV]
   **/
  PndSdsMCPoint(Int_t trackID, Int_t detID, Int_t sensorID, TVector3 posIn, TVector3 posOut, TVector3 momIn, TVector3 momOut, Double_t tof, Double_t length, Double_t eLoss);

  /** Copy constructor **/
  PndSdsMCPoint(const PndSdsMCPoint &point) : PndMCPoint(point), fSensorID(point.fSensorID) { *this = point; };

  /** Destructor **/
  virtual ~PndSdsMCPoint();

  /** Accessors **/
  Int_t GetSensorID() const { return fSensorID; }

  /** Modifiers **/

  void SetSensorID(Int_t sensorID) { fSensorID = sensorID; }
  virtual void SetTrackID(Int_t id) { FairMCPoint::SetTrackID(id); };

  /** Output to screen **/
  virtual void Print(const Option_t *opt = nullptr) const;

 protected:
  Int_t fSensorID;

  ClassDef(PndSdsMCPoint, 7);
};

#endif
