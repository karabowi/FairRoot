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

//*-- AUTHOR Denis Bertini
//*-- modified: 19/06/2005 by Denis Bertini
//*-- created : 18/06/2005

/////////////////////////////////////////////////////////////
// PndFtsDigiPar
//
// Container class for Digitisation parameters
//
/////////////////////////////////////////////////////////////

#include "PndFtsDigiPar.h"
#include "FairRuntimeDb.h"
#include "FairParIo.h"
#include "FairDetParIo.h"
#include "FairLogger.h"
#include "TClass.h"
#include <iostream>
#include <iomanip>

ClassImp(PndFtsDigiPar)

  PndFtsDigiPar::PndFtsDigiPar(const char *name, const char *title, const char *context)
  : FairParSet(name, title, context)
{
  // constructor does nothing yet
  detName = "Fts";
}

PndFtsDigiPar::~PndFtsDigiPar()
{
  // destructor
}

Bool_t PndFtsDigiPar::init(FairParIo *inp)
{
  // intitializes the container from an input
  LOG(info) << " PndFtsDigiPar::init ";

  FairDetParIo *input = inp->getDetParIo("PndFtsParIo");
  LOG(info) << " PndFtsDigiPar::init " << input;

  if (input)
    return (input->init(this));
  return kFALSE;
}

Int_t PndFtsDigiPar::write(FairParIo *output)
{
  // writes the container to an output
  FairDetParIo *out = output->getDetParIo("PndFtsParIo");
  if (out)
    return out->write(this);
  return -1;
}

void PndFtsDigiPar::clear()
{
  // clears the container
  status = kFALSE;
  resetInputVersions();
}

void PndFtsDigiPar::printParam()
{
  // prints the calibration parameters
  // to be done
}

void PndFtsDigiPar::readline(const char *, Int_t *, std::fstream *) // buf set f //[R.K.03/2017] unused variable(s)
{
}

void PndFtsDigiPar::readline(const char *, Int_t *) // buf  set//[R.K.03/2017] unused variable(s)
{
}

void PndFtsDigiPar::putAsciiHeader(TString &) // header //[R.K.03/2017] unused variable(s)
{
}

Bool_t PndFtsDigiPar::writeline(char *, Int_t, Int_t) // buf mod strip //[R.K.03/2017] unused variable(s)
{
  return kTRUE;
}

void PndFtsDigiPar::putParams(FairParamList *) // list //[R.K.03/2017] unused variable(s)
{
  // At present the container is empty
}

Bool_t PndFtsDigiPar::getParams(FairParamList *) // list //[R.K.03/2017] unused variable(s)
{
  // At present the container is empty
  return kTRUE;
}
