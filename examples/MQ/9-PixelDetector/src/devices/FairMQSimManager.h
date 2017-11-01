/********************************************************************************
 *    Copyright (C) 2017 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

/**
 * FairMQSimManager.h
 *
 * @since 2017-10-24
 * @author R. Karabowicz
 */

#ifndef FAIRMQSIMMANAGER_H_
#define FAIRMQSIMMANAGER_H_

#include <string>

#include "FairGenericRootManager.h"
#include "FairMQDevice.h"

class FairRunSim;
class TObjArray;
class FairPrimaryGenerator;

class FairMQSimManager : public FairMQDevice, public FairGenericRootManager
{
 public:
    FairMQSimManager();
    virtual ~FairMQSimManager();
  
    virtual void                Register(const char* name, const char* folderName, TNamed* obj, Bool_t toFile);
    virtual void                Register(const char* name,const char* folderName ,TCollection* obj, Bool_t toFile);
    virtual void                RegisterInputObject(const char* name, TObject* obj);
    virtual TClonesArray*       Register(TString branchName, TString className, TString folderName, Bool_t toFile);
    virtual FairWriteoutBuffer* RegisterWriteoutBuffer(TString branchName, FairWriteoutBuffer* buffer);

    virtual void  Fill();
    virtual void  Write();
    virtual void  CloseOutFile();
    // virtual void  WriteAndClose() = 0;

    virtual Int_t               GetBranchId(TString const &BrName);

    // access to the implementation class
    virtual FairRootManager*    GetFairRootManager() const;
    virtual Int_t  GetId() const;

    // ------ FairRunSim setters ------
    void SetNofEvents       (int64_t nofev)                 { fNofEvents = nofev;};
    void SetTransportName   (std::string str)               { fTransportName = str;};
    void SetMaterials       (std::string str)               { fMaterialsFile = str;};
    void SetDetectorArray   (TObjArray* array)              { fDetectorArray = array;};
    void SetGenerator       (FairPrimaryGenerator* primGen) { fPrimaryGenerator = primGen;};
    void SetStoreTraj       (bool flag=true)                { fStoreTrajFlag = flag;};
    // ------ ---------- -------- ------

 protected:
    virtual void InitTask();
    virtual void PreRun();
    virtual void PostRun();
    virtual bool ConditionalRun();
    
 private: 
    std::string fUpdateChannelName;

    FairRunSim*     fRunSim;
    // ------ FairRunSim settings ------
    int64_t               fNofEvents;
    std::string           fTransportName;
    std::string           fMaterialsFile;
    TObjArray*            fDetectorArray;
    FairPrimaryGenerator* fPrimaryGenerator;
    bool                  fStoreTrajFlag;
    // ------ ---------- -------- ------

    void UpdateParameterServer();
    void SendObject(TObject* obj, std::string chan);

    FairMQSimManager(const FairMQSimManager&);
    FairMQSimManager& operator=(const FairMQSimManager&);
};

#endif /* FAIRMQSIMMANAGER_H_ */
