/********************************************************************************
 * Copyright (C) 2014-2022 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH  *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
#ifndef FAIRTESTDETECTOR_H_
#define FAIRTESTDETECTOR_H_

#include "FairDetector.h"

#include <Rtypes.h>           // for Int_t, Double32_t, Double_t, etc
#include <TClonesArray.h>     // for TClonesArray
#include <TLorentzVector.h>   // for TLorentzVector
#include <TVector3.h>         // for TVector3

class FairTestDetectorPoint;
class FairVolume;
class TClonesArray;

class FairTestDetector : public FairDetector
{
  public:
    /**      Name :  Detector Name
     *       Active: kTRUE for active detectors (ProcessHits() will be called)
     *               kFALSE for inactive detectors
     */
    FairTestDetector(const char* Name, Bool_t Active);

    /**      default constructor    */
    FairTestDetector();

    /**       destructor     */
    ~FairTestDetector() override;

    /**      Initialization of the detector is done here    */
    void Initialize() override;

    /**       this method is called for each step during simulation
     *       (see FairMCApplication::Stepping())
     */
    Bool_t ProcessHits(FairVolume* v = 0) override;

    /**       Registers the produced collections in FAIRRootManager.     */
    void Register() override;

    /** Gets the produced collections */
    TClonesArray* GetCollection(Int_t iColl) const override;

    /**      has to be called after each event to reset the containers      */
    void Reset() override;

    /**      Create the detector geometry        */
    void ConstructGeometry() override;

    /**      This method is an example of how to add your own point
     *       of type FairTestDetectorPoint to the clones array
     */
    FairTestDetectorPoint* AddHit(Int_t trackID,
                                  Int_t detID,
                                  TVector3 pos,
                                  TVector3 mom,
                                  TVector3 posOut,
                                  TVector3 momOut,
                                  Double_t time,
                                  Double_t length,
                                  Double_t eLoss);

    /** The following methods can be implemented if you need to make
     *  any optional action in your detector during the transport.
     */

    /*
    virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
    {
        ;
    }
*/
    void SetSpecialPhysicsCuts() override { ; }
    void EndOfEvent() override;
    void FinishPrimary() override { ; }
    void FinishRun() override { ; }
    void BeginPrimary() override { ; }
    void PostTrack() override { ; }
    void PreTrack() override { ; }
    void BeginEvent() override { ; }

    /** Necessary function to enable this detector in TGeant4 MT mode */
    FairModule* CloneModule() const override;

    /** Necessary function to enable this detector in TGeant4 MT mode */
    Bool_t IsSensitive(const std::string& name) override;

  private:
    /** Track information to be stored until the track leaves the
    active volume.
    */
    Int_t fTrackID{-1};         //!  track index
    Int_t fVolumeID{-1};        //!  volume id
    TLorentzVector fPos{};      //!  position at entrance
    TLorentzVector fMom{};      //!  momentum at entrance
    TLorentzVector fPosOut{};   //!  position at entrance
    TLorentzVector fMomOut{};   //!  momentum at entrance
    Double32_t fTime{-1.};      //!  time
    Double32_t fLength{-1.};    //!  length
    Double32_t fELoss{-1.};     //!  energy loss
    Int_t fEventNr{0};          //! current event number;

    std::unique_ptr<TClonesArray> fFairTestDetectorPointCollection{new TClonesArray("FairTestDetectorPoint")};   //!

    FairTestDetector(const FairTestDetector&);
    FairTestDetector& operator=(const FairTestDetector&);

    ClassDefOverride(FairTestDetector, 1);
};

#endif /* FAIRTESTDETECTOR_H_ */
