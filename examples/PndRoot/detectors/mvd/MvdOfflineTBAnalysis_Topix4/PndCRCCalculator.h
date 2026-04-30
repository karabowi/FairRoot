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

/**
 * \brief PndCRCCalculator calculates the CRC checksum from a given vector<char>
 *
 * PndCRCCalculator is based on a look-up table initially calculated from the paramters of the used CRC code.
 * The output of the calculator was cross-checked with the online CRC calculator: http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
 * Both the generated look-up tables as the output match
 *
 *  \author T.Stockmanns <t.stockmanns@fz-juelich.de>
 */

#ifndef MVD_MVDOFFLINETBANALYSIS_PNDCRCCALCULATOR_H_
#define MVD_MVDOFFLINETBANALYSIS_PNDCRCCALCULATOR_H_

#include "Rtypes.h"
#include "TObject.h"

class PndCRCCalculator : public TObject {
 public:
  PndCRCCalculator();
  PndCRCCalculator(UInt_t order, ULong64_t polynom, ULong64_t CRCXor, UInt_t refIn, UInt_t refOut, UInt_t CRCInit_direct);
  // settings for ToPix: fOrder(16), fPolynom(0x8005), fCRCXor(0x0000), fRefIn(0), fRefOut(0), fCRCInit_direct(0)
  // settings for PASTA: fOrder(16), fPolynom(0x1021), fCRCXor(0x0000), fRefIn(0), fRefOut(0), fCRCInit_direct(0x04FA)
  virtual ~PndCRCCalculator();

  ULong64_t CalculateCRCTableFast(std::vector<char> p, ULong64_t len);
  ULong64_t ReflectBitsStream(ULong64_t crc, int bitnum);
  //	std::vector<char> ConvertData(std::vector<ULong64_t> topixFrame);
  void PrintCRCTable();

 protected:
  void GenerateCRCTable();

 private:
  // CRC parameters :

  const UInt_t fOrder;
  const ULong64_t fPolynom;
  const ULong64_t fCRCXor;
  const UInt_t fRefIn;
  const UInt_t fRefOut;

  ULong64_t fCRCMask;
  ULong64_t fCRCHighBit;
  ULong64_t fCRCInit_direct;
  ULong64_t fCRCTab[256];

  ClassDef(PndCRCCalculator, 1);
};

#endif /* MVD_MVDOFFLINETBANALYSIS_TOPIX4_PNDHAMMINGDECODER_H_ */
