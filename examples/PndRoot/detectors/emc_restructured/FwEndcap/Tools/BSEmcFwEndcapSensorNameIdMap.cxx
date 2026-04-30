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

#include "BSEmcFwEndcapSensorNameIdMap.h"

#include <math.h>
#include <stdio.h>

#include "TString.h"

#include "fairlogger/Logger.h"

template <typename T>
Int_t getsign(const T &a)
{
  return (a > 0 ? 1 : a < 0 ? -1 : 0);
}

BSEmcFwEndcapSensorNameIdMap::BSEmcFwEndcapSensorNameIdMap() : PndSensorNameIdMap() {}

BSEmcFwEndcapSensorNameIdMap::~BSEmcFwEndcapSensorNameIdMap() {}

Int_t BSEmcFwEndcapSensorNameIdMap::CreateId(TString &t_sensName) const
{
  LOG(trace) << "BSEmcFwEndcapSensorNameIdMap::CreateId(" << t_sensName << ")";

  ///////////////////////////////////////////////////////////////////////////
  // case new version of forward end-cap (from "emc_module3_2011_new.root" file)
  ///////////////////////////////////////////////////////////////////////////
  if (t_sensName.Contains("SubunitVolFwEndCap")) {
    Int_t copyNoSub = 0, copyNoBox = 0, copyNoCrys = 0;

    if (t_sensName.Contains("HalfSubunitVolFwEndCap")) { // reading the HalfSubunits of FwEndCap
      sscanf(t_sensName.Data(), "/cave_1/Emc3_0/HalfSubunitVolFwEndCap_%d/BoxVol_%d/CrystalVol_%d", &copyNoSub, &copyNoBox, &copyNoCrys);
    } else { // reading the Subunits of FwEndCap
      sscanf(t_sensName.Data(), "/cave_1/Emc3_0/SubunitVolFwEndCap_%d/BoxVol_%d/CrystalVol_%d", &copyNoSub, &copyNoBox, &copyNoCrys);
    }
    // cout << "copyNoSub = "<< copyNoSub<<" ,copyNoBox = "<< copyNoBox<<" ,copyNoCrys = "<< copyNoCrys<<"\n";

    // some presetting of parameters:
    Int_t SubunitCol = -100, SubunitRow = -100, CrystalCol = -100, CrystalRow = -100;
    Int_t RestOfHalfSubunitRowNo[6] = {5, 6, 7, 8, 9, 9}; // row number of 6 peripheral half Subunits
    Int_t RestOfHalfSubunitColNo[6] = {8, 7, 6, 5, 3, 2}; // column number of 6 peripheral half Subunits
    // determination of SubunitRow and SubunitCol:
    if (copyNoSub >= 1 && copyNoSub <= 10) { // the middle row of 10 HalfSubunits
      SubunitRow = 0;
      SubunitCol = pow(-1., 1 + (copyNoSub - 1) / 5) * (4 + (copyNoSub - 1) % 5);
    } else if (copyNoSub > 11 && copyNoSub < 24) { // the upper and lower pipe-region row of 10 HalfSubunits
      SubunitRow = pow(-1., (copyNoSub - 11) / 7) * 2;
      SubunitCol = ((copyNoSub - 11) % 7) - 3;
    } else if ((copyNoSub >= 25 && copyNoSub <= 27) || copyNoSub == 57 || copyNoSub == 58) { // the outermost left column of 5 HalfSubunits
      if (copyNoSub >= 25 && copyNoSub <= 27) {
        SubunitRow = copyNoSub - 25;
      } else {
        SubunitRow = copyNoSub - 59;
      }
      SubunitCol = -9;
    } else if (copyNoSub >= 40 && copyNoSub <= 44) { // the outermost right column of 5 HalfSubunits
      SubunitRow = 42 - copyNoSub;
      SubunitCol = 9;
    } else if (copyNoSub == 28 || copyNoSub == 39 || copyNoSub == 45 || copyNoSub == 56) { // 4 single peripheral half Subunits
      if (copyNoSub <= 39) {
        SubunitRow = RestOfHalfSubunitRowNo[0];
      } else {
        SubunitRow = -RestOfHalfSubunitRowNo[0];
      }
      SubunitCol = pow(-1., copyNoSub - 1) * RestOfHalfSubunitColNo[0];
    } else if (copyNoSub == 29 || copyNoSub == 38 || copyNoSub == 46 || copyNoSub == 55) { // 4 single peripheral half Subunits
      if (copyNoSub <= 38) {
        SubunitRow = RestOfHalfSubunitRowNo[1];
      } else {
        SubunitRow = -RestOfHalfSubunitRowNo[1];
      }
      SubunitCol = pow(-1., copyNoSub) * RestOfHalfSubunitColNo[1];
    } else if (copyNoSub == 30 || copyNoSub == 37 || copyNoSub == 47 || copyNoSub == 54) { // 4 single peripheral half Subunits
      if (copyNoSub <= 37) {
        SubunitRow = RestOfHalfSubunitRowNo[2];
      } else {
        SubunitRow = -RestOfHalfSubunitRowNo[2];
      }
      SubunitCol = pow(-1., copyNoSub - 1) * RestOfHalfSubunitColNo[2];
    } else if (copyNoSub == 31 || copyNoSub == 36 || copyNoSub == 48 || copyNoSub == 53) { // 4 single peripheral half Subunits
      if (copyNoSub <= 36) {
        SubunitRow = RestOfHalfSubunitRowNo[3];
      } else {
        SubunitRow = -RestOfHalfSubunitRowNo[3];
      }
      SubunitCol = pow(-1., copyNoSub) * RestOfHalfSubunitColNo[3];
    } else if (copyNoSub == 32 || copyNoSub == 35 || copyNoSub == 49 || copyNoSub == 52) { // 4 single peripheral half Subunits
      if (copyNoSub <= 35) {
        SubunitRow = RestOfHalfSubunitRowNo[4];
      } else {
        SubunitRow = -RestOfHalfSubunitRowNo[4];
      }
      SubunitCol = pow(-1., copyNoSub - 1) * RestOfHalfSubunitColNo[4];
    } else if (copyNoSub == 33 || copyNoSub == 34 || copyNoSub == 50 || copyNoSub == 51) { // 4 single peripheral half Subunits
      if (copyNoSub <= 34) {
        SubunitRow = RestOfHalfSubunitRowNo[5];
      } else {
        SubunitRow = -RestOfHalfSubunitRowNo[5];
      }
      SubunitCol = pow(-1., copyNoSub) * RestOfHalfSubunitColNo[5];
    } else if (copyNoSub >= 61 && copyNoSub <= 74) { // the middle column of full Subunits
      SubunitRow = pow(-1., (copyNoSub - 61) / 7) * ((copyNoSub - 61) % 7 + 3);
      SubunitCol = 0;
    } else if (copyNoSub % 100 >= 1 && copyNoSub % 100 <= 5) { // rest of the full Subunits
      if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
        SubunitRow = 1;
        SubunitCol = pow(-1., 1 + copyNoSub / 100) * (3 + copyNoSub % 100);
      } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
        SubunitRow = -1;
        SubunitCol = pow(-1., copyNoSub / 100) * (3 + copyNoSub % 100);
      }
    } else if (copyNoSub % 100 >= 6 && copyNoSub % 100 <= 11) { // rest of the full Subunits
      if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
        SubunitRow = 2;
        SubunitCol = pow(-1., 1 + copyNoSub / 100) * (2 + copyNoSub % 100 - 5);
      } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
        SubunitRow = -2;
        SubunitCol = pow(-1., copyNoSub / 100) * (2 + copyNoSub % 100 - 5);
      }
    } else if (copyNoSub % 100 >= 12 && copyNoSub % 100 <= 19) { // rest of the full Subunits
      if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
        SubunitRow = 3;
        SubunitCol = pow(-1., 1 + copyNoSub / 100) * (copyNoSub % 100 - 11);
      } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
        SubunitRow = -3;
        SubunitCol = pow(-1., copyNoSub / 100) * (copyNoSub % 100 - 11);
      }
    } else if (copyNoSub % 100 >= 20 && copyNoSub % 100 <= 27) { // rest of the full Subunits
      if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
        SubunitRow = 4;
        SubunitCol = pow(-1., 1 + copyNoSub / 100) * (copyNoSub % 100 - 19);
      } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
        SubunitRow = -4;
        SubunitCol = pow(-1., copyNoSub / 100) * (copyNoSub % 100 - 19);
      }
    } else if (copyNoSub % 100 >= 28 && copyNoSub % 100 <= 34) { // rest of the full Subunits
      if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
        SubunitRow = 5;
        SubunitCol = pow(-1., 1 + copyNoSub / 100) * (copyNoSub % 100 - 27);
      } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
        SubunitRow = -5;
        SubunitCol = pow(-1., copyNoSub / 100) * (copyNoSub % 100 - 27);
      }
    } else if (copyNoSub % 100 >= 35 && copyNoSub % 100 <= 40) { // rest of the full Subunits
      if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
        SubunitRow = 6;
        SubunitCol = pow(-1., 1 + copyNoSub / 100) * (copyNoSub % 100 - 34);
      } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
        SubunitRow = -6;
        SubunitCol = pow(-1., copyNoSub / 100) * (copyNoSub % 100 - 34);
      }
    } else if (copyNoSub % 100 >= 41 && copyNoSub % 100 <= 45) { // rest of the full Subunits
      if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
        SubunitRow = 7;
        SubunitCol = pow(-1., 1 + copyNoSub / 100) * (copyNoSub % 100 - 40);
      } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
        SubunitRow = -7;
        SubunitCol = pow(-1., copyNoSub / 100) * (copyNoSub % 100 - 40);
      }
    } else if (copyNoSub % 100 >= 46 && copyNoSub % 100 <= 49) { // rest of the full Subunits
      if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
        SubunitRow = 8;
        SubunitCol = pow(-1., 1 + copyNoSub / 100) * (copyNoSub % 100 - 45);
      } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
        SubunitRow = -8;
        SubunitCol = pow(-1., copyNoSub / 100) * (copyNoSub % 100 - 45);
      }
    } else if (copyNoSub % 100 == 50) { // rest of the full Subunits
      if (copyNoSub / 100 == 1 || copyNoSub / 100 == 4) {
        SubunitRow = 9;
        SubunitCol = pow(-1., 1 + copyNoSub / 100);
      } else { // here necessarily: copyNoSub/100 == 2 || copyNoSub/100 == 3
        SubunitRow = -9;
        SubunitCol = pow(-1., copyNoSub / 100);
      }
    }

    Int_t flag = 1; // this flag introducing is not necessary in this class but is also harmless
    if (fabs(SubunitRow) <= 1 && fabs(SubunitCol) <= 3) {
      flag = 0; // empty copyNoSub in the beam-pipe area
    } else if (fabs(SubunitCol) == 9 && fabs(SubunitRow) >= 3) {
      flag = 0; // empty copyNoSub in the residual area
    } else if (fabs(SubunitCol) == 8 && fabs(SubunitRow) >= 6) {
      flag = 0; //  -||-
    } else if (fabs(SubunitCol) == 7 && fabs(SubunitRow) >= 7) {
      flag = 0; //  -||-
    } else if (fabs(SubunitCol) == 6 && fabs(SubunitRow) >= 8) {
      flag = 0; //  -||-
    } else if (fabs(SubunitCol) >= 4 && fabs(SubunitRow) == 9) {
      flag = 0; //  -||-
    }

    // determination of CrystalRow and CrystalCol:
    if ((flag != 0) && copyNoSub >= 61) { // determination of CrystalRow and CrystalCol for the 214 full Subunits
      if (copyNoBox == 1 || copyNoBox == 4) {
        if (copyNoCrys == 1 || copyNoCrys == 3) {
          CrystalRow = SubunitRow * 4 + (3 - getsign(SubunitRow)) / 2;
        } else { // here necessarily: copyNoCrys==2 || copyNoCrys==4
          CrystalRow = SubunitRow * 4 + (1 - getsign(SubunitRow)) / 2;
        }
      } else if (copyNoBox == 2 || copyNoBox == 3) {
        if (copyNoCrys == 1 || copyNoCrys == 3) {
          CrystalRow = SubunitRow * 4 - (1 + getsign(SubunitRow)) / 2;
        } else { // here necessarily: copyNoCrys==2 || copyNoCrys==4
          CrystalRow = SubunitRow * 4 - (3 + getsign(SubunitRow)) / 2;
        }
      }

      if (copyNoSub >= 61 && copyNoSub <= 74) { // determination of CrystalCol for the 14 middle column full Subunits
        if (copyNoBox == 1 || copyNoBox == 3) {
          if (copyNoCrys == 1 || copyNoCrys == 4) {
            CrystalCol = 2;
          } else { // here necessarily: copyNoCrys==3 || copyNoCrys==2
            CrystalCol = 1;
          }
        } else if (copyNoBox == 4 || copyNoBox == 2) {
          if (copyNoCrys == 1 || copyNoCrys == 4) {
            CrystalCol = -1;
          } else { // here necessarily: copyNoCrys==3 || copyNoCrys==2
            CrystalCol = -2;
          }
        }
      } else { // determination of CrystalCol for the other 4*50=200 full Subunits
        if (copyNoBox == 1 || copyNoBox == 3) {
          if (copyNoCrys == 1 || copyNoCrys == 4) {
            CrystalCol = SubunitCol * 4 + (3 + getsign(SubunitCol)) / 2;
          } else { // here necessarily: copyNoCrys==3 || copyNoCrys==2
            CrystalCol = SubunitCol * 4 + (1 + getsign(SubunitCol)) / 2;
          }
        } else if (copyNoBox == 4 || copyNoBox == 2) {
          if (copyNoCrys == 1 || copyNoCrys == 4) {
            CrystalCol = SubunitCol * 4 - (1 - getsign(SubunitCol)) / 2;
          } else { // here necessarily: copyNoCrys==3 || copyNoCrys==2
            CrystalCol = SubunitCol * 4 - (3 - getsign(SubunitCol)) / 2;
          }
        }
      }
    }

    else if ((flag != 0) && copyNoSub <= 10) { // determination of CrystalRow and CrystalCol for the 10 middle-row half Subunits
      if (copyNoCrys == 1 || copyNoCrys == 3) {
        CrystalRow = 1;
      } else { // here necessarily: copyNoCrys==2 || copyNoCrys==4
        CrystalRow = -1;
      }

      if (copyNoBox == 1) {
        if (copyNoCrys == 1 || copyNoCrys == 4) {
          CrystalCol = SubunitCol * 4 + (3 + getsign(SubunitCol)) / 2;
        } else { // here necessarily: copyNoCrys==3 || copyNoCrys==2
          CrystalCol = SubunitCol * 4 + (1 + getsign(SubunitCol)) / 2;
        }
      } else { // here necessarily: copyNoBox==2
        if (copyNoCrys == 1 || copyNoCrys == 4) {
          CrystalCol = SubunitCol * 4 - (1 - getsign(SubunitCol)) / 2;
        } else { // here necessarily: copyNoCrys==3 || copyNoCrys==2
          CrystalCol = SubunitCol * 4 - (3 - getsign(SubunitCol)) / 2;
        }
      }
    } else if ((flag != 0) && copyNoSub > 11 && copyNoSub < 24) { // determination of CrystalRow and CrystalCol for the 10 upper and lower pipe-region row of half subunits
      if (copyNoCrys == 1 || copyNoCrys == 3) {
        CrystalRow = (1 + 17 * getsign(SubunitRow)) / 2;
      } else { // here necessarily: copyNoCrys==2 || copyNoCrys==4
        CrystalRow = (17 * getsign(SubunitRow) - 1) / 2;
      }

      if (SubunitCol == 0 && copyNoBox == 1) {
        if (copyNoCrys == 1 || copyNoCrys == 4) {
          CrystalCol = 2;
        } else {
          CrystalCol = 1;
        }
      } else if (SubunitCol == 0) { // here necessarily: copyNoBox==2
        if (copyNoCrys == 1 || copyNoCrys == 4) {
          CrystalCol = -1;
        } else {
          CrystalCol = -2;
        }
      } else if (copyNoBox == 1) {
        if (copyNoCrys == 1 || copyNoCrys == 4) {
          CrystalCol = SubunitCol * 4 + (3 + getsign(SubunitCol)) / 2;
        } else { // here necessarily: copyNoCrys==3 || copyNoCrys==2
          CrystalCol = SubunitCol * 4 + (1 + getsign(SubunitCol)) / 2;
        }
      } else { // here necessarily: copyNoBox==2
        if (copyNoCrys == 1 || copyNoCrys == 4) {
          CrystalCol = SubunitCol * 4 - (1 - getsign(SubunitCol)) / 2;
        } else { // here necessarily: copyNoCrys==3 || copyNoCrys==2
          CrystalCol = SubunitCol * 4 - (3 - getsign(SubunitCol)) / 2;
        }
      }
    } else if ((flag != 0) && ((copyNoSub >= 25 && copyNoSub <= 27) || copyNoSub == 57 || copyNoSub == 58 ||
                               (copyNoSub >= 40 && copyNoSub <= 44))) { // determination of CrystalRow and CrystalCol for the 2 outermost left and right columns of half Subunits
      if (copyNoBox == 1 && (copyNoCrys == 1 || copyNoCrys == 4)) {
        if (copyNoSub == 25 || copyNoSub == 42) {
          CrystalRow = 2;
        } else {
          CrystalRow = SubunitRow * 4 + (3 + getsign(SubunitRow)) / 2;
        }
      } else if (copyNoBox == 1) { // here necessarily: copyNoCrys==3 || copyNoCrys==2
        if (copyNoSub == 25 || copyNoSub == 42) {
          CrystalRow = 1;
        } else {
          CrystalRow = SubunitRow * 4 + (1 + getsign(SubunitRow)) / 2;
        }

      } else if (copyNoBox == 2 && (copyNoCrys == 1 || copyNoCrys == 4)) {
        if (copyNoSub == 25 || copyNoSub == 42) {
          CrystalRow = -1;
        } else {
          CrystalRow = SubunitRow * 4 - (1 - getsign(SubunitRow)) / 2;
        }
      } else if (copyNoBox == 2) { // here necessarily: copyNoCrys==3 || copyNoCrys==2  // FIXME Use braces for explicit logic, possible ambiguity
        if (copyNoSub == 25 || copyNoSub == 42) {
          CrystalRow = -2;
        } else {
          CrystalRow = SubunitRow * 4 - (3 - getsign(SubunitRow)) / 2;
        }
      }

      if (copyNoCrys == 1 || copyNoCrys == 3) {
        if (!(copyNoSub >= 40 && copyNoSub <= 44)) {
          CrystalCol = -36;
        } else {
          CrystalCol = 35;
        }
      } else // here necessarily: copyNoCrys==4 || copyNoCrys==2
        if (!(copyNoSub >= 40 && copyNoSub <= 44)) {
        CrystalCol = -35;
      } else {
        CrystalCol = 36;
      }
    } else if ((flag != 0) && (copyNoSub == 28 || copyNoSub == 29 || copyNoSub == 38 || copyNoSub == 39 || copyNoSub == 45 || copyNoSub == 46 || copyNoSub == 55 ||
                               copyNoSub == 56)) { // determination of CrystalRow and CrystalCol for the 8 single peripheral half Subunits
      if (copyNoBox == 1 && (copyNoCrys == 1 || copyNoCrys == 4)) {
        CrystalRow = SubunitRow * 4 + (3 - getsign(SubunitRow)) / 2;
      } else if (copyNoBox == 1) { // here necessarily: copyNoCrys==3 || copyNoCrys==2
        CrystalRow = SubunitRow * 4 + (1 - getsign(SubunitRow)) / 2;
      } else if (copyNoCrys == 1 || copyNoCrys == 4) { // here necessarily: copyNoBox==2
        CrystalRow = SubunitRow * 4 - (1 + getsign(SubunitRow)) / 2;
      } else { // here necessarily: copyNoBox==2 && (copyNoCrys==3 || copyNoCrys==2)
        CrystalRow = SubunitRow * 4 - (3 + getsign(SubunitRow)) / 2;
      }

      if (copyNoCrys == 4 || copyNoCrys == 2) {
        CrystalCol = SubunitCol * 4 + (1 - getsign(SubunitCol)) / 2;
      } else { // here necessarily: copyNoCrys==3 || copyNoCrys==1
        CrystalCol = SubunitCol * 4 - (1 + getsign(SubunitCol)) / 2;
      }
    } else if ((flag != 0) && ((copyNoSub >= 30 && copyNoSub <= 37) ||
                               (copyNoSub >= 47 && copyNoSub <= 54))) { // determination of CrystalRow and CrystalCol for the 16 single peripheral half Subunits
      if (copyNoCrys == 1 || copyNoCrys == 3) {
        CrystalRow = SubunitRow * 4 + (1 - 3 * getsign(SubunitRow)) / 2;
      } else { // here necessarily: copyNoCrys==4 || copyNoCrys==2
        CrystalRow = SubunitRow * 4 - (1 + 3 * getsign(SubunitRow)) / 2;
      }

      if (copyNoBox == 1 && (copyNoCrys == 1 || copyNoCrys == 4)) {
        CrystalCol = SubunitCol * 4 + (3 + getsign(SubunitCol)) / 2;
      } else if (copyNoBox == 1) { // here necessarily: copyNoCrys==3 || copyNoCrys==2
        CrystalCol = SubunitCol * 4 + (1 + getsign(SubunitCol)) / 2;
      } else if (copyNoCrys == 1 || copyNoCrys == 4) { // here necessarily: copyNoBox==2
        CrystalCol = SubunitCol * 4 - (1 - getsign(SubunitCol)) / 2;
      } else { // here necessarily: copyNoBox==2 && (copyNoCrys==3 || copyNoCrys==2)
        CrystalCol = SubunitCol * 4 - (3 - getsign(SubunitCol)) / 2;
      }
    }
    LOG(trace) << "BSEmcFwEndcapSensorNameIdMap::CreateId(" << t_sensName << ")";
    LOG(trace) << "module = " << 3 << ", row = " << (CrystalRow + 37) << ", copy = " << 0 << ", crystal = " << (CrystalCol + 36);
    LOG(trace) << "detectorId = " << 3 * 100000000 + (CrystalRow + 37) * 1000000 + 0 * 10000 + (CrystalCol + 36);

    return 3 * 100000000 + (CrystalRow + 37) * 1000000 + 0 * 10000 + (CrystalCol + 36);
  }
  return -1;
}
