/********************************************************************************
 * Copyright (C) 2025 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH       *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
//
//  FairRNTupleSource.h
//  FAIRROOT
//
//  Created by Radoslaw Karabowicz on 2025.04.09
//
//

#ifndef __FAIRROOT__FairRNTupleSource__
#define __FAIRROOT__FairRNTupleSource__

#include "FairFileSourceBase.h"

R__LOAD_LIBRARY(ROOTNTuple)

#include <ROOT/RNTupleModel.hxx>
#include <ROOT/RNTupleReader.hxx>

#include <TArrayI.h>
#include <TChain.h>
#include <TF1.h>
#include <TFile.h>
#include <TFolder.h>
#include <TString.h>
#include <TTree.h>

#include <fairlogger/Logger.h>

#include <list>
#include <map>
#include <memory>

using RNTupleReader = ROOT::Experimental::RNTupleReader;

class FairEventHeader;
class FairFileHeader;
class FairMCEventHeader;

class FairRNTupleSource : public FairFileSourceBase
{
  public:
    //    FairRNTupleSource(TFile* f, const char* Title = "InputRootFile", UInt_t identifier = 0);
    FairRNTupleSource(const TString* RootFileName, const char* Title = "InputRootFile", UInt_t identifier = 0);
    FairRNTupleSource(const TString RootFileName, const char* Title = "InputRootFile", UInt_t identifier = 0);
    // FairRNTupleSource(const FairRNTupleSource& file);
    ~FairRNTupleSource() override;

    Source_Type GetSourceType() override { return kRNTUPLE; }

    Bool_t Init() override;
    
    Int_t ReadEvent(UInt_t i = 0) override;
    void Close() override;

    void FillEventHeader(FairEventHeader* feh) override;

    /** Add a friend file (input) by name)*/
    void AddFriend(TString FileName);
    /**Add ROOT file to input, the file will be chained to already added files*/
    void AddFile(TString FileName);
    void AddFriendsToChain();
    void PrintFriendList();
    void CheckFriendChains();
    void CreateNewFriendChain(TString inputFile, TString inputLevel);
    UInt_t GetEntries() { return fNoOfEntries; }

    //    TList*              GetBranchNameList() {return fBranchNameList;}

    void SetInputFile(TString name);

    /** Set the repetition time of the beam when it can interact (beamTime) and when no interaction happen (gapTime).
     * The total repetition time is beamTime + gapTime */
    void SetBeamTime(Double_t beamTime, Double_t gapTime);
    /** Set the min and max limit for event time in ns */
    void SetEventTimeInterval(Double_t min, Double_t max);
    /** Set the mean time for the event in ns */
    void SetEventMeanTime(Double_t mean);
    void SetEventTime();
    Double_t GetDeltaEventTime();
    void SetFileHeader(FairFileHeader* f) { fFileHeader = f; }
    Double_t GetEventTime();

    /**Check the maximum event number we can run to*/
    Int_t CheckMaxEventNo(Int_t EvtEnd = 0) override;

    Bool_t ActivateObjectAny(void**, const std::type_info&, const char*) override;

    /**Set the status of the EvtHeader
     *@param Status:  True: The header was creatged in this session and has to be filled
              FALSE: We use an existing header from previous data level
     */
    void SetEvtHeaderNew(Bool_t Status) { fEvtHeaderIsNew = Status; }
    Bool_t IsEvtHeaderNew() { return fEvtHeaderIsNew; }

    /** Allow to disable the testing the file layout when adding files to a chain.
     */
    void SetCheckFileLayout(Bool_t enable) { fCheckFileLayout = enable; }

    /**Read one event from source to find out which RunId to use*/
    Bool_t SpecifyRunId() override;

    template<typename T>
    T InitObjectAs(const char* brname);

  private:
    /** Title of input source, could be input, background or signal*/
    TString fInputTitle;
    /** ROOT file name */
    std::string fFileName {""};
    /** List of all files added with AddFriend */
    std::list<TString> fFriendFileList;                                 //!
    std::list<TString> fInputChainList;                                 //!
    std::list<TString> fInputLevel;                                     //!
    std::map<TString, std::multimap<TString, TArrayI>> fRunIdInfoAll;   //!
    /***/
    UInt_t fSourceIdentifier;
    /**No of Entries in this source*/
    UInt_t fNoOfEntries;
    /**Initialization flag, true if initialized*/
    Bool_t IsInitialized;

    FairRNTupleSource(const FairRNTupleSource&);
    FairRNTupleSource operator=(const FairRNTupleSource&);

    std::unique_ptr<RNTupleReader> fReader;
    
    /** MC Event header */
    FairMCEventHeader* fMCHeader;   //!

    /**Event Header*/
    FairEventHeader* fEvtHeader;   //!

    /**File Header*/
    FairFileHeader* fFileHeader;   //!

    /** This is true if the event time used, came from simulation*/
    Bool_t fEventTimeInMCHeader;   //!
    /**This flag is true if the event header was created in this session
     * otherwise it is false which means the header was created in a previous data
     * level and used here (e.g. in the digi)
     */
    Bool_t fEvtHeaderIsNew;   //!

    /** for internal use, to return the same event time for the same entry*/
    UInt_t fCurrentEntryNo;   //!
    /** for internal use, to return the same event time for the same entry*/
    UInt_t fTimeforEntryNo;   //!

    /** min time for one event (ns) */
    Double_t fEventTimeMin;   //!
    /** max time for one Event (ns) */
    Double_t fEventTimeMax;   //!
    /** Time of event since th start (ns) */
    Double_t fEventTime;   //!
    /** Time of particles in beam (ns) */
    Double_t fBeamTime;   //!
    /** Time without particles in beam (gap) (ns) */
    Double_t fGapTime;   //!
    /** EventMean time used (P(t)=1/fEventMeanTime*Exp(-t/fEventMeanTime) */
    Double_t fEventMeanTime;   //!
    /** used to generate random numbers for event time; */
    std::unique_ptr<TF1> fTimeProb;   //!
    /** True if the file layout should be checked when adding files to a chain.
     *  Default value is true.
     */
    Bool_t fCheckFileLayout;   //!

    ClassDefOverride(FairRNTupleSource, 3);
};

template<typename TPtr>
TPtr FairRNTupleSource::InitObjectAs(const char* brname)
{
    static_assert(std::is_pointer<TPtr>::value, "Return type of GetObjectAs has to be pointer to const class");
    using T = typename std::remove_pointer<TPtr>::type;

    LOG(debug) << "FairRNTupleSource::InitObjectAs(\"" << brname << "\")";
    return fReader->GetModel().GetDefaultEntry().GetPtr<T>(brname).get();
}


#endif /* defined(__FAIRROOT__FairRNTupleSource__) */
