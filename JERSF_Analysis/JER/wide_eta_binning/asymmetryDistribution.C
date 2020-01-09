#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <TChain.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TH1F.h>
#include <TLatex.h>
#include <TFile.h>
#include <TLine.h>
#include <TStyle.h>
#include <TMath.h>
#include <TLegend.h>
#include <TPaveStats.h>
#include <TGraphAsymmErrors.h>
#include <TFrame.h>
#include <TString.h>

#include "/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/UHH2/DiJetJERC/include/constants.h"
#include "/nfs/dust/cms/user/matthies/AndreaMalara/PersonalCode/tdrstyle_all.C"


// void additionalplots() {
//   TString path ="/nfs/dust/cms/user/amalara/sframe_all/";
//   TString standard = path+"JER2017_v2_CrossCheck_DATA/Fall17_17Nov2017_V27/AK4CHS/";
//   TString cleaned = path+"JER2017_v2_L1Seed_DATA/Fall17_17Nov2017_V27/AK4CHS/";
//   // uhh2.AnalysisModuleRunner.DATA.DATA_RunF.root
//
//   std::vector<double> Pt_bins;
//   for (int i = 0; i < n_pt_bins_Si; i++) Pt_bins.push_back(pt_bins_Si[i]);
//   Pt_bins.push_back(1500);
//   std::vector<double> eta_bins_edge(eta_bins, eta_bins + sizeof(eta_bins)/sizeof(double));
//
//   TH2F* h_standard = new TH2F("standard", "standard", eta_bins_edge.size()-1, &eta_bins_edge[0], Pt_bins.size()-1, &Pt_bins[0]);
//
//   TFile* file;
//   TTree* tree;
//
//   Float_t pt, eta;
//   Double_t random;
//   Int_t ev;
//
//   std::vector<TString> v = {"B", "C", "D", "E", "F"};
//   for (auto iter = v.begin(); iter != v.end(); ++iter) {
//     std::cout << *iter << " " << cleaned+"uhh2.AnalysisModuleRunner.DATA.DATA_Run"+*iter+".root" << std::endl;
//     file = TFile::Open(cleaned+"uhh2.AnalysisModuleRunner.DATA.DATA_Run"+*iter+".root");
//     tree = (TTree*)file->Get("AnalysisTree");
//     tree->Branch("jet1_pt",&pt,"jet1_pt/F");
//     tree->Branch("jet1_eta",&eta,"jet1_eta/F");
//     // tree->Draw("jet1_pt:jet1_eta>>h_temp");
//     for (Int_t i = 0; i<tree->GetEntries(); i++) {
//       tree->GetEntry(i);
//       h_standard->Fill(pt,eta);
//     }
//     std::cout << h_standard->GetEntries() << std::endl;
//   }
//
//
//
//   new TCanvas;
//   h_standard->Draw("colz");
// }


void alpha2D() {
  TString path      ="/nfs/dust/cms/user/amalara/WorkingArea/uhh2_94X_v2/CMSSW_9_4_1/src/UHH2/JER2017/Analysis/";
  // TString input     ="hist_preparation/MC/wide_eta_bin/file/StandardPtBins/Fall17_17Nov2017_V27/AK4CHS/QCDPt/";
  TString input     ="hist_preparation/MC/wide_eta_bin/file/save_v1/StandardPtBins_CrossCheck/Fall17_17Nov2017_V27/AK4CHS/";
  TString output    ="JER/wide_eta_binning/file/StandardPtBins/Fall17_17Nov2017_V27/AK4CHS/standard/QCDPt/";
  TString namefile  ="alpha2D_old.pdf";
  TFile* file = TFile::Open(path + input+"alpha_spectrum.root");
  cout << path + input+"alpha_spectrum.root" << endl;
  TString name;
  TH2F* h_2;
  TCanvas* canv = new TCanvas("alpha2D", "alpha2D", 800,600);
  canv->SetTickx(0);
  canv->SetTicky(0);
  canv->SetRightMargin(0.15);

  canv->Print(path+output+"[");
  #define PLOT(mode,min,max,name1,canv_)\
  for (int eta = min; eta <= max; eta++) {\
    for (int pt = 1; pt <= 10; pt++) {\
      cout<< mode << " - eta " << eta << " - pt " << pt << endl;\
      name = name1; name += mode; name +="_eta"; name += eta; name += "_pt"; name += pt; name += ";1";\
      h_2 = (TH2F*)file->Get(name);\
      h_2->SetStats(kFALSE);\
      canv_->SetTitle(name);\
      h_2->Draw("colz");\
      canv_->Print(path+output+namefile+"[");\
    }\
  }\

  // PLOT("SM",1,9,"alpha2D_",canv)
  // PLOT("FE_reference",1,3,"alpha2D_",canv)
  // PLOT("FE_control",4,10,"alpha2D_",canv)
  // PLOT("FE",11,13,"alpha2D_",canv)

  canv->Print(path+output+namefile+"]");




  namefile  ="alpha1D_old.pdf";
  TH1F* h_1;
  TCanvas* canv1 = new TCanvas("alpha", "alpha", 800,600);
  canv1->SetTickx(0);
  canv1->SetTicky(0);
  canv1->SetRightMargin(0.15);

  PLOT("SM",1,9,"alpha_",canv1)
  PLOT("FE_reference",1,3,"alpha_",canv1)
  PLOT("FE_control",4,10,"alpha_",canv1)
  PLOT("FE",11,13,"alpha_",canv1)

  canv1->Print(path+output+namefile+"]");

}



void asymmetryBehaviour(double alpha_cut = 0.985) {
  TString path      ="/nfs/dust/cms/user/amalara/WorkingArea/uhh2_94X_v2/CMSSW_9_4_1/src/UHH2/JER2017/Analysis/";
  // TString input     ="hist_preparation/MC/wide_eta_bin/file/StandardPtBins/Fall17_17Nov2017_V27/AK4CHS/QCDPt/";
  TString input     ="hist_preparation/MC/wide_eta_bin/file/save_v1/StandardPtBins_CrossCheck/Fall17_17Nov2017_V27/AK4CHS/";
  TString output    = "alpha_studies/"; output += (int)(alpha_cut*1000); output += "/";
  gSystem->Exec("mkdir -p "+output);
  TFile* file = TFile::Open(path + input+"histograms_mc_incl_full.root");
  TString input_data="hist_preparation/data/wide_eta_bin/file/save_v1/StandardPtBins_CrossCheck/Fall17_17Nov2017_V27/AK4CHS/RunBCDEF/";
  TFile* file_data = TFile::Open(path + input_data+"histograms_data_incl_full.root");

  int color_data = kBlue;
  int color_MC = kRed;
  int color_gen = kGreen+2;
  TH1F* h;
  double low, up, asym, asymerr, kurt, skew;
  TString name;
  int eta_max = 13;
  int pt_max = 10;
  int alpha_max = 6;

  std::vector<double> alphas;
  alphas.push_back(0.05); alphas.push_back(0.1); alphas.push_back(0.15); alphas.push_back(0.20); alphas.push_back(0.25); alphas.push_back(0.3);



  std::vector<std::vector<std::vector<double>>> kurts_data(eta_max, std::vector<std::vector<double>> (pt_max, std::vector<double>(alpha_max)));
  std::vector<std::vector<std::vector<double>>> skews_data(eta_max, std::vector<std::vector<double>> (pt_max, std::vector<double>(alpha_max)));

  std::vector<std::vector<std::vector<double>>> kurts(eta_max, std::vector<std::vector<double>> (pt_max, std::vector<double>(alpha_max)));
  std::vector<std::vector<std::vector<double>>> skews(eta_max, std::vector<std::vector<double>> (pt_max, std::vector<double>(alpha_max)));

  std::vector<std::vector<std::vector<double>>> kurts_gen(eta_max, std::vector<std::vector<double>> (pt_max, std::vector<double>(alpha_max)));
  std::vector<std::vector<std::vector<double>>> skews_gen(eta_max, std::vector<std::vector<double>> (pt_max, std::vector<double>(alpha_max)));


  #define PLOTskew(file,mode,min_,max_,name1,kurts_,skews_)\
  for (int eta = min_; eta <= max_; eta++) {\
    for (int pt = 1; pt <= pt_max; pt++) {\
      for (int alpha = 1; alpha <= alpha_max; alpha++) {\
        name = name1; name += mode; name +="_eta"; name += eta; name += "_pt"; name += pt; name += "_alpha"; name += alpha;\
        h = (TH1F*)file->Get(name);\
        if (h->Integral() > 0) {\
          h->ComputeIntegral();\
          Double_t xq[2], yq[2];\
          xq[0] = std::min(alpha_cut, 1.-alpha_cut);\
          xq[1] = std::max(alpha_cut, 1.-alpha_cut);\
          h->GetQuantiles(2, yq, xq);\
          h->GetXaxis()->SetRange(h->FindBin(yq[0]), h->FindBin(yq[1]));\
          asym = h->GetRMS();\
          asymerr = h->GetRMSError();\
          kurt = h->GetKurtosis();\
          skew = h->GetSkewness();\
          low = h->GetBinCenter(h->FindBin(yq[0])); up = h->GetBinCenter(h->FindBin(yq[1]));\
        } else { asym = 0.; asymerr = 0.; low = 10.0; up = 10.0;};\
        if (!isfinite(skew)) skew = -10.;\
        if (!isfinite(kurt)) kurt = -10.;\
        kurts_.at(eta-1).at(pt-1).at(alpha-1) = kurt;\
        skews_.at(eta-1).at(pt-1).at(alpha-1) = skew;\
      }\
    }\
  }\

  PLOTskew(file_data,"SM",1,9,"asymm_",kurts_data,skews_data)
  PLOTskew(file_data,"FE_reference",1,3,"asymm_",kurts_data,skews_data)
  PLOTskew(file_data,"FE_control",4,10,"asymm_",kurts_data,skews_data)
  PLOTskew(file_data,"FE",11,13,"asymm_",kurts_data,skews_data)

  PLOTskew(file,"SM",1,9,"asymm_",kurts,skews)
  PLOTskew(file,"FE_reference",1,3,"asymm_",kurts,skews)
  PLOTskew(file,"FE_control",4,10,"asymm_",kurts,skews)
  PLOTskew(file,"FE",11,13,"asymm_",kurts,skews)

  PLOTskew(file,"SM",1,9,"gen_asymm_",kurts_gen,skews_gen)
  PLOTskew(file,"FE_reference",1,3,"gen_asymm_",kurts_gen,skews_gen)
  PLOTskew(file,"FE_control",4,10,"gen_asymm_",kurts_gen,skews_gen)
  PLOTskew(file,"FE",11,13,"gen_asymm_",kurts_gen,skews_gen)

  TGraph* gr_kurts      = new TGraph(alphas.size(), &(alphas[0]), &(kurts.at(12).at(7).at(0)));
  TGraph* gr_kurts_gen  = new TGraph(alphas.size(), &(alphas[0]), &(kurts_gen.at(12).at(7).at(0)));

  TGraph* gr_skews      = new TGraph(alphas.size(), &(alphas[0]), &(skews.at(12).at(7).at(0)));
  TGraph* gr_skews_gen  = new TGraph(alphas.size(), &(alphas[0]), &(skews_gen.at(12).at(7).at(0)));

  TCanvas* canv = tdrCanvas("kurtosis",0, 0.35, -1.0, 1.0, "alpha", "kurtosis");
  canv->SetTickx(0);
  canv->SetTicky(0);
  tdrDraw(gr_kurts, "P", kFullDotLarge, color_MC);
  tdrDraw(gr_kurts_gen, "P", kFullDotLarge, color_gen);

  tdrDraw(gr_skews, "P", kFullDotLarge, kMagenta);
  tdrDraw(gr_skews_gen, "P", kFullDotLarge, kOrange+1);

  TH1F* h_kurt_data = new TH1F("h_kurt_data","h_kurt_data", 100, -1.0,1.0);
  TH1F* h_skew_data = new TH1F("h_skew_data","h_skew_data", 100, -0.4,0.4);

  TH1F* h_kurt = new TH1F("h_kurt","h_kurt", 100, -1.0,1.0);
  TH1F* h_skew = new TH1F("h_skew","h_skew", 100, -0.4,0.4);

  TH1F* h_kurt_gen = new TH1F("h_kurt_gen","h_kurt_gen", 100, -1.0,1.0);
  TH1F* h_skew_gen = new TH1F("h_skew_gen","h_skew_gen", 100, -0.4,0.4);

  TH2F* h_skew_data_2D = new TH2F("h_skew_data_2D","h_skew_data_2D", pt_max, 0, pt_max, eta_max, 0, eta_max);
  TH2F* h_skew_2D = new TH2F("h_skew_2D","h_skew_2D", pt_max, 0, pt_max, eta_max, 0, eta_max);
  TH2F* h_skew_gen_2D = new TH2F("h_skew_gen_2D","h_skew_gen_2D", pt_max, 0, pt_max, eta_max, 0, eta_max);

  // TProfile* h_skew_data_2D_pt = new TProfile("h_skew_data_2D_pt","h_skew_data_2D_pt", pt_max, 0, pt_max , -0.4,0.4);
  // TProfile* h_skew_2D_pt = new TProfile("h_skew_2D_pt","h_skew_2D_pt", pt_max, 0, pt_max , -0.4,0.4);
  // TProfile* h_skew_gen_2D_pt = new TProfile("h_skew_gen_2D_pt","h_skew_gen_2D_pt", pt_max, 0, pt_max , -0.4,0.4);

  TString name_profile;

  #define DEFPROFILE(var)\
  name_profile = "h_skew_data_2D_pt"; name_profile+= #var;\
  TProfile* h_skew_data_2D_##var = new TProfile(name_profile,name_profile, var##_max, 0, var##_max , -0.4,0.4);\
  name_profile = "h_skew_2D_pt"; name_profile+= #var;\
  TProfile* h_skew_2D_##var = new TProfile(name_profile,name_profile, var##_max, 0, var##_max , -0.4,0.4);\
  name_profile = "h_skew_gen_2D_pt"; name_profile+= #var;\
  TProfile* h_skew_gen_2D_##var = new TProfile(name_profile,name_profile, var##_max, 0, var##_max , -0.4,0.4);\

  DEFPROFILE(pt)
  DEFPROFILE(eta)
  DEFPROFILE(alpha)


  for (int eta = 1; eta <= eta_max; eta++) {
    for (int pt = 1; pt <= pt_max; pt++) {
      double skew_data=0, skew=0, skew_gen=0;
      for (int alpha = 1; alpha <= alpha_max; alpha++) {
        h_kurt_data->Fill(kurts_data.at(eta-1).at(pt-1).at(alpha-1));
        h_skew_data->Fill(skews_data.at(eta-1).at(pt-1).at(alpha-1));
        h_kurt->Fill(kurts.at(eta-1).at(pt-1).at(alpha-1));
        h_skew->Fill(skews.at(eta-1).at(pt-1).at(alpha-1));
        h_kurt_gen->Fill(kurts_gen.at(eta-1).at(pt-1).at(alpha-1));
        h_skew_gen->Fill(skews_gen.at(eta-1).at(pt-1).at(alpha-1));

        h_skew_data_2D_pt->Fill(pt-1,skews_data.at(eta-1).at(pt-1).at(alpha-1));
        h_skew_2D_pt->Fill(pt-1,skews.at(eta-1).at(pt-1).at(alpha-1));
        h_skew_gen_2D_pt->Fill(pt-1,skews_gen.at(eta-1).at(pt-1).at(alpha-1));

        h_skew_data_2D_eta->Fill(eta-1,skews_data.at(eta-1).at(pt-1).at(alpha-1));
        h_skew_2D_eta->Fill(eta-1,skews.at(eta-1).at(pt-1).at(alpha-1));
        h_skew_gen_2D_eta->Fill(eta-1,skews_gen.at(eta-1).at(pt-1).at(alpha-1));

        h_skew_data_2D_alpha->Fill(alpha-1,skews_data.at(eta-1).at(pt-1).at(alpha-1));
        h_skew_2D_alpha->Fill(alpha-1,skews.at(eta-1).at(pt-1).at(alpha-1));
        h_skew_gen_2D_alpha->Fill(alpha-1,skews_gen.at(eta-1).at(pt-1).at(alpha-1));
        skew_data += skews_data.at(eta-1).at(pt-1).at(alpha-1);
        skew += skews.at(eta-1).at(pt-1).at(alpha-1);
        skew_gen += skews_gen.at(eta-1).at(pt-1).at(alpha-1);
      }
      h_skew_data_2D->SetBinContent(pt-1,eta-1, skew_data/6);
      h_skew_2D->SetBinContent(pt-1,eta-1, skew/6);
      h_skew_gen_2D->SetBinContent(pt-1,eta-1, skew_gen/6);
    }
  }

  #define PLOTPROFILE(var)\
  name_profile = "c_skew_2D_"; name_profile += #var;\
  TCanvas* c_skew_2D_##var = tdrCanvas(name_profile,0., var##_max, -0.1, 0.1, #var, "skewness");\
  h_skew_data_2D_##var ->SetMarkerColor(color_data);\
  h_skew_2D_##var ->SetMarkerColor(color_MC);\
  h_skew_gen_2D_##var ->SetMarkerColor(color_gen);\
  h_skew_data_2D_##var ->SetStats(kFALSE);\
  h_skew_2D_##var ->SetStats(kFALSE);\
  h_skew_gen_2D_##var ->SetStats(kFALSE);\
  h_skew_data_2D_##var ->Draw("same");\
  h_skew_2D_##var ->Draw("same");\
  h_skew_gen_2D_##var ->Draw("same");\
  c_skew_2D_##var->Print(output+name_profile+".pdf");\


  PLOTPROFILE(pt)
  PLOTPROFILE(eta)
  PLOTPROFILE(alpha)

  TString name2D;

  #define  PLOT2D(var)\
  name2D = "c_skew_"; name2D += #var; name2D += "2D";\
  h_skew_##var##2D ->SetStats(kFALSE);\
  h_skew_##var##2D->GetZaxis()->SetRangeUser(-3,1);\
  TCanvas* c_skew_##var##2D = tdrCanvas(name2D,0., pt_max, 0, alpha_max, "pt", "eta");\
  c_skew_##var##2D->SetRightMargin(0.15);\
  h_skew_##var##2D ->Draw("colz");\
  c_skew_##var##2D->Print(output+name2D+".pdf");\

  PLOT2D(data_)
  PLOT2D()
  PLOT2D(gen_)


  // h_skew_data_2D ->SetStats(kFALSE);
  // h_skew_2D ->SetStats(kFALSE);
  // h_skew_gen_2D ->SetStats(kFALSE);
  // h_skew_data_2D->GetZaxis()->SetRangeUser(-3,1);
  // h_skew_2D->GetZaxis()->SetRangeUser(-3,1);
  // h_skew_gen_2D->GetZaxis()->SetRangeUser(-3,1);
  // TCanvas* c_skew_data_2D = tdrCanvas("c_skew_data_2D",0., pt_max, 0, alpha_max, "pt", "eta");
  // c_skew_data_2D->SetRightMargin(0.15);
  // h_skew_data_2D ->Draw("colz");
  // c_skew_data_2D->Print(output+"c_skew_data_2D.pdf");
  // TCanvas* c_skew_2D = tdrCanvas("c_skew_2D",0., pt_max, 0, alpha_max, "pt", "eta");
  // c_skew_2D->SetRightMargin(0.15);
  // h_skew_2D ->Draw("colz");
  // c_skew_data_2D->Print(output+"c_skew_data_2D.pdf");
  // TCanvas* c_skew_gen_2D = tdrCanvas("c_skew_gen_2D",0., pt_max, 0, alpha_max, "pt", "eta");
  // c_skew_gen_2D->SetRightMargin(0.15);
  // h_skew_gen_2D ->Draw("colz");

  TCanvas* c_kurtosis = tdrCanvas("c_kurtosis",-1.0, 1.0, 0.0, 0.2, "kurtosis", "A.U.");
  h_kurt_data->Scale(1./h_kurt_data->Integral());
  h_kurt->Scale(1./h_kurt->Integral());
  h_kurt_gen->Scale(1./h_kurt_gen->Integral());
  tdrDraw(h_kurt_data, "", kFullCircle, color_data);
  tdrDraw(h_kurt, "", kFullCircle, color_MC);
  tdrDraw(h_kurt_gen, "", kFullCircle, color_gen);
  TCanvas* c_skewness = tdrCanvas("c_skewness",-0.4,0.4, 0.0, 0.2, "skewness", "A.U.");
  h_skew_data->Scale(1./h_skew_data->Integral());
  h_skew->Scale(1./h_skew->Integral());
  h_skew_gen->Scale(1./h_skew_gen->Integral());
  tdrDraw(h_skew_data, "", kFullCircle, color_data);
  tdrDraw(h_skew, "", kFullCircle, color_MC);
  tdrDraw(h_skew_gen, "", kFullCircle, color_gen);
  c_skewness->Print(output+"c_skewness.pdf");



}


void asymmetryDistribution() {
  TString path = "/nfs/dust/cms/user/amalara/sframe_all/JER2017_QCD/Fall17_17Nov2017_V27/AK4CHS/uhh2.AnalysisModuleRunner.MC.";
  TString name, name1, name2, text, Cut;
  std::vector<TString> names;

  names.push_back("QCDPt15to30");
  names.push_back("QCDPt30to50");
  names.push_back("QCDPt50to80");
  names.push_back("QCDPt80to120");
  names.push_back("QCDPt120to170");
  names.push_back("QCDPt170to300");
  names.push_back("QCDPt300to470");
  names.push_back("QCDPt470to600");
  names.push_back("QCDPt600to800");
  names.push_back("QCDPt800to1000");
  names.push_back("QCDPt1000to1400");
  names.push_back("QCDPt1400to1800");
  names.push_back("QCDPt1800to2400");
  names.push_back("QCDPt2400to3200");
  names.push_back("QCDPt3200toInf");

  std::vector<int> colors;
  colors.push_back(kRed);
  colors.push_back(kBlue);
  colors.push_back(kGreen+2);
  colors.push_back(kOrange+1);
  colors.push_back(kMagenta);
  colors.push_back(kYellow);
  colors.push_back(kCyan);
  colors.push_back(kAzure);
  colors.push_back(kBlack);
  colors.push_back(kViolet);
  colors.push_back(kTeal);
  colors.push_back(kBlue-10);
  colors.push_back(kGreen-10);
  colors.push_back(kYellow-10);
  colors.push_back(kMagenta-10);

  TCanvas* c_all = new TCanvas("c_all", "c_all", 800,600);
  TCanvas* c_all1 = new TCanvas("c_all1", "c_all1", 800,600);
  THStack *hs = new THStack("hs","");
  THStack *hs1 = new THStack("hs1","");


  #define  PLOT_(num)\
  TFile *_file##num = TFile::Open(path+names[num]+".root");\
  TTree* t##num = (TTree*)_file##num->Get("AnalysisTree");\
  name  = "h0"+names[num];\
  name1 = "h_1"+names[num];\
  name2 = "h_2"+names[num];\
  TCanvas* c##num = new TCanvas(name, name, 800,600);\
  TH1F* h##num = new TH1F(name, name, 200, 0.0, 1.0);\
  text = "asymmetry>>"; text += name; t##num->Draw(text, Cut);\
  h##num->SetLineColor(colors[num]); c##num->cd(); c##num->SetLogy(); h##num->Draw("hist"); hs->Add(h##num);\
  TCanvas* c_1##num = new TCanvas(name1, name1, 800,600);\
  TH1F* h_1##num = new TH1F(name1, name1, 200, -10.0, 10.0);\
  text = "TMath::Log10(weight)>>"; text += name1; t##num->Draw(text, Cut);\
  h_1##num->SetLineColor(colors[num]); c_1##num->cd(); c_1##num->SetLogy(); h_1##num->Draw(); hs1->Add(h_1##num);\
  TCanvas* c_2##num = new TCanvas(name2, name2, 800,600);\
  TH2F* h_2##num = new TH2F(name2, name2, 100, 0.0, 1.0, 200, -10.0, 10.0);\
  text = "TMath::Log10(weight):asymmetry>>"; text += name2; t##num->Draw(text, Cut);\
  c_2##num->cd(); h_2##num->Draw("colz");\

  Cut = "weight*(0<1)";
  Cut = "weight*((0<1)&&(nPU<75)&&(nPU>10))";
  // Cut = "weight*((0<1)&&(nPU<75)&&(nPU>10)&&(weight<50))";

  // Cut = "";

  PLOT_(0)
  PLOT_(1)
  PLOT_(2)
  PLOT_(3)
  PLOT_(4)
  PLOT_(5)
  PLOT_(6)
  PLOT_(7)
  PLOT_(8)
  PLOT_(9)
  PLOT_(10)
  PLOT_(11)
  PLOT_(12)
  PLOT_(13)
  PLOT_(14)

  c_all->cd();
  c_all->SetLogy();
  hs->Draw("nostack, hist");
  gPad->BuildLegend(0.75,0.75,0.95,0.95,"");


  c_all1->cd();
  c_all1->SetLogy();
  hs1->Draw("nostack, hist");
  gPad->BuildLegend(0.75,0.75,0.95,0.95,"");

  Cut = "weight*((0<1)&&(nPU<75)&&(nPU>10)&&(weight<30))";
}
