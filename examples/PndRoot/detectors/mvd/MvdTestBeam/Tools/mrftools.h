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
/* mrftools.h                                                 */
/* Toolbox                                                    */
/*                                               M.C. Mertens */
/*============================================================*/

#ifndef __MRFTOOLS_H__
#define __MRFTOOLS_H__

//#include <sys/types.h>
#include <map>
#include <string>
#include "mrf_confitem.h"

//! Helper functions for single bit manipulations.
namespace mrftools {

//! Retrieves a single bit from an integer value.
/*!
\param position Position of bit to investigate.
\param value Value whose bits are to be investigated.
\return \b True if the bit is set, \b False if the bit is not set.
*/
bool getIntBit(const UInt_t &position, const UInt_t &value);

//! Sets a single bit in an integer value.
/*!
\param position The positional index of the bit to be manipulated, 0 referring to the least significant bit.
\param value The integer value whose bits are to be manipulated.
\param state \b True sets the bit, \b False resets the bit.
*/
void setIntBit(const UInt_t &position, UInt_t &value, const bool &state);

//! Shifts the bits in an integer value.
/*!
\param positions Number of positions to be shifted. A positive value means shift to the MSB (left), negative means shift to the LSB (right).
\param value The integer value whose bits are to be shifted.
\return The original value shifted by the given number of positions.
*/
UInt_t shiftBy(const int &positions, const UInt_t &value);

unsigned int getIteratorItemCount(const std::map<std::string, TConfItem>::const_iterator &start, const std::map<std::string, TConfItem>::const_iterator &stop);
unsigned int getIteratorItemCount(const std::map<std::string, std::map<std::string, TConfItem>>::const_iterator &start,
                                  const std::map<std::string, std::map<std::string, TConfItem>>::const_iterator &stop);

//! Converts gray encoded values to bianry values
/*!
\param gray Gray encoded value.
\return The correponding binary value.
*/

UInt_t grayToBin(UInt_t gray);

} // namespace mrftools

#endif // __MRFTOOLS_H__
