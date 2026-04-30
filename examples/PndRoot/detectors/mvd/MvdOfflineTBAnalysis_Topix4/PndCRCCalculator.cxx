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


#include <MvdOfflineTBAnalysis_Topix4/PndCRCCalculator.h>
#include <vector>
#include <iostream>
#include <cmath>

ClassImp(PndCRCCalculator);

PndCRCCalculator::PndCRCCalculator() : fOrder(16), fPolynom(0x8005), fCRCXor(0x0000), fRefIn(0), fRefOut(0), fCRCInit_direct(0)
{
  fCRCMask = ((((unsigned long)1 << (fOrder - 1)) - 1) << 1) | 1;
  fCRCHighBit = (unsigned long)1 << (fOrder - 1);

  GenerateCRCTable();
}

PndCRCCalculator::PndCRCCalculator(UInt_t order, ULong64_t polynom, ULong64_t CRCXor, UInt_t refIn, UInt_t refOut, UInt_t CRCInit_direct)
  : fOrder(order), fPolynom(polynom), fCRCXor(CRCXor), fRefIn(refIn), fRefOut(refOut), fCRCInit_direct(CRCInit_direct)
{
  fCRCMask = ((((unsigned long)1 << (fOrder - 1)) - 1) << 1) | 1;
  fCRCHighBit = (unsigned long)1 << (fOrder - 1);

  GenerateCRCTable();
}

PndCRCCalculator::~PndCRCCalculator()
{
  // TODO Auto-generated destructor stub
}

// std::vector<char> PndCRCCalculator::ConvertData(std::vector<ULong64_t> topixFrame)
//{
//	std::vector<char> topix_data;
//	for (int i = 1; i < topixFrame.size() - 1; i++) {
//		for (int j = 0; j < 8; j++) {
//			if (j == 0 or j == 1 or j == 2) {
//				topix_data.push_back(0x00);
//			} else {
//				topix_data.push_back((topixFrame[i] >> (7 - j) * 2 * 4) & 0xff);
//			}
//		}
//	}
//	return topix_data;
//}

ULong64_t PndCRCCalculator::CalculateCRCTableFast(std::vector<char> p, ULong64_t len)
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

void PndCRCCalculator::GenerateCRCTable()
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

void PndCRCCalculator::PrintCRCTable()
{
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      std::cout << std::hex << fCRCTab[i * 16 + j] << " ";
    }
    std::cout << std::endl;
  }
}

ULong64_t PndCRCCalculator::ReflectBitsStream(ULong64_t crc, int bitnum)
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
