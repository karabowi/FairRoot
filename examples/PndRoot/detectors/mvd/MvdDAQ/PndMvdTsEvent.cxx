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

#include <stdlib.h>
#include "PndMvdTsEvent.h"

#include "PndMvdSiHit.h"
#include "PndMvdTdcData.h"
#include "PndMvdQdcData.h"

ClassImp(PndMvdTsEvent);

PndMvdTsEvent::PndMvdTsEvent()
  : fEventId(0), fSiHitList(new TClonesArray("PndMvdSiHit")), fTdcValues(new TClonesArray("PndMvdTdcData")), fQdcValues(new TClonesArray("PndMvdQdcData")), fNumScalerValues(0),
    fScalerValues(nullptr), fScalerValuesAllocSize(0), fExtClockCount(0), fExtClockResetCount(0)
{
}

PndMvdTsEvent::~PndMvdTsEvent()
{
  delete fSiHitList;
  delete fTdcValues;
  delete fQdcValues;
  //    delete fGiTdcValues;
  //    delete fGiQdcValues;
  if (fScalerValues) {
    free(fScalerValues);
  }
}

void PndMvdTsEvent::Clear(const Option_t *)
{
  fEventId = 0;
  fSiHitList->Clear();
  fTdcValues->Clear();
  fQdcValues->Clear();
  //    fGiTdcValues->Clear();
  //    fGiQdcValues->Clear();
  fNumScalerValues = 0;
}

void PndMvdTsEvent::FillSiHits(int adcChannel, DWORD *hits, WORD count)
{
  TClonesArray &hitList = *fSiHitList;
  for (int i = 0; i < count; i++) {
    // DWORD triggerNo = (hits[i]&0x1f000000) >> 24;    // bit 28 to 24  //[R.K.03/2017] FIXME unused variable
    WORD numFrames = (hits[i] & 0x00f80000) >> 19; // bit 19 to 23
    WORD channel = (hits[i] & 0x0007f000) >> 12;   // bit 12 to 18
    WORD height = (hits[i] & 0x00000fff);          // bit 0 to 11
    WORD box = adcChannel / 6;
    channel += (adcChannel % 6) * 128;

    new (hitList[hitList.GetEntriesFast()]) PndMvdSiHit(box, channel, height, numFrames);
  }
}

void PndMvdTsEvent::FillTdcValues(DWORD *tdcValues, WORD count)
{
  TClonesArray &tdcList = *fTdcValues;
  for (int i = 0; i < count; i++) {
    if ((tdcValues[i] & 0xf8000000) == 0) {
      int trailing = (tdcValues[i] & 0x04000000) >> 26;
      int channel = (tdcValues[i] & 0x03E00000) >> 21;
      int value = (tdcValues[i] & 0x001fffff);
      new (tdcList[tdcList.GetEntriesFast()]) PndMvdTdcData(channel, value, trailing);
    }
  }
}

void PndMvdTsEvent::FillQdcValues(DWORD *qdcValues, WORD count)
{
  TClonesArray &qdcList = *fQdcValues;
  for (int i = 0; i < count; i++) {
    int channel = (qdcValues[i] & 0xff000000) >> 24;
    int value = (qdcValues[i] & 0x00ffffff);
    new (qdcList[qdcList.GetEntriesFast()]) PndMvdQdcData(channel, value);
  }
}

void PndMvdTsEvent::FillGiTdcValues(DWORD *tdcValues, WORD count)
{
  TClonesArray &tdcList = *fTdcValues;
  for (int i = 0; i < count; i++) {
    if ((tdcValues[i] & 0xf8000000) == 0) {
      int trailing = (tdcValues[i] & 0x04000000) >> 26;
      int channel = (tdcValues[i] & 0x03E00000) >> 21;
      int value = (tdcValues[i] & 0x001fffff);
      new (tdcList[tdcList.GetEntriesFast()]) PndMvdTdcData(channel + 32, value, trailing);
    }
  }
}

void PndMvdTsEvent::FillGiQdcValues(DWORD *qdcValues, WORD count)
{
  TClonesArray &qdcList = *fQdcValues;
  for (int i = 0; i < count; i++) {
    int channel = (qdcValues[i] & 0x001F0000) >> 16;
    int value = (qdcValues[i] & 0x00000fff);
    int underflow = (qdcValues[i] & 0x00002000);
    int overflow = (qdcValues[i] & 0x00001000);
    if (underflow || overflow) {
      value = -1;
    }
    new (qdcList[qdcList.GetEntriesFast()]) PndMvdQdcData(channel + 32, value);
  }
}

void PndMvdTsEvent::FillScalerValues(DWORD *scalerValues, WORD count)
{
  if (fScalerValuesAllocSize < count) {
    fScalerValues = (UInt_t *)realloc(fScalerValues, sizeof(UInt_t) * count);
    fScalerValuesAllocSize = count;
  }
  fNumScalerValues = count;
  for (int i = 0; i < count; i++) {
    fScalerValues[i] = scalerValues[i];
  }
}
