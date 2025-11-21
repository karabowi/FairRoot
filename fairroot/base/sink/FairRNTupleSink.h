/********************************************************************************
 * Copyright (C) 2014-2024 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH  *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                             FairRNTupleSink                       -----
// -----                    Created 20.03.2025 by R. Karabowicz            -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef __FAIRROOT__FairRNTupleSink__
#define __FAIRROOT__FairRNTupleSink__

#include "FairSink.h"

R__LOAD_LIBRARY(ROOTNTuple)

#include <ROOT/REntry.hxx>
#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleFillStatus.hxx>
#include <ROOT/RNTupleModel.hxx>
#include <ROOT/RNTupleWriter.hxx>
#include <Rtypes.h>
#include <TBranch.h>
#include <TFile.h>
#include <TString.h>
#include <fairlogger/Logger.h>
#include <memory>
#include <typeinfo>

using REntry = ROOT::REntry;
using ROOT::RNTupleFillStatus;
using RNTupleModel = ROOT::RNTupleModel;
using RNTupleWriter = ROOT::RNTupleWriter;

class FairEventHeader;
class FairFileHeader;

class FairRNTupleSink : public FairSink
{
  public:
    FairRNTupleSink(const TString* RNTupleName, const char* Title = "OutputRNTuple");
    FairRNTupleSink(const TString RNTupleName, const char* Title = "OutputRNTuple");
    FairRNTupleSink(const FairRNTupleSink&) = delete;
    FairRNTupleSink operator=(const FairRNTupleSink&) = delete;
    ~FairRNTupleSink() override = default;

    Bool_t InitSink() override;
    void Close() override;
    void Reset() override;

    Sink_Type GetSinkType() override { return kRNTUPLESINK; }

    virtual void FillEventHeader(FairEventHeader* feh);

    void SetOutTree(TTree* fTree) override {}

    void Fill() override;

    Int_t Write(const char* name = nullptr, Int_t option = 0, Int_t bufsize = 0) override;

    void RegisterImpl(const char*, const char*, void*) override{};
    void RegisterAny(const char* brname, const std::type_info& oi, const std::type_info& pi, void* obj) override{};

    template<typename T>
    void RegisterRNTuple(const char* name, T*& obj);

    void WriteFolder() override;
    bool CreatePersistentBranchesAny() override;

    void WriteObject(TObject* f, const char*, Int_t option = 0) override;
    void WriteGeometry() override;

    FairSink* CloneSink() override;

  private:
    /** Title of input sink, could be input, background or signal*/
    std::string fOutputTitle;
    /** file name */
    std::string fFileName;
    /** ROOT file */
    std::unique_ptr<TFile> fRootFile;
    /** Output Tree  */
    /** Initialization flag, true if initialized */
    Bool_t fIsInitialized{kFALSE};

    std::unique_ptr<RNTupleModel> fModel;
    std::unique_ptr<RNTupleWriter> fWriter;
    std::unique_ptr<REntry> fEntry;

    /**File Header*/
    FairFileHeader* fFileHeader{nullptr};   //!

    bool fPersistentBranchesDone{false};   //!

    std::vector<std::pair<ROOT::RFieldToken, void*>> fTokenAddress;

    ClassDefOverride(FairRNTupleSink, 1);
};

template<typename T>
void FairRNTupleSink::RegisterRNTuple(const char* brname, T*& obj)
{
    LOG(info) << "RegisterRNTuple(\"" << brname << "\")";
    fModel->MakeField<T>(brname);
    fTokenAddress.push_back(std::make_pair(fModel->GetToken(brname), obj));
}

#endif /* defined(__FAIRROOT__FairRNTupleSink__) */
