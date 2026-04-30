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
// -----                PndDskTrackPoint header file                   -----
// -----                Created 27/02/09  by P. Koch                   -----
// -------------------------------------------------------------------------

#ifndef PNDDSKTRACKPOINT_H
#define PNDDSKTRACKPOINT_H

#include "FairMCPoint.h"

class PndDskTrackPoint : public FairMCPoint {

 public:
  /** Default constructor **/
  PndDskTrackPoint();

  /** Standard constructor with arguments
   *@param trackID         Index of MCTrack
   *@param detectorID      Detector ID where the first hit is produced
   *@param position        Position of Cerenkov when created [cm]
   *@param momentum        Momentum of Cerenkov when created  [eV]
   *@param time            Time since event start when created [ns]
   *@param length          Track length since creation [cm]
   *@param eLoss           Energy deposit [GeV]
   **/
  PndDskTrackPoint(Int_t trackID, Int_t detectorID, TVector3 position, TVector3 momentum, Double_t time, Double_t length, Double_t eLoss);

  /** Destructor **/
  virtual ~PndDskTrackPoint();

  /** Virtual method Print
   **
   ** Screen output of Cerenkov
   **/
  virtual void Print(const Option_t *opt) const;

  ClassDef(PndDskTrackPoint, 1)
};

#endif // PNDDSKTRACKPOINT_H
