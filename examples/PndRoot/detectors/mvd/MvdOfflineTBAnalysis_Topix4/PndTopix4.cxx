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
 * PndTopix4.cxx
 *
 *  Created on: 16.09.2015
 *      Author: Stockmanns
 */

#include <MvdOfflineTBAnalysis_Topix4/PndTopix4.h>

#include "mrftools.h"

using namespace ToPix4;

// ClassImp(PndTopix4)

PndTopix4::PndTopix4()
{
  // TODO Auto-generated constructor stub
}

PndTopix4::~PndTopix4()
{
  // TODO Auto-generated destructor stub
}

std::vector<ULong64_t> PndTopix4::GetRawData(TMrfData_8b *data)
{
  std::vector<ULong64_t> rawData;
  for (UInt_t i = 0; i < data->getNumWords(); i += 5) {
    ULong_t dataword = 0;
    for (UInt_t j = 0; j < 5; j++) {
      dataword = dataword << 8;
      dataword += data->getWord(i + j);
    }
    rawData.push_back(dataword);

    //		if (fVerbose > 2) {
    ULong_t frameCount = -1;
    ULong64_t header = GetType(dataword);
    if (header == 1)
      frameCount = GetFrameCount(dataword);

    //			LOG(info) << std::dec << "dataword No " << i / 5 << "/"	<< data->getNumWords() / 5 << ": " << std::hex
    //					<< dataword << " ";
    //		}
  }
  return rawData;
}

int PndTopix4::GetType(ULong64_t data)
{
  ULong_t header = data & 0xC000000000;
  header = header >> 38;
  return (int)header;
}

int PndTopix4::GetFrameCount(ULong64_t data)
{
  ULong_t frame = data & 0x3FC0000;
  frame = frame >> 18;
  return (int)frame;
}

ToPix4::frameHeader PndTopix4::BitAnalyzeHeader(ULong64_t &header)
{
  ToPix4::frameHeader tempHeader;
  ULong_t temp = header;
  tempHeader.fECC = temp & 0x3F;
  temp = temp >> 18;
  tempHeader.fFrameCount = temp & 0xFF;

  temp = temp >> 8;
  tempHeader.fChipAddress = temp & 0xFFF;

  return tempHeader;
}

ToPix4::frameTrailer PndTopix4::BitAnalyzeTrailer(ULong64_t &trailer)
{
  ToPix4::frameTrailer tempTrailer;
  ULong_t temp = trailer;
  tempTrailer.fECC = temp & 0x3F;

  temp = temp >> 6;
  tempTrailer.fFrameCRC = temp & 0xFFFF;

  temp = temp >> 16;
  tempTrailer.fNEvents = temp & 0xFFFF;

  return tempTrailer;
}

ToPix4::pixel PndTopix4::BitAnalyzePixelData(ULong64_t &data)
{
  // le_dataword = ((dataword & 0x0000000000fff000)>>12);¬
  // te_dataword = (dataword & 0x0000000000000fff);¬
  // pixeladdress = ((dataword & 0x0000003fff000000)>>24);¬

  ToPix4::pixel tempPixel;
  ULong_t temp = data;
  tempPixel.fTrailingEdge = mrftools::grayToBin(temp & 0X0000000000000FFF);

  temp = temp >> 12;
  tempPixel.fLeadingEdge = mrftools::grayToBin(temp & 0X0000000000000FFF);

  temp = temp >> 12;
  tempPixel.fPixelNumber = temp & 0X00000000000003FFF;

  //	if (fVerbose > 1) std::cout << "BitAnalyzePixelData: " << std::hex << data << " pixel " << std::dec << tempPixel.fPixelAddress << " " << tempPixel.fLeadingEdge << " " <<
  // tempPixel.fTrailingEdge << std::endl;

  return tempPixel;
}

ToPix4::pixelAddress PndTopix4::PixelNumberToPixelAddress(UInt_t pixelnumber)
{
  ToPix4::pixelAddress address;

  UInt_t temp = pixelnumber;

  address.fRow = temp & 0x7f; // todo check if this conversion is correct!
  temp = temp >> 7;
  address.fSide = temp & 0x1;
  temp = temp >> 1;
  address.fCol = temp & 0x3f;
  //	temp = temp >> 6;

  return address;
}

std::pair<UInt_t, UInt_t> PndTopix4::PixelNumberToMatrixAddress(UInt_t pixelnumber)
{
  // Matrix: 32 columns x 20 rows

  ToPix4::pixelAddress address;

  address = PixelNumberToPixelAddress(pixelnumber);
  //    UInt_t matrix_column, matrix_row;

  return PixelAddressToMatrixAddress(address);

  //    UInt_t sel = (double_column_address<<1) | (double_column_side);
  //
  //    if(sel == 0)
  //    {
  //        matrix_row = pixel_address;
  //        matrix_column = 1;
  //    }
  //    else if(sel ==1)
  //    {
  //        matrix_row = pixel_address;
  //        matrix_column = 0;
  //    }
  //    else if(sel == 6)
  //    {
  //        matrix_row = pixel_address;
  //        matrix_column = 19;
  //    }
  //    else if(sel == 7)
  //    {
  //        matrix_row = pixel_address;
  //        matrix_column = 18;
  //    }
  //    else if (sel==2)
  //    {
  //        if (pixel_address <32)
  //        {
  //            matrix_row = pixel_address;
  //            matrix_column = 3;
  //        }
  //        else if (pixel_address < 64)
  //        {
  //            matrix_row = 31 - (pixel_address-32);
  //            matrix_column = 4;
  //        }
  //        else if (pixel_address < 96)
  //        {
  //            matrix_row = (pixel_address-64);
  //            matrix_column = 7;
  //        }
  //        else if (pixel_address < 128)
  //        {
  //            matrix_row = 31 - (pixel_address-96);
  //            matrix_column = 8;
  //        }
  //    }
  //    else if (sel==3)
  //    {
  //        if (pixel_address <32)
  //        {
  //            matrix_row = pixel_address;
  //            matrix_column = 2;
  //        }
  //        else if (pixel_address < 64)
  //        {
  //            matrix_row = 31 - (pixel_address-32);
  //            matrix_column = 5;
  //        }
  //        else if (pixel_address < 96)
  //        {
  //            matrix_row = (pixel_address-64);
  //            matrix_column = 6;
  //        }
  //        else if (pixel_address < 128)
  //        {
  //            matrix_row = 31 - (pixel_address-96);
  //            matrix_column = 9;
  //        }
  //    }
  //    else if (sel==4)
  //    {
  //        if (pixel_address < 32)
  //        {
  //            matrix_row = pixel_address;
  //            matrix_column = 10;
  //        }
  //        else if (pixel_address < 64)
  //        {
  //            matrix_row = 31 - (pixel_address-32);
  //            matrix_column = 13;
  //        }
  //        else if (pixel_address < 96)
  //        {
  //            matrix_row = (pixel_address-64);
  //            matrix_column = 14;
  //        }
  //        else if (pixel_address < 128)
  //        {
  //            matrix_row = 31 - (pixel_address-96);
  //            matrix_column = 17;
  //        }
  //    }
  //    else if (sel==5)
  //    {
  //        if (pixel_address < 32)
  //        {
  //            matrix_row = pixel_address;
  //            matrix_column = 11;
  //        }
  //        else if (pixel_address < 64)
  //        {
  //            matrix_row = 31 - (pixel_address-32);
  //            matrix_column = 12;
  //        }
  //        else if (pixel_address < 96)
  //        {
  //            matrix_row = (pixel_address-64);
  //            matrix_column = 15;
  //        }
  //        else if (pixel_address < 128)
  //        {
  //            matrix_row = 31 - (pixel_address-96);
  //            matrix_column = 16;
  //        }
  //    }

  //    return std::pair<UInt_t, UInt_t>(matrix_column, matrix_row);
}

std::pair<UInt_t, UInt_t> PndTopix4::PixelAddressToMatrixAddress(ToPix4::pixelAddress address)
{
  UInt_t matrix_row = 0;
  UInt_t matrix_col = 0;

  if (address.fCol == 0 || address.fCol == 3) {
    matrix_row = address.fRow;
    matrix_col = address.fCol * 6 + !(address.fSide);
  }

  else {
    matrix_col = (address.fRow / 32) * 2 + 2 + ((address.fCol - 1) * 8);
    if ((address.fRow / 32) % 2 == 0) {
      matrix_row = address.fRow % 32;
      matrix_col += !(address.fSide);
    } else {
      matrix_row = 31 - address.fRow % 32;
      matrix_col += address.fSide;
    }
  }
  // std::cout << address.fRow << "/" << address.fCol << "/" << address.fSide << " --> " << matrix_row << "/" << matrix_col << std::endl;
  return std::pair<UInt_t, UInt_t>(matrix_col, matrix_row);
}

std::pair<UInt_t, UInt_t> PndTopix4::PixelAddressToMatrixAddress(UInt_t row, UInt_t col, Bool_t side)
{
  ToPix4::pixelAddress address(row, col, side);
  return PixelAddressToMatrixAddress(address);
}

ToPix4::pixelAddress PndTopix4::MatrixAddressToPixelAddress(std::pair<UInt_t, UInt_t> matrixAddress)
{
  UInt_t matrix_col = matrixAddress.first;
  UInt_t matrix_row = matrixAddress.second;

  ToPix4::pixelAddress address;
  if (matrix_col < 2 || matrix_col > 17) {
    address.fSide = !(matrix_col % 2);
    address.fCol = matrix_col / 2;
    address.fRow = matrix_row;
  } else {
    address.fCol = (matrix_col - 2) / 8 + 1;
    if (((matrix_col - 2) / 2) % 2 == 0) {
      address.fRow = (matrix_col - 2) / 2 * 32 + matrix_row;
      address.fSide = !((matrix_col - 2) % 2);
    } else {
      address.fRow = ((((matrix_col - 2) % 8) / 2) + 1) * 32 - (matrix_row + 1);
      address.fSide = ((matrix_col - 2) % 2);
    }
  }

  // std::cout << matrix_row << "/" << matrix_col << "-->" << address.fRow << "/" << address.fCol << "/" << address.fSide << std::endl;

  return address;
}

ULong64_t PndTopix4::ConvertToPix4HammingToStandardHamming(ULong64_t topixhamming)
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
