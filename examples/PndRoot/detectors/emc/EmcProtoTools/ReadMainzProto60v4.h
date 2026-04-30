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
 * ReadMainzProto60v4 class to read data files from
 * the proto60 tests at mami
 *
 * author: chammann@hiskp.uni-bonn.de
 * date: 6.1.2010
 *
 * *************************************************/

#ifndef ReadMainzProto60v4_h
#define ReadMainzProto60v4_h
#include "TObject.h"
#include "ReadMainzProto60.h"

class ReadMainzProto60v4 : public ReadMainzProto60 {
 public:
  void GetEnergies(Double_t *energies);             // energies has to be at least 60 long
  void GetTimes(Double_t *times);                   // times has to be at least 60 long
  void GetADCValues(Double_t *ADCs);                // ADCs has to be at least 60 long
  void GetTaggerTimes(Double_t *taggertimes);       // taggertimes has to be at least 16 long
  void GetTaggerEnergies(Double_t *taggerEnergies); // taggertimes has to be at least 16 long

  void Reset();

  void PrintEvent();

  Long_t ReadNextEvent(); // returns total number of events read, -1 if no event could be read (end of file)
  Long_t GetNumberOfEvents();

  ReadMainzProto60v4();
  ReadMainzProto60v4(const char *datafilename, const char *calibrationfilename);
  ~ReadMainzProto60v4();

 private:
  FILE *in;
  unsigned int read_one_event();
  Bool_t ResetToFirstEvent;
  void convert_60(void);
  void calibrate_60(void);
  void read_energy_factor(const char *filename);
  Long_t NumberOfEvents;

  double LG[60];
  double TIME[60];
  double TAGGER[16];
  double TP;
  double MU;
  double VETO;

  double LG_CAL[60];
  double TIME_CAL[60];
  double TAGGER_CAL[16];
  double TP_CAL;
  double MU_CAL;
  double VETO_CAL;

  unsigned int rawdata[32][16];
  unsigned int noe_of_board[16];
  int no_of_boards, what[16], geos[16];
  unsigned int geo_to_bnr[32];
  char versionsstr[15];

  int RANGE, BINS;
  int CAL_RANGE, CAL_BINS;
  int central;

  double lg_factor[60];
  double lg_ped[60];
  double ctime_factor;
  Bool_t IsInit;

  ClassDef(ReadMainzProto60v4, 1)
};
#endif
