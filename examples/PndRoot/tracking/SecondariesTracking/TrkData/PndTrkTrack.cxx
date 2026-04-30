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

//
// PndTrkTrack.cxx
//
//
//
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkTrack.h"
#include "TArc.h"
#include <iostream>

using namespace std;

PndTrkTrack::PndTrkTrack() : fCluster(PndTrkCluster()), fRefHit(nullptr), fCenterX(0), fCenterY(0), fRadius(0), fTanL(0), fZ0(0), fCharge(0), fPhiMin(0), fPhiMax(360) {}

PndTrkTrack::PndTrkTrack(PndTrkCluster *cluster)
  : fCluster(*cluster), fRefHit(nullptr), fCenterX(0), fCenterY(0), fRadius(0), fTanL(0), fZ0(0), fCharge(0), fPhiMin(0), fPhiMax(360)
{
}

PndTrkTrack::PndTrkTrack(PndTrkCluster *cluster, double x, double y, double radius)
  : fCluster(*cluster), fRefHit(nullptr), fCenterX(x), fCenterY(y), fRadius(radius), fTanL(0), fZ0(0), fCharge(0), fPhiMin(0), fPhiMax(360)
{
}

PndTrkTrack::PndTrkTrack(PndTrkHit *hit, PndTrkCluster *cluster, double x, double y, double radius)
  : fCluster(*cluster), fRefHit(hit), fCenterX(x), fCenterY(y), fRadius(radius), fTanL(0), fZ0(0), fCharge(0), fPhiMin(0), fPhiMax(360)
{
}

PndTrkTrack::PndTrkTrack(double x, double y, double radius)
  : fCluster(PndTrkCluster()), fRefHit(nullptr), fCenterX(x), fCenterY(y), fRadius(radius), fTanL(0), fZ0(0), fCharge(0), fPhiMin(0), fPhiMax(360)
{
}

PndTrkTrack::PndTrkTrack(PndTrack *trk) : fCluster(PndTrkCluster()), fRefHit(nullptr), fCenterX(0), fCenterY(0), fRadius(0), fTanL(0), fZ0(0), fCharge(0), fPhiMin(0), fPhiMax(360)
{

  TVector3 momentum = trk->GetParamFirst().GetMomentum();
  TVector3 position = trk->GetParamFirst().GetPosition();

  fCharge = TMath::Sign(1., trk->GetParamFirst().GetQp());

  Double_t pl = trk->GetParamFirst().GetMomentum().Z();
  Double_t pt = trk->GetParamFirst().GetMomentum().Perp();
  fRadius = pt / 0.006;
  fTanL = pl / pt;

  TVector2 radius = momentum.XYvector();
  radius = radius.Unit();
  double rotx = radius.X();
  double roty = radius.Y();
  TVector2 myrad(fCharge * roty, -fCharge * rotx);
  myrad *= fRadius;

  TVector2 center = myrad + position.XYvector();
  fCenterX = center.X();
  fCenterY = center.Y();

  fZ0 = 0; // CHECK

  // ----------------------------------------------
  // cluster? // CHECK

  fPhiMin = ComputePhi(trk->GetParamFirst().GetPosition());
  if (fPhiMin > 180)
    fPhiMin -= 360;

  fPhiMax = ComputePhi(trk->GetParamLast().GetPosition());
  if (fPhiMax > 180)
    fPhiMax -= 360;

  if (fCharge > 0 && fPhiMin < fPhiMax)
    fPhiMin += 360;
  else if (fCharge < 0 && fPhiMin > fPhiMax)
    fPhiMax += 360;
}

PndTrkTrack::PndTrkTrack(const PndTrkTrack &track)
  : TObject(track), fCluster(PndTrkCluster()), fRefHit(nullptr), fCenterX(0), fCenterY(0), fRadius(0), fTanL(0), fZ0(0), fCharge(0), fPhiMin(0), fPhiMax(360)
{
  *this = track;
}

PndTrkTrack::~PndTrkTrack()
{
  delete fRefHit;
}

// operator equals
PndTrkTrack &PndTrkTrack::operator=(const PndTrkTrack &track)
{
  fRefHit = track.fRefHit;
  fCluster = track.fCluster;
  fCenterX = track.fCenterX;
  fCenterY = track.fCenterY;
  fRadius = track.fRadius;
  fTanL = track.fTanL;
  fZ0 = track.fZ0;
  fCharge = track.fCharge;
  fPhiMin = track.fPhiMin;
  fPhiMax = track.fPhiMax;
  return *this;
}

Bool_t PndTrkTrack::operator==(PndTrkTrack track)
{
  int nofhits = GetCluster().GetNofHits();
  return nofhits == track.GetCluster().GetNofHits() && fRadius == track.GetRadius() && fCenterX == track.GetCenter().X() && fCenterY == track.GetCenter().Y(); // CHECK
}

void PndTrkTrack::Clear(Option_t *opt)
{
  delete fRefHit;
  fCluster.Clear(opt);
}

PndTrackCand PndTrkTrack::ConvertToPndTrackCand()
{
  PndTrackCand trkcand;
  for (int ihit = 0; ihit < fCluster.GetNofHits(); ihit++) {
    PndTrkHit *hit = fCluster.GetHit(ihit);
    int detid = hit->GetDetectorID();
    int hitid = hit->GetHitID();
    trkcand.AddHit(detid, hitid, ihit);
  }
  return trkcand;
}

PndTrack PndTrkTrack::ConvertToPndTrack()
{

  ComputeCharge();

  // first
  TVector3 pos1, mom1;
  PndTrkHit *hit1 = fCluster.GetHit(0);
  mom1 = ComputeMomentumAtPosition(hit1->GetPosition(), pos1);

  // last
  TVector3 pos2, mom2;
  PndTrkHit *hit2 = fCluster.GetHit(fCluster.GetNofHits() - 1);
  mom2 = ComputeMomentumAtPosition(hit2->GetPosition(), pos2);

  TVector3 dj(1, 0, 0), dk(0, 0, 1); // CHECK
  TVector3 dpos1(2., 2., 10.);       // CHECK
  TVector3 dpos2(2., 2., 10.);       // CHECK
  TVector3 dmom1(0.2, 0.2, 0.5);     // CHECK
  TVector3 dmom2(0.2, 0.2, 0.5);     // CHECK

  FairTrackParP firstpar(pos1, mom1, dpos1, dmom1, fCharge, pos1, dj, dk);

  FairTrackParP lastpar(pos2, mom2, dpos2, dmom2, fCharge, pos2, dj, dk);

  PndTrackCand trkcand = ConvertToPndTrackCand();
  PndTrack track(firstpar, lastpar, trkcand);
  if (mom1.Z() == -999)
    track.SetFlag(-1);
  return track;
}

// void OrderFrom(TVector3 point) {

//   TVector3 tmpposition = point;
//   double tmpdistance = 1000.;
//   for(int ihit = 0; ihit < fCluster.GetNofHits(); ihit++) {
//     PndTrkHit *hit = fCluster.GetHit(ihit);
//     TVector3 position = hit->GetPosition();
//     double distance = (position - point).Mag();
//     if(distance < tmpdistance) {
//       tmpposition = position;
//       tmpdistance = distance;
//     }
//   }

void PndTrkTrack::ComputeCharge()
{ // CHECK!!!

  /**
   // consider the centers
   Int_t nleft, nright = 0;

   // first
   PndTrkHit *hit1 = fCluster.GetHit(0);
   // last
   PndTrkHit *hit2 = fCluster.GetHit(fCluster->GetNofHits() - 1);

   // vector from 1st to last hit
   TVector3 firsttolast = hit2->GetPosition() - hit1->GetPosition();

   for(int ihit = 1; ihit < fCluster.GetNofHits() - 1; ihit++) {
   PndTrkHit *hit = fCluster.GetHit(ihit);

   // vector from 1st to this hit
   TVector3 position = hit->GetPosition() - hit1->GetPosition();

   double crossz = position.Cross(firsttolast).Z();

   // if  crossz > 0  hits stays 'on the right' (counterclockwise) --> negative
   // otherwise it stays 'on the left' (clockwise) --> positive
   if (crossz > 0.) nright++;
   else nleft++;
   }

   if(nright > nleft) fCharge = -1;
   else fCharge = 1;
  **/
  // consider the poca on track
  Int_t nmore = 0, nless = 0;

  // first
  PndTrkHit *hit1 = (PndTrkHit *)fCluster.GetHit(0);
  PndTrkTools tools;
  TVector3 position1 = tools.ComputePocaToPointOnCircle3(hit1->GetPosition().X(), hit1->GetPosition().Y(), fCenterX, fCenterY, fRadius);

  // phi of first hit with respect to center of curvature
  TVector3 direction1 = position1 - TVector3(fCenterX, fCenterY, 0);
  double tmpphi = direction1.Phi();
  //
  if (direction1.Y())
    tmpphi += 2 * TMath::Pi(); // CHECK

  for (int ihit = 1; ihit < fCluster.GetNofHits(); ihit++) {
    PndTrkHit *hit = fCluster.GetHit(ihit);
    TVector3 position = tools.ComputePocaToPointOnCircle3(hit->GetPosition().X(), hit->GetPosition().Y(), fCenterX, fCenterY, fRadius);

    // vector from 1st to this hit
    TVector3 direction = position - TVector3(fCenterX, fCenterY, 0);
    double phi = direction.Phi();
    if (ihit > 1)
      phi >= tmpphi ? nmore++ : nless++;
    tmpphi = phi;
    //    cout << "phi " << phi * TMath::RadToDeg() << " " << hit->GetHitID() << " " << hit->GetDetectorID() << endl;
  }

  if (nmore > nless)
    fCharge = -1;
  else
    fCharge = 1;

  //   cout << "fCharge " << fCharge << " " << nmore << " " << nless << endl;

  /**
  // first
  //   PndTrkHit *hit1 = (PndTrkHit*) fCluster.GetHit(0);
  TVector3 pos1 = hit1->GetPosition();
  // last
  PndTrkHit *hit2 = (PndTrkHit*) fCluster.GetHit(fCluster.GetNofHits() - 1);
  TVector3 pos2 = hit2->GetPosition();

  double m = (pos2.Y() - pos1.Y())/(pos2.X() - pos1.X());
  double q = pos1.Y() - m + pos1.X();

  TVector3 posbar(0.5 * (pos1.X() + pos2.X()), 0.5 * (pos1.Y() + pos2.Y()));

  double mortho = -1./m;
  double qortho = posbar.Y() - mortho * posbar.X();

  // solve the system
  // (x - xc)**2 + (y - yc)**2 = R**2
  // y = mortho * x + qortho
  // and find xI xII
  double alpha = 1 + mortho * mortho;
  double beta = mortho * (qortho - fCenterY) - fCenterX;
  double gamma = fCenterX * fCenterX + (qortho - fCenterY) * (qortho - fCenterY) - fRadius * fRadius;

  double xI = (-beta + TMath::Sqrt(beta * beta - alpha * gamma))/alpha;
  double yI = xI * mortho + qortho;
  double xII = (-beta - TMath::Sqrt(beta * beta - alpha * gamma))/alpha;
  double yII = xII * mortho + qortho;

  // mean hit
  int mean = (int) (fCluster.GetNofHits() * 0.5);
  PndTrkHit *hit_mean = (PndTrkHit*) fCluster.GetHit(mean);
  double distI = hit_mean->GetXYDistance(TVector3(xI, yI, 0.));
  double distII = hit_mean->GetXYDistance(TVector3(xII, yII, 0.));

  double x, y;
  if(distI < distII) {
   x = xI;
   y = yI;
  }
  else  {
   x = xII;
   y = yII;
  }

  // translation
  x -= pos1.X();
  y -= pos1.Y();

  double delta = (pos2 - pos1).Phi();
  double xrot = x * TMath::Cos(delta) + y * TMath::Sin(delta);
  double yrot = -x * TMath::Sin(delta) + y * TMath::Cos(delta);

  //  cout << "x, y " << xrot << " " << yrot << endl;

  **/
}

TVector3 PndTrkTrack::ComputeMomentumAtPosition(TVector3 position, TVector3 &newposition)
{
  TVector3 momentum(-999, -999, -999);
  newposition = position;
  if (fCharge == 0) {
    // cout << "ComputeMomentumAtPos: CHARGE = 0!" << endl;
    return momentum;
  }

  TVector2 center(fCenterX, fCenterY);
  TVector2 pos = TVector2(position.X(), position.Y());
  TVector2 myrad = center - pos;
  double distance = TMath::Abs(myrad.Mod() - fRadius);
  if (distance > 0.5) {
    // cout << "ComputeMomentumAtPosition: POINT NOT ON THE TRACK " << distance << endl;
    // return momentum;  // CHECK
    // alternative
    // CHECK
    PndTrkTools tools;
    newposition = tools.ComputePocaToPointOnCircle3(pos.X(), pos.Y(), fCenterX, fCenterY, fRadius);
    myrad = center - pos;
    distance = TMath::Abs(myrad.Mod() - fRadius);
    //   if(distance > 0.5) {
    //       cout << "ComputeMomentumAtPosition: AGAIN POINT NOT ON THE TRACK " << distance << endl;
    //       return momentum;
    //     }
  }

  Double_t rotx, roty;
  rotx = -fCharge * myrad.Y();
  roty = fCharge * myrad.X();

  //   cout << "COMPUTE MOMENTUM " << fRadius << endl;
  Double_t pt = 0.006 * fRadius;

  //   cout << "pt " << pt << endl;
  Double_t pl = -999;
  if (fTanL != -999)
    pl = pt * fTanL;
  //   cout << "pl " << pl << " tanl " << fTanL << endl;

  // Double_t ptot = TMath::Sqrt(pt * pt + pl * pl); //[R.K. 01/2017] unused variable

  //   cout << rotx << " " << roty << endl;
  momentum.SetX(rotx); // CHECK magnitude?
  momentum.SetY(roty); // CHECK magnitude?
  momentum.SetZ(0.);   // CHECK magnitude?
  if (momentum.Mag() > 1e-6) {
    //   momentum.Print();
    momentum.SetMag(pt);
    //   momentum.Print();
    momentum.SetZ(pl);
    //  momentum.Print();
  }
  return momentum;
}

Double_t PndTrkTrack::ComputePhi(TVector3 hit)
{
  /**
     TVector3 center(fCenterX, fCenterY, 0.);
     TVector3 fromcentertohit = hit - center;

     // I want the positive phi angle from x axis
     // in range [0, 360[.
     // I use TVector3::Phi() [fromcentertohit.Phi()]:
     // x   y      Phi        use!
     // -------------------------
     // +   +      0/90       phi
     // -   +     90/180      phi
     // -   -   -180/-90    phi + 360
     // +   -    -90/0      phi + 360
     //  cout << "phi " << hit.X() << " " << hit.Y() << endl;

     double phi = fromcentertohit.Phi();
     if(fromcentertohit.Y() < 0) phi += (2 * TMath::Pi());
     // cout << "final phi in rad " << phi << endl;
     return phi * TMath::RadToDeg();
  **/

  // x0 y0
  Double_t d = TMath::Sqrt(fCenterX * fCenterX + fCenterY * fCenterY) - fRadius;
  Double_t phi = TMath::ATan2(fCenterY, fCenterX);

  Double_t x0 = d * TMath::Cos(phi);
  Double_t y0 = d * TMath::Sin(phi);

  Double_t Phi0 = TMath::ATan2((y0 - fCenterY), (x0 - fCenterX));

  // CHECK :-)GOOD! ...
  TVector2 v(x0 - fCenterX, y0 - fCenterY);
  double alpha = TMath::ATan2(hit.Y() - y0 + fRadius * TMath::Sin(Phi0), hit.X() - x0 + fRadius * TMath::Cos(Phi0));
  TVector2 p(hit.X() - fCenterX, hit.Y() - fCenterY);

  Double_t Fi = -fCharge * TMath::ACos(v * p / (v.Mod() * p.Mod()));
  double pi = TMath::Pi();
  double pi2 = 2 * pi;

  // Fi = h * (pi2 - h * Fi) // should be correct
  if ((fCharge > 0 &&
       ((Phi0 > 0 && ((alpha > 0 && alpha > Phi0) || (alpha < 0 && alpha < Phi0 - pi))) || ((Phi0 < 0 && ((alpha > 0 && alpha < pi + Phi0) || (alpha < 0 && alpha > Phi0)))))))
    Fi = -(pi2 + Fi);
  else if ((fCharge < 0 &&
            ((Phi0 > 0 && ((alpha > 0 && alpha < Phi0) || (alpha < 0 && alpha > Phi0 - pi))) || ((Phi0 < 0 && ((alpha > 0 && alpha > pi + Phi0) || (alpha < 0 && alpha < Phi0)))))))
    Fi = pi2 - Fi;

  //  cout << "PHI ----------------- " <<  Fi * TMath::RadToDeg() << endl;

  return (Phi0 + Fi) * TMath::RadToDeg();
}

Double_t PndTrkTrack::ComputePhiFrom(TVector3 hit, TVector3 from)
{
  //  cout << "phi " << hit.X() << " " << hit.Y() << endl;

  TVector3 center(fCenterX, fCenterY, 0.);
  TVector3 fromcentertofrom = from - center;
  double phi0 = fromcentertofrom.Phi();

  double xtr = hit.X() - fCenterX;
  double ytr = hit.Y() - fCenterY;
  double xp = TMath::Cos(phi0) * xtr + TMath::Sin(phi0) * ytr;
  double yp = -TMath::Sin(phi0) * xtr + TMath::Cos(phi0) * ytr;

  TVector3 trarothit = TVector3(xp, yp, 0.);

  // I want the positive phi angle from x axis
  // in range [0, 360[.
  // I use TVector3::Phi() [fromcentertohit.Phi()]:
  // x   y      Phi        use!
  // -------------------------
  // +   +      0/90       phi
  // -   +     90/180      phi
  // -   -   -180/-90    phi + 360
  // +   -    -90/0      phi + 360

  double phi = trarothit.Phi();
  //  if(trarothit.Y() < 0) phi += (2 * TMath::Pi());

  //  cout << "final phi in rad " << phi << endl;
  //  return phi * TMath::RadToDeg();

  Double_t Phi0 = phi0;

  Double_t x0 = from.X();
  Double_t y0 = from.Y();

  //   Double_t Phi0 = TMath::ATan2((y0 - fCenterY),(x0 - fCenterX));

  //   // CHECK :-)GOOD! ...
  //   TVector2 v(x0 - fCenterX, y0 - fCenterY);
  double alpha = TMath::ATan2(hit.Y() - y0 + fRadius * TMath::Sin(Phi0), hit.X() - x0 + fRadius * TMath::Cos(Phi0));
  //   TVector2 p(hit.X() - fCenterX, hit.Y() - fCenterY);

  //   Double_t Fi = - fCharge *  TMath::ACos(v * p / (v.Mod() * p.Mod()));
  double pi = TMath::Pi();
  double pi2 = 2 * pi;

  //   // Fi = h * (pi2 - h * Fi) // should be correct

  Double_t Fi = phi;
  if ((fCharge > 0 &&
       ((Phi0 > 0 && ((alpha > 0 && alpha > Phi0) || (alpha < 0 && alpha < Phi0 - pi))) || ((Phi0 < 0 && ((alpha > 0 && alpha < pi + Phi0) || (alpha < 0 && alpha > Phi0)))))))
    Fi = -(pi2 + Fi);
  else if ((fCharge < 0 &&
            ((Phi0 > 0 && ((alpha > 0 && alpha < Phi0) || (alpha < 0 && alpha > Phi0 - pi))) || ((Phi0 < 0 && ((alpha > 0 && alpha > pi + Phi0) || (alpha < 0 && alpha < Phi0)))))))
    Fi = pi2 - Fi;

  //  cout << "PHI ----------------- " <<  Fi * TMath::RadToDeg() << endl;

  return Fi * TMath::RadToDeg();
}
// =======================================================================================
void PndTrkTrack::Draw(Color_t color)
{
  cout << "draw" << endl;

  if (fCluster.GetNofHits() > 0) {
    PndTrkHit *hit0 = fCluster.GetHit(0);
    fPhiMin = ComputePhi(hit0->GetPosition());
    //   if(fPhiMin > 180) fPhiMin -= 360;
    hit0->GetPosition().Print();

    PndTrkHit *hitN = fCluster.GetHit(fCluster.GetNofHits() - 1);
    fPhiMax = ComputePhi(hitN->GetPosition());
    //    if(fPhiMax > 180) fPhiMax -= 360;
    hitN->GetPosition().Print();

    //    if(fCharge > 0 && fPhiMin < fPhiMax) fPhiMin += 360;
    //     else if(fCharge < 0 && fPhiMin > fPhiMax) fPhiMax += 360;
  }
  TArc *track = new TArc(fCenterX, fCenterY, fRadius, fPhiMin, fPhiMax);

  cout << fCharge << " " << fCenterX << " " << fCenterY << " " << fRadius << " " << fPhiMin << " " << fPhiMax << endl;
  track->SetFillStyle(0);
  track->SetLineColor(color);
  track->Draw("only SAME");
}

void PndTrkTrack::LightUp()
{
  Draw(kYellow);
  fCluster.LightUp();
}

ClassImp(PndTrkTrack)
