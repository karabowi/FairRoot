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

#include "BSEmcDataConcentratorLookUp.h"

#include "TString.h"

#include "fairlogger/Logger.h"

#include "BSEmcDetectorID.h"

Int_t BSEmcDataConcentratorLookUp::GetNumberOfDCsFor(TString t_detector) const
{
  t_detector.ToLower();
  if (t_detector.Contains("fwendcap")) {
    return GetNumberOfDCsFor(BSEmcDetectorType::FWENDCAP);
  } else if (t_detector.Contains("bwendcap")) {
    return GetNumberOfDCsFor(BSEmcDetectorType::BWENDCAP);
  } else if (t_detector.Contains("barrel")) {
    return GetNumberOfDCsFor(BSEmcDetectorType::BARREL);
  } else if (t_detector.Contains("shashlik") || t_detector.Contains("shashlyk")) {
    return GetNumberOfDCsFor(BSEmcDetectorType::SHASHLYK);
  }
  return 0;
}

Int_t BSEmcDataConcentratorLookUp::GetNumberOfDCsFor(BSEmcDetectorType t_detector) const
{
  // TODO: These return values are arbitrary and not the actual number of DCs involved
  if (t_detector == BSEmcDetectorType::FWENDCAP) {
    return 43;
  } else if (t_detector == BSEmcDetectorType::BWENDCAP) {
    return 8;
  } else if (t_detector == BSEmcDetectorType::BARREL) {
    return 107;
  } else if (t_detector == BSEmcDetectorType::SHASHLYK) {
    return 12;
  }
  return -1;
}

Int_t BSEmcDataConcentratorLookUp::GetDCNumber(Int_t t_detectorId) const
{

  // --------------------- (experimental) MAP DIGI INTO VIRTUAL DATA CONCENTRATORS (DC) -------------------------

  // ----- The current map divides the EMC into rectangular sections of 120 units of XPad,YPad, thus ------------
  // ----- each section contains up to 120 crystals (but usually less, especially in the FWEndcap) --------------
  BSEmcDetectorID detectorID{t_detectorId};
  Int_t module = detectorID.GetModule();
  if (module == 1 || module == 2) {
    module = 2; // "redefinition" of barrel, easier for the switch
  }

  Int_t xpad = detectorID.GetXPad();
  Int_t ypad = detectorID.GetYPad();

  switch (module) {
  case 2: // barrel (0<x<160, 0<y<72)
          //---------->TOP ROW
    if (xpad >= 0 && xpad < 10 && ypad <= 72 && ypad > 60) {
      return 0;
    } // put digi in DC0
    if (xpad >= 10 && xpad < 20 && ypad <= 72 && ypad > 60) {
      return 1;
    } // put digi in DC1
    if (xpad >= 20 && xpad < 30 && ypad <= 72 && ypad > 60) {
      return 2;
    } // put digi in DC2
    if (xpad >= 30 && xpad < 40 && ypad <= 72 && ypad > 60) {
      return 3;
    } // put digi in DC3
    if (xpad >= 40 && xpad < 50 && ypad <= 72 && ypad > 60) {
      return 4;
    } // etc
    if (xpad >= 50 && xpad < 60 && ypad <= 72 && ypad > 60) {
      return 5;
    }
    if (xpad >= 60 && xpad < 70 && ypad <= 72 && ypad > 60) {
      return 6;
    }
    if (xpad >= 70 && xpad < 80 && ypad <= 72 && ypad > 60) {
      return 7;
    }
    if (xpad >= 80 && xpad < 90 && ypad <= 72 && ypad > 60) {
      return 8;
    }
    if (xpad >= 90 && xpad < 100 && ypad <= 72 && ypad > 60) {
      return 9;
    }
    if (xpad >= 100 && xpad < 110 && ypad <= 72 && ypad > 60) {
      return 10;
    }
    if (xpad >= 110 && xpad < 120 && ypad <= 72 && ypad > 60) {
      return 11;
    }
    if (xpad >= 120 && xpad < 130 && ypad <= 72 && ypad > 60) {
      return 12;
    }
    if (xpad >= 130 && xpad < 140 && ypad <= 72 && ypad > 60) {
      return 13;
    }
    if (xpad >= 140 && xpad < 150 && ypad <= 72 && ypad > 60) {
      return 14;
    }
    if (xpad >= 150 && ypad <= 72 && ypad > 60) {
      return 15;
    }
    //---------->2nd ROW
    if (xpad >= 0 && xpad < 10 && ypad <= 60 && ypad > 48) {
      return 16;
    }
    if (xpad >= 10 && xpad < 20 && ypad <= 60 && ypad > 48) {
      return 17;
    }
    if (xpad >= 20 && xpad < 30 && ypad <= 60 && ypad > 48) {
      return 18;
    }
    if (xpad >= 30 && xpad < 40 && ypad <= 60 && ypad > 48) {
      return 19;
    }
    if (xpad >= 40 && xpad < 50 && ypad <= 60 && ypad > 48) {
      return 20;
    }
    if (xpad >= 50 && xpad < 60 && ypad <= 60 && ypad > 48) {
      return 21;
    }
    if (xpad >= 60 && xpad < 70 && ypad <= 60 && ypad > 48) {
      return 22;
    }
    if (xpad >= 70 && xpad < 80 && ypad <= 60 && ypad > 48) {
      return 23;
    }
    if (xpad >= 80 && xpad < 90 && ypad <= 60 && ypad > 48) {
      return 24;
    }
    if (xpad >= 90 && xpad < 100 && ypad <= 60 && ypad > 48) {
      return 25;
    }
    if (xpad >= 100 && xpad < 110 && ypad <= 60 && ypad > 48) {
      return 26;
    }
    if (xpad >= 110 && xpad < 120 && ypad <= 60 && ypad > 48) {
      return 27;
    }
    if (xpad >= 120 && xpad < 130 && ypad <= 60 && ypad > 48) {
      return 28;
    }
    if (xpad >= 130 && xpad < 140 && ypad <= 60 && ypad > 48) {
      return 29;
    }
    if (xpad >= 140 && xpad < 150 && ypad <= 60 && ypad > 48) {
      return 30;
    }
    if (xpad >= 150 && ypad <= 60 && ypad > 48) {
      return 31;
    }
    //---------->3rd ROW
    if (xpad >= 0 && xpad < 10 && ypad <= 48 && ypad > 36) {
      return 32;
    }
    if (xpad >= 10 && xpad < 20 && ypad <= 48 && ypad > 36) {
      return 33;
    }
    if (xpad >= 20 && xpad < 30 && ypad <= 48 && ypad > 36) {
      return 34;
    }
    if (xpad >= 30 && xpad < 40 && ypad <= 48 && ypad > 36) {
      return 35;
    }
    if (xpad >= 40 && xpad < 50 && ypad <= 48 && ypad > 36) {
      return 36;
    }
    if (xpad >= 50 && xpad < 60 && ypad <= 48 && ypad > 36) {
      return 37;
    }
    if (xpad >= 60 && xpad < 70 && ypad <= 48 && ypad > 36) {
      return 38;
    }
    if (xpad >= 70 && xpad < 80 && ypad <= 48 && ypad > 36) {
      return 39;
    }
    if (xpad >= 80 && xpad < 90 && ypad <= 48 && ypad > 36) {
      return 40;
    }
    if (xpad >= 90 && xpad < 100 && ypad <= 48 && ypad > 36) {
      return 41;
    }
    if (xpad >= 100 && xpad < 110 && ypad <= 48 && ypad > 36) {
      return 42;
    }
    if (xpad >= 110 && xpad < 120 && ypad <= 48 && ypad > 36) {
      return 43;
    }
    if (xpad >= 120 && xpad < 130 && ypad <= 48 && ypad > 36) {
      return 44;
    }
    if (xpad >= 130 && xpad < 140 && ypad <= 48 && ypad > 36) {
      return 45;
    }
    if (xpad >= 140 && xpad < 150 && ypad <= 48 && ypad > 36) {
      return 46;
    }
    if (xpad >= 150 && ypad <= 48 && ypad > 36) {
      return 47;
    }
    //---------->4th ROW
    if (xpad >= 0 && xpad < 10 && ypad <= 36 && ypad > 24) {
      return 48;
    }
    if (xpad >= 10 && xpad < 20 && ypad <= 36 && ypad > 24) {
      return 49;
    }
    if (xpad >= 20 && xpad < 30 && ypad <= 36 && ypad > 24) {
      return 50;
    }
    if (xpad >= 30 && xpad < 40 && ypad <= 36 && ypad > 24) {
      return 51;
    }
    if (xpad >= 40 && xpad < 50 && ypad <= 36 && ypad > 24) {
      return 52;
    }
    if (xpad >= 50 && xpad < 60 && ypad <= 36 && ypad > 24) {
      return 53;
    }
    if (xpad >= 60 && xpad < 70 && ypad <= 36 && ypad > 24) {
      return 54;
    }
    if (xpad >= 70 && xpad < 80 && ypad <= 36 && ypad > 24) {
      return 55;
    }
    if (xpad >= 80 && xpad < 90 && ypad <= 36 && ypad > 24) {
      return 56;
    }
    if (xpad >= 90 && xpad < 100 && ypad <= 36 && ypad > 24) {
      return 57;
    }
    if (xpad >= 100 && xpad < 110 && ypad <= 36 && ypad > 24) {
      return 58;
    }
    if (xpad >= 110 && xpad < 120 && ypad <= 36 && ypad > 24) {
      return 59;
    }
    if (xpad >= 120 && xpad < 130 && ypad <= 36 && ypad > 24) {
      return 60;
    }
    if (xpad >= 130 && xpad < 140 && ypad <= 36 && ypad > 24) {
      return 61;
    }
    if (xpad >= 140 && xpad < 150 && ypad <= 36 && ypad > 24) {
      return 62;
    }
    if (xpad >= 150 && ypad <= 36 && ypad > 24) {
      return 63;
    }
    //---------->5th ROW
    if (xpad >= 0 && xpad < 10 && ypad <= 24 && ypad > 12) {
      return 64;
    }
    if (xpad >= 10 && xpad < 20 && ypad <= 24 && ypad > 12) {
      return 65;
    }
    if (xpad >= 20 && xpad < 30 && ypad <= 24 && ypad > 12) {
      return 66;
    }
    if (xpad >= 30 && xpad < 40 && ypad <= 24 && ypad > 12) {
      return 67;
    }
    if (xpad >= 40 && xpad < 50 && ypad <= 24 && ypad > 12) {
      return 68;
    }
    if (xpad >= 50 && xpad < 60 && ypad <= 24 && ypad > 12) {
      return 69;
    }
    if (xpad >= 60 && xpad < 70 && ypad <= 24 && ypad > 12) {
      return 70;
    }
    if (xpad >= 70 && xpad < 80 && ypad <= 24 && ypad > 12) {
      return 71;
    }
    if (xpad >= 80 && xpad < 90 && ypad <= 24 && ypad > 12) {
      return 72;
    }
    if (xpad >= 90 && xpad < 100 && ypad <= 24 && ypad > 12) {
      return 73;
    }
    if (xpad >= 100 && xpad < 110 && ypad <= 24 && ypad > 12) {
      return 74;
    }
    if (xpad >= 110 && xpad < 120 && ypad <= 24 && ypad > 12) {
      return 75;
    }
    if (xpad >= 120 && xpad < 130 && ypad <= 24 && ypad > 12) {
      return 76;
    }
    if (xpad >= 130 && xpad < 140 && ypad <= 24 && ypad > 12) {
      return 77;
    }
    if (xpad >= 140 && xpad < 150 && ypad <= 24 && ypad > 12) {
      return 78;
    }
    if (xpad >= 150 && ypad <= 24 && ypad > 12) {
      return 79;
    }
    //---------->6th ROW (BOTTOM OF BARREL)
    if (xpad >= 0 && xpad < 10 && ypad <= 12) {
      return 80;
    }
    if (xpad >= 10 && xpad < 20 && ypad <= 12) {
      return 81;
    }
    if (xpad >= 20 && xpad < 30 && ypad <= 12) {
      return 82;
    }
    if (xpad >= 30 && xpad < 40 && ypad <= 12) {
      return 83;
    }
    if (xpad >= 40 && xpad < 50 && ypad <= 12) {
      return 84;
    }
    if (xpad >= 50 && xpad < 60 && ypad <= 12) {
      return 85;
    }
    if (xpad >= 60 && xpad < 70 && ypad <= 12) {
      return 86;
    }
    if (xpad >= 70 && xpad < 80 && ypad <= 12) {
      return 87;
    }
    if (xpad >= 80 && xpad < 90 && ypad <= 12) {
      return 88;
    }
    if (xpad >= 90 && xpad < 100 && ypad <= 12) {
      return 89;
    }
    if (xpad >= 100 && xpad < 110 && ypad <= 12) {
      return 90;
    }
    if (xpad >= 110 && xpad < 120 && ypad <= 12) {
      return 91;
    }
    if (xpad >= 120 && xpad < 130 && ypad <= 12) {
      return 92;
    }
    if (xpad >= 130 && xpad < 140 && ypad <= 12) {
      return 93;
    }
    if (xpad >= 140 && xpad < 150 && ypad <= 12) {
      return 94;
    }
    if (xpad >= 150 && ypad <= 12) {
      return 95;
    }
    break;
  case 3: // FwEndcap (-36<x<36, -37<y<37)
          //---------->TOP ROW (OF FWENDCAP)
    if (xpad >= -36 && xpad < -24 && ypad <= 37 && ypad > 27) {
      return 96;
    }
    if (xpad >= -24 && xpad < -12 && ypad <= 37 && ypad > 27) {
      return 97;
    }
    if (xpad >= -12 && xpad < 0 && ypad <= 37 && ypad > 27) {
      return 98;
    }
    if (xpad >= 0 && xpad < 12 && ypad <= 37 && ypad > 27) {
      return 99;
    }
    if (xpad >= 12 && xpad < 24 && ypad <= 37 && ypad > 27) {
      return 100;
    }
    if (xpad >= 24 && ypad <= 37 && ypad > 27) {
      return 101;
    }
    //---------->2nd ROW
    if (xpad >= -36 && xpad < -24 && ypad <= 27 && ypad > 17) {
      return 102;
    }
    if (xpad >= -24 && xpad < -12 && ypad <= 27 && ypad > 17) {
      return 103;
    }
    if (xpad >= -12 && xpad < 0 && ypad <= 27 && ypad > 17) {
      return 104;
    }
    if (xpad >= 0 && xpad < 12 && ypad <= 27 && ypad > 17) {
      return 105;
    }
    if (xpad >= 12 && xpad < 24 && ypad <= 27 && ypad > 17) {
      return 106;
    }
    if (xpad >= 24 && ypad <= 27 && ypad > 17) {
      return 107;
    }
    //---------->3rd ROW
    if (xpad >= -36 && xpad < -24 && ypad <= 17 && ypad > 7) {
      return 108;
    }
    if (xpad >= -24 && xpad < -12 && ypad <= 17 && ypad > 7) {
      return 109;
    }
    if (xpad >= -12 && xpad < 0 && ypad <= 17 && ypad > 7) {
      return 110;
    }
    if (xpad >= 0 && xpad < 12 && ypad <= 17 && ypad > 7) {
      return 111;
    }
    if (xpad >= 12 && xpad < 24 && ypad <= 17 && ypad > 7) {
      return 112;
    }
    if (xpad >= 24 && ypad <= 17 && ypad > 7) {
      return 113;
    }
    //---------->4th ROW
    if (xpad >= -36 && xpad < -24 && ypad <= 7 && ypad > -3) {
      return 114;
    }
    if (xpad >= -24 && xpad < -12 && ypad <= 7 && ypad > -3) {
      return 115;
    }
    if (xpad >= -12 && xpad < 0 && ypad <= 7 && ypad > -3) {
      return 116;
    }
    if (xpad >= 0 && xpad < 12 && ypad <= 7 && ypad > -3) {
      return 117;
    }
    if (xpad >= 12 && xpad < 24 && ypad <= 7 && ypad > -3) {
      return 118;
    }
    if (xpad >= 24 && ypad <= 7 && ypad > -3) {
      return 119;
    }
    //---------->5th ROW
    if (xpad >= -36 && xpad < -24 && ypad <= -3 && ypad > -13) {
      return 120;
    }
    if (xpad >= -24 && xpad < -12 && ypad <= -3 && ypad > -13) {
      return 121;
    }
    if (xpad >= -12 && xpad < 0 && ypad <= -3 && ypad > -13) {
      return 122;
    }
    if (xpad >= 0 && xpad < 12 && ypad <= -3 && ypad > -13) {
      return 123;
    }
    if (xpad >= 12 && xpad < 24 && ypad <= -3 && ypad > -13) {
      return 124;
    }
    if (xpad >= 24 && ypad <= -3 && ypad > -13) {
      return 125;
    }
    //---------->6th ROW
    if (xpad >= -36 && xpad < -24 && ypad <= -13 && ypad > -23) {
      return 126;
    }
    if (xpad >= -24 && xpad < -12 && ypad <= -13 && ypad > -23) {
      return 127;
    }
    if (xpad >= -12 && xpad < 0 && ypad <= -13 && ypad > -23) {
      return 128;
    }
    if (xpad >= 0 && xpad < 12 && ypad <= -13 && ypad > -23) {
      return 129;
    }
    if (xpad >= 12 && xpad < 24 && ypad <= -13 && ypad > -23) {
      return 130;
    }
    if (xpad >= 24 && ypad <= -13 && ypad > -23) {
      return 131;
    }
    //---------->7th ROW
    if (xpad >= -36 && xpad < -24 && ypad <= -23 && ypad > -33) {
      return 132;
    }
    if (xpad >= -24 && xpad < -12 && ypad <= -23 && ypad > -33) {
      return 133;
    }
    if (xpad >= -12 && xpad < 0 && ypad <= -23 && ypad > -33) {
      return 134;
    }
    if (xpad >= 0 && xpad < 12 && ypad <= -23 && ypad > -33) {
      return 135;
    }
    if (xpad >= 12 && xpad < 24 && ypad <= -23 && ypad > -33) {
      return 136;
    }
    if (xpad >= 24 && ypad <= -23 && ypad > -33) {
      return 137;
    }
    //---------->8th ROW (BOTTOM OF FWENDCAP)
    if (ypad <= -33) {
      return 138;
    } // put crystals of last edge in 1 DC (~90 crystals)
    break;
  case 4: // BwEndcap (-14<x<15, -14<y<15)
          //---------->TOP ROW (OF BWENDCAP)
    if (xpad >= -14 && xpad < -7 && ypad <= 15 && ypad > 0) {
      return 139;
    }
    if (xpad >= -7 && xpad < 0 && ypad <= 15 && ypad > 0) {
      return 140;
    }
    if (xpad >= 0 && xpad < 7 && ypad <= 15 && ypad > 0) {
      return 141;
    }
    if (xpad >= 7 && ypad <= 15 && ypad > 0) {
      return 142;
    }
    //---------->2nd ROW (BOTTOM OF BWENDCAP)
    if (xpad >= -14 && xpad < -7 && ypad <= 0) {
      return 143;
    }
    if (xpad >= -7 && xpad < 0 && ypad <= 0) {
      return 144;
    }
    if (xpad >= 0 && xpad < 7 && ypad <= 0) {
      return 145;
    }
    if (xpad >= 7 && ypad <= 0) {
      return 146;
    }
    break;
  case 5: // shashlik (-27<x<1, 0<y<55)
          //---------->TOP ROW (OF SHASHLIK)
    if (xpad >= -27 && xpad < -18 && ypad <= 56 && ypad > 42) {
      return 147;
    }
    if (xpad >= -18 && xpad < -9 && ypad <= 56 && ypad > 42) {
      return 148;
    }
    if (xpad >= -9 && ypad <= 56 && ypad > 42) {
      return 149;
    }
    //---------->2nd ROW
    if (xpad >= -27 && xpad < -18 && ypad <= 42 && ypad > 28) {
      return 150;
    }
    if (xpad >= -18 && xpad < -9 && ypad <= 42 && ypad > 28) {
      return 151;
    }
    if (xpad >= -9 && ypad <= 42 && ypad > 28) {
      return 152;
    }
    //---------->3rd ROW
    if (xpad >= -27 && xpad < -18 && ypad <= 28 && ypad > 14) {
      return 153;
    }
    if (xpad >= -18 && xpad < -9 && ypad <= 28 && ypad > 14) {
      return 154;
    }
    if (xpad >= -9 && ypad <= 28 && ypad > 14) {
      return 155;
    }
    //---------->3rd ROW (BOTTOM OF SHASHLIK)
    if (xpad >= -27 && xpad < -18 && ypad <= 14) {
      return 156;
    }
    if (xpad >= -18 && xpad < -9 && ypad <= 14) {
      return 157;
    }
    if (xpad >= -9 && ypad <= 14) {
      return 158;
    }
    break;
  default: Error("GetDCnumber", "Unknown module");
  }

  LOG(warn) << "BSEmcDigiDCSetterProcess::GetDCnumber returns -1";
  LOG(warn) << "    (XPad, YPad) = (" << xpad << ", " << ypad << "), module = " << module;
  return -1; // failure
}

ClassImp(BSEmcDataConcentratorLookUp)
