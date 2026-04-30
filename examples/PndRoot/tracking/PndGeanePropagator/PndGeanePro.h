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

#ifndef PNDGEANEPRO_H
#define PNDGEANEPRO_H

#include "FairLogger.h"
#include "FairPropagator.h"
#include "TGeant3.h"
#include "TString.h"
#include "TVector3.h"
#include "TDatabasePDG.h"
#include "FairGeaneApplication.h"
#include "PndPropagator.h"
#include "PndGeaneGeometryMethods.h"

/**
 * @class PndGeanePro
 * @brief Interface to GEANE for PandaRoot.
 */
class PndGeanePro : public PndPropagator
{
 private:
  Float_t fDestinationLengthArray[1];   ///< track length when PropagateToTrack is chosen
  Float_t fInErrorMatrix[15];           ///< input error matrix
  Double_t fTransportMatrix[5][5];      ///< transport matrix
  Float_t fInitialPlane[6];             ///< initial plane
  Float_t fInitialPositionArray[3];     ///< initial position vector
  Float_t fInitialMomentumArray[3];     ///< initial momentum vector
  Float_t fFinalPlane[12];              ///< final plane
  Float_t fFinalPositionArray[3];       ///< final position vector
  Float_t fFinalMomentumArray[3];       ///< final momentum vector
  
  TGeant3* gMC3;                        ///< Geant3 interface
  Ertrio_t* afErtrio;                   ///< Geane output structure
  TDatabasePDG* fDBpdg;                 ///< PDG database

  TString fPropOption;                  ///< Geane propagation option
  Int_t fNrep;                          ///< Number of predictions
  Int_t fGeantCode;                     ///< Geant code of particle
  Int_t fProMode;                       ///< if 1: propagate to volume, if 3: propagate to plane
  Float_t fTrackTime;                   ///< time of track

  PndGeaneGeometryMethods fGeoMethods;  ///< helper class containing some purely geometric methods
  
  PndGeanePro(const PndGeanePro&);      ///< copy constructor
  PndGeanePro& operator=(const PndGeanePro&);

 public:
  PndGeanePro();  ///< default constructor
  ~PndGeanePro(); ///< destructor

  /**
   * @brief Method to initialize last track parameter of reconstructed track.
   * @details Position and momentum of particle at last point of track are copied
   *          to arrays that GEANE can use.
   */   
  void Init(FairTrackPar* TParam);
  
  /**
   * @brief Check initial parameters
   * @details Check for a NaN in initial parameters leading to a crash of TGeant3
   */
  Bool_t CheckIniParOk();
  
  /**
   * @brief Main propagate function that calls ERTRAK from GEANE to do the propagation.
   * @details Prior to calling this function, the GEANE must be set up properly.
   *          This is done by the Propagate(FairTrackPar*, FairTrackPar*) and
   *          Propagate(Float_t*, Float_t*, Float_t*, Float_t*, Int_t) functions.
   * @param Int_t pdg code of the particle to propagate
   * @return kTRUE if propagation successfull
   */
  Bool_t Propagate(Int_t PDG);
  
  /**
   * @brief Propagate function called by user.
   * @details Propagates track from start to end point and returns end point and momentum.
   *          Before calling Propagate, the destination of propagation must be set using
   *          - SetDestinationPlane,
   *          - SetDestinationVolume,
   *          - SetDestinationLenth,
   *          - SetDestinationPoint, or
   *          - SetDestinatioWire.
   *          In addition, the PCA propagation mode and direction must be set using SetPCAPropagation.
   * @param TStart start track parameters - usually the momentum and position at the last point of the reconstructed track
   * @param TEnd the return track parameters at the end of propagation
   * @param PDG the pdg code of the particle to propagate
   */
  virtual Bool_t Propagate(FairTrackPar* TStart, FairTrackPar* TEnd, int PDG);

  /**
   * @brief Propagate function called by user.
   * @details This propagate function takes in arrays with start position and momentum.
   *          Before calling this function, GEANE needs to be set up and the destination length needs to be set.
   */
  virtual Bool_t Propagate(Float_t* x1, Float_t* p1, Float_t* x2, Float_t* p2, Int_t PDG);

  /**
   * @brief Find the point of closest approach of the track to a point (measured position) or to a line (wire).
   * @param pca = 1 closest approach to point
   *            = 2 closest approach to wire
   *            = 0 no closest approach
   * @param PDGCode pdg code of particle
   * @param point point with respect to which pca is calculated
   * @param wire1 first anchor point of line with respect to which pca is calculated
   * @param wire2 second anchor point of line with respect to which pca is calculated
   * @param maxdistance = geometrical distance[start - point/wire extr] * 2
   * @return PCAOutputStruct containing:
   *         - PCAStatusFlag: if 0: success, else: failure
   *         - Radius: radius if the found circle
   *         - OnTrackPCA: point of closest approach on track
   *         - OnWirePCA: point of closest approach on wire
   *         - Distance: distance between track and wire in the PCA
   *         - TrackLength: track length to add to the GEANE one
   */
  virtual PndProp::PCAOutputStruct FindPCA(Int_t pca, Int_t PDGCode, TVector3 point, TVector3 wire1,
                                           TVector3 wire2, Double_t maxdistance);

  /**
   * @brief Method to set the plane to propagate particles to.
   * @param v0 first plane defining vectors
   * @param v1 second plane defining vectors
   * @param v2 third plane defining vectors
  */
  virtual Bool_t SetDestinationPlane(const TVector3& v0, const TVector3& v1, const TVector3& v2);

  /**
   * @brief Method to set the plane to propagate particles from.
   * @param v0 first plane defining vectors
   * @param v1 second plane defining vectors
  */
  virtual Bool_t SetOriginPlane(const TVector3& v0, const TVector3& v1);

  /**
   * @brief Method to set the volume to propagate particles to.
   * @param volName volume name
   * @param copyNo copy number
   * @param option additional options
  */
  virtual Bool_t SetDestinationVolume(std::string volName, Int_t copyNo, Int_t option);

  /**
   * @brief Method to set the length to propagate particles to
   * @param length track length
   */
  virtual Bool_t SetDestinationLength(Float_t length);

  /**
   * @brief Method to set point to propagate to.
   * @param point point to propagate to
   */
  virtual Bool_t SetDestinationPoint(TVector3 point);

  /**
   * @brief Method to set wire to propagate to.
   * @param wire1 first anchor point of wire to propagate to
   * @param wire2 second anchor point of wire to propagate to
   */
  virtual Bool_t SetDestinationWire(TVector3 wire1, TVector3 wire2);

  /**
   * @brief Method to set to propagate only parameters.
   */
  virtual Bool_t SetPropagateOnlyParameters();

  /**
   * @brief Method to set PCA mode, propagation direction and other propagation options,
   *        initializes track parameter and error matrix.
   * @param pca PCA mode = 1 closest approach to point
   *                     = 2 closest approach to wire
   *                     = 0 no closest approach
   * @param dir propagation direction = 1 forwards
   *                                  = -1 backwards
   * @param par track parameter at end of reconstructed track
   * @return kTRUE if successfull
   */
  virtual bool SetPCAPropagation(Int_t pca, Int_t dir = 1, FairTrackPar* par = nullptr);

  /**
   * @brief Method to set propagation to backwards propagation.
   */ 
  void SetBackProp() { fPropOption = "BPE"; }
  
  /**
   * @return Returns propagated track length at point of closest approach.
   */
  Float_t GetLengthAtPCA() { return fPcaOutput.TrackLength; }
  
  /**
   * @brief Method to retrieve the transport matrix.
   * @param trm Matrix in which the values of the transport matrix are stored.
   */
  void GetTransportMatrix(Double_t trm[5][5])
  {
    for (Int_t i = 0; i < 5; i++){
      for (Int_t j = 0; j < 5; j++) {
        trm[i][j] = fTransportMatrix[i][j];
      }
    }
  }

  ClassDef(PndGeanePro, 1);
};

#endif //PNDGEANEPRO_H
