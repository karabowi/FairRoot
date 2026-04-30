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
//	Tobias Baldauf		TUD	(adapted for MVD Pid)
//	Alicia Sanchez	     UniMainz   (adapted for HYP Pid)
//
//-----------------------------------------------------------

#ifndef PNDHYPPIDCAND_H
#define PNDHYPPIDCAND_H

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <vector>
#include <map>

// Collaborating Class Declarations --

class PndHypPidCand : public TObject {
 public:
  // Constructors/Destructors ---------
  PndHypPidCand() : fhyphits(0), fdE(), fdx(), fmomentum(), flikelihood() {}

  // CopyConstructor
  PndHypPidCand(PndHypPidCand &point) : TObject(point), fhyphits(point.fhyphits), fdE(point.fdE), fdx(point.fdx), fmomentum(point.fmomentum), flikelihood(point.flikelihood)
  {
    *this = point;
  };

  // operators

  // Setting -----------------------
  void SetLikelihood(int lundId, double likelihood);
  void AddHypHit(double dE, double dx, double p);

  // Getting -----------------------
  double GetHypHitdE(int hyphit) const;
  double GetHypHitdx(int hyphit) const;
  double GetHypHitMomentum(int hyphit) const;
  int GetHypHits() const;
  double GetLikelihood(int lundId);

 private:
  // Private Data Members ------------
  int fhyphits;
  std::vector<double> fdE;
  std::vector<double> fdx;
  std::vector<double> fmomentum;
  std::map<int, double> flikelihood;

  // public:
  ClassDef(PndHypPidCand, 1);
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
