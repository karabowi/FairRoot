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

/*============================================================*/
/* mrftools.cpp                                               */
/* Toolbox                                                    */
/*                                               M.C. Mertens */
/*============================================================*/

#include "mrftools.h"
#include <climits>

namespace mrftools {

static const UInt_t UInt_t_bitlength = sizeof(UInt_t) * CHAR_BIT;

bool getIntBit(const UInt_t &position, const UInt_t &value)
{
  if (position < UInt_t_bitlength) {
    return (value & (1 << position));
  } else {
    return false;
  }
}

void setIntBit(const UInt_t &position, UInt_t &value, const bool &state)
{
  if (position < UInt_t_bitlength) {
    if (state) {
      value |= (1 << position);
    } else {
      value &= (~(1 << position));
    }
  }
}

UInt_t shiftBy(const int &positions, const UInt_t &value)
{
  if (positions < 0) {
    return (value >> (-positions));
  } else if (positions > 0) {
    return (value << positions);
  } else {
    return value;
  }
}

unsigned int getIteratorItemCount(const std::map<std::string, TConfItem>::const_iterator &start, const std::map<std::string, TConfItem>::const_iterator &stop)
{
  std::map<std::string, TConfItem>::const_iterator iter;
  unsigned int count = 0;
  for (iter = start; iter != stop; ++iter) {
    ++count;
  }
  return count;
}

unsigned int getIteratorItemCount(const std::map<std::string, std::map<std::string, TConfItem>>::const_iterator &start,
                                  const std::map<std::string, std::map<std::string, TConfItem>>::const_iterator &stop)
{
  unsigned int count = 0;
  std::map<std::string, std::map<std::string, TConfItem>>::const_iterator iter;
  std::map<std::string, TConfItem>::const_iterator iter2;
  for (iter = start; iter != stop; ++iter) {
    for (iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2) {
      ++count;
    }
  }
  return count;
}

UInt_t grayToBin(UInt_t gray)
{
  // UInt_t result = 0;
  // setIntBit(31, result, getIntBit(31, gray));
  // for (unsigned int i = 30; i > 0; --i) {
  //        setIntBit(i, result, (getIntBit(i+1, result) && !(getIntBit(i, gray))) || (!(getIntBit(i+1, result)) && getIntBit(i, gray)));
  //}
  // setIntBit(0, result, (getIntBit(1, result) && !(getIntBit(0, gray))) || (!(getIntBit(1, result)) && getIntBit(0, gray)));

  // return result;

  for (UInt_t bit = 1U << 31; bit > 1; bit >>= 1) {
    if (gray & bit)
      gray ^= bit >> 1;
  }
  return gray;
}

} // namespace mrftools
