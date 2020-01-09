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

#define FILL_HISTOS(region,method)                                     \
asymmetries_##region.at(r).at(k).at(m) -> Fill( asy , weight );        \
asymmetries_pt_##region.at(r).at(k).at(m) -> Fill( pt_ave, weight );   \
asymmetries_rho_##region.at(r).at(k).at(m) -> Fill( rho, weight );     \
asymmetries_pt3_##region.at(r).at(k).at(m) -> Fill( jet3_pt, weight ); \
if ( m == AlphaBins-1 ) {                                              \
  h_JetAvePt_##method -> Fill( pt_ave, weight );                       \
  h_Jet1Pt_##method -> Fill( jet1_pt, weight );                        \
  h_Jet2Pt_##method -> Fill( jet2_pt, weight );                        \
  h_Jet3Pt_##method -> Fill( jet3_pt, weight );                        \
}                                                                      \

#define SELECT_ETA_ALPHA_BIN(region,method,cond1,cond2,cond3) \
if (cond1 || cond2) {                                         \
  h_alpha_select -> Fill( alpha, 1 );                         \
  for ( int m = 0 ; m < AlphaBins ; m++ ) {                   \
    if ( alpha < Alpha_bins[m] ) {                            \
      double asy = asymmetry;                                 \
      if (cond3) {                                            \
        asy = - asymmetry;                                    \
      }                                                       \
      FILL_HISTOS(region,method)                              \
      if ( excl_bin ) break;                                  \
    }                                                         \
  }                                                           \
  alpha_spectrum_##region.at(r).at(k) -> Fill(alpha, weight); \
}                                                             \

#define WRITE_HISTOS(region)                                  \
for( int m = 0; m < EtaBins_##region; m++ ) {                 \
  f->cd();                                                    \
  asymmetries_##region.at(m).at(p).at(r) -> Write();          \
  asymmetries_pt_##region.at(m).at(p).at(r) -> Write();       \
  f1->cd();                                                   \
  asymmetries_rho_##region.at(m).at(p).at(r) -> Write();      \
  asymmetries_pt3_##region.at(m).at(p).at(r) -> Write();      \
  f_alpha->cd();                                              \
  alpha_spectrum_##region.at(m).at(p) -> Write();             \
}                                                             \

void MakeHistograms(std::vector< std::vector< std::vector< TH1F* > > > &asymmetries, std::vector< std::vector< std::vector< TH1F* > > > &asymmetries_pt, std::vector< std::vector< std::vector< TH1F* > > > &asymmetries_rho, std::vector< std::vector< std::vector< TH1F* > > > &asymmetries_pt3, std::vector< std::vector< TH1F* > > &alpha_spectrum, TString text, TString extraText, int etaBins, int ptBins, int AlphaBins, int etaShift, int ptShift, int alphaShift) {
  for( int m = etaShift; m < etaBins+etaShift; m++ ) {
    std::vector< std::vector< TH1F* > > temp2, temp2pt, temp2rho, temp2pt3;
    std::vector< TH1F* > alpha_temp2;
    for( int p = 0; p < ptBins; p++ ) {
      std::vector< TH1F* > temp1, temp1pt, temp1rho, temp1pt3;
      TString name_alpha = "alpha"; name_alpha += extraText; name_alpha += "_eta"; name_alpha += m+1; name_alpha += "_pt"; name_alpha += p+1;
      TH1F *h1_alpha = new TH1F( name_alpha, name_alpha, 80, 0., 0.8 );
      h1_alpha ->GetYaxis()->SetTitle("a.u.");    h1_alpha ->GetXaxis()->SetTitle("Alpha");
      h1_alpha -> Sumw2(); alpha_temp2.push_back(h1_alpha);
      for( int r = 0; r < AlphaBins; r++ ) {
        TString name     = text;        name      += extraText; name     += "_eta"; name     += m+1; name     += "_pt"; name     += p+1; name     += "_alpha"; name     += r+1;
        TString name_pt  = text+"pt";   name_pt  += extraText; name_pt  += "_eta"; name_pt  += m+1; name_pt  += "_pt"; name_pt  += p+1; name_pt  += "_alpha"; name_pt  += r+1;
        TString name_rho = text+"rho";  name_rho += extraText; name_rho += "_eta"; name_rho += m+1; name_rho += "_pt"; name_rho += p+1; name_rho += "_alpha"; name_rho += r+1;
        TString name_pt3 = text+"pt3";  name_pt3 += extraText; name_pt3 += "_eta"; name_pt3 += m+1; name_pt3 += "_pt"; name_pt3 += p+1; name_pt3 += "_alpha"; name_pt3 += r+1;
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

  h_alpha_raw = new TH1F( "Alpha_raw" , "#alpha before selection" , 80, 0., 0.8 );
  h_alpha_raw -> SetXTitle( "#alpha_raw" );
  h_alpha_raw -> Sumw2();

  h_alpha_select = new TH1F( "Alpha" , "#alpha after selection" , 80, 0., 0.8 );
  h_alpha_select -> SetXTitle( "#alpha" );
  h_alpha_select -> Sumw2();

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

  MakeHistograms(asymmetries_SM,            asymmetries_pt_SM,            asymmetries_rho_SM,            asymmetries_pt3_SM,            alpha_spectrum_SM,            "asymm", "_SM",            EtaBins_SM,            PtBins_Central, AlphaBins, etaShift_SM,            0, 0);
  MakeHistograms(asymmetries_SM_control,    asymmetries_pt_SM_control,    asymmetries_rho_SM_control,    asymmetries_pt3_SM_control,    alpha_spectrum_SM_control,    "asymm", "_SM_control",    EtaBins_SM_control,    PtBins_HF,      AlphaBins, etaShift_SM_control,    0, 0);
  MakeHistograms(asymmetries_FE_reference,  asymmetries_pt_FE_reference,  asymmetries_rho_FE_reference,  asymmetries_pt3_FE_reference,  alpha_spectrum_FE_reference,  "asymm", "_FE_reference",  EtaBins_FE_reference,  PtBins_Central, AlphaBins, etaShift_FE_reference,  0, 0);
  MakeHistograms(asymmetries_FE_control,    asymmetries_pt_FE_control,    asymmetries_rho_FE_control,    asymmetries_pt3_FE_control,    alpha_spectrum_FE_control,    "asymm", "_FE_control",    EtaBins_FE_control,    PtBins_Central, AlphaBins, etaShift_FE_control,    0, 0);
  MakeHistograms(asymmetries_FE,            asymmetries_pt_FE,            asymmetries_rho_FE,            asymmetries_pt3_FE,            alpha_spectrum_FE,            "asymm", "_FE",            EtaBins_FE,            PtBins_HF,      AlphaBins, etaShift_FE,            0, 0);

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

  //2017
  std::vector<double> Eta_bins_SM(            eta_bins_JER + etaShift_SM,            eta_bins_JER + etaShift_SM + EtaBins_SM + 1);
  std::vector<double> Eta_bins_SM_control(    eta_bins_JER + etaShift_SM_control,    eta_bins_JER + etaShift_SM_control + EtaBins_SM_control + 1);
  std::vector<double> Eta_bins_FE_reference(  eta_bins_JER + etaShift_FE_reference,  eta_bins_JER + etaShift_FE_reference + EtaBins_FE_reference + 1);
  std::vector<double> Eta_bins_FE_control(    eta_bins_JER + etaShift_FE_control,    eta_bins_JER + etaShift_FE_control + EtaBins_FE_control + 1);
  std::vector<double> Eta_bins_FE(            eta_bins_JER + etaShift_FE,            eta_bins_JER + etaShift_FE + EtaBins_FE + 1);

  std::vector<int> Pt_bins_Central(pt_bins_Si, pt_bins_Si + sizeof(pt_bins_Si)/sizeof(double));
  std::vector<int> Pt_bins_HF(pt_bins_Si_HF, pt_bins_Si_HF + sizeof(pt_bins_Si_HF)/sizeof(double));
  Pt_bins_Central.push_back(1500);
  Pt_bins_HF.push_back(1500);

  std::vector<double> Alpha_bins;
  Alpha_bins.push_back(0.05); Alpha_bins.push_back(0.1);  Alpha_bins.push_back(0.15); Alpha_bins.push_back(0.20); Alpha_bins.push_back(0.25); Alpha_bins.push_back(0.3);
  bool cond1, cond2, cond3;

  // Triggers are called by index of this list
  bool trigger[PtBins_Central];
  bool ftrigger[PtBins_HF];

  std::fill_n(trigger, PtBins_Central, false);
  std::fill_n(ftrigger, PtBins_HF, false);

  if (pass_trigger_bl) {
    for( int i = 0; i < PtBins_Central; i++ ) {
      if (Pt_bins_Central[i] <= pt_ave && Pt_bins_Central[i + 1] >= pt_ave) {
        trigger[i] = true;
      }
    }
  }
  if (pass_trigger_bl) { // FIXME this is only a temporal solution
    for( int i = 0; i < PtBins_HF; i++ ) {
      if (Pt_bins_HF[i] <= pt_ave && Pt_bins_HF[i + 1] >= pt_ave) {
        ftrigger[i] = true;
      }
    }
  }

  h_PU -> Fill( npuIT, 1 );

  double weight = 1.0;
  double jet_threshold = 15;
  double alpha_raw = alpha_;

  h_alpha_raw -> Fill( alpha_raw, 1 );
  double parallel, perpendicular, complete, alpha;

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

    if ( TMath::Abs(TVector2::Phi_mpi_pi((probejet_phi - barreljet_phi))) > 2.7 ) {
      for ( int k = 0 ; k < PtBins_Central ; k++ ) {
        if (trigger[k]) {
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
      }
      for ( int k = 0 ; k < PtBins_HF ; k++ ) {
        if (ftrigger[k]) {
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
              SELECT_ETA_ALPHA_BIN(FE,FE,cond1,cond2,cond3)
            }

            break;
          }
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

  std::ofstream mytxtfile;
  mytxtfile.open (outdir+"counts.txt");
  mytxtfile << "Analyzed events #" <<  TotalEvents << "\n";
  mytxtfile.close();

  TFile *fpt = new TFile(outdir+"pt_data_incl_full.root","RECREATE"); ;
  fpt->cd();
  h_alpha_raw -> Write();
  h_alpha_select -> Write();
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

  f -> Close();
  f1-> Close();
  f_alpha -> Close();

}

void MySelector::Terminate() {
  // The Terminate() function is the last function to be called during
  // a query. It always runs on the client, it can be used to present
  // the results graphically or save the results to file.

}
