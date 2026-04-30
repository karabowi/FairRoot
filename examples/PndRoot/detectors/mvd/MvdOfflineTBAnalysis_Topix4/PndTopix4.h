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

/*
 * PndTopix4.h
 *
 *  Created on: 16.09.2015
 *      Author: Stockmanns
 */

#ifndef MVD_MVDOFFLINETBANALYSIS_TOPIX4_PNDTOPIX4_H_
#define MVD_MVDOFFLINETBANALYSIS_TOPIX4_PNDTOPIX4_H_

#include "Rtypes.h"
#include "mrfdata_8b.h"

namespace ToPix4 {
struct frameHeader {
  frameHeader() : fChipAddress(0), fFrameCount(0), fECC(0){};

  UInt_t fChipAddress;
  UInt_t fFrameCount;
  UInt_t fECC;
};

struct frameTrailer {
  frameTrailer() : fNEvents(0), fFrameCRC(0), fECC(0){};

  UInt_t fNEvents;
  UInt_t fFrameCRC;
  UInt_t fECC;
};

struct pixel {
  pixel() : fPixelNumber(0), fLeadingEdge(0), fTrailingEdge(0){};

  UInt_t fPixelNumber;
  UInt_t fLeadingEdge;
  UInt_t fTrailingEdge;
};

struct pixelAddress {
  pixelAddress() : fRow(0), fCol(0), fSide(0){};
  pixelAddress(UInt_t row, UInt_t dcol, Bool_t side) : fRow(row), fCol(dcol), fSide(side){}; // row 0-31 or 0-127; dcol 0-3; side 0-1
  pixelAddress(UInt_t row, UInt_t col) : fRow(row)
  { // row 0-31 or 0-127; col 0-7 (needed to translate pixel configuration data into matrix
    fCol = col / 2;
    fSide = !(col % 2);
  };
  UInt_t fRow;
  UInt_t fCol;
  Bool_t fSide;
};
} // namespace ToPix4

class PndTopix4 {
 public:
  PndTopix4();
  virtual ~PndTopix4();

  ULong64_t ConvertToPix4HammingToStandardHamming(ULong64_t topixhamming);

  int GetType(ULong64_t data); ///< returns if the data word is a header (0b01), trailer (0b10) or data (0b11)
  int GetFrameCount(ULong64_t data);
  ToPix4::pixel BitAnalyzePixelData(ULong64_t &data);
  ToPix4::frameHeader BitAnalyzeHeader(ULong64_t &header);
  ToPix4::frameTrailer BitAnalyzeTrailer(ULong64_t &trailer);

  std::vector<ULong64_t> GetRawData(TMrfData_8b *data);

  ToPix4::pixelAddress PixelNumberToPixelAddress(UInt_t pixelnumber);

  std::pair<UInt_t, UInt_t> PixelAddressToMatrixAddress(ToPix4::pixelAddress address);
  std::pair<UInt_t, UInt_t> PixelAddressToMatrixAddress(UInt_t row, UInt_t col, Bool_t side);

  std::pair<UInt_t, UInt_t> PixelNumberToMatrixAddress(UInt_t pixelnumber);

  ToPix4::pixelAddress MatrixAddressToPixelAddress(std::pair<UInt_t, UInt_t> matrixAddress);
  ToPix4::pixelAddress MatrixAddressToPixelAddress(UInt_t matrix_col, UInt_t matrix_row) { return MatrixAddressToPixelAddress(std::make_pair(matrix_col, matrix_row)); };

  std::pair<UInt_t, UInt_t> MatrixAddressToPixelAddressConf(UInt_t matrix_col, UInt_t matrix_row)
  { // converts a matrix address (0-20, 0-31) into a ToPix configuration address col 0-7, row 0-31 / 0-127
    ToPix4::pixelAddress address = MatrixAddressToPixelAddress(matrix_col, matrix_row);
    UInt_t col = address.fCol * 2;
    col += !(address.fSide);
    UInt_t row = address.fRow;
    return std::make_pair(col, row);
  }

  //	ClassDef(PndTopix4, 1);
};

#endif /* MVD_MVDOFFLINETBANALYSIS_TOPIX4_PNDTOPIX4_H_ */
