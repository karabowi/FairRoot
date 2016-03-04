/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
 
#include "BaseSourcePolicy.h"
#include "FairMQLogger.h"

#include "FairRunAna.h"
#include "FairFileSource.h"
#include "FairEventData.h"
#include "FairEventHeader.h"
//#include "FairLmdSource.h"
//#include "FairMbsSource.h"
//#include "FairMbsStreamSource.h"
//#include "FairMixedSource.h"
//#include "FairRemoteSource.h"
#include <type_traits>
#include <functional>

template<typename T, typename U>
  using enable_if_match2 = typename std::enable_if<std::is_same<T,U>::value ,int>::type;
template<typename T, typename U, typename V, typename W>
  using enable_if_match4 = typename std::enable_if<std::is_same<T,U>::value && std::is_same<V,W>::value ,int>::type;

struct tca{};
struct FevtData{};
template<typename FairSourceType, typename DataType, typename V>
  //template<typename FairSourceType, typename DataType>
  class FairSourceMQInterface : public BaseSourcePolicy<FairSourceMQInterface<FairSourceType, DataType, V>>
  //class FairSourceMQInterface : public BaseSourcePolicy<FairSourceMQInterface<FairSourceType, DataType>>
{
    typedef DataType* DataType_ptr;
    //    typedef FairSourceMQInterface<FairSourceType,DataType,V> this_type;
    typedef FairSourceMQInterface<FairSourceType,DataType,V> this_type;

  public:
    FairSourceMQInterface() :
    //    BaseSourcePolicy<FairSourceMQInterface<FairSourceType, DataType, V>>(),
    BaseSourcePolicy<FairSourceMQInterface<FairSourceType, DataType,V>>(),
        fSource(nullptr),
        fEventData(nullptr),
        fData(nullptr),
        fIndex(0),
        fMaxIndex(-1),
        fClassName(),
        fSourceName(),
        fBranchName(),
        fRunAna(nullptr)
    {
    }

    FairSourceMQInterface(const FairSourceMQInterface&) = delete;
    FairSourceMQInterface operator=(const FairSourceMQInterface&) = delete;

    virtual ~FairSourceMQInterface()
    {
        if (fData)
        {
            delete fData;
        }
        fData = nullptr;

	if (fEventData)
	{
	  delete fEventData;
	}
	fEventData = nullptr;

        if (fSource)
        {
            delete fSource;
        }
        fSource = nullptr;

        if (fRunAna)
        {
            delete fRunAna;
        }
        fRunAna = nullptr;
    }

    int64_t GetNumberOfEvent()
    {
        return fMaxIndex;
    }

    void SetFileProperties(const std::string &filename, const std::string &branchname)
    {
        fSourceName = filename;
        fBranchName = branchname;
    }


    //______________________________________________________________________________
    // FairFileSource

    template <typename T = FairSourceType, enable_if_match2<T, FairFileSource> = 0>
    void InitSource()
    {
        fRunAna = new FairRunAna();
        fSource = new FairSourceType(fSourceName.c_str());
        fSource->Init();
        fSource->ActivateObject((TObject**)&fData,fBranchName.c_str());
	fSource->ActivateObject((TObject**)&fEventHeader,"EventHeader.");
	fEventData = new FairEventData();
	fEventData->SetObject(fData);
	fMaxIndex = fSource->CheckMaxEventNo();
    }

    template <typename T = FairSourceType, enable_if_match2<T, FairFileSource> = 0>
    void SetIndex(int64_t eventIdx)
    {
        fIndex = eventIdx;
    }

    template <typename T = FairSourceType, typename U = V, enable_if_match4<T, FairFileSource,U,tca> = 0 > 
      DataType_ptr GetOutData()  
      {  
     	fSource->ReadEvent(fIndex);  
     	return fData;  
      } 
    
    template <typename T = FairSourceType, typename U=V, enable_if_match4<T, FairFileSource,U,FevtData> = 0 >
      FairEventData* GetOutData() 
      {
	fSource->ReadEvent(fIndex);
	fEventData->SetRunId        (fEventHeader->GetRunId()        );
	fEventData->SetEventTime    (fEventHeader->GetEventTime()    );
	fEventData->SetInputFileId  (fEventHeader->GetInputFileId()  );
	fEventData->SetMCEntryNumber(fEventHeader->GetMCEntryNumber());
	return fEventData;
      }
    
 protected:
    FairSourceType* fSource;
    FairEventData* fEventData;
    FairEventHeader* fEventHeader;
    DataType_ptr fData;
    int64_t fIndex;
    int64_t fMaxIndex;
    std::string fClassName;
    std::string fBranchName;
    std::string fSourceName;
    FairRunAna* fRunAna;
};
