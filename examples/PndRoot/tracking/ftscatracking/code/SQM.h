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

void PndFTSCAGBTracker::Refit_1(FTSCANPletV &triplet, const FTSCAHits &hits)
{

  //#include "Tang.h"

  Double_t det;
  TMatrixD H(4, 4);
  TMatrixD HY(4, 1);
  TMatrixD Cov(4, 4);
  TMatrixD HC(4, 1);

  float f1, f2, f3, f4, f5, Y, chi2;

  for (Int_t i = 0; i < 4; i++) {
    HY(i, 0) = 0;
    HC(i, 0) = 0;
    for (Int_t j = 0; j < 4; j++) {
      H(i, j) = 0;
    }
  }

  const int N = triplet.N();

  float Tx, Ty, Bx, By;

  //#include "SQM_0.h"

  cout << " **************** Param before Refit_1 ************ " << endl;

  PndFTSCATrackParamVector &param = triplet.Param();

  cout << " param.Tx()[0] " << param.Tx()[0] << " param.Ty()[0] " << param.Ty()[0] << " param.QP()[0] " << param.QP()[0] << " param.X()[0] " << param.X()[0] << " param.Y()[0] "
       << param.Y()[0] << " param.Z()[0] " << param.Z()[0] << " param.Chi2()[0] " << param.Chi2()[0] << " param.NDF()[0] " << param.NDF()[0] << endl;

  Tx = param.Tx()[0];
  Ty = param.Ty()[0];
  Bx = param.X()[0] - param.Tx()[0] * param.Z()[0];
  By = param.Y()[0] - param.Ty()[0] * param.Z()[0];

  float Tx_K = Tx;
  float Ty_K = Ty;
  float Bx_K = Bx;
  float By_K = By;

  cout << " **************** Before Refit_1 ************ " << endl;
  cout << " Tx " << Tx << endl;
  cout << " Ty " << Ty << endl;
  cout << " Bx " << Bx << endl;
  cout << " By " << By << endl;

  //  float_m active = triplet.IsValid();

  for (int it = 0; it < 3; it++) {

    for (Int_t i = 0; i < 4; i++) {
      HY(i, 0) = 0;
      HC(i, 0) = 0;
      for (Int_t j = 0; j < 4; j++) {
        H(i, j) = 0;
      }
    }

    for (int ihit = 0; ihit < N; ihit++) {
      const TESV &index = triplet.IHit(ihit);
      cout << "index.s station " << index.s << " index.e " << index.e << endl;
      const FTSCAElementsOnStation<FTSCAHit> &hits_st = hits.OnStationConst(index.s[0]);

      const FTSCAHit &hit = hits_st[index.e[0]];

      int ISta = (int)hit.IStation();
      cout << "chtck  hit.IStation()" << ISta << endl;

      // float fX =  param.X()[0] - param.Tx()[0]*(param.Z()[0] - hit.X0());
      // float fY =  param.Y()[0] - param.Ty()[0]*(param.Z()[0] - hit.X0());

      const PndFTSCAGBHit &hit_1 = fHits[hit.Id()];

      cout << " ************************************************* " << endl;
      cout << " MC_Tx " << hit_1.point_Px / hit_1.point_Pz << endl;
      cout << " MC_Ty " << hit_1.point_Py / hit_1.point_Pz << endl;
      cout << " MC_Bx " << (hit_1.point_X - (hit_1.point_Px / hit_1.point_Pz) * hit_1.point_Z) << endl;
      cout << " MC_By " << (hit_1.point_Y - (hit_1.point_Py / hit_1.point_Pz) * hit_1.point_Z) << endl;

      cout << "  hit.R() " << hit.R() << " hit.ErrR() " << sqrt(hit.Err2R()) << endl;
      cout << "  X " << hit.X1() << " Y " << hit.X2() << " Z " << hit.X0() << endl;

      const FTSCAStation &station = GetParameters().Station(ISta);
      const float s = station.f.sin;

      const float c = station.f.cos;

      cout << " s " << s << " c " << c << endl;

      float Tx_MC = hit_1.point_Px / hit_1.point_Pz;
      float Ty_MC = hit_1.point_Py / hit_1.point_Pz;
      float Bx_MC = (hit_1.point_X - (hit_1.point_Px / hit_1.point_Pz) * hit_1.point_Z);
      float By_MC = (hit_1.point_Y - (hit_1.point_Py / hit_1.point_Pz) * hit_1.point_Z);
      float R_MC = (c * (Tx_MC * hit.X0() + Bx_MC - hit.X1()) + s * (Ty_MC * hit.X0() + By_MC - hit.X2())) / sqrt(1.f + (c * Tx_MC + s * Ty_MC) * (c * Tx_MC + s * Ty_MC));

      if (s == 0) {

        float D2_mc_hit = sqrt((hit_1.point_X - hit.X1()) * (hit_1.point_X - hit.X1()) + (hit_1.point_Z - hit.X0()) * (hit_1.point_Z - hit.X0()));
        float Diff_D_R = D2_mc_hit - hit.R();
        cout << " D2_mc_hit " << D2_mc_hit << " hit.R() " << hit.R() << " Diff_D_R " << Diff_D_R << endl;
      }

      float diff_R;

      if (R_MC > 0.f)
        diff_R = R_MC - hit.R();
      else
        diff_R = R_MC + hit.R();

      cout << " diff_R " << diff_R << endl;

      float R_MC_1 =
        (c * (Tx_MC * hit_1.point_Z + Bx_MC - hit.X1()) + s * (Ty_MC * hit_1.point_Z + By_MC - hit.X2())) / sqrt(1.f + (c * Tx_MC + s * Ty_MC) * (c * Tx_MC + s * Ty_MC));

      float diff_R_1;
      if (R_MC_1 > 0.f)
        diff_R_1 = R_MC_1 - hit.R();
      else
        diff_R_1 = R_MC_1 + hit.R();

      cout << " diff_R_1 " << diff_R_1 << endl;

      float R_MC_2 = (c * (hit_1.point_X - hit.X1()) + s * (hit_1.point_Y - hit.X2())) / sqrt(1.f + (c * Tx_MC + s * Ty_MC) * (c * Tx_MC + s * Ty_MC));

      cout << " R_MC_2 " << R_MC_2 << " hit.R()" << hit.R() << endl;

      float diff_R_2;
      if (R_MC_2 < 0.f)
        diff_R_2 = R_MC_2 + hit.R();
      else
        diff_R_2 = R_MC_2 - hit.R();

      cout << " diff_R_2 " << diff_R_2 << endl;

      // dbg
      /*
      if ( ihit==0 ) {
      Tx = hit_1.point_Px/hit_1.point_Pz;
      Ty = hit_1.point_Py/hit_1.point_Pz;
      Bx = hit_1.point_X - (hit_1.point_Px/hit_1.point_Pz)*hit_1.point_Z;
      By = hit_1.point_Y - (hit_1.point_Py/hit_1.point_Pz)*hit_1.point_Z;
      }

      */

      float Y_point_of_wire = 0.f;
      // dbg

      float fX = Tx * hit.X0() + Bx;
      float fY = Ty * hit.X0() + By;

      cout << " Tx " << Tx << "  Bx " << Bx << " Ty " << Ty << " By " << By << endl;

      float tx = c * Tx + s * Ty;
      float rCorrection = sqrt(1.f + tx * tx);

      float Diff = c * (fX - hit.X1()) + s * (fY - hit.X2());
      // float Diff = c*(fX - hit.X1()) + s*(fY - Y_point_of_wire);

      float Diff_mc = c * (hit_1.point_X - hit.X1()) + s * (hit_1.point_Y - hit.X2());

      // float Diff_mc = c*(hit_1.point_X -hit.X1()) +  s*(hit_1.point_Y - Y_point_of_wire);

      cout << " Diff " << Diff << " Diff_mc " << Diff_mc << endl;

      cout << " ************************************************* " << endl;

      float R = hit.R();

      if (Diff < 0.f)
        R = (-1.f) * R;

      // if (Diff_mc  < 0.f ) R=(-1.f)*R; //dbg

      // float err = (sqrt(hit.Err2R())+ s*sqrt(hit.Err2X2()))*rCorrection;

      float err = sqrt(hit.Err2R());
      // float err1= 2*hit.R()*err;

      Y = (R - (Diff / rCorrection)) / err;
      // Y = (R*R - Diff*Diff/(rCorrection*rCorrection) )/err;

      f1 = (c * hit.X0() / rCorrection - c * Diff * tx / (rCorrection * rCorrection * rCorrection)) / err;

      // float rCorrection2= rCorrection*rCorrection;
      // f1 = (2*Diff*c*hit.X0()/rCorrection2 - (Diff*Diff*2*tx*c)/(rCorrection2*rCorrection2))/err;

      f2 = c / (rCorrection * err);
      // f2 = 2*Diff*c/(rCorrection*rCorrection*err);

      f3 = (s * hit.X0() / rCorrection - s * Diff * tx / (rCorrection * rCorrection * rCorrection)) / err;
      // f3 = (2*Diff*s*hit.X0()/rCorrection2- (Diff*Diff*2*tx*s)/(rCorrection2*rCorrection2))/err;

      f4 = s / (rCorrection * err);

      // f4 =2*Diff*s/(rCorrection*rCorrection*err);

      // f4=2*Diff*s/(rCorrection*rCorrection*err);

      H(0, 0) = H(0, 0) + f1 * f1;
      H(0, 1) = H(0, 1) + f1 * f2;
      H(0, 2) = H(0, 2) + f1 * f3;
      H(0, 3) = H(0, 3) + f1 * f4;
      H(1, 0) = H(0, 1);
      H(1, 1) = H(1, 1) + f2 * f2;
      H(1, 2) = H(1, 2) + f2 * f3;
      H(1, 3) = H(1, 3) + f2 * f4;
      H(2, 0) = H(0, 2);
      H(2, 1) = H(1, 2);
      H(2, 2) = H(2, 2) + f3 * f3;
      H(2, 3) = H(2, 3) + f3 * f4;
      H(3, 0) = H(0, 3);
      H(3, 1) = H(1, 3);
      H(3, 2) = H(2, 3);
      H(3, 3) = H(2, 3) + f4 * f4;

      HY(0, 0) = HY(0, 0) + Y * f1;
      HY(1, 0) = HY(1, 0) + Y * f2;
      HY(2, 0) = HY(2, 0) + Y * f3;
      HY(3, 0) = HY(3, 0) + Y * f4;

    } // ihit

    H.Print();
    H.Invert(&det);
    Cov = H;
    H.Print();
    Cov.Print();
    cout << "det= " << det << endl;

    HC = H * HY;
    HC.Print();

    cout << "DTx= " << HC(0, 0) << " +- " << sqrt(Cov(0, 0)) << endl;
    cout << "DBx= " << HC(1, 0) << " +- " << sqrt(Cov(1, 1)) << endl;
    cout << "DTy= " << HC(2, 0) << " +- " << sqrt(Cov(2, 2)) << endl;
    cout << "DBy= " << HC(3, 0) << " +- " << sqrt(Cov(3, 3)) << endl;

    Tx = Tx + HC(0, 0);
    Bx = Bx + HC(1, 0);
    Ty = Ty + HC(2, 0);
    By = By + HC(3, 0);

    cout << "Tx= " << Tx << endl;
    cout << "Ty= " << Ty << endl;
    cout << "Bx= " << Bx << endl;
    cout << "By= " << By << endl;

  } // it

  param.SetTx(Tx);
  param.SetTy(Ty);
  param.SetX(Tx * param.Z()[0] + Bx);
  param.SetY(Ty * param.Z()[0] + By);
  param.SetBx(Bx);
  // float Err2_Tx=(float)Cov(0,0);
  // param.SetCov( 5, Err2_Tx);

  float Tx_1 = param.Tx()[0];
  float Ty_1 = param.Ty()[0];
  float Bx_1 = param.Bx()[0];
  float By_1 = By;

  chi2 = 0.0;
  float chi2_k = 0.0;
  for (int ihit = 0; ihit < N; ihit++) {
    const TESV &index = triplet.IHit(ihit);
    cout << "index.s station " << index.s << " index.e " << index.e << endl;
    const FTSCAElementsOnStation<FTSCAHit> &hits_st = hits.OnStationConst(index.s[0]);

    const FTSCAHit &hit = hits_st[index.e[0]];

    int ISta = (int)hit.IStation();
    cout << "chtck  hit.IStation()" << ISta << endl;

    const FTSCAStation &station = GetParameters().Station(ISta);
    const float s_1 = station.f.sin;

    const float c_1 = station.f.cos;

    cout << " s_1 " << s_1 << " c_1 " << c_1 << endl;

    float tx_1 = c_1 * Tx_1 + s_1 * Ty_1;
    float rCorrection_1 = sqrt(1.f + tx_1 * tx_1);
    float fX_1 = Tx_1 * hit.X0() + Bx_1;
    float fY_1 = Ty_1 * hit.X0() + By_1;

    float Diff_1 = c_1 * (fX_1 - hit.X1()) + s_1 * (fY_1 - hit.X2());

    float tx_k = c_1 * Tx_K + s_1 * Ty_K;
    float rCorrection_k = sqrt(1.f + tx_k * tx_k);
    float fX_k = Tx_K * hit.X0() + Bx_K;
    float fY_k = Ty_K * hit.X0() + By_K;

    float Diff_k = c_1 * (fX_k - hit.X1()) + s_1 * (fY_k - hit.X2());
    float R_k = hit.R();
    if (Diff_k < 0.f)
      R_k = (-1.f) * R_k;
    // float err_k = (sqrt(hit.Err2R())+ s_1*sqrt(hit.Err2X2()) )*rCorrection_k;

    float Y_point_of_wire_1 = 0.f;
    // float Diff_1 = c_1*(fX_1 - hit.X1()) + s_1*(fY_1 - Y_point_of_wire_1);
    float R_1 = hit.R();
    if (Diff_1 < 0.f)
      R_1 = (-1.f) * R_1;

    // float err_1 = (sqrt(hit.Err2R())+ s_1*sqrt(hit.Err2X2()) )*rCorrection_1;
    float err_1 = sqrt(hit.Err2R());
    float err_k = err_1;

    float Mes, Mes_k;
    if (ISta > 50) {
      Mes = (Diff_1 / rCorrection_1 - R_1) / (sqrt(hit.Err2R()) + s_1 * sqrt(hit.Err2X2()) + c_1 * sqrt(hit.Err2X1()));
      Mes_k = (Diff_k / rCorrection_k - R_k) / (sqrt(hit.Err2R()) + s_1 * sqrt(hit.Err2X2()) + c_1 * sqrt(hit.Err2X1()));
    } else {
      // Mes = (Diff_1/rCorrection_1 - R_1)/(sqrt(hit.Err2R())+ s_1*sqrt(hit.Err2X2()) );
      // Mes_k = (Diff_k/rCorrection_k - R_k)/(sqrt(hit.Err2R())+ s_1*sqrt(hit.Err2X2()) );
      Mes = (Diff_1 / rCorrection_1 - R_1) / sqrt(hit.Err2R());
      Mes_k = (Diff_k / rCorrection_k - R_k) / sqrt(hit.Err2R());
    }
    float Mes1 = (Diff_1 - R_1 * rCorrection_1) / (err_1 * rCorrection_1);
    if (Mes > 10.f)
      cout << " change  sighn for hit " << ihit << endl;

    cout << " Mes " << Mes << " Mes1 " << Mes1 << endl;
    chi2 = chi2 + Mes * Mes;
    chi2_k = chi2_k + Mes_k * Mes_k;
    cout << " Mes*Mes " << Mes * Mes << endl;
  }
  cout << " chi2 " << chi2 << " chi2_k " << chi2_k << endl;

  param.SetChi2(chi2);
}
