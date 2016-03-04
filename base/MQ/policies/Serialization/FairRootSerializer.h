/* 
 * File:   RootSerializer.h
 * Author: winckler
 *
 * Created on January 14, 2015, 3:44 PM
 */

#ifndef FAIRROOTBASECLASSSERIALIZER_H
#define	FAIRROOTBASECLASSSERIALIZER_H

//std
#include <iostream>
//Root
#include "TClonesArray.h"
#include "TMessage.h"
#include "TObject.h"
//FairRoot
#include "FairEventData.h"
#include "FairMQMessage.h"
#include "RootSerializer.h"

//template <typename TPayload>
class FairRootSerializer
{
  public:
    FairRootSerializer()
        : fContainer(nullptr)
        , fMessage(nullptr)
        , fNumInput(0)
    {}

    FairRootSerializer(const FairRootSerializer&) = delete;
    FairRootSerializer operator=(const FairRootSerializer&) = delete;

    virtual ~FairRootSerializer()
    {
        if(fContainer)
            delete fContainer;
        fContainer=nullptr;
    }

    /* void InitContainer(const std::string &ClassName) */
    /* { */
    /*     fContainer = new TClonesArray(ClassName.c_str()); */
    /* } */

     void InitContainer(const std::string &ClassName) 
     { 
       fContainer = new FairEventData();
       fTCA = new TClonesArray(ClassName.c_str()); 
       fContainer->SetObject(fTCA);
     } 

    /* void InitContainer(FairEventData* mCont) */
    /* { */
    /*     fContainer = mCont; */
    /* } */
    
    ////////////////////////////////////////////////////////////////////////////////////////
    // serialize

    void DoSerialization(FairEventData* mCont)
    {
        TMessage* tm = new TMessage(kMESS_OBJECT);
        tm->WriteObject(mCont);
	/* MQLOG(INFO) << " mcont --> " << mCont->GetRunId() << " / " << mCont->GetMCEntryNumber(); */
	/* MQLOG(INFO) << ((TClonesArray*)mCont->GetObject())->GetEntries(); */
        // fMessage = fTransportFactory->CreateMessage(tm->Buffer(), tm->BufferSize(), free_tmessage, tm);
        fMessage->Rebuild(tm->Buffer(), tm->BufferSize(), free_tmessage, tm);
    }

    FairMQMessage* SerializeMsg(FairEventData* mCont)
    {
        DoSerialization(mCont);
        return fMessage;
    }

    void SetMessage(FairMQMessage* msg)
    {
        fMessage=msg;
    }

    FairMQMessage* GetMessage()
    {
        return fMessage;
    }

  protected:
    // TPayload* fPayload;
    FairEventData* fContainer;
    TClonesArray* fTCA;
    FairMQMessage* fMessage;
    int fNumInput;
};

class FairRootDeSerializer
{
  public:
    FairRootDeSerializer()
        : fContainer(nullptr)
        , fMessage(nullptr)
        , fNumInput(0)
    {}

    FairRootDeSerializer(const FairRootDeSerializer&) = delete;
    FairRootDeSerializer operator=(const FairRootDeSerializer&) = delete;

    virtual ~FairRootDeSerializer()
    {
        if(fContainer)
            delete fContainer;
        fContainer=nullptr;
    }

    /* void InitContainer(const std::string &ClassName) */
    /* { */
    /*     fContainer = new TClonesArray(ClassName.c_str()); */
    /* } */

     void InitContainer(const std::string &ClassName) 
     { 
       fContainer = new FairEventData();
       fTCA = new TClonesArray(ClassName.c_str()); 
       fContainer->SetObject(fTCA);
     } 

    /* void InitContainer(FairEventData* mCont) */
    /* { */
    /*     fContainer = mCont; */
    /* } */

    ////////////////////////////////////////////////////////////////////////////////////////
    // deserialize

    void DoDeSerialization(FairMQMessage* msg)
    {
        FairTMessage tm(msg->GetData(), msg->GetSize());
        fContainer  = (FairEventData*)(tm.ReadObject(tm.GetClass()));
	/* MQLOG(INFO) << " mcont --> " << fContainer->GetRunId() << " / " << fContainer->GetMCEntryNumber(); */
	/* MQLOG(INFO) << ((TClonesArray*)fContainer->GetObject())->GetEntries(); */
    }
    
    FairEventData* DeserializeMsg(FairMQMessage* msg)
    {
      DoDeSerialization(msg);
        return fContainer;
    }
    
    void SetMessage(FairMQMessage* msg)
    {
        fMessage=msg;
    }

    FairMQMessage* GetMessage()
    {
        return fMessage;
    }

  protected:
    // TPayload* fPayload;
    FairEventData* fContainer;
    TClonesArray* fTCA;
    FairMQMessage* fMessage;
    int fNumInput;
};

#endif /* FAIRROOTBASECLASSSERIALIZER_H */
