/* 
 * File:   FairRootOutFileManager.h
 * Author: R. Karabowicz
 *
 * Created on March 2, 2016
 */

#ifndef FAIRROOTOUTFILEMANAGER_H
#define	FAIRROOTOUTFILEMANAGER_H

// std
#include <iostream>
#include <vector>
#include <string>

// ROOT
#include "Rtypes.h"
#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "FairEventData.h"
#include "FairEventHeader.h"

#include "TKey.h"
#include "TFolder.h"
#include "TObjString.h"
#include "TList.h"

// FairRoot
#include "FairMQLogger.h"
#include "FairMQMessage.h"
#include "BaseSinkPolicy.h"

template <typename DataType>
class FairRootOutFileManager : public BaseSinkPolicy<FairRootOutFileManager<DataType>>
{
  public:
    FairRootOutFileManager();
    FairRootOutFileManager(const std::string &filename, const std::string &treename, 
                     const std::string &branchname, const std::string &Classname, const std::string &FileOption);

    FairRootOutFileManager(const FairRootOutFileManager&) = delete;
    FairRootOutFileManager operator=(const FairRootOutFileManager&) = delete;

    virtual ~FairRootOutFileManager();

    void SetRootFileProperties(const std::string &filename, const std::string &treename, 
			       const std::string &branchname, const std::string &Classname="", 
			       const std::string &FileOption="RECREATE", bool UseEventData=false, bool flowmode=true);
    
    void SetFileProperties(const std::string &filename, const std::string &treename, 
                           const std::string &branchname, const std::string &Classname="", 
                           const std::string &FileOption="RECREATE", bool UseEventData=false, bool flowmode=true);

    void AddToFile(std::vector<DataType>& InputData);
    void AddToFile(DataType* ObjArr, long size);
    void AddToFile(TClonesArray* InputData);
    void AddToFile(FairEventData* eventData);
    void AddToFile(FairMQMessage* msg);
    void InitOutputFile();
    void InitTCA(const std::string &classname);
    std::vector<std::vector<DataType> > GetAllObj(const std::string &filename,
                                                  const std::string &treename,
                                                  const std::string &branchname
                                                  );

  protected:
    virtual void Init();

    std::string fFileName;
    std::string fTreeName;
    std::string fBranchName;
    std::string fClassName;
    std::string fFileOption;
    //    bool fUseClonesArray;
    bool fUseEventData;
    bool fFlowMode;
    bool fWrite;

    TFile* fOutFile;
    TTree* fTree;
    TClonesArray* fOutput;
    FairEventHeader* fOutputEventHeader;
    FairEventData* fOutputEventData;
    DataType* fOutputData;
    TFolder* fFolder;
};

#include "FairRootOutFileManager.tpl"

#endif /* FAIRROOTOUTFILEMANAGER_H */
