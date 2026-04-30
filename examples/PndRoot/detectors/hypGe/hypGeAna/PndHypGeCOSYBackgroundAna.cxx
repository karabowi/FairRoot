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

/******************************************************

Analysis Task created by M.Steinen steinen@kph.uni-mainz.de
Analysis of Gamma Simulation with hypGe detectors
*******************************************************/

#include "PndHypGeCOSYBackgroundAna.h"
#include <iostream>
#include <TMath.h>
#include "PndHypGePoint.h"
#include "PndMCTrack.h"
#include <time.h>
#include <TCanvas.h>
#include <TSpectrum.h>
#include <TStyle.h>

//#include "RhoBase/TFactory.h"

using namespace std;

PndHypGeCOSYBackgroundAna::PndHypGeCOSYBackgroundAna() : FairTask("Panda HypGeGammaAna	 Task") {}
PndHypGeCOSYBackgroundAna::PndHypGeCOSYBackgroundAna(Int_t nEvents = 0) : FairTask("Panda HypGeGammaAna	 Task")
{
  NumberOfEvents = nEvents;
}

PndHypGeCOSYBackgroundAna::~PndHypGeCOSYBackgroundAna()
{
  delete hNHits;
  delete hCrystalHit;
  delete hNeutronOrigin;
  delete hCrystalOrigin;
}

InitStatus PndHypGeCOSYBackgroundAna::Init()
{
  cout << "HypGe COSYBackgroundSim Ana:\t"
       << "Starting Init of HypGe Gamma Ana" << endl;

  //----------get FairRootManager-------------------------------------
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndEmcHitProducer::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  if (fVerbose == 3)
    cout << "HypGe COSYBackgroundSim Ana:\t"
         << "Getting FairRootManager finished" << endl;

  //----------get input arrays---------------------------------------------
  fMcTr = (TClonesArray *)ioman->GetObject("MCTrack");
  // fHyp = (TClonesArray*) ioman->GetObject("HypPoint");
  fHypGe = (TClonesArray *)ioman->GetObject("HypGePoint");

  if (fVerbose == 3) {
    cout << "HypGe COSYBackgroundSim Ana:\t"
         << "Getting input arrays finished" << endl;
  }
  //-----------extracting file name--------------------------------------
  fName = ioman->GetInFile()->GetName();
  if (fVerbose == 3) {
    cout << "HypGe COSYBackgroundSim Ana:\t"
         << "Full input file name with path: " << fName << endl;
    cout << "HypGe COSYBackgroundSim Ana:\t"
         << "Chopping first " << fName.Last('/') + 1 << " chars" << endl;
  }
  fName.Remove(0, fName.Last('/') + 1);
  if (fVerbose == 3)
    cout << "HypGe COSYBackgroundSim Ana:\t"
         << "File name: " << fName << endl;

  if (fVerbose == 3)
    cout << "HypGe COSYBackgroundSim Ana:\t"
         << "File name extraction finished" << endl;

  //----------Get GeoManager---------------------------------------------

  fgeom = (TGeoManager *)gROOT->FindObject("FAIRGeom");
  if (fVerbose == 3)
    cout << "HypGe COSYBackgroundSim Ana:\t"
         << "Found GeoManager" << endl;
  //----------create histograms------------------------------------------

  // histogram with polar angle of detector interactions of primary neutrons
  hNHits = new TH1D("hNHits", "Polar angle of primary neutrons interacting with the crystals", 180, 90, 180);
  hNHits->SetXTitle("#Theta [#circ]");
  hNHits->SetYTitle("Counts / 0.5 #circ");

  // histogram to see which detector is hit
  hCrystalHit = new TH1D("hCrystalHit", "Hits per Crystal", 2, 0, 2);
  hCrystalHit->SetXTitle("Crystal number");
  hCrystalHit->SetYTitle("Counts per crystal");
  hCrystalHit->GetXaxis()->SetBinLabel(1, "Crystal 30 cm"); // Crystal_101
  hCrystalHit->GetXaxis()->SetBinLabel(2, "Crystal 90 cm"); // Crystal_202

  // histogram to see where neutron is coming from
  hNeutronOrigin = new TH1D("hNeutronOrigin", "Origin of Neutrons", 5, 0, 5);
  hNeutronOrigin->SetXTitle("Neutron Origin");
  hNeutronOrigin->SetYTitle("Counts");
  hNeutronOrigin->GetXaxis()->SetBinLabel(1, "Target");
  hNeutronOrigin->GetXaxis()->SetBinLabel(2, "Beam dump");
  hNeutronOrigin->GetXaxis()->SetBinLabel(3, "Cave");
  hNeutronOrigin->GetXaxis()->SetBinLabel(4, "Capsule");
  hNeutronOrigin->GetXaxis()->SetBinLabel(5, "Crystal");

  // histogram for correlation of neutron origin and crystal
  hCrystalOrigin = new TH2D("hCrystalOrigin", "Crystal -Neutron Origin - Correlation", 2, 0, 2, 5, 0, 5);
  hCrystalOrigin->SetXTitle("Crystal number");
  hCrystalOrigin->SetYTitle("Neutron Origin");
  hCrystalOrigin->GetXaxis()->SetBinLabel(1, "Crystal 30 cm"); // Crystal_101
  hCrystalOrigin->GetXaxis()->SetBinLabel(2, "Crystal 90 cm"); // Crystal_202
  hCrystalOrigin->GetYaxis()->SetBinLabel(1, "Target");
  hCrystalOrigin->GetYaxis()->SetBinLabel(2, "Beam dump");
  hCrystalOrigin->GetYaxis()->SetBinLabel(3, "Cave");
  hCrystalOrigin->GetYaxis()->SetBinLabel(4, "Capsule");
  hCrystalOrigin->GetYaxis()->SetBinLabel(5, "Crystal");

  // histogram for kin. energy of neutrons
  hNeutronEkin = new TH1D("hNeutronEkin", "E_{kin} of neutrons;E_{kin} of neutrons [MeV]; Counts", 200, 0, 20);

  // histogram for correlation of E_kin and neutron origin
  hNeutronEkinOrigin = new TH2D("hNeutronEkinOrigin", "E_{kin} -Neutron Origin - Correlation;E_{kin} of neutrons [MeV]; Neutron Origin", 200, 0, 20, 5, 0, 5);

  hNeutronEkinOrigin->GetYaxis()->SetBinLabel(1, "Target");
  hNeutronEkinOrigin->GetYaxis()->SetBinLabel(2, "Beam dump");
  hNeutronEkinOrigin->GetYaxis()->SetBinLabel(3, "Cave");
  hNeutronEkinOrigin->GetYaxis()->SetBinLabel(4, "Capsule");
  hNeutronEkinOrigin->GetYaxis()->SetBinLabel(5, "Crystal");

  // histogram with energy deposited by neutrons in detector
  hNeutronEnergyLoss = new TH1D("hNeutronEnergyLoss", "Energy loss of neutrons inside a crystal;Energy loss of neutrons [MeV]; Counts", 2000, 0, 2);

  // histogram with all particles in both crystals created outside of the detector
  hAllParticlesGe = new TH1D("hAllParticlesGe", "Particles interaction in the crystals;PDG Code of particle; Counts", 2e4, -1e4, 1e4);
  // histogram with all particles in first crystal created outside of the detector
  hAllParticlesCrystal1 = new TH1D("hAllParticlesCrystal1", "Particles interaction in the first crystal (30 cm);PDG Code of particle; Counts", 5000, -2500, 2500);
  // histogram with all particles in second crystal created outside of the detector
  hAllParticlesCrystal2 = new TH1D("hAllParticlesCrystal2", "Particles interaction in the second crystal (90 cm);PDG Code of particle; Counts", 5000, -2500, 2500);
  // histogram with all particles in piezo created outside of the crystal
  hAllParticlesPiezo = new TH1D("hAllParticlesPiezo", "Particles interaction in the piezo;PDG Code of particle; Counts", 2e4, -1e4, 1e4);
  // histogram with all particles in first Sipm created outside of the detector
  hAllParticlesSiPm1 = new TH1D("hAllParticlesSiPm1", "Particles interaction in the first SiPm (60 cm);PDG Code of particle; Counts", 5000, -2500, 2500);
  // histogram with all particles in second Sipm created outside of the detector
  hAllParticlesSiPm2 = new TH1D("hAllParticlesSiPm2", "Particles interaction in the second SiPm (30 cm);PDG Code of particle; Counts", 5000, -2500, 2500);

  // histogram for correlation of E_kin and particle PDG Code for all detectors
  hEkinAllParticles = new TH2D("hEkinAllParticles", "E_{kin} - Particle - Correlation;E_{kin} of particles [MeV]; particle", 200, 0, 200, 9, 1, 10);
  hEkinAllParticles->GetYaxis()->SetBinLabel(1, "#pi^{-}");
  hEkinAllParticles->GetYaxis()->SetBinLabel(2, "#mu^{+}");
  hEkinAllParticles->GetYaxis()->SetBinLabel(3, "e^{+}");
  hEkinAllParticles->GetYaxis()->SetBinLabel(4, "e^{-}");
  hEkinAllParticles->GetYaxis()->SetBinLabel(5, "#mu^{-}");
  hEkinAllParticles->GetYaxis()->SetBinLabel(6, "#gamma");
  hEkinAllParticles->GetYaxis()->SetBinLabel(7, "#pi^{+}");
  hEkinAllParticles->GetYaxis()->SetBinLabel(8, "neutron");
  hEkinAllParticles->GetYaxis()->SetBinLabel(9, "proton");

  // histogram for correlation of E_kin and particle PDG Code for crystal1 (30 cm)
  hEkinAllParticlesCrystal1 = new TH2D("hEkinAllParticlesCrystal1", "E_{kin} - Particle - Correlation (crystal1);E_{kin} of particles [MeV]; particle", 200, 0, 200, 9, 1, 10);
  hEkinAllParticlesCrystal1->GetYaxis()->SetBinLabel(1, "#pi^{-}");
  hEkinAllParticlesCrystal1->GetYaxis()->SetBinLabel(2, "#mu^{+}");
  hEkinAllParticlesCrystal1->GetYaxis()->SetBinLabel(3, "e^{+}");
  hEkinAllParticlesCrystal1->GetYaxis()->SetBinLabel(4, "e^{-}");
  hEkinAllParticlesCrystal1->GetYaxis()->SetBinLabel(5, "#mu^{-}");
  hEkinAllParticlesCrystal1->GetYaxis()->SetBinLabel(6, "#gamma");
  hEkinAllParticlesCrystal1->GetYaxis()->SetBinLabel(7, "#pi^{+}");
  hEkinAllParticlesCrystal1->GetYaxis()->SetBinLabel(8, "neutron");
  hEkinAllParticlesCrystal1->GetYaxis()->SetBinLabel(9, "proton");

  // histogram for correlation of E_kin and particle PDG Code for crystal2 (90 cm)
  hEkinAllParticlesCrystal2 = new TH2D("hEkinAllParticlesCrystal2", "E_{kin} - Particle - Correlation (Crystal2);E_{kin} of particles [MeV]; particle", 200, 0, 200, 9, 1, 10);
  hEkinAllParticlesCrystal2->GetYaxis()->SetBinLabel(1, "#pi^{-}");
  hEkinAllParticlesCrystal2->GetYaxis()->SetBinLabel(2, "#mu^{+}");
  hEkinAllParticlesCrystal2->GetYaxis()->SetBinLabel(3, "e^{+}");
  hEkinAllParticlesCrystal2->GetYaxis()->SetBinLabel(4, "e^{-}");
  hEkinAllParticlesCrystal2->GetYaxis()->SetBinLabel(5, "#mu^{-}");
  hEkinAllParticlesCrystal2->GetYaxis()->SetBinLabel(6, "#gamma");
  hEkinAllParticlesCrystal2->GetYaxis()->SetBinLabel(7, "#pi^{+}");
  hEkinAllParticlesCrystal2->GetYaxis()->SetBinLabel(8, "neutron");
  hEkinAllParticlesCrystal2->GetYaxis()->SetBinLabel(9, "proton");

  // histogram for correlation of E_kin and particle PDG Code for piezo
  hEkinAllParticlesPiezo = new TH2D("hEkinAllParticlesPiezo", "E_{kin} - Particle - Correlation (Piezo);E_{kin} of particles [MeV]; particle", 200, 0, 200, 9, 1, 10);
  hEkinAllParticlesPiezo->GetYaxis()->SetBinLabel(1, "#pi^{-}");
  hEkinAllParticlesPiezo->GetYaxis()->SetBinLabel(2, "#mu^{+}");
  hEkinAllParticlesPiezo->GetYaxis()->SetBinLabel(3, "e^{+}");
  hEkinAllParticlesPiezo->GetYaxis()->SetBinLabel(4, "e^{-}");
  hEkinAllParticlesPiezo->GetYaxis()->SetBinLabel(5, "#mu^{-}");
  hEkinAllParticlesPiezo->GetYaxis()->SetBinLabel(6, "#gamma");
  hEkinAllParticlesPiezo->GetYaxis()->SetBinLabel(7, "#pi^{+}");
  hEkinAllParticlesPiezo->GetYaxis()->SetBinLabel(8, "neutron");
  hEkinAllParticlesPiezo->GetYaxis()->SetBinLabel(9, "proton");

  // histogram for correlation of E_kin and particle PDG Code for SiPm1 (60 cm)
  hEkinAllParticlesSiPm1 = new TH2D("hEkinAllParticlesSiPm1", "E_{kin} - Particle - Correlation (SiPm1);E_{kin} of particles [MeV]; particle", 200, 0, 200, 9, 1, 10);
  hEkinAllParticlesSiPm1->GetYaxis()->SetBinLabel(1, "#pi^{-}");
  hEkinAllParticlesSiPm1->GetYaxis()->SetBinLabel(2, "#mu^{+}");
  hEkinAllParticlesSiPm1->GetYaxis()->SetBinLabel(3, "e^{+}");
  hEkinAllParticlesSiPm1->GetYaxis()->SetBinLabel(4, "e^{-}");
  hEkinAllParticlesSiPm1->GetYaxis()->SetBinLabel(5, "#mu^{-}");
  hEkinAllParticlesSiPm1->GetYaxis()->SetBinLabel(6, "#gamma");
  hEkinAllParticlesSiPm1->GetYaxis()->SetBinLabel(7, "#pi^{+}");
  hEkinAllParticlesSiPm1->GetYaxis()->SetBinLabel(8, "neutron");
  hEkinAllParticlesSiPm1->GetYaxis()->SetBinLabel(9, "proton");

  // histogram for correlation of E_kin and particle PDG Code for SiPm2 (30 cm)
  hEkinAllParticlesSiPm2 = new TH2D("hEkinAllParticlesSiPm2", "E_{kin} - Particle - Correlation (SiPm2);E_{kin} of particles [MeV]; particle", 200, 0, 200, 9, 1, 10);
  hEkinAllParticlesSiPm2->GetYaxis()->SetBinLabel(1, "#pi^{-}");
  hEkinAllParticlesSiPm2->GetYaxis()->SetBinLabel(2, "#mu^{+}");
  hEkinAllParticlesSiPm2->GetYaxis()->SetBinLabel(3, "e^{+}");
  hEkinAllParticlesSiPm2->GetYaxis()->SetBinLabel(4, "e^{-}");
  hEkinAllParticlesSiPm2->GetYaxis()->SetBinLabel(5, "#mu^{-}");
  hEkinAllParticlesSiPm2->GetYaxis()->SetBinLabel(6, "#gamma");
  hEkinAllParticlesSiPm2->GetYaxis()->SetBinLabel(7, "#pi^{+}");
  hEkinAllParticlesSiPm2->GetYaxis()->SetBinLabel(8, "neutron");
  hEkinAllParticlesSiPm2->GetYaxis()->SetBinLabel(9, "proton");

  // histogram for correlation of E_kin and gamma origin
  hGammaEkinOrigin = new TH2D("hGammaEkinOrigin", "E_{kin} -#gamma Origin - Correlation;E_{kin} of neutrons [MeV]; Gamma Origin", 200, 0, 20, 5, 0, 5);
  hGammaEkinOrigin->GetYaxis()->SetBinLabel(1, "Target");
  hGammaEkinOrigin->GetYaxis()->SetBinLabel(2, "Beam dump");
  hGammaEkinOrigin->GetYaxis()->SetBinLabel(3, "Cave");
  hGammaEkinOrigin->GetYaxis()->SetBinLabel(4, "Capsule");
  hGammaEkinOrigin->GetYaxis()->SetBinLabel(5, "Crystal");

  /// FILL HERE

  if (fVerbose == 3)
    cout << "HypGe COSYBackgroundSim Ana:\t"
         << "Histogram creation finished" << endl;

  //----------initialize EvtCount----------------------------------------
  EvtCount = 0;

  //----------Get number of events from FairRootManager-----------------------

  if (NumberOfEvents == 0)
    NumberOfEvents = ioman->GetInChain()->GetEntries();
  cout << "HypGe COSYBackgroundSim Ana:\tNumber of Events to analyze: " << NumberOfEvents << endl;

  cout << "HypGe COSYBackgroundSim Ana:\t"
       << "Init of HypGe COSYBackgroundSim Ana finished succesfully" << endl;
}

void PndHypGeCOSYBackgroundAna::Exec(Option_t *)
{
  Double_t NeutronEnergyLossArray[10];
  for (Int_t i = 0; i < 10; i++)
    NeutronEnergyLossArray[i] = 0;
  Int_t nNeutrons = -1;
  Int_t NeutronTrackID = -1;
  for (Int_t i = 0; i < fHypGe->GetEntriesFast(); i++) {
    PndHypGePoint *hitgam = (PndHypGePoint *)fHypGe->At(i);
    PndMCTrack *mcgam = (PndMCTrack *)fMcTr->At(hitgam->GetTrackID());
    // only Neutrons
    StartVertex = mcgam->GetStartVertex();
    VertexVolumeName = fgeom->FindNode(StartVertex.X(), StartVertex.Y(), StartVertex.Z())->GetName();
    if (hitgam->GetpdgCode() == 2112) {

      // cout <<"Event " <<EvtCount<< " \t\tStartVertex Radius: " <<StartVertex.Mag()<<endl;

      // Fill hNeutronOrigin

      // cout << VertexVolumeName << endl;

      if (hitgam->GetDetectorID() == 101) {
        hCrystalHit->Fill("Crystal 30 cm", 1);
        if (VertexVolumeName.Contains("Target")) {
          hNeutronOrigin->Fill("Target", 1);
          hCrystalOrigin->Fill("Crystal 30 cm", "Target", 1);
        } else {
          if (VertexVolumeName.Contains("BeamDump")) {
            hNeutronOrigin->Fill("Beam dump", 1);
            hCrystalOrigin->Fill("Crystal 30 cm", "Beam dump", 1);
          } else {
            if (VertexVolumeName.Contains("cave")) {
              hNeutronOrigin->Fill("Cave", 1);
              hCrystalOrigin->Fill("Crystal 30 cm", "Cave", 1);
            } else {
              if (VertexVolumeName.Contains("Capsule")) {
                hNeutronOrigin->Fill("Capsule", 1);
                hCrystalOrigin->Fill("Crystal 30 cm", "Capsule", 1);
              } else {
                if (VertexVolumeName.Contains("Crystal")) {
                  hNeutronOrigin->Fill("Crystal", 1);
                  hCrystalOrigin->Fill("Crystal 30 cm", "Crystal", 1);
                }
              }
            }
          }
        }
      } else if (hitgam->GetDetectorID() == 202) {
        hCrystalHit->Fill("Crystal 90 cm", 1);
        if (VertexVolumeName.Contains("Target")) {
          hNeutronOrigin->Fill("Target", 1);
          hCrystalOrigin->Fill("Crystal 90 cm", "Target", 1);
        } else {
          if (VertexVolumeName.Contains("BeamDump")) {
            hNeutronOrigin->Fill("Beam dump", 1);
            hCrystalOrigin->Fill("Crystal 90 cm", "Beam dump", 1);
          } else {
            if (VertexVolumeName.Contains("cave")) {
              hNeutronOrigin->Fill("Cave", 1);
              hCrystalOrigin->Fill("Crystal 90 cm", "Cave", 1);
            } else {
              if (VertexVolumeName.Contains("Capsule")) {
                hNeutronOrigin->Fill("Capsule", 1);
                hCrystalOrigin->Fill("Crystal 90 cm", "Capsule", 1);
              } else {
                if (VertexVolumeName.Contains("Crystal")) {
                  hNeutronOrigin->Fill("Crystal", 1);
                  hCrystalOrigin->Fill("Crystal 90 cm", "Crystal", 1);
                }
              }
            }
          }
        }
      }

      // E_kin stuff
      NeutronMomentum.SetX(hitgam->GetPx());
      NeutronMomentum.SetY(hitgam->GetPy());
      NeutronMomentum.SetZ(hitgam->GetPz());
      NeutronEkin = 1. / 2. * NeutronMomentum.Mag2() / 0.939565 * 1000;
      // cout << NeutronEkin << "\t\t\t" << NeutronMomentum.Mag2()<<endl;
      if (!VertexVolumeName.Contains("Crystal") && !VertexVolumeName.Contains("Capsule")) // created outside of the detector
      {
        hNeutronEkin->Fill(NeutronEkin);
        if (NeutronTrackID != hitgam->GetTrackID()) // new neutron (different TrackID)
        {
          NeutronTrackID = hitgam->GetTrackID();
          nNeutrons++;
        }
        NeutronEnergyLossArray[nNeutrons] += hitgam->GetEnergyLoss() * 1000;
        // cout << hitgam->GetEnergyLoss()*1000<< endl;
      }
      // fill E_kin - Neutron origin correlation
      if (VertexVolumeName.Contains("Target")) {
        hNeutronEkinOrigin->Fill(NeutronEkin, "Target", 1);
      } else {
        if (VertexVolumeName.Contains("BeamDump")) {
          hNeutronEkinOrigin->Fill(NeutronEkin, "Beam dump", 1);
        } else {
          if (VertexVolumeName.Contains("cave")) {
            hNeutronEkinOrigin->Fill(NeutronEkin, "Cave", 1);
          } else {
            if (VertexVolumeName.Contains("Capsule")) {
              hNeutronEkinOrigin->Fill(NeutronEkin, "Capsule", 1);
            } else {
              if (VertexVolumeName.Contains("Crystal")) {
                hNeutronEkinOrigin->Fill(NeutronEkin, "Crystal", 1);
              }
            }
          }
        }
      }
    } // end of only neutrons

    // all particles in Crystal
    if (!VertexVolumeName.Contains("Crystal") && !VertexVolumeName.Contains("Capsule")) // created outside of the detector
    {
      if (hitgam->GetDetectorID() == 101 || hitgam->GetDetectorID() == 102)
        hAllParticlesGe->Fill(hitgam->GetpdgCode());
      if (hitgam->GetDetectorID() == 101)
        hAllParticlesCrystal1->Fill(hitgam->GetpdgCode());
      if (hitgam->GetDetectorID() == 202)
        hAllParticlesCrystal2->Fill(hitgam->GetpdgCode());
      if (hitgam->GetDetectorID() == 1000)
        hAllParticlesPiezo->Fill(hitgam->GetpdgCode());
      if (hitgam->GetDetectorID() == 1001)
        hAllParticlesSiPm1->Fill(hitgam->GetpdgCode());
      if (hitgam->GetDetectorID() == 1002)
        hAllParticlesSiPm2->Fill(hitgam->GetpdgCode());
      ParticleMomentum.SetX(hitgam->GetPx());
      ParticleMomentum.SetY(hitgam->GetPy());
      ParticleMomentum.SetZ(hitgam->GetPz());
      Double_t ParticleMass = 0;
      TString ParticleName = "";
      switch ((int)hitgam->GetpdgCode()) {
      case -211:
        ParticleMass = 0.13957;
        ParticleName = "#pi^{-}";
        break; // pi-
      case -13:
        ParticleMass = 0.10561;
        ParticleName = "#mu^{+}";
        break; // mu+
      case -11:
        ParticleMass = 0.000511;
        ParticleName = "e^{+}";
        break; // e+
      case 11:
        ParticleMass = 0.000511;
        ParticleName = "e^{-}";
        break; // e-
      case 13:
        ParticleMass = 0.10561;
        ParticleName = "#mu^{-}";
        break; // mu-
      case 22:
        ParticleMass = 0;
        ParticleName = "#gamma";
        break; // gamma
      case 211:
        ParticleMass = 0.13957;
        ParticleName = "#pi^{+}";
        break; // pi+
      case 2112:
        ParticleMass = 0.939565;
        ParticleName = "neutron";
        break; // n
      case 2212:
        ParticleMass = 0.93827;
        ParticleName = "proton";
        break; // p
      default:
        ParticleMass = -1;
        std::cout.precision(10);
        cout << "Found new particle:\t" << hitgam->GetpdgCode() << "\t" << VertexVolumeName << endl;
        break;
      }
      if (ParticleMass != -1) {
        ParticleEkin = (sqrt(ParticleMass * ParticleMass + ParticleMomentum.Mag2()) - ParticleMass) * 1000; // Ekin in MeV
        // all particles (but gammas) in Crystal - E_{kin} - correlation
        if (!VertexVolumeName.Contains("Crystal") && !VertexVolumeName.Contains("Capsule")) // created outside of the detector
        {
          hEkinAllParticles->Fill(ParticleEkin, ParticleName.Data(), 1);
          if (hitgam->GetDetectorID() == 101)
            hEkinAllParticlesCrystal1->Fill(ParticleEkin, ParticleName.Data(), 1);
          if (hitgam->GetDetectorID() == 202)
            hEkinAllParticlesCrystal2->Fill(ParticleEkin, ParticleName.Data(), 1);
          if (hitgam->GetDetectorID() == 1000)
            hEkinAllParticlesPiezo->Fill(ParticleEkin, ParticleName.Data(), 1);
          if (hitgam->GetDetectorID() == 1001)
            hEkinAllParticlesSiPm1->Fill(ParticleEkin, ParticleName.Data(), 1);
          if (hitgam->GetDetectorID() == 1002)
            hEkinAllParticlesSiPm2->Fill(ParticleEkin, ParticleName.Data(), 1);
        }
      }

      // gammas only
      if (hitgam->GetpdgCode() == 22) {
        if (VertexVolumeName.Contains("Target")) {
          hGammaEkinOrigin->Fill(ParticleMomentum.Mag(), "Target", 1);
        } else {
          if (VertexVolumeName.Contains("BeamDump")) {
            hGammaEkinOrigin->Fill(ParticleMomentum.Mag(), "Beam dump", 1);
          } else {
            if (VertexVolumeName.Contains("cave")) {
              hGammaEkinOrigin->Fill(ParticleMomentum.Mag(), "Cave", 1);
            } else {
              if (VertexVolumeName.Contains("Capsule")) {
                hGammaEkinOrigin->Fill(ParticleMomentum.Mag(), "Capsule", 1);
              } else {
                if (VertexVolumeName.Contains("Crystal")) {
                  hGammaEkinOrigin->Fill(ParticleMomentum.Mag(), "Crystal", 1);
                }
              }
            }
          }
        }
      }
    }

  } // end of for loop

  // fill Neutron Engergy loss histogram
  if (nNeutrons > -1)
    cout << nNeutrons + 1 << endl;
  for (Int_t i = 0; i <= nNeutrons; i++) {
    cout << i << "\t\t" << NeutronEnergyLossArray[i] << endl;
    hNeutronEnergyLoss->Fill(NeutronEnergyLossArray[i]);
  }

  EvtCount++;
  if (!((EvtCount * 100) % NumberOfEvents)) // mark every % of proceeded events
  {
    cout << "HypGe COSYBackgroundSim Ana:\t" << EvtCount * 100 / NumberOfEvents << " % done" << endl;
  }
  // cout << "HypGe COSYBackgroundSim Ana:\t" << "Event Number"<< EvtCount << endl;
}

void PndHypGeCOSYBackgroundAna::Finish()
{
  cNHits = new TCanvas("cNHits", "Neutron Hits", 800, 600);
  cCrystalHit = new TCanvas("cCrystalHit", "Crystal hit", 800, 600);
  cNeutronOrigin = new TCanvas("cNeutronOrigin", "Origin of Neutrons", 800, 600);
  cCrystalOrigin = new TCanvas("cCrystalOrigin", "Correlation of Crystal hit and neutron origin", 800, 600);
  cNeutronEkin = new TCanvas("cNeutronEkin", "E_{kin} of neutrons", 800, 600);
  cNeutronEkinOrigin = new TCanvas("cNeutronEkinOrigin", "E_{kin} - Neutron Origin - Correlation", 800, 600);
  cNeutronEnergyLoss = new TCanvas("cNeutronEnergyLossn", "Energy loss of neutrons inside a crystal", 800, 600);

  cAllParticlesGe = new TCanvas("cAllParticles", "All particles interacting in the crystal", 800, 600);
  cAllParticlesCrystal1 = new TCanvas("cAllParticlesCrystal1", "All particles interacting in the first crystal", 800, 600);
  cAllParticlesCrystal2 = new TCanvas("cAllParticlesCrystal2", "All particles interacting in the second crystal", 800, 600);
  cEkinAllParticles = new TCanvas("cEkinAllParticles", "Correlation of E_{kin} of Particles and PDG code", 800, 600);
  cGammaEkinOrigin = new TCanvas("cGammaEkinOrigin", "E_{kin} - #gamma Origin - Correlation", 800, 600);

  cAllParticlesPiezo = new TCanvas("cAllParticlesPiazo", "All particles interacting in the piezo", 800, 600);
  cAllParticlesSiPm1 = new TCanvas("cAllParticlesSiPm1", "All particles interacting in the SiPm1", 800, 600);
  cAllParticlesSiPm2 = new TCanvas("cAllParticlesSiPm2", "All particles interacting in the SiPm2", 800, 600);
  cEkinAllParticlesSplit = new TCanvas("cEkinAllParticlesSplit", "Correlation of E_{kin} of Particles and PDG code", 800, 600);
  cEkinAllParticlesSplit->Divide(3, 2);

  hCrystalHit->Write();
  hNeutronOrigin->Write();
  hCrystalOrigin->Write();
  hNeutronEkin->Write();
  hNeutronEkinOrigin->Write();
  hAllParticlesGe->Write();
  hAllParticlesCrystal1->Write();
  hAllParticlesCrystal2->Write();
  hEkinAllParticles->Write();
  hGammaEkinOrigin->Write();
  hNeutronEnergyLoss->Write();

  hAllParticlesPiezo->Write();
  hAllParticlesSiPm1->Write();
  hAllParticlesSiPm2->Write();

  hEkinAllParticlesCrystal1->Write();
  hEkinAllParticlesCrystal2->Write();
  hEkinAllParticlesPiezo->Write();
  hEkinAllParticlesSiPm1->Write();
  hEkinAllParticlesSiPm2->Write();

  cCrystalHit->cd();
  hCrystalHit->Draw();

  cNeutronOrigin->cd();
  hNeutronOrigin->Draw();

  cCrystalOrigin->cd();
  gStyle->SetPalette(1);
  hCrystalOrigin->Draw("colz");

  cNeutronEkin->cd();
  hNeutronEkin->Draw();

  cNeutronEkinOrigin->cd();
  hNeutronEkinOrigin->Draw("LEGO2");

  cNeutronEnergyLoss->cd();
  hNeutronEnergyLoss->Draw();

  cAllParticlesGe->cd();
  hAllParticlesGe->Draw();

  cAllParticlesCrystal1->cd();
  hAllParticlesCrystal1->Draw();

  cAllParticlesCrystal2->cd();
  hAllParticlesCrystal2->Draw();

  cEkinAllParticles->cd();
  hEkinAllParticles->Draw("colz");

  cGammaEkinOrigin->cd();
  hGammaEkinOrigin->Draw("colz");

  cAllParticlesPiezo->cd();
  hAllParticlesPiezo->Draw();
  cAllParticlesSiPm1->cd();
  hAllParticlesSiPm1->Draw();
  cAllParticlesSiPm2->cd();
  hAllParticlesSiPm2->Draw();

  cEkinAllParticlesSplit->cd(1);
  hEkinAllParticlesCrystal1->Draw("colz");
  cEkinAllParticlesSplit->cd(2);
  hEkinAllParticlesCrystal2->Draw("colz");
  cEkinAllParticlesSplit->cd(3);
  hEkinAllParticlesPiezo->Draw("colz");
  cEkinAllParticlesSplit->cd(4);
  hEkinAllParticlesSiPm1->Draw("colz");
  cEkinAllParticlesSplit->cd(5);
  hEkinAllParticlesSiPm2->Draw("colz");

  for (int i = 0; i < hAllParticlesGe->GetNbinsX(); i++) {
    if (hAllParticlesGe->GetBinContent(i) != 0) {
      cout << "PDG code:\t" << hAllParticlesGe->GetBinCenter(i) - 0.5 << "\tCount:\t" << hAllParticlesGe->GetBinContent(i) << endl;
    }
  }

  cout << "HypGe COSYBackgroundSim Ana:\tAnalysis finished succesfully" << endl;
}

ClassImp(PndHypGeCOSYBackgroundAna)
