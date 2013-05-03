// -------------------------------------------------------------------------
// -----            FairAnaSelector header file                        -----
// -----            Created 14/10/11  by R. Karabowicz                 -----
// -----            Updated 01/02/12  by R. Karabowicz                 -----
// -------------------------------------------------------------------------

#ifndef FAIRANASELECTOR_H
#define FAIRANASELECTOR_H

#include "TSelector.h"                  // for TSelector

#include "Rtypes.h"                     // for Int_t, Bool_t, etc
#include "TSelectorList.h"              // for TSelectorList
#include "TString.h"                    // for TString
#include "TTree.h"                      // for TTree

#include <stddef.h>                     // for NULL

class FairRunAna;

class TFile;
class TList;
class TObject;
class TProofOutputFile;

class FairAnaSelector : public TSelector
{
  public :
    TProofOutputFile*     fProofFile;
    TFile*          fFile;
    TTree*          fChain;   //!pointer to the analyzed TTree or TChain
    FairRunAna*     fRunAna;

    FairAnaSelector(TTree* /*tree*/ =0) : fProofFile(0), fFile(0), fChain(0), fRunAna(NULL) { }
    virtual ~FairAnaSelector() { }
    virtual Int_t   Version() const {
      return 1;
    }
    virtual void    Begin(TTree* tree);
    virtual void    SlaveBegin(TTree* tree);
    virtual void    Init(TTree* tree);
    virtual Bool_t  Notify();
    virtual Bool_t  Process(Long64_t entry);
    virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) {
      return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0;
    }
    virtual void    SetOption(const char* option) {
      fOption = option;
    }
    virtual void    SetObject(TObject* obj) {
      fObject = obj;
    }
    virtual void    SetInputList(TList* input) {
      fInput = input;
    }
    virtual TList*  GetOutputList() const {
      return fOutput;
    }
    virtual void    SlaveTerminate();
    virtual void    Terminate();

    void SetFairRunAna(FairRunAna* runAna) {
      fRunAna = runAna;
    }

  private:

    FairAnaSelector(const FairAnaSelector&);
    FairAnaSelector operator=(const FairAnaSelector&);

    ClassDef(FairAnaSelector,0);
};

#endif //FAIRANASELECTOR_H
