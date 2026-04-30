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

#include "PndDiscDigitizedHit.h"

ClassImp(PndDiscDigitizedHit)

  PndDiscDigitizedHit::PndDiscDigitizedHit()
  : detector_id(0), readout_id(0), sensor_id(0), pixel_number(0), dead_time_entity(0), pixel_pos(0.), tdc_time(0.), digit_classifier(0)
{
}

PndDiscDigitizedHit::PndDiscDigitizedHit(FairLink const &fairln_mc_point, const Int_t &detector_id_, const Int_t &readout_id_, const Int_t &sensor_id_,
                                         const Int_t &dead_time_entity_, const Int_t &pixel_number_, const Double_t &pixel_pos_, const Double_t &tdc_time_,
                                         const Double_t &panda_timestamp, const Int_t classifier_)
  : FairTimeStamp(panda_timestamp), detector_id(detector_id_), readout_id(readout_id_), sensor_id(sensor_id_), pixel_number(pixel_number_), dead_time_entity(dead_time_entity_),
    pixel_pos(pixel_pos_), tdc_time(tdc_time_), digit_classifier(classifier_)
{
  AddLink(fairln_mc_point);
}

void PndDiscDigitizedHit::Print(std::ostream &out)
{
  out << "PndDiscDigitizedHit in\n"
      << "  detector          " << detector_id << std::endl
      << "  ROE               " << readout_id << std::endl
      << "  sensor            " << sensor_id << std::endl
      << "  dead_time_entity  " << dead_time_entity << std::endl
      << "  pixel             " << pixel_number << std::endl
      << "  with tdc time     " << tdc_time << std::endl
      << "  at fairtime       " << fTimeStamp << std::endl
      << "  classified as     " << digit_classifier << std::endl;
}
