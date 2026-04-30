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
// -----                      PndGemPoint header file                  -----
// -----                  Created 27/10/08  by R. Kliemt               -----
// -------------------------------------------------------------------------

/**  PndGemMCPoint.h
 *@author R.Kliemt
 *
 * Interception of MC track with a MVD detetcor. Holds in addition
 * to the base class the coordinates and momentum at the exit from
 * the active volume.
 **/

#ifndef PNDGEMMCPOINT_H
#define PNDGEMMCPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "PndMCPoint.h"
#include "TString.h"

#include <stdio.h>
#include <iostream>

using namespace std;

class PndGemMCPoint : public PndMCPoint {
  friend std::ostream &operator<<(std::ostream &out, const PndGemMCPoint &point)
  {
    out << "-I- PndGemMCPoint: PndGem Point for track " << point.GetTrackID() << " in detector " << point.GetDetectorID() << " with sensorID " << point.GetSensorId() << std::endl;
    out << "    Position in  (" << point.GetX() << ", " << point.GetY() << ", " << point.GetZ() << ") cm" << std::endl;
    out << "    Position out (" << point.GetXOut() << ", " << point.GetYOut() << ", " << point.GetZOut() << ") cm" << std::endl;
    out << "    Momentum (" << point.GetPx() << ", " << point.GetPy() << ", " << point.GetPz() << ") GeV" << std::endl;
    out << "    Time " << point.GetTime() << " ns,  Length " << point.GetLength() << " cm,  Energy loss " << point.GetEnergyLoss() * 1.0e06 << " keV" << std::endl;
    return out;
  }

 public:
  /** Default constructor **/
  PndGemMCPoint();

  /** Constructor with arguments
   *@param trackID       Index of MCTrack
   *@param detID         Detector ID
   *@param sensID        Sensor ID
   *@param posIn         Coordinates of ingoing point[cm]
   *@param posOut	 Coordinates of outgoing point[cm]
   *@param momIn         Momentum of track at entrance [GeV]
   *@param momOut        Momentum of track at exit [GeV]
   *@param tof           Time since event start [ns]
   *@param length        Track length since creation [cm]
   *@param eLoss         Energy deposit [GeV]
   **/
  PndGemMCPoint(Int_t trackID, Int_t detID, Int_t sensID, TVector3 posIn, TVector3 posOut, TVector3 momIn, TVector3 momOut, Double_t tof, Double_t length, Double_t eLoss);

  /** Copy constructor **/
  PndGemMCPoint(const PndGemMCPoint &point) : PndMCPoint(point) { *this = point; };

  /** Destructor **/
  virtual ~PndGemMCPoint();

  /** Accessors **/
  Int_t GetSensorId() const { return fSensorId; }

  /** Modifiers **/
  void SetSensorId(Int_t sensId) { fSensorId = sensId; };

  /** Output to screen **/
  virtual void Print(const Option_t *opt = nullptr) const;

 protected:
  Int_t fSensorId = 0;

  ClassDef(PndGemMCPoint, 2);
};

#endif
