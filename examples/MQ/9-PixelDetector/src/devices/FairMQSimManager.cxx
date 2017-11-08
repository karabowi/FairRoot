/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * FairMQSimManager.cxx
 *
 * @since 2017.10.24
 * @author R. Karabowicz
 */

#include "FairMQSimManager.h"
  
#include "FairMQLogger.h"
#include "FairMQMessage.h"
#include "FairMQProgOptions.h"

#include "FairRunSim.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"

#include "FairModule.h"
#include "FairCave.h"
#include "Pixel.h"
#include "FairPrimaryGenerator.h"
#include "FairBoxGenerator.h"
#include "FairParRootFileIo.h"
#include "FairParSet.h"

#include "FairMCEventHeader.h"

#include "TROOT.h"
#include "TRint.h"
#include "TVirtualMC.h"
#include "TVirtualMCApplication.h"
#include "TList.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TMessage.h"

using namespace std;

FairMQSimManager::FairMQSimManager()
  : FairMQDevice()
  , FairGenericRootManager()
  , fUpdateChannelName("updateChannel")
  , fRunSim(NULL)
  , fNofEvents(1)
  , fTransportName("TGeant3")
  , fMaterialsFile("")
  , fMagneticField(NULL)
  , fDetectorArray(NULL)
  , fPrimaryGenerator(NULL)
  , fStoreTrajFlag(false)
  , fTaskArray(NULL)
  , fFirstParameter(NULL)
  , fSecondParameter(NULL)
{
}

void FairMQSimManager::InitTask() 
{
  fRunSim = new FairRunSim();
  
  fRunSim->SetRootManager(this);

  if ( fFirstParameter || fSecondParameter ) {
    FairRuntimeDb *rtdb=fRunSim->GetRuntimeDb();
    if ( fFirstParameter )
      rtdb->setFirstInput(fFirstParameter);
    if ( fSecondParameter )
      rtdb->setSecondInput(fSecondParameter);
  }

  fRunSim->SetName(fTransportName.data());
  FairRootManager::Instance()->CreateRootFolder();
  // ------------------------------------------------------------------------
  
  // -----   Create media   -------------------------------------------------
  fRunSim->SetMaterials(fMaterialsFile.data());
  // ------------------------------------------------------------------------
  
  // -----   Magnetic field   -------------------------------------------
  if ( fMagneticField )
    fRunSim->SetField(fMagneticField);
  // --------------------------------------------------------------------

  // -----   Create geometry   ----------------------------------------------
  for ( int idet = 0 ; idet < fDetectorArray->GetEntries() ; idet++ ) {
    fRunSim->AddModule((FairModule*)(fDetectorArray->At(idet)));
  }
  // ------------------------------------------------------------------------

  if ( fPrimaryGenerator )
    fRunSim->SetGenerator(fPrimaryGenerator);
  // ------------------------------------------------------------------------

  fRunSim->SetStoreTraj(fStoreTrajFlag);

  // -----   Set tasks   ----------------------------------------------------
  if ( fTaskArray ) {
    for ( int itask = 0 ; itask < fTaskArray->GetEntries() ; itask++ ) {
      fRunSim->AddTask((FairTask*)(fTaskArray->At(itask)));
    }
  }
  // ------------------------------------------------------------------------
   
  // -----   Initialize simulation run   ------------------------------------
  fRunSim->Init();
  // ------------------------------------------------------------------------

}

void FairMQSimManager::PreRun()
{
}

bool FairMQSimManager::ConditionalRun()
{
  UpdateParameterServer();
  fRunSim->Run(fNofEvents);
  return false;
}

void FairMQSimManager::UpdateParameterServer()
{
  FairRuntimeDb* rtdb = fRunSim->GetRuntimeDb();
  
  printf("FairMQSimManager::UpdateParameterServer() (%d containers)\n",rtdb->getListOfContainers()->GetEntries());

  // send the parameters to be saved
  TIter next(rtdb->getListOfContainers());
  FairParSet* cont;
  while ((cont=static_cast<FairParSet*>(next())))
    {
      std::string ridString = std::string("RUNID") + std::to_string(fRunSim->GetRunId()) + std::string("RUNID") + std::string(cont->getDescription());
      cont->setDescription(ridString.data());
      SendObject(cont,fUpdateChannelName);
    }

  printf("FairMQSimManager::UpdateParameterServer() finished\n");
}

void FairMQSimManager::SendObject(TObject* obj, std::string chan) {
  TMessage* tmsg = new TMessage(kMESS_OBJECT);
  tmsg->WriteObject(obj);

  FairMQMessagePtr mess(NewMessage(tmsg->Buffer(),
                                   tmsg->BufferSize(),
                                   [](void* /*data*/, void* object){ delete static_cast<TMessage*>(object); },
                                   tmsg));
  std::unique_ptr<FairMQMessage> rep(NewMessage());
  
  printf ("sending %s",obj->GetName());
  if (Send(mess, chan) > 0)
    {
      if (Receive(rep, chan) > 0)
        {
          std::string repString = string(static_cast<char*>(rep->GetData()), rep->GetSize());
          printf (" -> %s\n",repString.data());
        }
    }
}

void FairMQSimManager::PostRun()
{
}

FairMQSimManager::~FairMQSimManager()
{
}

//_____________________________________________________________________________
void FairMQSimManager::Register(const char* name, const char* folderName, TNamed* obj, Bool_t toFile)
{
  //  fRootManager->Register(name, folderName, obj, toFile);
}

//_____________________________________________________________________________
void FairMQSimManager::Register(const char* name, const char* folderName, TCollection* obj, Bool_t toFile)
{
  //  fRootManager->Register(name, folderName, obj, toFile);
}

//_____________________________________________________________________________
void FairMQSimManager::RegisterInputObject(const char* name, TObject* obj)
{
  //  fRootManager->RegisterInputObject(name,obj);
}

//_____________________________________________________________________________
TClonesArray* FairMQSimManager::Register(TString branchName, TString className, TString folderName, Bool_t toFile)
{
  return NULL;
}

//_____________________________________________________________________________
FairWriteoutBuffer* FairMQSimManager::RegisterWriteoutBuffer(TString branchName, FairWriteoutBuffer* buffer)
{
  return NULL;
}

void  FairMQSimManager::Fill()
{
  /// Fill the Root tree.
  LOG(DEBUG) << "called FairMQSimManager::Fill()!!!!";
  TList* branchNameList = FairRootManager::Instance()->GetBranchNameList();
  TObjString* ObjStr;
  
  for (auto& mi : fChannels)
    {
      LOG(DEBUG) << "trying channel >" << mi.first.data() << "<";

      FairMQParts parts;
      
      for(Int_t t=0; t<branchNameList->GetEntries(); t++) 
        {
          ObjStr= static_cast<TObjString*>(branchNameList->TList::At(t));
          LOG(DEBUG) << "              branch >" << ObjStr->GetString().Data() << "<";
          std::string modifiedBranchName = std::string("#") + ObjStr->GetString().Data() + "#";
          if ( mi.first.find(modifiedBranchName) != std::string::npos || mi.first.find("#all#") != std::string::npos ) 
            {
              TObject* object   = (FairRootManager::Instance()->GetObject(ObjStr->GetString()))->Clone();
              TMessage* message = new TMessage(kMESS_OBJECT);
              message->WriteObject(object);
              parts.AddPart(NewMessage(message->Buffer(),
                                       message->BufferSize(),			     
                                       [](void* /*data*/, void* hint) { delete (TMessage*)hint;},
                                       message));
              LOG(DEBUG) << "channel >" << mi.first.data() << "< --> >" << ObjStr->GetString().Data() << "<";
            }
        }
      if ( parts.Size() > 0 )
        Send(parts,mi.first.data());
    }
}
 
void  FairMQSimManager::Write()
{
}

void  FairMQSimManager::CloseOutFile()
{
}

Int_t               FairMQSimManager::GetBranchId(TString const &BrName)
{
  return 0;
}


FairRootManager*    FairMQSimManager::GetFairRootManager() const
{
  return NULL;
}
Int_t  FairMQSimManager::GetId() const
{
  return 0;
}
