#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "TROOT.h"
#include "TMath.h"
#include "/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/UHH2/DiJetJERC/include/constants.h"
#include "/nfs/dust/cms/user/matthies/AndreaMalara/PersonalCode/tdrstyle_all.C"

typedef std::vector<TString> VecTS;
typedef std::vector<string> VecS;
typedef std::vector<double> VecD;
typedef std::vector<std::vector<double>> VecDD;
typedef std::vector<std::vector<std::vector<double>>> VecDDD;
typedef std::map<TString, VecD> MapD;
typedef std::map<TString, VecDD> MapDD;

VecTS systematics_name_all({"gaustails_0.95","JEC_up","JEC_down","PU_up","PU_down","PLI_up","PLI_down","alpha","pTdep"});
VecTS systematics_name_split({"gaustails_0.95","JEC","PU", "PLI", "alpha","pTdep", "rest"});
VecTS systematics_name_minimal({"gaustails_0.95","JEC", "rest"});

bool dosplit = true; // split source of Uncertainties
int method = 4; //2-uncorr 4-corr
int pt_dep_method = 4; //4-min value 5-max value

// NO MERGE SM and FE
int shift_SM = 11; // How many point to skip from the end
int shift_FE = 3; // How many point to skip from the beginning
int shift_barrel = 1; // How many point used to calculate SF in the previous step

bool isOverlap = false; // just define the global variable. The value should be adjusted afterwards in the code; //TODO

TString path_ = "/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/UHH2/DiJetJERC/JERSF_Analysis/JER/wide_eta_binning/file/";

TGraphErrors* CreateTGraphSF(TString filename = "SF_final_tex.txt") {

  filename += "SF_final_tex.txt";

  if (gSystem->AccessPathName(filename)) throw runtime_error("check: "+filename);

  VecD eta, eta_center, eta_err, SF, SF_Err;
  eta.push_back(0.); string line;
  ifstream myfile(filename, ios::in);
  while (!myfile.eof()) {
    std::getline(myfile, line); if (line.length()==0) continue;
    line = line.substr(line.find("{{")+2,line.find("}}")-2);
    VecD values; TString tok; int from = 0;
    while (((TString)line).Tokenize(tok, from, ", ")) {values.push_back(tok.Atof());}
    eta.push_back(values[0]);
    SF.push_back(values[1]);
    SF_Err.push_back(values[2]-values[1]);
  }
  myfile.close();

  for (size_t i = 0; i < eta.size()-1; i++) { eta_center.push_back((eta[i+1]+eta[i])/2); eta_err.push_back((eta[i+1]-eta[i])/2); }
  TGraphErrors* gr = new TGraphErrors(SF.size(), &(eta_center[0]), &SF[0], &(eta_err[0]), &SF_Err[0]);
  return gr;
}

TGraphErrors* CreateTGraphSF(VecD eta, VecDD jer) {
  VecD eta_center, eta_err, SF, SF_Err;
  for (unsigned int i = 0; i < eta.size()-1; i++) {
    eta_center.push_back((eta[i+1]+eta[i])/2);
    eta_err.push_back((eta[i+1]-eta[i])/2);
    SF.push_back(jer[i][0]);
    SF_Err.push_back(jer[i][1]);
  }
  TGraphErrors* gr = new TGraphErrors(SF.size(), &(eta_center[0]), &SF[0], &(eta_err[0]), &SF_Err[0]);
  return gr;
}



void LoadSF2(VecDDD &SFs, TString filename, int skip_front=0, int skip_back=0) {
  VecDD SF;
  VecD eta_bin_SM_center; 						//0
  VecD eta_bin_SM_error;    					//1
  VecD SF_uncorrelated_SM;       		//2
  VecD SF_uncorrelated_SM_error;			//3
  VecD SF_correlated_SM;        		 	//4
  VecD SF_correlated_SM_error;   		//5
  VecD SF_uncorrelated_SM_ptdep_min; //6
  VecD SF_uncorrelated_SM_ptdep_max;	//7
  VecD SF_correlated_SM_ptdep_min;   //8
  VecD SF_correlated_SM_ptdep_max;   //9

  // SF_uncorrelated_FE, SF_uncorrelated_FE_error, SF_correlated_FE, SF_correlated_FE_error, eta_bin_FE_center, eta_bin_FE_error;
  if (gSystem->AccessPathName(filename)) { std::cout << "check: " << filename << '\n'; return false;}

  std::ifstream file(filename, ios::in);
  SF.push_back(eta_bin_SM_center);
  SF.push_back(eta_bin_SM_error);
  SF.push_back(SF_uncorrelated_SM);
  SF.push_back(SF_uncorrelated_SM_error);
  SF.push_back(SF_correlated_SM);
  SF.push_back(SF_correlated_SM_error);
  SF.push_back(SF_uncorrelated_SM_ptdep_min);
  SF.push_back(SF_uncorrelated_SM_ptdep_max);
  SF.push_back(SF_correlated_SM_ptdep_min);
  SF.push_back(SF_correlated_SM_ptdep_max);

  std::string line;
  getline(file, line);
  int count = 0;

  while (!file.eof()) {
    getline(file, line);
    count++;
    if (count<=skip_front || count>skip_back)   continue;
    std::istringstream iss(line);
    VecS results(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());
    for (unsigned int i = 0; i < results.size(); i++) { SF.at(i).push_back(std::stold(results[i]));}
  }

  SFs.push_back(SF);
  SF.clear();
  file.close();
}


VecDD LoadSF(TString filename, int skip_front=0, int skip_back=0) {
  VecDD SF;
  VecD eta_bin_SM_center; 						//0
  VecD eta_bin_SM_error;    					//1
  VecD SF_uncorrelated_SM;       		//2
  VecD SF_uncorrelated_SM_error;			//3
  VecD SF_correlated_SM;        		 	//4
  VecD SF_correlated_SM_error;   		//5
  VecD SF_uncorrelated_SM_ptdep_min; //6
  VecD SF_uncorrelated_SM_ptdep_max;	//7
  VecD SF_correlated_SM_ptdep_min;   //8
  VecD SF_correlated_SM_ptdep_max;   //9

  // SF_uncorrelated_FE, SF_uncorrelated_FE_error, SF_correlated_FE, SF_correlated_FE_error, eta_bin_FE_center, eta_bin_FE_error;
  if (gSystem->AccessPathName(filename)) { std::cout << "check: " << filename << '\n'; throw runtime_error("Error in loading files."); }

  std::ifstream file(filename, ios::in);
  SF.push_back(eta_bin_SM_center);
  SF.push_back(eta_bin_SM_error);
  SF.push_back(SF_uncorrelated_SM);
  SF.push_back(SF_uncorrelated_SM_error);
  SF.push_back(SF_correlated_SM);
  SF.push_back(SF_correlated_SM_error);
  SF.push_back(SF_uncorrelated_SM_ptdep_min);
  SF.push_back(SF_uncorrelated_SM_ptdep_max);
  SF.push_back(SF_correlated_SM_ptdep_min);
  SF.push_back(SF_correlated_SM_ptdep_max);

  std::string line;
  getline(file, line);
  int count = 0;

  while (!file.eof()) {
    getline(file, line);
    count++;
    if (count<=skip_front || count>skip_back)   continue;
    std::istringstream iss(line);
    VecS results(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());
    for (unsigned int i = 0; i < results.size(); i++) { SF.at(i).push_back(std::stold(results[i]));}
  }

  file.close();
  return SF;
}


void Load_all_SF2(VecDDD & SFs, TString path, TString central_SF_txt, int skip_front=0,int skip_back=0) {
  TString filename = path+central_SF_txt;
  LoadSF2(SFs, filename, skip_front, skip_back);
  for (unsigned int i = 0; i < systematics_name_all.size(); i++) {
    TString temp = central_SF_txt.Copy();
    TString sys = systematics_name_all.at(i);
    // pt_dep values are saved in all the folder. But the one in the nominal folder are calculated wrt to nominal values. It is saved only the Max and Min value of the difference of the SF
    if (sys.Contains("gaustails")) filename = path+temp.ReplaceAll("standard",sys);
    else if (sys.Contains("_")) filename = path+temp.ReplaceAll("standard",sys(0,sys.First("_"))+"/"+sys(sys.First("_")+1,sys.Length()));
    else if (sys.Contains("pTdep")) filename = path+central_SF_txt;
    else filename = path+temp.ReplaceAll("standard",sys);
    LoadSF2(SFs, filename, skip_front, skip_back);
  }
}

void Load_all_SF(MapDD & SFs, TString path, TString central_SF_txt, int skip_front=0,int skip_back=0) {
  TString filename = path+central_SF_txt;
  SFs["nominal"] = LoadSF(filename, skip_front, skip_back);
  for (unsigned int i = 0; i < systematics_name_all.size(); i++) {
    TString temp = central_SF_txt.Copy();
    TString sys = systematics_name_all.at(i);
    // pt_dep values are saved in all the folder. But the one in the nominal folder are calculated wrt to nominal values. It is saved only the Max and Min value of the difference of the SF
    if (sys.Contains("gaustails")) filename = path+temp.ReplaceAll("standard",sys);
    else if (sys.Contains("_")) filename = path+temp.ReplaceAll("standard",sys(0,sys.First("_"))+"/"+sys(sys.First("_")+1,sys.Length()));
    else if (sys.Contains("pTdep")) filename = path+central_SF_txt;
    else filename = path+temp.ReplaceAll("standard",sys);
    SFs[sys] = LoadSF(filename, skip_front, skip_back);
  }
}

void GetValuesAndUncertainties2(VecD &SF, VecD &stat, VecD &eta_bin_center, VecD &eta_bin_error, VecDD &systematics, VecDDD & SFs, double etabins) {
  for (unsigned int i = 0; i < 1+systematics_name_all.size(); i++){
    if (i==0) {
      // Getting Central Values & Statistical Uncertainties
      for (unsigned int j = 0; j < etabins; j++){
        eta_bin_center.push_back(SFs.at(0).at(0).at(j));
        eta_bin_error.push_back(SFs.at(0).at(1).at(j));
        SF.push_back(SFs.at(0).at(method).at(j));
        stat.push_back(SFs.at(0).at(method+1).at(j));
      }
    } else if (i < systematics_name_all.size()) {
      // Getting Systematics Uncertainties. Diff wrt the nominal value
      VecD temp;
      // for (unsigned int j = 0; j < etabins; j++) temp.push_back(TMath::Abs(SFs.at(0).at(method).at(j) - SFs.at(i).at(method).at(j)));
      for (unsigned int j = 0; j < etabins; j++) temp.push_back(SFs.at(i).at(method).at(j) - SFs.at(0).at(method).at(j));
      systematics.push_back(temp);
      temp.clear();
    }
    else{
      VecD temp;
      // Getting Systematics Uncertainties for pt_dep. It is saved only the Max and Min value
      for (unsigned int j = 0; j < etabins; j++) temp.push_back((SFs.at(i).at(method+pt_dep_method).at(j) + SFs.at(i).at(method+pt_dep_method+1).at(j))/2);
      systematics.push_back(temp);
      temp.clear();
    }
  }
}

void GetValuesAndUncertainties(VecD &SF, VecD &stat, VecD &eta_bin_center, VecD &eta_bin_error, MapD &systematics, MapDD & SFs, double etabins) {

  VecD rest;
  for (unsigned int i = 0; i < etabins; i++){
    eta_bin_center.push_back(SFs["nominal"].at(0).at(i));
    eta_bin_error.push_back(SFs["nominal"].at(1).at(i));
    SF.push_back(SFs["nominal"].at(method).at(i));
    stat.push_back(SFs["nominal"].at(method+1).at(i));
    rest.push_back(0);
  }

  for (TString sys: systematics_name_all) {
    VecD temp;
    if (sys=="pTdep") {
      for (unsigned int i = 0; i < etabins; i++) temp.push_back((SFs[sys].at(method+pt_dep_method).at(i) + SFs[sys].at(method+pt_dep_method+1).at(i))/2);
    } else { for (unsigned int i = 0; i < etabins; i++) temp.push_back(SFs[sys].at(method).at(i) - SFs["nominal"].at(method).at(i));}
    systematics[sys] = temp;
    temp.clear();
  }

  for (TString sys: {"JEC","PU","PLI"}) {
    VecD temp;
    for (unsigned int i = 0; i < etabins; i++) {
      double sys_up = systematics[sys+"_up"].at(i);
      double sys_down = systematics[sys+"_down"].at(i);
      temp.push_back(TMath::Sqrt(TMath::Power(sys_up+sys_down,2)+2*TMath::Power(sys_up-sys_down,2))/2);
      if (dosplit) SF.at(i) += (sys_up-sys_down)/2;
    }
    systematics[sys] = temp;
    temp.clear();
  }

  for (TString sys: systematics_name_split) {
    if(std::find(systematics_name_minimal.begin(), systematics_name_minimal.end(), sys) != systematics_name_minimal.end()) continue;
    for (unsigned int i = 0; i < etabins; i++) rest.at(i) += TMath::Power(systematics[sys].at(i),2);
  }
  for (unsigned int i = 0; i < etabins; i++) rest.at(i) += TMath::Sqrt(rest.at(i));
  systematics["rest"] = rest;

}


void CalculateTotalError2(VecD &total_error, VecD &systematics_all, VecDD systematics, VecD &stat, double etabins, VecD SF, VecD eta_bin_center, VecD eta_bin_err) {
  TString text = " \t\t\t & stat ";
  for (unsigned int j = 0; j < systematics_name_all.size(); j++){
    text +=  " & " + systematics_name_all.at(j);
  }
  text += "\\\\";
  for (unsigned int i = 0; i < etabins; i++) {
    double err = 0;
    text = Form("$[ %.2f-%.2f]$ & $%.2f $ & $ %.2f $", eta_bin_center.at(i)-eta_bin_err.at(i), eta_bin_center.at(i)+eta_bin_err.at(i), SF.at(i), stat.at(i)/SF.at(i)*100);
    for (unsigned int j = 0; j < systematics_name_all.size(); j++) {
      err += TMath::Power(systematics.at(j).at(i),2);
      text += Form(" & $ %.2f $", 100*systematics.at(j).at(i)/SF.at(i));
    }
    err = TMath::Sqrt(err);
    systematics_all.push_back(err);
    total_error.push_back(TMath::Sqrt(TMath::Power(err,2)+TMath::Power(stat.at(i),2)));
    text += Form("&$%.2f$", 100*TMath::Sqrt(TMath::Power(err,2)+TMath::Power(stat.at(i),2)-TMath::Power(systematics.at(0).at(i),2))/SF.at(i));
    text += Form(" & $ %.2f $\\\\", 100*TMath::Sqrt(TMath::Power(err,2)+TMath::Power(stat.at(i),2))/SF.at(i));
  }
}


void CalculateTotalError(VecD &total_error, VecD &systematics_all, MapD systematics, VecD &stat) {
  for (unsigned int i = 0; i < stat.size(); i++) {
    double err = 0;
    for (TString sys: dosplit? systematics_name_minimal: systematics_name_all) err += TMath::Power(systematics[sys].at(i),2);
    total_error.push_back(TMath::Sqrt(err+TMath::Power(stat.at(i),2)));
    systematics_all.push_back(TMath::Sqrt(err));

  }
}

void Plot_Uncertainties2(TString name_method, VecD eta_bins, VecD eta_bin_center, VecD SF, VecD eta_bin_error, VecD stat, VecD systematics_all, VecDD systematics, VecD total_error, TString path, VecD SF_2) {
  std::vector<int> colors;
  colors.push_back(kBlue-4); colors.push_back(kGreen-2); colors.push_back(kOrange);
  colors.push_back(kViolet-3); colors.push_back(kCyan); colors.push_back(kMagenta);
  colors.push_back(kAzure+7); colors.push_back(kSpring); colors.push_back(kBlack); colors.push_back(kCyan+2);


  TCanvas* canv_SF = tdrCanvas("Statistics_"+name_method, eta_bins[0]-0.1, eta_bins[eta_bins.size()-1]+0.1, 0.5, 3.0, "#eta", "JER SF");
  canv_SF->SetTickx(0);
  canv_SF->SetTicky(0);
  TLegend *leg = tdrLeg(0.60,0.67,0.80,0.92, 0.025, 42, kBlack);
  tdrHeader(leg,"Uncertainties", 12);
  TGraphErrors* gr_stat = new TGraphErrors(eta_bins.size()-1, &(eta_bin_center[0]), &SF[0], &(eta_bin_error[0]), &stat[0]);
  TGraphErrors* gr_syst = new TGraphErrors(eta_bins.size()-1, &(eta_bin_center[0]), &SF[0], &(eta_bin_error[0]), &systematics_all[0]);
  TGraphErrors* gr_tot  = new TGraphErrors(eta_bins.size()-1, &(eta_bin_center[0]), &SF[0], &(eta_bin_error[0]), &total_error[0]);
  leg->AddEntry(gr_stat, "stat","f");
  leg->AddEntry(gr_syst, "syst","f");
  leg->AddEntry(gr_tot, "stat+syst","f");
  tdrDraw(gr_tot,  "P5", kFullDotLarge, kGreen-2, kSolid, kGreen-2, 3005, kGreen-2);
  tdrDraw(gr_syst, "P5", kFullDotLarge, kBlue-4,  kSolid, kBlue-4,  3005, kBlue-4);
  tdrDraw(gr_stat, "P5", kFullDotLarge, kRed+1,   kSolid, kRed+1,   3005, kRed+1);
  leg->Draw("same");

  canv_SF->Print(path+"SF_"+name_method+".pdf","pdf");

  TCanvas* canv_stat = tdrCanvas("Uncertainties_"+name_method,eta_bins[0]-0.1, eta_bins[eta_bins.size()-1]+0.1, 0.0001, 100.0, "#eta", "Uncertainties");
  canv_stat->SetTickx(0);
  canv_stat->SetTicky(0);
  canv_stat->SetLogy();
  leg = tdrLeg(0.55,0.67,0.80,0.92, 0.025, 42, kBlack);
  leg->SetNColumns(2);
  tdrHeader(leg,"Uncertainties", 12);
  TH1* h = new TH1F("", "", eta_bins.size()-1, &eta_bins[0] );
  for (unsigned int i = 0; i < eta_bins.size()-1; i++) { h->SetBinContent(i+1,stat.at(i));}
  leg->AddEntry(h, "stat","l");
  for (unsigned int j = 0; j <= systematics_name_all.size(); j++){
    TH1F* h = new TH1F("", "", eta_bins.size()-1, &eta_bins[0] );
    h->SetLineWidth(5);
    if (j != systematics_name_all.size()) {
      for (unsigned int i = 0; i < eta_bins.size()-1; i++) {h->SetBinContent(i+1,TMath::Abs(systematics.at(j).at(i)));}
      tdrDraw(h, "hist", kFullDotLarge, colors[j], kSolid, colors[j], 0, colors[j]);
      leg->AddEntry(h, systematics_name_all[j],"l");
    } else {
      for (unsigned int i = 0; i < eta_bins.size()-1; i++) {
        if (i < shift_FE && name_method=="SM") continue;
        if (i >= SF.size() - shift_SM && name_method=="FE") continue;
        h->SetBinContent(i+1, (TMath::Abs(SF.at(i)-SF_2.at((name_method=="FE")?i+shift_FE: i-shift_FE))/2));
      }
      tdrDraw(h, "hist", kFullDotLarge, colors[j], kSolid, colors[j], 0, colors[j]);
      leg->AddEntry(h, "|SM-FE|/2","l");
    }
  }

  tdrDraw(h, "hist", kFullDotLarge, kRed+1, kSolid, kRed+1, 0, kRed+1);
  h->SetLineWidth(8); h->SetLineStyle(9);
  leg->Draw("same");

  canv_stat->Print(path+"Uncertainties_"+name_method+".pdf","pdf");

}

void Plot_Uncertainties(TString name_method, VecD eta_bins, VecD eta_bin_center, VecD SF, VecD eta_bin_error, VecD stat, VecD systematics_all, MapD systematics, VecD total_error, TString path, VecD SF_2) {
  std::map<TString, int> colors;
  colors["gaustails_0.95"] = kBlue-4;  colors["SMvsFE"] = kCyan+2; colors["rest"] = kOrange;
  colors["JEC_up"] = kGreen-2; colors["JEC_down"] = kGreen+1; colors["JEC"] = kGreen-2;
  colors["PU_up"] = kAzure+7; colors["PU_down"] = kAzure+10;
  colors["PLI_up"] = kViolet+1; colors["PLI_down"] = kViolet-2;
  colors["alpha"] = kOrange; colors["pTdep"] = kBlack;

  TCanvas* canv_SF = tdrCanvas("Statistics_"+name_method, eta_bins[0]-0.1, eta_bins[eta_bins.size()-1]+0.1, 0.5, 3.0, "#eta", "JER SF");
  canv_SF->SetTickx(0);
  canv_SF->SetTicky(0);
  TLegend *leg = tdrLeg(0.60,0.67,0.80,0.92, 0.025, 42, kBlack);
  tdrHeader(leg,"Uncertainties", 12);
  TGraphErrors* gr_stat = new TGraphErrors(eta_bins.size()-1, &(eta_bin_center[0]), &SF[0], &(eta_bin_error[0]), &stat[0]);
  TGraphErrors* gr_syst = new TGraphErrors(eta_bins.size()-1, &(eta_bin_center[0]), &SF[0], &(eta_bin_error[0]), &systematics_all[0]);
  TGraphErrors* gr_tot  = new TGraphErrors(eta_bins.size()-1, &(eta_bin_center[0]), &SF[0], &(eta_bin_error[0]), &total_error[0]);
  leg->AddEntry(gr_stat, "stat","f");
  leg->AddEntry(gr_syst, "syst","f");
  leg->AddEntry(gr_tot, "stat+syst","f");
  tdrDraw(gr_tot,  "P5", kFullDotLarge, kGreen-2, kSolid, kGreen-2, 3005, kGreen-2);
  tdrDraw(gr_syst, "P5", kFullDotLarge, kBlue-4,  kSolid, kBlue-4,  3005, kBlue-4);
  tdrDraw(gr_stat, "P5", kFullDotLarge, kRed+1,   kSolid, kRed+1,   3005, kRed+1);
  leg->Draw("same");

  canv_SF->Print(path+"SF_"+name_method+".pdf","pdf");

  for (TString split: {"split","all"}) {
    TCanvas* canv_stat = tdrCanvas("Uncertainties_"+name_method+split,eta_bins[0]-0.1, eta_bins[eta_bins.size()-1]+0.1, 0.0001, 100.0, "#eta", "Uncertainties");
    canv_stat->SetTickx(0);
    canv_stat->SetTicky(0);
    canv_stat->SetLogy();
    leg = tdrLeg(0.55,0.67,0.80,0.92, 0.025, 42, kBlack);
    leg->SetNColumns(2);
    tdrHeader(leg,"Uncertainties", 12);
    TH1* hstat = new TH1F("", "", eta_bins.size()-1, &eta_bins[0] );
    for (unsigned int i = 0; i < eta_bins.size()-1; i++) { hstat->SetBinContent(i+1,stat.at(i));}
    leg->AddEntry(hstat, "stat","l");
    for (TString sys: (split=="split")? systematics_name_minimal: systematics_name_all){
      TH1F* h = new TH1F("", "", eta_bins.size()-1, &eta_bins[0] );
      h->SetLineWidth(5);
      for (unsigned int i = 0; i < eta_bins.size()-1; i++) {h->SetBinContent(i+1,TMath::Abs(systematics[sys].at(i)));}
      tdrDraw(h, "hist", kFullDotLarge, colors[sys], kSolid, colors[sys], 0, colors[sys]);
      leg->AddEntry(h, sys,"l");
    }
    if (isOverlap) {
      TH1* h = new TH1F("", "", eta_bins.size()-1, &eta_bins[0] );
      h->SetLineWidth(5);
      for (unsigned int i = 0; i < eta_bins.size()-1; i++) {
        systematics["SMvsFE"].push_back(0);
        double val = 0;
        if (!(i < shift_FE && name_method=="SM") && !(i >= SF.size() - shift_SM && name_method=="FE") ) {
          val = TMath::Abs(SF.at(i)-SF_2.at((name_method=="FE")?i+shift_FE: i-shift_FE))/2;
        }
        systematics["SMvsFE"].at(i) = val;
        // err += TMath::Power(systematics["SMvsFE"].at(i),2);
        h->SetBinContent(i+1, systematics["SMvsFE"].at(i));
      }
      tdrDraw(h, "hist", kFullDotLarge, colors["SMvsFE"], kSolid, colors["SMvsFE"], 0, colors["SMvsFE"]);
      leg->AddEntry(h, "|SM-FE|/2","l");
    }

    tdrDraw(hstat, "hist", kFullDotLarge, kRed+1, kSolid, kRed+1, 0, kRed+1);
    hstat->SetLineWidth(8); hstat->SetLineStyle(9);
    leg->Draw("same");

    canv_stat->Print(path+"Uncertainties_"+name_method+"_"+split+".pdf","pdf");
  }

}


// SFs == (n_systematics, columns in files, eta_bins)
void plot_SF_systematics_(TString path = "", TString QCD_DATA = "QCDPt/RunBCDEF/",TString DATA = "RunBCDEF", TString TITLE_NAME = "") {
  gPrintViaErrorHandler = kTRUE;
  gErrorIgnoreLevel = kFatal;

  extraText  = "Preliminary";  // default extra text is "Preliminary"
  lumi_13TeV = "[MC 102X] Run2018 41.53 fb^{-1}";
  lumi_sqrtS = "13 TeV";       // used with iPeriod = 0, e.g. for simulation-only plots (default is an empty string)

  if (QCD_DATA.Contains("RunA"))     lumi_13TeV = "[MC 102X] Run2018A     14.00 fb^{-1}";
  if (QCD_DATA.Contains("RunB"))     lumi_13TeV = "[MC 102X] Run2018B      7.10 fb^{-1}";
  if (QCD_DATA.Contains("RunC"))     lumi_13TeV = "[MC 102X] Run2018C      6.94 fb^{-1}";
  if (QCD_DATA.Contains("RunD"))     lumi_13TeV = "[MC 102X] Run2018D     31.93 fb^{-1}";
  if (QCD_DATA.Contains("RunAB"))    lumi_13TeV = "[MC 102X] Run2018AB    14.10 fb^{-1}";
  if (QCD_DATA.Contains("RunABC"))   lumi_13TeV = "[MC 102X] Run2018ABC   28.04 fb^{-1}";
  if (QCD_DATA.Contains("RunABCD"))  lumi_13TeV = "[MC 102X] Run2018ABCD  59.97 fb^{-1}";

  //lumi_13TeV = "[MC Pythia8] RunII";

  VecD eta_bins_all(eta_bins_JER,                eta_bins_JER + sizeof(eta_bins_JER)/sizeof(double));
  VecD eta_bins_SM(eta_bins_JER,                 eta_bins_JER + sizeof(eta_bins_JER)/sizeof(double) - shift_SM);
  VecD eta_bins_FE(eta_bins_JER + shift_FE,      eta_bins_JER + sizeof(eta_bins_JER)/sizeof(double));

  std::cout << std::string(230, 42) << "\n";
  std::cout << "all:\t" <<eta_bins_all.size() << "|"<<"\t"; for (auto x: eta_bins_all) std::cout << x << "\t";
  std::cout << "\nSM:\t" <<eta_bins_SM.size() <<" |"<<"\t"; for (auto x: eta_bins_SM) std::cout << x << "\t";
  std::cout << "\nFE:\t" <<eta_bins_FE.size() << "|"<<"\t"; for (auto x: eta_bins_FE) std::cout << x << "\t";
  std::cout << "\n" << std::string(230, 42) << "\n";

  if (shift_SM+shift_FE>eta_bins_all.size()-1) throw runtime_error("THERE IS SEPARATION BETWEEN SM and FE.");
  else if (shift_SM+shift_FE==eta_bins_all.size()-1) { isOverlap = false; std::cout << "THERE IS NO OVERLAP BETWEEN SM AND FE" << '\n';}
  else std::cout << "THERE IS OVERLAP BETWEEN SM AND FE OF " << eta_bins_all.size()-1-shift_SM-shift_FE << '\n';

  VecDDD SFs_SM2, SFs_FE2;
  TString central_SM, central_FE, filename;

  MapDD SFs_SM, SFs_FE;

  central_SM = "standard/"+QCD_DATA+"output/scalefactors_ST.txt";
  central_FE = "standard/"+QCD_DATA+"output/scalefactors_FE.txt";

  /////////////////////////
  //   Standard Method   //
  /////////////////////////

  Load_all_SF2(SFs_SM2, path, central_SM,0,eta_bins_all.size()-1-shift_SM);
  Load_all_SF(SFs_SM, path, central_SM,0,eta_bins_all.size()-1-shift_SM);

  if (SFs_SM.size() != systematics_name_all.size()+1) { std::cout << "ERROR SM" << '\n'; std::cout << SFs_SM.size() << '\n'; return false; }

  VecD SF_SM2, stat_SM2, eta_bin_SM_center2, eta_bin_SM_error2;
  VecD SF_SM, SF_SM_split, stat_SM, eta_bin_SM_center, eta_bin_SM_error;
  VecDD systematics_SM2, systematics_SM2_split;
  MapD systematics_SM;
  VecD total_error_SM2, total_sys_SM2, total_error_SM, total_sys_SM;

  GetValuesAndUncertainties2(SF_SM2, stat_SM2, eta_bin_SM_center2, eta_bin_SM_error2, systematics_SM2, SFs_SM2, eta_bins_SM.size()-1);
  GetValuesAndUncertainties (SF_SM, stat_SM, eta_bin_SM_center, eta_bin_SM_error, systematics_SM, SFs_SM, eta_bins_SM.size()-1);

  CalculateTotalError2(total_error_SM2, total_sys_SM2, systematics_SM2, stat_SM, eta_bins_SM.size()-1, SF_SM, eta_bin_SM_center, eta_bin_SM_error);
  CalculateTotalError(total_error_SM, total_sys_SM, systematics_SM, stat_SM);

  /////////////////////////
  //  Forward Extention  //
  /////////////////////////

  Load_all_SF2(SFs_FE2, path, central_FE,shift_FE-shift_barrel,eta_bins_all.size()-1);
  Load_all_SF(SFs_FE, path, central_FE,shift_FE-shift_barrel,eta_bins_all.size()-1);

  if (SFs_FE.size() != systematics_name_all.size()+1) { std::cout << "ERROR FE" << '\n'; std::cout << SFs_FE.size() << '\n'; return false; }

  VecD SF_FE2, stat_FE2, eta_bin_FE_center2, eta_bin_FE_error2;
  VecD SF_FE, SF_FE_split, stat_FE, eta_bin_FE_center, eta_bin_FE_error;
  VecDD systematics_FE2, systematics_FE2_split;
  MapD systematics_FE;
  VecD total_error_FE2, total_sys_FE2, total_error_FE,total_sys_FE;

  GetValuesAndUncertainties2(SF_FE2, stat_FE2, eta_bin_FE_center2, eta_bin_FE_error2, systematics_FE2, SFs_FE2, eta_bins_FE.size()-1);
  GetValuesAndUncertainties (SF_FE, stat_FE, eta_bin_FE_center, eta_bin_FE_error, systematics_FE, SFs_FE, eta_bins_FE.size()-1);

  CalculateTotalError2(total_error_FE2, total_sys_FE2, systematics_FE2, stat_FE, eta_bins_FE.size()-1, SF_FE, eta_bin_FE_center, eta_bin_FE_error);
  CalculateTotalError(total_error_FE, total_sys_FE, systematics_FE, stat_FE);


  std::cout << std::string(230, 42) << "\n ETA" << '\n';
  for (size_t i = 0; i < eta_bins_all.size()-1; i++) std::cout << Form("|%.3f-%.3f|", eta_bins_all[i],eta_bins_all[i+1]) << '\t';
  std::cout << "\n";
  for (size_t i = 0; i < eta_bins_all.size()-1; i++) {
    if (i<eta_bins_all.size()-1-shift_SM) std::cout << SF_SM[i] << '\t' << '\t';
    else std::cout << '\t'<< '\t';
  }
  std::cout << "\n";
  for (size_t i = 0; i < eta_bins_all.size()-1; i++) {
    if (i>shift_FE-1) std::cout << SF_FE[i-shift_FE] << '\t' << '\t';
    else std::cout << '\t' << '\t';
  }
  std::cout << "\n" << std::string(230, 42)  << "\n";

  /////////////////////////
  //     Combination     //
  /////////////////////////

  VecD SF_final2, SF_final_error2, SF_final_error_stat2, SF_final_error_syst2, eta_bin_all_center2, eta_bin_all_error2;
  SF_final2.clear(); SF_final_error2.clear(); SF_final_error_stat2.clear();
  SF_final_error_syst2.clear(); eta_bin_all_center2.clear(); eta_bin_all_error2.clear();

  for (unsigned int i = 0; i < eta_bins_all.size()-1; i++) {
    int index = i-shift_FE;
    if (i < shift_FE){
      /***/eta_bin_all_center2.push_back(eta_bin_SM_center2.at(i));
      /****/eta_bin_all_error2.push_back(eta_bin_SM_error2.at(i));
      /*************/SF_final2.push_back(SF_SM2.at(i));
      /*******/SF_final_error2.push_back(total_error_SM2.at(i));
      /**/SF_final_error_stat2.push_back(stat_SM2.at(i));
      /**/SF_final_error_syst2.push_back(total_sys_SM2.at(i));
    } else if (i < eta_bins_all.size() - 1 - shift_SM){
      /***/eta_bin_all_center2.push_back(eta_bin_SM_center2.at(i));
      /****/eta_bin_all_error2.push_back(eta_bin_SM_error2.at(i));
      /*************/SF_final2.push_back((SF_SM2.at(i)+SF_FE2.at(index))/2);
      // /****/SF_final_error.push_back(TMath::Sqrt(TMath::Power((stat_SM.at(i)+stat_FE.at(index))/2, 2)+TMath::Power((total_sys_SM2.at(i)+total_sys_FE2.at(index))/2, 2)+TMath::Power((SF_SM.at(i)-SF_FE.at(index))/2, 2)));
      /*******/SF_final_error2.push_back(TMath::Sqrt(stat_SM2.at(i)*stat_SM2.at(i)+stat_FE2.at(index)*stat_FE2.at(index) +total_sys_SM2.at(i)*total_sys_SM2.at(i)+total_sys_FE2.at(index)*total_sys_FE2.at(index)+TMath::Power((SF_SM2.at(i)-SF_FE2.at(index))/2, 2)));
      /**/SF_final_error_stat2.push_back(TMath::Sqrt(stat_SM2.at(i)*stat_SM2.at(i)+stat_FE2.at(index)*stat_FE2.at(index) ));
      /**/SF_final_error_syst2.push_back(TMath::Sqrt(TMath::Power(total_sys_SM2.at(i),2)+TMath::Power(total_sys_FE2.at(index),2)+TMath::Power((SF_SM2.at(i)-SF_FE2.at(index))/2, 2)));
    } else {
      /***/eta_bin_all_center2.push_back(eta_bin_FE_center2.at(index));
      /****/eta_bin_all_error2.push_back(eta_bin_FE_error2.at(index));
      /*************/SF_final2.push_back(SF_FE2.at(index));
      /*******/SF_final_error2.push_back(total_error_FE2.at(index));
      /**/SF_final_error_stat2.push_back(stat_FE2.at(index));
      /**/SF_final_error_syst2.push_back(total_sys_FE2.at(index));
    }
  }

  VecD SF_final, SF_final_error, SF_final_error_stat, SF_final_error_syst, eta_bin_all_center, eta_bin_all_error;
  MapD SF_sys;
  SF_final.clear(); SF_final_error.clear(); SF_final_error_stat.clear();
  SF_final_error_syst.clear(); eta_bin_all_center.clear(); eta_bin_all_error.clear();
  for (VecTS vec: {systematics_name_all,systematics_name_split}) {
    for (TString sys: vec) {
      VecD temp(eta_bins_all.size(),0);
      SF_sys[sys] = temp;
    }
  }

  for (unsigned int i = 0; i < eta_bins_all.size()-1; i++) {
    int index = i-shift_FE;
    if (i < shift_FE){
      /***/eta_bin_all_center.push_back(eta_bin_SM_center.at(i));
      /****/eta_bin_all_error.push_back(eta_bin_SM_error.at(i));
      /*************/SF_final.push_back(SF_SM.at(i));
      /*******/SF_final_error.push_back(total_error_SM.at(i));
      /**/SF_final_error_stat.push_back(stat_SM.at(i));
      /**/SF_final_error_syst.push_back(total_sys_SM.at(i));
      for (VecTS vec: {systematics_name_all,systematics_name_split}) {
        for (TString sys: vec) {
          SF_sys[sys].at(i) = systematics_SM[sys].at(i);
        }
      }
    } else if (i < eta_bins_all.size() - 1 - shift_SM){
      /***/eta_bin_all_center.push_back(eta_bin_SM_center.at(i));
      /****/eta_bin_all_error.push_back(eta_bin_SM_error.at(i));
      /*************/SF_final.push_back((SF_SM.at(i)+SF_FE.at(index))/2);
      // /****/SF_final_error.push_back(TMath::Sqrt(TMath::Power((stat_SM.at(i)+stat_FE.at(index))/2, 2)+TMath::Power((total_sys_SM2.at(i)+total_sys_FE2.at(index))/2, 2)+TMath::Power((SF_SM.at(i)-SF_FE.at(index))/2, 2)));
      /*******/SF_final_error.push_back(TMath::Sqrt(TMath::Power(stat_SM.at(i),2)+TMath::Power(stat_FE.at(index),2) +TMath::Power(total_sys_SM.at(i),2)+TMath::Power(total_sys_FE.at(index),2)+TMath::Power((SF_SM.at(i)-SF_FE.at(index))/2, 2)));
      /**/SF_final_error_stat.push_back(TMath::Sqrt(TMath::Power(stat_SM.at(i),2)+TMath::Power(stat_FE.at(index),2)));
      /**/SF_final_error_syst.push_back(TMath::Sqrt(TMath::Power(total_sys_SM.at(i),2)+TMath::Power(total_sys_FE.at(index),2)+TMath::Power((SF_SM.at(i)-SF_FE.at(index))/2, 2)));
      for (VecTS vec: {systematics_name_all,systematics_name_split}) {
        for (TString sys: vec) SF_sys[sys].at(i) = TMath::Sqrt(TMath::Power(systematics_SM[sys].at(i),2)+TMath::Power(systematics_FE[sys].at(index),2));
      }
    } else {
      /***/eta_bin_all_center.push_back(eta_bin_FE_center.at(index));
      /****/eta_bin_all_error.push_back(eta_bin_FE_error.at(index));
      /*************/SF_final.push_back(SF_FE.at(index));
      /*******/SF_final_error.push_back(total_error_FE.at(index));
      /**/SF_final_error_stat.push_back(stat_FE.at(index));
      /**/SF_final_error_syst.push_back(total_sys_FE.at(index));
      for (VecTS vec: {systematics_name_all,systematics_name_split}) {
        for (TString sys: vec) SF_sys[sys].at(i) = systematics_FE[sys].at(index);
      }
    }
  }

  std::cout << "eta \t SF[\%] \t err[\%]\n";
  for (size_t i = 0; i < SF_SM.size(); i++) std::cout << i << "\t" << Form("%.2f",(SF_SM[i]/SF_SM2[i]-1)*100) << "\t" << Form("%.2f",(total_sys_SM[i]/total_sys_SM2[i]-1)*100) << '\n';

  for (size_t i = 0; i < SF_FE.size(); i++) std::cout << i << "\t" << Form("%.2f",(SF_FE[i]/SF_FE2[i]-1)*100) << "\t" << Form("%.2f",(total_sys_FE[i]/total_sys_FE2[i]-1)*100) << '\n';

  std::cout << "eta \t SF[\%] \t err[\%]\tsys[\%] \n";
  for (size_t i = 0; i < SF_final.size(); i++) std::cout << i << "\t" << Form("%.2f",(SF_final[i]/SF_final2[i]-1)*100) << "\t" << Form("%.2f",(SF_final_error[i]/SF_final_error2[i]-1)*100) << "\t" << Form("%.2f",(SF_final_error_syst[i]/SF_final_error_syst2[i]-1)*100) << '\n';


  /////////////////////////
  // PLOTS AND TXT FILES //
  /////////////////////////

  // Needed for future plots
  ofstream SF_file_final;
  std::cout << path+"standard/"+QCD_DATA+"SF_final_tex.txt" << '\n';
  SF_file_final.open (path+"standard/"+QCD_DATA+"SF_final_tex.txt");
  for (unsigned int i = 0; i < SF_final.size(); i++) {
    SF_file_final << "{{" << eta_bin_all_center[i]+eta_bin_all_error[i] << ", " << SF_final[i] << ", " << SF_final[i]+SF_final_error[i] << ", " << SF_final[i]-SF_final_error[i] << "}},\n";
  }

  ofstream SF_file_twiki;
  SF_file_twiki.open (path+"standard/"+QCD_DATA+"SF_final_twiki.txt");
  SF_file_twiki << "|  *abs(eta) region* |";
  for (size_t i = 0; i < eta_bin_all_center.size(); i++) SF_file_twiki << Form("|%.3f-%.3f|", eta_bin_all_center[i]-eta_bin_all_error[i], eta_bin_all_center[i]+eta_bin_all_error[i]);
  SF_file_twiki << '\n' << "|  *Data/MC SF*      |";
  for (size_t i = 0; i < eta_bin_all_center.size(); i++) SF_file_twiki << Form("|%.4f|", SF_final[i]);
  SF_file_twiki << '\n' << "|  *Stat.Unc*        |";
  for (size_t i = 0; i < eta_bin_all_center.size(); i++) SF_file_twiki << Form("|%.4f|", SF_final_error_stat[i]);
  SF_file_twiki << '\n' << "|  *Syst.Unc*        |";
  for (size_t i = 0; i < eta_bin_all_center.size(); i++) SF_file_twiki << Form("|%.4f|", SF_final_error_syst[i]);
  SF_file_twiki << '\n' << "|  *Total.Unc*       |";
  for (size_t i = 0; i < eta_bin_all_center.size(); i++) SF_file_twiki << Form("|%.4f|", SF_final_error[i]);
  SF_file_twiki.close();

  VecS systematics_text_DB;
  systematics_text_DB.push_back("{1 JetEta 0 None ScaleFactor}");

  for (unsigned int i = 0; i < SF_final.size(); i++) {
    double eta = eta_bin_all_center[i];
    double eta_err = eta_bin_all_error[i];
    double SF = SF_final[i];
    double SF_err = SF_final_error[i];
    std::string text1 = Form("%.3f %.3f 3 %.4f %.4f %.4f",  (eta-eta_err),  (eta+eta_err), SF, SF-SF_err, SF+SF_err );
    std::string text2 = Form("%.3f %.3f 3 %.4f %.4f %.4f", -(eta+eta_err), -(eta-eta_err), SF, SF-SF_err, SF+SF_err );
    systematics_text_DB.push_back(text1);
    systematics_text_DB.insert(systematics_text_DB.begin()+1,text2);
  }
  if ((systematics_text_DB.size()-1)%2!=0) throw runtime_error("In plot_SF_systematics.cxx.");

  ofstream SF_file_DB;
  SF_file_DB.open (path+"standard/"+QCD_DATA+"SF_final_DB.txt");
  for (auto text: systematics_text_DB) SF_file_DB << text << std::endl;
  SF_file_DB.close();

  systematics_text_DB.clear();
  systematics_text_DB.push_back("{ 2 JetEta JetPt 0 None ScaleFactor }");

  for (unsigned int i = 0; i < SF_final.size(); i++) {
    double eta = eta_bin_all_center[i];
    double eta_err = eta_bin_all_error[i];
    double SF = SF_final[i];
    double SF_err = SF_final_error[i];
    std::string text1 = Form("%.3f %.3f %d %d 11 %.4f %.4f %.4f",  (eta-eta_err),  (eta+eta_err), 0, int(PT_trigger_max), SF, SF-SF_err, SF+SF_err);
    std::string text2 = Form("%.3f %.3f %d %d 11 %.4f %.4f %.4f", -(eta+eta_err), -(eta-eta_err), 0, int(PT_trigger_max), SF, SF-SF_err, SF+SF_err);
    text1 += Form(" %.4f %.4f", SF-SF_final_error_stat[i], SF+SF_final_error_stat[i]);
    text2 += Form(" %.4f %.4f", SF-SF_final_error_stat[i], SF+SF_final_error_stat[i]);
    text1 += Form(" %.4f %.4f %.4f %.4f %.4f %.4f", SF+SF_sys["JEC_down"].at(i), SF+SF_sys["JEC_up"].at(i), SF-SF_sys["gaustails_0.95"].at(i), SF+SF_sys["gaustails_0.95"].at(i), SF-SF_sys["rest"].at(i), SF+SF_sys["rest"].at(i));
    text2 += Form(" %.4f %.4f %.4f %.4f %.4f %.4f", SF+SF_sys["JEC_down"].at(i), SF+SF_sys["JEC_up"].at(i), SF-SF_sys["gaustails_0.95"].at(i), SF+SF_sys["gaustails_0.95"].at(i), SF-SF_sys["rest"].at(i), SF+SF_sys["rest"].at(i));
    systematics_text_DB.push_back(text1);
    systematics_text_DB.insert(systematics_text_DB.begin()+1,text2);
  }
  if ((systematics_text_DB.size()-1)%2!=0) throw runtime_error("In plot_SF_systematics.cxx.");

  ofstream SF_file_DB_UncertaintiesSplit;
  SF_file_DB_UncertaintiesSplit.open (path+"standard/"+QCD_DATA+"SF_final_DB_UncertaintiesSplit.txt");

  for (auto text: systematics_text_DB) SF_file_DB_UncertaintiesSplit << text << std::endl;

  SF_file_DB_UncertaintiesSplit.close();


  for (VecTS vec: {systematics_name_all,systematics_name_split}) {
    for (TString sysName: vec) {
      VecD sys_SM(&(systematics_SM[sysName][0]), &(systematics_SM[sysName][0]) + systematics_SM[sysName].size());
      VecD sys_FE(&(systematics_FE[sysName][0]), &(systematics_FE[sysName][0]) + systematics_FE[sysName].size());
      for (size_t i = 0; i < sys_SM.size(); i++) sys_SM[i] /=  SF_SM[i];
      for (size_t i = 0; i < sys_FE.size(); i++) sys_FE[i] /=  SF_FE[i];

      VecD dummy_SM(eta_bin_SM_center.size(),0.001); VecD dummy_FE(eta_bin_FE_center.size(),0.001);
      TGraphErrors* gr_SM = new TGraphErrors(eta_bin_SM_center.size(), &(eta_bin_SM_center[0]), &(sys_SM[0]), &(eta_bin_SM_error[0]), &(dummy_SM[0]));
      TGraphErrors* gr_FE = new TGraphErrors(eta_bin_FE_center.size(), &(eta_bin_FE_center[0]), &(sys_FE[0]), &(eta_bin_FE_error[0]), &(dummy_FE[0]));
      double ymax = 0.2, ymin=-0.2;
      TCanvas* canv_sys = tdrCanvas(sysName, eta_bins_all[0]-0.1, eta_bins_all[eta_bins_all.size()-1]+0.1, ymin, ymax, "#eta", "SF_{nominal} - SF_{"+sysName+"} [\%]");
      TLegend *leg_sys = tdrLeg(0.64,0.67,0.79,0.92, 0.040, 42, kBlack);
      tdrHeader(leg_sys,"", 12);
      tdrDraw(gr_SM, "", kFullDotLarge, kRed+1, kSolid, kRed+1, 3005, kRed+1);
      tdrDraw(gr_FE, "", kFullDotLarge, kBlue-4, kSolid, kBlue-4, 3005, kBlue-4);
      leg_sys->AddEntry(gr_SM,  "SM","l");
      leg_sys->AddEntry(gr_FE,  "FE","l");
      leg_sys->Draw("same");
      canv_sys->Print(path+"standard/"+QCD_DATA+"sys_"+sysName+".pdf","pdf");

    }
  }

  Plot_Uncertainties("SM", eta_bins_SM, eta_bin_SM_center, SF_SM, eta_bin_SM_error, stat_SM, total_sys_SM, systematics_SM, total_error_SM, path+"standard/"+QCD_DATA, SF_FE);
  Plot_Uncertainties("FE", eta_bins_FE, eta_bin_FE_center, SF_FE, eta_bin_FE_error, stat_FE, total_sys_FE, systematics_FE, total_error_FE, path+"standard/"+QCD_DATA, SF_SM);


  TCanvas* canv_SF_final = tdrCanvas("SF_final", eta_bins_all.at(0)-0.1, eta_bins_all.at(eta_bins_all.size()-1)+0.5, 0., 3.0, "#eta", "JER SF");
  canv_SF_final->SetTickx(0);
  canv_SF_final->SetTicky(0);
  TLegend *leg_final = tdrLeg(0.57,0.67,0.79,0.92, 0.035, 42, kBlack);
  tdrHeader(leg_final,"", 12);

  VecD etaSummer16_25nsV1 = {0, 0.522, 0.783, 1.131, 1.305, 1.740, 1.930, 2.043, 2.322, 2.5, 2.853, 2.964, 3.139, 5.191};
  VecDD jerSummer16_25nsV1 = {{1.1595,0.0645},{1.1948,0.0652},{1.1464,0.0632},{1.1609,0.1025},{1.1278,0.0986},{1.1000,0.1079},{1.1426,0.1214},{1.1512,0.1140},{1.2963,0.2371},{1.3418,0.2091},{1.7788,0.2008},{1.1869,0.1243},{1.1922,0.1488}};

  VecD etaFall17_V3 = {0, 0.522, 0.783, 1.131, 1.305, 1.740, 1.930, 2.043, 2.322, 2.5, 2.853, 2.964, 3.139, 5.191};
  VecDD jerFall17_V3 = {{1.1432,0.0222},{1.1815,0.0484},{1.0989,0.0456},{1.1137,0.1397},{1.1307,0.1470},{1.1600,0.0976},{1.2393,0.1909},{1.2604,0.1501},{1.4085,0.2020},{1.9909,0.5684},{2.2923,0.3743},{1.2696,0.1089},{1.1542,0.1524}};

  VecD etaAutumn18_V4 = {0, 0.522, 0.783, 1.131, 1.305, 1.740, 1.930, 2.043, 2.322, 2.5, 2.853, 2.964, 3.139, 5.191};
  VecDD jerAutumn18_V4_RunD = {{1.1588,0.0392},{1.1504,0.0728},{1.1253,0.0343},{1.1217,0.0826},{1.1069,0.0890},{1.0916,0.0430},{1.0977,0.0951},{1.1177,0.0721},{1.4494,0.1502},{2.3588,0.6411},{2.2520,0.3542},{1.1759,0.0540},{1.0777,0.0542}};
  VecDD jerAutumn18_V4_RunABC = {{1.1677,0.0477},{1.1475,0.0520},{1.1029,0.0310},{1.0781,0.0950},{1.1006,0.0846},{1.1019,0.0386},{1.0459,0.1578},{1.1612,0.0646},{1.2299,0.1087},{1.6736,0.3792},{1.7292,0.2007},{1.2257,0.0452},{1.0733,0.0676}};
  VecDD jerAutumn18_V4_RunABCD = {{1.1545,0.0308},{1.1481,0.0515},{1.0998,0.0386},{1.0929,0.0856},{1.1093,0.0718},{1.1005,0.0515},{1.0603,0.1301},{1.1287,0.0531},{1.3397,0.1147},{2.0325,0.5361},{2.0567,0.3060},{1.1868,0.0376},{1.0922,0.0489}};


  std::map<TString, TGraphErrors*> map_gr;

  map_gr["Summer16_25nsV1"] = CreateTGraphSF(etaSummer16_25nsV1, jerSummer16_25nsV1);
  map_gr["Fall17_V3"] = CreateTGraphSF(etaFall17_V3, jerFall17_V3);
  map_gr["Autumn18_V4_RunD"]    = CreateTGraphSF(etaAutumn18_V4, jerAutumn18_V4_RunD);
  map_gr["Autumn18_V4_RunABC"]  = CreateTGraphSF(etaAutumn18_V4, jerAutumn18_V4_RunABC);
  map_gr["Autumn18_V4_RunABCD"] = CreateTGraphSF(etaAutumn18_V4, jerAutumn18_V4_RunABCD);
  map_gr["SFAutumn18_V7"+DATA] = CreateTGraphSF(path_+"MergeL2Res/Autumn18_V17_save/AK4CHS/standard/QCDHT/"+DATA+"/");
  map_gr["SFAutumn18_V8"+DATA] = CreateTGraphSF(path_+"MergeL2Res/Autumn18_V19/AK4CHS/standard/QCDHT/"+DATA+"/");
  map_gr["SFAutumn18_V8_AK4Puppi"+DATA] = CreateTGraphSF(path_+"MergeL2Res/Autumn18_V19/AK4Puppi/standard/QCDHT/"+DATA+"/");
  // map_gr["SFAutumn18_V8_AK8Puppi"+DATA] = CreateTGraphSF(path_+"MergeL2Res/Autumn18_V19/AK8Puppi/standard/QCDHT/"+DATA+"/");

  TGraphErrors* gr_final = new TGraphErrors(SF_final.size(), &(eta_bin_all_center[0]), &SF_final[0], &(eta_bin_all_error[0]), &SF_final_error[0]); //tot

  // tdrDraw(map_gr["Summer16_25nsV1"], "P5", kFullDotLarge, kRed+1, kSolid, kRed+1, 3004, kRed+1);
  // tdrDraw(map_gr["Fall17_V3"], "P5", kFullDotLarge, kOrange-1, kSolid, kOrange-1, 3004, kOrange-1);
  // tdrDraw(map_gr["SFAutumn18_V7"+DATA], "P5", kFullDotLarge, kRed+1, kSolid, kRed+1, 3005, kRed+1);
  tdrDraw(map_gr["SFAutumn18_V8"+DATA], "P5", kFullDotLarge, kGreen-1, kSolid, kGreen-1, 3005, kGreen-1);
  tdrDraw(map_gr["SFAutumn18_V8_AK4Puppi"+DATA], "P5", kFullDotLarge, kRed+1, kSolid, kRed+1, 3005, kRed+1);
  tdrDraw(gr_final, "P5", kFullDotLarge, kBlue-4, kSolid, kBlue-4, 3005, kBlue-4);
  // leg_final->AddEntry(map_gr["Summer16_25nsV1"], "Summer16_25nsV1","f");
  // leg_final->AddEntry(map_gr["Fall17_V3"], "Fall17_V3","f");
  // leg_final->AddEntry(map_gr["SFAutumn18_V7"+DATA],"JEC_V17_"+DATA,"f");
  leg_final->AddEntry(map_gr["SFAutumn18_V8"+DATA],"JEC_V19_AK4CHS_"+DATA,"f");
  leg_final->AddEntry(map_gr["SFAutumn18_V8_AK4Puppi"+DATA],"JEC_V19_AK4Puppi_"+DATA,"f");
  leg_final->AddEntry(gr_final, TITLE_NAME,"f");
  leg_final->Draw("same");

  canv_SF_final->Print(path+"standard/"+QCD_DATA+"SF_final.pdf","pdf");

  return true;

}



void plot_SF_systematics() {

  TString path ;

  VecTS studies;
  studies.push_back("MergeL2Res");

  VecTS JECs;
  // JECs.push_back("Autumn18_V17");
  JECs.push_back("Autumn18_V19");

  VecTS JETs;
  JETs.push_back("AK4CHS");
  JETs.push_back("AK4Puppi");
  JETs.push_back("AK8Puppi");

  VecTS QCDS;
  QCDS.push_back("QCDHT");

  VecTS DATAS;
  DATAS.push_back("RunABC");
  DATAS.push_back("RunD");
  DATAS.push_back("RunABCD");



  for(TString study : studies){
    for(TString JEC : JECs){
      for(TString JET : JETs){
        path = path_+study+"/"+JEC+"/"+JET+"/";
        if (!gSystem->AccessPathName(path)) {
          std::cout << path << '\n';
          for(TString QCD : QCDS){
            for(TString DATA : DATAS){
              TString QCD_DATA = QCD+"/"+DATA+"/";
              TString TITLE_NAME = "JEC"; TITLE_NAME+= JEC(JEC.Index("_"),JEC.Length())+"_"+JET+"_"+DATA;
              std::cout << "start: " << QCD_DATA << " " << TITLE_NAME << '\n';
              plot_SF_systematics_(path, QCD_DATA,DATA,TITLE_NAME);
              std::cout << "end: " << QCD_DATA << '\n';
              sleep(5);
            }
          }
        }
      }
    }
  }
}
