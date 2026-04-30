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

//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Track candidate. Basically a list of hit indices.
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    	TUM     (original author)
//	Tobias Baldauf		TUD	(adapted for SSD Pid)
//
//-----------------------------------------------------------

#ifndef PNDSSDPIDCAND_H
#define PNDSSDPIDCAND_H

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <vector>
#include <map>

// Collaborating Class Declarations --

class PndSdsPidCand : public TObject {
 public:
  // Constructors/Destructors ---------
  PndSdsPidCand() : fssdhits(0), fdE(), fdx(), fmomentum(), flikelihood() {}

  // CopyConstructor
  PndSdsPidCand(PndSdsPidCand &point) : TObject(point), fssdhits(point.fssdhits), fdE(point.fdE), fdx(point.fdx), fmomentum(point.fmomentum), flikelihood(point.flikelihood)
  {
    *this = point;
  };

  // operators

  // Setting -----------------------
  void SetLikelihood(int lundId, double likelihood);
  void AddSsdHit(double dE, double dx, double p);

  // Getting -----------------------
  double GetSsdHitdE(int ssdhit) const;
  double GetSsdHitdx(int ssdhit) const;
  double GetSsdHitMomentum(int ssdhit) const;
  int GetSsdHits() const;
  double GetLikelihood(int lundId);

 private:
  // Private Data Members ------------
  int fssdhits;
  std::vector<double> fdE;
  std::vector<double> fdx;
  std::vector<double> fmomentum;
  std::map<int, double> flikelihood;

  // public:
  ClassDef(PndSdsPidCand, 1);
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
