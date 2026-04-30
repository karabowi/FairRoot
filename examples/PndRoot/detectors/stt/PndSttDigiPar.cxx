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
// PndSttDigiPar
//
// Container class for Digitisation parameters
//
/////////////////////////////////////////////////////////////

#include "PndSttDigiPar.h"
#include "FairRuntimeDb.h"
#include "FairParIo.h"
#include "FairDetParIo.h"
#include "FairLogger.h"
#include "TClass.h"
#include <iostream>
#include <iomanip>

ClassImp(PndSttDigiPar)

  PndSttDigiPar::PndSttDigiPar(const char *name, const char *title, const char *context)
  : FairParSet(name, title, context)
{
  // constructor does nothing yet
  detName = "Stt";
}

PndSttDigiPar::~PndSttDigiPar()
{
  // destructor
}

Bool_t PndSttDigiPar::init(FairParIo *inp)
{
  // intitializes the container from an input
  LOG(info) << " PndSttDigiPar::init ";

  FairDetParIo *input = inp->getDetParIo("PndSttParIo");
  LOG(info) << " PndSttDigiPar::init " << input;

  if (input)
    return (input->init(this));
  return kFALSE;
}

Int_t PndSttDigiPar::write(FairParIo *output)
{
  // writes the container to an output
  FairDetParIo *out = output->getDetParIo("PndSttParIo");
  if (out)
    return out->write(this);
  return -1;
}

void PndSttDigiPar::clear()
{
  // clears the container
  status = kFALSE;
  resetInputVersions();
}

void PndSttDigiPar::printParam()
{
  // prints the calibration parameters
  // to be done
}

// void PndSttDigiPar::readline(const char *buf, Int_t *set, std::fstream *f)
void PndSttDigiPar::readline(const char *, Int_t *, std::fstream *) {}

// void PndSttDigiPar::readline(const char *buf, Int_t *set)
void PndSttDigiPar::readline(const char *, Int_t *) {}

// void PndSttDigiPar::putAsciiHeader(TString& header)
void PndSttDigiPar::putAsciiHeader(TString &) {}

// Bool_t PndSttDigiPar::writeline(char *buf, Int_t mod, Int_t strip)
Bool_t PndSttDigiPar::writeline(char *, Int_t, Int_t)
{
  return kTRUE;
}
