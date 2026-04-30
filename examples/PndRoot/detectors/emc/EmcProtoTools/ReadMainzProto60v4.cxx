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

/**************************************

  chammann@hiskp.uni-bonn.de
  code to parse data files taken from
  peter.drexler@exp2.physik.uni-giessen.de

 **************************************/

#ifdef GNUPRAGMA
#pragma interface
#endif

#include "ReadMainzProto60v4.h"
#include <stdlib.h>
#include <stdio.h>
#include "TNtuple.h"

// #include "NTECEvent.h"

void ReadMainzProto60v4::GetEnergies(Double_t *energies)
{
  for (int n = 0; n < 60; n++) {
    energies[n] = LG_CAL[n];
  }
}

void ReadMainzProto60v4::GetTimes(Double_t *times)
{
  for (int n = 0; n < 60; n++) {
    times[n] = TIME[n];
  }
}

void ReadMainzProto60v4::GetADCValues(Double_t *ADCs)
{
  for (int n = 0; n < 60; n++) {
    ADCs[n] = LG[n];
  }
}
void ReadMainzProto60v4::GetTaggerTimes(Double_t *taggertimes)
{
  for (int n = 0; n < 16; n++) {
    taggertimes[n] = TAGGER_CAL[n];
  }
}

void ReadMainzProto60v4::GetTaggerEnergies(Double_t *taggerEnergies)
{
  taggerEnergies[0] = 1441.06;
  taggerEnergies[1] = 1356.89;
  taggerEnergies[2] = 1257.31;
  taggerEnergies[3] = 1156.56;
  taggerEnergies[4] = 1057.70;
  taggerEnergies[5] = 956.16;
  taggerEnergies[6] = 857.98;
  taggerEnergies[7] = 756.58;
  taggerEnergies[8] = 657.34;
  taggerEnergies[9] = 556.97;
  taggerEnergies[10] = 456.26;
  taggerEnergies[11] = 355.88;
  taggerEnergies[12] = 256.41;
  taggerEnergies[13] = 158.31;
  taggerEnergies[14] = 0.0;
  taggerEnergies[15] = 187.99;
}

void ReadMainzProto60v4::read_energy_factor(const char *filename)
{
  FILE *fin;
  //    char infilename[100];
  char instr[100];
  int ch;
  double ped, sigped, thr, mu, gmev, fr_mev, thr_mev;

  //    sprintf(infilename, "%s.thr_mev", filename);
  printf("filename read in: %s\n", filename);

  fin = fopen(filename, "rt");
  if (fin == nullptr) {
    printf("No file \"%s\" found!\n", filename);
    for (int n = 0; n < 60; n++) {
      lg_factor[n] = lg_ped[n] = 0;
    }
  } else {
    fgets(instr, 100, fin); // root filename
    printf("filename read in: %s\n", instr);
    fgets(instr, 100, fin); // Ch Ped Sigped Thr Mu GMeV FR_MeV Thr_MeV
    printf("header read in: %s\n", instr);
    for (int i = 0; i < 60; i++) {
      fgets(instr, 100, fin); // values
      if (strlen(instr) < 2)
        fgets(instr, 100, fin); // empty line

      sscanf(instr, "%i%lf%lf%lf%lf%lf%lf%lf", &ch, &ped, &sigped, &thr, &mu, &gmev, &fr_mev, &thr_mev);
      printf("line %i: %s -> ch: %i, factor: %f, ped: %f\n", i, instr, ch, gmev, ped);
      if (i + 1 == ch) {
        lg_factor[i] = gmev;
        lg_ped[i] = ped;
      } else {
        printf("Wrong channel in %s detected!\n", filename);
        exit(0);
      }
    }
  }
}

void ReadMainzProto60v4::PrintEvent()
{
  printf("Printing Event Number %li\n", NumberOfEvents);
  printf("ADCValues:\n");
  for (Int_t row = 0; row < 6; row++) {
    for (Int_t col = 0; col < 10; col++) {
      printf("% 8.3f ", LG[row * 10 + col]);
    }
    printf("\n");
  }

  printf("Energies:\n");
  for (Int_t row = 0; row < 6; row++) {
    for (Int_t col = 0; col < 10; col++) {
      printf("% 8.3f ", LG_CAL[row * 10 + col]);
    }
    printf("\n");
  }
  printf("Times:\n");
  for (Int_t row = 0; row < 6; row++) {
    for (Int_t col = 0; col < 10; col++) {
      printf("% 8.3f ", TIME_CAL[row * 10 + col]);
    }
    printf("\n");
  }

  printf("TaggerTimes:\n");
  for (Int_t row = 0; row < 2; row++) {
    for (Int_t col = 0; col < 8; col++) {
      printf("% 8.3f ", TAGGER_CAL[row * 8 + col]);
    }
    printf("\n");
  }
  printf("TaggerEnergies:\n");
  Double_t tagger_energies[16];
  GetTaggerEnergies(tagger_energies);
  for (Int_t row = 0; row < 2; row++) {
    for (Int_t col = 0; col < 8; col++) {
      printf("% 8.3f ", tagger_energies[row * 8 + col]);
    }
    printf("\n");
  }
}

ReadMainzProto60v4::ReadMainzProto60v4()
{
  IsInit = kFALSE;
  printf("do datafile given, nothing to do\n");
}

ReadMainzProto60v4::ReadMainzProto60v4(const char *datafilename, const char *calibrationfilename)
{
  IsInit = kFALSE;
  ctime_factor = 0.180;
  RANGE = 3860;
  BINS = 3860;
  CAL_RANGE = 1000;
  CAL_BINS = 4000;
  central = 34;
  strcpy(versionsstr, "data_all_4.1.3");
  NumberOfEvents = 0;
  ResetToFirstEvent = kFALSE;

  // unsigned int rv; //[R.K. 01/2017] unused variable?
  // unsigned int noe=0; //[R.K. 01/2017] unused variable?
  // int how_often=1000; //[R.K. 01/2017] unused variable?

  // int tagged_crystal=35; //[R.K. 01/2017] unused variable?
  in = fopen(datafilename, "rb");
  if (in == nullptr) {
    printf("can't read datafile %s !!!\n", datafilename);
    return;
  }
  read_energy_factor(calibrationfilename);

  IsInit = kTRUE;
}

ReadMainzProto60v4::~ReadMainzProto60v4()
{
  fclose(in);
}

Long_t ReadMainzProto60v4::ReadNextEvent()
{
  if (read_one_event() != 0) {
    NumberOfEvents++;
    convert_60();
    calibrate_60();
    return NumberOfEvents;
  } else {
    return -1;
  }
}

Long_t ReadMainzProto60v4::GetNumberOfEvents()
{
  Long_t RetVal = 0;
  Reset();
  while (ReadNextEvent() != -1)
    ;
  RetVal = NumberOfEvents;
  Reset();
  return RetVal;
}

void ReadMainzProto60v4::Reset()
{
  NumberOfEvents = 0;
  ResetToFirstEvent = kTRUE;
}

void ReadMainzProto60v4::calibrate_60(void)
{
  for (int n = 0; n < 60; n++) {
    TIME_CAL[n] = ((TIME[n]) * ctime_factor);
    LG_CAL[n] = (((LG[n]) - lg_ped[n]) * lg_factor[n]);
  }
  for (int n = 0; n < 16; n++) {
    TAGGER_CAL[n] = ((TAGGER[n]) * ctime_factor);
  }
  TP_CAL = ((TP)*ctime_factor);
  MU_CAL = ((MU)*ctime_factor);
  VETO_CAL = ((VETO)*ctime_factor);
}

void ReadMainzProto60v4::convert_60(void)
{
  for (int n = 0; n < 32; n++) {
    TIME[n] = rawdata[n][0];
  }
  for (int n = 32; n < 60; n++) {
    TIME[n] = rawdata[n - 32][1];
  }
  for (int n = 0; n < 16; n++) {
    LG[n] = rawdata[n][2];
  }
  for (int n = 16; n < 32; n++) {
    LG[n] = rawdata[n - 16][3];
  }
  for (int n = 32; n < 48; n++) {
    LG[n] = rawdata[n - 32][4];
  }
  for (int n = 48; n < 60; n++) {
    LG[n] = rawdata[n - 48][5];
  }
  for (int n = 0; n < 16; n++) {
    TAGGER[n] = rawdata[n][6];
  }

  TIME[central] = rawdata[28][1]; // correct position for Proto60 Mainz
  LG[central] = rawdata[12][5];   // correct position for Proto60 Mainz
  TP = rawdata[31][1];
  MU = rawdata[30][1];
  VETO = rawdata[29][1];
}

unsigned int ReadMainzProto60v4::read_one_event()
{
  static unsigned int dataword = 0xffffffff; // current data word
  static unsigned int events = 0;            // event counter
  static unsigned int no_of_ev = 0;          // number of words in file
  static char ververgl[100];

  //  unsigned int rv;  // return value
  int wie_oft = 200000;                                      // how often status report
  unsigned int dec_geo, dec_tow, dec_noch, dec_data, dec_ch; // decoded event informations
  int n;                                                     // counting variables
  int board_no;

  memset(noe_of_board, 0, sizeof(noe_of_board)); //
  memset(rawdata, 0, sizeof(rawdata));           //

  if (dataword == 0xffffffff || ResetToFirstEvent) { // init readout
    ResetToFirstEvent = kFALSE;
    events = 0;
    fseek(in, 0, 2);          // setze Zeiger auf Ende der Datei
    no_of_ev = ftell(in) / 4; // Dateilaenge/4 pro header, data word, trail
    fseek(in, 0, 0);          // setze Zeiger auf Anfang der Datei

    n = 0;
    do {
      fread(&ververgl[n], 1, 1, in);
      n++;
    } while (ververgl[n - 1] != 0);

    printf("The version of the readout was: %s (expected: %s)\n\n", ververgl, versionsstr);
    fread(&no_of_boards, sizeof(no_of_boards), 1, in);
    printf("%i boards were read out.\n", no_of_boards);
    fread(what, sizeof(what), 1, in);
    fread(geos, sizeof(geos), 1, in);
    for (n = 0; n < no_of_boards; n++) {
      printf("Board no %i is a %i\n", n, what[n]);
      geo_to_bnr[geos[n]] = n;
    }
  }
  for (board_no = 0; board_no < no_of_boards; board_no++) {
    if (what[board_no] < 80) {
      /* Header  **********************/
      if (0 == fread((char *)&dataword, sizeof(dataword), 1, in))
        return (0);
      events++;
      if (events % wie_oft == 0)
        printf("%i of %i read in (%3.0f%%)\n", events, no_of_ev, ((double)events) * 100 / ((double)no_of_ev));
      dec_tow = ((dataword >> 24) & 0x7); // decode type of dataword
      if (dec_tow != 2) {                 // not a header
        //      do{
        printf("H");
        //        fread((char*) &dataword, sizeof(dataword), 1, in);
        //        events++; if(events%wie_oft==0) printf("%i von %i read in (%3.0f%%)\n", events, no_of_[1]ev, ((double) events)*100/ ((double)no_of_ev));
        //        dec_tow=((dataword>>24) & 0x7);
        //      }while(dec_tow!=2);  // repeat until first header
      }
      dec_geo = (dataword >> 27);          // decode board number
      dec_noch = ((dataword >> 8) & 0x3f); // decode no of events in board
      /* Header  **********************/

      /* Data Words  **********************/
      for (n = 0; n < (int)dec_noch; n++) {
        if (0 == fread((char *)&dataword, sizeof(dataword), 1, in))
          return (0);
        events++;
        if (events % wie_oft == 0)
          printf("%i von %i read in (%3.0f%%)\n", events, no_of_ev, ((double)events) * 100 / ((double)no_of_ev));
        dec_tow = ((dataword >> 24) & 0x7);      // decode type of dataword
        dec_data = (dataword & 0xfff);           // decode actual data
        if ((what[board_no] % 10) == 1) {        // ch on board max 32
          dec_ch = ((dataword >> 16) & 0x3f);    // decode channel number
        } else if ((what[board_no] % 10) == 0) { // ch on board max 16, N version of board
          dec_ch = ((dataword >> 17) & 0x1f);    // decode dataword
        } else
          printf("ERROR in WHAT!!!\n\n\n");
        if (dec_tow == 0 && (dec_geo == (dataword >> 27))) { // tow and geo is ok
          //	  rawdata[dec_ch][ geo_to_bnr[dec_geo] ] = dec_data;
          if (what[board_no] == 30 || what[board_no] == 31) // tdc
            if (((dataword >> 14) & 0x1) == 0)
              dec_data = 0; // valid data?
          rawdata[dec_ch][board_no] = dec_data;
        } else
          printf("D");
      }
      /* Data Words  **********************/

      /* Trail  **********************/
      if (0 == fread((char *)&dataword, sizeof(dataword), 1, in))
        return (0);
      events++;
      if (events % wie_oft == 0)
        printf("%i of %i read in (%3.0f%%)\n", events, no_of_ev, ((double)events) * 100 / ((double)no_of_ev));
      dec_tow = ((dataword >> 24) & 0x7); // decode type of word
      if (dec_tow != 4)
        printf("T");                                             // if it wasn't trail
      noe_of_board[geo_to_bnr[dec_geo]] = ((dataword)&0xffffff); //

      /* Trail **********************/

    } else if (what[board_no] == 90) {
      for (n = 0; n < 7; n++) { // header, counter 1-4, direct data in, trail
        if (0 == fread((char *)&dataword, sizeof(dataword), 1, in))
          return (0);
        events++;
        if (events % wie_oft == 0)
          printf("%i of %i read in (%3.0f%%)\n", events, no_of_ev, ((double)events) * 100 / ((double)no_of_ev));
        rawdata[n][board_no] = dec_data;
      }
    }
  }
  //  rv=events; // of there is more data, return number of read in events
  return 1;
} // read_one_event
