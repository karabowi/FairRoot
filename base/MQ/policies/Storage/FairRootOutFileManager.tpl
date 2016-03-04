/* 
 * File:   FairRootOutFileManager.tpl
 * Author: R. Karabowicz
 *
 * Created on March 2, 2016
 */

template <typename DataType>
FairRootOutFileManager<DataType>::FairRootOutFileManager() 
  : BaseSinkPolicy<FairRootOutFileManager<DataType>>(),
    fFileName(),
    fTreeName(),
    fBranchName(),
    fClassName(),
    fFileOption(),
    //    fUseClonesArray(false),
    fUseEventData(false),
    fFlowMode(true),
    fWrite(false),
    fOutFile(nullptr),
    fTree(nullptr),
    fOutput(nullptr),
    fOutputData(nullptr),
    fOutputEventData(nullptr),
    fOutputEventHeader(nullptr),
    fFolder(nullptr)
{
}

template <typename DataType>
FairRootOutFileManager<DataType>::FairRootOutFileManager(const std::string &filename, const std::string &treename, const std::string &branchname, const std::string &Classname, const std::string &FileOption)
  : BaseSinkPolicy<FairRootOutFileManager<DataType>>(),
    fFileName(filename),
    fTreeName(treename),
    fBranchName(branchname),
    fClassName(Classname),
    fFileOption(FileOption),
    //    fUseClonesArray(false),
    fUseEventData(false),
    fFlowMode(true),
    fWrite(false),
    fOutFile(nullptr),
    fTree(nullptr),
    fOutput(nullptr),
    fOutputData(nullptr),
    fOutputEventData(nullptr),
    fOutputEventHeader(nullptr),
    fFolder(nullptr)
{
}

template <typename DataType>
FairRootOutFileManager<DataType>::~FairRootOutFileManager() 
{
    if (fFlowMode && fWrite)
        fTree->Write("", TObject::kOverwrite);

    if (fTree)
        delete fTree;

    if (fOutFile)
    {
        if (fOutFile->IsOpen())
            fOutFile->Close();
        delete fOutFile;
    }
    
    if (fFolder)
        delete fFolder;
}

template <typename DataType>
void FairRootOutFileManager<DataType>::SetFileProperties(const std::string &filename, const std::string &treename, const std::string &branchname, const std::string &ClassName, const std::string &FileOption, bool UseEventData, bool flowmode)
{
    fFileName=filename;
    fTreeName=treename;
    fBranchName=branchname;
    fClassName=ClassName;
    fFileOption=FileOption;
    fUseEventData=UseEventData;
    fFlowMode=flowmode;
    if (!std::is_base_of<TObject, DataType>::value && fUseEventData)
    {
        fUseEventData=false;
        MQLOG(WARN)<<"Deactivate TClonesArray method : the data class must inherit from TObject.";
    } 
}

template <typename DataType>
void FairRootOutFileManager<DataType>::SetRootFileProperties(const std::string &filename, const std::string &treename, const std::string &branchname, const std::string &ClassName, const std::string &FileOption, bool UseEventData, bool flowmode)
{
    SetFileProperties(filename, treename, branchname, ClassName, FileOption, UseEventData, flowmode);
}

template <typename DataType>
std::vector<std::vector<DataType> > FairRootOutFileManager<DataType>::GetAllObj(const std::string &filename, const std::string &treename, const std::string &branchname)
{
    fWrite=false;
    TFile* file=TFile::Open(filename.c_str(),"READ");
    std::vector<std::vector<DataType> > Allobj;
    std::vector<DataType> TempObj;
    if (file)
    {
        fTree=(TTree*)file->Get(fTreeName.c_str());
    }
    else
        MQLOG(ERROR)<<"Could not open file"<<fTreeName.c_str();

    if (fTree)
    {
        if (fUseEventData)
        {
	    fOutput = new TClonesArray(fClassName.c_str());
	    fOutputEventData = new FairEventData();
	    fOutputEventData->SetObject(fOutput);
            fTree->SetBranchAddress(fBranchName.c_str(), &fOutput);
	    fOutputEventHeader = new FairEventHeader();
	    fTree->SetBranchAddress("EventHeader.",&fOutputEventHeader);

            for (Long64_t i = 0; i < fTree->GetEntries(); i++)
            {
                TempObj.clear();
                fTree->GetEntry(i);
                for (Int_t iobj = 0; iobj < fOutput->GetEntriesFast(); ++iobj)
                {
                    DataType* Data_i = reinterpret_cast<DataType*>(fOutput->At(iobj));
                    if (!Data_i)
                        continue;
                    TempObj.push_back(*Data_i);
                }
                Allobj.push_back(TempObj);
            }
        }
        else
        {
            fTree->SetBranchAddress(branchname.c_str(),&fOutputData);
            for (Long64_t i = 0; i < fTree->GetEntries(); i++)
            {
                fTree->GetEntry(i);
                DataType Data_i=*fOutputData;
                TempObj.push_back(Data_i);
            }
            Allobj.push_back(TempObj);
        }
    }
    else
        MQLOG(ERROR)<<"Could not find tree "<<treename.c_str();

    if (file) file->Close();
    return Allobj;
}

template <typename DataType>
void FairRootOutFileManager<DataType>::AddToFile(std::vector<DataType>& InputData) 
{
    if (fUseEventData)
      {
        fOutput->Delete();

        for (unsigned int i(0); i < InputData.size(); ++i)
	{
            new ((*fOutput)[i]) DataType(InputData.at(i));
        }

        if (fOutput->IsEmpty())
        {
            MQLOG(DEBUG) << "FairRootOutFileManager::AddToFile(vector<DataType>&): No Output array!";
        }

        fTree->Fill();
    }
    else
    {
        for (unsigned int i(0); i < InputData.size(); ++i)
        {
            fOutputData=&InputData.at(i);
            fTree->Fill();
        }
    }
    if (!fFlowMode)
        fTree->Write("", TObject::kOverwrite);
}

template <typename DataType>
void FairRootOutFileManager<DataType>::AddToFile(TClonesArray* InputData) 
{
    if (fUseEventData)
      {
        fOutput=InputData;
        fTree->SetBranchAddress(fBranchName.c_str(), &fOutput);

        if (fOutput->IsEmpty())
        {
            MQLOG(DEBUG) << "FairRootOutFileManager::AddToFile(TClonesArray*): No Output array!";
        }
        fTree->Fill();
        if (!fFlowMode)
            fTree->Write("", TObject::kOverwrite);
    }
    else
    {
        MQLOG(DEBUG) << "FairRootOutFileManager::AddToFile(TClonesArray*): TClonesArray not set as output container";
    }
}

template <typename DataType>
void FairRootOutFileManager<DataType>::AddToFile(FairEventData* eventData) {
    if (fUseEventData)
      {
        fOutputEventData=eventData;
	fOutput            = (TClonesArray*)fOutputEventData->GetObject();
	fOutputEventHeader = (FairEventHeader*)eventData;
        fTree->SetBranchAddress(fBranchName.c_str(), &fOutput);
	fTree->SetBranchAddress("EventHeader.", &fOutputEventHeader);

        fTree->Fill();
        if (!fFlowMode)
            fTree->Write("", TObject::kOverwrite);
    }
    else
    {
        MQLOG(DEBUG) << "FairRootOutFileManager::AddToFile(TClonesArray*): TClonesArray not set as output container";
    }
}

template <typename DataType>
void FairRootOutFileManager<DataType>::AddToFile(DataType* ObjArr, long size)
{
    //todo : check if fine when update option
    if (fUseEventData)
    {
        fOutput->Delete();

        for (unsigned int i(0); i < size; ++i)
        {
            new ((*fOutput)[i]) DataType(ObjArr[i]);
        }

        if (fOutput->IsEmpty())
        {
            MQLOG(DEBUG) << "FairRootOutFileManager::AddToFile(vector<DataType>&): No Output array!";
        }

        fTree->Fill();
        if (!fFlowMode)
            fTree->Write("", TObject::kOverwrite);
    }
    else
    {
        std::vector<DataType>  DataVector(ObjArr, ObjArr +size);
        AddToFile(DataVector);
    }
}

template <typename DataType>
void FairRootOutFileManager<DataType>::AddToFile(FairMQMessage* msg)
{
    int inputSize = msg->GetSize();
    int NumInput=0;
    if (inputSize>0)
        NumInput = inputSize / sizeof(DataType);
    fOutputData = static_cast<DataType*>(msg->GetData());
    AddToFile(fOutputData,NumInput);
}

template <typename DataType>
void FairRootOutFileManager<DataType>::InitOutputFile()
{
    fWrite=true;
    fOutFile = TFile::Open(fFileName.c_str(),fFileOption.c_str());

    bool updateTree=false;

    // if given option is update attempt to get tree from file
    if (fFileOption=="UPDATE")
    {
        fTree=(TTree*)fOutFile->Get(fTreeName.c_str());
        if (fTree) 
        {
            updateTree=true;
            MQLOG(INFO) <<"Update tree";
        }
        else
        {
            updateTree=false;
            MQLOG(INFO) <<"Create new tree";
        }
    }

    // if tree not found or option is not UPDATE, create a new tree
    if (!updateTree)
        fTree = new TTree(fTreeName.c_str(), "Test output");

    // direct storage or TClonesArray
    if (fUseEventData)
      {
        fOutput = new TClonesArray(fClassName.c_str());
	fOutputEventData = new FairEventData();
	fOutputEventData->SetObject(fOutput);
	fOutputEventHeader = new FairEventHeader();
	fOutputEventHeader->SetName("EventHeader.");
        if (updateTree) {
            fTree->SetBranchAddress(fBranchName.c_str(), &fOutput);
	    fTree->SetBranchAddress("EventHeader.",&fOutputEventHeader);
	}
        else {
            fTree->Branch(fBranchName.c_str(),"TClonesArray", &fOutput);
	    fTree->Branch("EventHeader.","FairEventHeader",&fOutputEventHeader);
	}
    }
    else
    {
        if (updateTree)
            fTree->SetBranchAddress(fBranchName.c_str(), &fOutputData);
        else
            fTree->Branch(fBranchName.c_str(),fClassName.c_str(), &fOutputData);
    }

    fFolder = new TFolder("cbmout", "Main Output Folder");
    
    TFolder* fold = fFolder->AddFolder("Pixel","Pixel");
    fold->Add(fOutput);
    TFolder* foldEventHeader = fFolder->AddFolder("EvtHeader","EvtHeader");
    foldEventHeader->Add(fOutputEventHeader);
    fFolder->Write();

    TList* BranchNameList = new TList();
    BranchNameList->AddLast(new TObjString("EventHeader."));
    BranchNameList->AddLast(new TObjString(fBranchName.c_str()));
    BranchNameList->Write("BranchList", TObject::kSingleKey);

    BranchNameList->Delete();
    delete BranchNameList;
}

template <typename DataType>
void FairRootOutFileManager<DataType>::Init()
{
    fWrite=true;
    fOutFile = TFile::Open(fFileName.c_str(),fFileOption.c_str());

    bool updateTree = false;

    // if given option is update attempt to get tree from file
    if (fFileOption == "UPDATE")
    {
        fTree = (TTree*)fOutFile->Get(fTreeName.c_str());
        if (fTree) 
        {
            updateTree=true;
            MQLOG(INFO) <<"Update tree";
        }
        else
        {
            updateTree=false;
            MQLOG(INFO) <<"Create new tree";
        }
    }

    // if tree not found or option is not UPDATE, create a new tree
    if (!updateTree)
        fTree = new TTree(fTreeName.c_str(), "Test output");

    // direct storage or TClonesArray
    if (fUseEventData)
    {
        fOutput = new TClonesArray(fClassName.c_str());
	fOutputEventData->SetObject(fOutput);
	fOutputEventHeader = new FairEventHeader();
	fOutputEventHeader->SetName("EventHeader.");
	fTree->SetBranchAddress("EventHeader.",&fOutputEventHeader);
	fTree->SetBranchAddress(fBranchName.c_str(), &fOutput);
        if (updateTree)
            fTree ->SetBranchAddress(fBranchName.c_str(), &fOutput);
        else
            fTree->Branch(fBranchName.c_str(),"TClonesArray", &fOutput);
    }
    else
    {
        if (updateTree)
            fTree->SetBranchAddress(fBranchName.c_str(), &fOutputData);
        else
            fTree->Branch(fBranchName.c_str(),fClassName.c_str(), &fOutputData);
    }
}
