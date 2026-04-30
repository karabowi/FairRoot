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
/* mrf_confitem.h                                             */
/* MVD Readout Framework Configuration Item                   */
/*                                               M.C. Mertens */
/*============================================================*/

#ifndef __MRF_CONFITEM_H__
#define __MRF_CONFITEM_H__

#include "mrfbase.h"

//! Configuration Item to be used in TMrfData structures.
/*!
Holds information about values, position and length of data items.
*/
class TConfItem {
 public:
  TConfItem();

  //! Standard constructor with initialization list.
  /*!
  \param value Value to be set.
  \param position Position of the value in the data stream.
  \param length Length of the storage space reserved in the data stream.
  \param min Optional lower bounds for value.
  \param max Optional upper bounds for value.
  \param flags Optional flag field.
  */
  // TConfItem(const mrf::registertype value, const UInt_t position, const UInt_t length);

  TConfItem(const mrf::registertype value, const UInt_t position, const UInt_t length);

  TConfItem(const mrf::registertype value, const UInt_t position, const UInt_t length, const UInt_t min, const UInt_t max, const UInt_t flags = 0);

  //! Holds the data value.
  mrf::registertype value;

  //! Holds the position of the data value in the data stream.
  UInt_t position;

  //! Holds the length of reserved space for the data value in the data stream.
  UInt_t length;

  //! Minimum valid value. Corresponds to \b disabled for state data.
  UInt_t min;

  //! Maximum valid value. Corresponds to \b enabled for state data.
  UInt_t max;

  //! Flag field to store additional properties of data.
  UInt_t flags;

 protected:
 private:
};

#endif // __MRF_CONFITEM_H__
