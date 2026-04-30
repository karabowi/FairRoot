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

#include "ReadMainzProto60v6.h"
#include <stdlib.h>
#include <stdio.h>
#include "TNtuple.h"

// #include "NTECEvent.h"

void ReadMainzProto60v6::GetEnergies(Double_t *energies)
{
  for (int n = 0; n < 60; n++) {
    energies[n] = LG_CAL[n];
  }
}

void ReadMainzProto60v6::GetTimes(Double_t *times)
{
  for (int n = 0; n < 60; n++) {
    times[n] = TIME[n];
  }
}

void ReadMainzProto60v6::GetADCValues(Double_t *ADCs)
{
  for (int n = 0; n < 60; n++) {
    ADCs[n] = LG[n];
  }
}
void ReadMainzProto60v6::GetTaggerTimes(Double_t *taggertimes)
{
  for (int n = 0; n < 16; n++) {
    taggertimes[n] = TAGGER_CAL[n];
  }
}

void ReadMainzProto60v6::GetTaggerEnergies(Double_t *taggerEnergies)
{
  /*	// new energy calibration

    taggerEnergies[0]=692.27;
    taggerEnergies[1]=658.17;
    taggerEnergies[2]=609.87;
    taggerEnergies[3]=587.47;
    taggerEnergies[4]=518.84;
    taggerEnergies[5]=453.01;
    taggerEnergies[6]=396.68;
    taggerEnergies[7]=348.89;
    taggerEnergies[8]=278.88;
    taggerEnergies[9]=208.78;
    taggerEnergies[10]=160.74;
    taggerEnergies[11]=115.54;
    taggerEnergies[12]=101.37;
    taggerEnergies[13]=91.96;
    taggerEnergies[14]=68.55;
    taggerEnergies[15]=59.24;
  */
  // old energy calibration
  taggerEnergies[0] = 685.58;
  taggerEnergies[1] = 650.81;
  taggerEnergies[2] = 601.77;
  taggerEnergies[3] = 579.10;
  taggerEnergies[4] = 509.88;
  taggerEnergies[5] = 443.77;
  taggerEnergies[6] = 387.38;
  taggerEnergies[7] = 339.67;
  taggerEnergies[8] = 269.95;
  taggerEnergies[9] = 200.33;
  taggerEnergies[10] = 152.72;
  taggerEnergies[11] = 107.98;
  taggerEnergies[12] = 93.97;
  taggerEnergies[13] = 84.67;
  taggerEnergies[14] = 61.54;
  taggerEnergies[15] = 52.34;
}

void ReadMainzProto60v6::read_energy_factor(const char *filename)
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

void ReadMainzProto60v6::PrintEvent()
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

ReadMainzProto60v6::ReadMainzProto60v6()
{
  IsInit = kFALSE;
  printf("do datafile given, nothing to do\n");
}

ReadMainzProto60v6::ReadMainzProto60v6(const char *datafilename, const char *calibrationfilename)
{
  IsInit = kFALSE;
  ctime_factor = 0.180;
  central = 34;
  strcpy(versionsstr, "data_all_6.0.0");
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

ReadMainzProto60v6::~ReadMainzProto60v6()
{
  fclose(in);
}

Long_t ReadMainzProto60v6::ReadNextEvent()
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

Long_t ReadMainzProto60v6::GetNumberOfEvents()
{
  Long_t RetVal = 0;
  Reset();
  while (ReadNextEvent() != -1)
    ;
  RetVal = NumberOfEvents;
  Reset();
  return RetVal;
}

void ReadMainzProto60v6::Reset()
{
  NumberOfEvents = 0;
  ResetToFirstEvent = kTRUE;
}

void ReadMainzProto60v6::calibrate_60(void)
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

void ReadMainzProto60v6::convert_60(void)
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

  for (int n = 0; n < 8; n++) {
    TIME_VETO[n] = rawdata[19 + n][7];
    if (n % 2 == 0) // even
      LG_VETO[n] = rawdata[15 + (n / 2)][7];
    else
      LG_VETO[n] = rawdata[12 + ((n - 1) / 2)][7];
  }

  TIME[central] = rawdata[28][1]; // correct position for Proto60 Mainz
  LG[central] = rawdata[12][5];   // correct position for Proto60 Mainz
  TP = rawdata[31][1];
  MU = rawdata[30][1];
  VETO = rawdata[29][1];
}

unsigned int ReadMainzProto60v6::read_one_event()
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

    n = 0;
    do {
      fread(&starttime[n], 1, 1, in);
      n++;
    } while (starttime[n - 1] != 0);
    printf("The readout was started: %s\n", starttime);

    n = 0;
    do {
      fread(&stoptime[n], 1, 1, in);
      n++;
    } while (stoptime[n - 1] != 0);
    printf("The readout was stopped: %s\n\n", stoptime);

    fread(&no_of_boards, sizeof(no_of_boards), 1, in);
    printf("%i boards were read out.\n", no_of_boards);
    while (no_of_boards == 0)
      ;
    fread(what, sizeof(what), 1, in);
    fread(geos, sizeof(geos), 1, in);
    for (n = 0; n < no_of_boards; n++) {
      printf("Board no %i is a %i (geo %i)\n", n, what[n], geos[n]);
      if (geos[n] > 43)
        geos[n] = 43;
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
          if (what[board_no] == 30 || what[board_no] == 31)  // tdc
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
