/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * FairMQEx9aFindHitsProcessor.cxx
 *
 * @since 2017-03-22
 * @author R. Karabowicz
 */

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "TGeoBBox.h"
#include "TGeoManager.h"
#include "TGeoNode.h"

#include "FairMQEx9aFindHitsProcessor.h"
#include "FairMQLogger.h"
#include "FairMQProgOptions.h"

#include "FairMQMessage.h"

#include "PixelPayload.h"
#include "PixelDigiPar.h"

using namespace std;

// special class to expose protected TMessage constructor
class Ex9aTMessageFHP : public TMessage
{
  public:
  Ex9aTMessageFHP(void* buf, Int_t len)
    : TMessage(buf, len)
  {
    ResetBit(kIsOwner);
  }
};

FairMQEx9aFindHitsProcessor::FairMQEx9aFindHitsProcessor()
  : FairMQDevice()
  , fInputChannelName("data-in")
  , fOutputChannelName("data-out")
  , fParamChannelName("param")
  , fEventHeader(NULL)
  , fNewRunId(1)
  , fCurrentRunId(-1)
  , fDataToKeep("")
  , fReceivedMsgs(0)
  , fSentMsgs(0)
  , fParCList(NULL)
  , fGeoPar(nullptr)
  , fDigiPar(NULL)
    // *************************************
  , fMaxFEperCol(100)
  , fPitchX(0.01)
  , fPitchY(0.01)
  , fFeCols(100)
  , fFeRows(100)
  , actBoxDx(10.)
  , actBoxDy(10.)
  , actBoxDz(1.)
    // *************************************

{
}

FairMQEx9aFindHitsProcessor::~FairMQEx9aFindHitsProcessor()
{
  LOG(INFO) << "deteling fGeoPar"; 
  delete fGeoPar;
  fGeoPar=nullptr;
}


void FairMQEx9aFindHitsProcessor::Init()
{
  fDataToKeep        = fConfig->GetValue<std::string>("keep-data");
  fInputChannelName  = fConfig->GetValue<std::string>("in-channel");
  fOutputChannelName = fConfig->GetValue<std::string>("out-channel");
  fParamChannelName  = fConfig->GetValue<std::string>("par-channel");

    //fHitFinder->InitMQ(fRootParFileName,fAsciiParFileName);
  fGeoPar = new FairGeoParSet("FairGeoParSet");
  fParCList = new TList();
  fParCList->Add(fGeoPar);
  fDigiPar = new PixelDigiPar("PixelDigiParameters");
  fParCList->Add(fDigiPar);

  OnData(fInputChannelName, &FairMQEx9aFindHitsProcessor::ProcessData);
}

bool FairMQEx9aFindHitsProcessor::ProcessData(FairMQParts& parts, int /*index*/)
{
  // LOG(TRACE)<<"message received with " << parts.Size() << " parts!";
  fReceivedMsgs++;
  
  if ( parts.Size() == 0 ) return 0; // probably impossible, but still check

  // expecting even number of parts in the form: header,data,header,data,header,data and so on...
  int nPPE = 2; // nof parts per event

  if ( parts.Size()%nPPE >= 1 )
    LOG(INFO) << "received " << parts.Size() << " parts, will ignore last part!!!";

  // creating output multipart message
  FairMQParts partsOut;

  for ( int ievent = 0 ; ievent < parts.Size()/nPPE ; ievent++ ) {
    // the first part should be the event header
    PixelPayload::EventHeader* payloadE = static_cast<PixelPayload::EventHeader*>(parts.At(nPPE*ievent)->GetData());
    // LOG(TRACE) << "GOT EVENT " << payloadE->fMCEntryNo << " OF RUN " << payloadE->fRunId << " (part " << payloadE->fPartNo << ")";

    fNewRunId = payloadE->fRunId;
    if(fNewRunId!=fCurrentRunId)
      {
        fCurrentRunId=fNewRunId;
        UpdateParameters();

        LOG(INFO) << "Parameters updated, back to ProcessData(" << parts.Size() << " parts!)";
      }

    // the second part should the TClonesArray with necessary data... now assuming Digi
    PixelPayload::Digi* payloadD = static_cast<PixelPayload::Digi*>(parts.At(nPPE*ievent+1)->GetData());
    int digiArraySize = parts.At(nPPE*ievent+1)->GetSize();
    int nofDigis      = digiArraySize / sizeof(PixelPayload::Digi);

    // LOG(TRACE) << "    EVENT HAS " << nofDigis << " DIGIS!!!";

    // create eventHeader part
    PixelPayload::EventHeader* header = new PixelPayload::EventHeader();
    header->fRunId     = payloadE->fRunId;
    header->fMCEntryNo = payloadE->fMCEntryNo;
    header->fPartNo    = payloadE->fPartNo;
    FairMQMessagePtr msgHeader(NewMessage(header,
                                          sizeof(PixelPayload::EventHeader),
                                          [](void* data, void* /*hint*/) { delete static_cast<PixelPayload::EventHeader*>(data); }
                                          ));
    partsOut.AddPart(std::move(msgHeader));

    // create part with hits
    int nofHits = 0;
    PixelPayload::Hit* hitPayload = new PixelPayload::Hit[nofDigis];

    for ( int idigi = 0 ; idigi < nofDigis ; idigi++ ) {
      FindHit(payloadD[idigi].fDetectorID,
              payloadD[idigi].fFeID,
              payloadD[idigi].fCol,
              payloadD[idigi].fRow,
              idigi,
              hitPayload[idigi].posX,
              hitPayload[idigi].posY,
              hitPayload[idigi].posZ,
              hitPayload[idigi].dposX,
              hitPayload[idigi].dposY,
              hitPayload[idigi].dposZ);
      hitPayload[idigi].fDetectorID = payloadD[idigi].fDetectorID;
      nofHits++;
    }

    int hitsSize = nofHits*sizeof(PixelPayload::Hit);

    FairMQMessagePtr msgTCA(NewMessage(hitPayload,
                                       hitsSize,
                                       [](void* data, void* /*hint*/) { delete static_cast<PixelPayload::Hit*>(data); }
                                       ));
    partsOut.AddPart(std::move(msgTCA));
  }
  
  Send(partsOut, fOutputChannelName);
  fSentMsgs++;

  return true;
}

// should return -666 if hit not found
int FairMQEx9aFindHitsProcessor::FindHit(int detId, int feId, int col, int row, int index,
                                         double& posX, double& posY, double& posZ, 
                                         double& errX, double& errY, double& errZ) {

  double locPosCalc[3];
  locPosCalc[0] = ( ((feId-1)/fMaxFEperCol)*fFeCols + col + 0.5 )*fPitchX;
  locPosCalc[1] = ( ((feId-1)%fMaxFEperCol)*fFeRows + row + 0.5 )*fPitchY;
  locPosCalc[2] = 0.;
  
  locPosCalc[0] -= fDetIdToPixelBBoxMap[detId]->GetDX();
  locPosCalc[1] -= fDetIdToPixelBBoxMap[detId]->GetDY();
  
  double globPos[3];
  
  // LOG(INFO) << "node name = >>" << fDetIdToPixelNodeMap[detId]->GetName() << "<< title = >>" << fDetIdToPixelNodeMap[detId]->GetTitle() << "<<";
  // fDetIdToPixelNodeMap[detId]->GetMatrix()->Print();
  if ( !fDetIdToPixelNodeMap[detId] ) {
    LOG(ERROR) << "not able to find proper node for the given detId = " << detId << " (have " << fDetIdToPixelNodeMap.size() << " nodes in map)";
    return -1;
  }
  fDetIdToPixelNodeMap[detId]->LocalToMaster(locPosCalc,globPos);

  posX = globPos[0];
  posY = globPos[1];
  posZ = globPos[2];
  errX = fPitchX/sqrt(12.);
  errY = fPitchY/sqrt(12.);
  errZ = fDetIdToPixelBBoxMap[detId]->GetDZ();

// should return -666 if hit not found
  return index;
}
// -------------------------------------------------------------------------

void FairMQEx9aFindHitsProcessor::PostRun()
{
    MQLOG(INFO) << "FairMQEx9aFindHitsProcessor::PostRun() Received " << fReceivedMsgs << " and sent " << fSentMsgs << " messages!";
}


void FairMQEx9aFindHitsProcessor::CustomCleanup(void* /*data*/, void *hint)
{
    delete (std::string*)hint;
}


void FairMQEx9aFindHitsProcessor::UpdateParameters() {
  for ( int iparC = 0 ; iparC < fParCList->GetEntries() ; iparC++ ) {
    FairParGenericSet* tempObj = (FairParGenericSet*)(fParCList->At(iparC));
    fParCList->Remove(tempObj);
    fParCList->AddAt(UpdateParameter(tempObj),iparC);
  }
}

FairParGenericSet* FairMQEx9aFindHitsProcessor::UpdateParameter(FairParGenericSet* thisPar) {
  std::string paramName = thisPar->GetName();
  //  boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
  std::string* reqStr = new std::string(paramName + "," + std::to_string(fCurrentRunId));
  LOG(WARN) << "Requesting parameter \"" << paramName << "\" for Run ID " << fCurrentRunId << " (" << thisPar << ")";
  std::unique_ptr<FairMQMessage> req(NewMessage(const_cast<char*>(reqStr->c_str()), reqStr->length(), CustomCleanup, reqStr));
  std::unique_ptr<FairMQMessage> rep(NewMessage());
  
  if (Send(req,fParamChannelName) > 0)
    {
      if (Receive(rep,fParamChannelName) > 0)
	{
	  Ex9aTMessageFHP tm(rep->GetData(), rep->GetSize());
	  thisPar = (FairParGenericSet*)tm.ReadObject(tm.GetClass());
	  LOG(WARN) << "Received parameter"<< paramName <<" from the server (" << thisPar << ")";

          if ( paramName == "PixelDigiParameters" ) 
            { 
              fDigiPar = (PixelDigiPar*)thisPar;
              fFeCols = fDigiPar->GetFECols();
              fFeRows = fDigiPar->GetFERows();
              fMaxFEperCol = fDigiPar->GetMaxFEperCol();
              fPitchX = fDigiPar->GetXPitch();
              fPitchY = fDigiPar->GetYPitch();
              
              // LOG(INFO) << "FairMQEx9aFindHitsProc::UpdateParameter() fFeCols      = " << fFeCols;
              // LOG(INFO) << "FairMQEx9aFindHitsProc::UpdateParameter() fFeRows      = " << fFeRows;
              // LOG(INFO) << "FairMQEx9aFindHitsProc::UpdateParameter() fMaxFEperCol = " << fMaxFEperCol;
              // LOG(INFO) << "FairMQEx9aFindHitsProc::UpdateParameter() fPitchX      = " << fPitchX;
              // LOG(INFO) << "FairMQEx9aFindHitsProc::UpdateParameter() fPitchY      = " << fPitchY;
            }
          else // if ( paramName == "FairParGeoSet" )
            {
              // LOG(INFO) << "Should have gGeoManager";
              // gGeoManager->Print();
              TGeoNode* topNode = (TGeoNode*)gGeoManager->GetTopNode();
              if ( topNode ) 
                {
                  TObjArray* nodeList = topNode->GetNodes();
                  // LOG(INFO) << "topNode name is \"" << topNode->GetName() << "\" and it has " << nodeList->GetEntries() << " daughters";
                  for ( Int_t inode = 0 ; inode < nodeList->GetEntries() ; inode++ ) 
                    {
                      // LOG(INFO) << "subnode[" << inode << "] name is \"" << nodeList->At(inode)->GetName() << "\"";
                      //Pixel1_1 //  TString nodeName = Form("/cave/Pixel%d_%d",detId/256,detId%256);
                      string nodeName = nodeList->At(inode)->GetName();
                      if ( nodeName.find("Pixel") == 0 && nodeName.length() == 8 ) 
                        {
                          int detId = (int(nodeName[5])-48)*256+(int(nodeName[7])-48);
                          TGeoNode*   curNode   = (TGeoNode*)nodeList->At(inode);
                          TGeoVolume* curVolume = curNode->GetVolume();
                          TGeoBBox*   curBBox   = static_cast<TGeoBBox*>(curVolume->GetShape());
                          fDetIdToPixelNodeMap.insert(std::pair<int,TGeoNode*>(detId,curNode));
                          fDetIdToPixelBBoxMap.insert(std::pair<int,TGeoBBox*>(detId,curBBox));
                          // LOG(INFO) << "putting as [" << detId << "] node " << curNode << " and " << curBBox;
                        }
                    }
                }
            }

	  return thisPar;
	}
    } 

  return NULL;
}
