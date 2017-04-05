/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

#ifndef FAIRMQEXAMPLE9aFINDHITSPROCESSOR_H_
#define FAIRMQEXAMPLE9aFINDHITSPROCESSOR_H_ 
#include <string>

#include "FairEventHeader.h"
#include "FairGeoParSet.h"
#include "FairParGenericSet.h"

#include "FairMQDevice.h"
#include "FairMQParts.h"

#include "TClonesArray.h"
#include "TList.h"
#include "TMessage.h"

class TGeoNode;
class TGeoBBox;

#include "PixelPayload.h"
#include "PixelDigi.h"

class PixelDigiPar;

class FairMQEx9aFindHitsProcessor : public FairMQDevice
{
  public:
    FairMQEx9aFindHitsProcessor();
    virtual ~FairMQEx9aFindHitsProcessor();

    void SetDataToKeep(std::string tStr) { fDataToKeep = tStr;}

    void SetInputChannelName (std::string tstr) {fInputChannelName = tstr;}
    void SetOutputChannelName(std::string tstr) {fOutputChannelName = tstr;}
    void SetParamChannelName (std::string tstr) {fParamChannelName  = tstr;}

  protected:
    bool ProcessData(FairMQParts&, int);
    virtual void Init();
    virtual void PostRun();

 private:
    std::string     fInputChannelName;
    std::string     fOutputChannelName;
    std::string     fParamChannelName;

    static void CustomCleanup(void *data, void *hint);

    FairEventHeader* fEventHeader;

    int fNewRunId;
    int fCurrentRunId;

    std::string fDataToKeep;

    int fReceivedMsgs = 0;
    int fSentMsgs = 0;

    TList* fParCList;
    FairGeoParSet* fGeoPar;
    PixelDigiPar*     fDigiPar;

    void UpdateParameters();
    FairParGenericSet* UpdateParameter(FairParGenericSet* thisPar);
    // *************************************
    // geometry parameters:
    int    fMaxFEperCol;
    double fPitchX;
    double fPitchY;

    int    fFeCols;
    int    fFeRows;

    std::map <int,TGeoNode*> fDetIdToPixelNodeMap;
    std::map <int,TGeoBBox*> fDetIdToPixelBBoxMap;
    double actBoxDx; //array
    double actBoxDy; //array
    double actBoxDz; //array
    // *************************************
    int FindHit(int detId, int feId, int col, int row, int index,
                double& posX, double& posY, double& posZ, 
                double& errX, double& errY, double& errZ);
      
    FairMQEx9aFindHitsProcessor(const FairMQEx9aFindHitsProcessor&);    
    FairMQEx9aFindHitsProcessor& operator=(const FairMQEx9aFindHitsProcessor&);    
};

#endif /* FAIRMQEXAMPLE9aFINDHITSPROCESSOR_H_ */
