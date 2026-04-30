//****************************************************************************
//*                   This file is part of PandaRoot.                        *
//*                                                                          *
//*            PandaRoot is distributed under the terms of the               *
//*              GNU General Public License (GPL) version 3,                 *
//*                 copied verbatim in the file "LICENSE".                   *
//*                                                                          *
//*  Copyright (C) 2006 - 2024 FAIR GmbH and copyright holders of PandaRoot  *
//*     The copyright holders are listed in the file "COPYRIGHTHOLDERS".     *
//*               The authors are listed in the file "AUTHORS".              *
//****************************************************************************


#ifndef PNDSTT2_H
#define PNDSTT2_H

#include "FairDetector.h"
//#include "PndStt2GeoPar.h"
//#include "PndGeoSttPar.h"
#include "PndStt2Point.h"

#include <TLorentzVector.h>
#include <TVector3.h>

class FairVolume;
class TClonesArray;
//class PndGeoHandling;
class PndStt2GeoHandler;
//class PndStt2GeoPar;
class PndGeoSttPar;

class PndStt2 : public FairDetector
{

  public:
    /**      Name :  Detector Name
     *       Active: kTRUE for active detectors (ProcessHits() will be called)
     *               kFALSE for inactive detectors
     */
    PndStt2(const char* Name, Bool_t Active);

    /**      default constructor    */
    PndStt2();

    /**       destructor     */
    virtual ~PndStt2();

    /**      Initialization of the detector is done here    */
    virtual void Initialize();

    /**       this method is called for each step during simulation
     *       (see FairMCApplication::Stepping())
     */
    virtual Bool_t ProcessHits(FairVolume* v = 0);

    /**       Registers the produced collections in FAIRRootManager.     */
    virtual void Register();

    /** Gets the produced collections */
    virtual TClonesArray* GetCollection(Int_t iColl) const;

    /**      has to be called after each event to reset the containers      */
    virtual void Reset();

    /**      Create the detector geometry        */
    void ConstructGeometry();

    /**      This method is an example of how to add your own point
     *       of type PndStt2Point to the clones array
     */
    void AddHit(Int_t trackID, Int_t detID, Int_t tubeID, TVector3 posIn, TVector3 posOut, TVector3 momIn, TVector3 momOut, Double_t time, Double_t length, Double_t eLoss);

    /** The following methods can be implemented if you need to make
     *  any optional action in your detector during the transport.
     */
     
    /** Private method ResetParameters
     **
     ** Resets the private members for the track parameters
     **/
    void ResetParameters() {};

    virtual void SetSpecialPhysicsCuts() { ; }
    virtual void EndOfEvent();
    virtual void FinishPrimary() { ; }
    virtual void FinishRun();
    virtual void BeginPrimary() { ; }
    virtual void PostTrack() { ; }
    virtual void PreTrack() { ; }
    virtual void BeginEvent() { ; }

    virtual FairModule* CloneModule() const;

    virtual Bool_t IsSensitive(const std::string& name);

    void InitOnWorker();

    std::vector<FairMCPoint*> GetPointVectorForIndex(Int_t iColl) const override {
        if (iColl == 0) {
            // Build a vector of base pointers from the concrete vector — no copy of data
            std::vector<FairMCPoint*> ptrs;
            ptrs.reserve(fPointVector->size());
            for (auto& p : *fPointVector)
                ptrs.push_back(&p);   // FairMCPoint* points into the existing vector elements
            return ptrs;
        }
        return {};
    }

   protected:
    std::vector<std::string> fListOfSensitives;
    bool CheckIfSensitive(std::string name);

    /** Track information to be stored until the track leaves the
    active volume.
    */
    Int_t fTrackID;         //!  track index
    Int_t fDetID;           //!  detector id
    Int_t fTubeID;          //!  tube id
    TLorentzVector fPosIn;  //!  position at entrance
    TLorentzVector fPosOut; //!  position at entrance
    TLorentzVector fMomIn;  //!  momentum at entrance
    TLorentzVector fMomOut; //!  momentum at entrance
    Double32_t fTime;                //!  time
    Double32_t fLength;              //!  length
    Double32_t fELoss;               //!  energy loss

    //PndGeoHandling *fGeoH;  //! Gives Access to the Path info of a hit
    PndStt2GeoHandler *fSttGeoH;
    //PndStt2GeoPar *fSttGeoPar;
    PndGeoSttPar *fSttGeoPar;

    /** container for data points */
    //    TClonesArray* fPndStt2PointCollection;   //!
    std::vector<PndStt2Point>* fPointVector{new std::vector<PndStt2Point>};

        PndStt2(const PndStt2&);
        PndStt2& operator=(const PndStt2&);

    void DefineSensitiveVolumes();
    void SetDefaultSensorNames() { fListOfSensitives.push_back("ArCO2Sensitive"); }

    ClassDef(PndStt2, 1);
};

#endif   // PNDSTT2_H
