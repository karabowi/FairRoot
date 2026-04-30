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

#ifndef PNDRICHPOINT_H
#define PNDRICHPOINT_H 1

#include "FairMCPoint.h"

#include "TObject.h"
#include "TVector3.h"

class PndRichPoint : public FairMCPoint {

 public:
  /** Default constructor **/
  PndRichPoint();

  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param pos      Ccoordinates at entrance to active volume [cm]
   *@param mom      Momentum of track at entrance [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  PndRichPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss);

  /** Destructor **/
  virtual ~PndRichPoint();

  /** Output to screen **/
  virtual void Print(const Option_t *opt) const;

 private:
  /** Copy constructor **/
  PndRichPoint(const PndRichPoint &point);
  PndRichPoint operator=(const PndRichPoint &point);

  ClassDef(PndRichPoint, 1)
};

#endif
