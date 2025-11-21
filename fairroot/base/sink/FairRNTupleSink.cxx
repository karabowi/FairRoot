/********************************************************************************
 * Copyright (C) 2014-2024 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH  *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                             FairRNTupleSink            m          -----
// -----                    Created 20.03.2025 by R. Karabowicz            -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include "FairRNTupleSink.h"

#include "FairEventHeader.h"
#include "FairMonitor.h"       // to store histograms at the end
#include "FairRootManager.h"   // to GetTreeName()

R__LOAD_LIBRARY(ROOTNTuple)

#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleModel.hxx>
#include <ROOT/RNTupleWriter.hxx>
#include <TBranch.h>
#include <TClass.h>
#include <TCollection.h>
#include <TFolder.h>
#include <TGeoManager.h>   // for TGeoManager, gGeoManager
#include <TIterator.h>
#include <TNamed.h>
#include <TObjArray.h>
#include <TObject.h>
#include <TROOT.h>
#include <TString.h>
#include <TTree.h>
#include <cstdlib>    // free
#include <cxxabi.h>   // __cxa_demangle
#include <fairlogger/Logger.h>
#include <string>

using REntry = ROOT::REntry;
using RNTupleWriter = ROOT::RNTupleWriter;
using RNTupleModel = ROOT::RNTupleModel;

FairRNTupleSink::FairRNTupleSink(const TString* RootFileName, const char* Title)
    : FairSink()
    , fOutputTitle(Title)
    , fFileName(RootFileName->Data())
{
    fRootFile.reset(TFile::Open(fFileName.c_str(), "recreate"));
    fModel = RNTupleModel::Create();
}

FairRNTupleSink::FairRNTupleSink(const TString RootFileName, const char* Title)
    : FairSink()
    , fOutputTitle(Title)
    , fFileName(RootFileName.Data())
{
    fRootFile.reset(TFile::Open(fFileName.c_str(), "recreate"));
    fModel = RNTupleModel::Create();
}

Bool_t FairRNTupleSink::InitSink()
{
    fIsInitialized = kTRUE;
    return kTRUE;
}

void FairRNTupleSink::Close() {}

void FairRNTupleSink::Reset() {}

void FairRNTupleSink::FillEventHeader(FairEventHeader* /* feh */) {}

void FairRNTupleSink::WriteFolder()
{
    CreatePersistentBranchesAny();
}

bool FairRNTupleSink::CreatePersistentBranchesAny()
{
    //    fWriter = RNTupleWriter::Recreate(std::move(fModel), "fairdata", fFileName);
    fWriter = RNTupleWriter::Append(std::move(fModel), "fairdata", *fRootFile);
    fEntry = fWriter->GetModel().CreateBareEntry();
    for (auto tokenAddress : fTokenAddress) {
        fEntry->BindRawPtr(tokenAddress.first, tokenAddress.second);
    }
    fPersistentBranchesDone = true;
    return true;
}

void FairRNTupleSink::WriteObject(TObject* f, const char* name, Int_t option)
{
    fRootFile->cd();
    f->Write(name, option);
}

void FairRNTupleSink::WriteGeometry()
{
    //    fRootFile->WriteTObject(gGeoManager);
}

void FairRNTupleSink::Fill()
{
    RNTupleFillStatus status;
    fWriter->FillNoFlush(*fEntry, status);
    if (status.ShouldFlushCluster()) {
        // If we are asked to flush, first try to do as much work as possible outside of the critical section:
        // FlushColumns() will flush column data and trigger compression, but not actually write to storage.
        // (A framework may of course also decide to flush more often.)
        fWriter->FlushColumns();

        {
            // FlushCluster() will flush data to the underlying TFile, so it requires synchronization.
            fWriter->FlushCluster();
        }
    }
}

Int_t FairRNTupleSink::Write(const char*, Int_t, Int_t)
{
    return 0;
}
//_____________________________________________________________________________

//_____________________________________________________________________________
FairSink* FairRNTupleSink::CloneSink()
{
    return nullptr;
}
//_____________________________________________________________________________
