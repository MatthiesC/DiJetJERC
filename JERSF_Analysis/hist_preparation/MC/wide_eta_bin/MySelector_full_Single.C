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
#include <TH2.h>
#include <TH3.h>
#include <TStyle.h>
#include <TLorentzVector.h>
#include <TRandom3.h>
#include "MySelector.h"
#include "/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/UHH2/DiJetJERC/include/constants.h"

#define FILL_HISTOS(region,method)                                                                        \
asymmetries_##region.at(r).at(k).at(m) -> Fill( asy , weight );                                           \
asymmetries_pt_##region.at(r).at(k).at(m) -> Fill( pt_ave, weight );                                      \
asymmetries_rho_##region.at(r).at(k).at(m) -> Fill( rho, weight );                                        \
asymmetries_pt3_##region.at(r).at(k).at(m) -> Fill( jet3_pt, weight );                                    \
dR_##region.at(r).at(k).at(m) -> Fill( Delta_R_radiation_barrel, Delta_R_radiation_probe, weight );       \
dR_probe_##region.at(r).at(k).at(m) -> Fill( Delta_R_radiation_probe, asy, weight );                      \
dR_barrel_##region.at(r).at(k).at(m) -> Fill( Delta_R_radiation_barrel, asy, weight );                    \
dR3_##region.at(r).at(k).at(m) -> Fill( asy, Delta_R_radiation_barrel, Delta_R_radiation_probe, weight ); \
if (DR1 < s_delta_R) MC_Truth_asymmetries_##region.at(r).at(k).at(m) -> Fill( Resp1, weight );            \
if (DR2 < s_delta_R) MC_Truth_asymmetries_##region.at(r).at(k).at(m) -> Fill( Resp2, weight );            \
if ( m == AlphaBins-1 ) {                                                                                 \
  h_JetAvePt_##method -> Fill( pt_ave, weight );                                                          \
  h_Jet1Pt_##method -> Fill( jet1_pt, weight );                                                           \
  h_Jet2Pt_##method -> Fill( jet2_pt, weight );                                                           \
  h_Jet3Pt_##method -> Fill( jet3_pt, weight );                                                           \
  h_rho_##method -> Fill( rho, weight );                                                                  \
}                                                                                                         \


#define FILL_GEN_HISTOS(region)                                                                                             \
if (TMath::Abs(gen_asy) < 5) {                                                                                              \
  gen_asymmetries_##region.at(r).at(k).at(m) -> Fill( gen_asy , weight );                                                   \
  gen_asymmetries_pt_##region.at(r).at(k).at(m) -> Fill( gen_pt_ave, weight );                                              \
  gen_asymmetries_pt3_##region.at(r).at(k).at(m) -> Fill( genjet3_pt, weight );                                             \
  gen_dR_##region.at(r).at(k).at(m) -> Fill( gen_Delta_R_radiation_barrel, gen_Delta_R_radiation_probe, weight );           \
  gen_dR_probe_##region.at(r).at(k).at(m) -> Fill( gen_Delta_R_radiation_probe, gen_asy, weight );                          \
  gen_dR_barrel_##region.at(r).at(k).at(m) -> Fill( gen_Delta_R_radiation_barrel, gen_asy, weight );                        \
  gen_dR3_##region.at(r).at(k).at(m) -> Fill( gen_asy, gen_Delta_R_radiation_barrel, gen_Delta_R_radiation_probe, weight ); \
}                                                                                                                           \


#define SELECT_ETA_ALPHA_BIN(region,method,cond1,cond2,cond3)                                                                                                 \
if (cond1 || cond2) {                                                                                                                                         \
  for ( int m = 0 ; m < AlphaBins ; m++ ) {                                                                                                                   \
    if ( alpha < Alpha_bins[m] ) {                                                                                                                            \
      double asy = asymmetry;                                                                                                                                 \
      double Delta_R_radiation_barrel = TMath::Sqrt(TMath::Power(barreljet_eta- jet3_eta,2) + TMath::Power(TVector2::Phi_mpi_pi(barreljet_phi- jet3_phi),2)); \
      double Delta_R_radiation_probe  = TMath::Sqrt(TMath::Power(probejet_eta - jet3_eta,2) + TMath::Power(TVector2::Phi_mpi_pi(probejet_phi - jet3_phi),2)); \
      if (cond3) {                                                                                                                                            \
        asy = - asymmetry;                                                                                                                                    \
        std::swap(Delta_R_radiation_barrel, Delta_R_radiation_probe);                                                                                         \
      }                                                                                                                                                       \
      FILL_HISTOS(region,method)                                                                                                                              \
      if ( excl_bin ) break;                                                                                                                                  \
    }                                                                                                                                                         \
  }                                                                                                                                                           \
  alpha_spectrum_##region.at(r).at(k) -> Fill(alpha, weight);                                                                                                 \
  alpha2D_##region.at(r).at(k) -> Fill(alpha, alphaGen, weight);                                                                                              \
}                                                                                                                                                             \


#define SELECT_ETA_ALPHA_BIN_2(region,method,cond1,cond2,cond3)                                                                                               \
if (cond1 || cond2) {                                                                                                                                         \
  for ( int m = 0 ; m < AlphaBins ; m++ ) {                                                                                                                   \
    if ( alpha < Alpha_bins[m] ) {                                                                                                                            \
      double asy = asymmetry;                                                                                                                                 \
      double Delta_R_radiation_barrel = TMath::Sqrt(TMath::Power(barreljet_eta- jet3_eta,2) + TMath::Power(TVector2::Phi_mpi_pi(barreljet_phi- jet3_phi),2)); \
      double Delta_R_radiation_probe  = TMath::Sqrt(TMath::Power(probejet_eta - jet3_eta,2) + TMath::Power(TVector2::Phi_mpi_pi(probejet_phi - jet3_phi),2)); \
      double eta_probe = TMath::Abs(TMath::Power(barreljet_eta- jet3_eta,2));                                                                                 \
      double eta_barrel= TMath::Abs(TMath::Power(probejet_eta - jet3_eta,2));                                                                                 \
      double phi_probe = TMath::Abs(TMath::Power(TVector2::Phi_mpi_pi(barreljet_phi- jet3_phi),2));                                                           \
      double phi_barrel= TMath::Abs(TMath::Power(TVector2::Phi_mpi_pi(probejet_phi - jet3_phi),2));                                                           \
      if (cond3) {                                                                                                                                            \
        asy = - asymmetry;                                                                                                                                    \
        std::swap(Delta_R_radiation_barrel, Delta_R_radiation_probe);                                                                                         \
        std::swap(eta_probe, eta_barrel);                                                                                                                     \
        std::swap(phi_probe, phi_barrel);                                                                                                                     \
      }                                                                                                                                                       \
      FILL_HISTOS(region,method)                                                                                                                              \
      for (unsigned int i = 0; i < dR_bins.size()-1; i++) {                                                                                                   \
        if (Delta_R_radiation_probe > dR_bins[i] && Delta_R_radiation_probe < dR_bins[i+1]) { asy_dR_barrel_FE.at(r).at(k).at(m).at(i)-> Fill( asy, Delta_R_radiation_barrel, weight); }\
        if (Delta_R_radiation_barrel> dR_bins[i] && Delta_R_radiation_barrel< dR_bins[i+1]) { asy_dR_probe_FE.at(r).at(k).at(m).at(i) -> Fill( asy, Delta_R_radiation_probe, weight ); }\
      }                                                                                                                                                       \
      if ( excl_bin ) break;                                                                                                                                  \
    }                                                                                                                                                         \
  }                                                                                                                                                           \
  alpha_spectrum_##region.at(r).at(k) -> Fill(alpha, weight);                                                                                                 \
  alpha2D_##region.at(r).at(k) -> Fill(alpha, alphaGen, weight);                                                                                              \
}                                                                                                                                                             \

#define SELECT_ETA_ALPHA_BIN_GEN(region,cond1,cond2,cond3)                                                                                                    \
if (cond1 || cond2) {                                                                                                                                         \
  for ( int m = 0 ; m < AlphaBins ; m++ ) {                                                                                                                   \
    if ( alphaGen < Alpha_bins[m] ) {                                                                                                                         \
      double gen_asy = gen_asymmetry;                                                                                                                         \
      double gen_Delta_R_radiation_barrel= TMath::Sqrt(TMath::Power(barrelgenjet_eta-genjet3_eta,2)+TMath::Power(TVector2::Phi_mpi_pi(barrelgenjet_phi-genjet3_phi),2));\
      double gen_Delta_R_radiation_probe = TMath::Sqrt(TMath::Power(probegenjet_eta-genjet3_eta,2)+TMath::Power(TVector2::Phi_mpi_pi(probegenjet_phi-genjet3_phi),2));\
      if (cond3) {                                                                                                                                            \
        gen_asy = - gen_asymmetry;                                                                                                                            \
        std::swap(gen_Delta_R_radiation_barrel, gen_Delta_R_radiation_probe);                                                                                 \
      }                                                                                                                                                       \
      FILL_GEN_HISTOS(region)                                                                                                                                 \
      if ( excl_bin ) break;                                                                                                                                  \
    }                                                                                                                                                         \
  }                                                                                                                                                           \
}                                                                                                                                                             \

#define WRITE_HISTOS(region)                                  \
for( int m = 0; m < EtaBins_##region; m++ ) {                 \
  f->cd();                                                    \
  asymmetries_##region.at(m).at(p).at(r) -> Write();          \
  asymmetries_pt_##region.at(m).at(p).at(r) -> Write();       \
  gen_asymmetries_##region.at(m).at(p).at(r) -> Write();      \
  gen_asymmetries_pt_##region.at(m).at(p).at(r) -> Write();   \
  MC_Truth_asymmetries_##region.at(m).at(p).at(r) -> Write(); \
  f1->cd();                                                   \
  asymmetries_rho_##region.at(m).at(p).at(r) -> Write();      \
  asymmetries_pt3_##region.at(m).at(p).at(r) -> Write();      \
  gen_asymmetries_pt3_##region.at(m).at(p).at(r) -> Write();  \
  f2->cd();                                                   \
  dR_##region.at(m).at(p).at(r) -> Write();                   \
  gen_dR_##region.at(m).at(p).at(r) -> Write();               \
  dR_probe_##region.at(m).at(p).at(r) -> Write();             \
  gen_dR_probe_##region.at(m).at(p).at(r) -> Write();         \
  dR_barrel_##region.at(m).at(p).at(r) -> Write();            \
  gen_dR_barrel_##region.at(m).at(p).at(r) -> Write();        \
  dR3_##region.at(m).at(p).at(r) -> Write();                  \
  gen_dR3_##region.at(m).at(p).at(r) -> Write();              \
  f_alpha->cd();                                              \
  alpha_spectrum_##region.at(m).at(p) -> Write();             \
  alpha2D_##region.at(m).at(p) -> Write();                    \
}                                                             \

void MakeHistograms(std::vector< std::vector< std::vector< TH1F* > > > &asymmetries, std::vector< std::vector< std::vector< TH1F* > > > &asymmetries_pt, std::vector< std::vector< std::vector< TH1F* > > > &asymmetries_rho, std::vector< std::vector< std::vector< TH1F* > > > &asymmetries_pt3, std::vector< std::vector< TH1F* > > &alpha_spectrum, std::vector< std::vector< std::vector< TH1F* > > > &gen_asymmetries, std::vector< std::vector< std::vector< TH1F* > > > &gen_asymmetries_pt, std::vector< std::vector< std::vector< TH1F* > > > &gen_asymmetries_rho, std::vector< std::vector< std::vector< TH1F* > > > &gen_asymmetries_pt3, std::vector< std::vector< std::vector< TH1F* > > > &MC_Truth_asymmetries, std::vector< std::vector< std::vector< TH2F* > > > &dR, std::vector< std::vector< std::vector< TH2F* > > > &gen_dR, std::vector< std::vector< std::vector< TH2F* > > > &dR_probe, std::vector< std::vector< std::vector< TH2F* > > > &gen_dR_probe, std::vector< std::vector< std::vector< TH2F* > > > &dR_barrel, std::vector< std::vector< std::vector< TH2F* > > > &gen_dR_barrel, std::vector< std::vector< std::vector< TH3F* > > > &dR3, std::vector< std::vector< std::vector< TH3F* > > > &gen_dR3, std::vector< std::vector< TH2F* > > &alpha2D, TString text, TString extraText, int etaBins, int ptBins, int AlphaBins, int etaShift, int ptShift, int alphaShift) {
  for( int m = etaShift; m < etaBins+etaShift; m++ ) {
    std::vector< std::vector< TH1F* > > temp2, temp2pt, temp2rho, temp2pt3, gen_temp2rho, gen_temp2pt3, gen_temp2, gen_temp2pt, temp2_MCTruth;
    std::vector< std::vector< TH2F* > > temp2_dR, gen_temp2_dR, temp2_dR_probe, gen_temp2_dR_probe, temp2_dR_barrel, gen_temp2_dR_barrel;
    std::vector< TH1F* > alpha_temp2;
    std::vector< std::vector< TH3F* > > temp2_dR3, gen_temp2_dR3 ;
    std::vector< TH2F* > alpha2D_temp2;
    for( int p = 0; p < ptBins; p++ ) {
      std::vector< TH1F* > temp1, temp1pt, temp1rho, temp1pt3, gen_temp1rho, gen_temp1pt3, gen_temp1, gen_temp1pt, temp1_MCTruth;
      std::vector< TH2F* > temp1_dR, gen_temp1_dR, temp1_dR_probe, gen_temp1_dR_probe, temp1_dR_barrel, gen_temp1_dR_barrel;
      std::vector< TH3F* > temp1_dR3, gen_temp1_dR3 ;
      TString name_alpha   = "alpha";   name_alpha  += extraText; name_alpha  += "_eta"; name_alpha  += m+1; name_alpha  += "_pt"; name_alpha  += p+1;
      TString name_alpha2  = "alpha2D"; name_alpha2 += extraText; name_alpha2 += "_eta"; name_alpha2 += m+1; name_alpha2 += "_pt"; name_alpha2 += p+1;
      TH1F *h1_alpha = new TH1F( name_alpha, name_alpha, 80, 0., 0.8 );
      TH2F* h2_alpha = new TH2F(name_alpha2, name_alpha2, AlphaBins, 0., 0.3, AlphaBins, 0., 0.3);
      h1_alpha ->GetYaxis()->SetTitle("a.u.");    h1_alpha ->GetXaxis()->SetTitle("Alpha");
      h1_alpha -> Sumw2(); alpha_temp2.push_back(h1_alpha);
      h2_alpha ->GetXaxis()->SetTitle("alpha_reco"); h2_alpha ->GetYaxis()->SetTitle("alpha_gen");
      h2_alpha -> Sumw2(); alpha2D_temp2.push_back(h2_alpha);

      for( int r = 0; r < AlphaBins; r++ ) {
        TString name     = text;        name     += extraText; name     += "_eta"; name     += m+1; name     += "_pt"; name     += p+1; name     += "_alpha"; name     += r+1;
        TString name_pt  = text+"pt";   name_pt  += extraText; name_pt  += "_eta"; name_pt  += m+1; name_pt  += "_pt"; name_pt  += p+1; name_pt  += "_alpha"; name_pt  += r+1;
        TString name_rho = text+"rho";  name_rho += extraText; name_rho += "_eta"; name_rho += m+1; name_rho += "_pt"; name_rho += p+1; name_rho += "_alpha"; name_rho += r+1;
        TString name_pt3 = text+"pt3";  name_pt3 += extraText; name_pt3 += "_eta"; name_pt3 += m+1; name_pt3 += "_pt"; name_pt3 += p+1; name_pt3 += "_alpha"; name_pt3 += r+1;

        TString gen_name     = "gen_"+text;        gen_name     += extraText; gen_name     += "_eta"; gen_name     += m+1; gen_name     += "_pt"; gen_name     += p+1; gen_name     += "_alpha"; gen_name     += r+1;
        TString gen_name_pt  = "gen_"+text+"pt";   gen_name_pt  += extraText; gen_name_pt  += "_eta"; gen_name_pt  += m+1; gen_name_pt  += "_pt"; gen_name_pt  += p+1; gen_name_pt  += "_alpha"; gen_name_pt  += r+1;
        TString gen_name_rho = "gen_"+text+"rho";  gen_name_rho += extraText; gen_name_rho += "_eta"; gen_name_rho += m+1; gen_name_rho += "_pt"; gen_name_rho += p+1; gen_name_rho += "_alpha"; gen_name_rho += r+1;
        TString gen_name_pt3 = "gen_"+text+"pt3";  gen_name_pt3 += extraText; gen_name_pt3 += "_eta"; gen_name_pt3 += m+1; gen_name_pt3 += "_pt"; gen_name_pt3 += p+1; gen_name_pt3 += "_alpha"; gen_name_pt3 += r+1;
        TString name_MCTruth = "mctruth";          name_MCTruth += extraText; name_MCTruth += "_eta"; name_MCTruth += m+1; name_MCTruth += "_pt"; name_MCTruth += p+1; name_MCTruth += "_alpha"; name_MCTruth += r+1;

        TString name_dR             = "dR";            name_dR             += extraText; name_dR             += "_eta"; name_dR            += m+1; name_dR             += "_pt"; name_dR             += p+1; name_dR             += "_alpha"; name_dR            += r+1;
        TString gen_name_dR         = "gen_dR";        gen_name_dR         += extraText; gen_name_dR         += "_eta"; gen_name_dR        += m+1; gen_name_dR         += "_pt"; gen_name_dR         += p+1; gen_name_dR         += "_alpha"; gen_name_dR        += r+1;
        TString name_dR_probe       = "dR_probe";      name_dR_probe       += extraText; name_dR_probe       += "_eta"; name_dR_probe      += m+1; name_dR_probe       += "_pt"; name_dR_probe       += p+1; name_dR_probe       += "_alpha"; name_dR_probe      += r+1;
        TString gen_name_dR_probe   = "gen_dR_probe";  gen_name_dR_probe   += extraText; gen_name_dR_probe   += "_eta"; gen_name_dR_probe  += m+1; gen_name_dR_probe   += "_pt"; gen_name_dR_probe   += p+1; gen_name_dR_probe   += "_alpha"; gen_name_dR_probe  += r+1;
        TString name_dR_barrel      = "dR_barrel";     name_dR_barrel     += extraText; name_dR_barrel      += "_eta"; name_dR_barrel     += m+1; name_dR_barrel      += "_pt"; name_dR_barrel      += p+1; name_dR_barrel      += "_alpha"; name_dR_barrel     += r+1;
        TString gen_name_dR_barrel  = "gen_dR_barrel"; gen_name_dR_barrel += extraText; gen_name_dR_barrel  += "_eta"; gen_name_dR_barrel += m+1; gen_name_dR_barrel  += "_pt"; gen_name_dR_barrel  += p+1; gen_name_dR_barrel  += "_alpha"; gen_name_dR_barrel += r+1;
        TString name_dR3            = "dR3";           name_dR3           += extraText; name_dR3             += "_eta"; name_dR3           += m+1; name_dR3            += "_pt"; name_dR3            += p+1; name_dR3            += "_alpha"; name_dR3           += r+1;
        TString gen_name_dR3        = "gen_dR3";       gen_name_dR3       += extraText; gen_name_dR3        += "_eta"; gen_name_dR3       += m+1; gen_name_dR3        += "_pt"; gen_name_dR3        += p+1; gen_name_dR3        += "_alpha"; gen_name_dR3       += r+1;

        TH1F *h1 = new TH1F( name, name, 160, -0.8, 0.8 );
        h1 ->GetYaxis()->SetTitle("a.u.");    h1 ->GetXaxis()->SetTitle("Asymmetry");
        h1 -> Sumw2(); temp1.push_back(h1);
        TH1F *h2 = new TH1F( name_pt, name_pt, 50, 0, 1500 );
        h2 ->GetYaxis()->SetTitle("a.u.");    h2 ->GetXaxis()->SetTitle("Pt[GeV]");
        h2 -> Sumw2(); temp1pt.push_back(h2);
        TH1F *h3 = new TH1F( name_rho, name_rho, 100, 0, 100 );
        h3 ->GetYaxis()->SetTitle("a.u.");    h3 ->GetXaxis()->SetTitle("rho");
        h3 -> Sumw2(); temp1rho.push_back(h3);
        TH1F *h4 = new TH1F( name_pt3, name_pt3, 50, 0, 1500 );
        h4 ->GetYaxis()->SetTitle("a.u.");    h4 ->GetXaxis()->SetTitle("Pt[GeV]");
        h4 -> Sumw2(); temp1pt3.push_back(h4);
        TH1F *gen_h1 = new TH1F( gen_name, gen_name, 160, -0.8, 0.8 );
        gen_h1 ->GetYaxis()->SetTitle("a.u.");    gen_h1 ->GetXaxis()->SetTitle("Asymmetry");
        gen_h1 -> Sumw2(); gen_temp1.push_back(gen_h1);
        TH1F *gen_h2 = new TH1F( gen_name_pt, gen_name_pt, 50, 0, 1500 );
        gen_h2 ->GetYaxis()->SetTitle("a.u.");    gen_h2 ->GetXaxis()->SetTitle("Pt[GeV]");
        gen_h2 -> Sumw2(); gen_temp1pt.push_back(gen_h2);
        TH1F *gen_h3 = new TH1F( gen_name_rho, gen_name_rho, 100, 0, 100 );
        gen_h3 ->GetYaxis()->SetTitle("a.u.");    gen_h3 ->GetXaxis()->SetTitle("rho");
        gen_h3 -> Sumw2(); gen_temp1rho.push_back(gen_h3);
        TH1F *gen_h4 = new TH1F( gen_name_pt3, gen_name_pt3, 50, 0, 1500 );
        gen_h4 ->GetYaxis()->SetTitle("a.u.");    gen_h4 ->GetXaxis()->SetTitle("Pt[GeV]");
        gen_h4 -> Sumw2(); gen_temp1pt3.push_back(gen_h4);
        TH1F *h1_MCTruth = new TH1F( name_MCTruth, name_MCTruth, 200, 0, 2.0 );
        h1_MCTruth ->GetYaxis()->SetTitle("a.u.");    h1_MCTruth ->GetXaxis()->SetTitle("Response");
        h1_MCTruth -> Sumw2(); temp1_MCTruth.push_back(h1_MCTruth);
        TH2F *h2_dR = new TH2F( name_dR, name_dR, 60, 0, 6.0, 60, 0, 6.0 );
        h2_dR ->GetXaxis()->SetTitle("#Delta R (jet_{barrel}, jet_{3})");    h2_dR ->GetYaxis()->SetTitle("#Delta R (jet_{probe}, jet_{3})");
        h2_dR -> Sumw2(); temp1_dR.push_back(h2_dR);
        TH2F *gen_h2_dR = new TH2F( gen_name_dR, gen_name_dR, 60, 0, 6.0, 60, 0, 6.0 );
        gen_h2_dR ->GetXaxis()->SetTitle("#Delta R (jet_{barrel}, jet_{3})");    gen_h2_dR ->GetYaxis()->SetTitle("#Delta R (jet_{probe}, jet_{3})");
        gen_h2_dR -> Sumw2(); gen_temp1_dR.push_back(gen_h2_dR);
        TH2F *h2_dR_probe = new TH2F( name_dR_probe, name_dR_probe, 60, 0, 6.0, 160, -0.8, 0.8 );
        h2_dR_probe ->GetXaxis()->SetTitle("#Delta R (jet_{probe}, jet_{3})");    h2_dR_probe ->GetYaxis()->SetTitle("Asymmetry");
        h2_dR_probe -> Sumw2(); temp1_dR_probe.push_back(h2_dR_probe);
        TH2F *gen_h2_dR_probe = new TH2F( gen_name_dR_probe, gen_name_dR_probe, 60, 0, 6.0, 160, -0.8, 0.8 );
        gen_h2_dR_probe ->GetXaxis()->SetTitle("#Delta R (jet_{probe}, jet_{3})");    gen_h2_dR_probe ->GetYaxis()->SetTitle("Asymmetry");
        gen_h2_dR_probe -> Sumw2(); gen_temp1_dR_probe.push_back(gen_h2_dR_probe);
        TH2F *h2_dR_barrel = new TH2F( name_dR_barrel, name_dR_barrel, 60, 0, 6.0, 160, -0.8, 0.8 );
        h2_dR_barrel ->GetXaxis()->SetTitle("#Delta R (jet_{barrel}, jet_{3})");    h2_dR_barrel ->GetYaxis()->SetTitle("Asymmetry");
        h2_dR_barrel -> Sumw2(); temp1_dR_barrel.push_back(h2_dR_barrel);
        TH2F *gen_h2_dR_barrel = new TH2F( gen_name_dR_barrel, gen_name_dR_barrel, 60, 0, 6.0, 160, -0.8, 0.8 );
        gen_h2_dR_barrel ->GetXaxis()->SetTitle("#Delta R (jet_{barrel}, jet_{3})");    gen_h2_dR_barrel ->GetYaxis()->SetTitle("Asymmetry");
        gen_h2_dR_barrel -> Sumw2(); gen_temp1_dR_barrel.push_back(gen_h2_dR_barrel);
        TH3F *h3_dR3 = new TH3F( name_dR3, name_dR3, 160, -0.8, 0.8, 60, 0, 6.0, 60, 0, 6.0 );
        h3_dR3 ->GetXaxis()->SetTitle("Asymmetry");    h3_dR3 ->GetYaxis()->SetTitle("#Delta R (jet_{barrel}, jet_{3})");    h3_dR3 ->GetZaxis()->SetTitle("#Delta R (jet_{probe}, jet_{3})");
        h3_dR3 -> Sumw2(); temp1_dR3.push_back(h3_dR3);
        TH3F *gen_h3_dR3 = new TH3F( gen_name_dR3, gen_name_dR3, 160, -0.8, 0.8, 60, 0, 6.0, 60, 0, 6.0 );
        gen_h3_dR3 ->GetXaxis()->SetTitle("Asymmetry");    gen_h3_dR3 ->GetYaxis()->SetTitle("#Delta R (jet_{barrel}, jet_{3})");    gen_h3_dR3 ->GetZaxis()->SetTitle("#Delta R (jet_{probe}, jet_{3})");
        gen_h3_dR3 -> Sumw2(); gen_temp1_dR3.push_back(gen_h3_dR3);
      }
      temp2.push_back(temp1); temp2pt.push_back(temp1pt); temp2rho.push_back(temp1rho);  temp2pt3.push_back(temp1pt3);
      gen_temp2.push_back(gen_temp1); gen_temp2pt.push_back(gen_temp1pt); gen_temp2rho.push_back(gen_temp1rho); gen_temp2pt3.push_back(gen_temp1pt3);
      temp2_MCTruth.push_back(temp1_MCTruth); temp2_dR.push_back(temp1_dR); gen_temp2_dR.push_back(gen_temp1_dR); temp2_dR_probe.push_back(temp1_dR_probe); gen_temp2_dR_probe.push_back(gen_temp1_dR_probe); temp2_dR_barrel.push_back(temp1_dR_barrel); gen_temp2_dR_barrel.push_back(gen_temp1_dR_barrel);
      temp2_dR3.push_back(temp1_dR3); gen_temp2_dR3.push_back(gen_temp1_dR3);
    }
    asymmetries.push_back(temp2); asymmetries_pt.push_back(temp2pt); asymmetries_rho.push_back(temp2rho); asymmetries_pt3.push_back(temp2pt3);
    gen_asymmetries.push_back(gen_temp2); gen_asymmetries_pt.push_back(gen_temp2pt); gen_asymmetries_rho.push_back(gen_temp2rho); gen_asymmetries_pt3.push_back(gen_temp2pt3);
    MC_Truth_asymmetries.push_back(temp2_MCTruth); dR.push_back(temp2_dR); gen_dR.push_back(gen_temp2_dR); dR_probe.push_back(temp2_dR_probe); gen_dR_probe.push_back(gen_temp2_dR_probe); dR_barrel.push_back(temp2_dR_barrel); gen_dR_barrel.push_back(gen_temp2_dR_barrel);
    dR3.push_back(temp2_dR3); gen_dR3.push_back(gen_temp2_dR3);
    alpha_spectrum.push_back(alpha_temp2); alpha2D.push_back(alpha2D_temp2);
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
  unmatchegGenJets = 0;
}

void MySelector::SlaveBegin(TTree * /*tree*/) {
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).

  TString option = GetOption();

  h_JetAvePt_SM = new TH1F( "JetAvePt" , "Inclusive Jet Ave Pt" , 50 , 0 , 2000 );
  h_JetAvePt_SM -> SetXTitle( "Pt_{ave}[GeV]" );
  h_JetAvePt_SM -> Sumw2();
  // histograms.push_back(h_JetAvePt_SM);

  h_Jet1Pt_SM = new TH1F( "Jet1Pt" , "Inclusive Jet 1 Pt" , 50 , 0 , 2000 );
  h_Jet1Pt_SM -> SetXTitle( "Pt_1[GeV]" );
  h_Jet1Pt_SM -> Sumw2();
  // histograms.push_back(h_Jet1Pt_SM);

  h_Jet2Pt_SM = new TH1F( "Jet2Pt" , "Inclusive Jet 2 Pt" , 50 , 0 , 2000 );
  h_Jet2Pt_SM -> SetXTitle( "Pt_2[GeV]" );
  h_Jet2Pt_SM -> Sumw2();
  // histograms.push_back(h_Jet2Pt_SM);

  h_Jet3Pt_SM = new TH1F( "Jet3Pt" , "Inclusive Jet 3 Pt" , 50 , 0 , 2000 );
  h_Jet3Pt_SM -> SetXTitle( "Pt_3[GeV]" );
  h_Jet3Pt_SM -> Sumw2();
  // histograms.push_back(h_Jet3Pt_SM);

  h_JetAvePt_FE = new TH1F( "FEJetAvePt" , "Inclusive FEJet Ave Pt" , 50 , 0 , 2000 );
  h_JetAvePt_FE -> SetXTitle( "Pt_{ave}[GeV]" );
  h_JetAvePt_FE -> Sumw2();
  // histograms.push_back(h_JetAvePt_FE);

  h_Jet1Pt_FE = new TH1F( "FEJet1Pt" , "Inclusive FEJet 1 Pt" , 50 , 0 , 2000 );
  h_Jet1Pt_FE -> SetXTitle( "Pt_1[GeV]" );
  h_Jet1Pt_FE -> Sumw2();
  histograms.push_back(h_Jet1Pt_FE);

  h_Jet2Pt_FE = new TH1F( "FEJet2Pt" , "Inclusive FEJet 2 Pt" , 50 , 0 , 2000 );
  h_Jet2Pt_FE -> SetXTitle( "Pt_2[GeV]" );
  h_Jet2Pt_FE -> Sumw2();
  histograms.push_back(h_Jet2Pt_FE);

  h_Jet3Pt_FE = new TH1F( "FEJet3Pt" , "Inclusive FEJet 3 Pt" , 50 , 0 , 2000 );
  h_Jet3Pt_FE -> SetXTitle( "Pt_3[GeV]" );
  h_Jet3Pt_FE -> Sumw2();
  // histograms.push_back(h_Jet3Pt_FE);

  h_PU = new TH1F( "PileUp" , "PU distribution" , 60 , 0 , 60 );
  h_PU -> SetXTitle( "PU" );
  h_PU -> Sumw2();
  // histograms.push_back(h_PU);

  h_rho_SM = new TH1F( "Rho" , "Rho distribution" , 60 , 0 , 30 );
  h_rho_SM -> SetXTitle( "Rho" );
  h_rho_SM -> Sumw2();
  histograms.push_back(h_rho_SM);

  h_rho_FE = new TH1F( "RhoFWD" , "RhoFWD distribution" , 60 , 0 , 30 );
  h_rho_FE -> SetXTitle( "RhoFWD" );
  h_rho_FE -> Sumw2();
  histograms.push_back(h_rho_FE);

  EtaBins_SM            = 10; // st method bins
  EtaBins_SM_control    =  3; // st method bins control
  EtaBins_FE_reference  =  3; // fe method bins reference
  EtaBins_FE_control    =  7; // fe method bins control
  EtaBins_FE            =  3; // fe method bins

  etaShift_SM           = 0;
  etaShift_SM_control   = EtaBins_SM;
  etaShift_FE_reference = 0;
  etaShift_FE_control   = EtaBins_FE_reference;
  etaShift_FE           = EtaBins_FE_reference + EtaBins_FE_control;

  PtBins_Central = n_pt_bins_Si;
  PtBins_HF = n_pt_bins_Si_HF;
  AlphaBins = 6;

  MakeHistograms(asymmetries_SM,             asymmetries_pt_SM,           asymmetries_rho_SM,           asymmetries_pt3_SM,           alpha_spectrum_SM,            gen_asymmetries_SM,           gen_asymmetries_pt_SM,            gen_asymmetries_rho_SM,           gen_asymmetries_pt3_SM,           MC_Truth_asymmetries_SM,            dR_SM,            gen_dR_SM,            dR_probe_SM,            gen_dR_probe_SM,            dR_barrel_SM,           gen_dR_barrel_SM,           dR3_SM,           gen_dR3_SM,           alpha2D_SM,           "asymm",  "_SM",            EtaBins_SM,           PtBins_Central,  AlphaBins,   etaShift_SM,            0, 0);
  MakeHistograms(asymmetries_SM_control,     asymmetries_pt_SM_control,   asymmetries_rho_SM_control,   asymmetries_pt3_SM_control,   alpha_spectrum_SM_control,    gen_asymmetries_SM_control,   gen_asymmetries_pt_SM_control,    gen_asymmetries_rho_SM_control,   gen_asymmetries_pt3_SM_control,   MC_Truth_asymmetries_SM_control,    dR_SM_control,    gen_dR_SM_control,    dR_probe_SM_control,    gen_dR_probe_SM_control,    dR_barrel_SM_control,   gen_dR_barrel_SM_control,   dR3_SM_control,   gen_dR3_SM_control,   alpha2D_SM_control,   "asymm",  "_SM_control",    EtaBins_SM_control,   PtBins_Central,  AlphaBins,   etaShift_SM_control,    0, 0);
  MakeHistograms(asymmetries_FE_reference,  asymmetries_pt_FE_reference,  asymmetries_rho_FE_reference, asymmetries_pt3_FE_reference, alpha_spectrum_FE_reference,  gen_asymmetries_FE_reference, gen_asymmetries_pt_FE_reference,  gen_asymmetries_rho_FE_reference, gen_asymmetries_pt3_FE_reference, MC_Truth_asymmetries_FE_reference,  dR_FE_reference,  gen_dR_FE_reference,  dR_probe_FE_reference,  gen_dR_probe_FE_reference,  dR_barrel_FE_reference, gen_dR_barrel_FE_reference, dR3_FE_reference, gen_dR3_FE_reference, alpha2D_FE_reference, "asymm",  "_FE_reference",  EtaBins_FE_reference, PtBins_Central, AlphaBins,    etaShift_FE_reference,  0, 0);
  MakeHistograms(asymmetries_FE_control,     asymmetries_pt_FE_control,   asymmetries_rho_FE_control,   asymmetries_pt3_FE_control,   alpha_spectrum_FE_control,    gen_asymmetries_FE_control,   gen_asymmetries_pt_FE_control,    gen_asymmetries_rho_FE_control,   gen_asymmetries_pt3_FE_control,   MC_Truth_asymmetries_FE_control,    dR_FE_control,    gen_dR_FE_control,    dR_probe_FE_control,    gen_dR_probe_FE_control,    dR_barrel_FE_control,   gen_dR_barrel_FE_control,   dR3_FE_control,   gen_dR3_FE_control,   alpha2D_FE_control,  "asymm",  "_FE_control",    EtaBins_FE_control,   PtBins_Central, AlphaBins,    etaShift_FE_control,    0, 0);
  MakeHistograms(asymmetries_FE,             asymmetries_pt_FE,           asymmetries_rho_FE,           asymmetries_pt3_FE,           alpha_spectrum_FE,            gen_asymmetries_FE,           gen_asymmetries_pt_FE,            gen_asymmetries_rho_FE,           gen_asymmetries_pt3_FE,           MC_Truth_asymmetries_FE,            dR_FE,            gen_dR_FE,            dR_probe_FE,            gen_dR_probe_FE,            dR_barrel_FE,           gen_dR_barrel_FE,           dR3_FE,           gen_dR3_FE,           alpha2D_FE,           "asymm",  "_FE",            EtaBins_FE,           PtBins_Central,  AlphaBins,   etaShift_FE,            0, 0);

  dR_bins.push_back(0.0); dR_bins.push_back(0.4); dR_bins.push_back(0.8); dR_bins.push_back(1.2); dR_bins.push_back(1.6);
  dR_bins.push_back(2.0); dR_bins.push_back(2.4); dR_bins.push_back(2.8); dR_bins.push_back(3.2); dR_bins.push_back(3.6);
  dR_bins.push_back(4.0); dR_bins.push_back(4.4); dR_bins.push_back(4.8); dR_bins.push_back(5.2); dR_bins.push_back(5.6); dR_bins.push_back(6.0);

  for( int m = 0; m < EtaBins_FE; m++ ) {
    std::vector< std::vector< std::vector< TH2F* > > > temp1_barrel, gen_temp1_barrel, temp1_probe, gen_temp1_probe;
    for( int p = 0; p < PtBins_HF; p++ ) {
      std::vector< std::vector< TH2F* > > temp2_barrel, gen_temp2_barrel, temp2_probe, gen_temp2_probe;
      for( int r = 0; r < AlphaBins; r++ ) {
        std::vector< TH2F* > temp3_barrel, gen_temp3_barrel, temp3_probe, gen_temp3_probe;
        for (unsigned int i = 0; i < dR_bins.size()-1; i++) {
          TString name_dR = "asy_dR_barrel_forward_"; name_dR += "probe"; name_dR += m+2+EtaBins_FE_control ; name_dR += "_pt"; name_dR += p+1; name_dR += "_alpha"; name_dR += r+1; name_dR += "_dR_probe"; name_dR += i;
          TH2F *h2 = new TH2F( name_dR, name_dR, 160, -0.8, 0.8, 60, 0, 6.0 );
          h2 ->GetXaxis()->SetTitle("Asymmetry");    h2 ->GetYaxis()->SetTitle("#Delta R (jet_{barrel}, jet_{3})");
          h2 -> Sumw2(); temp3_barrel.push_back(h2);
          name_dR = "gen_asy_dR_barrel_forward_"; name_dR += "probe"; name_dR += m+2+EtaBins_FE_control ; name_dR += "_pt"; name_dR += p+1; name_dR += "_alpha"; name_dR += r+1; name_dR += "_dR_probe"; name_dR += i;
          h2 = new TH2F( name_dR, name_dR, 160, -0.8, 0.8, 60, 0, 6.0 );
          h2 ->GetXaxis()->SetTitle("Asymmetry");    h2 ->GetYaxis()->SetTitle("#Delta R (jet_{barrel}, jet_{3})");
          h2 -> Sumw2(); gen_temp3_barrel.push_back(h2);
          name_dR = "asy_dR_probe_forward_"; name_dR += "probe"; name_dR += m+2+EtaBins_FE_control ; name_dR += "_pt"; name_dR += p+1; name_dR += "_alpha"; name_dR += r+1; name_dR += "_dR_barrel"; name_dR += i;
          h2 = new TH2F( name_dR, name_dR, 160, -0.8, 0.8, 60, 0, 6.0 );
          h2 ->GetXaxis()->SetTitle("Asymmetry");    h2 ->GetYaxis()->SetTitle("#Delta R (jet_{probe}, jet_{3})");
          h2 -> Sumw2(); temp3_probe.push_back(h2);
          name_dR = "gen_asy_dR_probe_forward_"; name_dR += "probe"; name_dR += m+2+EtaBins_FE_control ; name_dR += "_pt"; name_dR += p+1; name_dR += "_alpha"; name_dR += r+1; name_dR += "_dR_barrel"; name_dR += i;
          h2 = new TH2F( name_dR, name_dR, 160, -0.8, 0.8, 60, 0, 6.0 );
          h2 ->GetXaxis()->SetTitle("Asymmetry");    h2 ->GetYaxis()->SetTitle("#Delta R (jet_{probe}, jet_{3})");
          h2 -> Sumw2(); gen_temp3_probe.push_back(h2);
        }
        temp2_barrel.push_back(temp3_barrel); gen_temp2_barrel.push_back(gen_temp3_barrel); temp2_probe.push_back(temp3_probe); gen_temp2_probe.push_back(gen_temp3_probe);
      }
      temp1_barrel.push_back(temp2_barrel); gen_temp1_barrel.push_back(gen_temp2_barrel); temp1_probe.push_back(temp2_probe); gen_temp1_probe.push_back(gen_temp2_probe);
    }
    asy_dR_barrel_FE.push_back(temp1_barrel);
    gen_asy_dR_barrel_FE.push_back(gen_temp1_barrel);
    asy_dR_probe_FE.push_back(temp1_probe);
    gen_asy_dR_probe_FE.push_back(gen_temp1_probe);
  }

}

Bool_t MySelector::Process(Long64_t entry) {
  // The Process() function is called for each entry in the tree (or possibly
  // keyed object in the case of PROOF) to be processed. The entry argument
  // specifies which entry in the currently loaded tree is to be processed.
  // It can be passed to either MySelector::GetEntry() or TBranch::GetEntry()
  // to read either all or the required parts of the data. When processing
  // keyed objects with PROOF, the object is already loaded and is available
  // via the fObject pointer.
  //
  // This function should contain the "body" of the analysis. It can contain
  // simple or elaborate selection criteria, run algorithms on the data
  // of the event and typically fill histograms.
  //
  // The processing can be stopped by calling Abort().
  //
  // Use fStatus to set the return value of TTree::Process().
  //
  // The return value is currently not used.

  ++TotalEvents;
  if ( TotalEvents%100000 == 0 ) {  std::cout << "            Analyzing event #" << TotalEvents << std::endl; }

  GetEntry( entry );
  BuildEvent();

  if (weight <= 0 || weight > 50) {
    TString filename = fChain->GetDirectory()->GetPath();
    std::cout << "WARNING: weight was very small/large " << weight << std::endl;
    std::cout << "Filename is " << filename << std::endl;
    std::cout << "leading jet eta is " << probejet_eta << std::endl;
    weight = 0;
    return kFALSE;
  }

  //2017
  std::vector<double> Eta_bins_SM(            eta_bins + etaShift_SM,            eta_bins + etaShift_SM + EtaBins_SM + 1);
  std::vector<double> Eta_bins_SM_control(    eta_bins + etaShift_SM_control,    eta_bins + etaShift_SM_control + EtaBins_SM_control + 1);
  std::vector<double> Eta_bins_FE_reference(  eta_bins + etaShift_FE_reference,  eta_bins + etaShift_FE_reference + EtaBins_FE_reference + 1);
  std::vector<double> Eta_bins_FE_control(    eta_bins + etaShift_FE_control,    eta_bins + etaShift_FE_control + EtaBins_FE_control + 1);
  std::vector<double> Eta_bins_FE(            eta_bins + etaShift_FE,            eta_bins + etaShift_FE + EtaBins_FE + 1);

  std::vector<int> Pt_bins_Central(pt_bins_Si, pt_bins_Si + sizeof(pt_bins_Si)/sizeof(double));
  std::vector<int> Pt_bins_HF(pt_bins_Si_HF, pt_bins_Si_HF + sizeof(pt_bins_Si_HF)/sizeof(double));
  Pt_bins_Central.push_back(1500);
  Pt_bins_HF.push_back(1500);

  std::vector<double> Alpha_bins;
  Alpha_bins.push_back(0.05); Alpha_bins.push_back(0.1);  Alpha_bins.push_back(0.15); Alpha_bins.push_back(0.20); Alpha_bins.push_back(0.25); Alpha_bins.push_back(0.3);
  bool cond1, cond2, cond3;

  if (npuIT < 10 || npuIT > 75) {
    return kFALSE;
  }
  h_PU -> Fill( npuIT, 1 ); //weight );

  double gen_threshold = 10;
  double jet_threshold = 15;
  double alpha_raw = alpha_;

  double parallel, perpendicular, complete, alpha;
  double parallelGen, perpendicularGen, completeGen, alphaGen;

  // Below I choose what kind of asymmetries I want to study!
  //    bool excl_bin = true;  // exclusive
  bool excl_bin = false; // inclusive

  int flag1 = 0; // 0 -> complete_alpha
  // 1 -> parallel
  // 2 -> perpendicular

  if ( jet2_pt > jet_threshold &&  (njet > 1) ) {
    if ( jet3_pt > jet_threshold ) {
      complete =  alpha_raw; //2 * jet3_pt/( jet1_pt + jet2_pt );
      parallel = alpha_raw; //(2*Jets[2]*(Jets[0]-Jets[1]))/((Jets[0]-Jets[1]).Pt()*( jet1_pt + jet2_pt ));
      perpendicular = alpha_raw; //TMath::Sqrt( TMath::Power(complete, 2 ) - TMath::Power(parallel, 2) );
    } else {
      if ((njet <3)) {
        complete = 0. ;
        parallel = 0. ;
        perpendicular = 0. ;
      } else {
        complete = 1. ;
        parallel = 1. ;
        perpendicular = 1. ;
      }
    }
    if (flag1 == 0 ) alpha = TMath::Abs(complete);
    if (flag1 == 1 ) alpha = TMath::Abs(parallel);
    if (flag1 == 2 ) alpha = TMath::Abs(perpendicular);

    double DPhi1, DPhi2, DEta1, DEta2, Resp1, Resp2;
    double ProbeBarrelGen, ProbeProbeGen, BarrelBarrelGen, BarrelProbeGen;
    bool switch_genjet = false;
    bool esclude_jet=false;

    ProbeProbeGen   = TMath::Abs(TVector2::Phi_mpi_pi(probejet_phi - probegenjet_phi) );
    ProbeBarrelGen  = TMath::Abs(TVector2::Phi_mpi_pi(probejet_phi - barrelgenjet_phi));
    BarrelBarrelGen = TMath::Abs(TVector2::Phi_mpi_pi(barreljet_phi- barrelgenjet_phi));
    BarrelProbeGen  = TMath::Abs(TVector2::Phi_mpi_pi(barreljet_phi- probegenjet_phi) );

    if ( (ProbeProbeGen < ProbeBarrelGen) && (BarrelBarrelGen < BarrelProbeGen)) {
      // probe jet close to probe genjet && barrel jet close to barrel genjet
      DPhi1 = TMath::Abs(TVector2::Phi_mpi_pi(probejet_phi - probegenjet_phi) );
      DPhi2 = TMath::Abs(TVector2::Phi_mpi_pi(barreljet_phi- barrelgenjet_phi));
      DEta1 = TMath::Abs(probejet_eta - probegenjet_eta);
      DEta2 = TMath::Abs(barreljet_eta- barrelgenjet_eta);
      Resp1 = probejet_pt / probegenjet_pt;
      Resp2 = barreljet_pt/ barrelgenjet_pt;
    } else if ( (ProbeBarrelGen < ProbeProbeGen) && (BarrelProbeGen < BarrelBarrelGen) ) {
      // probe jet close to barrel genjet && barrel jet close to probe genjet
      DPhi1 = TMath::Abs(TVector2::Phi_mpi_pi(probejet_phi - barrelgenjet_phi));
      DPhi2 = TMath::Abs(TVector2::Phi_mpi_pi(barreljet_phi- probegenjet_phi));
      DEta1 = TMath::Abs(probejet_eta - barrelgenjet_eta);
      DEta2 = TMath::Abs(barreljet_eta- probegenjet_eta);
      Resp1 = probejet_pt / barrelgenjet_pt;
      Resp2 = barreljet_pt/ probegenjet_pt;
    } else {
      // both probe and barrel jet close to either barrel genjet or genjet
      DPhi1 = DPhi2 = DEta1 = DEta2 = TMath::Pi();
      Resp1 = Resp2 = 100;
      unmatchegGenJets++;
      return true;
    }

    double DR1, DR2;
    DR1 = TMath::Sqrt( TMath::Power(DPhi1, 2 ) + TMath::Power(DEta1, 2) );
    DR2 = TMath::Sqrt( TMath::Power(DPhi2, 2 ) + TMath::Power(DEta2, 2) );

    if ( TMath::Abs(TVector2::Phi_mpi_pi((probejet_phi - barreljet_phi))) > 2.7 ) {
      for ( int k = 0 ; k < PtBins_Central ; k++ ) {
        if ((pt_ave > Pt_bins_Central[k]) && (pt_ave < Pt_bins_Central[k+1]) ) {

          for ( int r = 0 ; r < EtaBins_SM ; r++ ) {
            cond1 = (TMath::Abs(barreljet_eta) > Eta_bins_SM[r] && TMath::Abs(barreljet_eta) < Eta_bins_SM[r+1] && TMath::Abs(probejet_eta) > Eta_bins_SM[r] && TMath::Abs(probejet_eta) < Eta_bins_SM[r+1]);
            cond2 = false;
            cond3 = ((rand()%2)+1)==1;
            SELECT_ETA_ALPHA_BIN(SM,SM,cond1,cond2,cond3)
          }
          for ( int r = 0 ; r < EtaBins_FE_reference ; r++ ) {
            cond1 = (TMath::Abs(barreljet_eta)> 0. && TMath::Abs(barreljet_eta)< s_eta_barr && TMath::Abs(probejet_eta) > Eta_bins_FE_reference[r] && TMath::Abs(probejet_eta) < Eta_bins_FE_reference[r+1]);
            cond2 = (TMath::Abs(probejet_eta) > 0. && TMath::Abs(probejet_eta) < s_eta_barr && TMath::Abs(barreljet_eta)> Eta_bins_FE_reference[r] && TMath::Abs(barreljet_eta)< Eta_bins_FE_reference[r+1]);
            if (Eta_bins_FE_reference[r] < s_eta_barr) {
              cond3 = ((rand()%2)+1)==1;
            }
            else{
              cond3 = cond2;
            }
            SELECT_ETA_ALPHA_BIN(FE_reference,FE,cond1,cond2,cond3)
          }
          for ( int r = 0 ; r < EtaBins_FE_control ; r++ ) {
            cond1 = (TMath::Abs(barreljet_eta)> 0. && TMath::Abs(barreljet_eta)< s_eta_barr && TMath::Abs(probejet_eta) > Eta_bins_FE_control[r] && TMath::Abs(probejet_eta) < Eta_bins_FE_control[r+1]);
            cond2 = (TMath::Abs(probejet_eta) > 0. && TMath::Abs(probejet_eta) < s_eta_barr && TMath::Abs(barreljet_eta)> Eta_bins_FE_control[r] && TMath::Abs(barreljet_eta)< Eta_bins_FE_control[r+1]);
            cond3 = cond2;
            SELECT_ETA_ALPHA_BIN(FE_control,FE,cond1,cond2,cond3)
          }
          break;
        }
      }
      for ( int k = 0 ; k < PtBins_HF ; k++ ) {
        if ((pt_ave > Pt_bins_HF[k]) && (pt_ave < Pt_bins_HF[k+1]) ) {
          for ( int r = 0 ; r < EtaBins_SM_control; r++ ) {
            cond1 = (TMath::Abs(barreljet_eta) > Eta_bins_SM_control[r] && TMath::Abs(barreljet_eta) < Eta_bins_SM_control[r+1] && TMath::Abs(probejet_eta) > Eta_bins_SM_control[r] && TMath::Abs(probejet_eta) < Eta_bins_SM_control[r+1]);
            cond2 = false;
            cond3 = ((rand()%2)+1)==1;
            SELECT_ETA_ALPHA_BIN(SM_control,SM,cond1,cond2,cond3)
          }
          for ( int r = 0 ; r < EtaBins_FE ; r++ ) {
            cond1 = (TMath::Abs(barreljet_eta)> 0. && TMath::Abs(barreljet_eta)< s_eta_barr && TMath::Abs(probejet_eta) > Eta_bins_FE[r] && TMath::Abs(probejet_eta) < Eta_bins_FE[r+1]);
            cond2 = (TMath::Abs(probejet_eta) > 0. && TMath::Abs(probejet_eta) < s_eta_barr && TMath::Abs(barreljet_eta)> Eta_bins_FE[r] && TMath::Abs(barreljet_eta)< Eta_bins_FE[r+1]);
            cond3 = cond2;
            SELECT_ETA_ALPHA_BIN_2(FE,FE,cond1,cond2,cond3)
          }
          break;
        }
      }
    }
  }

  //    same part for GenJets
  double gen_alpha_raw = gen_alpha_;
  if ( genjet2_pt > gen_threshold) {
    if ( genjet3_pt > gen_threshold && ngenjet > 1 ) {//?
      completeGen =  gen_alpha_raw; //2 * jet3_pt/( jet1_pt + jet2_pt );
      parallelGen = gen_alpha_raw; //(2*Jets[2]*(Jets[0]-Jets[1]))/((Jets[0]-Jets[1]).Pt()*( jet1_pt + jet2_pt ));
      perpendicularGen = gen_alpha_raw; //TMath::Sqrt( TMath::Power(complete, 2 ) - TMath::Power(parallel, 2) );
    } else {
      if (ngenjet < 3) {
        completeGen = 0. ;
        parallelGen = 0. ;
        perpendicularGen = 0. ;
      } else {
        completeGen = 1. ;
        parallelGen = 1. ;
        perpendicularGen = 1. ;
      }
    }

    if (flag1 == 0 ) alphaGen = TMath::Abs(completeGen);
    if (flag1 == 1 ) alphaGen = TMath::Abs(parallelGen);
    if (flag1 == 2 ) alphaGen = TMath::Abs(perpendicularGen);

    if ( TMath::Abs(TVector2::Phi_mpi_pi((barrelgenjet_phi - probegenjet_phi)))  > 2.7 ) { // I fill alpha_max histograms
      for ( int k = 0 ; k < PtBins_Central ; k++ ) {
        if ((gen_pt_ave > Pt_bins_Central[k]) && (gen_pt_ave < Pt_bins_Central[k+1])) {

          for ( int r = 0 ; r < EtaBins_SM ; r++ ) {
            cond1 = (TMath::Abs(barrelgenjet_eta) > Eta_bins_SM[r] && TMath::Abs(barrelgenjet_eta) < Eta_bins_SM[r+1] && TMath::Abs(probegenjet_eta) > Eta_bins_SM[r] && TMath::Abs(probegenjet_eta) < Eta_bins_SM[r+1]);
            cond2 = false;
            cond3 = ((rand()%2)+1)==1;
            SELECT_ETA_ALPHA_BIN_GEN(SM,cond1,cond2,cond3)
          }
          for ( int r = 0 ; r < EtaBins_FE_reference ; r++ ) {
            cond1 = (TMath::Abs(barrelgenjet_eta) > 0. && TMath::Abs(barrelgenjet_eta) < s_eta_barr && TMath::Abs(probegenjet_eta) > Eta_bins_FE_reference[r] && TMath::Abs(probegenjet_eta) < Eta_bins_FE_reference[r+1]);
            cond2 = (TMath::Abs(probegenjet_eta) > 0. && TMath::Abs(probegenjet_eta) < s_eta_barr && TMath::Abs(barrelgenjet_eta) > Eta_bins_FE_reference[r] && TMath::Abs(barrelgenjet_eta) < Eta_bins_FE_reference[r+1]);
            if (Eta_bins_FE_reference[r] < s_eta_barr) {
              cond3 = ((rand()%2)+1)==1;
            }
            else{
              cond3 = cond2;
            }
            SELECT_ETA_ALPHA_BIN_GEN(FE_reference,cond1,cond2,cond3)
          }

          for ( int r = 0 ; r < EtaBins_FE_control ; r++ ) {
            cond1 = (TMath::Abs(barrelgenjet_eta) > 0. && TMath::Abs(barrelgenjet_eta) < s_eta_barr && TMath::Abs(probegenjet_eta) > Eta_bins_FE_control[r] && TMath::Abs(probegenjet_eta) < Eta_bins_FE_control[r+1]);
            cond2 = (TMath::Abs(probegenjet_eta) > 0. && TMath::Abs(probegenjet_eta) < s_eta_barr && TMath::Abs(barrelgenjet_eta) > Eta_bins_FE_control[r] && TMath::Abs(barrelgenjet_eta) < Eta_bins_FE_control[r+1]);
            cond3 = cond2;
            SELECT_ETA_ALPHA_BIN_GEN(FE_control,cond1,cond2,cond3)
          }

          break;
        }
      }

      for ( int k = 0 ; k < PtBins_HF ; k++ ) {
        if ((gen_pt_ave > Pt_bins_HF[k]) && (gen_pt_ave < Pt_bins_HF[k+1])) {

          for ( int r = 0 ; r < EtaBins_SM_control ; r++ ) {
            cond1 = (TMath::Abs(barrelgenjet_eta) > Eta_bins_SM_control[r] && TMath::Abs(barrelgenjet_eta) < Eta_bins_SM_control[r+1] && TMath::Abs(probegenjet_eta) > Eta_bins_SM_control[r] && TMath::Abs(probegenjet_eta) < Eta_bins_SM_control[r+1]);
            cond2 = false;
            cond3 = ((rand()%2)+1)==1;
            SELECT_ETA_ALPHA_BIN_GEN(SM_control,cond1,cond2,cond3)
          }

          for ( int r = 0 ; r < EtaBins_FE ; r++ ) {
            cond1 = (TMath::Abs(barrelgenjet_eta) > 0. && TMath::Abs(barrelgenjet_eta) < s_eta_barr && TMath::Abs(probegenjet_eta) > Eta_bins_FE[r] && TMath::Abs(probegenjet_eta) < Eta_bins_FE[r+1]);
            cond2 = (TMath::Abs(probegenjet_eta) > 0. && TMath::Abs(probegenjet_eta) < s_eta_barr && TMath::Abs(barrelgenjet_eta) > Eta_bins_FE[r] && TMath::Abs(barrelgenjet_eta) < Eta_bins_FE[r+1]);
            cond3 = cond2;
            SELECT_ETA_ALPHA_BIN_GEN(FE,cond1,cond2,cond3)
          }

          break;
        }
      }
    }
  }
  return kTRUE;
}

void MySelector::SlaveTerminate() {
  // The SlaveTerminate() function is called after all entries or objects
  // have been processed. When running with PROOF SlaveTerminate() is called
  // on each slave server.

  std::cout <<"            Analyzed events #" <<  TotalEvents << std::endl;
  std::cout <<"            unmatchegGenJets events #" <<  unmatchegGenJets << std::endl;

  std::ofstream mytxtfile;
  mytxtfile.open (outdir+"counts.txt");
  mytxtfile << "Analyzed events #" <<  TotalEvents << "\n";
  mytxtfile << "unmatchegGenJets events #" <<  unmatchegGenJets << "\n";
  mytxtfile.close();

  TFile *fpt = new TFile(outdir+"pt_mc_incl_full.root","RECREATE"); ;
  fpt->cd();
  h_JetAvePt_FE -> Write();
  h_Jet1Pt_FE -> Write();
  h_Jet2Pt_FE -> Write();
  h_Jet3Pt_FE -> Write();
  h_JetAvePt_SM -> Write();
  h_Jet1Pt_SM -> Write();
  h_Jet2Pt_SM -> Write();
  h_Jet3Pt_SM -> Write();
  fpt->Close();

  TFile *fprim = new TFile(outdir+"PU_incl_full.root","RECREATE"); ;
  fprim->cd();
  h_PU -> Write();
  fprim -> Close();

  TFile *f  = new TFile(outdir+"histograms_mc_incl_full.root","RECREATE");
  TFile *f1 = new TFile(outdir+"histograms_mc_incl_full_control.root","RECREATE");
  TFile *f2 = new TFile(outdir+"histograms_mc_incl_full_2D.root","RECREATE");
  TFile *f3 = new TFile(outdir+"histograms_mc_incl_full_2D_dR.root","RECREATE");
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
      for( int m = 0; m < EtaBins_FE; m++ ) {
        f3->cd();
        for (unsigned int i = 0; i < dR_bins.size()-1; i++) {
          asy_dR_barrel_FE.at(m).at(p).at(r).at(i) -> Write();
          asy_dR_probe_FE.at(m).at(p).at(r).at(i) -> Write();
        }
      }
    }
  }

  f->cd();
  h_rho_SM -> Write();
  h_rho_FE -> Write();
  f -> Close();
  f1-> Close();
  f2-> Close();
  f3-> Close();
  f_alpha -> Close();

}

void MySelector::Terminate() {
  // The Terminate() function is the last function to be called during
  // a query. It always runs on the client, it can be used to present
  // the results graphically or save the results to file.

}
