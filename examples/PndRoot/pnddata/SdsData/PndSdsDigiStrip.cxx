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

#include "PndSdsDigiStrip.h"
#include "PndSdsDigi.h"

PndSdsDigiStrip::PndSdsDigiStrip() : fChannel(-1) {}

PndSdsDigiStrip::PndSdsDigiStrip(std::vector<Int_t> index, Int_t detID, Int_t sensorID, Int_t fe, Int_t chan, Double_t charge, Double_t timestamp)
  : PndSdsDigi(index, detID, sensorID, fe, charge, timestamp), fChannel(chan)
{
}

PndSdsDigiStrip::PndSdsDigiStrip(Int_t index, Int_t detID, Int_t sensorID, Int_t fe, Int_t chan, Double_t charge, Double_t timestamp)
  : PndSdsDigi(index, detID, sensorID, fe, charge, timestamp), fChannel(chan)
{
  fChannel = chan;
}

// const void PndSdsDigiStrip::Print(){
//      std::cout << *((PndSdsStrip*)this) << std::endl;
// }

// Bool_t const HasNeighbour(const PndSdsDigiStrip& d2)
// {
//   return true; // Dummy so far
// }

Bool_t PndSdsDigiStrip::operator==(const PndSdsDigiStrip &d2) const
{
  return (fDetID == d2.GetDetID() && fSensorID == d2.GetSensorID() && fFE == d2.GetFE() && fChannel == d2.GetChannel()
          //   && fCharge==d2.GetCharge()
          //           && fMCID==d2.GetMCID()
          //   && fTimeStamp==d2.GetTimeStamp()
  );
}

bool PndSdsDigiStrip::equal(FairTimeStamp *data)
{
  PndSdsDigiStrip *myDigi = dynamic_cast<PndSdsDigiStrip *>(data);
  // const PndSdsDigiStrip* const oldData = reinterpret_cast <const PndSdsDigiStrip* const> (this);
  //  std::cout << "PndSdsDigiStrip::equal: New " << *myDigi<< std::endl;
  //  std::cout << "PndSdsDigiStrip::equal: old " << *this << std::endl;
  //  std::cout << "PndSdsDigiStrip::equal: PrintOld ";
  //  Print();
  std::cout << std::endl << std::endl;
  if (myDigi != nullptr) {
    if (fSensorID == myDigi->GetSensorID())
      if (fFE == myDigi->GetFE())
        if (fChannel == myDigi->GetChannel())
          return true;
  }
  return false;
}

std::ostream &operator<<(std::ostream &out, const PndSdsDigiStrip &digi)
{
  return digi.Print(out);
}

ClassImp(PndSdsDigiStrip);
