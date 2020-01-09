#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "TROOT.h"
#include "TMath.h"
#include "/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/UHH2/DiJetJERC/include/constants.h"
#include "/nfs/dust/cms/user/matthies/AndreaMalara/PersonalCode/tdrstyle_all.C"


void PlotConvergence() {

    double SF_V1[13][4] = {
      {0.522, 1.13514, 1.15980, 1.11048},
      {0.783, 1.15044, 1.17391, 1.12697},
      {1.131, 1.09849, 1.15115, 1.04583},
      {1.305, 1.11220, 1.19943, 1.02498},
      {1.740, 1.13714, 1.19724, 1.07703},
      {1.930, 1.16148, 1.24258, 1.08039},
      {2.043, 1.22625, 1.34355, 1.10895},
      {2.322, 1.21735, 1.36674, 1.06797},
      {2.500, 1.2963, 1.5334, 1.0592},
      {2.853, 1.3418, 1.5509, 1.1327},
      {2.964, 2.50302, 2.87286, 2.13318},
      {3.139, 1.30288, 1.44249, 1.16327},
      {5.191, 1.22603, 1.31457, 1.13749},
    };

    double SF_V2[13][4] = {
      {0.522, 1.14305, 1.17463, 1.11147},
      {0.783, 1.15469, 1.19308, 1.1163},
      {1.131, 1.09858, 1.14846, 1.0487},
      {1.305, 1.11228, 1.23488, 0.989692},
      {1.74, 1.13846, 1.24552, 1.0314},
      {1.93, 1.166, 1.25024, 1.08176},
      {2.043, 1.23372, 1.50596, 0.961485},
      {2.322, 1.22344, 1.37008, 1.07681},
      {2.5, 1.25777, 1.56002, 0.955516},
      {2.853, 1.94442, 2.25561, 1.63323},
      {2.964, 2.52625, 2.89482, 2.15768},
      {3.139, 1.29669, 1.44568, 1.1477},
      {5.191, 1.2515, 1.35555, 1.14745},
    };

    double SF_V3[13][4] = {
      {0.522, 1.1432, 1.16543, 1.12097},
      {0.783, 1.18152, 1.22987, 1.13317},
      {1.131, 1.09887, 1.14444, 1.0533},
      {1.305, 1.11365, 1.25332, 0.973979},
      {1.74, 1.13072, 1.27776, 0.98369},
      {1.93, 1.15996, 1.25759, 1.06232},
      {2.043, 1.23926, 1.43013, 1.04839},
      {2.322, 1.26039, 1.41049, 1.11029},
      {2.5, 1.40853, 1.61049, 1.20657},
      {2.853, 1.9909, 2.55927, 1.42253},
      {2.964, 2.29227, 2.66654, 1.918},
      {3.139, 1.26957, 1.37847, 1.16067},
      {5.191, 1.15425, 1.30663, 1.00187},
    };


    std::vector<double> eta_bins_all(eta_bins, eta_bins + sizeof(eta_bins)/sizeof(double));


    TCanvas* canv = tdrCanvas("SF_final", eta_bins_all.at(0)-0.1, eta_bins_all.at(eta_bins_all.size()-1)+0.5, 0., 1.5, "#eta", "JER SF");
    canv->SetTickx(0);
    canv->SetTicky(0);
    TLegend *leg = tdrLeg(0.65,0.67,0.80,0.92, 0.040, 42, kBlack);
    tdrHeader(leg,"", 12);

    std::vector<double> eta_center, eta_err;
    std::vector<double> V1_center, V1_err;
    std::vector<double> V2_center, V2_err;

    for (unsigned int i = 0; i < 13; i++) {
      eta_center.push_back((eta_bins_all[i+1]+eta_bins_all[i])/2);
      eta_err.push_back((eta_bins_all[i+1]-eta_bins_all[i])/2);
      // V1_center.push_back(SF_V1[i][1]/SF_V3[i][1]);
      // V2_center.push_back(SF_V2[i][1]/SF_V3[i][1]);
      // V1_center.push_back(TMath::Abs(1-SF_V1[i][2]/SF_V1[i][1])/TMath::Abs(1-SF_V3[i][2]/SF_V3[i][1]));
      // V2_center.push_back(TMath::Abs(1-SF_V2[i][2]/SF_V2[i][1])/TMath::Abs(1-SF_V3[i][2]/SF_V3[i][1]));

      V1_center.push_back(TMath::Abs(1-SF_V3[i][2]/SF_V3[i][1]));
      V2_center.push_back(TMath::Abs(1-SF_V2[i][2]/SF_V2[i][1]));
      V1_err.push_back((SF_V3[i][2]/SF_V3[i][1]));
      V2_err.push_back((SF_V3[i][2]/SF_V3[i][1]));
    }


    TGraphErrors* gr_V1 = new TGraphErrors(V1_center.size(), &(eta_center[0]), &V1_center[0], &(eta_err[0]), &V1_err[0]);
    TGraphErrors* gr_V2 = new TGraphErrors(V2_center.size(), &(eta_center[0]), &V2_center[0], &(eta_err[0]), &V2_err[0]);

    tdrDraw(gr_V1, "P5", kFullDotLarge, kRed+1, kSolid, kRed+1, 3005, kRed+1);
    tdrDraw(gr_V2, "P5", kFullDotLarge, kBlue+1, kSolid, kBlue+1, 3005, kBlue+1);

    leg->AddEntry(gr_V1, "V1","f");
    leg->AddEntry(gr_V2, "V2","f");
    leg->Draw("same");

    // canv->Print("convergence/convergence.pdf","pdf");
    // canv->Print("convergence/convergence.pdf","pdf");



}
