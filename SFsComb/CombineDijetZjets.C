// Combine Dijet and Z+jets JER SFs, stored as 2D histograms
// author: A.Karavdina
// date: 14.07.2019
// Run it with following command:
// root -l -b -q CombineDijetZjets.C\(\"Comb_ZjetV16_DijetV16\"\)

//exract x values for one y bin of TH2 histogram and store them as TGraphErrors (Z+jets)
TGraphErrors* extractGraph(TH2F *h1, double y_val){ 
  //  cout<<"y_val = "<<y_val<<endl;
  //  if(y_val<0.261) y_val+=0.261;//Add very first bin
  //  if(y_val>0.3 && y_val<0.6525) y_val=0.6525+0.05;//Read proper 2nd bin
  int n_y = h1->GetNbinsY();
  int n_x = h1->GetNbinsX();
  int y_bin = h1->GetYaxis()->FindBin(y_val);

  double x_val[n_x], x_val_err[n_x], SF_val[n_x], SF_val_err[n_x];
  for(int ix=1;ix<n_x+1;ix++){
  //  int nx = 0;
  //  for(int ix=1;ix<6;ix++){//TEST for 2.85 eta
    x_val[ix-1] = h1->GetXaxis()->GetBinCenter(ix);
    x_val_err[ix-1] = 1e-2;
    SF_val[ix-1] = h1->GetBinContent(ix,y_bin); 
    SF_val_err[ix-1] = h1->GetBinError(ix,y_bin);
    //  nx++;
  }
  TGraphErrors* gr = new TGraphErrors(n_x,x_val,SF_val,x_val_err,SF_val_err);
  //  TGraphErrors* gr = new TGraphErrors(nx,x_val,SF_val,x_val_err,SF_val_err);
  return gr;
}


//exract x values for one y bin of TH2Poly histogram and store them as TGraphErrors (Dijets)
TGraphErrors* extractGraph(TH2Poly *h1, double y_val){ 
  TList* binlist = h1->GetBins();
  vector<double> xval, xvalerr,SFval,SFvalerr; 
  for(const auto&& bin: *binlist){
    if(y_val>=((TH2PolyBin*)bin)->GetYMin() && y_val<((TH2PolyBin*)bin)->GetYMax()){
      SFval.push_back(h1->GetBinContent(((TH2PolyBin*)bin)->GetBinNumber()));
      SFvalerr.push_back(h1->GetBinError(((TH2PolyBin*)bin)->GetBinNumber()));
      //      cout<<"Dijet stat error: "<<h1->GetBinError(((TH2PolyBin*)bin)->GetBinNumber())<<endl;
      xval.push_back(0.5*(((TH2PolyBin*)bin)->GetXMin()+((TH2PolyBin*)bin)->GetXMax()));
      xvalerr.push_back(1e-2);
    }
  }
  int n_x = SFval.size();
  double x_val[n_x], x_val_err[n_x], SF_val[n_x], SF_val_err[n_x];
  for(int i=0;i<n_x;i++){
    x_val[i] = xval[i];
    x_val_err[i] = xvalerr[i];
    SF_val[i] = SFval[i];
    SF_val_err[i] = SFvalerr[i];
  }

  TGraphErrors* gr = new TGraphErrors(n_x,x_val,SF_val,x_val_err,SF_val_err);
  return gr;
} 

//Clean points not filled due to low statistics
TGraphErrors* CleanEmptyPoints(TGraphErrors* input){
  double *Yval = input->GetY();
  double *YvalError = input->GetEY();
  double *Xval = input->GetX();
  double *XvalError = input->GetEX();
  int count=0;
  vector<double> Xnew,Ynew,Xerrornew,Yerrornew;
  for(int i=0;i<input->GetN();i++){
    //    if(YvalError[i]<1e-4 || Yval[i]==0) continue;
    //    if(Yval[i]==0) continue;
    if(Yval[i]==0 or Yval[i]<1) continue;//remove empty points and points with SFs<1
    count++;
    Xnew.push_back(Xval[i]);       
    Ynew.push_back(Yval[i]);
    Xerrornew.push_back(XvalError[i]);       
    Yerrornew.push_back(YvalError[i]);    
  }

  const int NnewSize =  count;
  double Xnew_m[NnewSize],Ynew_m[NnewSize],Xerrornew_m[NnewSize],Yerrornew_m[NnewSize]; //because silly ROOT doesn't know how to treat vectors
  for(int i=0;i<NnewSize;i++){
    Xnew_m[i] = Xnew[i];
    Ynew_m[i] = Ynew[i];
    Xerrornew_m[i] = Xerrornew[i];
    Yerrornew_m[i] = Yerrornew[i];
  }

  TGraphErrors* output = new TGraphErrors(count,Xnew_m,Ynew_m,Xerrornew_m,Yerrornew_m);
  if(input->GetN()!=output->GetN()) cout<<"Number of points in input: "<<input->GetN()<<" in output: "<<output->GetN()<<endl;
  return output;
}


//add Dijet systematics, hardcoded values are based on total JER SF uncertainty estimate V1 
//https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetResolution#2018_data
void addDijetSystemtics(TGraphErrors* gr, double eta_val){
  double systm_err = 0;
  // if(fabs(eta_val)<1.3)  systm_err = 0.10;
  // else if(fabs(eta_val)<2.85)  systm_err = 0.20;
  //   else if(fabs(eta_val)<2.96 && fabs(eta_val)>2.85) systm_err = 0.90;
  // else if(fabs(eta_val)<5.2) systm_err = 0.20;
  // //  cout<<"eta = "<<eta_val<<"  systm_err = "<<systm_err<<endl;
  if(fabs(eta_val)<1.3)  systm_err = 0.07;
  else if(fabs(eta_val)<2.5)  systm_err = 0.10;
  //  else if(fabs(eta_val)<2.96 && fabs(eta_val)>2.5) systm_err = 0.30;
  else if(fabs(eta_val)<2.96 && fabs(eta_val)>2.5) systm_err = 0.20;
  //else if(fabs(eta_val)<2.96 && fabs(eta_val)>2.5) systm_err = 0.10; //TEST
  else if(fabs(eta_val)<5.2) systm_err = 0.15;
  for(int i;i<gr->GetN();i++)
    gr->SetPointError(i,1e-2,TMath::Hypot(gr->GetErrorY(i),systm_err));
}

//Step-like function (stolen from trigger thresholds fit)
Double_t SmoothFit(Double_t *v, Double_t *par){
  Double_t fitval  = 0.;
  if(par[2] != 0.){
    //    fitval = 0.5 * par[2] * (1. + TMath::Erf((v[0]-par[0]) / (TMath::Power(2, 0.5) * par[1] ) ) );
    fitval = 0.5 * par[2] * (par[3] + TMath::Erf((v[0]-par[0]) / (TMath::Power(2, 0.5) * par[1] ) ) );
  }
  return fitval;
}

// SmoothFitECAL proposed by Mikko:
// The basic starting point was that our jet response should follow the NSC parameterisation
// sigma/pT = sqrt(N^2 / pT^2 + S^2 / pT^2 + C^2)
// There parameters N, S and C can differ between the two so the data/MC ratio is generally fit by 
// (sigma/pT)_{data/MC} = sqrt(N'^2 / pT^2 + S'^2 / pT^2 + C'^2) / sqrt(N^2 / pT^2 + S^2 / pT^2 + C^2)
// Given that we usually expect only small differences, one can write
// N’ = k_N * N
// S’ = k_S * S
// C’ = k_C * C
// At the moment, we effectively assume k_N = k_S = k_C = k such that the above equation simplifies to a flat constant
// (sigma/pT)_{data/MC} = k
// My proposal is to assume that at least k_N and k_C are different. This is motivated by physics. Former is sensitive to pileup noise, which random cone measurements suggest is well modelled, so k_N ~ 1. On the other hand the ECAL crystal inter calibrations in the end caps are questionable, so k_C >> 1.
// However, it is not a priori clear to me what to assume for S, whether k_S ~ 1, k_S >>1 or something in between. In addition, the factorisation of the effects into N, S and C in the MC truth JER fit does not seem to be perfect at least in the end caps, so it would be better to use some effective parameterization that only has N+C terms. Thus my proposal for this function f3, which has S=0 (although equation was written with k_S=k_C) and N+C are enlarged to incorporate relevant parts of S:

Double_t SmoothFitECAL(Double_t *v, Double_t *par){
  Double_t fitval  = 0.;
  if(par[2] != 0.){
    fitval = sqrt(pow(par[0],2)/(v[0]*v[0])+pow(sqrt(par[3])*par[1],2)/v[0]+pow(par[3]*par[2],2)) / sqrt(pow(par[3]*par[0],2)/(v[0]*v[0])+pow(par[3]*par[1],2)/v[0]+pow(par[3]*par[2],2)) * par[3];
    //    fitval = sqrt(pow(par[0],2)/(v[0]*v[0])+pow(par[1],2)/v[0]+pow(par[2],2)) / sqrt(pow(par[0],2)/(v[0]*v[0])+pow(par[1],2)/v[0]+pow(par[3]*par[2],2)) * par[3];
  }
  return fitval;
}

Double_t SmoothFitECALReduced(Double_t *v, Double_t *par){
  Double_t fitval  = 0.;
  if(par[2] != 0.){
    fitval = sqrt(pow(par[0]*par[2],2)/(v[0]*v[0])+pow(par[1]*par[3],2)) / sqrt(pow(par[2],2)/(v[0]*v[0])+pow(par[3],2)) ;  }
  return fitval;
}


//evaluate fit results and store output as txt file
void OutputSFs(TF1* fit_func_custom, TString name, double eta_val1, double eta_val2){
  double total_uncert = 0.10;
  // dijet alone:
  // https://indico.cern.ch/event/837707/#5-dijet-jer-sf-update-for-2018
  // https://twiki.cern.ch/twiki/bin/view/Sandbox/TestTopic11111209
  // To cover discrepancy between ABC and D, uncertainty increased in some bins 
  // Z+jets ABC vs D comparison: https://indico.cern.ch/event/840539/#14-update-on-jer-from-zjet
  if((eta_val1-2.322)<1e-2 && (eta_val2-2.5)<1e-2) total_uncert = 0.10;//dijet alone 0.06
  if((eta_val1-2.5)<1e-2 && (eta_val2-2.65)<1e-2) total_uncert = 0.40;//dijet alone 0.30
  if((eta_val1-2.65)<1e-2 && (eta_val2-2.853)<1e-2) total_uncert = 0.50; //dijet alone 0.30
  if((eta_val1-2.853)<1e-2 && (eta_val2-2.964)<1e-2) total_uncert = 0.30;//dijet alone 0.30
  if((eta_val1-2.964)<1e-2 && (eta_val2-3.139)<1e-2) total_uncert = 0.20;//dijet alone 0.10


  std::ofstream outfile("JERSFs_Zjets_Dijets_Combined.txt", std::ios_base::app | std::ios_base::out);
  int pt_min = 0; int pt_max = 500;
  double pt_cur = pt_min;
  while(pt_cur<pt_max+10){
    int pt_1 = pt_cur;
    int pt_2 = pt_cur+10;
    if(pt_1>100)
      pt_2 = pt_cur+25;
    if(pt_2>pt_max) pt_2=7000;
    int pt_eval = pt_1+(pt_2-pt_1)*0.5;
    double SF_cnt = fit_func_custom->Eval(pt_eval);
    double SF_dn = SF_cnt-total_uncert;
    double SF_up = SF_cnt+total_uncert;
    if(SF_cnt<1.0) SF_cnt=1.0;
    if(SF_dn<1.0) SF_dn=1.0;
    if(SF_up<1.0) SF_up=1.0;
    outfile<<-eta_val2<<" "<<-eta_val1<<" "<<pt_1<<" "<<pt_2<<" 3 "<<SF_cnt<<" "<<SF_dn<<" "<<SF_up<<"\n";
    pt_cur = pt_2;
  }
  pt_cur = pt_min;
  while(pt_cur<pt_max+10){
    int pt_1 = pt_cur;
    int pt_2 = pt_cur+10;
    if(pt_1>100)
      pt_2 = pt_cur+25;
    if(pt_2>pt_max) pt_2=7000;
    int pt_eval = pt_1+(pt_2-pt_1)*0.5;
    double SF_cnt = fit_func_custom->Eval(pt_eval);
    double SF_dn = SF_cnt-total_uncert;
    double SF_up = SF_cnt+total_uncert;
    if(SF_cnt<1.0) SF_cnt=1.0;
    if(SF_dn<1.0) SF_dn=1.0;
    if(SF_up<1.0) SF_up=1.0;
    outfile<<eta_val1<<" "<<eta_val2<<" "<<pt_1<<" "<<pt_2<<" 3 "<<SF_cnt<<" "<<SF_dn<<" "<<SF_up<<"\n";
    //    outfile<<-eta_val2<<" "<<-eta_val1<<" "<<pt_1<<" "<<pt_2<<" 3 "<<SF_cnt<<" "<<SF_dn<<" "<<SF_up<<"\n";
    //    outfile<<-eta_val2<<" "<<-eta_val1<<" "<<pt_1<<" "<<pt_2<<" "<<fit_func_custom->Eval(pt_eval)<<"\n";//saves "Hello" to the outfile with the insertion operator
    pt_cur = pt_2;
  }
}

//fit 2 graphs, store result as plot
int doFit(TGraphErrors* gr1, TGraphErrors*gr2, TString lab1, TString lab2, TString leg_title, TString name, double chi2_thr, TString tag_name, bool skipZjets, bool fixP3_1, double eta_val1, double eta_val2){
  gStyle->SetOptStat(0);
  gStyle->SetTitleSize(0.045,"x");  
  gStyle->SetTitleSize(0.045,"y");
  gStyle->SetTitleYOffset(0.9);
  double w = 600;
  double h = 600;
  gr1->SetMarkerColor(kRed);
  gr2->SetMarkerColor(kBlue);
  gr1->SetMarkerStyle(20);
  gr2->SetMarkerStyle(22);
  gr1->SetMarkerSize(1.2);
  gr2->SetMarkerSize(1.2);

  TMultiGraph *mg = new TMultiGraph();
  //  if(!skipZjets) mg->Add(gr1,"p");
  mg->Add(gr1,"p");
  mg->Add(gr2,"p");
  TLegend* legend = new TLegend(0.15,0.75,0.38,0.95);
  legend->SetHeader(leg_title,"C"); // option "C" allows to center the header
  legend->AddEntry(gr1,lab1,"lep");
  legend->AddEntry(gr2,lab2,"lep");

  TLatex *tex2_rel = new TLatex();
  tex2_rel->SetNDC();
  tex2_rel->SetTextSize(0.035); 

  TCanvas* c0 = new TCanvas("SFCombined","Dijet and Z+jets JER SFs",w,h);
  mg->Draw("a");

  //Try with const fit first
  TF1* fit_func = new TF1("poly0","pol0");
  TFitResultPtr fit_SF = mg->Fit(fit_func,"FQN");
  float chi2 = fit_func->GetChisquare()/fit_func->GetNDF();
  TString chi2_val_str; 
  chi2_val_str.Form("%3.1f",chi2);
  TString chi2_rel = "#chi^{2}/n.d.f = ";
  chi2_rel +=chi2_val_str;
  TString par0_val_str; TString par0_err_str;
  float SF_const = fit_func->GetParameter(0);
  float SF_const_err = fit_func->GetParError(0);
  par0_val_str.Form("%3.2f",SF_const);
  par0_err_str.Form("%3.2f",SF_const_err);
  TString prt_SF_const = "Const Fit: ";
  prt_SF_const +=par0_val_str;  prt_SF_const += " +/- "; prt_SF_const += par0_err_str;
  tex2_rel->DrawLatex(0.54,0.85,prt_SF_const);
  tex2_rel->DrawLatex(0.54,0.80,chi2_rel);

  //If const fit does not look good, try step-like function
  if(chi2>chi2_thr){
    // //Fit with Error function, aka function #1
    // TF1* fit_func_custom = new TF1("SF_custom",SmoothFit,15,1000,4);
    // //   fit_func_custom->SetParameters(200, 20., 2.5);
    // //    fit_func_custom->SetParameters(210, 10., 0.5, 4.0);
    // if(skipZjets) fit_func_custom->SetParameters(180, 50., 1.5, 2.5);
    // else fit_func_custom->SetParameters(180, 50., 1.8, 4.0);
    // fit_func_custom->SetParNames("p0", "p1", "N","p3");

    // // //Fit with function suggested by Mikko, aka Function #2
    // TF1* fit_func_custom = new TF1("SF_custom",SmoothFitECAL,15,1000,4);
    // fit_func_custom->SetParameters(5*2, 0, 0.05*sqrt(2), 2.020);
    // fit_func_custom->FixParameter(1, 0);
    // fit_func_custom->FixParameter(2, 0.05*sqrt(2.));
    // fit_func_custom->SetParNames("p0", "p1", "p2","p3");

    //Another fit function suggested by Mikko, aka Function #3
    TF1* fit_func_custom = new TF1("SF_custom",SmoothFitECALReduced,15,1000,4);
    fit_func_custom->SetParameters(1,2, 5*2,0.05*sqrt(2));


    //function3 as shown on sl.6 in https://indico.cern.ch/event/843251/contributions/3541684/attachments/1897252/3130524/20190826_JER_SFs_V16_DiJet_ZJets_Combine.pdf
    fit_func_custom->FixParameter(2, 25);
    if(fixP3_1){
      fit_func_custom->FixParameter(3, 0.05*sqrt(2.));
    }
    else{
      fit_func_custom->FixParameter(3, 0.14);
    }

    // if(fixP3_1){
    //   //      fit_func_custom->FixParameter(3, 0.05*sqrt(2.));
    //   // fit_func_custom->FixParameter(2, 8.59);
    //   // fit_func_custom->FixParameter(3, 0.062);
    //   fit_func_custom->FixParameter(2, 8.86);
    //   fit_func_custom->FixParameter(3, 0.098);

    // }
    // else{
    //   //      fit_func_custom->FixParameter(3, 0.14);
    //   fit_func_custom->FixParameter(2, 8.86);
    //   fit_func_custom->FixParameter(3, 0.098);
    //   // fit_func_custom->FixParameter(2, 8.59);
    //   // fit_func_custom->FixParameter(3, 0.062);
    // }
    fit_func_custom->SetParNames("p0", "p1", "p2","p3");



    TFitResultPtr fit_SF2 = mg->Fit(fit_func_custom,"FR");

    //    cout<<"chi2 fit: "<<fit_func_custom->GetChisquare()/fit_func_custom->GetNDF()<<endl;
    /*Create a TGraphErrors to hold the confidence intervals*/
   TGraphErrors *grint1 = new TGraphErrors(gr1->GetN());
   for (int i=0; i<gr1->GetN(); i++)
     grint1->SetPoint(i, gr1->GetX()[i], 0);
   /*Compute the confidence intervals at the x points of the created graph*/
   (TVirtualFitter::GetFitter())->GetConfidenceIntervals(grint1);
   for (int i=0; i<gr1->GetN(); i++)
     grint1->SetPoint(i, gr1->GetX()[i], fit_func_custom->Eval(gr1->GetX()[i]));
   TGraphErrors *grint2 = new TGraphErrors(gr2->GetN());
   for (int i=0; i<gr2->GetN(); i++){
     grint2->SetPoint(i, gr2->GetX()[i], 0);
     cout<<"pt_bin: "<<gr2->GetX()[i]<<endl;
   }
   /*Compute the confidence intervals at the x points of the created graph*/
   (TVirtualFitter::GetFitter())->GetConfidenceIntervals(grint2);
   for (int i=0; i<gr2->GetN(); i++)
     grint2->SetPoint(i, gr2->GetX()[i], fit_func_custom->Eval(gr2->GetX()[i]));
   //Now the "grint" graph contains function values as its y-coordinates
   //and confidence intervals as the errors on these coordinates
   grint1->SetLineColor(kRed-1);
   grint1->SetFillColorAlpha(kRed-1,0.3);
   grint1->Draw("e3 same");
   grint2->SetLineColor(kBlue-1);
   grint2->SetFillColorAlpha(kBlue-1,0.3);
   grint2->Draw("e3 same");

   chi2 = fit_func_custom->GetChisquare()/fit_func_custom->GetNDF();
   chi2_val_str.Form("%3.1f",chi2);
   TString chi2_rel = "#chi^{2}/n.d.f = ";
   chi2_rel +=chi2_val_str;
   TString step_par0_val_str; TString step_par0_err_str;
   TString step_par1_val_str; TString step_par1_err_str;
   TString step_par2_val_str; TString step_par2_err_str;
   TString step_par3_val_str; TString step_par3_err_str;
   float Step_par0 = fit_func_custom->GetParameter(0);
   float Step_par0_err = fit_func_custom->GetParError(0);
   float Step_par1 = fit_func_custom->GetParameter(1);
   float Step_par1_err = fit_func_custom->GetParError(1);
   float Step_par2 = fit_func_custom->GetParameter(2);
   float Step_par2_err = fit_func_custom->GetParError(2);
   float Step_par3 = fit_func_custom->GetParameter(3);
   float Step_par3_err = fit_func_custom->GetParError(3);

   step_par0_val_str.Form("%3.2f",Step_par0);
   step_par0_err_str.Form("%3.2f",Step_par0_err);
   step_par1_val_str.Form("%3.2f",Step_par1);
   step_par1_err_str.Form("%3.2f",Step_par1_err);
   step_par2_val_str.Form("%3.2f",Step_par2);
   step_par2_err_str.Form("%3.2f",Step_par2_err);
   step_par3_val_str.Form("%3.2f",Step_par3);
   step_par3_err_str.Form("%3.2f",Step_par3_err);

   TString prt0_SF_step = "par0 = ";
   prt0_SF_step +=step_par0_val_str;  prt0_SF_step += " +/- "; prt0_SF_step += step_par0_err_str;
   TString prt1_SF_step = "par1 = ";
   prt1_SF_step +=step_par1_val_str;  prt1_SF_step += " +/- "; prt1_SF_step += step_par1_err_str;
   TString prt2_SF_step = "par2 = ";
   prt2_SF_step +=step_par2_val_str;  prt2_SF_step += " +/- "; prt2_SF_step += step_par2_err_str;
   TString prt3_SF_step = "par3 = ";
   prt3_SF_step +=step_par3_val_str;  prt3_SF_step += " +/- "; prt3_SF_step += step_par3_err_str;

   tex2_rel->DrawLatex(0.54,0.33,"Step Fit:");
   tex2_rel->DrawLatex(0.54,0.29,prt0_SF_step);
   tex2_rel->DrawLatex(0.54,0.25,prt1_SF_step);
   tex2_rel->DrawLatex(0.54,0.21,prt2_SF_step);
   tex2_rel->DrawLatex(0.54,0.17,prt3_SF_step);
   tex2_rel->DrawLatex(0.54,0.13,chi2_rel);
   OutputSFs(fit_func_custom, name, eta_val1, eta_val2);
  }
  else{
    TFitResultPtr fit_SF = mg->Fit(fit_func,"F");

    /*Create a TGraphErrors to hold the confidence intervals*/
   TGraphErrors *grint1 = new TGraphErrors(gr1->GetN());
   for (int i=0; i<gr1->GetN(); i++)
     grint1->SetPoint(i, gr1->GetX()[i], 0);
   /*Compute the confidence intervals at the x points of the created graph*/
   (TVirtualFitter::GetFitter())->GetConfidenceIntervals(grint1);
   for (int i=0; i<gr1->GetN(); i++)
     grint1->SetPoint(i, gr1->GetX()[i], fit_func->Eval(gr1->GetX()[i]));
   TGraphErrors *grint2 = new TGraphErrors(gr2->GetN());
   for (int i=0; i<gr2->GetN(); i++)
     grint2->SetPoint(i, gr2->GetX()[i], 0);
   /*Compute the confidence intervals at the x points of the created graph*/
   (TVirtualFitter::GetFitter())->GetConfidenceIntervals(grint2);
   for (int i=0; i<gr2->GetN(); i++)
     grint2->SetPoint(i, gr2->GetX()[i], fit_func->Eval(gr2->GetX()[i]));
   //Now the "grint" graph contains function values as its y-coordinates
   //and confidence intervals as the errors on these coordinates
   grint1->SetLineColor(kRed-1);
   grint1->SetFillColorAlpha(kRed-1,0.3);
   grint1->Draw("e3 same");
   grint2->SetLineColor(kBlue-1);
   grint2->SetFillColorAlpha(kBlue-1,0.3);
   grint2->Draw("e3 same");
  }

  mg->GetXaxis()->SetTitle("p_{T}");
  mg->GetYaxis()->SetTitle("JER SF");
  mg->GetYaxis()->SetRangeUser(0,3.0);
  mg->GetXaxis()->SetLimits(10,2000.);
  c0->Modified();
  legend->Draw();
  c0->SetLogx();
  c0->SaveAs(name+"_"+tag_name+".pdf");


  return 0;

}



void CombineDijetZjets(TString tag_name="Ngenjet"){
  gStyle->SetOptStat(0);
  gStyle->SetTitleSize(0.045,"x");  
  gStyle->SetTitleSize(0.045,"y");
  gStyle->SetTitleYOffset(0.9);

  double w = 600;
  double h = 600;


 //Files after dijet and Z+jets standalone analysis
 // TString path_dijet = "/nfs/dust/cms/user/amalara/WorkingArea/UHH2_102X_v1/CMSSW_10_2_10/src/UHH2/DiJetJERC/JERSF_Analysis/JER/wide_eta_binning/file/MergeL2Res/Autumn18_V16h/AK4CHS/standard/QCDHT/RunABCD/output/"; //V16h

 // TString path_dijet = "/nfs/dust/cms/user/karavdia/CMSSW_10_2_10/src/UHH2/JERSF/Analysis/JER/wide_eta_bin_moreECbins_morePtbins_EnergyEtaCut_fixJetSorting/file/StandardPtBins/Autumn18_V15/AK4CHS/standard/QCDHT/RunABCD/output/"; //V15
 // TString path_dijet = "/nfs/dust/cms/user/karavdia/CMSSW_10_2_10/src/UHH2/JERSF/Analysis/JER/wide_eta_bin_moreECbins_morePtbins_EnergyEtaCut_fixJetSorting_positiveEta/file/StandardPtBins/Autumn18_V15/AK4CHS/standard/QCDHT/RunABCD/output/";
  //  TString path_dijet = "/nfs/dust/cms/user/amalara/WorkingArea/UHH2_102X_v1/CMSSW_10_2_10/src/UHH2/DiJetJERC/JERSF_Analysis/JER/wide_eta_binning/file/MergeL2Res/Autumn18_V16/AK4CHS/standard/QCDHT/RunABCD/output/"; //V16
  //  TString path_dijet = "/nfs/dust/cms/user/amalara/WorkingArea/UHH2_102X_v1/CMSSW_10_2_10/src/UHH2/DiJetJERC/JERSF_Analysis/JER/wide_eta_binning/file/MergeL2Res/Autumn18_V16/AK4CHS/standard/QCDHT/RunABC/output/"; //V16
  //  TString path_dijet = "/nfs/dust/cms/user/amalara/WorkingArea/UHH2_102X_v1/CMSSW_10_2_10/src/UHH2/DiJetJERC/JERSF_Analysis/JER/wide_eta_binning/file/MergeL2Res/Autumn18_V16/AK4CHS/standard/QCDHT/RunD/output/"; //V16

  //  TString path_dijet = "/nfs/dust/cms/user/amalara/WorkingArea/UHH2_102X_v1/CMSSW_10_2_10/src/UHH2/DiJetJERC/JERSF_Analysis/JER/wide_eta_binning/file/MergeL2Res/Autumn18_V17/AK4CHS/standard/QCDHT/RunABC/output/"; //V17
  //  TString path_dijet = "/nfs/dust/cms/user/amalara/WorkingArea/UHH2_102X_v1/CMSSW_10_2_10/src/UHH2/DiJetJERC/JERSF_Analysis/JER/wide_eta_binning/file/MergeL2Res/Autumn18_V17/AK4CHS/standard/QCDHT/RunD/output/"; //V17
  TString path_dijet = "/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/UHH2/DiJetJERC/JERSF_Analysis/JER/wide_eta_binning/file/MergeL2Res/Autumn18_V17/AK4CHS/standard/QCDHT/RunABCD/output/"; //V17
  TString name_dijet = "DijetJERSF2D.root";

 // TString path_zjet = "/afs/desy.de/user/k/karavdia/www/JEC_plots/ZJets_JER_combination/RunABCD/";
 // TString name_zjet = "h2_jer_sf.root";

 // TString path_zjet = "/afs/desy.de/user/k/karavdia/www/JEC_plots/ZJets_JER_combination/August19/";
 TString path_zjet = "/afs/desy.de/user/k/karavdia/www/JEC_plots/ZJets_JER_combination/August19/abs_eta/";
 TString name_zjet = "results.root";
 // TString name_zjet = "2018ABC.root";
 // TString name_zjet = "2018D.root";

 TString gl_label_dijet = "Dijet";
 TString gl_label_zjet = "Zjets";

 TFile *input_dijet = TFile::Open(path_dijet+name_dijet);
 TFile *input_zjet = TFile::Open(path_zjet+name_zjet);

 //input Z+jets root files contain TCanvas with hists
 // TCanvas *c1 = (TCanvas*)input_zjet->Get("c4");
 // TH2F *h_zjets = (TH2F*)c1->GetPrimitive("jersf");
 TH2F *h_zjets = (TH2F*)input_zjet->Get("JER_SF");

 //input dijet root files contain hists stored directly
 TH2Poly *h_dijets = (TH2Poly*)input_dijet->Get("2D_SF_FE");
 // TH2Poly *h_dijets = (TH2Poly*)input_dijet->Get("2D_SF_SM");


 // eta bins:
 int n_eta_bins = 15;
 double eta_bins[] = {0, 0.522, 0.783, 1.131, 1.305, 1.740, 1.930, 2.043, 2.322, 2.5, 2.65, 2.853, 2.964, 3.139, 5.191};
 TString eta_bins_str[] = {"0", "0.522", "0.783", "1.131", "1.305", "1.740", "1.930", "2.043", "2.322", "2.5", "2.65", "2.853", "2.964", "3.139", "5.191"};
 TString eta_bins_name [] = {"0", "0_522", "0_783", "1_131", "1_305", "1_740", "1_930", "2_043", "2_322", "2_5", "2_65", "2_853", "2_964", "3_139", "5_191"};
 TGraphErrors* gr_zjets; TGraphErrors* gr_dijets;

 for(int ieta=0;ieta< n_eta_bins-1;ieta++){
 // for(int ieta=0;ieta<1;ieta++){//TEST
   double eta_val =  eta_bins[ieta]+1e-2;
   cout<<"eta_val = "<<eta_val<<endl;
   bool skipZjets = false;
   //if(eta_val>2.65 && eta_val<2.85) skipZjets = true;//FixME: adjust Z+jets binning
   //   if(eta_val>2.85 && eta_val<2.97) skipZjets = true;//FixME: used to set diefferent params 
   gr_zjets = extractGraph(h_zjets, eta_val);
   addDijetSystemtics(gr_zjets,eta_val);//Add total systematics as for dijet
   gr_dijets = extractGraph(h_dijets, eta_val);
   gr_dijets = CleanEmptyPoints(gr_dijets);
   addDijetSystemtics(gr_dijets,eta_val);//Add total systematics for dijet
   TString legend_title = eta_bins_str[ieta];
   legend_title+="<|#eta|<"; legend_title+=eta_bins_str[ieta+1];
   //   int fitres = doFit(gr_zjets, gr_dijets, "Z+jets", "Dijets",legend_title,"JERSF_Eta_"+eta_bins_name[ieta],2, tag_name, skipZjets);
   //   int fitres = doFit(gr_zjets, gr_dijets, "Z+jets", "Dijets",legend_title,"JERSF_Eta_"+eta_bins_name[ieta],3.0, tag_name, skipZjets);
   bool fix_Par3_1 = true;//in ECALReduced fit, set par3 to lower value for eta<2.85 and to higher value otherwise
   //   cout<<"eta_val = "<<eta_val<<endl;
   if(eta_val>2.6) fix_Par3_1 = false;
   //   int fitres = doFit(gr_zjets, gr_dijets, "Z+jets", "Dijets",legend_title,"JERSF_Eta_"+eta_bins_name[ieta],2.4, tag_name, skipZjets, fix_Par3_1, eta_bins[ieta], eta_bins[ieta+1]);
   int fitres = doFit(gr_zjets, gr_dijets, "Z+jets", "Dijets",legend_title,"JERSF_Eta_"+eta_bins_name[ieta],1.5, tag_name, skipZjets, fix_Par3_1, eta_bins[ieta], eta_bins[ieta+1]);
 }
 

}
