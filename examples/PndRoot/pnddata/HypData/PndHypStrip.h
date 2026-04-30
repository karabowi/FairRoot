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

#ifndef _PNDHYPSTRIP_HH
#define _PNDHYPSTRIP_HH

#include "PndHypHit.h"
#include "FairGeoVector.h"
#include <iostream>
#include <vector>

class PndHypStrip {
 public:
  PndHypStrip() : fnumber(-1), fcharge(-1.0)
  //			_fe = -1;
  {
  }

  //		  PndHypStrip(const std::string& detName, int fe, int nr, double charge){
  PndHypStrip(Int_t nr, Double_t charge)
    : //		  	_detName = detName;
      //			_fe = fe;
      fnumber(nr), fcharge(charge)
  //			_index = 0;
  //			_detID = 0;
  //			_trackID = 0;
  {
  }

  void SetIndex(Int_t nr) { fnumber = nr; }
  void SetCharge(Double_t charge) { fcharge = charge; }
  //		  void SetDetName(const std::string& detName) {_detName = detName;}
  //		  void SetFE (int fe) {_fe = fe;}
  //		  void SetIndex (int fe) {_fe = fe;}

  int GetIndex() const { return fnumber; }
  double GetCharge() const { return fcharge; }
  //		  std::string GetDetName() const {return _detName;}
  //		  int  GetFE() const {return _fe;}

  friend std::ostream &operator<<(std::ostream &out, const PndHypStrip &strip)
  {
    /*
    out << "Detector: " << strip.GetDetName() << " FE: " <<
    strip.GetFE() << " Strip Nr.: " << strip.GetChannel() << " "
        << " Charge: " << strip.GetCharge();
    */
    out << " Strip Nr.: " << strip.GetIndex() << " "
        << " Charge: " << strip.GetCharge();

    return out;
  }

 private:
  //		  int _fe;
  Int_t fnumber;
  Double_t fcharge;
  //		  int _index;
  //		  int _detID;
  //		  int _trackID;

  //		  std::string _detName;
};

#endif // fMVDSTRIP_HH
