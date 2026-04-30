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

/****************************************************
 * ReadMainzProto60 class to read data files from
 * the proto60 tests at mami
 *
 * author: chammann@hiskp.uni-bonn.de
 * date: 6.1.2010
 *
 * *************************************************/

#ifndef ReadMainzProto60_h
#define ReadMainzProto60_h
#include "TObject.h"

class ReadMainzProto60 {
 public:
  virtual void GetEnergies(Double_t *energies) = 0;             // energies has to be at least 60 long
  virtual void GetTimes(Double_t *times) = 0;                   // times has to be at least 60 long
  virtual void GetADCValues(Double_t *ADCs) = 0;                // ADCs has to be at least 60 long
  virtual void GetTaggerTimes(Double_t *taggertimes) = 0;       // taggertimes has to be at least 16 long
  virtual void GetTaggerEnergies(Double_t *taggerEnergies) = 0; // taggertimes has to be at least 16 long

  virtual void Reset() = 0;

  virtual void PrintEvent() = 0;

  virtual Long_t ReadNextEvent() = 0; // returns total number of events read, -1 if no event could be read (end of file)
  virtual Long_t GetNumberOfEvents() = 0;

  ReadMainzProto60(){};
  virtual ~ReadMainzProto60(){};

 private:
  ClassDef(ReadMainzProto60, 1)
};
#endif
