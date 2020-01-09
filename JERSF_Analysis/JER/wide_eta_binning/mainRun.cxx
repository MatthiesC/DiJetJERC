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
#include "functions.C"
#include "tdrstyle_all.C"

double min_fit = 100.;
double max_fit = 1200.;

// Code by Andrea Malara
// Based on code by Marek Niedziela, Matthias Schröder, Kristin Goebel

void MCFIT(TH1* hist, TH1F* mcERR, double &N, double &S, double &C, double &Nerr, double &Serr, double &Cerr, double &mcChi, int &mcNDF) {
  TF1* mcFIT = new TF1( "mcFIT", "TMath::Sqrt( ([0]*[0]/(x*x))+[1]*[1]/x+[2]*[2] )", min_fit, max_fit);
  mcFIT->SetParameters(0.00015, 0.8, 0.04);
  mcFIT->SetParLimits(0, 0., 10.);
  mcFIT->SetParLimits(1, 0., 2.);
  mcFIT->SetParLimits(2, 0., 1.);
  hist-> Fit("mcFIT", "RMQ+");
  N = mcFIT -> GetParameter( 0 );
  S = mcFIT -> GetParameter( 1 );
  C = mcFIT -> GetParameter( 2 );
  Nerr = mcFIT -> GetParError( 0 );
  Serr = mcFIT -> GetParError( 1 );
  Cerr = mcFIT -> GetParError( 2 );
  mcChi = mcFIT->GetChisquare();
  mcNDF = mcFIT->GetNDF();
  mcFIT->SetLineColor(kBlue+2);
  // std::cout << mcERR->GetName() << " "<< mcERR->GetEntries() << '\n';
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(mcERR,0.68);
  mcERR->SetStats(kFALSE);
  mcERR->GetXaxis()->SetRange(min_fit,max_fit);
}


void DTFIT(TH1* hist, TH1F* dtERR, double &N, double &S, double &C, double &kNS, double &kC, double &Nerr, double &Serr, double &Cerr, double &kNSerr, double &kCerr, double &dtChi, int &dtNDF, int m, bool isFE) {
  TF1 * dtFIT = new TF1( "dtFIT", "TMath::Sqrt( ([3]*[3]*[0]*[0]/(x*x))+[3]*[3]*[1]*[1]/x+[4]*[4]*[2]*[2] )", min_fit, max_fit);
  dtFIT -> FixParameter(0, N);
  dtFIT -> FixParameter(1, S);
  dtFIT -> FixParameter(2, C);
  dtFIT -> SetParameter(3, 1.2);
  dtFIT -> SetParameter(4, 1.2);
  dtFIT -> SetParLimits(3, 0.9, 3);
  dtFIT -> SetParLimits(4, 0.9, 3);
  // if (!isFE && m == 3) { dtFIT -> SetParLimits(3, 0.0, 15); dtFIT -> SetParLimits(4, 0.0, 15); }
  // if (!isFE && m == 3) { dtFIT -> SetParameter(0, N); dtFIT -> SetParameter(1, S); dtFIT -> SetParameter(2, C); dtFIT -> FixParameter(3,1); dtFIT -> FixParameter(4, 1); }
  if (hist->GetEntries() != 0) hist -> Fit("dtFIT", "RMQ+");
  else hist->GetListOfFunctions()->Add(dtFIT);

  kNS = dtFIT -> GetParameter( 3 );
  kC = dtFIT -> GetParameter( 4 );
  kNSerr = dtFIT -> GetParError( 3 );
  kCerr = dtFIT -> GetParError( 4 );
  dtChi = dtFIT->GetChisquare();
  dtNDF = dtFIT->GetNDF();
  // std::cout << dtERR->GetName() << " " << dtERR->GetEntries() << '\n';
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(dtERR,0.68);
  dtERR->SetStats(kFALSE);
  dtERR->GetXaxis()->SetRange(min_fit,max_fit);
}

void PLOT_ALLWIDTHS(std::vector< std::vector< TH1F* > > h_data, std::vector< std::vector< TH1F* > > h_MC, std::vector< std::vector< TH1F* > > h_gen, TString outdir) {
  int color_data = kBlue;
  int color_MC = kRed;
  int color_gen = kGreen+2;
  for( unsigned int m = 0; m < h_data.size(); m++ ){
    for( unsigned int p = 0; p < h_data.at(m).size(); p++ ){
      TString canvName  = h_data.at(m).at(p)->GetTitle();
      TString nameXaxis = h_data.at(m).at(p)->GetXaxis()->GetTitle();
      TString nameYaxis = h_data.at(m).at(p)->GetYaxis()->GetTitle();
      std::vector<TH1*> vec;
      vec.push_back(h_data.at(m).at(p));
      vec.push_back(h_MC.at(m).at(p));
      vec.push_back(h_gen.at(m).at(p));
      double x_min, x_max, y_min, y_max;
      findExtreme2(vec, &x_min, &x_max, &y_min, &y_max);
      TCanvas* canv = tdrCanvas(canvName, 0, 0.35, 0, y_max*1.6, nameXaxis, nameYaxis);
      canv->SetTickx(0);
      canv->SetTicky(0);
      tdrDraw(h_data.at(m).at(p), "", kFullCircle, color_data );
      tdrDraw(h_MC.at(m).at(p),   "", kFullCircle, color_MC );
      tdrDraw(h_gen.at(m).at(p),  "", kFullCircle, color_gen );
      canv -> Update();
      canv -> Print(outdir+canvName+".pdf","pdf");
      // canv -> Print(outdir+canvName+".png","png");
    }
  }
}

void PLOT_MCT(std::vector< TH1F* > h_MCTruth, std::vector< TH1F* > h_uncor, std::vector< TH1F* > h_cor, std::vector< TH1F* > h_015 , TString outdir, std::vector<double> eta_bins, bool isFE) {
  int MCTruth      = kMagenta;
  int color_uncor  = kRed;
  int color_cor    = kBlue;
  int color_015    = kGreen+2;
  for( unsigned int m = 0; m < h_cor.size(); m++ ){
    TString canvName  = h_cor.at(m)->GetTitle();
    TString nameXaxis = h_cor.at(m)->GetXaxis()->GetTitle();
    TString nameYaxis = h_cor.at(m)->GetYaxis()->GetTitle();
    nameXaxis = "p_{T} [GeV]";
    std::vector<TH1*> vec;

    if (isFE) vec.push_back(h_MCTruth.at(m+1));
    else vec.push_back(h_MCTruth.at(m));
    vec.push_back(h_MCTruth.at(m));
    vec.push_back(h_uncor.at(m));
    vec.push_back(h_cor.at(m));
    vec.push_back(h_015.at(m));
    double x_min, x_max, y_min, y_max;
    findExtreme(vec, &x_min, &x_max, &y_min, &y_max);
    TCanvas* canv = tdrCanvas(canvName, x_min, x_max, y_min, y_max, nameXaxis, nameYaxis);
    canv->SetTickx(0);
    canv->SetTicky(0);
    tdrDraw(h_MCTruth.at(m),  "", kFullCircle, MCTruth );
    tdrDraw(h_uncor.at(m),    "", kFullCircle, color_uncor );
    tdrDraw(h_cor.at(m),      "", kFullCircle, color_cor );
    tdrDraw(h_015.at(m),      "", kFullCircle, color_015 );

    char legTitle[100];
    TLegend *leg = tdrLeg(0.6,0.7,0.9,0.9);
    sprintf(legTitle,     "#eta #in [%.3f,%.3f]", eta_bins[m], eta_bins[m+1]);
    leg->AddEntry(h_MCTruth.at(m),  "MC Truth",         "lep");
    leg->AddEntry(h_uncor.at(m),    "MC Uncorrelated",  "lep");
    leg->AddEntry(h_cor.at(m),      "MC Correlated",    "lep");
    leg->AddEntry(h_015.at(m),      "MC 015",           "lep");

    if(isFE){
      if (m<9) canvName = canvName(0, canvName.Length()-1);
      else canvName = canvName(0, canvName.Length()-2);
      canvName += (m+2);
    }
    canv->Print(outdir+canvName+".pdf","pdf");
    // canv->Print(outdir+canvName+".png","png");
  }
}

void PLOT_ASY(std::vector< std::vector< std::vector< TH1F* > > > h_data, std::vector< std::vector< std::vector< TH1F* > > > h_MC, std::vector< std::vector< std::vector< TH1F* > > > h_gen, std::vector< std::vector< std::vector< double > > > h_data_width, std::vector< std::vector< std::vector< double > > > h_MC_width, std::vector< std::vector< std::vector< double > > > h_gen_width, std::vector< std::vector< std::vector< double > > > h_data_width_err, std::vector< std::vector< std::vector< double > > > h_MC_width_err, std::vector< std::vector< std::vector< double > > > h_gen_width_err, TString outdir, std::vector<double> eta_bins, std::vector<double> Pt_bins_Central, std::vector<double> Pt_bins_HF, std::vector<double> alpha, std::vector< std::vector< std::vector< double > > > lower_x_data, std::vector< std::vector< std::vector< double > > > upper_x_data, std::vector< std::vector< std::vector< double > > > lower_x, std::vector< std::vector< std::vector< double > > > upper_x, std::vector< std::vector< std::vector< double > > > gen_lower_x, std::vector< std::vector< std::vector< double > > > gen_upper_x) {
  int color_data = kBlue;
  int color_MC = kRed;
  int color_gen = kGreen+2;
  for( unsigned int m = 0; m < h_data.size(); m++ ){
    std::vector<double> Pt_bins;
    if (eta_bins[m]< eta_cut) for (size_t i = 0; i <= h_data.at(m).size(); i++) { Pt_bins.push_back(Pt_bins_Central[i]); }
    else for (size_t i = 0; i <= h_data.at(m).size(); i++) { Pt_bins.push_back(Pt_bins_HF[i]); }
    for( unsigned int p = 0; p < h_data.at(m).size(); p++ ){
      for( unsigned int r = 0; r < h_data.at(m).at(p).size(); r++ ){
        h_data.at(m).at(p).at(r)-> Scale(1./h_data.at(m).at(p).at(r) -> Integral());
        h_MC.at(m).at(p).at(r)  -> Scale(1./h_MC.at(m).at(p).at(r) -> Integral());
        h_gen.at(m).at(p).at(r) -> Scale(1./h_gen.at(m).at(p).at(r) -> Integral());

        TString canvName  = h_data.at(m).at(p).at(r)->GetTitle();
        TString nameXaxis = h_data.at(m).at(p).at(r)->GetXaxis()->GetTitle();
        TString nameYaxis = h_data.at(m).at(p).at(r)->GetYaxis()->GetTitle();
        std::vector<TH1*> vec;
        vec.push_back(h_data.at(m).at(p).at(r));
        vec.push_back(h_MC.at(m).at(p).at(r));
        vec.push_back(h_gen.at(m).at(p).at(r));
        double x_min, x_max, y_min, y_max;
        findExtreme(vec, &x_min, &x_max, &y_min, &y_max);
        // TCanvas* canv = tdrCanvas(canvName, x_min, x_max, y_min, y_max*1.5, nameXaxis, nameYaxis);
        TCanvas* canv = tdrCanvas(canvName, x_min, x_max, 0.00001, 100, nameXaxis, nameYaxis);
        canv->SetLogy();
        canv->SetTickx(0);
        canv->SetTicky(0);
        tdrDraw(h_data.at(m).at(p).at(r), "", kFullCircle, color_data );
        tdrDraw(h_MC.at(m).at(p).at(r),   "", kFullCircle, color_MC );
        tdrDraw(h_gen.at(m).at(p).at(r),  "", kFullCircle, color_gen );
        TLine* line_lower_data = new TLine(lower_x_data.at(m).at(p).at(r), 0.00001, lower_x_data.at(m).at(p).at(r), 1);
        TLine* line_upper_data = new TLine(upper_x_data.at(m).at(p).at(r), 0.00001, upper_x_data.at(m).at(p).at(r), 1);
        TLine* line_lower_MC = new TLine(lower_x.at(m).at(p).at(r), 0.00001, lower_x.at(m).at(p).at(r), 1);
        TLine* line_upper_MC = new TLine(upper_x.at(m).at(p).at(r), 0.00001, upper_x.at(m).at(p).at(r), 1);
        TLine* line_lower_gen = new TLine(gen_lower_x.at(m).at(p).at(r), 0.00001, gen_lower_x.at(m).at(p).at(r), 1);
        TLine* line_upper_gen = new TLine(gen_upper_x.at(m).at(p).at(r), 0.00001, gen_upper_x.at(m).at(p).at(r), 1);

        line_lower_data->SetLineWidth(1); line_lower_data->SetLineColor(color_data); line_lower_data->Draw("same");
        line_upper_data->SetLineWidth(1); line_upper_data->SetLineColor(color_data); line_upper_data->Draw("same");
        line_lower_MC->SetLineWidth(1); line_lower_MC->SetLineColor(color_MC); line_lower_MC->Draw("same");
        line_upper_MC->SetLineWidth(1); line_upper_MC->SetLineColor(color_MC); line_upper_MC->Draw("same");
        line_lower_gen->SetLineWidth(1); line_lower_gen->SetLineColor(color_gen); line_lower_gen->Draw("same");
        line_upper_gen->SetLineWidth(1); line_upper_gen->SetLineColor(color_gen); line_upper_gen->Draw("same");

        TLegend *leg = tdrLeg(0.45,0.7,0.95,0.9);
        tdrHeader(leg, Form("#eta #in [%.3f,%.3f], p_{T} #in [%.0f,%.0f] GeV", eta_bins[m], eta_bins[m+1], Pt_bins[p], Pt_bins[p+1]));

        leg->AddEntry((TObject*)0, Form("#alpha < %.2f", alpha.at(r)), "");
        leg->AddEntry(h_data.at(m).at(p).at(r), Form("data, %.4f +- %.4f", h_data_width.at(m).at(p).at(r), h_data_width_err.at(m).at(p).at(r)), "lep");
        leg->AddEntry(h_MC.at(m).at(p).at(r),   Form("MC,  %.4f +- %.4f",  h_MC_width.at(m).at(p).at(r),   h_MC_width_err.at(m).at(p).at(r)),   "lep");
        leg->AddEntry(h_gen.at(m).at(p).at(r),  Form("gen,  %.4f +- %.4f", h_gen_width.at(m).at(p).at(r),  h_gen_width_err.at(m).at(p).at(r)),  "lep");
        leg->Draw("same");

        canv->Update();
        canv -> Print(outdir+canvName+".pdf","pdf");
        // canv -> Print(outdir+canvName+".png","png");
      }
    }
  }
}

void PLOT_WIDTH(std::vector< std::vector< TH1F* > > h_data, std::vector< std::vector< TH1F* > > h_MC, std::vector< std::vector< TH1F* > > h_gen, TString outdir, std::vector<double> eta_bins, std::vector<double> Pt_bins_Central, std::vector<double> Pt_bins_HF) {
  int color_data = kBlue;
  int color_MC = kRed;
  int color_gen = kGreen+2;
  for( unsigned int m = 0; m < h_data.size(); m++ ){
    std::vector<double> Pt_bins;
    if (eta_bins[m]< eta_cut) for (size_t i = 0; i <= h_data.at(m).size(); i++) { Pt_bins.push_back(Pt_bins_Central[i]); }
    else for (size_t i = 0; i <= h_data.at(m).size(); i++) { Pt_bins.push_back(Pt_bins_HF[i]); }
    for( unsigned int p = 0; p < h_data.at(m).size(); p++ ){
      TString canvName  = h_data.at(m).at(p)->GetTitle();
      TString nameXaxis = h_data.at(m).at(p)->GetXaxis()->GetTitle();
      TString nameYaxis = h_data.at(m).at(p)->GetYaxis()->GetTitle();
      std::vector<TH1*> vec;
      vec.push_back(h_data.at(m).at(p));
      vec.push_back(h_MC.at(m).at(p));
      vec.push_back(h_gen.at(m).at(p));
      double x_min, x_max, y_min, y_max;
      findExtreme2(vec, &x_min, &x_max, &y_min, &y_max);
      TCanvas* canv = tdrCanvas(canvName, 0, 0.35, 0, y_max*1.6, nameXaxis, nameYaxis);
      canv->SetTickx(0);
      canv->SetTicky(0);
      tdrDraw(h_data.at(m).at(p), "", kFullCircle, color_data );
      tdrDraw(h_MC.at(m).at(p),   "", kFullCircle, color_MC );
      tdrDraw(h_gen.at(m).at(p),  "", kFullCircle, color_gen );

      if( h_MC.at(m).at(p)  -> GetEntries() != 0 ) h_MC.at(m).at(p)   -> GetFunction("linfit")->SetLineColor(color_MC);
      if( h_gen.at(m).at(p) -> GetEntries() != 0 ) h_gen.at(m).at(p)  -> GetFunction("linfit")->SetLineColor(color_gen);
      if( h_data.at(m).at(p)-> GetEntries() != 0 ) h_data.at(m).at(p) -> GetFunction("linfit")->SetLineColor(color_data);

      TF1* f;
      char line[100];
      TLegend *legend;

      if (h_MC.at(m).at(p) -> GetFunction("linfit")) {
        f = h_MC.at(m).at(p) -> GetFunction("linfit");
        legend = tdrLeg(0.50,0.70,0.70,0.85, 0.025, 42, color_MC);
        tdrHeader(legend,"MC", 22);
        sprintf(line, "#chi^{2}/ndf = %.2f/%d", f->GetChisquare(),  f->GetNDF());       legend->AddEntry((TObject*)0, line, "");
        sprintf(line, "p0 = %.5f #pm %.5f",     f->GetParameter(0), f->GetParError(0)); legend->AddEntry((TObject*)0, line, "");
        sprintf(line, "p1 = %.5f #pm %.5f",     f->GetParameter(1), f->GetParError(1)); legend->AddEntry((TObject*)0, line, "");
        legend->Draw("same");
      }

      if (h_gen.at(m).at(p) -> GetFunction("linfit")) {
        f = h_gen.at(m).at(p) -> GetFunction("linfit");
        legend = tdrLeg(0.70,0.70,0.90,0.85, 0.025, 42, color_gen);
        tdrHeader(legend,"gen", 22);
        sprintf(line, "#chi^{2}/ndf = %.2f/%d", f->GetChisquare(), f->GetNDF());      legend->AddEntry((TObject*)0, line, "");
        sprintf(line, "p0 = %.5f #pm %.5f", f->GetParameter(0), f->GetParError(0));   legend->AddEntry((TObject*)0, line, "");
        sprintf(line, "p1 = %.5f #pm %.5f", f->GetParameter(1), f->GetParError(1));    legend->AddEntry((TObject*)0, line, "");
        legend->Draw("same");
      }

      if (h_data.at(m).at(p) -> GetFunction("linfit")) {
        f = h_data.at(m).at(p) -> GetFunction("linfit");
        legend = tdrLeg(0.30,0.70,0.50,0.85, 0.025, 42, color_data);
        tdrHeader(legend,"Data", 22);
        sprintf(line, "#chi^{2}/ndf = %.2f/%d", f->GetChisquare(), f->GetNDF());      legend->AddEntry((TObject*)0, line, "");
        sprintf(line, "p0 = %.5f #pm %.5f", f->GetParameter(0), f->GetParError(0));   legend->AddEntry((TObject*)0, line, "");
        sprintf(line, "p1 = %.5f #pm %.5f", f->GetParameter(1), f->GetParError(1));    legend->AddEntry((TObject*)0, line, "");
        legend->Draw("same");
      }

      TLegend *leg = tdrLeg(0.35,0.85,0.9,0.89, 0.025, 42, kBlack);
      tdrHeader(leg, Form("#eta #in [%.3f,%.3f], p_{T} #in [%.0f,%.0f] GeV", eta_bins[m], eta_bins[m+1], Pt_bins[p], Pt_bins[p+1]));
      leg->Draw("same");

      canv -> Update();
      canv -> Print(outdir+canvName+".pdf","pdf");
      // canv -> Print(outdir+canvName+".png","png");
    }
  }
}



void PLOT_WIDTH_gr(std::vector< std::vector< TGraphErrors* > > h_data, std::vector< std::vector< TGraphErrors* > > h_MC, std::vector< std::vector< TGraphErrors* > > h_gen, TString outdir, std::vector<double> eta_bins, std::vector<double> Pt_bins_Central, std::vector<double> Pt_bins_HF, bool isFE) {
  int color_data = kBlue;
  int color_MC = kRed;
  int color_gen = kGreen+2;
  for( unsigned int m = 0; m < h_data.size(); m++ ){
    std::vector<double> Pt_bins;
    if (eta_bins[m]< eta_cut) for (size_t i = 0; i <= h_data.at(m).size(); i++) { Pt_bins.push_back(Pt_bins_Central[i]); }
    else for (size_t i = 0; i <= h_data.at(m).size(); i++) { Pt_bins.push_back(Pt_bins_HF[i]); }
    for( unsigned int p = 0; p < h_data.at(m).size(); p++ ){

      double range = 0.05;
      if ( p == 0) range = 0.25;
      if ( p == 1) range = 0.2;
      if ( p == 2) range = 0.15;
      if ( p == 3) range = 0.15;
      if ( p == 4) range = 0.1;
      if ( p == 5) range = 0.1;

      if (!isFE && m >= 6 && p == 2) range = 0.2;
      if (!isFE && m >= 2 && p == 3) range = 0.15;
      if (!isFE && m >= 4 && p >= 6) range = 0.1;
      if ( isFE && m >= 11&& p == 0) range = 0.2;
      if ( isFE && m == 10&& p == 8) range = 0.15;

      TString canvName  = h_data.at(m).at(p)->GetTitle();
      TString nameXaxis = "#alpha_{max}";
      TString nameYaxis = "#sigma_{A}";
      std::vector<TGraphErrors*> vec;
      double x_min, x_max, y_min, y_max;
      vec.push_back(h_data.at(m).at(p));
      vec.push_back(h_MC.at(m).at(p));
      vec.push_back(h_gen.at(m).at(p));
      findExtreme2(vec, &x_min, &x_max, &y_min, &y_max);

      TCanvas* canv = tdrCanvas(canvName, 0, 0.35, 0, y_max*1.6, nameXaxis, nameYaxis);
      canv->SetTickx(0);
      canv->SetTicky(0);
      tdrDraw(h_data.at(m).at(p), "p ", kFullCircle, color_data );
      tdrDraw(h_MC.at(m).at(p),   "p ", kFullCircle, color_MC );
      tdrDraw(h_gen.at(m).at(p),  "p ", kFullCircle, color_gen );

      TF1* f;
      TF1* Temp = new TF1();
      char line[100];
      TLegend *legend;

      if (h_MC.at(m).at(p) -> GetFunction("lin_extrapol_mc")) {
        f = h_MC.at(m).at(p) -> GetFunction("lin_extrapol_mc"); f->SetLineStyle(2); f->SetLineColor(color_MC);
        Temp = (TF1*) h_MC.at(m).at(p)->GetFunction("lin_extrapol_mc")->Clone(); gStyle -> SetOptFit(0000);
        Temp->SetRange(range,1); Temp->SetLineStyle(1); Temp->Draw("same");
        legend = tdrLeg(0.50,0.70,0.70,0.85, 0.025, 42, color_MC);
        tdrHeader(legend,"MC", 22);
        sprintf(line, "#chi^{2}/ndf = %.2f/%d", f->GetChisquare(),  f->GetNDF());       legend->AddEntry((TObject*)0, line, "");
        sprintf(line, "p0 = %.5f #pm %.5f",     f->GetParameter(0), f->GetParError(0)); legend->AddEntry((TObject*)0, line, "");
        sprintf(line, "p1 = %.5f #pm %.5f",     f->GetParameter(1), f->GetParError(1)); legend->AddEntry((TObject*)0, line, "");
        legend->Draw("same");
      }

      if (h_gen.at(m).at(p) -> GetFunction("lin_extrapol_mc")) {

        f = h_gen.at(m).at(p) -> GetFunction("lin_extrapol_mc"); f->SetLineStyle(2); f->SetLineColor(color_gen);
        Temp = (TF1*) h_gen.at(m).at(p)->GetFunction("lin_extrapol_mc")->Clone(); gStyle -> SetOptFit(0000);
        Temp->SetRange(range,1); Temp->SetLineStyle(1); Temp->Draw("same");
        legend = tdrLeg(0.70,0.70,0.90,0.85, 0.025, 42, color_gen);
        tdrHeader(legend,"gen", 22);
        sprintf(line, "#chi^{2}/ndf = %.2f/%d", f->GetChisquare(),  f->GetNDF());       legend->AddEntry((TObject*)0, line, "");
        sprintf(line, "p0 = %.5f #pm %.5f",     f->GetParameter(0), f->GetParError(0)); legend->AddEntry((TObject*)0, line, "");
        sprintf(line, "p1 = %.5f #pm %.5f",     f->GetParameter(1), f->GetParError(1)); legend->AddEntry((TObject*)0, line, "");
        legend->Draw("same");
      }

      if (h_data.at(m).at(p) -> GetFunction("lin_extrapol_mc")) {

        f = h_data.at(m).at(p) -> GetFunction("lin_extrapol_mc"); f->SetLineStyle(2); f->SetLineColor(color_data);
        Temp = (TF1*) h_data.at(m).at(p)->GetFunction("lin_extrapol_mc")->Clone(); gStyle -> SetOptFit(0000);
        Temp->SetRange(range,1); Temp->SetLineStyle(1); Temp->Draw("same");

        legend = tdrLeg(0.30,0.70,0.50,0.85, 0.025, 42, color_data);
        tdrHeader(legend,"Data", 22);
        sprintf(line, "#chi^{2}/ndf = %.2f/%d", f->GetChisquare(),  f->GetNDF());       legend->AddEntry((TObject*)0, line, "");
        sprintf(line, "p0 = %.5f #pm %.5f",     f->GetParameter(0), f->GetParError(0)); legend->AddEntry((TObject*)0, line, "");
        sprintf(line, "p1 = %.5f #pm %.5f",     f->GetParameter(1), f->GetParError(1)); legend->AddEntry((TObject*)0, line, "");
        legend->Draw("same");
      }

      TLegend *leg = tdrLeg(0.35,0.85,0.9,0.89, 0.025, 42, kBlack);
      tdrHeader(leg, Form("#eta #in [%.3f,%.3f], p_{T} #in [%.0f,%.0f] GeV", eta_bins[m], eta_bins[m+1], Pt_bins[p], Pt_bins[p+1]));
      leg->Draw("same");

      TString add_name = "Extrapol_";
      if (isFE) add_name = "Extrapol_FE_";

      canv->Print(outdir+add_name+Form("Eta%i_pt%i.pdf", m+1, p+1));
      // canv->Print(outdir+add_name+Form("Eta%i_pt%i.png", m+1, p+1));

      // if( h_MC.at(m).at(p)  -> GetEntries() != 0 ) h_MC.at(m).at(p)   -> GetFunction("linfit")->SetLineColor(color_MC);
      // if( h_gen.at(m).at(p) -> GetEntries() != 0 ) h_gen.at(m).at(p)  -> GetFunction("linfit")->SetLineColor(color_gen);
      // if( h_data.at(m).at(p)-> GetEntries() != 0 ) h_data.at(m).at(p) -> GetFunction("linfit")->SetLineColor(color_data);

    }
  }
}

void SFtoTXT(std::ofstream& texfile, std::vector< TH1F* > h_JER, std::vector< std::vector< std::vector< double > > > width_pt, std::vector<double> &SF, std::vector<double> &SF_err, std::vector<double> &SF_ptdep_min, std::vector<double> &SF_ptdep_max, std::vector<double> &eta_bin_center, std::vector<double> &eta_bin_err, std::vector<double> eta_bins, int shift, bool isFE, bool isCorr ) {
  for( unsigned int m = 0; m < h_JER.size(); m++ ){
    TF1 * constfit = h_JER.at(m) -> GetFunction("constfit");
    TF1 * NSC_ratio = h_JER.at(m) -> GetFunction("NSC_ratio");
    h_JER.at(m)->GetFunction("NSC_ratio")->SetBit(TF1::kNotDraw);
    if (constfit==0)  continue;
    // std::cout << findMinMax(h_JER.at(m), width_pt.at(m), NSC_ratio, constfit, 1) << " " <<  findMinMax(h_JER.at(m), width_pt.at(m), NSC_ratio, constfit, 0) << '\n';
    SF_ptdep_min.push_back(findMinMax(h_JER.at(m), width_pt.at(m), NSC_ratio, constfit, 1));
    SF_ptdep_max.push_back(findMinMax(h_JER.at(m), width_pt.at(m), NSC_ratio, constfit, 0));
    h_JER.at(m) -> Write();
    int diff = 0;
    if (isFE) diff = shift - h_JER.size();
    if (!isCorr) {
      eta_bin_center.push_back((eta_bins[diff+m+1]+eta_bins[diff+m]) /2);
      eta_bin_err.push_back((eta_bins[diff+m+1]-eta_bins[diff+m]) /2);
    }
    SF.push_back(constfit -> GetParameter( 0 ));
    SF_err.push_back(constfit -> GetParError( 0 ));
    texfile << constfit -> GetParameter( 0 ) << " \\pm " << constfit -> GetParError( 0 ) << " & ";
    if(m == h_JER.size()-1 ) texfile << "\\\\";
  }
}

void PLOT_SF(std::vector< TH1F* > h_uncor, std::vector< TH1F* > h_cor, std::vector< TH1F* > h_015, TString outdir, std::vector<double> eta_bins, bool isFE) {
  int color_uncor = kBlue;
  int color_cor   = kRed;
  int color_015   = kGreen+2;
  for( unsigned int m = 0; m < h_uncor.size(); m++ ){
    TString canvName  = h_uncor.at(m)->GetTitle();
    TString nameXaxis = "p_{T} [GeV]";
    TString nameYaxis = h_uncor.at(m)->GetYaxis()->GetTitle();
    std::vector<TH1*> vec;
    vec.push_back(h_uncor.at(m));
    vec.push_back(h_cor.at(m));
    vec.push_back(h_015.at(m));
    double x_min, x_max, y_min, y_max;
    findExtreme(vec, &x_min, &x_max, &y_min, &y_max);
    TCanvas* canv = tdrCanvas(canvName, x_min, x_max, 0, 3, nameXaxis, nameYaxis);
    canv->SetTickx(0);
    canv->SetTicky(0);
    tdrDraw(h_uncor.at(m), "", kFullCircle, color_uncor );
    tdrDraw(h_cor.at(m), "", kFullCircle, color_cor );
    tdrDraw(h_015.at(m), "", kFullCircle, color_015 );

    TF1* f;
    char line[100];
    TLegend *legend;

    if(h_uncor.at(m)->GetFunction("constfit")){
      f = h_uncor.at(m) -> GetFunction("constfit"); f->SetLineColor(color_uncor);
      legend = tdrLeg(0.30,0.70,0.50,0.90, 0.025, 42, color_uncor);
      tdrHeader(legend,"JER", 22);
      sprintf(line, "#chi^{2}/ndf = %.2f/%d", f->GetChisquare(), f->GetNDF());        legend->AddEntry((TObject*)0, line, "");
      sprintf(line, "p0 = %.5f #pm %.5f",     f->GetParameter(0), f->GetParError(0)); legend->AddEntry((TObject*)0, line, "");
      legend->Draw("same");
    } else { std::cout << "Fit uncor function at bin " << m << " was not found" << std::endl; }

    if(h_cor.at(m)->GetFunction("constfit")){
      f = h_cor.at(m) -> GetFunction("constfit"); f->SetLineColor(color_cor);
      legend = tdrLeg(0.50,0.70,0.70,0.90, 0.025, 42, color_cor);
      tdrHeader(legend,"JER cor", 22);
      sprintf(line, "#chi^{2}/ndf = %.2f/%d", f->GetChisquare(), f->GetNDF());        legend->AddEntry((TObject*)0, line, "");
      sprintf(line, "p0 = %.5f #pm %.5f",     f->GetParameter(0), f->GetParError(0)); legend->AddEntry((TObject*)0, line, "");
      legend->Draw("same");
    } else { std::cout << "Fit cor function at bin " << m << " was not found" << std::endl; }

    if(h_015.at(m)->GetFunction("constfit")){
      f = h_015.at(m) -> GetFunction("constfit"); f->SetLineColor(color_015);
      legend = tdrLeg(0.70,0.70,0.90,0.90, 0.025, 42, color_uncor);
      tdrHeader(legend,"JER 0.15", 22);
      sprintf(line, "#chi^{2}/ndf = %.2f/%d", f->GetChisquare(), f->GetNDF());        legend->AddEntry((TObject*)0, line, "");
      sprintf(line, "p0 = %.5f #pm %.5f",     f->GetParameter(0), f->GetParError(0)); legend->AddEntry((TObject*)0, line, "");
      legend->Draw("same");
    } else { /*std::cout << "Fit 015 function at bin " << m << " was not found" << std::endl;*/ }


    TLegend *leg = tdrLeg(0.6,0.15,0.9,0.35, 0.04, 42, kBlack);
    tdrHeader(leg, Form("#eta #in [%.3f,%.3f]", eta_bins[m], eta_bins[m+1]));
    leg->AddEntry(h_uncor.at(m),  "#sigma_{JER}^{data}/#sigma_{JER}^{MC}","lep");
    leg->AddEntry(h_cor.at(m),    "#sigma_{JER}^{data}/#sigma_{JER}^{MC} correlated","lep");
    leg->AddEntry(h_015.at(m),    "#sigma_{0.15}^{data}/#sigma_{0.15}^{MC}","lep");
    leg->Draw("same");

    if (isFE) {
      if (m<9) canvName = canvName(0, canvName.Length()-1);
      else canvName = canvName(0, canvName.Length()-2);
      canvName += (m+2);
    }
    canv -> Print(outdir+canvName+".pdf","pdf");
    // canv -> Print(outdir+canvName+".png","png");
  }
}

void PLOT_NCS(std::vector< TH1F* > h_data, std::vector< TH1F* > h_MC, std::vector< TH1F* > h_SF, TString outdir, std::vector<double> eta_bins, bool isFE) {
  int color_data  = kBlue;
  int color_MC    = kRed;
  int color_NSC   = kGreen+2;
  for( unsigned int m = 0; m < h_data.size(); m++ ){
    h_data.at(m)->SetStats(kFALSE);
    h_MC.at(m)->SetStats(kFALSE);
    double N, S, C, kNS, kC, Nerr, Serr, Cerr, kNSerr, kCerr, mcChi, dtChi;
    int mcNDF, dtNDF;
    TH1F* mcERR = (TH1F*)h_data.at(m)->Clone();
    // TH1F* mcERR = (TH1F*)h_MC.at(m)->Clone();
    TH1F* dtERR = (TH1F*)h_data.at(m)->Clone();
    MCFIT(h_MC.at(m), mcERR, N, S, C, Nerr, Serr, Cerr, mcChi, mcNDF);
    DTFIT(h_data.at(m), dtERR, N, S, C, kNS, kC, Nerr, Serr, Cerr, kNSerr, kCerr, dtChi, dtNDF, m, isFE);
    if (h_data.at(m)-> GetFunction("dtFIT")!=0) h_data.at(m)-> GetFunction("dtFIT")->SetLineColor(color_data+2);
    if (h_MC.at(m)-> GetFunction("mcFIT")!=0) h_MC.at(m)-> GetFunction("mcFIT")->SetLineColor(color_MC+2);
    mcERR->SetFillColorAlpha(color_MC+2,0.35);
    dtERR->SetFillColorAlpha(color_data+2,0.35);
    TString canvName  = h_data.at(m)->GetTitle();
    TString nameXaxis = h_data.at(m)->GetXaxis()->GetTitle();
    TString nameYaxis = h_data.at(m)->GetYaxis()->GetTitle();
    std::vector<TH1*> vec;
    vec.push_back(h_data.at(m));
    vec.push_back(h_MC.at(m));
    double x_min, x_max, y_min, y_max;
    findExtreme(vec, &x_min, &x_max, &y_min, &y_max);
    TCanvas* canv = tdrCanvas(canvName, x_min, x_max, y_min, y_max, nameXaxis, nameYaxis);
    canv->SetTickx(0);
    canv->SetTicky(0);
    mcERR->Draw("E4 SAME");
    dtERR->Draw("E4 SAME");
    tdrDraw(h_data.at(m), "", kFullCircle, color_data );
    tdrDraw(h_MC.at(m), "", kFullCircle, color_MC );
    TLegend *leg = tdrLeg(0.6,0.7,0.9,0.9);
    char legTitle[100];
    sprintf(legTitle,     "#eta #in [%.3f,%.3f]", eta_bins[m], eta_bins[m+1]);
    tdrHeader(leg,legTitle);
    leg->AddEntry(h_data.at(m),"data","lep");
    leg->AddEntry(h_MC.at(m),"MC","lep");
    char line[100];
    TLegend *legend;
    legend = tdrLeg(0.50,0.55,0.70,0.7, 0.025, 42, color_MC);
    legend->AddEntry((TObject*)0, "MC", "");
    sprintf(line, "#chi^{2}/ndf = %.2f/%d", mcChi,mcNDF); legend->AddEntry((TObject*)0,line,"");
    sprintf(line, "N = %.5f #pm %.5f", N, Nerr);  legend->AddEntry((TObject*)0, line, "");
    sprintf(line, "S = %.5f #pm %.5f", S, Serr);  legend->AddEntry((TObject*)0, line, "");
    sprintf(line, "C = %.5f #pm %.5f", C, Cerr);  legend->AddEntry((TObject*)0, line, "");
    legend->Draw("same");
    legend = tdrLeg(0.70,0.55,0.85,0.7, 0.025, 42, color_data);
    legend->AddEntry((TObject*)0, "data", "");
    sprintf(line, "#chi^{2}/ndf = %.2f/%d", dtChi,dtNDF); legend->AddEntry((TObject*)0,line,"");
    sprintf(line, "k_{NS} = %.5f #pm %.5f", kNS,kNSerr);legend->AddEntry((TObject*)0, line, "");
    sprintf(line, "k_{C}  = %.5f #pm %.5f", kC, kCerr); legend->AddEntry((TObject*)0, line, "");
    legend->AddEntry((TObject*)0, "", "");
    legend->Draw("same");
    if(isFE){
      if (m<9) canvName = canvName(0, canvName.Length()-1);
      else canvName = canvName(0, canvName.Length()-2);
      canvName += (m+2);
    }
    canv -> Print(outdir+"pdfy/JERs/"+canvName+".pdf","pdf");
    // canv -> Print(outdir+"pdfy/JERs/"+canvName+".png","png");
    // canv -> Print(outdir+"pdfy/JERs/"+canvName+".root","root");

    canvName  = h_SF.at(m)->GetTitle();
    nameXaxis = h_SF.at(m)->GetXaxis()->GetTitle();
    nameYaxis = h_SF.at(m)->GetYaxis()->GetTitle();
    vec.clear();
    vec.push_back(h_SF.at(m));
    findExtreme(vec, &x_min, &x_max, &y_min, &y_max);
    canv = tdrCanvas(canvName, x_min, x_max, y_min, y_max, nameXaxis, nameYaxis);
    canv->SetTickx(0);
    canv->SetTicky(0);
    tdrDraw(h_SF.at(m), "", kFullCircle, color_data );
    TF1 * constfit = new TF1( "constfit", "pol0", min_fit, max_fit );
    constfit->SetLineColor(color_MC);
    constfit->SetLineWidth(2);
    if (h_SF.at(m)->GetEntries() != 0) h_SF.at(m) -> Fit("constfit","RMQ+");
    else h_SF.at(m)->GetListOfFunctions()->Add(constfit);
    TF1 *NSC_ratio = new TF1("NSC_ratio", "TMath::Sqrt( ([3]*[3]*[0]*[0]/(x*x))+[3]*[3]*[1]*[1]/x+[4]*[4]*[2]*[2] )/TMath::Sqrt( ([0]*[0]/(x*x))+[1]*[1]/x+[2]*[2] )",min_fit,max_fit);
    NSC_ratio -> SetParameters(N, S, C, kNS, kC);
    NSC_ratio->SetLineColor(color_NSC);
    NSC_ratio->SetLineWidth(2);
    NSC_ratio->Draw("same");
    h_SF.at(m)->GetListOfFunctions()->Add(NSC_ratio);
    leg = tdrLeg(0.55,0.7,0.9,0.9);
    sprintf(legTitle,     "#eta #in [%.3f,%.3f]", eta_bins[m], eta_bins[m+1]);
    tdrHeader(leg,legTitle);
    leg->AddEntry(h_SF.at(m),"#sigma_{JER}^{data}/#sigma_{JER}^{mc} correlated","lep");
    leg->AddEntry(constfit,"Constant Fit","l");
    sprintf(legTitle,     "#eta #in [%.3f,%.3f]", eta_bins[m], eta_bins[m+1]);
    leg->AddEntry(NSC_ratio,"Ratio of NSC-Fits","l");
    if(isFE){
      if (m<9) canvName = canvName(0, canvName.Length()-1);
      else canvName = canvName(0, canvName.Length()-2);
      canvName += (m+2);
    }
    canv -> Print(outdir+"pdfy/NSC_SFs/NSC"+canvName+".pdf","pdf");
    // canv -> Print(outdir+"pdfy/NSC_SFs/NSC"+canvName+".png","png");
    canv->Write();
  }
}

// ------------------------------
//          MAIN PROGRAM
// ------------------------------
//
// bool data_ = false;
// bool real_data = true;
// const char* filename = "/nfs/dust/cms/user/amalara/WorkingArea/UHH2_102X_v1/CMSSW_10_2_10/src/UHH2/JERSF/Analysis/hist_preparation/MC/wide_eta_bin/file/Single/Fall17_17Nov2017_V10/AK4CHS/histograms_mc_incl_full.root"
// const char* filename_data = "/nfs/dust/cms/user/amalara/WorkingArea/UHH2_102X_v1/CMSSW_10_2_10/src/UHH2/JERSF/Analysis/hist_preparation/data/wide_eta_bin/file/Single/Fall17_17Nov2017_V10/AK4CHS/RunBCDEF/histograms_data_incl_full.root"
//
// TString Trigger = "Single"
//
// double gaustails = 0.985
// float shiftForPLI = 0.0
// int ref_shift = 3
// int shift = ref_shift

int mainRun( bool data_, const char* filename, const char* filename_data, TString lumi, TString label_mc, TString label_data, TString Trigger, TString outdir, double gaustails = 0.985, float shiftForPLI = 0.0, int ref_shift = 3){

  // bool debug = true;
  bool debug = false;

  gPrintViaErrorHandler = kTRUE;
  gErrorIgnoreLevel = kFatal;
  double hist_max_value = 0.3;
  double hist_max_value_SF = 3.0;
  std::cout << "Start" << '\n';

  ////////////////////////////////////////////////////////////////////////////
  //    I load all histograms I will need                                   //
  ////////////////////////////////////////////////////////////////////////////

  bool real_data = true; // If false: compare MC to MC:
  // read out genlevel info from both MC and data.
  // If true: compare MC to data:
  // use MC gen info for data as well.
  //bool data = false;     // MC is not data and gets this boolean.

  if ( !real_data && (label_data == "Data" || label_data == "data")){
    std::cout << "WARNING: are you using data?" << std::endl;
    std::cout << "Program has set real_data=" << real_data << std::endl;
    std::cout << "PLI will be corrected with MC gen info only: " << real_data <<std::endl;
  }

  // ------------------------------
  //           bin values
  // ------------------------------

  bool isAK8 = outdir.Contains("AK8");
  std::string year = outdir.Contains("Autumn18")? "2018": (outdir.Contains("Fall17")? "2017": (outdir.Contains("Summer16")? "2016": "" )) ;
  std::cout << outdir << "\t" << isAK8 << "\t" << year << "\n";

  int EtaBins_SM            = std::count_if(eta_bins_JER, eta_bins_JER+n_eta_bins_JER, [](double i) { return i<eta_cut; });; // st method bins
  int EtaBins_SM_control    = std::count_if(eta_bins_JER, eta_bins_JER+n_eta_bins_JER, [](double i) { return i>eta_cut; });; // st method bins control
  int EtaBins_FE_reference  = std::count_if(eta_bins_JER, eta_bins_JER+n_eta_bins_JER, [](double i) { return i<s_eta_barr;});; // fe method bins reference
  int EtaBins_FE_control    = std::count_if(eta_bins_JER, eta_bins_JER+n_eta_bins_JER, [](double i) { return (i>=s_eta_barr)&&(i<eta_cut);});; // fe method bins control
  int EtaBins_FE            = std::count_if(eta_bins_JER, eta_bins_JER+n_eta_bins_JER, [](double i) { return i>eta_cut; });; // fe method bins

  std::cout << "EtaBins_SM " << EtaBins_SM << '\n';
  std::cout << "EtaBins_SM_control " << EtaBins_SM_control << '\n';
  std::cout << "EtaBins_FE_reference " << EtaBins_FE_reference << '\n';
  std::cout << "EtaBins_FE_control " << EtaBins_FE_control << '\n';
  std::cout << "EtaBins_FE " << EtaBins_FE << '\n';

  int shift_ = EtaBins_SM + EtaBins_FE;// TODO recheck
  if (debug) std::cout << "shift_: " << shift_ << '\n';

  int etaShift_SM           = 0;
  int etaShift_SM_control   = EtaBins_SM;
  int etaShift_FE_reference = 0;
  int etaShift_FE_control   = EtaBins_FE_reference;
  int etaShift_FE           = EtaBins_FE_reference + EtaBins_FE_control;

  int PtBins_Central = 9, PtBins_HF = 6, AlphaBins = 6;

  std::vector<double> Pt_bins_Central, Pt_bins_HF;

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



  std::cout << "Trigger " << Trigger << " " << Pt_bins_Central.size() << " " << Pt_bins_HF.size() << '\n';
  std::cout << "Pt_bins_Central\t"; for (size_t i = 0; i < Pt_bins_Central.size(); i++) std::cout << Pt_bins_Central[i] << '\t'; std::cout << '\n';
  std::cout << "Pt_bins_HF\t";  for (size_t i = 0; i < Pt_bins_HF.size(); i++) std::cout << Pt_bins_HF[i] << '\t'; std::cout << '\n';

  std::vector<double> eta_bins_edge_SM(eta_bins_JER, eta_bins_JER + sizeof(eta_bins_JER)/sizeof(double));
  std::vector<double> eta_bins_edge_FE(eta_bins_JER+1, eta_bins_JER + sizeof(eta_bins_JER)/sizeof(double));

  // std::vector<double> eta_bins_edge_SM(eta_bins2, eta_bins2 + sizeof(eta_bins2)/sizeof(double));
  // std::vector<double> eta_bins_edge_FE(eta_bins2, eta_bins2 + sizeof(eta_bins2)/sizeof(double));


  std::cout << "Eta bins " << Trigger << " " << eta_bins_edge_SM.size() << " " << eta_bins_edge_FE.size() << '\n';
  for (size_t i = 0; i < eta_bins_edge_SM.size(); i++) std::cout << eta_bins_edge_SM[i] << '\t'; std::cout << '\n';
  for (size_t i = 0; i < eta_bins_edge_FE.size(); i++) std::cout << eta_bins_edge_FE[i] << '\t'; std::cout << '\n';

  // EtaBins_FE   = 3;
  // EtaBins_SM       = n_eta_bins - EtaBins_FE - 1;
  // etaShift_FE_control  = eta_bins_edge_FE.size() -1 ;

  std::vector<double> alpha;
  alpha.push_back(0.05); alpha.push_back(0.1);  alpha.push_back(0.15);
  alpha.push_back(0.20); alpha.push_back(0.25); alpha.push_back(0.3);

  TFile *f, *f_data;
  f = new TFile( filename, "READ");
  f_data = new TFile( filename_data, "READ");

  std::cout << "filename MC " << filename << '\n';
  std::cout << "filename DATA " << filename_data << '\n';

  // ------------------------------
  //      loading histograms
  // ------------------------------

  std::vector< std::vector< std::vector< TH1F* > > > asymmetries_SM, gen_asymmetries_SM, asymmetries_data_SM, MC_Truth_asymmetries_SM;
  std::vector< std::vector< std::vector< TH1F* > > > asymmetries_pt_SM, gen_asymmetries_pt_SM, asymmetries_pt_data_SM;

  std::vector< std::vector< std::vector< TH1F* > > > asymmetries_FE, gen_asymmetries_FE, asymmetries_data_FE, MC_Truth_asymmetries_FE;
  std::vector< std::vector< std::vector< TH1F* > > > asymmetries_pt_FE, gen_asymmetries_pt_FE, asymmetries_pt_data_FE;

  std::vector< std::vector< std::vector< TH1F* > > > gen_asymmetries_data_SM, gen_asymmetries_data_FE, gen_asymmetries_pt_data_SM, gen_asymmetries_pt_data_FE;

  std::vector< std::vector< std::vector< TH1F* > > > dummy_hists;

  // real_data = true, data = false
  histLoadAsym( *f_data,  real_data,  "asymm_SM",           asymmetries_data_SM,  gen_asymmetries_data_SM,  EtaBins_SM,           PtBins_Central, AlphaBins, etaShift_SM);
  histLoadAsym( *f,       data_,      "asymm_SM",           asymmetries_SM,       gen_asymmetries_SM,       EtaBins_SM,           PtBins_Central, AlphaBins, etaShift_SM);
  histLoadAsym( *f_data,  real_data,  "asymm_SM_control",   asymmetries_data_SM,  gen_asymmetries_data_SM,  EtaBins_SM_control,   PtBins_HF,      AlphaBins, etaShift_SM_control);
  histLoadAsym( *f,       data_,      "asymm_SM_control",   asymmetries_SM,       gen_asymmetries_SM,       EtaBins_SM_control,   PtBins_HF,      AlphaBins, etaShift_SM_control);
  histLoadAsym( *f_data,  real_data,  "asymm_FE_reference", asymmetries_data_FE,  gen_asymmetries_data_FE,  EtaBins_FE_reference, PtBins_Central, AlphaBins, etaShift_FE_reference);
  histLoadAsym( *f,       data_,      "asymm_FE_reference", asymmetries_FE,       gen_asymmetries_FE,       EtaBins_FE_reference, PtBins_Central, AlphaBins, etaShift_FE_reference);
  histLoadAsym( *f_data,  real_data,  "asymm_FE_control",   asymmetries_data_FE,  gen_asymmetries_data_FE,  EtaBins_FE_control,   PtBins_Central, AlphaBins, etaShift_FE_control);
  histLoadAsym( *f,       data_,      "asymm_FE_control",   asymmetries_FE,       gen_asymmetries_FE,       EtaBins_FE_control,   PtBins_Central, AlphaBins, etaShift_FE_control);
  histLoadAsym( *f_data,  real_data,  "asymm_FE",           asymmetries_data_FE,  gen_asymmetries_data_FE,  EtaBins_FE,           PtBins_HF,      AlphaBins, etaShift_FE);
  histLoadAsym( *f,       data_,      "asymm_FE",           asymmetries_FE,       gen_asymmetries_FE,       EtaBins_FE,           PtBins_HF,      AlphaBins, etaShift_FE);

  if (debug) {
    std::cout << "asymmetries_data_SM " << asymmetries_data_SM.size() << "=" << EtaBins_SM << "+" << EtaBins_SM_control << '\n';
    std::cout << "asymmetries_data_FE " << asymmetries_data_FE.size() << "=" << EtaBins_FE_reference << "+" << EtaBins_FE_control << "+" << EtaBins_FE << '\n';
  }

  histLoadAsym( *f_data,  real_data,  "asymmpt_SM",           asymmetries_pt_data_SM,  gen_asymmetries_pt_data_SM,  EtaBins_SM,           PtBins_Central, AlphaBins, etaShift_SM);
  histLoadAsym( *f,       data_,      "asymmpt_SM",           asymmetries_pt_SM,       gen_asymmetries_pt_SM,       EtaBins_SM,           PtBins_Central, AlphaBins, etaShift_SM);
  histLoadAsym( *f_data,  real_data,  "asymmpt_SM_control",   asymmetries_pt_data_SM,  gen_asymmetries_pt_data_SM,  EtaBins_SM_control,   PtBins_HF,      AlphaBins, etaShift_SM_control);
  histLoadAsym( *f,       data_,      "asymmpt_SM_control",   asymmetries_pt_SM,       gen_asymmetries_pt_SM,       EtaBins_SM_control,   PtBins_HF,      AlphaBins, etaShift_SM_control);
  histLoadAsym( *f_data,  real_data,  "asymmpt_FE_reference", asymmetries_pt_data_FE,  gen_asymmetries_pt_data_FE,  EtaBins_FE_reference, PtBins_Central, AlphaBins, etaShift_FE_reference);
  histLoadAsym( *f,       data_,      "asymmpt_FE_reference", asymmetries_pt_FE,       gen_asymmetries_pt_FE,       EtaBins_FE_reference, PtBins_Central, AlphaBins, etaShift_FE_reference);
  histLoadAsym( *f_data,  real_data,  "asymmpt_FE_control",   asymmetries_pt_data_FE,  gen_asymmetries_pt_data_FE,  EtaBins_FE_control,   PtBins_Central, AlphaBins, etaShift_FE_control);
  histLoadAsym( *f,       data_,      "asymmpt_FE_control",   asymmetries_pt_FE,       gen_asymmetries_pt_FE,       EtaBins_FE_control,   PtBins_Central, AlphaBins, etaShift_FE_control);
  histLoadAsym( *f_data,  real_data,  "asymmpt_FE",           asymmetries_pt_data_FE,  gen_asymmetries_pt_data_FE,  EtaBins_FE,           PtBins_HF,      AlphaBins, etaShift_FE);
  histLoadAsym( *f,       data_,      "asymmpt_FE",           asymmetries_pt_FE,       gen_asymmetries_pt_FE,       EtaBins_FE,           PtBins_HF,      AlphaBins, etaShift_FE);

  histLoadAsym( *f,       data_,       "mctruth_SM",           MC_Truth_asymmetries_SM,  dummy_hists,       EtaBins_SM,           PtBins_Central, AlphaBins, etaShift_SM);
  histLoadAsym( *f,       data_,       "mctruth_SM_control",   MC_Truth_asymmetries_SM,  dummy_hists,       EtaBins_SM_control,   PtBins_HF,      AlphaBins, etaShift_SM_control);
  histLoadAsym( *f,       data_,       "mctruth_FE_reference", MC_Truth_asymmetries_FE,  dummy_hists,       EtaBins_FE_reference, PtBins_Central, AlphaBins, etaShift_FE_reference);
  histLoadAsym( *f,       data_,       "mctruth_FE_control",   MC_Truth_asymmetries_FE,  dummy_hists,       EtaBins_FE_control,   PtBins_Central, AlphaBins, etaShift_FE_control);
  histLoadAsym( *f,       data_,       "mctruth_FE",           MC_Truth_asymmetries_FE,  dummy_hists,       EtaBins_FE,           PtBins_HF,      AlphaBins, etaShift_FE);

  if (debug) {
    std::cout << "asymmpt_SM " << asymmetries_pt_data_SM.size() << '\n';
    std::cout << "asymmpt_FE " << asymmetries_pt_data_FE.size() << '\n';
  }

  // std::vector < TH2F* > Map_mean_data;
  //
  // Fill_Map3D(asymmetries_data_SM, Map_mean_data, eta_bins_edge_SM, Pt_bins_Central);
  //
  // TFile maps("pdfy/maps/maps.root","RECREATE");
  // for (unsigned int r = 0; r < alpha.size(); r++) {
  //   TString canvName = "Map_mean_"; canvName += r;
  //   TString nameXaxis = "#eta";
  //   TString nameYaxis = "p_{T} (GeV)";
  //   TCanvas* canv = tdrCanvas(canvName, 0, 6, 0, 1100, nameXaxis, nameYaxis);
  //   canv->SetTickx(0);
  //   canv->SetTicky(0);
  //   gStyle->SetPalette(kLightTemperature);
  //   Map_mean_data.at(r)->Draw("colz");
  //   canv->Update();
  //   Map_mean_data.at(r)->Write();
  //   canv -> Print(outdir+"pdfy/maps/"+canvName+".pdf","pdf");
  //   canv -> Print(outdir+"pdfy/maps/"+canvName+".png","png");
  // }
  // maps.Close();

  ////////////////////////////////////////////////////////////////////////////
  //    I calculate pt_mean for each alpha and pt bin and eta bin.          //
  ////////////////////////////////////////////////////////////////////////////

  std::vector< std::vector< std::vector< double > > > width_pt_SM, gen_width_pt_SM, width_pt_data_SM, gen_width_pt_data_SM;
  std::vector< std::vector< std::vector< double > > > width_pt_FE, gen_width_pt_FE, width_pt_data_FE, gen_width_pt_data_FE;

  histMeanPt( asymmetries_pt_SM,          width_pt_SM);
  histMeanPt( gen_asymmetries_pt_SM,      gen_width_pt_SM);
  histMeanPt( asymmetries_pt_data_SM,     width_pt_data_SM);
  histMeanPt( gen_asymmetries_pt_data_SM, gen_width_pt_data_SM);

  histMeanPt( asymmetries_pt_FE,          width_pt_FE );
  histMeanPt( gen_asymmetries_pt_FE,      gen_width_pt_FE );
  histMeanPt( asymmetries_pt_data_FE,     width_pt_data_FE );
  histMeanPt( gen_asymmetries_pt_data_FE, gen_width_pt_data_FE );

  if (debug) {
    std::cout << "histMeanPt_SM " << width_pt_SM.size() << '\n';
    std::cout << "histMeanPt_FE " << width_pt_FE.size() << '\n';
  }

  // gen_all_data is for running a cross check with smeared MC.

  ////////////////////////////////////////////////////////////////////////////
  //    I calculate width of asymmetry distributions only for               //
  //    alpha bins above 10 GeV thresholds (too soft contriubtions)         //
  //    e.g. for bin p_T_ave (55-75) alpha 0.1 corresponds to 57 GeV jets   //
  ////////////////////////////////////////////////////////////////////////////

  std::vector< std::vector< std::vector< double > > > asymmetries_width_SM, gen_asymmetries_width_SM, asymmetries_width_data_SM, gen_asymmetries_width_data_SM, lower_x_SM, upper_x_SM, gen_lower_x_SM, gen_upper_x_SM, lower_x_data_SM, upper_x_data_SM, gen_lower_x_data_SM, gen_upper_x_data_SM;
  std::vector< std::vector< std::vector< double > > > asymmetries_width_SM_error, gen_asymmetries_width_SM_error, asymmetries_width_data_SM_error, gen_asymmetries_width_data_SM_error;
  std::vector< std::vector< std::vector< double > > > asymmetries_width_FE, gen_asymmetries_width_FE, asymmetries_width_data_FE, gen_asymmetries_width_data_FE, lower_x_FE, upper_x_FE, gen_lower_x_FE, gen_upper_x_FE, lower_x_data_FE, upper_x_data_FE, gen_lower_x_data_FE, gen_upper_x_data_FE;
  std::vector< std::vector< std::vector< double > > > asymmetries_width_FE_error, gen_asymmetries_width_FE_error, asymmetries_width_data_FE_error, gen_asymmetries_width_data_FE_error;

  histWidthAsym( asymmetries_SM , asymmetries_width_SM, asymmetries_width_SM_error, false, gaustails, 0, lower_x_SM, upper_x_SM);
  histWidthAsym( gen_asymmetries_SM , gen_asymmetries_width_SM, gen_asymmetries_width_SM_error, false, gaustails, 0, gen_lower_x_SM, gen_upper_x_SM);
  histWidthAsym( asymmetries_data_SM , asymmetries_width_data_SM, asymmetries_width_data_SM_error, false, gaustails, 0, lower_x_data_SM, upper_x_data_SM);
  histWidthAsym( gen_asymmetries_data_SM , gen_asymmetries_width_data_SM, gen_asymmetries_width_data_SM_error, false, gaustails, 0, gen_lower_x_data_SM, gen_upper_x_data_SM);

  histWidthAsym( asymmetries_FE , asymmetries_width_FE, asymmetries_width_FE_error, false, gaustails, 1, lower_x_FE, upper_x_FE);
  histWidthAsym( gen_asymmetries_FE , gen_asymmetries_width_FE, gen_asymmetries_width_FE_error, false, gaustails, 1, gen_lower_x_FE, gen_upper_x_FE);
  histWidthAsym( asymmetries_data_FE , asymmetries_width_data_FE, asymmetries_width_data_FE_error, false, gaustails, 1, lower_x_data_FE, upper_x_data_FE);
  histWidthAsym( gen_asymmetries_data_FE , gen_asymmetries_width_data_FE, gen_asymmetries_width_data_FE_error, false, gaustails, 1, gen_lower_x_data_FE, gen_upper_x_data_FE);

  if (debug) {
    std::cout << "asymmetries_width_SM " << asymmetries_width_SM.size() << '\n';
    std::cout << "asymmetries_width_Fe " << asymmetries_width_FE.size() << '\n';
  }

  ////////////////////////////////////////////////////////////////////////////
  //    I calculate widths, this time also including                        //
  //    alpha bins below 10GeV threshold                                    //
  ////////////////////////////////////////////////////////////////////////////

  std::vector< std::vector< std::vector< double > > > soft_asymmetries_width_SM, soft_gen_asymmetries_width_SM, soft_asymmetries_width_data_SM, soft_gen_asymmetries_width_data_SM;
  std::vector< std::vector< std::vector< double > > > soft_asymmetries_width_SM_error, soft_gen_asymmetries_width_SM_error, soft_asymmetries_width_data_SM_error, soft_gen_asymmetries_width_data_SM_error;

  std::vector< std::vector< std::vector< double > > > soft_asymmetries_width_FE, gen_soft_asymmetries_width_FE, soft_asymmetries_width_data_FE, gen_soft_asymmetries_width_data_FE;
  std::vector< std::vector< std::vector< double > > > soft_asymmetries_width_FE_error, gen_soft_asymmetries_width_FE_error, soft_asymmetries_width_data_FE_error, gen_soft_asymmetries_width_data_FE_error;
  std::vector< std::vector< std::vector< double > > > dummy_vec_x, dummy_vec_y;

  histWidthAsym( asymmetries_SM , soft_asymmetries_width_SM, soft_asymmetries_width_SM_error, true, gaustails, 0, dummy_vec_x, dummy_vec_y);
  histWidthAsym( gen_asymmetries_SM , soft_gen_asymmetries_width_SM, soft_gen_asymmetries_width_SM_error, true, gaustails, 0, dummy_vec_x, dummy_vec_y);
  histWidthAsym( asymmetries_data_SM , soft_asymmetries_width_data_SM, soft_asymmetries_width_data_SM_error, true, gaustails, 0, dummy_vec_x, dummy_vec_y);
  histWidthAsym( gen_asymmetries_data_SM , soft_gen_asymmetries_width_data_SM, soft_gen_asymmetries_width_data_SM_error, true, gaustails, 0, dummy_vec_x, dummy_vec_y);

  histWidthAsym( asymmetries_FE , soft_asymmetries_width_FE, soft_asymmetries_width_FE_error, true, gaustails, 1, dummy_vec_x, dummy_vec_y);
  histWidthAsym( gen_asymmetries_FE , gen_soft_asymmetries_width_FE, gen_soft_asymmetries_width_FE_error, true, gaustails, 1, dummy_vec_x, dummy_vec_y);
  histWidthAsym( asymmetries_data_FE , soft_asymmetries_width_data_FE, soft_asymmetries_width_data_FE_error, true, gaustails, 1, dummy_vec_x, dummy_vec_y);
  histWidthAsym( gen_asymmetries_data_FE , gen_soft_asymmetries_width_data_FE, gen_soft_asymmetries_width_data_FE_error, true, gaustails, 1, dummy_vec_x, dummy_vec_y);

  if (debug) {
    std::cout << "asymmetries_SM " << asymmetries_SM.size() << '\n';
    std::cout << "asymmetries_FE " << asymmetries_FE.size() << '\n';
  }

  ////////////////////////////////////////////////////////////////////////////
  //    Calculate mcTruth resolution for cross check with dijet calculation //
  ////////////////////////////////////////////////////////////////////////////

  std::vector< std::vector< std::vector< double > > > mcTruth_res_SM, mcTruth_res_FE;
  std::vector< std::vector< std::vector< double > > > mcTruth_res_SM_error, mcTruth_res_FE_error;

  histWidthMCTruth( MC_Truth_asymmetries_SM, mcTruth_res_SM, mcTruth_res_SM_error);
  histWidthMCTruth( MC_Truth_asymmetries_FE, mcTruth_res_FE, mcTruth_res_FE_error);

  if (debug) {
    std::cout << "MC_Truth_asymmetries_SM " << MC_Truth_asymmetries_SM.size() << '\n';
    std::cout << "MC_Truth_asymmetries_FE " << MC_Truth_asymmetries_FE.size() << '\n';
  }

  ////////////////////////////////////////////////////////////////////////////
  //     I fill width(alpha_max) histograms                                 //
  ////////////////////////////////////////////////////////////////////////////

  std::vector< std::vector< TH1F* > > widths_hist_SM, gen_widths_hist_SM, widths_hist_data_SM, gen_widths_hist_data_SM;
  std::vector< std::vector< TH1F* > > widths_hist_FE, gen_widths_hist_FE, widths_hist_data_FE, gen_widths_hist_data_FE;

  fill_widths_hists( "widths", widths_hist_SM , asymmetries_width_SM, asymmetries_width_SM_error );
  fill_widths_hists( "widths_gen", gen_widths_hist_SM , gen_asymmetries_width_SM, gen_asymmetries_width_SM_error );
  fill_widths_hists( "widths_data", widths_hist_data_SM , asymmetries_width_data_SM, asymmetries_width_data_SM_error );
  fill_widths_hists( "widths_gen_data", gen_widths_hist_data_SM , gen_asymmetries_width_data_SM, gen_asymmetries_width_data_SM_error );

  fill_widths_hists( "widths_fe", widths_hist_FE , asymmetries_width_FE, asymmetries_width_FE_error );
  fill_widths_hists( "widths_gen_fe", gen_widths_hist_FE , gen_asymmetries_width_FE, gen_asymmetries_width_FE_error );
  fill_widths_hists( "widths_data_fe", widths_hist_data_FE , asymmetries_width_data_FE, asymmetries_width_data_FE_error );
  fill_widths_hists( "widths_gen_data_fe", gen_widths_hist_data_FE , gen_asymmetries_width_data_FE, gen_asymmetries_width_data_FE_error );

  if (debug) {
    std::cout << "widths_hist_SM " << widths_hist_SM.size() << '\n';
    std::cout << "widths_hist_FE " << widths_hist_FE.size() << '\n';
  }

  ////////////////////////////////////////////////////////////////////////////
  //    I do same for alpha unconstrained widths                            //
  //    one needs these plots to prove which points should be rejected!     //
  ////////////////////////////////////////////////////////////////////////////

  std::vector< std::vector< TH1F* > > soft_widths_hist_SM, soft_gen_widths_hist_SM, soft_widths_hist_data_SM, soft_gen_widths_hist_data_SM;
  std::vector< std::vector< TH1F* > > soft_widths_hist_FE, soft_gen_widths_hist_FE, soft_widths_hist_data_FE, soft_gen_widths_hist_data_FE;

  fill_widths_hists( "all_widths", soft_widths_hist_SM, soft_asymmetries_width_SM, soft_asymmetries_width_SM_error );
  fill_widths_hists( "all_widths_gen", soft_gen_widths_hist_SM, soft_gen_asymmetries_width_SM, soft_gen_asymmetries_width_SM_error );
  fill_widths_hists( "all_widths_data", soft_widths_hist_data_SM, soft_asymmetries_width_data_SM, soft_asymmetries_width_data_SM_error );
  fill_widths_hists( "all_widths_gen_data", soft_gen_widths_hist_data_SM, soft_gen_asymmetries_width_data_SM, soft_gen_asymmetries_width_data_SM_error );

  fill_widths_hists( "all_widths_fe", soft_widths_hist_FE, soft_asymmetries_width_FE, soft_asymmetries_width_FE_error );
  fill_widths_hists( "all_widths_gen_fe", soft_gen_widths_hist_FE, gen_soft_asymmetries_width_FE, gen_soft_asymmetries_width_FE_error );
  fill_widths_hists( "all_widths_data_fe", soft_widths_hist_data_FE, soft_asymmetries_width_data_FE, soft_asymmetries_width_data_FE_error );
  fill_widths_hists( "all_widths_gen_data_fe", soft_gen_widths_hist_data_FE, gen_soft_asymmetries_width_data_FE, gen_soft_asymmetries_width_data_FE_error );

  ////////////////////////////////////////////////////////////////////////////
  //    I fit line or const to width(alpha_max)                             //
  ////////////////////////////////////////////////////////////////////////////

  std::vector< std::vector< double > > extrapolated_widths_SM, extrapolated_gen_widths_SM, extrapolated_widths_data_SM, extrapolated_gen_widths_data_SM;
  std::vector< std::vector< double > > extrapolated_widths_SM_error, extrapolated_gen_widths_SM_error, extrapolated_widths_data_SM_error, extrapolated_gen_widths_data_SM_error;

  std::vector< std::vector< double > > extrapolated_widths_FE, extrapolated_gen_widths_FE, extrapolated_widths_data_FE, extrapolated_gen_widths_data_FE;
  std::vector< std::vector< double > > extrapolated_widths_FE_error, extrapolated_gen_widths_FE_error, extrapolated_widths_data_FE_error, extrapolated_gen_widths_data_FE_error;

  histLinFit( widths_hist_SM , extrapolated_widths_SM, extrapolated_widths_SM_error, false );
  histLinFit( gen_widths_hist_SM , extrapolated_gen_widths_SM, extrapolated_gen_widths_SM_error, false );
  histLinFit( widths_hist_data_SM , extrapolated_widths_data_SM, extrapolated_widths_data_SM_error, false );
  histLinFit( gen_widths_hist_data_SM , extrapolated_gen_widths_data_SM, extrapolated_gen_widths_data_SM_error, false );

  histLinFit( widths_hist_FE , extrapolated_widths_FE, extrapolated_widths_FE_error, true );
  histLinFit( gen_widths_hist_FE , extrapolated_gen_widths_FE, extrapolated_gen_widths_FE_error, true );
  histLinFit( widths_hist_data_FE , extrapolated_widths_data_FE, extrapolated_widths_data_FE_error, true );
  histLinFit( gen_widths_hist_data_FE , extrapolated_gen_widths_data_FE, extrapolated_gen_widths_data_FE_error, true );

  if (debug) {
    std::cout << "extrapolated_widths_SM " << extrapolated_widths_SM.size() << '\n';
    std::cout << "extrapolated_widths_FE " << extrapolated_widths_FE.size() << '\n';
  }

  ////////////////////////////////////////////////////////////////////////////
  //    Correlated fit                                                      //
  ////////////////////////////////////////////////////////////////////////////

  std::vector< std::vector< double > > extrapolated_widths_correlated_SM, extrapolated_gen_widths_correlated_SM, extrapolated_widths_correlated_data_SM, extrapolated_gen_widths_correlated_data_SM;
  std::vector< std::vector< double > > extrapolated_widths_correlated_SM_error, extrapolated_gen_widths_correlated_SM_error, extrapolated_widths_correlated_data_SM_error, extrapolated_gen_widths_correlated_data_SM_error;

  std::vector< std::vector< double > > extrapolated_widths_correlated_FE, extrapolated_gen_widths_correlated_FE, extrapolated_widths_correlated_data_FE, extrapolated_gen_widths_correlated_data_FE;
  std::vector< std::vector< double > > extrapolated_widths_correlated_FE_error, extrapolated_gen_widths_correlated_FE_error, extrapolated_widths_correlated_data_FE_error, extrapolated_gen_widths_correlated_data_FE_error;

  std::vector< std::vector< TGraphErrors *> > MC_correlated_graphs_SM, data_correlated_graphs_SM, gen_correlated_graphs_SM, gen_data_correlated_graphs_SM;
  std::vector< std::vector< TGraphErrors *> > MC_correlated_graphs_FE, data_correlated_graphs_FE, gen_correlated_graphs_FE, gen_data_correlated_graphs_FE;
  TH1F* h_chi2_tot = new TH1F("h_chi2_tot","h_chi2_tot",100, -10,10);

  histLinCorFit(asymmetries_width_SM, asymmetries_width_SM_error, MC_correlated_graphs_SM, extrapolated_widths_correlated_SM, extrapolated_widths_correlated_SM_error, false, true, h_chi2_tot);
  histLinCorFit(asymmetries_width_data_SM, asymmetries_width_data_SM_error, data_correlated_graphs_SM, extrapolated_widths_correlated_data_SM, extrapolated_widths_correlated_data_SM_error, false, false, h_chi2_tot);
  histLinCorFit(gen_asymmetries_width_SM, gen_asymmetries_width_SM_error, gen_correlated_graphs_SM, extrapolated_gen_widths_correlated_SM, extrapolated_gen_widths_correlated_SM_error, false, false, h_chi2_tot);
  histLinCorFit(gen_asymmetries_width_data_SM, gen_asymmetries_width_data_SM_error, gen_data_correlated_graphs_SM, extrapolated_gen_widths_correlated_data_SM, extrapolated_gen_widths_correlated_data_SM_error, false, false, h_chi2_tot);

  histLinCorFit(asymmetries_width_FE, asymmetries_width_FE_error, MC_correlated_graphs_FE, extrapolated_widths_correlated_FE, extrapolated_widths_correlated_FE_error, true, true, h_chi2_tot);
  histLinCorFit(asymmetries_width_data_FE, asymmetries_width_data_FE_error, data_correlated_graphs_FE, extrapolated_widths_correlated_data_FE, extrapolated_widths_correlated_data_FE_error, true, false, h_chi2_tot);
  histLinCorFit(gen_asymmetries_width_FE, gen_asymmetries_width_FE_error, gen_correlated_graphs_FE, extrapolated_gen_widths_correlated_FE, extrapolated_gen_widths_correlated_FE_error, true, false, h_chi2_tot);
  histLinCorFit(gen_asymmetries_width_data_FE, gen_asymmetries_width_data_FE_error, gen_data_correlated_graphs_FE, extrapolated_gen_widths_correlated_data_FE, extrapolated_gen_widths_correlated_data_FE_error, true, false, h_chi2_tot);

  if (debug) {
    std::cout << "extrapolated_widths_correlated_SM " << extrapolated_widths_correlated_SM.size() << '\n';
    std::cout << "extrapolated_widths_correlated_FE " << extrapolated_widths_correlated_FE.size() << '\n';
  }

  ////////////////////////////////////////////////////////////////////////////
  //    I make histograms ratio of widths(alpha=0.15)                       //
  ////////////////////////////////////////////////////////////////////////////

  std::vector< TH1F* > widths_015_ratios_SM, widths_015_ratios_FE;

  widths_015_ratios( "widths_015_SM_ratios", widths_015_ratios_SM, asymmetries_width_data_SM, asymmetries_width_data_SM_error, asymmetries_width_SM, asymmetries_width_SM_error, width_pt_SM );
  widths_015_ratios( "widths_015_FE_ratios", widths_015_ratios_FE, asymmetries_width_data_FE, asymmetries_width_data_FE_error, asymmetries_width_FE, asymmetries_width_FE_error, width_pt_FE );

  ////////////////////////////////////////////////////////////////////////////
  //    I correct for PLI using b parameter from line fit to sigma_A(alpha) //
  ////////////////////////////////////////////////////////////////////////////

  std::vector< std::vector< double > > JER_uncorrelated_corrected_MC_SM, JER_uncorrelated_corrected_data_SM;
  std::vector< std::vector< double > > JER_uncorrelated_corrected_MC_SM_error, JER_uncorrelated_corrected_data_SM_error;

  std::vector< std::vector< double > > JER_uncorrelated_corrected_MC_FE_ref, JER_uncorrelated_corrected_data_FE_ref;
  std::vector< std::vector< double > > JER_uncorrelated_corrected_MC_FE_ref_error, JER_uncorrelated_corrected_data_FE_ref_error;

  correctJERwithPLI( JER_uncorrelated_corrected_MC_SM, JER_uncorrelated_corrected_MC_SM_error, extrapolated_widths_SM, extrapolated_widths_SM_error, extrapolated_gen_widths_SM, extrapolated_gen_widths_SM_error, shiftForPLI);
  if( real_data ){
    // correct data with PLI from MC
    correctJERwithPLI( JER_uncorrelated_corrected_data_SM, JER_uncorrelated_corrected_data_SM_error, extrapolated_widths_data_SM, extrapolated_widths_data_SM_error, extrapolated_gen_widths_SM, extrapolated_gen_widths_SM_error, shiftForPLI);
  } else {
    // correct 'data' with own PLI
    correctJERwithPLI( JER_uncorrelated_corrected_data_SM, JER_uncorrelated_corrected_data_SM_error, extrapolated_widths_data_SM, extrapolated_widths_data_SM_error, extrapolated_gen_widths_data_SM, extrapolated_gen_widths_data_SM_error, shiftForPLI);
  }

  correctJERwithPLI( JER_uncorrelated_corrected_MC_FE_ref, JER_uncorrelated_corrected_MC_FE_ref_error, extrapolated_widths_FE, extrapolated_widths_FE_error, extrapolated_gen_widths_FE, extrapolated_gen_widths_FE_error, shiftForPLI);
  if( real_data ){
    // correct data with PLI from MC
    correctJERwithPLI( JER_uncorrelated_corrected_data_FE_ref, JER_uncorrelated_corrected_data_FE_ref_error, extrapolated_widths_data_FE, extrapolated_widths_data_FE_error, extrapolated_gen_widths_FE, extrapolated_gen_widths_FE_error, shiftForPLI);
  } else {
    // correct 'data' with own PLI
    correctJERwithPLI( JER_uncorrelated_corrected_data_FE_ref, JER_uncorrelated_corrected_data_FE_ref_error, extrapolated_widths_data_FE, extrapolated_widths_data_FE_error, extrapolated_gen_widths_data_FE, extrapolated_gen_widths_data_FE_error, shiftForPLI);
  }

  if (debug) {
    std::cout << "JER_uncorrelated_corrected_MC_SM " << JER_uncorrelated_corrected_MC_SM.size() << '\n';
    std::cout << "JER_uncorrelated_corrected_MC_FE_ref " << JER_uncorrelated_corrected_MC_FE_ref.size() << '\n';
  }

  ////////////////////////////////////////////////////////////////////////////
  //    PLI corrected using b parameters                                    //
  ////////////////////////////////////////////////////////////////////////////
  //    Same correction but for correlated fit results                      //
  ////////////////////////////////////////////////////////////////////////////

  std::vector< std::vector< double > > JER_correlated_corrected_MC_SM, JER_correlated_corrected_data_SM;
  std::vector< std::vector< double > > JER_correlated_corrected_MC_SM_error, JER_correlated_corrected_data_SM_error;

  std::vector< std::vector< double > > JER_correlated_corrected_MC_FE_ref, JER_correlated_corrected_data_FE_ref;
  std::vector< std::vector< double > > JER_correlated_corrected_MC_FE_ref_error, JER_correlated_corrected_data_FE_ref_error;

  correctJERwithPLI( JER_correlated_corrected_MC_SM, JER_correlated_corrected_MC_SM_error, extrapolated_widths_correlated_SM, extrapolated_widths_correlated_SM_error, extrapolated_gen_widths_correlated_SM, extrapolated_gen_widths_correlated_SM_error, shiftForPLI);
  if( real_data ){
    // correct data with PLI from MC
    correctJERwithPLI( JER_correlated_corrected_data_SM, JER_correlated_corrected_data_SM_error, extrapolated_widths_correlated_data_SM, extrapolated_widths_correlated_data_SM_error, extrapolated_gen_widths_correlated_SM, extrapolated_gen_widths_correlated_SM_error, shiftForPLI);
  } else {
    // correct 'data' with own PLI
    correctJERwithPLI( JER_correlated_corrected_data_SM, JER_correlated_corrected_data_SM_error, extrapolated_widths_correlated_data_SM, extrapolated_widths_correlated_data_SM_error, extrapolated_gen_widths_correlated_data_SM, extrapolated_gen_widths_correlated_data_SM_error, shiftForPLI);
  }

  correctJERwithPLI( JER_correlated_corrected_MC_FE_ref, JER_correlated_corrected_MC_FE_ref_error, extrapolated_widths_correlated_FE, extrapolated_widths_correlated_FE_error, extrapolated_gen_widths_correlated_FE, extrapolated_gen_widths_correlated_FE_error, shiftForPLI);
  if( real_data ){
    // correct data with PLI from MC
    correctJERwithPLI( JER_correlated_corrected_data_FE_ref, JER_correlated_corrected_data_FE_ref_error, extrapolated_widths_correlated_data_FE, extrapolated_widths_correlated_data_FE_error, extrapolated_gen_widths_correlated_FE, extrapolated_gen_widths_correlated_FE_error, shiftForPLI);
  } else {
    // correct 'data' with own PLI
    correctJERwithPLI( JER_correlated_corrected_data_FE_ref, JER_correlated_corrected_data_FE_ref_error, extrapolated_widths_correlated_data_FE, extrapolated_widths_correlated_data_FE_error, extrapolated_gen_widths_correlated_data_FE, extrapolated_gen_widths_correlated_data_FE_error, shiftForPLI);
  }

  if (debug) {
    std::cout << "JER_correlated_corrected_MC_SM " << JER_correlated_corrected_MC_SM.size() << '\n';
    std::cout << "JER_correlated_corrected_MC_FE_ref " << JER_correlated_corrected_MC_FE_ref.size() << '\n';
    std::cout << "TEST" << '\n';
    std::cout << JER_correlated_corrected_data_FE_ref.size() << " " << JER_correlated_corrected_data_FE_ref[0].size() << '\n';
    std::cout << extrapolated_widths_correlated_data_FE.size() << " " << extrapolated_widths_correlated_data_FE[0].size() << '\n';
    std::cout << extrapolated_gen_widths_correlated_FE.size() << " " << extrapolated_gen_widths_correlated_FE[0].size() << '\n';
    for (size_t i = 0; i < JER_correlated_corrected_data_FE_ref.size(); i++) {
      for (size_t j = 0; j < JER_correlated_corrected_data_FE_ref[i].size(); j++) {
        std::cout << i << " " << j << " " << JER_correlated_corrected_data_FE_ref.at(i).at(j) << " " << extrapolated_widths_correlated_data_FE.at(i).at(j) << " " << extrapolated_gen_widths_correlated_FE.at(i).at(j) << " " << TMath::Sqrt(2)*TMath::Sqrt( extrapolated_widths_correlated_data_FE.at(i).at(j) * extrapolated_widths_correlated_data_FE.at(i).at(j) - extrapolated_gen_widths_correlated_FE.at(i).at(j) * extrapolated_gen_widths_correlated_FE.at(i).at(j) ) << '\n';
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////////
  //    I do the same for widths at alpha = 0.15                            //
  ////////////////////////////////////////////////////////////////////////////

  std::vector< std::vector< double > > JER015_MC_SM, JER015_data_SM;
  std::vector< std::vector< double > > JER015_MC_SM_error, JER015_data_SM_error;

  std::vector< std::vector< double > > JER015_MC_FE_ref, JER015_data_FE_ref;
  std::vector< std::vector< double > > JER015_MC_FE_ref_error, JER015_data_FE_ref_error;

  correctJERwithPLI015(JER015_MC_SM, JER015_MC_SM_error, asymmetries_width_SM, asymmetries_width_SM_error, gen_asymmetries_width_SM, gen_asymmetries_width_SM_error, shiftForPLI);
  if( real_data ){
    // correct data with PLI from MC
    correctJERwithPLI015(JER015_data_SM, JER015_data_SM_error, asymmetries_width_data_SM, asymmetries_width_data_SM_error, gen_asymmetries_width_SM, gen_asymmetries_width_SM_error, shiftForPLI);
  } else {
    // correct 'data' with own PLI
    correctJERwithPLI015(JER015_data_SM, JER015_data_SM_error, asymmetries_width_data_SM, asymmetries_width_data_SM_error, gen_asymmetries_width_data_SM, gen_asymmetries_width_data_SM_error, shiftForPLI);
  }
  correctJERwithPLI015(JER015_MC_FE_ref, JER015_MC_FE_ref_error, asymmetries_width_FE, asymmetries_width_FE_error, gen_asymmetries_width_FE, gen_asymmetries_width_FE_error, shiftForPLI);
  if( real_data ){
    // correct data with PLI from MC
    correctJERwithPLI015(JER015_data_FE_ref, JER015_data_FE_ref_error, asymmetries_width_data_FE, asymmetries_width_data_FE_error, gen_asymmetries_width_FE, gen_asymmetries_width_FE_error, shiftForPLI);
  } else {
    // correct 'data' with own PLI
    correctJERwithPLI015(JER015_data_FE_ref, JER015_data_FE_ref_error, asymmetries_width_data_FE, asymmetries_width_data_FE_error, gen_asymmetries_width_data_FE, gen_asymmetries_width_data_FE_error, shiftForPLI);
  }


  ////////////////////////////////////////////////////////////////////////////
  //    I corrected alpha = 0.15 widhs for PLI correct way                  //
  ////////////////////////////////////////////////////////////////////////////
  //    I correct forward widths for Ref region.                            //
  ////////////////////////////////////////////////////////////////////////////

  std::vector< std::vector< double > > JER_uncorrelated_corrected_MC_FE, JER_uncorrelated_corrected_data_FE;
  std::vector< std::vector< double > > JER_uncorrelated_corrected_MC_FE_error, JER_uncorrelated_corrected_data_FE_error;

  correctForRef( "mccorrected", JER_uncorrelated_corrected_MC_FE, JER_uncorrelated_corrected_MC_FE_error, JER_uncorrelated_corrected_MC_FE_ref, JER_uncorrelated_corrected_MC_FE_ref_error, width_pt_FE, ref_shift, outdir);
  correctForRef( "datacorrect", JER_uncorrelated_corrected_data_FE, JER_uncorrelated_corrected_data_FE_error, JER_uncorrelated_corrected_data_FE_ref, JER_uncorrelated_corrected_data_FE_ref_error, width_pt_FE, ref_shift, outdir);

  if (debug) std::cout << "JER_uncorrelated_corrected_MC_FE " << JER_uncorrelated_corrected_MC_FE.size() << '\n';

  ////////////////////////////////////////////////////////////////////////////
  //    forward widths corrected for Ref widths!                            //
  ////////////////////////////////////////////////////////////////////////////
  //    same correction for correlated fit results                          //
  ////////////////////////////////////////////////////////////////////////////

  std::vector< std::vector< double > > JER_correlated_corrected_MC_FE, JER_correlated_corrected_data_FE;
  std::vector< std::vector< double > > JER_correlated_corrected_MC_FE_error, JER_correlated_corrected_data_FE_error;

  correctForRef( "mc_cor_corrected", JER_correlated_corrected_MC_FE,   JER_correlated_corrected_MC_FE_error,   JER_correlated_corrected_MC_FE_ref,   JER_correlated_corrected_MC_FE_ref_error,   width_pt_FE, ref_shift, outdir);
  correctForRef( "data_cor_correct", JER_correlated_corrected_data_FE, JER_correlated_corrected_data_FE_error, JER_correlated_corrected_data_FE_ref, JER_correlated_corrected_data_FE_ref_error, width_pt_FE, ref_shift, outdir);

  if (debug) std::cout << "JER_correlated_corrected_MC_FE " << JER_correlated_corrected_MC_FE.size() << '\n';

  ////////////////////////////////////////////////////////////////////////////
  //    ref region corrected for correlated fit                             //
  ////////////////////////////////////////////////////////////////////////////
  //    and again, Ref region for widths at alpha = 0.15                    //
  ////////////////////////////////////////////////////////////////////////////

  std::vector< std::vector< double > > JER015_MC_FE, JER015_data_FE;
  std::vector< std::vector< double > > JER015_MC_FE_error, JER015_data_FE_error;

  correctForRef( "mccorrected015", JER015_MC_FE,   JER015_MC_FE_error,   JER015_MC_FE_ref,   JER015_MC_FE_ref_error,   width_pt_FE, ref_shift, outdir);
  correctForRef( "datacorrect015", JER015_data_FE, JER015_data_FE_error, JER015_data_FE_ref, JER015_data_FE_ref_error, width_pt_FE, ref_shift, outdir);

  ////////////////////////////////////////////////////////////////////////////
  //    Ref reg corrected for widths at alpha = 0.15                        //
  ////////////////////////////////////////////////////////////////////////////
  //    I make make vectors with ratios of widths                           //
  ////////////////////////////////////////////////////////////////////////////

  std::vector< std::vector< double > > scales_uncorrelated_SM, scales_uncorrelated_FE, scales_uncorrelated_FE_control;
  std::vector< std::vector< double > > scales_uncorrelated_SM_error, scales_uncorrelated_FE_error, scales_uncorrelated_FE_control_error;

  makeScales( scales_uncorrelated_SM,         scales_uncorrelated_SM_error,         JER_uncorrelated_corrected_data_SM,     JER_uncorrelated_corrected_data_SM_error,     JER_uncorrelated_corrected_MC_SM,     JER_uncorrelated_corrected_MC_SM_error );
  makeScales( scales_uncorrelated_FE,         scales_uncorrelated_FE_error,         JER_uncorrelated_corrected_data_FE,     JER_uncorrelated_corrected_data_FE_error,     JER_uncorrelated_corrected_MC_FE,     JER_uncorrelated_corrected_MC_FE_error );
  makeScales( scales_uncorrelated_FE_control, scales_uncorrelated_FE_control_error, JER_uncorrelated_corrected_data_FE_ref, JER_uncorrelated_corrected_data_FE_ref_error, JER_uncorrelated_corrected_MC_FE_ref, JER_uncorrelated_corrected_MC_FE_ref_error );

  // same thing for correlated fit results
  std::vector< std::vector< double > > scales_correlated_SM, scales_correlated_FE, scales_correlated_FE_control;
  std::vector< std::vector< double > > scales_correlated_SM_error, scales_correlated_FE_error, scales_correlated_FE_control_error;

  makeScales( scales_correlated_SM,         scales_correlated_SM_error,         JER_correlated_corrected_data_SM,          JER_correlated_corrected_data_SM_error,          JER_correlated_corrected_MC_SM,          JER_correlated_corrected_MC_SM_error );
  makeScales( scales_correlated_FE,         scales_correlated_FE_error,         JER_correlated_corrected_data_FE,       JER_correlated_corrected_data_FE_error,       JER_correlated_corrected_MC_FE,       JER_correlated_corrected_MC_FE_error );
  makeScales( scales_correlated_FE_control, scales_correlated_FE_control_error, JER_correlated_corrected_data_FE_ref, JER_correlated_corrected_data_FE_ref_error, JER_correlated_corrected_MC_FE_ref, JER_correlated_corrected_MC_FE_ref_error );  // uncorrected for reference region width, as a cross check. (i think)

  std::vector< std::vector< double > > scales015_SM, scales015_FE;
  std::vector< std::vector< double > > scales015_SM_error, scales015_FE_error;

  makeScales( scales015_SM, scales015_SM_error, JER015_data_SM, JER015_data_SM_error, JER015_MC_SM, JER015_MC_SM_error );
  makeScales( scales015_FE, scales015_FE_error, JER015_data_FE, JER015_data_FE_error, JER015_MC_FE, JER015_MC_FE_error );

  if (debug) {
    std::cout << "scales_uncorrelated_SM " << scales_uncorrelated_SM.size() << '\n';
    std::cout << "scales_uncorrelated_FE " << scales_uncorrelated_FE.size() << '\n';
    std::cout << "scales_uncorrelated_FE_control " << scales_uncorrelated_FE_control.size() << '\n';
    std::cout << "scales_correlated_FE " << scales_correlated_FE.size() << " " << scales_correlated_FE[0].size() << '\n';
    std::cout << "scales_correlated_FE_control " << scales_correlated_FE_control.size() << " " << scales_correlated_FE_control[0].size() << '\n';
  }

  ////////////////////////////////////////////////////////////////////////////
  //    I make plots with MCTruth: Res from dijet                           //
  ////////////////////////////////////////////////////////////////////////////

  std::vector< TH1F* > JER_MC_Truth_SM, JER_MC_Truth_FE;

  fill_mctruth_hist( "MC_Truth"    , JER_MC_Truth_SM, mcTruth_res_SM, mcTruth_res_SM_error, width_pt_SM, hist_max_value);
  fill_mctruth_hist( "MC_Truth_Fwd", JER_MC_Truth_FE, mcTruth_res_FE, mcTruth_res_FE_error, width_pt_FE, hist_max_value);

  ////////////////////////////////////////////////////////////////////////////
  //    I make histograms with  JERs and scale factors                      //
  ////////////////////////////////////////////////////////////////////////////

  std::vector< TH1F* > JER_uncorrelated_MC_hist_SM,         JER_uncorrelated_data_hist_SM,          JER_uncorrelated_scale_hist_SM,         JER015_uncorrelated_MC_hist_SM;
  std::vector< TH1F* > JER_uncorrelated_MC_hist_FE,         JER_uncorrelated_data_hist_FE,          JER_uncorrelated_scale_hist_FE,         JER015_uncorrelated_MC_hist_FE;
  std::vector< TH1F* > JER_uncorrelated_MC_hist_FE_control, JER_uncorrelated_data_hist_FE_control,  JER_FE_uncorrelated_scale_control_hist, JER015_uncorrelated_MC_hist_SM_control;

  fill_hist( "MC_JER_uncorrelated_SM",    JER_uncorrelated_MC_hist_SM,    JER_uncorrelated_corrected_MC_SM,   JER_uncorrelated_corrected_MC_SM_error,   width_pt_SM, hist_max_value);
  fill_hist( "data_JER_uncorrelated_SM",  JER_uncorrelated_data_hist_SM , JER_uncorrelated_corrected_data_SM, JER_uncorrelated_corrected_data_SM_error, width_pt_SM, hist_max_value);
  fill_hist( "SF_uncorrelated_SM",        JER_uncorrelated_scale_hist_SM, scales_uncorrelated_SM,             scales_uncorrelated_SM_error,             width_pt_SM, hist_max_value_SF);

  fill_hist( "MC_JER_uncorrelated_FE",    JER_uncorrelated_MC_hist_FE,    JER_uncorrelated_corrected_MC_FE,   JER_uncorrelated_corrected_MC_FE_error,   width_pt_FE, hist_max_value);
  fill_hist( "data_JER_uncorrelated_FE",  JER_uncorrelated_data_hist_FE,  JER_uncorrelated_corrected_data_FE, JER_uncorrelated_corrected_data_FE_error, width_pt_FE, hist_max_value);
  fill_hist( "SF_uncorrelated_FE",        JER_uncorrelated_scale_hist_FE, scales_uncorrelated_FE,             scales_uncorrelated_FE_error,             width_pt_FE, hist_max_value_SF);

  fill_hist( "MC_JER_uncorrelated_FE_control",    JER_uncorrelated_MC_hist_FE_control,    JER_uncorrelated_corrected_MC_FE_ref,   JER_uncorrelated_corrected_MC_FE_ref_error,   width_pt_FE, hist_max_value);
  fill_hist( "data_JER_uncorrelated_FE_control",  JER_uncorrelated_data_hist_FE_control,  JER_uncorrelated_corrected_data_FE_ref, JER_uncorrelated_corrected_data_FE_ref_error, width_pt_FE, hist_max_value);
  fill_hist( "SF_uncorrelated_FE_control",        JER_FE_uncorrelated_scale_control_hist, scales_uncorrelated_FE_control,         scales_uncorrelated_FE_control_error,           width_pt_FE, hist_max_value_SF);

  fill_hist( "MC_JER015_uncorrelated_SM", JER015_uncorrelated_MC_hist_SM, JER015_MC_SM,                       JER015_MC_SM_error,                       width_pt_SM,      hist_max_value);
  fill_hist( "MC_JER015_uncorrelated_FE", JER015_uncorrelated_MC_hist_FE, JER015_MC_FE,                       JER015_MC_FE_error,                       width_pt_FE,      hist_max_value);
  // fill_hist( "MC_JER015_uncorrelated_FE_control", JER015_uncorrelated_MC_hist_FE_control, JER015_MC_FE_control,                   JER015_MC_FE_error,                           width_pt_FE,      0.2);

  std::vector< TH1F* > JER015_scale_hist_SM, JER015_scale_hist_FE;

  fill_hist( "SF_SM015", JER015_scale_hist_SM,  scales015_SM, scales015_SM_error, width_pt_SM, hist_max_value_SF);
  fill_hist( "SF_FE015", JER015_scale_hist_FE,  scales015_FE, scales015_FE_error, width_pt_FE, hist_max_value_SF);

  std::vector< TH1F* > JER_correlated_MC_hist_SM,         JER_correlated_data_hist_SM,         JER_correlated_scale_hist_SM;
  std::vector< TH1F* > JER_correlated_MC_hist_FE,         JER_correlated_data_hist_FE,         JER_correlated_scale_hist_FE;
  std::vector< TH1F* > JER_correlated_MC_hist_FE_control, JER_correlated_data_hist_FE_control, JER_correlated_scale_hist_FE_control;

  fill_hist( "MC_JER_correlated_SM",   JER_correlated_MC_hist_SM,    JER_correlated_corrected_MC_SM,         JER_correlated_corrected_MC_SM_error,         width_pt_SM, hist_max_value);
  fill_hist( "data_JER_correlated_SM", JER_correlated_data_hist_SM,  JER_correlated_corrected_data_SM,       JER_correlated_corrected_data_SM_error,       width_pt_SM, hist_max_value);
  fill_hist( "SF_correlated_SM",       JER_correlated_scale_hist_SM, scales_correlated_SM, scales_correlated_SM_error, width_pt_SM, hist_max_value_SF);

  fill_hist( "MC_JER_correlated_FE",   JER_correlated_MC_hist_FE,    JER_correlated_corrected_MC_FE,   JER_correlated_corrected_MC_FE_error,   width_pt_FE, hist_max_value);
  fill_hist( "data_JER_correlated_FE", JER_correlated_data_hist_FE,  JER_correlated_corrected_data_FE, JER_correlated_corrected_data_FE_error, width_pt_FE, hist_max_value);
  fill_hist( "SF_correlated_FE",       JER_correlated_scale_hist_FE, scales_correlated_FE,   scales_correlated_FE_error,   width_pt_FE, hist_max_value_SF);

  fill_hist( "MC_JER_correlated_FE_control",   JER_correlated_MC_hist_FE_control,    JER_correlated_corrected_MC_FE_ref,   JER_correlated_corrected_MC_FE_ref_error,   width_pt_FE, hist_max_value);
  fill_hist( "data_JER_correlated_FE_control", JER_correlated_data_hist_FE_control,  JER_correlated_corrected_data_FE_ref, JER_correlated_corrected_data_FE_ref_error, width_pt_FE, hist_max_value);
  fill_hist( "SF_correlated_FE_control",       JER_correlated_scale_hist_FE_control, scales_correlated_FE_control, scales_correlated_FE_control_error, width_pt_FE, hist_max_value_SF);


  //////////////////////////////////////////////////////////////////////////////////////////
  //    store SFs as 2D (pt,eta) histograms                                               //
  //////////////////////////////////////////////////////////////////////////////////////////
  TH2Poly* JER_SF_correlated_SM_2D = fill_2Dhist( "2D_SF_SM", scales_correlated_SM, scales_correlated_SM_error, Pt_bins_Central, Pt_bins_HF, eta_bins_edge_SM,eta_cut);
  TH2Poly* JER_SF_correlated_FE_2D = fill_2Dhist( "2D_SF_FE", scales_correlated_FE, scales_correlated_FE_error, Pt_bins_Central, Pt_bins_HF, eta_bins_edge_FE,eta_cut);
  TFile JERSF2Droot(outdir+"output/DijetJERSF2D.root","RECREATE");
  JER_SF_correlated_FE_2D->Write();
  JER_SF_correlated_SM_2D->Write();
  JERSF2Droot.Close();

  TCanvas* canv_2D_SF = new TCanvas();
  gStyle->SetPaintTextFormat("5.2f");
  canv_2D_SF->SetTickx(0);
  canv_2D_SF->SetTicky(0);
  JER_SF_correlated_FE_2D->Draw("colz TEXT0");
  canv_2D_SF->SaveAs(outdir+"output/DijetJERSF2D_FE.pdf");
  JER_SF_correlated_SM_2D->Draw("colz TEXT0");
  canv_2D_SF->SaveAs(outdir+"output/DijetJERSF2D_SM.pdf");
  delete JER_SF_correlated_SM_2D;
  delete JER_SF_correlated_FE_2D;

  //////////////////////////////////////////////////////////////////////////////////////////
  //    resolution cross check with mcTruth                                               //
  //////////////////////////////////////////////////////////////////////////////////////////
  int iPeriod = 4;    // 1=7TeV, 2=8TeV, 3=7+8TeV, 7=7+8+13TeV, 0=free form (uses lumi_sqrtS)
  extraText  = "Preliminary";  // default extra text is "Preliminary"
  lumi_13TeV = lumi; //"2.1 fb^{-1}";
  lumi_sqrtS = "13 TeV";       // used with iPeriod = 0, e.g. for simulation-only plots (default is an empty string)

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);

  std::cout << "START PLOTTING" << '\n';

  TFile fMCTruth("output/MCTruth.root","RECREATE");
  PLOT_MCT(JER_MC_Truth_SM,JER_uncorrelated_MC_hist_SM,JER_correlated_MC_hist_SM,JER015_uncorrelated_MC_hist_SM,outdir+"pdfy/MCTruth/",eta_bins_edge_SM, false);
  PLOT_MCT(JER_MC_Truth_FE,JER_uncorrelated_MC_hist_FE,JER_correlated_MC_hist_FE,JER015_uncorrelated_MC_hist_FE,outdir+"pdfy/MCTruth/",eta_bins_edge_FE, true);
  fMCTruth.Close();

  bool plot_all = true;
  // bool plot_all = false;
  if (plot_all) {
    ////////////////////////////////////////////////////////////////////////////
    //  Plots Asymmetries                                                     //
    ////////////////////////////////////////////////////////////////////////////
    PLOT_ASY(asymmetries_data_SM,  asymmetries_SM, gen_asymmetries_SM,  asymmetries_width_data_SM, asymmetries_width_SM, gen_asymmetries_width_SM, asymmetries_width_data_SM_error, asymmetries_width_SM_error, gen_asymmetries_width_SM_error, outdir+"output/asymmetries/", eta_bins_edge_SM, Pt_bins_Central, Pt_bins_HF, alpha, lower_x_data_SM, upper_x_data_SM, lower_x_SM, upper_x_SM, gen_lower_x_SM, gen_upper_x_SM);
    PLOT_ASY(asymmetries_data_FE,  asymmetries_FE, gen_asymmetries_FE,  asymmetries_width_data_FE, asymmetries_width_FE, gen_asymmetries_width_FE, asymmetries_width_data_FE_error, asymmetries_width_FE_error, gen_asymmetries_width_FE_error, outdir+"output/asymmetries/", eta_bins_edge_SM, Pt_bins_Central, Pt_bins_HF, alpha, lower_x_data_FE, upper_x_data_FE, lower_x_FE, upper_x_FE, gen_lower_x_FE, gen_upper_x_FE);

    TFile asyroot(outdir+"output/asymmetries.root","RECREATE");
    for( unsigned int m = 0; m < asymmetries_SM.size(); m++){
      for( unsigned int p = 0; p < asymmetries_SM.at(m).size(); p++){
        for( unsigned int r = 0; r < asymmetries_SM.at(m).at(p).size(); r++){
          asymmetries_SM.at(m).at(p).at(r) -> Write();
          gen_asymmetries_SM.at(m).at(p).at(r) -> Write();
          asymmetries_data_SM.at(m).at(p).at(r) -> Write();
        }
      }
    }
    for( unsigned int m = 0; m < asymmetries_FE.size(); m++){
      for( unsigned int p = 0; p < asymmetries_FE.at(m).size(); p++){
        for( unsigned int r = 0; r < asymmetries_FE.at(m).at(p).size(); r++){
          asymmetries_FE.at(m).at(p).at(r) -> Write();
          gen_asymmetries_FE.at(m).at(p).at(r) -> Write();
          asymmetries_data_FE.at(m).at(p).at(r) -> Write();
        }
      }
    }
    asyroot.Close();

  }
  if (plot_all) {
    ////////////////////////////////////////////////////////////////////////////
    //  Plots with widths(alpha)                                              //
    ////////////////////////////////////////////////////////////////////////////

    PLOT_WIDTH(widths_hist_data_SM, widths_hist_SM, gen_widths_hist_SM, outdir+"pdfy/widths/", eta_bins_edge_SM,Pt_bins_Central,Pt_bins_HF);
    PLOT_WIDTH(widths_hist_data_FE, widths_hist_FE, gen_widths_hist_FE, outdir+"pdfy/widths/", eta_bins_edge_SM,Pt_bins_Central,Pt_bins_HF);

    PLOT_WIDTH_gr(data_correlated_graphs_SM, MC_correlated_graphs_SM, gen_correlated_graphs_SM, outdir+"ClosureTest/", eta_bins_edge_SM, Pt_bins_Central, Pt_bins_HF, false);
    PLOT_WIDTH_gr(data_correlated_graphs_FE, MC_correlated_graphs_FE, gen_correlated_graphs_FE, outdir+"ClosureTest/", eta_bins_edge_SM, Pt_bins_Central, Pt_bins_HF, true);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  Plots with all points of widths(alpha)                                                                                               //
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PLOT_ALLWIDTHS(soft_widths_hist_data_SM, soft_widths_hist_SM, soft_gen_widths_hist_SM,outdir+"pdfy/widths/allPoints_");
    PLOT_ALLWIDTHS(soft_widths_hist_data_FE, soft_widths_hist_FE, soft_gen_widths_hist_FE,outdir+"pdfy/widths/allPoints_");

    TFile widthroot(outdir+"output/widths.root","RECREATE");
    for( unsigned int m = 0; m < widths_hist_SM.size(); m++ ){
      for( unsigned int p = 0; p < widths_hist_SM.at(m).size(); p++ ){
        widths_hist_SM.at(m).at(p) -> Write();
        gen_widths_hist_SM.at(m).at(p) -> Write();
        widths_hist_data_SM.at(m).at(p) -> Write();
      }
    }
    for( unsigned int m = 0; m < widths_hist_FE.size(); m++ ){
      for( unsigned int p = 0; p < widths_hist_FE.at(m).size(); p++ ){
        widths_hist_FE.at(m).at(p) -> Write();
        gen_widths_hist_FE.at(m).at(p) -> Write();
        widths_hist_data_FE.at(m).at(p) -> Write();
      }
    }
    h_chi2_tot->Write();
    widthroot.Close();

  }

  std::cout << "plot_all : " << plot_all << '\n';
  /////////////////////////////////////////////////////////////////////////////////////////
  // plot with JERs with NSC fit
  /////////////////////////////////////////////////////////////////////////////////////////

  TFile NSCroot(outdir+"output/NSC.root","RECREATE");
  PLOT_NCS(JER_uncorrelated_data_hist_SM,JER_uncorrelated_MC_hist_SM,JER_uncorrelated_scale_hist_SM,outdir,eta_bins_edge_SM,false);
  PLOT_NCS(JER_correlated_data_hist_SM,JER_correlated_MC_hist_SM,JER_correlated_scale_hist_SM,outdir,eta_bins_edge_SM,false);
  PLOT_NCS(JER_uncorrelated_data_hist_FE,JER_uncorrelated_MC_hist_FE,JER_uncorrelated_scale_hist_FE,outdir,eta_bins_edge_FE, true);
  PLOT_NCS(JER_correlated_data_hist_FE,JER_correlated_MC_hist_FE,JER_correlated_scale_hist_FE,outdir,eta_bins_edge_FE, true);

  NSCroot.Close();

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  TFile JERroot(outdir+"output/JERs.root","RECREATE");

  for( unsigned int m = 0; m < JER_uncorrelated_MC_hist_SM.size(); m++ ){           JER_uncorrelated_MC_hist_SM.at(m)           -> Write();}
  for( unsigned int m = 0; m < JER_uncorrelated_data_hist_SM.size(); m++ ){         JER_uncorrelated_data_hist_SM.at(m)         -> Write();}
  for( unsigned int m = 0; m < JER_uncorrelated_MC_hist_FE.size(); m++ ){           JER_uncorrelated_MC_hist_FE.at(m)           -> Write();}
  for( unsigned int m = 0; m < JER_uncorrelated_data_hist_FE.size(); m++ ){         JER_uncorrelated_data_hist_FE.at(m)         -> Write();}
  for( unsigned int m = 0; m < JER_uncorrelated_MC_hist_FE_control.size(); m++ ){   JER_uncorrelated_MC_hist_FE_control.at(m)   -> Write();}
  for( unsigned int m = 0; m < JER_uncorrelated_data_hist_FE_control.size(); m++ ){ JER_uncorrelated_data_hist_FE_control.at(m) -> Write();}
  for( unsigned int m = 0; m < JER_correlated_MC_hist_SM.size(); m++ ){             JER_correlated_MC_hist_SM.at(m)             -> Write();}
  for( unsigned int m = 0; m < JER_correlated_data_hist_SM.size(); m++ ){           JER_correlated_data_hist_SM.at(m)           -> Write();}
  for( unsigned int m = 0; m < JER_correlated_MC_hist_FE.size(); m++ ){             JER_correlated_MC_hist_FE.at(m)             -> Write();}
  for( unsigned int m = 0; m < JER_correlated_data_hist_FE.size(); m++ ){           JER_correlated_data_hist_FE.at(m)           -> Write();}
  for( unsigned int m = 0; m < JER_correlated_MC_hist_FE_control.size(); m++ ){     JER_correlated_MC_hist_FE_control.at(m)     -> Write();}
  for( unsigned int m = 0; m < JER_correlated_data_hist_FE_control.size(); m++ ){   JER_correlated_data_hist_FE_control.at(m)   -> Write();}

  JERroot.Close();

  TFile gbis(outdir+"output/SFs.root","RECREATE");

  std::vector<double> SF_uncorrelated_SM, SF_uncorrelated_SM_error, SF_correlated_SM, SF_correlated_SM_error, SF_uncorrelated_FE, SF_uncorrelated_FE_error, SF_correlated_FE, SF_correlated_FE_error, eta_bin_SM_center, eta_bin_SM_error, eta_bin_FE_center, eta_bin_FE_error;
  std::vector<double> SF_uncorrelated_SM_ptdep_min, SF_correlated_SM_ptdep_min, SF_uncorrelated_FE_ptdep_min, SF_correlated_FE_ptdep_min;
  std::vector<double> SF_uncorrelated_SM_ptdep_max, SF_correlated_SM_ptdep_max, SF_uncorrelated_FE_ptdep_max, SF_correlated_FE_ptdep_max;
  ofstream texfile;
  texfile.open ("output/scalefactors_tex.txt");

  texfile << "standard method\n";
  SFtoTXT(texfile, JER_uncorrelated_scale_hist_SM, width_pt_SM, SF_uncorrelated_SM, SF_uncorrelated_SM_error, SF_uncorrelated_SM_ptdep_min, SF_uncorrelated_SM_ptdep_max, eta_bin_SM_center, eta_bin_SM_error, eta_bins_edge_SM, shift_, false, false);

  texfile << "\n forward extension \n";
  SFtoTXT(texfile, JER_uncorrelated_scale_hist_FE, width_pt_FE, SF_uncorrelated_FE, SF_uncorrelated_FE_error, SF_uncorrelated_FE_ptdep_min, SF_uncorrelated_FE_ptdep_max, eta_bin_FE_center, eta_bin_FE_error, eta_bins_edge_SM, shift_, true, false);

  texfile << "\n standard method, correlated fit\n";
  SFtoTXT(texfile, JER_correlated_scale_hist_SM, width_pt_SM, SF_correlated_SM, SF_correlated_SM_error, SF_correlated_SM_ptdep_min, SF_correlated_SM_ptdep_max, eta_bin_SM_center, eta_bin_SM_error, eta_bins_edge_SM, shift_, false, true);

  texfile << "\n forward extension, correlated fit \n";
  SFtoTXT(texfile, JER_correlated_scale_hist_FE, width_pt_FE, SF_correlated_FE, SF_correlated_FE_error, SF_correlated_FE_ptdep_min, SF_correlated_FE_ptdep_max, eta_bin_FE_center, eta_bin_FE_error, eta_bins_edge_SM, shift_, true, true);

  texfile << "\n";
  texfile.close();

  gbis.Close();
  ofstream txt_ST, txt_FE;
  txt_ST.open ("output/scalefactors_ST.txt");
  txt_FE.open ("output/scalefactors_FE.txt");
  // std::cout << "#eta_bin_SM_center" << " " << "eta_bin_SM_error" << " " << "SF_uncorrelated_SM" << " " << "SF_uncorrelated_SM_error" << " " << "SF_correlated_SM" << " " << "SF_correlated_SM_error" << "\n";
  txt_ST << "#eta_bin_SM_center" << " " << "eta_bin_SM_error" << " " << "SF_uncorrelated_SM" << " " << "SF_uncorrelated_SM_error" << " " << "SF_correlated_SM" << " " << "SF_correlated_SM_error" << "\n";
  txt_FE << "#eta_bin_FE_center" << " " << "eta_bin_FE_error" << " " << "SF_uncorrelated_FE" << " " << "SF_uncorrelated_FE_error" << " " << "SF_correlated_FE" << " " << "SF_correlated_FE_error" << "\n";

  // std::cout << eta_bin_SM_center.size() << " " << eta_bin_SM_error.size() << " " << SF_uncorrelated_SM.size() << " " << SF_uncorrelated_SM_error.size() << " " << SF_correlated_SM.size() << " " << SF_correlated_SM_error.size() << " " << SF_uncorrelated_SM_ptdep_min.size() << " " << SF_uncorrelated_SM_ptdep_max.size() << " " << SF_correlated_SM_ptdep_min.size() << " " << SF_correlated_SM_ptdep_max.size() << "\n";
  for (unsigned int i = 0; i < JER_uncorrelated_scale_hist_SM.size(); i++) {
    txt_ST << eta_bin_SM_center[i] << " " << eta_bin_SM_error[i] << " " << SF_uncorrelated_SM[i] << " " << SF_uncorrelated_SM_error[i] << " " << SF_correlated_SM[i] << " " << SF_correlated_SM_error[i] << " " << SF_uncorrelated_SM_ptdep_min[i] << " " << SF_uncorrelated_SM_ptdep_max[i] << " " << SF_correlated_SM_ptdep_min[i] << " " << SF_correlated_SM_ptdep_max[i] << "\n";
    if (debug) std::cout << eta_bin_SM_center[i] << " " << eta_bin_SM_error[i] << " " << SF_uncorrelated_SM[i] << " " << SF_uncorrelated_SM_error[i] << " " << SF_correlated_SM[i] << " " << SF_correlated_SM_error[i] << " " << SF_uncorrelated_SM_ptdep_min[i] << " " << SF_uncorrelated_SM_ptdep_max[i] << " " << SF_correlated_SM_ptdep_min[i] << " " << SF_correlated_SM_ptdep_max[i] << "\n";
  }
  for (unsigned int i = 0; i < JER_uncorrelated_scale_hist_FE.size(); i++) {
    txt_FE << eta_bin_FE_center[i] << " " << eta_bin_FE_error[i] << " " << SF_uncorrelated_FE[i] << " " << SF_uncorrelated_FE_error[i] << " " << SF_correlated_FE[i] << " " << SF_correlated_FE_error[i] << " " << SF_uncorrelated_FE_ptdep_min[i] << " " << SF_uncorrelated_FE_ptdep_max[i] << " " << SF_correlated_FE_ptdep_min[i] << " " << SF_correlated_FE_ptdep_max[i] << "\n";
    if (debug) std::cout << eta_bin_FE_center[i] << " " << eta_bin_FE_error[i] << " " << SF_uncorrelated_FE[i] << " " << SF_uncorrelated_FE_error[i] << " " << SF_correlated_FE[i] << " " << SF_correlated_FE_error[i] << " " << SF_uncorrelated_FE_ptdep_min[i] << " " << SF_uncorrelated_FE_ptdep_max[i] << " " << SF_correlated_FE_ptdep_min[i] << " " << SF_correlated_FE_ptdep_max[i] << "\n";
  }
  txt_ST.close();
  txt_FE.close();

  TCanvas* canv_SF = tdrCanvas("JER SF",eta_bins_edge_SM[0]-0.1, eta_bins_edge_SM[EtaBins_SM + EtaBins_FE]+0.5, 0.8, 3.0, "#eta", "JER SF");
  canv_SF->SetTickx(0);
  canv_SF->SetTicky(0);
  TGraphErrors *gr_st = new TGraphErrors(JER_correlated_scale_hist_SM.size(), &(eta_bin_SM_center[0]), &(SF_correlated_SM[0]), &(eta_bin_SM_error[0]), &(SF_correlated_SM_error[0]));
  TGraphErrors *gr_fe = new TGraphErrors(JER_correlated_scale_hist_FE.size(), &(eta_bin_FE_center[0]), &(SF_correlated_FE[0]), &(eta_bin_FE_error[0]), &(SF_correlated_FE_error[0]));
  tdrDraw(gr_st, "P", kFullDotLarge, kRed);
  tdrDraw(gr_fe, "P", kFullDotLarge, kBlue);

  TLegend *leg = tdrLeg(0.70,0.75,0.9,0.9);
  leg->AddEntry(gr_st, "Standard","lep");
  leg->AddEntry(gr_fe, "Forward ext", "lep");
  leg->Draw("same");

  canv_SF->SaveAs(outdir+"output/SF.pdf");
  // canv_SF->SaveAs(outdir+"output/SF.png");
  // canv_SF->SaveAs(outdir+"output/SF.root");

  //////////////////////////////////////////////////////////////////////////////////////////
  //    SF plots overlayed with ...                                                       //
  //////////////////////////////////////////////////////////////////////////////////////////
  TFile SFsoverlayed(outdir+"output/SFsoverlayed.root","RECREATE");

  PLOT_SF(JER_uncorrelated_scale_hist_SM, JER_correlated_scale_hist_SM, JER015_scale_hist_SM, outdir+"pdfy/SFs/", eta_bins_edge_SM, false);
  PLOT_SF(JER_uncorrelated_scale_hist_FE, JER_correlated_scale_hist_FE, JER015_scale_hist_FE, outdir+"pdfy/SFs/", eta_bins_edge_FE, true);

  SFsoverlayed.Close();

  return true;

}
