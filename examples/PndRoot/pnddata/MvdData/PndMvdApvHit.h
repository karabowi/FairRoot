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

// --------------------------------------------------------
// ----     PndMvdApvHit header file                       ---
// ----     Created 07.01.09 HGZ by Lars Ackermann      ---
// ----     on the basic of the ana tool of dts         ---
// --------------------------------------------------------

/** PndMvdApvHit.h
 *@author L.Ackermann <lars.ackermann@physik.tu-dresden.de>
 **/

#ifndef PNDMVDAPV_H
#define PNDMVDAPV_H
#include "TObject.h"
#include <iostream>

//! Class to store data of Apv-Sensors

class PndMvdApvHit : public TObject {
 public:
  PndMvdApvHit() : fEventID(-1), fModuleID(-1), fFeID(-1), fTriggerID(-1.), fChannel(-1), fTimestamp(-1.), fFrameID(-1), fADC(-1.), fLength(0) {}
  /**
  main constructor, used to create ApvHit with resonable information
  @param eventID event ID
  @param moduleID ID of used sensor module
  @param fe frontend ID
  @param trigger trigger ID
  @param timestamp trigger time stamp
  @param frame frame ID
  @param channel cannel
  @param adc ADC height in channel
  @param length frame length in this channel
  */
  PndMvdApvHit(long int eventID, Int_t moduleID, Int_t fe, Double_t triggerID, Double_t timestamp, Int_t frame, Int_t channel, Double_t adc, Int_t length)
    : fEventID(eventID), fModuleID(moduleID), fFeID(fe), fTriggerID(triggerID), fChannel(channel), fTimestamp(timestamp), fFrameID(frame), fADC(adc), fLength(length)
  {
  }
  ~PndMvdApvHit() {}

  /**
  @fn Int_t GetChannel() const
      get hit channel index
      @return channel index of hit
      */
  Int_t GetChannel() const { return fChannel; }

  /**
  @fn Int_t GetTimestamp() const
      get hit timestamp
      @return timestamp in the event
      */
  Double_t GetTimestamp() const { return fTimestamp; }

  /**
  @fn Int_t GetFrameID() const
  @return index of frame where the hit occured in the event
  */
  Int_t GetFrameID() const { return fFrameID; }

  /**
  @fn Double_t GetADC() const
  @return amplitude of the hit
  */
  Double_t GetADC() const { return fADC; }

  /**
  @fn Int_t GetLength() const
  @return length of the hit over threshold
  */
  Int_t GetLength() const { return fLength; }

  /**
  @fn Int_t GetFeID() const
  @return fe ID
  */
  Int_t GetFeID() const { return fFeID; }

  /**
  @fn void SetFeID(Int_t new_FE)
set new FeID (needed for faking)
  @return void
  */
  void SetFeID(Int_t new_FE)
  {
    fFeID = new_FE;
    return;
  }

  /**
  @fn long int GetEventID() const
  @return event ID
  */
  long int GetEventID() const { return fEventID; }

  /**
        @fn Int_t GetModuleID() const
        @return module ID
        */
  Int_t GetModuleID() const { return fModuleID; }

  /**
  @fn Int_t GetTriggerID() const
  @return trigger ID
  */
  Double_t GetTriggerID() const { return fTriggerID; }

 private:
  long int fEventID;   /// store event ID
  Int_t fModuleID;     /// store moduleID
  Int_t fFeID;         /// store fe ID (should be betwen 0 and 2)
  Double_t fTriggerID; /// store trigger ID
  Int_t fChannel;      /// store channel (should be betwen 0 and 127)
  Double_t fTimestamp; /// store timestamp
  Int_t fFrameID;      /// store frame ID
  Double_t fADC;       /// store ADC height
  Int_t fLength;       /// store frame length

  ClassDef(PndMvdApvHit, 1);
};
ClassImp(PndMvdApvHit);
#endif
