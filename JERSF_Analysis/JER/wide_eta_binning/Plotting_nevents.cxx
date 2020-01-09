#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "TROOT.h"
#include "TMath.h"
#include "/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/UHH2/DiJetJERC/include/constants.h"
#include "/nfs/dust/cms/user/matthies/AndreaMalara/PersonalCode/tdrstyle_all.C"


void Plotting_nevents() {
  writeExtraText = false;

  TString path_ = "/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/UHH2/DiJetJERC/JERSF_Analysis/";

  TString store_path = path_+"hist_preparation/MC/wide_eta_bin/file/";
  TString save_path  = path_+"JER/wide_eta_binning/file/";
  TString fileName, path;

  std::vector<TString> studies;
  studies.push_back("MergeL2Res");

  std::vector<TString> JECs;
  JECs.push_back("Autumn18_V19");

  std::vector<TString> JETs;
  JETs.push_back("AK4CHS");
  JETs.push_back("AK8Puppi");

  std::vector<TString> QCDS_DATAS;
  QCDS_DATAS.push_back("QCDHT");
  QCDS_DATAS.push_back("RunD");
  QCDS_DATAS.push_back("RunABC");
  QCDS_DATAS.push_back("RunABCD");

  int tot = 0;

  for(TString study : studies){
    for(TString JEC : JECs){
      for(TString JET : JETs){
        path = save_path+study+"/"+JEC+"/"+JET+"/standard/";
        std::cout << "saving in: " << path << '\n';
        for(TString DATA : QCDS_DATAS){
          fileName = store_path+study+"/"+JEC+"/"+JET+"/"+DATA+"/histograms_nevents.root";
          if (DATA.Contains("Run")) fileName.ReplaceAll("MC","data");
          std::cout << "Loading: " << fileName << '\n';
          TFile *file = TFile::Open(fileName);
          for (size_t i = 0; i < 6; i++) {
            for (std::string mode: {"central", "HF"}) {
              TH2F* h = (TH2F*)file->Get((mode+"_"+std::to_string(i)).c_str());
              h->SetStats(0);
              double xmin=-0.1, xmax=5.3, ymin=0, ymax=1600;
              if (mode=="HF") { xmin = 2.8; ymin = 50; ymax = 450;}
              else xmax = 3;
              TCanvas* c = tdrCanvas(("n_event_"+std::to_string(i)+mode).c_str()+DATA+JEC+JET, xmin, xmax, ymin, ymax, "#eta", "p_{T}",kRectangular, 5, 9);
              c->SetRightMargin(0.15);
              h->Draw("colz text same");
              c->SaveAs(path+DATA+"_"+mode+"_"+std::to_string(i)+".pdf");
              tot++;
              // sleep(5);
            }
          }
        }
      }
    }
  }

  std::cout << "Done: " << tot << '\n';
}
