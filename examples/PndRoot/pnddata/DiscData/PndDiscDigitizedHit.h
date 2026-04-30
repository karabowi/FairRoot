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

//-------------------------------------------------------------------------
// Author:      Oliver Merle (Oliver.Merle@exp2.physik.uni-giessen.de)
// Changes:     Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Date:        30.11.2015
// Description: Digitizing hits from Monte Carlo simulations
//-------------------------------------------------------------------------

#ifndef PNDDISCDIGITIZEDHIT_HH
#define PNDDISCDIGITIZEDHIT_HH

#include "FairTimeStamp.h"
#include "FairLogger.h"

#include "TObject.h"
#include "TString.h"
#include "PndDetectorList.h"

#include <iostream>

class PndDiscDigitizedHit : public FairTimeStamp {

 public:
  PndDiscDigitizedHit();
  PndDiscDigitizedHit(FairLink const &fairln_mc_point, const Int_t &detector_id_, const Int_t &readout_id_, const Int_t &sensor_id_, const Int_t &dead_time_entity_,
                      const Int_t &pixel_number_, const Double_t &pixel_pos_, const Double_t &tdc_time, const Double_t &panda_timestamp, const Int_t classifier);

  virtual ~PndDiscDigitizedHit() {}

  // getters ...
  const Int_t &GetDetectorID() const { return detector_id; }
  const Int_t &GetReadoutID() const { return readout_id; }
  const Int_t &GetSensorID() const { return sensor_id; }
  const Int_t &GetPixelNumber() const { return pixel_number; }
  const Int_t &GetDeadTimeEntity() const { return dead_time_entity; }
  const Double_t &GetTdcTime() const { return tdc_time; }
  const Double_t &GetPixelPosition() const { return pixel_pos; }
  const Int_t &GetClassifier() const { return digit_classifier; }

  virtual bool SortOrder(const FairTimeStamp *s2) const
  {
    PndDiscDigitizedHit &digit = *(PndDiscDigitizedHit *)s2;
    if (detector_id < digit.GetDetectorID())
      return true;
    else if (detector_id > digit.GetDetectorID())
      return false;
    if (readout_id < digit.GetReadoutID())
      return true;
    else if (readout_id > digit.GetReadoutID())
      return false;
    if (dead_time_entity < digit.GetDeadTimeEntity())
      return true;
    else if (dead_time_entity > digit.GetDeadTimeEntity())
      return false;
    // if      (pixel_number < digit.GetPixelNumber()) return true;
    // else if (pixel_number > digit.GetPixelNumber()) return false;
    return false; // equal
  }

  // the less operator to sort by 'deadtime entity':
  virtual bool operator<(const PndDiscDigitizedHit &digit) const
  {
    if (detector_id < digit.GetDetectorID())
      return true;
    else if (detector_id > digit.GetDetectorID())
      return false;
    if (readout_id < digit.GetReadoutID())
      return true;
    else if (readout_id > digit.GetReadoutID())
      return false;
    if (dead_time_entity < digit.GetDeadTimeEntity())
      return true;
    else if (dead_time_entity > digit.GetDeadTimeEntity())
      return false;
    // if      (pixel_number < digit.GetPixelNumber()) return true;
    // else if (pixel_number > digit.GetPixelNumber()) return false;
    return false; // equal
  }

  // is something using this operator??
  virtual bool operator==(const PndDiscDigitizedHit &digit) const
  {
    return ((detector_id == digit.GetDetectorID()) && (readout_id == digit.GetReadoutID()) && (dead_time_entity == digit.GetDeadTimeEntity()));
  }

  // needed by FairWriteoutBuffer
  virtual bool equal(FairTimeStamp *data)
  {
    PndDiscDigitizedHit *digit = (PndDiscDigitizedHit *)data;
    return (*this) == (*digit);
  }

  virtual void Print(std::ostream &out = std::cout);

 protected:
  Int_t detector_id;      // detector id (quarter that has fired)
  Int_t readout_id;       // id number of the readout element
  Int_t sensor_id;        // id number of the sensor on an readout element (to handle more than one sensor)
  Int_t pixel_number;     // pixel pos
  Int_t dead_time_entity; // used to group hits by dead_time_entity (used in operators for time based simulation)
  Double_t pixel_pos;     // binned pixel pos ...

  Double_t tdc_time;      // time determined by tdc
  Int_t digit_classifier; // 0 - typical hit, 1 - dark count, 2 - from mixed background

  ClassDef(PndDiscDigitizedHit, 4)
};

#endif // PNDDISCDIGITIZEDHIT_HH
