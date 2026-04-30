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
 * PndMvdReadInTBData.cxx
 *
 *  Created on: 23.10.2014
 *      Author: Stockmanns
 */

#include "PndMvdReadInTBData.h"

#include "mrfdata_8b.h"
#include "mrftools.h"
#include "boost/archive/binary_oarchive.hpp"
#include "boost/archive/binary_iarchive.hpp"
#include "boost/serialization/binary_object.hpp"
#include <boost/archive/archive_exception.hpp>

PndMvdReadInTBData::PndMvdReadInTBData()
  : fDigiArray(0), fClockFrequency(0), fSuperFrameCount(0), fOldFrameCount(0), fOldAllHeaderCount(0), fFirstHeader(kTRUE), fFE(-1), fNonSequentialFC(0), fHammingLossFrameCount(0),
    fCRCLossFrameCount(0), fTotalHitCount(0), fPreFrameLossHitCount(0), fHammingLossHitCount(0), fCRCLossHitCount(0), fCorrectHitCount(0), fHeaderPresent(kFALSE),
    fTrailerPresent(kFALSE), fDoubleHeader(0), fDoubleTrailer(0), fVerbose(0), fOrder(16), fPolynom(0x8005), fCRCXor(0x0000), fRefIn(0), fRefOut(0), fCRCInit_direct(0),
    fDataCount(0), fFileCounter(0), fTotalFrameCount(0), fTotalHeaderCount(0), fTotalTrailerCount(0)
{
  // TODO Auto-generated constructor stub
  fCRCMask = ((((unsigned long)1 << (fOrder - 1)) - 1) << 1) | 1;
  fCRCHighBit = (unsigned long)1 << (fOrder - 1);

  GenerateCRCTable();
}

PndMvdReadInTBData::~PndMvdReadInTBData()
{
  fFileHandle->close();
  delete (fFileHandle);
}

void PndMvdReadInTBData::GenerateCRCTable()
{
  // make CRC lookup table used by table algorithms
  ULong64_t bit, crc;
  for (int i = 0; i < 256; i++) {
    crc = (ULong64_t)i;
    if (fRefIn) {
      crc = ReflectBitsStream(crc, 8);
    }
    crc <<= fOrder - 8;

    for (int j = 0; j < 8; j++) {
      bit = crc & fCRCHighBit;
      crc <<= 1;
      if (bit) {
        crc ^= fPolynom;
      }
    }
    if (fRefIn) {
      crc = ReflectBitsStream(crc, fOrder);
    }
    crc &= fCRCMask;
    fCRCTab[i] = crc;
  }
}

UShort_t PndMvdReadInTBData::CheckHammingCode(ULong64_t dataword, int dataword_length)
{
  // This function expects a standard hamming encoded dataword (including hamming bits at 2^i positions) and returns the hamming bits
  // If the dataword is correct, the hamming bits are 0
  // If an error occured, the hamming bits are !=0
  // If a 1-bit error occured, the hamming bits give the possions of the wrong bit

  ULong64_t shift = 1;
  ULong64_t hamming_code = 0;

  if (dataword_length > 63) {
    std::cout << "Error: maximum length of datastream is 63" << std::endl;
    return 0xffff;
  }

  // loop over all hamming bits
  for (int i = 0; pow(2, i) < dataword_length; ++i) {
    int start = pow(2, i);
    int stepwidth = 2 * start;
    int parity_bit = 0;
    for (int j = start; j < dataword_length + 1; j += stepwidth) {
      for (int k = j; k < j + stepwidth / 2 && k < dataword_length + 1; ++k) {
        parity_bit ^= ((dataword & (shift << (k - 1))) >> (k - 1));
      }
    }
    hamming_code += (parity_bit << i);
  }
  return hamming_code;
}

ULong64_t PndMvdReadInTBData::ConvertToPix4HammingToStandardHamming(ULong64_t topixhamming)
{
  // The ToPix header and trailer data word consists of 34 data bits and 6 hamming bits.
  //								       bit no:	39    38   37   36   35         9    8    7    6    5    4    3    2    1    0
  // In a ToPix4 dataword the hamming bits are at the end of the word:           (d34)(d33)(d32)(d31)(d30) ... (d04)(d03)(d02)(d01)(H 6)(H 5)(H 4)(H 3)(H 2)(H 1)
  // At the standard hamming encoding the hamming bits are at the 2^i positions  (d34)(d33)(d32)(d31)(d30) ... (d07)(d06)(d05)(d04)(H 3)(d03)(d02)(d01)(H 2)(H 1)
  // This function expects a ToPix4 encoded dataword and moves the hamming bits at the end to the defined positions.

  ULong64_t standard_hamming = 0;

  standard_hamming = (topixhamming & 0xff00000000);
  standard_hamming += ((topixhamming & 0x00fffe0000) >> 1);
  standard_hamming += ((topixhamming & 0x000001fc00) >> 2);
  standard_hamming += ((topixhamming & 0x0000000380) >> 3);
  standard_hamming += ((topixhamming & 0x0000000040) >> 4);

  standard_hamming += ((topixhamming & 0x0000000020) << 26);
  standard_hamming += ((topixhamming & 0x0000000010) << 11);
  standard_hamming += ((topixhamming & 0x0000000008) << 4);
  standard_hamming += ((topixhamming & 0x0000000004) << 1);
  standard_hamming += (topixhamming & 0x0000000002);
  standard_hamming += (topixhamming & 0x0000000001);

  return standard_hamming;
}

void PndMvdReadInTBData::Init()
{
  // std::cout << "PndMvdReadInTBData::Init called" << std::endl;
  //	for (int i = 0; i < fFileName.size(); i++){
  std::ifstream *ifs = new std::ifstream(fFileNames[fFileCounter], std::ios::binary);
  std::cout << "File: " << fFileNames[fFileCounter] << " is good: " << ifs->good() << std::endl;
  fFileCounter++;
  fFileHandle = ifs;
  //	}
}

Bool_t PndMvdReadInTBData::ReadInData(TClonesArray *sdsDigiContainer, TClonesArray *headerContainer, TClonesArray *allheaderContainer)
{
  TMrfData_8b *tempdata;
  tempdata = new TMrfData_8b;
  ULong_t dataword = 0;
  Bool_t endOfFile = kFALSE;

  fOutputArray = sdsDigiContainer;
  fOutputArrayHeader = headerContainer;
  fOutputArrayAllHeader = allheaderContainer;
  std::vector<ULong64_t> rawArray;
  endOfFile |= ReadInRawData(fFileHandle, rawArray);
  AnalyzeData(rawArray, fClockFrequency);
  return endOfFile;
}

Bool_t PndMvdReadInTBData::ReadInRawData(std::ifstream *fileHandle, std::vector<ULong64_t> &rawData)
{
  TMrfData_8b *tempdata;
  tempdata = new TMrfData_8b;
  ULong_t dataword = 0;
  Bool_t endOfFile = kFALSE;
  if (fileHandle->good()) {
    if (fVerbose > 2) {
      std::cout << std::endl;
      std::cout << "PndMvdReadInTBData::ReadInRawData reading file " << std::endl;
    }
    try {
      boost::archive::binary_iarchive iar(*fileHandle); // this line causes an "Invalid Signature Error" at the end of the file but the file is still good
      iar >> tempdata;
    } catch (boost::archive::archive_exception &exception) {
      if (fVerbose > 1) {
        std::cout << "PndMvdReadInTBData::ReadInRawData: Error found in reading file "
                  << " : " << fileHandle->good() << " " << fileHandle->eof() << " Exception: " << exception.code << std::endl;
      }
      if (fVerbose > 1) {
        std::cout << exception.what() << std::endl;
      }
      if (exception.code == 3) {
        if (fFileCounter < fFileNames.size()) {
          std::cout << fFE << " open new file " << fFileNames[fFileCounter] << std::endl;
          fileHandle->close();
          delete (fFileHandle);
          std::ifstream *ifs = new std::ifstream(fFileNames[fFileCounter], std::ios::binary);
          fFileCounter++;
          return endOfFile;
        } else {
          std::cout << fFE << " All files read! Finishing FE " << std::endl;
          endOfFile = kTRUE;
          return endOfFile;
        }
      }
    }
    if (fVerbose > 2)
      std::cout << fFE << " PndMvdReadInTBData::ReadInRawData: NWords: " << tempdata->getNumWords() << std::endl;
    for (UInt_t i = 0; i < tempdata->getNumWords(); i += 5) {
      dataword = 0;
      for (uint j = 0; j < 5; j++) {
        dataword = dataword << 8;
        dataword += tempdata->getWord(i + j);
      }
      rawData.push_back(dataword);
      ULong_t frameCount = dataword & 0x3FC0000;
      frameCount = frameCount >> 18;
      if (fVerbose > 2)
        std::cout << std::dec << "dataword No " << i / 5 << "/" << tempdata->getNumWords() / 5 << ": " << std::hex << dataword << " " << std::dec << frameCount << std::endl;
    }
  } else {
    std::cout << fFE << " An error occured " << std::endl;
    std::cout << fFE << " fileHandle->good() " << fileHandle->good() << std::endl;
    std::cout << fFE << " fileHandle->eof()  " << fileHandle->eof() << std::endl;
    std::cout << fFE << " fileHandle->fail() " << fileHandle->fail() << std::endl;
    std::cout << fFE << " fileHandle->bad()  " << fileHandle->bad() << std::endl;

    endOfFile = kFALSE;
    return endOfFile;
  }
  return endOfFile;
}

ULong64_t PndMvdReadInTBData::CalculateCRCTableFast(std::vector<char> p, ULong64_t len)
{

  // fast lookup table algorithm without augmented zero bytes, e.g. used in pkzip.
  // only usable with polynom orders of 8, 16, 24 or 32.

  ULong64_t crc = fCRCInit_direct;

  std::vector<char>::iterator it = p.begin();

  if (fRefIn) {
    crc = ReflectBitsStream(crc, fOrder);
  }

  if (!fRefIn) {
    while (len--) {
      crc = (crc << 8) ^ fCRCTab[((crc >> (fOrder - 8)) & 0xff) ^ (*it & 0xff)];
      it++;
    }
  } else {
    while (len--) {
      crc = (crc >> 8) ^ fCRCTab[(crc & 0xff) ^ (*it & 0xff)];
      it++;
    }
  }
  if (fRefOut ^ fRefIn) {
    crc = ReflectBitsStream(crc, fOrder);
  }
  crc ^= fCRCXor;
  crc &= fCRCMask;

  return (crc);
}

ULong64_t PndMvdReadInTBData::ReflectBitsStream(ULong64_t crc, int bitnum)
{

  // reflects the lower 'bitnum' bits of 'crc'

  ULong64_t i, j = 1, crcout = 0;

  for (i = (ULong64_t)1 << (bitnum - 1); i; i >>= 1) {
    if (crc & i) {
      crcout |= j;
    }
    j <<= 1;
  }
  return (crcout);
}

void PndMvdReadInTBData::AnalyzeToPixFrame(Double_t clockFrequency)
{
  if (fVerbose > 2) {
    std::cout << fFE << " PndMvdReadInTBData::AnalyzeToPixFrame: fToPixFrame size: " << std::dec << fToPixFrame.size() << " header " << std::hex << fToPixFrame[0] << std::endl;
  }

  ULong_t hammingcheck = CheckHammingCode(ConvertToPix4HammingToStandardHamming(fToPixFrame[0]), 40);
  if (hammingcheck != 0) {
    if (fVerbose > 1) {
      std::cout << "Wrong Hamming Code found! (Header) : " << std::hex << fToPixFrame[0] << " Parity bits " << hammingcheck << std::endl;
    }
    fHammingLossFrameCount++;
    fHammingLossHitCount += fToPixFrame.size() - 2;
    return;
  }

  hammingcheck = CheckHammingCode(ConvertToPix4HammingToStandardHamming(fToPixFrame[fToPixFrame.size() - 1]), 40);
  if (hammingcheck != 0) {
    if (fVerbose > 1) {
      std::cout << "Wrong Hamming Code found! (Trailer): " << std::hex << fToPixFrame[0] << " Parity bits " << hammingcheck << std::endl;
    }
    fHammingLossFrameCount++;
    fHammingLossHitCount += fToPixFrame.size() - 2;
    return;
  }

  PndSdsDigiTopix4 recentPixel;

  std::vector<char> topix_data; // vector necessary to do crc check

  for (int i = 1; i < fToPixFrame.size() - 1; i++) {
    for (int j = 0; j < 8; j++) {
      if (j == 0 or j == 1 or j == 2) {
        topix_data.push_back(0x00);
      } else {
        topix_data.push_back((fToPixFrame[i] >> (7 - j) * 2 * 4) & 0xff);
      }
    }
  }

  ULong64_t crc_calculated = CalculateCRCTableFast(topix_data, topix_data.size());

  if (crc_calculated != ((fToPixFrame.back() >> 6) & 0xffff)) {
    fCRCLossFrameCount++;
    fCRCLossHitCount += fToPixFrame.size() - 2;
    if (fVerbose == -1) {
      std::cout << fFE << " CRC WRONG! Frame will be deleted. Calculated CRC: " << std::hex << crc_calculated << " topix CRC: " << ((fToPixFrame.back() >> 6) & 0xffff)
                << std::endl;

      for (int y = 0; y < fToPixFrame.size(); y++) {
        if (y == 0) {
          std::cout << fFE << " " << std::hex << fToPixFrame[y] << " - FCount " << fRecentFrameHeader.fFrameCount << std::endl;
        } else {
          std::cout << fFE << " " << std::hex << fToPixFrame[y] << std::endl;
        }
      }
      std::cout << std::endl;
    }
    fToPixFrame.clear(); // delete topix frame due to negative CRC check.
    return;
  }

  for (int i = 0; i < fToPixFrame.size(); i++) {
    ULong64_t header = (fToPixFrame[i] & 0xC000000000) >> 38;

    switch (header) {
    case 1: {
      fRecentFrameHeader = BitAnalyzeHeader(fToPixFrame[i]);

      if (fVerbose > 2)
        std::cout << fFE << " FrameHeader: rawData: " << std::hex << fToPixFrame[i] << " chip " << std::dec << fRecentFrameHeader.fChipAddress << " framecount "
                  << fRecentFrameHeader.fFrameCount << std::endl;

      if (fOldFrameCount + 1 != fRecentFrameHeader.fFrameCount) {
        if (!(fOldFrameCount == 255 & fRecentFrameHeader.fFrameCount == 0)) {
          if (fVerbose > 1)
            std::cout << fFE << "-E- non sequential FC: " << fOldFrameCount << " " << fRecentFrameHeader.fFrameCount << std::endl;
          fNonSequentialFC++;
        }
      }

      if (fOldFrameCount > fRecentFrameHeader.fFrameCount) {
        fSuperFrameCount++;
        if (fVerbose > 1)
          std::cout << fFE << " SuperFrameCount increased: " << std::dec << fSuperFrameCount << " oldFC " << fOldFrameCount << " recent FC " << fRecentFrameHeader.fFrameCount
                    << std::endl;
      }
      // fOldFrameCount = fRecentFrameHeader.fFrameCount;
      Int_t deltaFrameCount = ((int)(fRecentFrameHeader.fFrameCount - fOldFrameCount) < 0 ? ((fRecentFrameHeader.fFrameCount - fOldFrameCount) + 256)
                                                                                          : (fRecentFrameHeader.fFrameCount - fOldFrameCount));
      //  new ((*fOutputArrayHeader)[fOutputArrayHeader->GetEntriesFast()]) PndSdsDigiTopix4Header(fRecentFrameHeader.fFrameCount, fFE, fRecentFrameHeader.fChipAddress,
      //  fRecentFrameHeader.fECC, fTotalFrameCount,deltaFrameCount, 0, fToPixFrame.size()-2 );
      new ((*fOutputArrayHeader)[fOutputArrayHeader->GetEntriesFast()])
        PndSdsDigiTopix4Header(fRecentFrameHeader.fFrameCount, 0, fFE, fRecentFrameHeader.fECC, fTotalFrameCount, deltaFrameCount, 0, fToPixFrame.size() - 2);
      if (deltaFrameCount > 1)
        LOG(warn) << " OutputArrayHeader deltaFrameCount > 1 " << deltaFrameCount;
      fOldFrameCount = fRecentFrameHeader.fFrameCount;
    } break;

    case 2: {
      fRecentFrameTrailer = BitAnalyzeTrailer(fToPixFrame[i]);
      if (fRecentFrameTrailer.fFrameCRC != 0 && (fToPixFrame.size() < 20)) {
        // std::cout << fFE << " Frame counter " << fRecentFrameHeader.fFrameCount << std::endl;
        if (fVerbose == -1) {
          for (int y = 0; y < fToPixFrame.size(); y++) {
            if (y == 0) {
              std::cout << fFE << " " << std::hex << fToPixFrame[y] << " - FCount " << fRecentFrameHeader.fFrameCount << std::endl;
            } else {
              std::cout << fFE << " " << std::hex << fToPixFrame[y] << std::endl;
            }
          }
          std::cout << std::endl;
        }
      }
      if (fVerbose > 2)
        std::cout << fFE << " FrameTrailer: nEvents " << fRecentFrameTrailer.fNEvents << " frame CRC: " << fRecentFrameTrailer.fFrameCRC << std::endl;

      PndSdsDigiTopix4Header *header_trailer = (PndSdsDigiTopix4Header *)(fOutputArrayHeader->Last());
      header_trailer->SetNumberOfEvents(fRecentFrameTrailer.fNEvents);
    } break;

    case 3:
      recentPixel = ProcessData(fToPixFrame[i], fRecentFrameHeader, clockFrequency);
      if (fVerbose > 2)
        std::cout << fFE << " Pixel: " << recentPixel << std::endl;
      new ((*fOutputArray)[fOutputArray->GetEntriesFast()]) PndSdsDigiTopix4(recentPixel);
      fCorrectHitCount++;
      break;
    }
  }
}

void PndMvdReadInTBData::AnalyzeData(std::vector<ULong64_t> &rawData, Double_t clockFrequency)
{
  if (fVerbose > 2)
    std::cout << "PndMvdReadInTBData::AnalyzeData rawData.size(): " << rawData.size() << std::endl;
  for (int i = 0; i < rawData.size(); i++) {
    ULong_t header = rawData[i] & 0xC000000000;
    header = header >> 38;
    if (fFirstHeader) {
      if (header == 1) {
        fFirstHeader = kFALSE;
      } else {
        continue;
      }
    }

    if (header == 1) // header word found
    {
      fTotalHeaderCount++;

      fRecentAllFrameHeader = BitAnalyzeHeader(rawData.at(i));

      Int_t deltaAllFrameCount = ((int)(fRecentAllFrameHeader.fFrameCount - fOldAllHeaderCount) < 0 ? ((fRecentAllFrameHeader.fFrameCount - fOldAllHeaderCount) + 256)
                                                                                                    : (fRecentAllFrameHeader.fFrameCount - fOldAllHeaderCount));
      new ((*fOutputArrayAllHeader)[fOutputArrayAllHeader->GetEntriesFast()])
        PndSdsDigiTopix4Header(fRecentAllFrameHeader.fFrameCount, fFE, fRecentAllFrameHeader.fChipAddress, fRecentAllFrameHeader.fECC, fTotalHeaderCount, deltaAllFrameCount, 0, 0);
      if (deltaAllFrameCount > 1)
        LOG(warn) << " deltaAllFrameCount > 1: " << deltaAllFrameCount;
      fOldAllHeaderCount = fRecentAllFrameHeader.fFrameCount;

      if (fHeaderPresent == kTRUE) {
        // double header found, cant check previous data without trailer, clear vector
        fDoubleHeader++;

        if (fVerbose > 1) {
          std::cout << "Double Header Found! count: " << fDoubleHeader << "| FE: " << fFE << std::hex << " last ToPixFrame element: " << fToPixFrame.back() << " new frame header "
                    << rawData[i] << std::endl;
        }
        fPreFrameLossHitCount += fToPixFrame.size() - 1;
        fToPixFrame.clear();
        // load new header into vector
        fToPixFrame.push_back(rawData[i]);
      } else {
        // header found, start recording topix frame
        fTrailerPresent = kFALSE;
        fHeaderPresent = kTRUE;
        fToPixFrame.push_back(rawData[i]);
      }
    }

    else if (header == 2) // trailer word found
    {
      fTotalTrailerCount++;
      if (fTrailerPresent == kTRUE) {
        // double trailer found, cant give the hits a valid timestamp without the header, clear vector

        fToPixFrame.clear();
        fDoubleTrailer++;
        if (fVerbose > 1) {
          std::cout << "Double Trailer Found! Double header counter: " << fDoubleTrailer << std::endl;
        }
      } else {
        if (fHeaderPresent == kTRUE) {
          // one topix frame found! Go and analyze the vector...
          fHeaderPresent = kFALSE;
          fTrailerPresent = kTRUE;
          fToPixFrame.push_back(rawData[i]);
          //  if (fVerbose > 1)
          //  {
          //	std::cout << "ToPix Frame found! Go and analyze this amont of data: " << fToPixFrame.size() << std::endl;
          //}

          fTotalFrameCount++;
          AnalyzeToPixFrame(clockFrequency);
          fToPixFrame.clear();
        } else {
          if (fVerbose > 1) {
            std::cout << "Trailer without header found! Double header counter: " << fDoubleHeader << std::endl;
          }
          // trailer without header, can happen at the beginning of the file or the header was not detected correctly
          // this case is in principle impossible to enter
          fPreFrameLossHitCount += fToPixFrame.size() - 1;
          fDoubleTrailer++;
          fToPixFrame.clear();
          continue;
        }
      }
    } else if (header == 3) // data word found
    {
      fTotalHitCount++;
      if (fHeaderPresent == kTRUE) {
        // found data while a active header is present, go and save the data
        fToPixFrame.push_back(rawData[i]);

      } else {
        // found data without a valid header, may happen at the beginning of the file or the header was detected
        fPreFrameLossHitCount++;
        continue;
      }
    } else {
      // not interessting
    }
  }
}

frameHeader PndMvdReadInTBData::BitAnalyzeHeader(ULong64_t &header)
{
  frameHeader tempHeader;
  ULong_t temp = header;
  tempHeader.fECC = temp & 0x3F;
  temp = temp >> 18;
  tempHeader.fFrameCount = temp & 0xFF;

  temp = temp >> 8;
  tempHeader.fChipAddress = temp & 0xFFF;

  return tempHeader;
}

frameTrailer PndMvdReadInTBData::BitAnalyzeTrailer(ULong64_t &trailer)
{
  frameTrailer tempTrailer;
  ULong_t temp = trailer;
  tempTrailer.fECC = temp & 0x3F;

  temp = temp >> 6;
  tempTrailer.fFrameCRC = temp & 0xFFFF;

  temp = temp >> 16;
  tempTrailer.fNEvents = temp & 0xFFFF;

  return tempTrailer;
}

pixel PndMvdReadInTBData::BitAnalyzePixelData(ULong64_t &data)
{
  // le_dataword = ((dataword & 0x0000000000fff000)>>12);¬
  // te_dataword = (dataword & 0x0000000000000fff);¬
  // pixeladdress = ((dataword & 0x0000003fff000000)>>24);¬

  pixel tempPixel;
  ULong_t temp = data;
  tempPixel.fTrailingEdge = mrftools::grayToBin(temp & 0X0000000000000FFF);

  temp = temp >> 12;
  tempPixel.fLeadingEdge = mrftools::grayToBin(temp & 0X0000000000000FFF);

  temp = temp >> 12;
  tempPixel.fPixelAddress = temp & 0X00000000000003FFF;

  //	if (fVerbose > 1) std::cout << "BitAnalyzePixelData: " << std::hex << data << " pixel " << std::dec << tempPixel.fPixelAddress << " " << tempPixel.fLeadingEdge << " " <<
  // tempPixel.fTrailingEdge << std::endl;

  return tempPixel;
}

PndSdsDigiTopix4 PndMvdReadInTBData::ProcessData(ULong64_t &data, frameHeader &header, Double_t &clockFrequency)
{
  if (fVerbose > 1)
    std::cout << "PndMvdReadInTBData::ProcessData raw Data: " << data << std::endl;
  pixel pixelData = BitAnalyzePixelData(data);
  std::pair<UInt_t, UInt_t> pixelAddress = PixeladdressToMatrixAddress(pixelData.fPixelAddress);
  Double_t timestamp = ((Double_t)fSuperFrameCount * 256. * 4096. + (Double_t)header.fFrameCount * 4096. + (Double_t)pixelData.fLeadingEdge) / clockFrequency * 1000.;
  Double_t timestamp_independent = ((Double_t)fTotalHeaderCount * 4096. + (Double_t)pixelData.fLeadingEdge) / clockFrequency * 1000.;

  std::vector<Int_t> indices; // just for compatibility with PndSdsDigiPixel
  //	return PndSdsDigiTopix4(indices, 0, 0, fFE, pixelAddress.first, pixelAddress.second, pixelData.fLeadingEdge, pixelData.fTrailingEdge, header.fFrameCount, timestamp,
  // fCorrectHitCount,fTotalHitCount, timestamp_independent);
  return PndSdsDigiTopix4(indices, 0, fFE - 1, 0, pixelAddress.first, pixelAddress.second, pixelData.fLeadingEdge, pixelData.fTrailingEdge, header.fFrameCount, timestamp,
                          fCorrectHitCount, fTotalHitCount, timestamp_independent);
}

std::pair<UInt_t, UInt_t> PndMvdReadInTBData::PixeladdressToMatrixAddress(UInt_t pixelglobaladdress)
{
  // Matrix: 32 columns x 20 rows

  UInt_t double_column_address = 0;
  UInt_t double_column_side = 0;
  UInt_t pixel_address = 0;

  UInt_t matrix_column, matrix_row;

  UInt_t temp = pixelglobaladdress;

  pixel_address = temp & 0x7f; // todo check if this conversion is correct!
  temp = temp >> 7;
  double_column_side = temp & 0x1;
  temp = temp >> 1;
  double_column_address = temp & 0x3f;
  temp = temp >> 6;

  //   if (pixel_address > 127) {
  //   	std::cout << "-E- PndMvdReadInTBData::PixeladdressToMatrixAddress PixelAddress > 128 " << pixel_address << std::endl;
  //   }

  //   temp = temp >> 6;

  //   if (fVerbose > 1) std::cout << "PixeladdressToMatrix rawData " << pixelglobaladdress << " dc " << double_column_address << " dcs " << double_column_side << " pixel " <<
  //   pixel_address << std::endl;

  UInt_t sel = (double_column_address << 1) | (double_column_side);

  if (sel == 0) {
    matrix_row = pixel_address;
    matrix_column = 1;
  } else if (sel == 1) {
    matrix_row = pixel_address;
    matrix_column = 0;
  } else if (sel == 6) {
    matrix_row = pixel_address;
    matrix_column = 19;
  } else if (sel == 7) {
    matrix_row = pixel_address;
    matrix_column = 18;
  } else if (sel == 2) {
    if (pixel_address < 32) {
      matrix_row = pixel_address;
      matrix_column = 3;
    } else if (pixel_address < 64) {
      matrix_row = 31 - (pixel_address - 32);
      matrix_column = 4;
    } else if (pixel_address < 96) {
      matrix_row = (pixel_address - 64);
      matrix_column = 7;
    } else if (pixel_address < 128) {
      matrix_row = 31 - (pixel_address - 96);
      matrix_column = 8;
    }
  } else if (sel == 3) {
    if (pixel_address < 32) {
      matrix_row = pixel_address;
      matrix_column = 2;
    } else if (pixel_address < 64) {
      matrix_row = 31 - (pixel_address - 32);
      matrix_column = 5;
    } else if (pixel_address < 96) {
      matrix_row = (pixel_address - 64);
      matrix_column = 6;
    } else if (pixel_address < 128) {
      matrix_row = 31 - (pixel_address - 96);
      matrix_column = 9;
    }
  } else if (sel == 4) {
    if (pixel_address < 32) {
      matrix_row = pixel_address;
      matrix_column = 10;
    } else if (pixel_address < 64) {
      matrix_row = 31 - (pixel_address - 32);
      matrix_column = 13;
    } else if (pixel_address < 96) {
      matrix_row = (pixel_address - 64);
      matrix_column = 14;
    } else if (pixel_address < 128) {
      matrix_row = 31 - (pixel_address - 96);
      matrix_column = 17;
    }
  } else if (sel == 5) {
    if (pixel_address < 32) {
      matrix_row = pixel_address;
      matrix_column = 11;
    } else if (pixel_address < 64) {
      matrix_row = 31 - (pixel_address - 32);
      matrix_column = 12;
    } else if (pixel_address < 96) {
      matrix_row = (pixel_address - 64);
      matrix_column = 15;
    } else if (pixel_address < 128) {
      matrix_row = 31 - (pixel_address - 96);
      matrix_column = 16;
    }
  }

  return std::pair<UInt_t, UInt_t>(matrix_column, matrix_row);
}
