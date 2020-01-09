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
asymmetries_dR1_##region.at(r).at(k).at(m)->Fill( DR1, weight);                                         \
asymmetries_dR2_##region.at(r).at(k).at(m)->Fill( DR2, weight);                                         \
if (DR1 < s_delta_R) MC_Truth_asymmetries_##region.at(r).at(k).at(m)->Fill( Resp1, weight);             \
if (DR2 < s_delta_R) MC_Truth_asymmetries_##region.at(r).at(k).at(m)->Fill( Resp2, weight);             \
MC_Truth_asymmetries_2D_##region.at(r).at(k).at(m)->Fill(Resp1, Resp2,weight);                          \
if ( m == AlphaBins-1 ) {                                                                               \
  h_JetAvePt_##method->Fill( pt_ave, weight);                                                           \
  h_Jet1Pt_##method->Fill( barreljet_pt, weight);                                                       \
  h_Jet2Pt_##method->Fill( probejet_pt, weight);                                                        \
  h_Jet3Pt_##method->Fill( jet3_pt, weight);                                                            \
  h_rho_##method->Fill( rho, weight);                                                                   \
  if (asy!=0) h_relerr_##method->Fill(TMath::Abs(weight/asy), 1.);                                      \
}                                                                                                       \

#define FILL_GEN_HISTOS(region)                                                                                           \
if (TMath::Abs(gen_asy) < 5) {                                                                                            \
  gen_asymmetries_##region.at(r).at(k).at(m)->Fill( gen_asy , weight);                                                    \
  gen_asymmetries_pt_##region.at(r).at(k).at(m)->Fill( gen_pt_ave, weight);                                               \
  gen_asymmetries_pt3_##region.at(r).at(k).at(m)->Fill( genjet3_pt, weight);                                              \
  gen_dR_##region.at(r).at(k).at(m)->Fill( gen_Delta_R_radiation_barrel, gen_Delta_R_radiation_probe, weight);            \
  gen_dR_probe_##region.at(r).at(k).at(m)->Fill( gen_Delta_R_radiation_probe, gen_asy, weight);                           \
  gen_dR_barrel_##region.at(r).at(k).at(m)->Fill( gen_Delta_R_radiation_barrel, gen_asy, weight);                         \
  gen_dR3_##region.at(r).at(k).at(m)->Fill( gen_asy, gen_Delta_R_radiation_barrel, gen_Delta_R_radiation_probe, weight);  \
}                                                                                                                         \

#define SELECT_ETA_ALPHA_BIN(region,method,cond1,cond2)         \
if (cond1 || cond2) {                                           \
  h_alpha_sel->Fill(alpha, 1);                                  \
  for ( int m = 0 ; m < AlphaBins ; m++ ) {                     \
    if ( alpha < Alpha_bins[m] ) {                              \
      if (dofill) nevents_central[k][r+shift][m] +=1;           \
      else nevents_HF[k][r+shift][m] +=1;                       \
      double asy = asymmetry;                                   \
      FILL_HISTOS(region,method)                                \
      if ( excl_bin ) break;                                    \
    }                                                           \
  }                                                             \
  alpha_spectrum_##region.at(r).at(k)->Fill(alpha, weight);     \
  alpha2D_##region.at(r).at(k)->Fill(alpha, alphaGen, weight);  \
}                                                               \

#define SELECT_ETA_ALPHA_BIN_GEN(region,cond1,cond2)  \
if (cond1 || cond2) {                                 \
  for ( int m = 0 ; m < AlphaBins ; m++ ) {           \
    if ( alphaGen < Alpha_bins[m] ) {                 \
      double gen_asy = gen_asymmetry;                 \
      FILL_GEN_HISTOS(region)                         \
      if ( excl_bin ) break;                          \
    }                                                 \
  }                                                   \
}                                                     \

#define WRITE_HISTOS(region)                                    \
for( int m = 0; m < EtaBins_##region; m++ ) {                   \
  f->cd();                                                      \
  asymmetries_##region.at(m).at(p).at(r)->Write();              \
  asymmetries_pt_##region.at(m).at(p).at(r)->Write();           \
  gen_asymmetries_##region.at(m).at(p).at(r)->Write();          \
  gen_asymmetries_pt_##region.at(m).at(p).at(r)->Write();       \
  MC_Truth_asymmetries_##region.at(m).at(p).at(r)->Write();     \
  MC_Truth_asymmetries_2D_##region.at(m).at(p).at(r)->Write();  \
  f1->cd();                                                     \
  asymmetries_rho_##region.at(m).at(p).at(r)->Write();          \
  asymmetries_pt3_##region.at(m).at(p).at(r)->Write();          \
  asymmetries_dR1_##region.at(m).at(p).at(r)->Write();          \
  asymmetries_dR2_##region.at(m).at(p).at(r)->Write();          \
  gen_asymmetries_pt3_##region.at(m).at(p).at(r)->Write();      \
  f2->cd();                                                     \
  dR_##region.at(m).at(p).at(r)->Write();                       \
  gen_dR_##region.at(m).at(p).at(r)->Write();                   \
  dR_probe_##region.at(m).at(p).at(r)->Write();                 \
  gen_dR_probe_##region.at(m).at(p).at(r)->Write();             \
  dR_barrel_##region.at(m).at(p).at(r)->Write();                \
  gen_dR_barrel_##region.at(m).at(p).at(r)->Write();            \
  dR3_##region.at(m).at(p).at(r)->Write();                      \
  gen_dR3_##region.at(m).at(p).at(r)->Write();                  \
  f_alpha->cd();                                                \
  alpha_spectrum_##region.at(m).at(p)->Write();                 \
  alpha2D_##region.at(m).at(p)->Write();                        \
}                                                               \

void MakeHistograms(std::vector< std::vector< std::vector< TH1F* > > > &asymmetries, std::vector< std::vector< std::vector< TH1F* > > > &asymmetries_pt, std::vector< std::vector< std::vector< TH1F* > > > &asymmetries_rho, std::vector< std::vector< std::vector< TH1F* > > > &asymmetries_pt3, std::vector< std::vector< std::vector< TH1F* > > > &asymmetries_dR1, std::vector< std::vector< std::vector< TH1F* > > > &asymmetries_dR2, std::vector< std::vector< TH1F* > > &alpha_spectrum, std::vector< std::vector< std::vector< TH1F* > > > &gen_asymmetries, std::vector< std::vector< std::vector< TH1F* > > > &gen_asymmetries_pt, std::vector< std::vector< std::vector< TH1F* > > > &gen_asymmetries_rho, std::vector< std::vector< std::vector< TH1F* > > > &gen_asymmetries_pt3, std::vector< std::vector< std::vector< TH1F* > > > &MC_Truth_asymmetries, std::vector< std::vector< std::vector< TH2F* > > > &MC_Truth_asymmetries_2D, std::vector< std::vector< std::vector< TH2F* > > > &dR, std::vector< std::vector< std::vector< TH2F* > > > &gen_dR, std::vector< std::vector< std::vector< TH2F* > > > &dR_probe, std::vector< std::vector< std::vector< TH2F* > > > &gen_dR_probe, std::vector< std::vector< std::vector< TH2F* > > > &dR_barrel, std::vector< std::vector< std::vector< TH2F* > > > &gen_dR_barrel, std::vector< std::vector< std::vector< TH3F* > > > &dR3, std::vector< std::vector< std::vector< TH3F* > > > &gen_dR3, std::vector< std::vector< TH2F* > > &alpha2D, TString text, TString extraText, int etaBins, int ptBins, int AlphaBins, int etaShift, int ptShift, int alphaShift) {
  for( int m = etaShift; m < etaBins+etaShift; m++ ) {
    std::vector< std::vector< TH1F* > > temp2, temp2pt, temp2rho, temp2pt3, temp2dR1, temp2dR2, gen_temp2rho, gen_temp2pt3, gen_temp2, gen_temp2pt, temp2_MCTruth;
    std::vector< std::vector< TH2F* > > temp2_dR, gen_temp2_dR, temp2_dR_probe, gen_temp2_dR_probe, temp2_dR_barrel, gen_temp2_dR_barrel, temp2_MCTruth2D;
    std::vector< TH1F* > alpha_temp2;
    std::vector< std::vector< TH3F* > > temp2_dR3, gen_temp2_dR3 ;
    std::vector< TH2F* > alpha2D_temp2;
    for( int p = 0; p < ptBins; p++ ) {
      std::vector< TH1F* > temp1, temp1pt, temp1rho, temp1pt3, temp1dR1, temp1dR2, gen_temp1rho, gen_temp1pt3, gen_temp1, gen_temp1pt, temp1_MCTruth;
      std::vector< TH2F* > temp1_dR, gen_temp1_dR, temp1_dR_probe, gen_temp1_dR_probe, temp1_dR_barrel, gen_temp1_dR_barrel, temp1_MCTruth2D;
      std::vector< TH3F* > temp1_dR3, gen_temp1_dR3 ;
      TString name_alpha   = "alpha";   name_alpha  += extraText; name_alpha  += "_eta"; name_alpha  += m+1; name_alpha  += "_pt"; name_alpha  += p+1;
      TString name_alpha2  = "alpha2D"; name_alpha2 += extraText; name_alpha2 += "_eta"; name_alpha2 += m+1; name_alpha2 += "_pt"; name_alpha2 += p+1;
      TH1F *h1_alpha = new TH1F( name_alpha, name_alpha, 80, 0., 0.8);
      TH2F* h2_alpha = new TH2F(name_alpha2, name_alpha2, AlphaBins, 0., 0.3, AlphaBins, 0., 0.3);
      h1_alpha->SetYTitle("a.u.");    h1_alpha->SetXTitle("Alpha");
      h1_alpha->Sumw2(); alpha_temp2.push_back(h1_alpha);
      h2_alpha->SetXTitle("alpha_reco"); h2_alpha->SetYTitle("alpha_gen");
      h2_alpha->Sumw2(); alpha2D_temp2.push_back(h2_alpha);

      for( int r = 0; r < AlphaBins; r++ ) {
        TString name     = text;        name     += extraText; name     += "_eta"; name     += m+1; name     += "_pt"; name     += p+1; name     += "_alpha"; name     += r+1;
        TString name_pt  = text+"pt";   name_pt  += extraText; name_pt  += "_eta"; name_pt  += m+1; name_pt  += "_pt"; name_pt  += p+1; name_pt  += "_alpha"; name_pt  += r+1;
        TString name_rho = text+"rho";  name_rho += extraText; name_rho += "_eta"; name_rho += m+1; name_rho += "_pt"; name_rho += p+1; name_rho += "_alpha"; name_rho += r+1;
        TString name_pt3 = text+"pt3";  name_pt3 += extraText; name_pt3 += "_eta"; name_pt3 += m+1; name_pt3 += "_pt"; name_pt3 += p+1; name_pt3 += "_alpha"; name_pt3 += r+1;
        TString name_dR1 = text+"dR1";  name_dR1 += extraText; name_dR1 += "_eta"; name_dR1 += m+1; name_dR1 += "_pt"; name_dR1 += p+1; name_dR1 += "_alpha"; name_dR1 += r+1;
        TString name_dR2 = text+"dR2";  name_dR2 += extraText; name_dR2 += "_eta"; name_dR2 += m+1; name_dR2 += "_pt"; name_dR2 += p+1; name_dR2 += "_alpha"; name_dR2 += r+1;

        TString gen_name        = "gen_"+text;        gen_name        += extraText; gen_name        += "_eta"; gen_name       += m+1; gen_name        += "_pt"; gen_name        += p+1; gen_name        += "_alpha"; gen_name       += r+1;
        TString gen_name_pt     = "gen_"+text+"pt";   gen_name_pt     += extraText; gen_name_pt     += "_eta"; gen_name_pt    += m+1; gen_name_pt     += "_pt"; gen_name_pt     += p+1; gen_name_pt     += "_alpha"; gen_name_pt    += r+1;
        TString gen_name_rho    = "gen_"+text+"rho";  gen_name_rho    += extraText; gen_name_rho    += "_eta"; gen_name_rho   += m+1; gen_name_rho    += "_pt"; gen_name_rho    += p+1; gen_name_rho    += "_alpha"; gen_name_rho   += r+1;
        TString gen_name_pt3    = "gen_"+text+"pt3";  gen_name_pt3    += extraText; gen_name_pt3    += "_eta"; gen_name_pt3   += m+1; gen_name_pt3    += "_pt"; gen_name_pt3    += p+1; gen_name_pt3    += "_alpha"; gen_name_pt3   += r+1;
        TString name_MCTruth    = "mctruth";          name_MCTruth    += extraText; name_MCTruth    += "_eta"; name_MCTruth   += m+1; name_MCTruth    += "_pt"; name_MCTruth    += p+1; name_MCTruth    += "_alpha"; name_MCTruth   += r+1;
        TString name_MCTruth2D  = "mctruth2D";        name_MCTruth2D  += extraText; name_MCTruth2D  += "_eta"; name_MCTruth2D += m+1; name_MCTruth2D  += "_pt"; name_MCTruth2D  += p+1; name_MCTruth2D  += "_alpha"; name_MCTruth2D += r+1;

        TString name_dR             = "dR";            name_dR            += extraText; name_dR             += "_eta"; name_dR            += m+1; name_dR             += "_pt"; name_dR             += p+1; name_dR             += "_alpha"; name_dR            += r+1;
        TString gen_name_dR         = "gen_dR";        gen_name_dR        += extraText; gen_name_dR         += "_eta"; gen_name_dR        += m+1; gen_name_dR         += "_pt"; gen_name_dR         += p+1; gen_name_dR         += "_alpha"; gen_name_dR        += r+1;
        TString name_dR_probe       = "dR_probe";      name_dR_probe      += extraText; name_dR_probe       += "_eta"; name_dR_probe      += m+1; name_dR_probe       += "_pt"; name_dR_probe       += p+1; name_dR_probe       += "_alpha"; name_dR_probe      += r+1;
        TString gen_name_dR_probe   = "gen_dR_probe";  gen_name_dR_probe  += extraText; gen_name_dR_probe   += "_eta"; gen_name_dR_probe  += m+1; gen_name_dR_probe   += "_pt"; gen_name_dR_probe   += p+1; gen_name_dR_probe   += "_alpha"; gen_name_dR_probe  += r+1;
        TString name_dR_barrel      = "dR_barrel";     name_dR_barrel     += extraText; name_dR_barrel      += "_eta"; name_dR_barrel     += m+1; name_dR_barrel      += "_pt"; name_dR_barrel      += p+1; name_dR_barrel      += "_alpha"; name_dR_barrel     += r+1;
        TString gen_name_dR_barrel  = "gen_dR_barrel"; gen_name_dR_barrel += extraText; gen_name_dR_barrel  += "_eta"; gen_name_dR_barrel += m+1; gen_name_dR_barrel  += "_pt"; gen_name_dR_barrel  += p+1; gen_name_dR_barrel  += "_alpha"; gen_name_dR_barrel += r+1;
        TString name_dR3            = "dR3";           name_dR3           += extraText; name_dR3            += "_eta"; name_dR3           += m+1; name_dR3            += "_pt"; name_dR3            += p+1; name_dR3            += "_alpha"; name_dR3           += r+1;
        TString gen_name_dR3        = "gen_dR3";       gen_name_dR3       += extraText; gen_name_dR3        += "_eta"; gen_name_dR3       += m+1; gen_name_dR3        += "_pt"; gen_name_dR3        += p+1; gen_name_dR3        += "_alpha"; gen_name_dR3       += r+1;

        TH1F *h1 = new TH1F(name,     name,     160,-0.8, 0.8);   h1->SetXTitle("Asymmetry"); h1->SetYTitle("a.u.");  h1->Sumw2();  temp1.push_back(h1);
        TH1F *h2 = new TH1F(name_pt,  name_pt,  50,   0,  1500);  h2->SetXTitle("Pt[GeV]");   h2->SetYTitle("a.u.");  h2->Sumw2();  temp1pt.push_back(h2);
        TH1F *h3 = new TH1F(name_rho, name_rho, 100,  0,  100);   h3->SetXTitle("rho");       h3->SetYTitle("a.u.");  h3->Sumw2();  temp1rho.push_back(h3);
        TH1F *h4 = new TH1F(name_pt3, name_pt3, 50,   0,  1500);  h4->SetXTitle("Pt[GeV]");   h4->SetYTitle("a.u.");  h4->Sumw2();  temp1pt3.push_back(h4);
        TH1F *h5 = new TH1F(name_dR1, name_dR1, 60,   0,  6.0);   h5->SetXTitle("dR");        h5->SetYTitle("a.u.");  h5->Sumw2();  temp1dR1.push_back(h5);
        TH1F *h6 = new TH1F(name_dR2, name_dR2, 60,   0,  6.0);   h6->SetXTitle("dR");        h6->SetYTitle("a.u.");  h6->Sumw2();  temp1dR2.push_back(h6);
        TH1F *gen_h1 = new TH1F(gen_name,     gen_name,     160,-0.8, 0.8);   gen_h1->SetXTitle("Asymmetry"); gen_h1->SetYTitle("a.u.");  gen_h1->Sumw2();  gen_temp1.push_back(gen_h1);
        TH1F *gen_h2 = new TH1F(gen_name_pt,  gen_name_pt,  50,   0,  1500);  gen_h2->SetXTitle("Pt[GeV]");   gen_h2->SetYTitle("a.u.");  gen_h2->Sumw2();  gen_temp1pt.push_back(gen_h2);
        TH1F *gen_h3 = new TH1F(gen_name_rho, gen_name_rho, 100,  0,  100);   gen_h3->SetXTitle("rho");       gen_h3->SetYTitle("a.u.");  gen_h3->Sumw2();  gen_temp1rho.push_back(gen_h3);
        TH1F *gen_h4 = new TH1F(gen_name_pt3, gen_name_pt3, 50,   0,  1500);  gen_h4->SetXTitle("Pt[GeV]");   gen_h4->SetYTitle("a.u.");  gen_h4->Sumw2();  gen_temp1pt3.push_back(gen_h4);
        TH1F *h1_MCTruth    = new TH1F(name_MCTruth,    name_MCTruth,   200, 0, 2.0);               h1_MCTruth->SetXTitle("Response");      h1_MCTruth->SetYTitle("a.u.");          h1_MCTruth->Sumw2();    temp1_MCTruth.push_back(h1_MCTruth);
        TH2F *h2_MCTruth2D  = new TH2F(name_MCTruth2D,  name_MCTruth2D, 200, 0, 2.0, 200, 0, 2.0); h2_MCTruth2D->SetXTitle("Response 1");  h2_MCTruth2D->SetXTitle("Response 2");  h2_MCTruth2D->Sumw2();  temp1_MCTruth2D.push_back(h2_MCTruth2D);
        TH2F *h2_dR             = new TH2F(name_dR,             name_dR,            60, 0, 6.0, 60,   0,  6.0); h2_dR->SetXTitle("#Delta R (jet_{barrel}, jet_{3})");             h2_dR->SetYTitle("#Delta R (jet_{probe}, jet_{3})");      h2_dR->Sumw2();             temp1_dR.push_back(h2_dR);
        TH2F *gen_h2_dR         = new TH2F(gen_name_dR,         gen_name_dR,        60, 0, 6.0, 60,   0,  6.0); gen_h2_dR->SetXTitle("#Delta R (jet_{barrel}, jet_{3})");         gen_h2_dR->SetYTitle("#Delta R (jet_{probe}, jet_{3})");  gen_h2_dR->Sumw2();         gen_temp1_dR.push_back(gen_h2_dR);
        TH2F *h2_dR_probe       = new TH2F(name_dR_probe,       name_dR_probe,      60, 0, 6.0, 160,-0.8, 0.8); h2_dR_probe->SetXTitle("#Delta R (jet_{probe}, jet_{3})");        h2_dR_probe->SetYTitle("Asymmetry");                      h2_dR_probe->Sumw2();       temp1_dR_probe.push_back(h2_dR_probe);
        TH2F *gen_h2_dR_probe   = new TH2F(gen_name_dR_probe,   gen_name_dR_probe,  60, 0, 6.0, 160,-0.8, 0.8); gen_h2_dR_probe->SetXTitle("#Delta R (jet_{probe}, jet_{3})");    gen_h2_dR_probe->SetYTitle("Asymmetry");                  gen_h2_dR_probe->Sumw2();   gen_temp1_dR_probe.push_back(gen_h2_dR_probe);
        TH2F *h2_dR_barrel      = new TH2F(name_dR_barrel,      name_dR_barrel,     60, 0, 6.0, 160,-0.8, 0.8); h2_dR_barrel->SetXTitle("#Delta R (jet_{barrel}, jet_{3})");      h2_dR_barrel->SetYTitle("Asymmetry");                     h2_dR_barrel->Sumw2();      temp1_dR_barrel.push_back(h2_dR_barrel);
        TH2F *gen_h2_dR_barrel  = new TH2F(gen_name_dR_barrel,  gen_name_dR_barrel, 60, 0, 6.0, 160,-0.8, 0.8); gen_h2_dR_barrel->SetXTitle("#Delta R (jet_{barrel}, jet_{3})");  gen_h2_dR_barrel->SetYTitle("Asymmetry");                 gen_h2_dR_barrel->Sumw2();  gen_temp1_dR_barrel.push_back(gen_h2_dR_barrel);
        TH3F *h3_dR3      = new TH3F(name_dR3,      name_dR3,     160, -0.8, 0.8, 60, 0, 6.0, 60, 0, 6.0);  h3_dR3->SetXTitle("Asymmetry");     h3_dR3->SetYTitle("#Delta R (jet_{barrel}, jet_{3})");      h3_dR3->SetZTitle("#Delta R (jet_{probe}, jet_{3})");     h3_dR3->Sumw2();      temp1_dR3.push_back(h3_dR3);
        TH3F *gen_h3_dR3  = new TH3F(gen_name_dR3,  gen_name_dR3, 160, -0.8, 0.8, 60, 0, 6.0, 60, 0, 6.0);  gen_h3_dR3->SetXTitle("Asymmetry"); gen_h3_dR3->SetYTitle("#Delta R (jet_{barrel}, jet_{3})");  gen_h3_dR3->SetZTitle("#Delta R (jet_{probe}, jet_{3})"); gen_h3_dR3->Sumw2();  gen_temp1_dR3.push_back(gen_h3_dR3);
      }
      temp2.push_back(temp1); temp2pt.push_back(temp1pt); temp2rho.push_back(temp1rho);  temp2pt3.push_back(temp1pt3);  temp2dR1.push_back(temp1dR1);  temp2dR2.push_back(temp1dR2);
      gen_temp2.push_back(gen_temp1); gen_temp2pt.push_back(gen_temp1pt); gen_temp2rho.push_back(gen_temp1rho); gen_temp2pt3.push_back(gen_temp1pt3);
      temp2_MCTruth.push_back(temp1_MCTruth); temp2_MCTruth2D.push_back(temp1_MCTruth2D); temp2_dR.push_back(temp1_dR); gen_temp2_dR.push_back(gen_temp1_dR); temp2_dR_probe.push_back(temp1_dR_probe); gen_temp2_dR_probe.push_back(gen_temp1_dR_probe); temp2_dR_barrel.push_back(temp1_dR_barrel); gen_temp2_dR_barrel.push_back(gen_temp1_dR_barrel);
      temp2_dR3.push_back(temp1_dR3); gen_temp2_dR3.push_back(gen_temp1_dR3);
    }
    asymmetries.push_back(temp2); asymmetries_pt.push_back(temp2pt); asymmetries_rho.push_back(temp2rho); asymmetries_pt3.push_back(temp2pt3); asymmetries_dR1.push_back(temp2dR1); asymmetries_dR2.push_back(temp2dR2);
    gen_asymmetries.push_back(gen_temp2); gen_asymmetries_pt.push_back(gen_temp2pt); gen_asymmetries_rho.push_back(gen_temp2rho); gen_asymmetries_pt3.push_back(gen_temp2pt3);
    MC_Truth_asymmetries.push_back(temp2_MCTruth); MC_Truth_asymmetries_2D.push_back(temp2_MCTruth2D); dR.push_back(temp2_dR); gen_dR.push_back(gen_temp2_dR); dR_probe.push_back(temp2_dR_probe); gen_dR_probe.push_back(gen_temp2_dR_probe); dR_barrel.push_back(temp2_dR_barrel); gen_dR_barrel.push_back(gen_temp2_dR_barrel);
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
  h_alpha_raw   = new TH1F("Alpha_raw",   "#alpha before selection",80, 0., 0.8);  h_alpha_raw->SetXTitle("#alpha_raw");       h_alpha_raw->SetYTitle("a.u.");   h_alpha_raw->Sumw2();
  h_alpha_sel   = new TH1F("Alpha",       "#alpha after selection", 80, 0., 0.8);   h_alpha_sel->SetXTitle("#alpha");           h_alpha_sel->SetYTitle("a.u.");   h_alpha_sel->Sumw2();
  h_relerr_SM   = new TH1F("RelerrSM",    "Relerr SM",            1e07, 0 , 1e07);  h_relerr_SM->SetXTitle("relerr");           h_relerr_SM->SetYTitle("a.u.");   h_relerr_SM->Sumw2();
  h_relerr_FE   = new TH1F("RelerrFE",    "Relerr FE",            1e07, 0 , 1e07);  h_relerr_FE->SetXTitle("relerr");           h_relerr_FE->SetYTitle("a.u.");   h_relerr_FE->Sumw2();
  h_PUweight    = new TH1F("WeightPileUp","WieghtedPU distribution",60, 0., 60);    h_PUweight->SetXTitle("PU");                h_PUweight->SetYTitle("a.u.");    h_PUweight->Sumw2();
  h_rho_SM      = new TH1F("Rho",         "Rho distribution",       60, 0., 30);    h_rho_SM->SetXTitle("Rho");                 h_rho_SM->SetYTitle("a.u.");      h_rho_SM->Sumw2();
  h_rho_FE      = new TH1F("RhoFWD",      "RhoFWD distribution",    60, 0., 30);    h_rho_FE->SetXTitle("RhoFWD");              h_rho_FE->SetYTitle("a.u.");      h_rho_FE->Sumw2();

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



  MakeHistograms(asymmetries_SM,             asymmetries_pt_SM,           asymmetries_rho_SM,           asymmetries_pt3_SM,           asymmetries_dR1_SM,           asymmetries_dR2_SM,           alpha_spectrum_SM,            gen_asymmetries_SM,           gen_asymmetries_pt_SM,            gen_asymmetries_rho_SM,           gen_asymmetries_pt3_SM,           MC_Truth_asymmetries_SM,            MC_Truth_asymmetries_2D_SM,            dR_SM,            gen_dR_SM,            dR_probe_SM,            gen_dR_probe_SM,            dR_barrel_SM,           gen_dR_barrel_SM,           dR3_SM,           gen_dR3_SM,           alpha2D_SM,           "asymm",  "_SM",            EtaBins_SM,           PtBins_Central,  AlphaBins,   etaShift_SM,            0, 0);
  MakeHistograms(asymmetries_SM_control,     asymmetries_pt_SM_control,   asymmetries_rho_SM_control,   asymmetries_pt3_SM_control,   asymmetries_dR1_SM_control,   asymmetries_dR2_SM_control,   alpha_spectrum_SM_control,    gen_asymmetries_SM_control,   gen_asymmetries_pt_SM_control,    gen_asymmetries_rho_SM_control,   gen_asymmetries_pt3_SM_control,   MC_Truth_asymmetries_SM_control,    MC_Truth_asymmetries_2D_SM_control,    dR_SM_control,    gen_dR_SM_control,    dR_probe_SM_control,    gen_dR_probe_SM_control,    dR_barrel_SM_control,   gen_dR_barrel_SM_control,   dR3_SM_control,   gen_dR3_SM_control,   alpha2D_SM_control,   "asymm",  "_SM_control",    EtaBins_SM_control,   PtBins_Central,  AlphaBins,   etaShift_SM_control,    0, 0);
  MakeHistograms(asymmetries_FE_reference,  asymmetries_pt_FE_reference,  asymmetries_rho_FE_reference, asymmetries_pt3_FE_reference, asymmetries_dR1_FE_reference, asymmetries_dR2_FE_reference, alpha_spectrum_FE_reference,  gen_asymmetries_FE_reference, gen_asymmetries_pt_FE_reference,  gen_asymmetries_rho_FE_reference, gen_asymmetries_pt3_FE_reference, MC_Truth_asymmetries_FE_reference,  MC_Truth_asymmetries_2D_FE_reference,  dR_FE_reference,  gen_dR_FE_reference,  dR_probe_FE_reference,  gen_dR_probe_FE_reference,  dR_barrel_FE_reference, gen_dR_barrel_FE_reference, dR3_FE_reference, gen_dR3_FE_reference, alpha2D_FE_reference, "asymm",  "_FE_reference",  EtaBins_FE_reference, PtBins_Central, AlphaBins,    etaShift_FE_reference,  0, 0);
  MakeHistograms(asymmetries_FE_control,     asymmetries_pt_FE_control,   asymmetries_rho_FE_control,   asymmetries_pt3_FE_control,   asymmetries_dR1_FE_control,   asymmetries_dR2_FE_control,   alpha_spectrum_FE_control,    gen_asymmetries_FE_control,   gen_asymmetries_pt_FE_control,    gen_asymmetries_rho_FE_control,   gen_asymmetries_pt3_FE_control,   MC_Truth_asymmetries_FE_control,    MC_Truth_asymmetries_2D_FE_control,    dR_FE_control,    gen_dR_FE_control,    dR_probe_FE_control,    gen_dR_probe_FE_control,    dR_barrel_FE_control,   gen_dR_barrel_FE_control,   dR3_FE_control,   gen_dR3_FE_control,   alpha2D_FE_control,  "asymm",  "_FE_control",    EtaBins_FE_control,   PtBins_Central, AlphaBins,    etaShift_FE_control,    0, 0);
  MakeHistograms(asymmetries_FE,             asymmetries_pt_FE,           asymmetries_rho_FE,           asymmetries_pt3_FE,           asymmetries_dR1_FE,           asymmetries_dR2_FE,           alpha_spectrum_FE,            gen_asymmetries_FE,           gen_asymmetries_pt_FE,            gen_asymmetries_rho_FE,           gen_asymmetries_pt3_FE,           MC_Truth_asymmetries_FE,            MC_Truth_asymmetries_2D_FE,            dR_FE,            gen_dR_FE,            dR_probe_FE,            gen_dR_probe_FE,            dR_barrel_FE,           gen_dR_barrel_FE,           dR3_FE,           gen_dR3_FE,           alpha2D_FE,           "asymm",  "_FE",            EtaBins_FE,           PtBins_Central,  AlphaBins,   etaShift_FE,            0, 0);

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
          TString name_dR; TH2F *h2;
          name_dR = "asy_dR_barrel_forward_";     name_dR += "probe"; name_dR += m+2+EtaBins_FE_control;  name_dR += "_pt"; name_dR += p+1; name_dR += "_alpha"; name_dR += r+1; name_dR += "_dR_probe";  name_dR += i;
          h2      = new TH2F(name_dR, name_dR, 160, -0.8, 0.8, 60, 0, 6.0); h2->SetXTitle("Asymmetry"); h2->SetYTitle("#Delta R (jet_{barrel}, jet_{3})");  h2->Sumw2();  temp3_barrel.push_back(h2);
          name_dR = "gen_asy_dR_barrel_forward_"; name_dR += "probe"; name_dR += m+2+EtaBins_FE_control;  name_dR += "_pt"; name_dR += p+1; name_dR += "_alpha"; name_dR += r+1; name_dR += "_dR_probe";  name_dR += i;
          h2      = new TH2F(name_dR, name_dR, 160, -0.8, 0.8, 60, 0, 6.0); h2->SetXTitle("Asymmetry"); h2->SetYTitle("#Delta R (jet_{barrel}, jet_{3})");  h2->Sumw2();  gen_temp3_barrel.push_back(h2);
          name_dR = "asy_dR_probe_forward_";      name_dR += "probe"; name_dR += m+2+EtaBins_FE_control;  name_dR += "_pt"; name_dR += p+1; name_dR += "_alpha"; name_dR += r+1; name_dR += "_dR_barrel"; name_dR += i;
          h2      = new TH2F(name_dR, name_dR, 160, -0.8, 0.8, 60, 0, 6.0); h2->SetXTitle("Asymmetry"); h2->SetYTitle("#Delta R (jet_{probe}, jet_{3})");   h2->Sumw2();  temp3_probe.push_back(h2);
          name_dR = "gen_asy_dR_probe_forward_";  name_dR += "probe"; name_dR += m+2+EtaBins_FE_control;  name_dR += "_pt"; name_dR += p+1; name_dR += "_alpha"; name_dR += r+1; name_dR += "_dR_barrel"; name_dR += i;
          h2      = new TH2F(name_dR, name_dR, 160, -0.8, 0.8, 60, 0, 6.0); h2->SetXTitle("Asymmetry"); h2->SetYTitle("#Delta R (jet_{probe}, jet_{3})"); h2->Sumw2();  gen_temp3_probe.push_back(h2);
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

bool MySelector::Process(Long64_t entry) {

  ++TotalEvents;
  if ( TotalEvents%100000 == 0 ) {  std::cout << "\t\tAnalyzing event #" << TotalEvents << std::endl; }
  GetEntry(entry);
  BuildEvent();

  bool cond1, cond2;

  double gen_thr = 10;
  double jet_thr=15;
  double alpha_raw = alpha_;
  double gen_alpha_raw = gen_alpha_;
  double alpha, alphaGen;

  // Below I choose what kind of asymmetries I want to study! excl_bin = true for exclusive bins
  bool excl_bin = false; // inclusive


  // if (nPU < 10 || nPU > 75) { std::cout << "error nPU " << nPU << std::endl; return kFALSE;} // TODO this is for 2017
  if (njet<2) return kTRUE;
  if (ngenjet<2) return kTRUE;
  if (barreljet_pt < jet_thr && probejet_pt < jet_thr) return kTRUE;
  if (barrelgenjet_pt < gen_thr && probegenjet_pt < gen_thr) return kTRUE;
  if ( TMath::Abs(TVector2::Phi_mpi_pi((probejet_phi - barreljet_phi))) < s_delta_phi-0.1 ) { /*std::cout << "Jets are not back to back: " << TMath::Abs(TVector2::Phi_mpi_pi((probejet_phi - barreljet_phi))) << std::endl;*/ return kTRUE;}
  if ( TMath::Abs(TVector2::Phi_mpi_pi((barrelgenjet_phi - probegenjet_phi))) < s_delta_phi-0.1 ) { unmatchegGenJets++; /*std::cout << "GenJets are not back to back: " << TMath::Abs(TVector2::Phi_mpi_pi((barrelgenjet_phi - probegenjet_phi))) << " " << event << " " << run << std::endl;*/ return kTRUE;}

  if ( genjet3_pt > gen_thr ) alphaGen = TMath::Abs(gen_alpha_raw);
  else alphaGen = (ngenjet <3) ? 0. : 1. ;

  if ( jet3_pt > jet_thr ) alpha = TMath::Abs(alpha_raw);
  else alpha = (njet <3) ? 0. : 1. ;

  h_PU->Fill(nPU, 1);
  h_alpha_raw->Fill(alpha_raw, 1);
  h_PUweight->Fill(nPU,weight);


  double DPhi1 = TMath::Abs(TVector2::Phi_mpi_pi(probejet_phi - probegenjet_phi));
  double DPhi2 = TMath::Abs(TVector2::Phi_mpi_pi(barreljet_phi- barrelgenjet_phi));
  double DEta1 = TMath::Abs(probejet_eta - probegenjet_eta);
  double DEta2 = TMath::Abs(barreljet_eta- barrelgenjet_eta);
  double Resp1 = probejet_pt / probegenjet_pt;
  double Resp2 = barreljet_pt/ barrelgenjet_pt;

  double DR1 = TMath::Sqrt( TMath::Power(DPhi1, 2 ) + TMath::Power(DEta1, 2));
  double DR2 = TMath::Sqrt( TMath::Power(DPhi2, 2 ) + TMath::Power(DEta2, 2));

  bool dofill; int shift;
  bool isHF = probejet_eta>eta_cut? true : false;

  if (!isHF) {
    dofill=true;
    for ( int k = 0 ; k < PtBins_Central ; k++ ) {
      if ((pt_ave > Pt_bins_Central[k]) && (pt_ave < Pt_bins_Central[k+1]) ) {
        for ( int r = 0 ; r < EtaBins_SM ; r++ ) {
          if (!is_JER_SM) continue;
          cond1 = (TMath::Abs(barreljet_eta) > Eta_bins_SM[r] && TMath::Abs(barreljet_eta) < Eta_bins_SM[r+1] && TMath::Abs(probejet_eta) > Eta_bins_SM[r] && TMath::Abs(probejet_eta) < Eta_bins_SM[r+1]);
          cond2 = false;
          shift = 0;
          SELECT_ETA_ALPHA_BIN(SM,SM,cond1,cond2)
        }
        for ( int r = 0 ; r < EtaBins_FE_reference ; r++ ) {
          if (is_JER_SM) continue;
          cond1 = (TMath::Abs(barreljet_eta)> 0. && TMath::Abs(barreljet_eta)< s_eta_barr && TMath::Abs(probejet_eta) > Eta_bins_FE_reference[r] && TMath::Abs(probejet_eta) < Eta_bins_FE_reference[r+1]);
          cond2 = (TMath::Abs(probejet_eta) > 0. && TMath::Abs(probejet_eta) < s_eta_barr && TMath::Abs(barreljet_eta)> Eta_bins_FE_reference[r] && TMath::Abs(barreljet_eta)< Eta_bins_FE_reference[r+1]);
          shift = 0;
          SELECT_ETA_ALPHA_BIN(FE_reference,FE,cond1,cond2)
        }
        for ( int r = 0 ; r < EtaBins_FE_control ; r++ ) {
          if (is_JER_SM) continue;
          cond1 = (TMath::Abs(barreljet_eta)> 0. && TMath::Abs(barreljet_eta)< s_eta_barr && TMath::Abs(probejet_eta) > Eta_bins_FE_control[r] && TMath::Abs(probejet_eta) < Eta_bins_FE_control[r+1]);
          cond2 = (TMath::Abs(probejet_eta) > 0. && TMath::Abs(probejet_eta) < s_eta_barr && TMath::Abs(barreljet_eta)> Eta_bins_FE_control[r] && TMath::Abs(barreljet_eta)< Eta_bins_FE_control[r+1]);
          shift = etaShift_FE_control;
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
          SELECT_ETA_ALPHA_BIN(SM_control,SM,cond1,cond2)
        }
        for ( int r = 0 ; r < EtaBins_FE ; r++ ) {
          if (is_JER_SM) continue;
          cond1 = (TMath::Abs(barreljet_eta)> 0. && TMath::Abs(barreljet_eta)< s_eta_barr && TMath::Abs(probejet_eta) > Eta_bins_FE[r] && TMath::Abs(probejet_eta) < Eta_bins_FE[r+1]);
          cond2 = (TMath::Abs(probejet_eta) > 0. && TMath::Abs(probejet_eta) < s_eta_barr && TMath::Abs(barreljet_eta)> Eta_bins_FE[r] && TMath::Abs(barreljet_eta)< Eta_bins_FE[r+1]);
          shift = etaShift_FE;
          SELECT_ETA_ALPHA_BIN(FE,FE,cond1,cond2)
        }
        break;
      }
    }
  }

  if (!isHF) {
    dofill=true;
    for ( int k = 0 ; k < PtBins_Central ; k++ ) {
      if ((gen_pt_ave > Pt_bins_Central[k]) && (gen_pt_ave < Pt_bins_Central[k+1])) {
        for ( int r = 0 ; r < EtaBins_SM ; r++ ) {
          if (!is_JER_SM) continue;
          cond1 = (TMath::Abs(barrelgenjet_eta) > Eta_bins_SM[r] && TMath::Abs(barrelgenjet_eta) < Eta_bins_SM[r+1] && TMath::Abs(probegenjet_eta) > Eta_bins_SM[r] && TMath::Abs(probegenjet_eta) < Eta_bins_SM[r+1]);
          cond2 = false;
          SELECT_ETA_ALPHA_BIN_GEN(SM,cond1,cond2)
        }
        for ( int r = 0 ; r < EtaBins_FE_reference ; r++ ) {
          if (is_JER_SM) continue;
          cond1 = (TMath::Abs(barrelgenjet_eta) > 0. && TMath::Abs(barrelgenjet_eta) < s_eta_barr && TMath::Abs(probegenjet_eta) > Eta_bins_FE_reference[r] && TMath::Abs(probegenjet_eta) < Eta_bins_FE_reference[r+1]);
          cond2 = (TMath::Abs(probegenjet_eta) > 0. && TMath::Abs(probegenjet_eta) < s_eta_barr && TMath::Abs(barrelgenjet_eta) > Eta_bins_FE_reference[r] && TMath::Abs(barrelgenjet_eta) < Eta_bins_FE_reference[r+1]);
          SELECT_ETA_ALPHA_BIN_GEN(FE_reference,cond1,cond2)
        }

        for ( int r = 0 ; r < EtaBins_FE_control ; r++ ) {
          if (is_JER_SM) continue;
          cond1 = (TMath::Abs(barrelgenjet_eta) > 0. && TMath::Abs(barrelgenjet_eta) < s_eta_barr && TMath::Abs(probegenjet_eta) > Eta_bins_FE_control[r] && TMath::Abs(probegenjet_eta) < Eta_bins_FE_control[r+1]);
          cond2 = (TMath::Abs(probegenjet_eta) > 0. && TMath::Abs(probegenjet_eta) < s_eta_barr && TMath::Abs(barrelgenjet_eta) > Eta_bins_FE_control[r] && TMath::Abs(barrelgenjet_eta) < Eta_bins_FE_control[r+1]);
          SELECT_ETA_ALPHA_BIN_GEN(FE_control,cond1,cond2)
        }

        break;
      }
    }
  } else {
    dofill=false;
    for ( int k = 0 ; k < PtBins_HF ; k++ ) {
      if ((gen_pt_ave > Pt_bins_HF[k]) && (gen_pt_ave < Pt_bins_HF[k+1])) {
        for ( int r = 0 ; r < EtaBins_SM_control ; r++ ) {
          if (!is_JER_SM) continue;
          cond1 = (TMath::Abs(barrelgenjet_eta) > Eta_bins_SM_control[r] && TMath::Abs(barrelgenjet_eta) < Eta_bins_SM_control[r+1] && TMath::Abs(probegenjet_eta) > Eta_bins_SM_control[r] && TMath::Abs(probegenjet_eta) < Eta_bins_SM_control[r+1]);
          cond2 = false;
          SELECT_ETA_ALPHA_BIN_GEN(SM_control,cond1,cond2)
        }
        for ( int r = 0 ; r < EtaBins_FE ; r++ ) {
          if (is_JER_SM) continue;
          cond1 = (TMath::Abs(barrelgenjet_eta) > 0. && TMath::Abs(barrelgenjet_eta) < s_eta_barr && TMath::Abs(probegenjet_eta) > Eta_bins_FE[r] && TMath::Abs(probegenjet_eta) < Eta_bins_FE[r+1]);
          cond2 = (TMath::Abs(probegenjet_eta) > 0. && TMath::Abs(probegenjet_eta) < s_eta_barr && TMath::Abs(barrelgenjet_eta) > Eta_bins_FE[r] && TMath::Abs(barrelgenjet_eta) < Eta_bins_FE[r+1]);
          SELECT_ETA_ALPHA_BIN_GEN(FE,cond1,cond2)
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
  std::cout <<"\t\tunmatchegGenJets events #" <<  unmatchegGenJets << std::endl;

  std::ofstream mytxtfile;
  mytxtfile.open (outdir+"counts.txt");
  mytxtfile << "Analyzed events #" <<  TotalEvents << "\n";
  mytxtfile << "unmatchegGenJets events #" <<  unmatchegGenJets << "\n";
  mytxtfile.close();

  TFile *fpt = new TFile(outdir+"pt_mc_incl_full.root","RECREATE"); ;
  fpt->cd();
  h_PU->Write();
  h_alpha_raw->Write();
  h_alpha_sel->Write();
  h_PUweight->Write();
  h_rho_SM->Write();
  h_rho_FE->Write();
  h_relerr_SM->Write();
  h_relerr_FE->Write();
  h_JetAvePt_FE->Write();
  h_Jet1Pt_FE->Write();
  h_Jet2Pt_FE->Write();
  h_Jet3Pt_FE->Write();
  h_JetAvePt_SM->Write();
  h_Jet1Pt_SM->Write();
  h_Jet2Pt_SM->Write();
  h_Jet3Pt_SM->Write();
  fpt->Close();

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
          asy_dR_barrel_FE.at(m).at(p).at(r).at(i)->Write();
          asy_dR_probe_FE.at(m).at(p).at(r).at(i)->Write();
        }
      }
    }
  }

  f->Close();
  f1->Close();
  f2->Close();
  f3->Close();
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
