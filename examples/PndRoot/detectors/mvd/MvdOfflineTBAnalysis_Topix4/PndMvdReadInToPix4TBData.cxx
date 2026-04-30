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
 * PndMvdReadInToPix4TBData.cxx
 *
 *  Created on: 23.10.2014
 *      Author: Stockmanns
 */

#include <MvdOfflineTBAnalysis_Topix4/PndMvdReadInToPix4TBData.h>

#include "mrfdata_8b.h"
#include "mrftools.h"
#include "boost/archive/binary_oarchive.hpp"
#include "boost/archive/binary_iarchive.hpp"
#include "boost/serialization/binary_object.hpp"
#include <boost/archive/archive_exception.hpp>
#include "FairMQLogger.h"

using namespace ToPix4;

PndMvdReadInToPix4TBData::PndMvdReadInToPix4TBData()
  : fClockFrequency(0), fSuperFrameCount(0), fOldFrameCount(0), fOldAllHeaderCount(0), fFirstHeader(kTRUE), fFE(-1), fNonSequentialFC(0), fHammingLossFrameCount(0),
    fCRCLossFrameCount(0), fTotalHitCount(0), fPreFrameLossHitCount(0), fHammingLossHitCount(0), fCRCLossHitCount(0), fCorrectHitCount(0), fHeaderPresent(kFALSE),
    fTrailerPresent(kFALSE), fDoubleHeader(0), fDoubleTrailer(0), fVerbose(0), fFileCounter(0), fTotalFrameCount(0), fTotalHeaderCount(0), fTotalTrailerCount(0), fFileHandle(0),
    fTimeStampCorrection(0.0), fFilter(kFALSE), fNFilteredHits(0)
{
  fStatusValues.resize(Last, 0);
}

PndMvdReadInToPix4TBData::~PndMvdReadInToPix4TBData()
{
  if (fFileHandle != 0) {
    fFileHandle->close();
    delete (fFileHandle);
  }
}

void PndMvdReadInToPix4TBData::Init()
{
  // std::cout << "PndMvdReadInToPix4TBData::Init called" << std::endl;
  //	for (int i = 0; i < fFileName.size(); i++){
  std::ifstream *ifs = new std::ifstream(fFileNames[fFileCounter], std::ios::binary);
  if (ifs->good() == kFALSE)
    LOG(error) << "File: " << fFileNames[fFileCounter] << " is good: " << ifs->good();
  fFileCounter++;
  fFileHandle = ifs;
  //	}
}

Bool_t PndMvdReadInToPix4TBData::ReadInData(std::vector<std::vector<PndSdsDigiTopix4>> &data)
{
  ULong_t dataword = 0;
  Bool_t endOfFile = kFALSE;

  //	fOutputArrayAllHeader = allheaderContainer;
  std::vector<ULong64_t> rawArray;
  TMrfData_8b *mrfData = 0;
  endOfFile |= ReadInDataFromFile(mrfData);
  if (mrfData != 0) {
    rawArray = fTopix.GetRawData(mrfData);
    delete (mrfData);
  }
  //	endOfFile |= ReadInRawData(fFileHandle, rawArray);
  data = AnalyzeData(rawArray, fClockFrequency);
  return endOfFile;
}

Bool_t PndMvdReadInToPix4TBData::ReadInDataFromFile(TMrfData_8b *&data)
{
  Bool_t endOfFile = kFALSE;

  if (fFileHandle->good()) {
    if (fVerbose > 1)
      LOG(debug) << "PndMvdReadInToPix4TBData::ReadInRawData reading file ";
    try {
      boost::archive::binary_iarchive iar(*fFileHandle); // this line causes an "Invalid Signature Error" at the end of the file but the file is still good
      iar >> data;
    } catch (boost::archive::archive_exception &exception) {
      LOG(warning) << "PndMvdReadInToPix4TBData::ReadInRawData: Error found in reading file "
                   << " : " << fFileHandle->good() << " " << fFileHandle->eof() << " Exception: " << exception.code << " " << exception.what();

      if (exception.code == 3) {
        if (fFileCounter < fFileNames.size()) {
          LOG(info) << fFE << " open new file " << fFileNames[fFileCounter];
          fFileHandle->close();
          delete (fFileHandle);
          std::ifstream *ifs = new std::ifstream(fFileNames[fFileCounter], std::ios::binary);
          fFileHandle = ifs;
          fFileCounter++;
          return endOfFile;
        } else {
          LOG(info) << fFE << " All files read! Finishing FE ";
          endOfFile = kTRUE;
          return endOfFile;
        }
      }
      return kTRUE;
    }
  } else {
    LOG(error) << fFE << " An error occured ";
    LOG(error) << fFE << " fFileHandle->good() " << fFileHandle->good();
    LOG(error) << fFE << " fFileHandle->eof()  " << fFileHandle->eof();
    LOG(error) << fFE << " fFileHandle->fail() " << fFileHandle->fail();
    LOG(error) << fFE << " fFileHandle->bad()  " << fFileHandle->bad();

    endOfFile = kTRUE;
    return endOfFile;
  }
  return endOfFile;
}

// std::vector<ULong64_t> PndMvdReadInToPix4TBData::GetRawData(TMrfData_8b* data)
//{
//	std::vector<ULong64_t> rawData;
//	for (UInt_t i = 0; i < data->getNumWords(); i += 5) {
//		ULong_t dataword = 0;
//		for (uint j = 0; j < 5; j++) {
//			dataword = dataword << 8;
//			dataword += data->getWord(i + j);
//		}
//		rawData.push_back(dataword);
//
//		if (fVerbose > 2) {
//			ULong_t frameCount = -1;
//			ULong64_t header = fTopix.GetHeader(dataword);
//			if (header == 1)
//				frameCount = fTopix.GetFrameCount(dataword);
//
//			LOG(info) << std::dec << "dataword No " << i / 5 << "/"	<< data->getNumWords() / 5 << ": " << std::hex
//					<< dataword << " ";
//		}
//	}
//	return rawData;
//}

// Bool_t PndMvdReadInToPix4TBData::ReadInRawData(std::ifstream* fileHandle, std::vector<ULong64_t>& rawData) {
//	TMrfData_8b* tempdata;
//	//tempdata = new TMrfData_8b;
//	ULong_t dataword = 0;
//	Bool_t endOfFile = kFALSE;
//	//fVerbose = 3;
//	if (fileHandle->good()) {
//		if (fVerbose > 2)
//			LOG(debug) << "PndMvdReadInToPix4TBData::ReadInRawData reading file ";
//
//		try {
//			boost::archive::binary_iarchive iar(*fileHandle); //this line causes an "Invalid Signature Error" at the end of the file but the file is still good
//			iar >> tempdata;
//		} catch (boost::archive::archive_exception& exception) {
//			LOG(warning) << "PndMvdReadInToPix4TBData::ReadInRawData: Error found in reading file "
//						<< " : " << fileHandle->good() << " " << fileHandle->eof() << " Exception: "
//						<< exception.code << " " << exception.what();
//
//			if (exception.code == 3) {
//				if (fFileCounter < fFileNames.size()) {
//					LOG(info) << fFE << " open new file " << fFileNames[fFileCounter];
//					fileHandle->close();
//					delete (fFileHandle);
//					std::ifstream* ifs = new std::ifstream(fFileNames[fFileCounter], std::ios::binary);
//					fFileCounter++;
//					return endOfFile;
//				} else {
//					LOG(info) << fFE << " All files read! Finishing FE ";
//					endOfFile = kTRUE;
//					return endOfFile;
//				}
//			}
//			return kTRUE;
//		}
//		if (fVerbose > 2)
//			LOG(debug) << fFE << " PndMvdReadInToPix4TBData::ReadInRawData: NWords: " << tempdata->getNumWords();
//		for (UInt_t i = 0; i < tempdata->getNumWords(); i += 5) {
//			dataword = 0;
//			for (uint j = 0; j < 5; j++) {
//				dataword = dataword << 8;
//				dataword += tempdata->getWord(i + j);
//			}
//			rawData.push_back(dataword);
//
//			if (fVerbose > 2) {
//				ULong_t frameCount = -1;
//				ULong64_t header = fTopix.GetHeader(dataword);
//				if (header == 1)
//					frameCount = fTopix.GetFrameCount(dataword);
//
//				LOG(debug) << std::dec << "dataword No " << i / 5 << "/"	<< tempdata->getNumWords() / 5 << ": " << std::hex
//						<< dataword << " " << std::dec << header << " : " << frameCount;
//			}
//		}
//		delete(tempdata);
//	} else {
//		LOG(error)<< fFE << " An error occured ";
//		LOG(error) << fFE << " fileHandle->good() " << fileHandle->good();
//		LOG(error) << fFE << " fileHandle->eof()  " << fileHandle->eof();
//		LOG(error) << fFE << " fileHandle->fail() " << fileHandle->fail();
//		LOG(error) << fFE << " fileHandle->bad()  " << fileHandle->bad();
//
//		endOfFile = kFALSE;
//		return endOfFile;
//	}
//	return endOfFile;
//}

std::vector<std::vector<PndSdsDigiTopix4>> PndMvdReadInToPix4TBData::AnalyzeData(std::vector<ULong64_t> &rawData, Double_t clockFrequency)
{
  std::vector<std::vector<PndSdsDigiTopix4>> result;
  if (fVerbose > 2)
    LOG(debug) << "PndMvdReadInToPix4TBData::AnalyzeData rawData.size(): " << rawData.size();
  for (int i = 0; i < rawData.size(); i++) {
    if (BuildFrame(rawData[i]) == true) { // a frame was found
      std::vector<PndSdsDigiTopix4> hitList = AnalyzeToPixFrame(clockFrequency);
      // LOG(info) << "TestData: " << hitList.front() << std::endl;
      result.push_back(hitList);
      fToPixFrame.clear();
    }
  }
  return result;
}

bool PndMvdReadInToPix4TBData::BuildFrame(ULong64_t &rawData)
{
  int header = fTopix.GetType(rawData);
  if (fFirstHeader) { // if data stream does not start with a header all data is thrown away until a header is present
    if (header == 1) {
      fFirstHeader = kFALSE;
    } else {
      return false;
    }
  }

  if (header == 1) // header word found
  {
    fStatusValues[TotalHeaderCount]++;

    ToPix4::frameHeader frameHeader = fTopix.BitAnalyzeHeader(rawData);

    Int_t deltaAllFrameCount =
      ((int)(frameHeader.fFrameCount - fOldAllHeaderCount) < 0 ? ((frameHeader.fFrameCount - fOldAllHeaderCount) + 256) : (frameHeader.fFrameCount - fOldAllHeaderCount));
    //		new ((*fOutputArrayAllHeader)[fOutputArrayAllHeader->GetEntriesFast()]) PndSdsDigiTopix4Header(frameHeader.fFrameCount, fFE,
    //				frameHeader.fChipAddress, frameHeader.fECC, fTotalHeaderCount, deltaAllFrameCount, 0, 0);
    if (deltaAllFrameCount > 1 && fVerbose > 0)
      LOG(warn) << " deltaAllFrameCount > 1: " << deltaAllFrameCount;
    fOldAllHeaderCount = frameHeader.fFrameCount;

    if (fHeaderPresent == kTRUE) {
      // double header found, cannot check previous data without trailer, clear vector
      fDoubleHeader++;
      fStatusValues[DoubleHeader]++;

      if (fVerbose > 1) {
        std::cout << "Double Header Found! count: " << fDoubleHeader << "| FE: " << fFE << std::hex << " last ToPixFrame element: " << fToPixFrame.back() << " new frame header "
                  << rawData << std::endl;
      }
      fPreFrameLossHitCount += fToPixFrame.size() - 1;
      fToPixFrame.clear();
      // load new header into vector
      fToPixFrame.push_back(rawData);
    } else {
      // header found, start recording topix frame
      fTrailerPresent = kFALSE;
      fHeaderPresent = kTRUE;
      fToPixFrame.push_back(rawData);
    }
  }

  else if (header == 2) // trailer word found
  {
    fTotalTrailerCount++;
    fStatusValues[TotalTrailerCount]++;
    if (fTrailerPresent == kTRUE) {
      // double trailer found, cannot give the hits a valid timestamp without the header, clear vector

      fToPixFrame.clear();
      fStatusValues[DoubleTrailer]++;

      if (fVerbose > 1) {
        std::cout << "Double Trailer Found! Double header counter: " << fDoubleTrailer << std::endl;
      }
    } else {
      if (fHeaderPresent == kTRUE) {
        // one topix frame found! Go and analyze the vector...
        fHeaderPresent = kFALSE;
        fTrailerPresent = kTRUE;
        fToPixFrame.push_back(rawData);
        //  if (fVerbose > 1)
        //  {
        //	std::cout << "ToPix Frame found! Go and analyze this amount of data: " << fToPixFrame.size() << std::endl;
        //}

        fStatusValues[TotalFrameCount]++;
        // AnalyzeToPixFrame(clockFrequency);
        return true;
      } else {
        if (fVerbose > 1) {
          std::cout << "Trailer without header found! Double header counter: " << fDoubleHeader << std::endl;
        }
        // trailer without header, can happen at the beginning of the file or the header was not detected correctly
        // this case is in principle impossible to enter
        fPreFrameLossHitCount += fToPixFrame.size() - 1;
        fDoubleTrailer++;
        fStatusValues[DoubleTrailer]++;
        fToPixFrame.clear();
      }
    }
  } else if (header == 3) // data word found
  {
    fTotalHitCount++;
    fStatusValues[TotalHitCount]++;
    if (fHeaderPresent == kTRUE) { // found data while a active header is present, go and save the data
      fToPixFrame.push_back(rawData);
    } else { // found data without a valid header, may happen at the beginning of the file or the header was detected
      fPreFrameLossHitCount++;
      fStatusValues[PreFrameLossHitCount]++;
    }
  }
  return false;
}

std::vector<PndSdsDigiTopix4> PndMvdReadInToPix4TBData::AnalyzeToPixFrame(Double_t clockFrequency)
{
  std::vector<PndSdsDigiTopix4> hitList;
  if (fVerbose > 2) {
    std::cout << fFE << " PndMvdReadInToPix4TBData::AnalyzeToPixFrame: fToPixFrame size: " << std::dec << fToPixFrame.size() << " header " << std::hex << fToPixFrame[0]
              << std::endl;
  }

  if (CheckDataIntegrity(fToPixFrame) != true) {
    fToPixFrame.clear();
    return hitList;
  }

  for (int i = 0; i < fToPixFrame.size(); i++) {
    ULong64_t header = fTopix.GetType(fToPixFrame[i]);

    switch (header) {
    case 1: {
      fRecentFrameHeader = fTopix.BitAnalyzeHeader(fToPixFrame[i]);

      if (fVerbose > 2)
        std::cout << fFE << " FrameHeader: rawData: " << std::hex << fToPixFrame[i] << " chip " << std::dec << fRecentFrameHeader.fChipAddress << " framecount "
                  << fRecentFrameHeader.fFrameCount << std::endl;

      Int_t deltaFrameCount = GetDeltaFrameCount();
      if (deltaFrameCount > 1) {
        if (fVerbose > 1)
          std::cout << fFE << "-E- non sequential FC: " << fOldFrameCount << " " << fRecentFrameHeader.fFrameCount << std::endl;
        fNonSequentialFC++;
        fStatusValues[NonSequentialFC]++;
      }

      if (fOldFrameCount > fRecentFrameHeader.fFrameCount) {
        fSuperFrameCount++;
        fStatusValues[SuperFrameCount]++;
        if (fVerbose > 1)
          std::cout << fFE << " SuperFrameCount increased: " << std::dec << fSuperFrameCount << " oldFC " << fOldFrameCount << " recent FC " << fRecentFrameHeader.fFrameCount
                    << std::endl;
      }
      // fOldFrameCount = fRecentFrameHeader.fFrameCount;

      //  new ((*fOutputArrayHeader)[fOutputArrayHeader->GetEntriesFast()]) PndSdsDigiTopix4Header(fRecentFrameHeader.fFrameCount, fFE, fRecentFrameHeader.fChipAddress,
      //  fRecentFrameHeader.fECC, fTotalFrameCount,deltaFrameCount, 0, fToPixFrame.size()-2 );
      // new ((*fOutputArrayHeader)[fOutputArrayHeader->GetEntriesFast()]) PndSdsDigiTopix4Header(fRecentFrameHeader.fFrameCount, 0, fFE,
      //		fRecentFrameHeader.fECC, fTotalFrameCount, deltaFrameCount, 0, fToPixFrame.size() - 2);

      fOldFrameCount = fRecentFrameHeader.fFrameCount;
    } break;

    case 2: {
      fRecentFrameTrailer = fTopix.BitAnalyzeTrailer(fToPixFrame[i]);
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

      // PndSdsDigiTopix4Header * header_trailer = (PndSdsDigiTopix4Header*) (fOutputArrayHeader->Last());
      // header_trailer->SetNumberOfEvents(fRecentFrameTrailer.fNEvents);
    } break;

    case 3:
      PndSdsDigiTopix4 recentPixel = ProcessData(fToPixFrame[i], fRecentFrameHeader, clockFrequency);
      if (fVerbose > 1)
        LOG(info) << "RecentPixel: " << recentPixel;
      if (fFilter == kTRUE) {
        if (HitToFilter(recentPixel) == true) {
          // LOG(info) << "Hit to Filter found!";
          fNFilteredHits++;
          fStatusValues[NFilteredHits]++;
          break;
        }
      }
      hitList.push_back(recentPixel);
      if (fVerbose > 2)
        std::cout << fFE << " Pixel: " << recentPixel << std::endl;
      // WriteoutToPix4Digi(recentPixel);
      fCorrectHitCount++;
      fStatusValues[CorrectHitCount]++;
      break;
    }
  }
  return hitList;
}

Bool_t PndMvdReadInToPix4TBData::HitToFilter(PndSdsDigiTopix4 &hit)
{
  int leadingEdge = hit.GetLeadingEdge();
  int trailingEdge = hit.GetTrailingEdge();
  if (leadingEdge == 2729 || leadingEdge == 2730)
    return true;
  if (trailingEdge == 2730 || trailingEdge == 2731)
    return true;
  return false;
}

Int_t PndMvdReadInToPix4TBData::GetDeltaFrameCount()
{
  return ((int)(fRecentFrameHeader.fFrameCount - fOldFrameCount) < 0 ? ((fRecentFrameHeader.fFrameCount - fOldFrameCount) + 256)
                                                                     : (fRecentFrameHeader.fFrameCount - fOldFrameCount));
}

bool PndMvdReadInToPix4TBData::CheckDataIntegrity(std::vector<ULong64_t> topix4Frame)
{
  ULong_t hammingcheck = fHamming.CheckHammingCode(fTopix.ConvertToPix4HammingToStandardHamming(topix4Frame[0]), 40); // check hamming of header
  if (hammingcheck != 0) {
    if (fVerbose > 1) {
      std::cout << "Wrong Hamming Code found! (Header) : " << std::hex << topix4Frame[0] << " Parity bits " << hammingcheck << std::endl;
    }
    fHammingLossFrameCount++;
    fStatusValues[HammingLossFrameCount]++;
    fHammingLossHitCount += topix4Frame.size() - 2;
    fStatusValues[HammingLossHitCount] += topix4Frame.size() - 2;
    return false;
  }

  hammingcheck = fHamming.CheckHammingCode(fTopix.ConvertToPix4HammingToStandardHamming(topix4Frame[topix4Frame.size() - 1]), 40); // check hamming of trailer
  if (hammingcheck != 0) {
    if (fVerbose > 1) {
      std::cout << "Wrong Hamming Code found! (Trailer): " << std::hex << topix4Frame[0] << " Parity bits " << hammingcheck << std::endl;
    }
    fHammingLossFrameCount++;
    fStatusValues[HammingLossFrameCount]++;
    fHammingLossHitCount += topix4Frame.size() - 2;
    fStatusValues[HammingLossHitCount] += topix4Frame.size() - 2;
    return false;
  }

  std::vector<char> topix_data = fHamming.ConvertData(topix4Frame);
  ULong64_t crc_calculated = fHamming.CalculateCRCTableFast(topix_data, topix_data.size());

  if (crc_calculated != ((topix4Frame.back() >> 6) & 0xffff)) {
    fCRCLossFrameCount++;
    fStatusValues[CRCLossFrameCount]++;
    fCRCLossHitCount += topix4Frame.size() - 2;
    fStatusValues[CRCLossHitCount] += topix4Frame.size() - 2;
    if (fVerbose == -1) {
      std::cout << fFE << " CRC WRONG! Frame will be deleted. Calculated CRC: " << std::hex << crc_calculated << " topix CRC: " << ((topix4Frame.back() >> 6) & 0xffff)
                << std::endl;

      for (int y = 0; y < topix4Frame.size(); y++) {
        if (y == 0) {
          std::cout << fFE << " " << std::hex << topix4Frame[y] << " - FCount " << fRecentFrameHeader.fFrameCount << std::endl;
        } else {
          std::cout << fFE << " " << std::hex << topix4Frame[y] << std::endl;
        }
      }
      std::cout << std::endl;
    }
    topix4Frame.clear(); // delete topix frame due to negative CRC check.
    return false;
  }
  return true;
}

PndSdsDigiTopix4 PndMvdReadInToPix4TBData::ProcessData(ULong64_t &data, ToPix4::frameHeader &header, Double_t &clockFrequency)
{
  if (fVerbose > 1)
    std::cout << "PndMvdReadInToPix4TBData::ProcessData raw Data: " << data << std::endl;
  pixel pixelData = fTopix.BitAnalyzePixelData(data);
  std::pair<UInt_t, UInt_t> pixelAddress = fTopix.PixelNumberToMatrixAddress(pixelData.fPixelNumber);

  Int_t frameCountHeader = header.fFrameCount;
  if (pixelData.fLeadingEdge > pixelData.fTrailingEdge) {
    frameCountHeader--;
    // std::cout << "PndMvdReadInToPix4TBData::ProcessData frameCount corrected" << std::endl;
  }
  Double_t timestamp = ((Double_t)fSuperFrameCount * 256. * 4096. + (Double_t)frameCountHeader * 4096. + (Double_t)pixelData.fLeadingEdge) / clockFrequency * 1000.;
  timestamp += fTimeStampCorrection;
  Double_t timestamp_independent = ((Double_t)fTotalHeaderCount * 4096. + (Double_t)pixelData.fLeadingEdge) / clockFrequency * 1000.;

  std::vector<Int_t> indices; // just for compatibility with PndSdsDigiPixel
  //	return PndSdsDigiTopix4(indices, 0, 0, fFE, pixelAddress.first, pixelAddress.second, pixelData.fLeadingEdge, pixelData.fTrailingEdge, header.fFrameCount, timestamp,
  // fCorrectHitCount,fTotalHitCount, timestamp_independent);
  PndSdsDigiTopix4 result(indices, 0, fFE - 1, 0, pixelAddress.first, pixelAddress.second, pixelData.fLeadingEdge, pixelData.fTrailingEdge, header.fFrameCount, timestamp,
                          fCorrectHitCount, fTotalHitCount, timestamp_independent);
  result.SetTimeStampError(1 / clockFrequency * 1000); /// todo: check if sqrt(12) has to be added
  return result;
}
