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

#ifndef TS_EVENT_CLASS
#define TS_EVENT_CLASS

// #pragma once
#include <TObject.h>
#include <TClonesArray.h>

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned long long QWORD;

class PndMvdTsEvent : public TObject {
 public:
  PndMvdTsEvent();
  virtual ~PndMvdTsEvent();

 public:
  void Clear(const Option_t * = nullptr);
  void SetEventId(UInt_t eventId) { fEventId = eventId; }
  void FillSiHits(int adcChannel, DWORD *hits, WORD count);
  void FillTdcValues(DWORD *tdcValues, WORD count);
  void FillQdcValues(DWORD *qdcValues, WORD count);
  void FillGiTdcValues(DWORD *tdcValues, WORD count);
  void FillGiQdcValues(DWORD *qdcValues, WORD count);
  void FillScalerValues(DWORD *qdcValues, WORD count);
  void SetExtClockCount(QWORD value, DWORD resetCount)
  {
    fExtClockCount = value;
    fExtClockResetCount = resetCount;
  }

  UInt_t GetEventId() { return fEventId; }
  TClonesArray *GetSiHitList() { return fSiHitList; }
  TClonesArray *GetTdcValues() { return fTdcValues; }
  TClonesArray *GetQdcValues() { return fQdcValues; }
  //    TClonesArray* GetGiTdcValues() {return fGiTdcValues;}
  //    TClonesArray* GetGiQdcValues() {return fGiQdcValues;}
  Int_t GetScalerValueCount() { return fNumScalerValues; }
  UInt_t *GetScalerValues() { return fScalerValues; }

  ULong64_t GetExtClockTimeStamp(UInt_t &resetCount)
  {
    resetCount = fExtClockResetCount;
    return fExtClockCount;
  }

 protected:
  // data:
 public:
 protected:
  UInt_t fEventId;          // unique ID for the Event
  TClonesArray *fSiHitList; // list of hit channels on silicon strip detectors
  TClonesArray *fTdcValues; // TDC raw values
  TClonesArray *fQdcValues; // QDC raw values
  //    TClonesArray* fGiTdcValues;     //TDC raw values (for Giessen TDC)
  //    TClonesArray* fGiQdcValues;     //QDC raw values (for Giessen QDC)
  Int_t fNumScalerValues;
  UInt_t *fScalerValues;         //[fNumScalerValues] Scaler values (usually 16 UInt_t values in array)
  size_t fScalerValuesAllocSize; //! allocated memory for fScalerValues
  ULong64_t fExtClockCount;      // counter value of ext. clock at time of trigger
  UInt_t fExtClockResetCount;    // number of times the ext. clock was reset (since loading of FPGA)

 private:
  PndMvdTsEvent(const PndMvdTsEvent &) = delete;
  PndMvdTsEvent &operator=(const PndMvdTsEvent &) = delete;
  ClassDef(PndMvdTsEvent, 2);
};

#endif
