/********************************************************************************
 * Copyright (C) 2025 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH       *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

//
//  FairRNTupleSource.cxx
//  FAIRROOT
//
//  Created by Radoslaw Karabowicz on 2025.04.09
//
//
#include "FairRNTupleSource.h"

#include "FairEventHeader.h"
#include "FairMCEventHeader.h"
#include "FairRootManager.h"

#include <TClass.h>
#include <TCollection.h>   // for TIter
#include <TDirectory.h>    // for TDirectory::TContext
#include <TFolder.h>
#include <TList.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TObject.h>
#include <TROOT.h>
#include <TRandom.h>   // for TRandom, gRandom
#include <TString.h>
#include <algorithm>   // for find
#include <cmath>       // fmod
#include <cstdlib>     // exit
#include <fairlogger/Logger.h>
#include <list>   // for _List_iterator, list, etc
#include <map>    // multimap
#include <typeinfo>
#include <vector>

using fairroot::detail::maybe_owning_ptr;
using fairroot::detail::non_owning;

FairRNTupleSource::FairRNTupleSource(const TString* RootFileName, const char* Title, UInt_t)
    : FairFileSourceBase(std::unique_ptr<TFile>{TFile::Open(RootFileName->Data())})
    , fInputTitle(Title)
    , fFileName(RootFileName->Data())
    , fInputLevel()
    , fRunIdInfoAll()
    , fSourceIdentifier(0)
    , fNoOfEntries(-1)
    , IsInitialized(kFALSE)
    , fMCHeader(0)
    , fEvtHeader(0)
    , fFileHeader(0)
    , fEventTimeInMCHeader(kTRUE)
    , fEvtHeaderIsNew(kFALSE)
    , fCurrentEntryNo(0)
    , fTimeforEntryNo(-1)
    , fEventTimeMin(0.)
    , fEventTimeMax(0.)
    , fEventTime(0.)
    , fBeamTime(-1.)
    , fGapTime(-1.)
    , fEventMeanTime(0.)
    , fCheckFileLayout(kTRUE)
{
    LOG(debug) << "FairRNTupleSource created------------";
}

FairRNTupleSource::FairRNTupleSource(const TString RootFileName, const char* Title, UInt_t)
    : FairFileSourceBase(std::unique_ptr<TFile>{TFile::Open(RootFileName.Data())})
    , fInputTitle(Title)
    , fFileName(RootFileName.Data())
    , fInputLevel()
    , fRunIdInfoAll()
    , fSourceIdentifier(0)
    , fNoOfEntries(-1)
    , IsInitialized(kFALSE)
    , fMCHeader(0)
    , fEvtHeader(0)
    , fFileHeader(0)
    , fEventTimeInMCHeader(kTRUE)
    , fEvtHeaderIsNew(kFALSE)
    , fCurrentEntryNo(0)
    , fTimeforEntryNo(-1)
    , fEventTimeMin(0.)
    , fEventTimeMax(0.)
    , fEventTime(0.)
    , fBeamTime(-1.)
    , fGapTime(-1.)
    , fEventMeanTime(0.)
    , fCheckFileLayout(kTRUE)
{
    LOG(debug) << "FairRNTupleSource created------------";
}

FairRNTupleSource::~FairRNTupleSource() = default;

Bool_t FairRNTupleSource::Init()
{
    if (IsInitialized) {
        LOG(info) << "FairRNTupleSource already initialized";
        return kTRUE;
    }
    fReader = RNTupleReader::Open("fairdata", fFileName);
    // Loop through the top-level fields of the input RNTuple
    for (const auto &value : fReader->GetModel().GetDefaultEntry()) {
        LOG(info) << "FIELDNAME = \"" << value.GetField().GetFieldName() << "\"";
        if ( value.GetField().GetFieldName() == "MCEventHeader" )
            fMCHeader = value.GetPtr<FairMCEventHeader>().get();
        else if ( value.GetField().GetFieldName() == "EventHeader" )
            fEvtHeader = value.GetPtr<FairEventHeader>().get();
    }
    
    return kTRUE;
}

Int_t FairRNTupleSource::ReadEvent(UInt_t i)
{
    fCurrentEntryNo = i;
    fEventTime = GetEventTime();
    fReader->LoadEntry(fCurrentEntryNo);

    return 0;
}

Int_t FairRNTupleSource::CheckMaxEventNo(Int_t EvtEnd)
{
    Int_t MaxEventNo = 0;
    if (EvtEnd != 0) {
        MaxEventNo = EvtEnd;
    } else {
        if (fReader)
            MaxEventNo = fReader->GetNEntries();
    }

    return MaxEventNo;
}

Bool_t FairRNTupleSource::SpecifyRunId()
{
    /**Read the first entry in Chain to fill the event header and get the RunID*/
    fReader->LoadEntry(0);
    return true;
}

void FairRNTupleSource::Close() { }

Bool_t FairRNTupleSource::ActivateObjectAny(void** obj, const std::type_info& info, const char* BrName)
{
    return kFALSE;
}

void FairRNTupleSource::SetEventMeanTime(Double_t mean)
{
    fEventMeanTime = mean;
    fTimeProb = std::make_unique<TF1>("TimeProb", "(1/[0])*exp(-x/[0])", 0., mean * 10);
    fTimeProb->SetParameter(0, mean);
    fTimeProb->GetRandom();
    fEventTimeInMCHeader = kFALSE;
}

void FairRNTupleSource::SetEventTimeInterval(Double_t min, Double_t max)
{
    // disable fTimeProb for the uniform distribution
    fTimeProb.reset();
    fEventTimeMin = min;
    fEventTimeMax = max;
    fEventMeanTime = (fEventTimeMin + fEventTimeMax) / 2;
    fEventTimeInMCHeader = kFALSE;
}

void FairRNTupleSource::SetBeamTime(Double_t beamTime, Double_t gapTime)
{
    fBeamTime = beamTime;
    fGapTime = gapTime;
}

void FairRNTupleSource::SetEventTime()
{
    // Check if the time for the current entry is already set
    if (fTimeforEntryNo == fCurrentEntryNo)
        return;
    LOG(debug) << "Set event time for Entry = " << fTimeforEntryNo << " , where the current entry is "
               << fCurrentEntryNo << " and eventTime is " << fEventTime;
    if (fBeamTime < 0) {
        fEventTime += GetDeltaEventTime();
    } else {
        do {
            fEventTime += GetDeltaEventTime();
        } while (fmod(fEventTime, fBeamTime + fGapTime) > fBeamTime);
    }
    LOG(debug) << "New time = " << fEventTime;
    fTimeforEntryNo = fCurrentEntryNo;
}

Double_t FairRNTupleSource::GetDeltaEventTime()
{
    Double_t deltaTime = 0;
    if (fTimeProb) {
        deltaTime = fTimeProb->GetRandom();
        LOG(debug) << "Time set via sampling method : " << deltaTime;
    } else {
        deltaTime = gRandom->Uniform(fEventTimeMin, fEventTimeMax);
        LOG(debug) << "Time set via Uniform Random : " << deltaTime;
    }
    return deltaTime;
}

Double_t FairRNTupleSource::GetEventTime()
{
    LOG(debug) << "-- Get Event Time --";
    if (!fEvtHeaderIsNew && fEvtHeader != 0) {
        Double_t EvtTime = fEvtHeader->GetEventTime();
        if (!(EvtTime < 0)) {
            return EvtTime;
        }
    }

    if (fEventTimeInMCHeader && !fMCHeader) {
        LOG(debug) << "No MCEventHeader, time is set to 0";
        return 0;
    } else if (fEventTimeInMCHeader && fMCHeader) {
        fEventTime = fMCHeader->GetT();
        LOG(debug) << "Get event time from MCEventHeader : " << fEventTime << " ns";
        return fEventTime;
    } else {

        if (fTimeforEntryNo != fCurrentEntryNo) {
            SetEventTime();
        }
        LOG(debug) << "Calculate event time from user input : " << fEventTime << " ns";
        return fEventTime;
    }
}

void FairRNTupleSource::FillEventHeader(FairEventHeader* feh)
{
    feh->SetEventTime(GetEventTime());
    if (fEvtHeader) {
        feh->SetRunId(fEvtHeader->GetRunId());
        feh->SetMCEntryNumber(fEvtHeader->GetMCEntryNumber());
    }
    if (fMCHeader) {
        feh->SetRunId(fMCHeader->GetRunID());
        feh->SetMCEntryNumber(fMCHeader->GetEventID());
    }
}
