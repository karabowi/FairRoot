/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----                   FairRun source file                         -----
// -----            Created 06/01/04  by M. Al-Turany                  -----
// -------------------------------------------------------------------------

#include "FairRun.h"

#include "FairEventHeader.h"            // for FairEventHeader
#include "FairFileHeader.h"             // for FairFileHeader
#include "FairLogger.h"                 // for FairLogger, MESSAGE_ORIGIN
#include "FairRootManager.h"            // for FairRootManager
#include "FairRuntimeDb.h"              // for FairRuntimeDb
#include "FairTask.h"                   // for FairTask
#include "FairLinkManager.h"            // for FairLinkManager

#include "FairFileSink.h"               // only temporary, should be removed after the move to FairSink is finished

#include "TFile.h"                      // for TFile
#include "TList.h"                      // for TList
#include "TObject.h"                    // for TObject

#include <stddef.h>                     // for NULL

//_____________________________________________________________________________
TMCThreadLocal FairRun* FairRun::fRunInstance= 0;
//_____________________________________________________________________________
FairRun* FairRun::Instance()
{
  return fRunInstance;
}
//_____________________________________________________________________________
FairRun::FairRun(Bool_t isMaster)
  :TNamed(),
   fNTasks(0),
   fRtdb(FairRuntimeDb::instance()),
   fTask(new FairTask("FairTaskList")),
   fRootManager(0),
   fSink(0),
   fRunId(0),
   fAna(kFALSE),
   fEvtHeader(NULL),
   fFileHeader(new FairFileHeader()),
   fGenerateRunInfo(kFALSE),
   fIsMaster(isMaster),
   fMarkFill(kTRUE)
{
  if (fRunInstance) {
    Fatal("FairRun", "Singleton instance already exists.");
    return;
  }
  fRunInstance=this;

    fRootManager = FairRootManager::Instance();

  new FairLinkManager();
}
//_____________________________________________________________________________
FairRun::~FairRun()
{
  LOG(DEBUG) << "Enter Destructor of FairRun" << FairLogger::endl;
  delete fTask;  // There is another tasklist in MCApplication,
  // but this should be independent
  delete fRtdb;  // who is responsible for the RuntimeDataBase
  delete fEvtHeader;
  LOG(DEBUG) << "Leave Destructor of FairRun" << FairLogger::endl;
}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairRun::AddTask(FairTask* t)
{
  fTask->Add(t);
  fNTasks++;
  fFileHeader->AddTaskClassName(t->ClassName());
}
//_____________________________________________________________________________
void FairRun::SetTask(FairTask* t)
{
  delete fTask;
  fTask = t;
  fFileHeader->AddTaskClassName(t->ClassName());
}
//_____________________________________________________________________________
void FairRun::CreateGeometryFile(const char* geofile)
{
  fRootManager->CreateGeometryFile(geofile);
}
//_____________________________________________________________________________
FairTask* FairRun::GetTask(const char* taskName)
{
  TList* taskList = fTask->GetListOfTasks();
  TObject* task = taskList->FindObject(taskName);
  return dynamic_cast<FairTask*>(task);
}
//_____________________________________________________________________________
FairEventHeader*  FairRun::GetEventHeader()
{
  if ( NULL == fEvtHeader ) {
    fEvtHeader = new FairEventHeader();
  }
  return fEvtHeader;
}

void FairRun::SetUseFairLinks(Bool_t val)
{
        fRootManager->SetUseFairLinks(val);
}

//_____________________________________________________________________________
void FairRun::SetWriteRunInfoFile(Bool_t write)
{
  LOG(WARNING) << "Function FairRun::SetWriteRunInfoFile(Bool_t) is depcrecated and will vanish in future versions of FairRoot.\n";
  LOG(WARNING) << "Please use FairRun::SetGenerateRunInfo(Bool_t) instead." << FairLogger::endl;

  fGenerateRunInfo = write;
}

Bool_t FairRun::GetWriteRunInfoFile()
{
  LOG(WARNING) << "Function FairRun::GetWriteRunInfoFile() is depcrecated and will vanish in future versions of FairRoot.\n";
  LOG(WARNING) << "Please use FairRun::IsRunInfoGenerated() instead." << FairLogger::endl;

  return fGenerateRunInfo;
}

//_____________________________________________________________________________
void FairRun::SetOutputFile(const char* fname)
{
  LOG(WARNING) << "FairRun::SetOutputFile() deprecated. Use FairFileSink." << FairLogger::endl;
  fSink = new FairFileSink(fname);
  if (fRootManager) fRootManager->SetSink(fSink);
}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairRun::SetOutputFile(TFile* f)
{
  LOG(WARNING) << "FairRun::SetOutputFile() deprecated. Use FairFileSink." << FairLogger::endl;
  fSink = new FairFileSink(f);
  if (fRootManager) fRootManager->SetSink(fSink);
}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairRun::SetOutputFileName(const TString& name) {
  LOG(WARNING) << "FairRun::SetOutputFileName() deprecated. Use FairFileSink." << FairLogger::endl;
  fSink = new FairFileSink(name);
  if (fRootManager) fRootManager->SetSink(fSink);
}
//_____________________________________________________________________________

ClassImp(FairRun)
