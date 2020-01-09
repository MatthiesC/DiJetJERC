#define MySelector_cxx
// The class definition in MySelector.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// Root > T->Process("MySelector.C")
// Root > T->Process("MySelector.C","some options")
// Root > T->Process("MySelector.C+")
//

#include <iostream>
#include <algorithm>
#include <vector>
#include <TH2.h>
#include <TH3.h>
#include <TStyle.h>
#include <TLorentzVector.h>
#include <TRandom3.h>
#include "MySelector.h"
#include "/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/UHH2/DiJetJERC/include/constants.h"

#define FILL_HISTOS(region,method)                                                                      \
if (TMath::Abs(weight/asy)>5*1e06) continue;                                                            \
asymmetries_##region.at(r).at(k).at(m)->Fill( asy , weight);                                            \
asymmetries_pt_##region.at(r).at(k).at(m)->Fill( pt_ave, weight);                                       \
asymmetries_rho_##region.at(r).at(k).at(m)->Fill( rho, weight);                                         \
asymmetries_pt3_##region.at(r).at(k).at(m)->Fill( jet3_pt, weight);                                     \
if ( m == AlphaBins-1 ) {                                                                               \
  h_JetAvePt_##method->Fill( pt_ave, weight);                                                           \
  h_Jet1Pt_##method->Fill( barreljet_pt, weight);                                                       \
  h_Jet2Pt_##method->Fill( probejet_pt, weight);                                                        \
  h_Jet3Pt_##method->Fill( jet3_pt, weight);                                                            \
}                                                                                                       \


#define SELECT_ETA_ALPHA_BIN(region,method,cond1,cond2)     \
if (cond1 || cond2) {                                       \
  h_alpha_sel->Fill(alpha, 1);                              \
  for ( int m = 0 ; m < AlphaBins ; m++ ) {                 \
    if ( alpha < Alpha_bins[m] ) {                          \
      if (dofill) nevents_central[k][r+shift][m] +=1;       \
      else nevents_HF[k][r+shift][m] +=1;                   \
      double asy = asymmetry;                               \
      FILL_HISTOS(region,method)                            \
      if ( excl_bin ) break;                                \
    }                                                       \
  }                                                         \
  alpha_spectrum_##region.at(r).at(k)->Fill(alpha, weight); \
}                                                           \

#define WRITE_HISTOS(region)                                    \
for( int m = 0; m < EtaBins_##region; m++ ) {                   \
  f->cd();                                                      \
  asymmetries_##region.at(m).at(p).at(r)->Write();              \
  asymmetries_pt_##region.at(m).at(p).at(r)->Write();           \
  f1->cd();                                                     \
  asymmetries_rho_##region.at(m).at(p).at(r)->Write();          \
  asymmetries_pt3_##region.at(m).at(p).at(r)->Write();          \
  f_alpha->cd();                                                \
  alpha_spectrum_##region.at(m).at(p)->Write();                 \
}                                                               \

void MakeHistograms(std::vector< std::vector< std::vector< TH1F* > > > &asymmetries, std::vector< std::vector< std::vector< TH1F* > > > &asymmetries_pt, std::vector< std::vector< std::vector< TH1F* > > > &asymmetries_rho, std::vector< std::vector< std::vector< TH1F* > > > &asymmetries_pt3, std::vector< std::vector< TH1F* > > &alpha_spectrum, TString text, TString extraText, int etaBins, int ptBins, int AlphaBins, int etaShift, int ptShift, int alphaShift) {
  for( int m = etaShift; m < etaBins+etaShift; m++ ) {
    std::vector< std::vector< TH1F* > > temp2, temp2pt, temp2rho, temp2pt3;
    std::vector< TH1F* > alpha_temp2;
    for( int p = 0; p < ptBins; p++ ) {
      std::vector< TH1F* > temp1, temp1pt, temp1rho, temp1pt3;
      TString name_alpha = "alpha"; name_alpha += extraText; name_alpha += "_eta"; name_alpha += m+1; name_alpha += "_pt"; name_alpha += p+1;
      TH1F *h1_alpha = new TH1F( name_alpha, name_alpha, 80, 0., 0.8); h1_alpha->SetXTitle("Alpha"); h1_alpha->SetYTitle("a.u.");  h1_alpha->Sumw2();  alpha_temp2.push_back(h1_alpha);
      for( int r = 0; r < AlphaBins; r++ ) {
        TString name     = text;        name     += extraText; name     += "_eta"; name     += m+1; name     += "_pt"; name     += p+1; name     += "_alpha"; name     += r+1;
        TString name_pt  = text+"pt";   name_pt  += extraText; name_pt  += "_eta"; name_pt  += m+1; name_pt  += "_pt"; name_pt  += p+1; name_pt  += "_alpha"; name_pt  += r+1;
        TString name_rho = text+"rho";  name_rho += extraText; name_rho += "_eta"; name_rho += m+1; name_rho += "_pt"; name_rho += p+1; name_rho += "_alpha"; name_rho += r+1;
        TString name_pt3 = text+"pt3";  name_pt3 += extraText; name_pt3 += "_eta"; name_pt3 += m+1; name_pt3 += "_pt"; name_pt3 += p+1; name_pt3 += "_alpha"; name_pt3 += r+1;
        TH1F *h1 = new TH1F(name,     name,     160,-0.8, 0.8);   h1->SetXTitle("Asymmetry"); h1->SetYTitle("a.u.");  h1->Sumw2();  temp1.push_back(h1);
        TH1F *h2 = new TH1F(name_pt,  name_pt,  50,   0,  1500);  h2->SetXTitle("Pt[GeV]");   h2->SetYTitle("a.u.");  h2->Sumw2();  temp1pt.push_back(h2);
        TH1F *h3 = new TH1F(name_rho, name_rho, 100,  0,  100);   h3->SetXTitle("rho");       h3->SetYTitle("a.u.");  h3->Sumw2();  temp1rho.push_back(h3);
        TH1F *h4 = new TH1F(name_pt3, name_pt3, 50,   0,  1500);  h4->SetXTitle("Pt[GeV]");   h4->SetYTitle("a.u.");  h4->Sumw2();  temp1pt3.push_back(h4);
      }
      temp2.push_back(temp1); temp2pt.push_back(temp1pt); temp2rho.push_back(temp1rho);  temp2pt3.push_back(temp1pt3);
    }
    asymmetries.push_back(temp2); asymmetries_pt.push_back(temp2pt); asymmetries_rho.push_back(temp2rho); asymmetries_pt3.push_back(temp2pt3);
    alpha_spectrum.push_back(alpha_temp2);
  }
}

void MySelector::BuildEvent() {
}

void MySelector::Begin(TTree * /*tree*/) {
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).

  TString option = GetOption();

  TotalEvents = 0;
}

void MySelector::SlaveBegin(TTree * /*tree*/) {

  TString option = GetOption();

  EtaBins_SM            = std::count_if(eta_bins_JER, eta_bins_JER+n_eta_bins_JER, [](double i) { return i<eta_cut; });
  EtaBins_SM_control    = std::count_if(eta_bins_JER, eta_bins_JER+n_eta_bins_JER, [](double i) { return i>eta_cut; });
  EtaBins_FE_reference  = std::count_if(eta_bins_JER, eta_bins_JER+n_eta_bins_JER, [](double i) { return i<s_eta_barr;});
  EtaBins_FE_control    = std::count_if(eta_bins_JER, eta_bins_JER+n_eta_bins_JER, [](double i) { return (i>=s_eta_barr)&&(i<eta_cut);});
  EtaBins_FE            = std::count_if(eta_bins_JER, eta_bins_JER+n_eta_bins_JER, [](double i) { return i>eta_cut; });

  etaShift_SM           = 0;
  etaShift_SM_control   = EtaBins_SM;
  etaShift_FE_reference = 0;
  etaShift_FE_control   = EtaBins_FE_reference;
  etaShift_FE           = EtaBins_FE_reference + EtaBins_FE_control;



  for (size_t i = etaShift_SM;            i < etaShift_SM           + EtaBins_SM            + 1; i++)  Eta_bins_SM.push_back(eta_bins_JER[i]);
  for (size_t i = etaShift_SM_control;    i < etaShift_SM_control   + EtaBins_SM_control    + 1; i++)  Eta_bins_SM_control.push_back(eta_bins_JER[i]);
  for (size_t i = etaShift_FE_reference;  i < etaShift_FE_reference + EtaBins_FE_reference  + 1; i++)  Eta_bins_FE_reference.push_back(eta_bins_JER[i]);
  for (size_t i = etaShift_FE_control;    i < etaShift_FE_control   + EtaBins_FE_control    + 1; i++)  Eta_bins_FE_control.push_back(eta_bins_JER[i]);
  for (size_t i = etaShift_FE;            i < etaShift_FE           + EtaBins_FE            + 1; i++)  Eta_bins_FE.push_back(eta_bins_JER[i]);

  std::string triggerName = "DiJet";
  if (isAK8) triggerName = "SingleJet";
  std::string name_pt_bin = triggerName+"_central_";
  if (isAK8) name_pt_bin += "AK8_";
  name_pt_bin += year+"_ptbins";
  PtBins_Central = pt_trigger_thr.at(name_pt_bin).size();
  for (auto &pt: pt_trigger_thr.at(name_pt_bin)) Pt_bins_Central.push_back(pt);
  name_pt_bin = triggerName+"_forward_";
  if (isAK8) name_pt_bin += "AK8_";
  name_pt_bin += year+"_ptbins";
  PtBins_HF = pt_trigger_thr.at(name_pt_bin).size();
  for (auto &pt: pt_trigger_thr.at(name_pt_bin)) Pt_bins_HF.push_back(pt);
  Pt_bins_Central.push_back(1500);
  Pt_bins_HF.push_back(1500);

  AlphaBins = 6;
  for (auto &alpha: {0.05,0.10,0.15,0.20,0.25,0.30}) Alpha_bins.push_back(alpha);

  h_JetAvePt_SM = new TH1F("JetAvePt" ,   "Inclusive Jet Ave Pt",   50, 0., 2000);  h_JetAvePt_SM->SetXTitle("Pt_{ave}[GeV]");  h_JetAvePt_SM->SetYTitle("a.u."); h_JetAvePt_SM->Sumw2();
  h_Jet1Pt_SM   = new TH1F("Jet1Pt",      "Inclusive Jet 1 Pt",     50, 0., 2000);  h_Jet1Pt_SM->SetXTitle("Pt_1[GeV]");        h_Jet1Pt_SM->SetYTitle("a.u.");   h_Jet1Pt_SM->Sumw2();
  h_Jet2Pt_SM   = new TH1F("Jet2Pt",      "Inclusive Jet 2 Pt",     50, 0., 2000);  h_Jet2Pt_SM->SetXTitle("Pt_2[GeV]");        h_Jet2Pt_SM->SetYTitle("a.u.");   h_Jet2Pt_SM->Sumw2();
  h_Jet3Pt_SM   = new TH1F("Jet3Pt",      "Inclusive Jet 3 Pt",     50, 0., 2000);  h_Jet3Pt_SM->SetXTitle("Pt_3[GeV]");        h_Jet3Pt_SM->SetYTitle("a.u.");   h_Jet3Pt_SM->Sumw2();
  h_JetAvePt_FE = new TH1F("FEJetAvePt",  "Inclusive FEJet Ave Pt", 50, 0., 2000);  h_JetAvePt_FE->SetXTitle("Pt_{ave}[GeV]");  h_JetAvePt_FE->SetYTitle("a.u."); h_JetAvePt_FE->Sumw2();
  h_Jet1Pt_FE   = new TH1F("FEJet1Pt",    "Inclusive FEJet 1 Pt",   50, 0., 2000);  h_Jet1Pt_FE->SetXTitle("Pt_1[GeV]");        h_Jet1Pt_FE->SetYTitle("a.u.");   h_Jet1Pt_FE->Sumw2();
  h_Jet2Pt_FE   = new TH1F("FEJet2Pt",    "Inclusive FEJet 2 Pt",   50, 0., 2000);  h_Jet2Pt_FE->SetXTitle("Pt_2[GeV]");        h_Jet2Pt_FE->SetYTitle("a.u.");   h_Jet2Pt_FE->Sumw2();
  h_Jet3Pt_FE   = new TH1F("FEJet3Pt",    "Inclusive FEJet 3 Pt",   50, 0., 2000);  h_Jet3Pt_FE->SetXTitle("Pt_3[GeV]");        h_Jet3Pt_FE->SetYTitle("a.u.");   h_Jet3Pt_FE->Sumw2();
  h_PU          = new TH1F("PileUp",      "PU distribution",        60, 0., 60);    h_PU->SetXTitle("PU");                      h_PU->SetYTitle("a.u.");          h_PU->Sumw2();
  h_alpha_raw   = new TH1F("Alpha_raw",   "#alpha before selection",80, 0., 0.8 );  h_alpha_raw->SetXTitle("#alpha_raw");       h_alpha_raw->SetYTitle("a.u.");   h_alpha_raw->Sumw2();
  h_alpha_sel   = new TH1F("Alpha",       "#alpha after selection", 80, 0., 0.8);   h_alpha_sel->SetXTitle("#alpha");           h_alpha_sel->SetYTitle("a.u.");   h_alpha_sel->Sumw2();

  if (true) {
    std::cout << "\nConstructor: " << PtBins_Central << " " << PtBins_HF;
    std::cout << "\nPt_bins_Central: ";         for (size_t i = 0; i < Pt_bins_Central.size(); i++) std::cout << " " << Pt_bins_Central[i];
    std::cout << "\nPt_bins_HF: ";              for (size_t i = 0; i < Pt_bins_HF.size(); i++) std::cout << " " << Pt_bins_HF[i];
    std::cout << "\nAlpha_bins: ";              for (size_t i = 0; i < Alpha_bins.size(); i++) std::cout << " " << Alpha_bins[i];
    std::cout << "\nEta_bins_SM : ";            for (size_t i = 0; i < Eta_bins_SM.size(); i++) std::cout << " " << Eta_bins_SM[i];
    std::cout << "\nEta_bins_SM_control : ";    for (size_t i = 0; i < Eta_bins_SM_control.size(); i++) std::cout << " " << Eta_bins_SM_control[i];
    std::cout << "\nEta_bins_FE_reference : ";  for (size_t i = 0; i < Eta_bins_FE_reference.size(); i++) std::cout << " " << Eta_bins_FE_reference[i];
    std::cout << "\nEta_bins_FE_control : ";    for (size_t i = 0; i < Eta_bins_FE_control.size(); i++) std::cout << " " << Eta_bins_FE_control[i];
    std::cout << "\nEta_bins_FE : ";            for (size_t i = 0; i < Eta_bins_FE.size(); i++) std::cout << " " << Eta_bins_FE[i];
    std::cout << "\n" << std::endl;
  }


  for ( int k = 0 ; k < PtBins_Central ; k++ ) {
    std::vector< std::vector< double > >  temp;
    for (int r = 0; r < 14; r++) {
      std::vector< double >  temp2;
      for (int m = 0; m < AlphaBins; m++) temp2.push_back(0);
      temp.push_back(temp2);
    }
    nevents_central.push_back(temp);
  }

  for ( int k = 0 ; k < PtBins_HF ; k++ ) {
    std::vector< std::vector< double > >  temp;
    for (int r = 0; r < 14; r++) {
      std::vector< double >  temp2;
      for (int m = 0; m < AlphaBins; m++) temp2.push_back(0);
      temp.push_back(temp2);
    }
    nevents_HF.push_back(temp);
  }

  // for ( int k = 0 ; k < PtBins_HF ; k++ ) nevents_HF.push_back(std::vector< std::vector< double > > (0));
  // for (size_t i = 0; i < nevents_central.size(); i++) std::cout << " " << nevents_central[i];
  // for (size_t i = 0; i < nevents_HF.size(); i++) std::cout << " " << nevents_HF[i];

  MakeHistograms(asymmetries_SM,            asymmetries_pt_SM,            asymmetries_rho_SM,            asymmetries_pt3_SM,            alpha_spectrum_SM,            "asymm", "_SM",            EtaBins_SM,            PtBins_Central, AlphaBins, etaShift_SM,            0, 0);
  MakeHistograms(asymmetries_SM_control,    asymmetries_pt_SM_control,    asymmetries_rho_SM_control,    asymmetries_pt3_SM_control,    alpha_spectrum_SM_control,    "asymm", "_SM_control",    EtaBins_SM_control,    PtBins_HF,      AlphaBins, etaShift_SM_control,    0, 0);
  MakeHistograms(asymmetries_FE_reference,  asymmetries_pt_FE_reference,  asymmetries_rho_FE_reference,  asymmetries_pt3_FE_reference,  alpha_spectrum_FE_reference,  "asymm", "_FE_reference",  EtaBins_FE_reference,  PtBins_Central, AlphaBins, etaShift_FE_reference,  0, 0);
  MakeHistograms(asymmetries_FE_control,    asymmetries_pt_FE_control,    asymmetries_rho_FE_control,    asymmetries_pt3_FE_control,    alpha_spectrum_FE_control,    "asymm", "_FE_control",    EtaBins_FE_control,    PtBins_Central, AlphaBins, etaShift_FE_control,    0, 0);
  MakeHistograms(asymmetries_FE,            asymmetries_pt_FE,            asymmetries_rho_FE,            asymmetries_pt3_FE,            alpha_spectrum_FE,            "asymm", "_FE",            EtaBins_FE,            PtBins_HF,      AlphaBins, etaShift_FE,            0, 0);

}

bool MySelector::Process(Long64_t entry) {

  ++TotalEvents;
  if ( TotalEvents%100000 == 0 ) {  std::cout << "\t\tAnalyzing event #" << TotalEvents << std::endl; }
  GetEntry(entry);
  BuildEvent();

  bool cond1, cond2;

  // double jet_thr = jet_threshold_min;
  double jet_thr=15;
  double alpha_raw = alpha_;
  double alpha;

  // Below I choose what kind of asymmetries I want to study! excl_bin = true for exclusive bins
  bool excl_bin = false; // inclusive

  if (njet<2) return kTRUE;
  if (barreljet_pt < jet_thr && probejet_pt < jet_thr) return kTRUE;
  if ( TMath::Abs(TVector2::Phi_mpi_pi((probejet_phi - barreljet_phi))) < s_delta_phi ) { std::cout << "Jets are not back to back" << std::endl; return kTRUE;}

  if ( jet3_pt > jet_thr ) alpha = TMath::Abs(alpha_raw);
  else alpha = (njet <3) ? 0. : 1. ;

  h_PU->Fill(nPU, 1);
  h_alpha_raw->Fill(alpha_raw, 1);

  bool dofill; int shift;
  bool isHF = probejet_eta>eta_cut? true : false;
  // bool isHF = TMath::Abs(probejet_eta)>eta_cut? true : false;

  if (!isHF) {
    dofill=true;
    for ( int k = 0 ; k < PtBins_Central ; k++ ) {
      if ((pt_ave > Pt_bins_Central[k]) && (pt_ave < Pt_bins_Central[k+1]) ) {
        for ( int r = 0 ; r < EtaBins_SM ; r++ ) {
          if (!is_JER_SM) continue;
          cond1 = (TMath::Abs(barreljet_eta) > Eta_bins_SM[r] && TMath::Abs(barreljet_eta) < Eta_bins_SM[r+1] && TMath::Abs(probejet_eta) > Eta_bins_SM[r] && TMath::Abs(probejet_eta) < Eta_bins_SM[r+1]);
          cond2 = false;
          // std::cout << "Eta_bins_SM " << Eta_bins_SM[r] << "-" << Eta_bins_SM[r+1] << " " << cond1 << " " << cond2 << std::endl;
          shift = 0;
          SELECT_ETA_ALPHA_BIN(SM,SM,cond1,cond2)
        }
        for ( int r = 0 ; r < EtaBins_FE_reference ; r++ ) {
          if (is_JER_SM) continue;
          cond1 = (TMath::Abs(barreljet_eta)> 0. && TMath::Abs(barreljet_eta)< s_eta_barr && TMath::Abs(probejet_eta) > Eta_bins_FE_reference[r] && TMath::Abs(probejet_eta) < Eta_bins_FE_reference[r+1]);
          cond2 = (TMath::Abs(probejet_eta) > 0. && TMath::Abs(probejet_eta) < s_eta_barr && TMath::Abs(barreljet_eta)> Eta_bins_FE_reference[r] && TMath::Abs(barreljet_eta)< Eta_bins_FE_reference[r+1]);
          shift = 0;
          // std::cout << "EtaBins_FE_reference " << Eta_bins_FE_reference[r] << "-" << Eta_bins_FE_reference[r+1] << " " << cond1 << " " << cond2 << std::endl;
          SELECT_ETA_ALPHA_BIN(FE_reference,FE,cond1,cond2)
        }
        for ( int r = 0 ; r < EtaBins_FE_control ; r++ ) {
          if (is_JER_SM) continue;
          cond1 = (TMath::Abs(barreljet_eta)> 0. && TMath::Abs(barreljet_eta)< s_eta_barr && TMath::Abs(probejet_eta) > Eta_bins_FE_control[r] && TMath::Abs(probejet_eta) < Eta_bins_FE_control[r+1]);
          cond2 = (TMath::Abs(probejet_eta) > 0. && TMath::Abs(probejet_eta) < s_eta_barr && TMath::Abs(barreljet_eta)> Eta_bins_FE_control[r] && TMath::Abs(barreljet_eta)< Eta_bins_FE_control[r+1]);
          shift = etaShift_FE_control;
          // std::cout << "EtaBins_FE_control " << Eta_bins_FE_control[r] << "-" << Eta_bins_FE_control[r+1] << " " << cond1 << " " << cond2 << std::endl;
          SELECT_ETA_ALPHA_BIN(FE_control,FE,cond1,cond2)
        }
        break;
      }
    }
  } else {
    dofill=false;
    for ( int k = 0 ; k < PtBins_HF ; k++ ) {
      if ((pt_ave > Pt_bins_HF[k]) && (pt_ave < Pt_bins_HF[k+1]) ) {
        for ( int r = 0 ; r < EtaBins_SM_control; r++ ) {
          if (!is_JER_SM) continue;
          cond1 = (TMath::Abs(barreljet_eta) > Eta_bins_SM_control[r] && TMath::Abs(barreljet_eta) < Eta_bins_SM_control[r+1] && TMath::Abs(probejet_eta) > Eta_bins_SM_control[r] && TMath::Abs(probejet_eta) < Eta_bins_SM_control[r+1]);
          cond2 = false;
          shift = etaShift_SM_control;
          // std::cout << "Eta_bins_SM_control " << Eta_bins_SM_control[r] << "-" << Eta_bins_SM_control[r+1] << " " << cond1 << " " << cond2 << std::endl;
          SELECT_ETA_ALPHA_BIN(SM_control,SM,cond1,cond2)
        }
        for ( int r = 0 ; r < EtaBins_FE ; r++ ) {
          if (is_JER_SM) continue;
          cond1 = (TMath::Abs(barreljet_eta)> 0. && TMath::Abs(barreljet_eta)< s_eta_barr && TMath::Abs(probejet_eta) > Eta_bins_FE[r] && TMath::Abs(probejet_eta) < Eta_bins_FE[r+1]);
          cond2 = (TMath::Abs(probejet_eta) > 0. && TMath::Abs(probejet_eta) < s_eta_barr && TMath::Abs(barreljet_eta)> Eta_bins_FE[r] && TMath::Abs(barreljet_eta)< Eta_bins_FE[r+1]);
          // std::cout << "Eta_bins_FE " << Eta_bins_FE[r] << "-" << Eta_bins_FE[r+1] << " " << cond1 << " " << cond2 << std::endl;
          shift = etaShift_FE;
          SELECT_ETA_ALPHA_BIN(FE,FE,cond1,cond2)
        }
        break;
      }
    }
  }
  return kTRUE;
}

void MySelector::SlaveTerminate() {
  // The SlaveTerminate() function is called after all entries or objects
  // have been processed. When running with PROOF SlaveTerminate() is called
  // on each slave server.

  std::cout <<"\t\tAnalyzed events #" <<  TotalEvents << std::endl;

  std::ofstream mytxtfile;
  mytxtfile.open (outdir+"counts.txt");
  mytxtfile << "Analyzed events #" <<  TotalEvents << "\n";
  mytxtfile.close();

  TFile *fpt = new TFile(outdir+"pt_data_incl_full.root","RECREATE"); ;
  fpt->cd();
  h_PU->Write();
  h_alpha_raw->Write();
  h_alpha_sel->Write();
  h_JetAvePt_FE->Write();
  h_Jet1Pt_FE->Write();
  h_Jet2Pt_FE->Write();
  h_Jet3Pt_FE->Write();
  h_JetAvePt_SM->Write();
  h_Jet1Pt_SM->Write();
  h_Jet2Pt_SM->Write();
  h_Jet3Pt_SM->Write();
  fpt->Close();

  TFile *f  = new TFile(outdir+"histograms_data_incl_full.root","RECREATE");
  TFile *f1 = new TFile(outdir+"histograms_data_incl_full_control.root","RECREATE");
  TFile *f_alpha = new TFile(outdir+"alpha_spectrum.root","RECREATE"); ;

  for( int r = 0; r < AlphaBins; r++ ) {
    for( int p = 0; p < PtBins_Central; p++ ) {
      WRITE_HISTOS(SM)
      WRITE_HISTOS(FE_reference)
      WRITE_HISTOS(FE_control)
    }
    for( int p = 0; p < PtBins_HF; p++ ) {
      WRITE_HISTOS(SM_control)
      WRITE_HISTOS(FE)
    }
  }

  f->Close();
  f1->Close();
  f_alpha->Close();


  std::vector<TH2F*> h_nevents_central, h_nevents_HF;

  std::vector<double> Pt_bins_Central_D(Pt_bins_Central.begin(), Pt_bins_Central.end());
  std::vector<double> Pt_bins_HF_D(Pt_bins_HF.begin(), Pt_bins_HF.end());

  for (int m = 0; m < 6; m++){
    h_nevents_central.push_back(new TH2F(("central_"+std::to_string(m)).c_str(),("central_"+std::to_string(m)).c_str(),n_eta_bins_JER-1,&eta_bins_JER[0], Pt_bins_Central_D.size()-1,&Pt_bins_Central_D[0]));
    h_nevents_HF.push_back(new TH2F(("HF_"+std::to_string(m)).c_str(),("HF_"+std::to_string(m)).c_str(),n_eta_bins_JER-1,&eta_bins_JER[0], Pt_bins_HF_D.size()-1,&Pt_bins_HF_D[0]));
  }
  std::cout << "Pt_bins_Central: " << nevents_central.size() << std::endl;
  for (size_t i = 0; i < nevents_central.size(); i++) std::cout << "\t" << Pt_bins_Central_D[i];
  std::cout << std::endl;

  for (int r = 0; r < 14; r++) {
    for (int m = 0; m < 6; m++){
      std::cout << r << " " << m << " ";
      for ( int k = 0 ; k < nevents_central.size() ; k++ ) {
        std::cout << "\t" << nevents_central[k][r][m];
        h_nevents_central[m]->SetBinContent(h_nevents_central[m]->GetXaxis()->FindBin(eta_bins_JER[r]), h_nevents_central[m]->GetYaxis()->FindBin(Pt_bins_Central_D.at(k)), nevents_central[k][r][m]);
      }
      std::cout << std::endl;
    } std::cout << std::endl;
  }

  std::cout << "Pt_bins_HF: " << nevents_HF.size() << std::endl;
  for (size_t i = 0; i < nevents_HF.size(); i++) std::cout << "\t" << Pt_bins_HF_D[i];
  std::cout << std::endl;

  for (int r = 0; r < 14; r++) {
    for (int m = 0; m < 6; m++){
      std::cout << r << " " << m << " ";
      for ( int k = 0 ; k < nevents_HF.size() ; k++ ) {
        std::cout << "\t" << nevents_HF[k][r][m];
        h_nevents_HF[m]->SetBinContent(h_nevents_HF[m]->GetXaxis()->FindBin(eta_bins_JER[r]), h_nevents_HF[m]->GetYaxis()->FindBin(Pt_bins_HF_D.at(k)), nevents_HF[k][r][m]);
      }
      std::cout << std::endl;
    }std::cout << std::endl;
  }

  TFile *f_nevents  = new TFile(outdir+"histograms_nevents.root","RECREATE");
  for (int m = 0; m < 6; m++){
    h_nevents_central[m]->Write();
    h_nevents_HF[m]->Write();
  }
  f_nevents->Close();

}

void MySelector::Terminate() {
  // The Terminate() function is the last function to be called during
  // a query. It always runs on the client, it can be used to present
  // the results graphically or save the results to file.

}
