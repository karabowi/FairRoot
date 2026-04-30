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
 * PndMvdReadInTBData.h
 *
 *  Created on: 23.10.2014
 *      Author: Stockmanns
 */

#ifndef PNDMVDREADINTBDATA_H_
#define PNDMVDREADINTBDATA_H_

#include "Rtypes.h"
#include "TClonesArray.h"
#include "TString.h"

#include "PndSdsDigiTopix4.h"
#include "PndSdsDigiTopix4Header.h"

#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

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
  pixel() : fPixelAddress(0), fLeadingEdge(0), fTrailingEdge(0){};

  UInt_t fPixelAddress;
  UInt_t fLeadingEdge;
  UInt_t fTrailingEdge;
};

class PndMvdReadInTBData {
 public:
  PndMvdReadInTBData();
  virtual ~PndMvdReadInTBData();
  void SetFileName(std::vector<TString> fileName)
  {

    std::cout << "number of entires " << fileName.size() << std::endl;
    for (int i = 0; i < fileName.size(); i++) {
      std::cout << i << " " << fileName[i] << std::endl;
    }
    fFileNames = fileName;
  }

  void Init();

  Bool_t ReadInData(TClonesArray *sdsDigiContainer, TClonesArray *headerContainer, TClonesArray *allheaderContainer);
  Bool_t ReadInRawData(std::ifstream *fileHandle, std::vector<ULong64_t> &rawData); //<input is fileHandle, output vector of raw data, output is end of file
  void AnalyzeData(std::vector<ULong64_t> &rawData, Double_t clockFrequency);

  pixel BitAnalyzePixelData(ULong64_t &data);
  frameHeader BitAnalyzeHeader(ULong64_t &header);
  frameTrailer BitAnalyzeTrailer(ULong64_t &trailer);

  std::pair<UInt_t, UInt_t> PixeladdressToMatrixAddress(UInt_t pixelglobaladdress);

  void SetClockFrequency(Double_t val) { fClockFrequency = val; }
  void SetFE(Int_t val) { fFE = val; }
  UInt_t GetNonSequenctialFC() const { return fNonSequentialFC; }
  UInt_t GetDoubleHeader() const { return fDoubleHeader; }
  UInt_t GetDoubleTrailer() const { return fDoubleTrailer; }
  UInt_t GetSuperFrameCount() const { return fSuperFrameCount; }

  UInt_t GetTotalHitCount() const { return fTotalHitCount; }
  UInt_t GetCorrectHitCount() const { return fCorrectHitCount; }
  UInt_t GetPreFrameLossHitCount() const { return fPreFrameLossHitCount; }
  UInt_t GetHammingLossHitCount() const { return fHammingLossHitCount; }
  UInt_t GetCRCLossHitCount() const { return fCRCLossHitCount; }

  UInt_t GetTotalFrameCount() const { return fTotalFrameCount; }
  UInt_t GetCorrectFrameCount() const { return fCorrectFrameCount; }
  UInt_t GetHammingLossFrameCount() const { return fHammingLossFrameCount; }
  UInt_t GetCRCLossFrameCount() const { return fCRCLossFrameCount; }

  UInt_t GetTotalHeaderCount() const { return fTotalHeaderCount; }
  UInt_t GetTotalTrailerCount() const { return fTotalTrailerCount; }

  PndSdsDigiTopix4 ProcessData(ULong64_t &data, frameHeader &header, Double_t &clockFrequency);
  ULong64_t ReflectBitsStream(ULong64_t crc, int bitnum);
  ULong64_t CalculateCRCTableFast(std::vector<char> p, ULong64_t len);
  void GenerateCRCTable();
  UShort_t CheckHammingCode(ULong64_t dataword, int dataword_length);
  ULong64_t ConvertToPix4HammingToStandardHamming(ULong64_t topixhamming);
  void AnalyzeToPixFrame(Double_t clockFrequency);
  void SetVerbose(Int_t val) { fVerbose = val; }

 private:
  std::vector<TString> fFileNames;
  std::ifstream *fFileHandle;
  Double_t fClockFrequency;
  TClonesArray *fOutputArray;
  TClonesArray *fOutputArrayHeader;
  TClonesArray *fOutputArrayAllHeader;
  UInt_t fSuperFrameCount;
  UInt_t fOldFrameCount;
  UInt_t fOldAllHeaderCount;
  UInt_t fNonSequentialFC;
  UInt_t fHammingLossFrameCount;
  UInt_t fCRCLossFrameCount;

  UInt_t fTotalHitCount;
  UInt_t fPreFrameLossHitCount;
  UInt_t fHammingLossHitCount;
  UInt_t fCRCLossHitCount;
  UInt_t fCorrectHitCount;

  frameHeader fRecentFrameHeader;
  frameHeader fRecentAllFrameHeader;
  frameTrailer fRecentFrameTrailer;
  Bool_t fFirstHeader;
  Bool_t fHeaderPresent;
  Bool_t fTrailerPresent;
  UInt_t fDoubleHeader;
  UInt_t fDoubleTrailer;

  UInt_t fTotalHeaderCount;
  UInt_t fTotalTrailerCount;

  Int_t fVerbose; // fVerbose==5 gives all detected errors from ToPix build in error detection
  Int_t fFE;
  Int_t fFileCounter;
  UInt_t fDataCount;
  UInt_t fTotalFrameCount;
  UInt_t fCorrectFrameCount;
  std::vector<ULong64_t> fToPixFrame;

  TClonesArray *fDigiArray;
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
};

#endif /* PNDMVDREADINTBDATA_H_ */
