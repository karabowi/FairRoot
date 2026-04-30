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
 * PndHammingEncoder.cxx
 *
 *  Created on: 16.09.2015
 *      Author: Stockmanns
 */

#include <MvdOfflineTBAnalysis_Topix4/PndHammingDecoder.h>
#include <vector>
#include <iostream>
#include <cmath>

PndHammingDecoder::PndHammingDecoder() : fOrder(16), fPolynom(0x8005), fCRCXor(0x0000), fRefIn(0), fRefOut(0), fCRCInit_direct(0)
{
  fCRCMask = ((((unsigned long)1 << (fOrder - 1)) - 1) << 1) | 1;
  fCRCHighBit = (unsigned long)1 << (fOrder - 1);

  GenerateCRCTable();
}

PndHammingDecoder::~PndHammingDecoder()
{
  // TODO Auto-generated destructor stub
}

UShort_t PndHammingDecoder::CheckHammingCode(ULong64_t dataword, int dataword_length)
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

std::vector<char> PndHammingDecoder::ConvertData(std::vector<ULong64_t> topixFrame)
{
  std::vector<char> topix_data;
  for (unsigned int i = 1; i < topixFrame.size() - 1; i++) {
    for (int j = 0; j < 8; j++) {
      if (j == 0 or j == 1 or j == 2) {
        topix_data.push_back(0x00);
      } else {
        topix_data.push_back((topixFrame[i] >> (7 - j) * 2 * 4) & 0xff);
      }
    }
  }
  return topix_data;
}

ULong64_t PndHammingDecoder::CalculateCRCTableFast(std::vector<char> p, ULong64_t len)
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

void PndHammingDecoder::GenerateCRCTable()
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

ULong64_t PndHammingDecoder::ReflectBitsStream(ULong64_t crc, int bitnum)
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
