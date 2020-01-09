#include <iostream>
#include <memory>
#include <stdlib.h>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/EventHelper.h"
#include "UHH2/core/include/Jet.h"
#include "UHH2/core/include/L1Jet.h"
#include "UHH2/core/include/Utils.h"
#include "UHH2/common/include/CommonModules.h"
#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/LumiSelection.h"
#include "UHH2/common/include/TriggerSelection.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/MuonIds.h"
#include "UHH2/common/include/ElectronIds.h"
#include "UHH2/common/include/PrintingModules.h"

#include "UHH2/DiJetJERC/include/JECAnalysisHists.h"
#include "UHH2/DiJetJERC/include/JECCrossCheckHists.h"
#include "UHH2/DiJetJERC/include/JECRunnumberHists.h"
#include "UHH2/DiJetJERC/include/JECAnalysisRecoGenMatchedHistsFractions.h"
#include "UHH2/DiJetJERC/include/JECAnalysisPUjetsHists.h"
#include "UHH2/DiJetJERC/include/LumiHists.h"
#include "UHH2/DiJetJERC/include/selection.h"
#include "UHH2/DiJetJERC/include/constants.h"
#include "TClonesArray.h"
#include "TString.h"
#include "Riostream.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2Poly.h"

using namespace std;
using namespace uhh2;

class AnalysisModule_DiJetTrg: public uhh2::AnalysisModule {

public:
  explicit AnalysisModule_DiJetTrg(uhh2::Context&);
  virtual bool process(uhh2::Event&) override;
  void init_JEC(uhh2::Context& ctx);
  void declare_output(uhh2::Context& ctx);
  void init_hists(uhh2::Context& ctx);
  ~AnalysisModule_DiJetTrg();

protected:

  // correctors
  std::unique_ptr<JetCorrector> jet_corrector_A, jet_corrector_B, jet_corrector_C, jet_corrector_D, jet_corrector_E, jet_corrector_F,
  jet_corrector_G,jet_corrector_H, jet_corrector_MC;
  std::unique_ptr<GenericJetResolutionSmearer> jet_resolution_smearer;
  // cleaners
  std::unique_ptr<JetCleaner> jetID;
  std::unique_ptr<JetCleaner> jetPUid;
  std::unique_ptr<JetCleaner> jetcleaner;
  std::unique_ptr<GenJetCleaner> genjetcleaner;
  std::unique_ptr<MuonCleaner>     muoSR_cleaner;
  std::unique_ptr<ElectronCleaner> eleSR_cleaner;

  // selections
  std::unique_ptr<uhh2::Selection> lumi_sel;
  std::unique_ptr<uhh2::AnalysisModule> PVCleaner;
  std::unique_ptr<uhh2::AndSelection> metfilters_sel;

  std::unique_ptr<uhh2::Selection> triggerSiMu_sel;

  std::vector<std::unique_ptr<uhh2::Selection> > trigger_sel, trigger_HFJEC_sel;

  //// Data/MC scale factors
  std::unique_ptr<uhh2::AnalysisModule> pileupSF, LumiWeight_module;
  std::unique_ptr<AnalysisModule> Jet_printer, GenJet_printer, GenParticles_printer;


  Event::Handle<float> tt_jet1_pt;  Event::Handle<float> tt_jet2_pt;  Event::Handle<float> tt_jet3_pt;
  Event::Handle<float> tt_jet4_pt;  Event::Handle<float> tt_jet5_pt;  Event::Handle<float> tt_jet6_pt;  Event::Handle<float> tt_jet7_pt;
  Event::Handle<float> tt_jet1_eta; Event::Handle<float> tt_jet2_eta; Event::Handle<float> tt_jet3_eta;
  Event::Handle<float> tt_jet1_phi; Event::Handle<float> tt_jet2_phi; Event::Handle<float> tt_jet3_phi;
  Event::Handle<float> tt_jet1_ptRaw;  Event::Handle<float> tt_jet2_ptRaw;  Event::Handle<float> tt_jet3_ptRaw;
  Event::Handle<float> tt_jet1_pt_onoff_Resp;     Event::Handle<float> tt_jet2_pt_onoff_Resp;
  Event::Handle<int> tt_nvertices;
  Event::Handle<float> tt_pv0Z;     Event::Handle<float> tt_pv0X; Event::Handle<float> tt_pv0Y;
  Event::Handle<float> tt_probejet_eta;   Event::Handle<float> tt_probejet_phi;   Event::Handle<float> tt_probejet_pt;  Event::Handle<float> tt_probejet_ptRaw;
  Event::Handle<float> tt_barreljet_eta;  Event::Handle<float> tt_barreljet_phi;  Event::Handle<float> tt_barreljet_pt; Event::Handle<float> tt_barreljet_ptRaw;
  Event::Handle<float> tt_pt_ave;
  Event::Handle<float> tt_alpha;
  Event::Handle<float> tt_rel_r; Event::Handle<float> tt_mpf_r;
  Event::Handle<float> tt_asymmetry;
  Event::Handle<float> tt_B;
  Event::Handle<float> tt_MET;
  Event::Handle<float> tt_DeltaMET; //MET before - after TypeI correection
  Event::Handle<float> tt_Bgen;
  Event::Handle<float> tt_genMET;
  Event::Handle<float> tt_nPU;
  Event::Handle<int> tt_matchJetId_0;
  Event::Handle<int> tt_matchJetId_1;
  Event::Handle<float> tt_ev_weight;
  Event::Handle<float> tt_jets_pt;//sum of jets pT
  Event::Handle<int> tt_jet_n;//number of jets
  Event::Handle<float> tt_rho;//event energy density
  // Event::Handle<int> tt_nGoodvertices;
  Event::Handle<int> tt_partonFlavor; //only MC
  Event::Handle<int> tt_flavorBarreljet, tt_flavorProbejet, tt_flavorLeadingjet, tt_flavorSubleadingjet, tt_flavor3rdjet, tt_response3rdjet; //only MC
  Event::Handle<float> tt_response_leadingjet; Event::Handle<float> tt_response_Subleadingjet;
  Event::Handle<float> tt_had_n_Efrac, tt_had_ch_Efrac, tt_mu_Efrac, tt_ph_Efrac;
  Event::Handle<float> tt_inst_lumi, tt_integrated_lumi_in_bin, tt_integrated_lumi;
  Event::Handle<int> tt_lumibin;
  Event::Handle<int> tt_Nmuon; Event::Handle<float> tt_muon_pt;
  Event::Handle<int> tt_Nele; Event::Handle<float> tt_ele_pt;
  Event::Handle<float> tt_dR_jet3_barreljet;  Event::Handle<float> tt_dR_jet3_probejet;  Event::Handle<float> tt_dR_jet3_RECO_GEN;
  Event::Handle<float> tt_dR_GenJet_GenParticle_barrel_matched;   Event::Handle<float> tt_dR_GenJet_GenParticle_probe_matched;
  Event::Handle<int> tt_trigger40;
  Event::Handle<int> tt_trigger60;
  Event::Handle<int> tt_trigger80;
  Event::Handle<int> tt_trigger140;
  Event::Handle<int> tt_trigger200;
  Event::Handle<int> tt_trigger260;
  Event::Handle<int> tt_trigger320;
  Event::Handle<int> tt_trigger400;
  Event::Handle<int> tt_trigger450;
  Event::Handle<int> tt_trigger500;

  Event::Handle<int> tt_trigger40_HFJEC;
  Event::Handle<int> tt_trigger60_HFJEC;
  Event::Handle<int> tt_trigger80_HFJEC;
  Event::Handle<int> tt_trigger100_HFJEC;
  Event::Handle<int> tt_trigger140_HFJEC;
  Event::Handle<int> tt_trigger160_HFJEC;
  Event::Handle<int> tt_trigger200_HFJEC;
  Event::Handle<int> tt_trigger220_HFJEC;
  Event::Handle<int> tt_trigger260_HFJEC;
  Event::Handle<int> tt_trigger300_HFJEC;
  Event::Handle<int> tt_trigger320_HFJEC;
  Event::Handle<int> tt_trigger400_HFJEC;

  // MC
  Event::Handle<float> tt_gen_pthat;    Event::Handle<float> tt_gen_weight;   Event::Handle<float> tt_gen_PUpthat;
  Event::Handle<float> tt_genjet1_pt;   Event::Handle<float> tt_genjet2_pt;   Event::Handle<float> tt_genjet3_pt;
  Event::Handle<float> tt_genjet4_pt;   Event::Handle<float> tt_genjet5_pt;   Event::Handle<float> tt_genjet6_pt;   Event::Handle<float> tt_genjet7_pt;
  Event::Handle<float> tt_genjet1_eta;  Event::Handle<float> tt_genjet2_eta;  Event::Handle<float> tt_genjet3_eta;
  Event::Handle<float> tt_genjet1_phi;  Event::Handle<float> tt_genjet2_phi;  Event::Handle<float> tt_genjet3_phi;
  Event::Handle<float> tt_probegenjet_eta;  Event::Handle<float> tt_probegenjet_phi;  Event::Handle<float> tt_probegenjet_pt;   Event::Handle<float> tt_probegenjet_ptRaw;
  Event::Handle<float> tt_barrelgenjet_eta; Event::Handle<float> tt_barrelgenjet_phi; Event::Handle<float> tt_barrelgenjet_pt;  Event::Handle<float> tt_barrelgenjet_ptRaw;
  Event::Handle<float> tt_gen_alpha;  Event::Handle<float> tt_gen_pt_ave; Event::Handle<float> tt_gen_asymmetry;
  Event::Handle<int> tt_genjet_n;
  Event::Handle<int> tt_no_mc_spikes;
  Event::Handle<bool> tt_JER_SM; //event should be used for StandardMethod JER SFs
  //Different energy fractions in jets
  Event::Handle<float> tt_probejet_neutEmEF;  Event::Handle<float> tt_probejet_neutHadEF;   Event::Handle<float> tt_probejet_chEmEF;  Event::Handle<float> tt_probejet_chHadEF;   Event::Handle<float> tt_probejet_photonEF;  Event::Handle<float> tt_probejet_muonEF;
  Event::Handle<float> tt_barreljet_neutEmEF; Event::Handle<float> tt_barreljet_neutHadEF;  Event::Handle<float> tt_barreljet_chEmEF; Event::Handle<float> tt_barreljet_chHadEF;  Event::Handle<float> tt_barreljet_photonEF; Event::Handle<float> tt_barreljet_muonEF;
  Event::Handle<float> tt_probejet_ptptcl;  Event::Handle<int> tt_probejet_statusptcl;
  Event::Handle<float> tt_barreljet_ptptcl; Event::Handle<int> tt_barreljet_statusptcl;
  Event::Handle<float> tt_jet3_ptptcl;      Event::Handle<int> tt_jet3_statusptcl;
  Event::Handle<int> tt_jet1_genID;     Event::Handle<int> tt_jet2_genID;     Event::Handle<int> tt_jet3_genID;
  Event::Handle<int> tt_jet4_genID;     Event::Handle<int> tt_jet5_genID;     Event::Handle<int> tt_jet6_genID;
  Event::Handle<float> tt_probejet_dRminParton;   Event::Handle<float> tt_barreljet_dRminParton;
  Event::Handle<int> tt_genptcl_n;//number of partons with pt>0
  Event::Handle<float> tt_responseBarreljet, tt_responseProbejet; //only MC

  Event::Handle<int> tt_run;
  Event::Handle<long long> tt_evID;
  Event::Handle<int> tt_lumiSec;

  Event::Handle<int> tt_jet1_l1bx;
  Event::Handle<int> tt_jet2_l1bx;
  Event::Handle<int> tt_jet3_l1bx;


  std::unique_ptr<JECAnalysisRecoGenMatchedHistsFractions> h_matched_all;//uses PF and GEN fractions only
  std::unique_ptr<JECAnalysisRecoGenMatchedHistsFractions> h_matched_pt[n_pt-1];//uses PF and GEN fractions only

  std::unique_ptr<JECAnalysisPUjetsHists> h_goodRECO_all;
  std::unique_ptr<JECAnalysisPUjetsHists> h_badRECO_all;
  std::unique_ptr<JECAnalysisPUjetsHists> h_goodRECO_3rd_all;
  std::unique_ptr<JECAnalysisPUjetsHists> h_badRECO_3rd_all;

  std::unique_ptr<JECAnalysisPUjetsHists> h_goodpartonRECO_all;
  std::unique_ptr<JECAnalysisPUjetsHists> h_badpartonRECO_all;
  std::unique_ptr<JECAnalysisPUjetsHists> h_goodpartonRECO_3rd_all;
  std::unique_ptr<JECAnalysisPUjetsHists> h_badpartonRECO_3rd_all;

  std::unique_ptr<JECAnalysisPUjetsHists> h_dataRECO_all;
  std::unique_ptr<JECAnalysisPUjetsHists> h_dataRECO_3rd_all;

  std::unique_ptr<JECAnalysisPUjetsHists> h_goodRECO_pt[n_pt-1];
  std::unique_ptr<JECAnalysisPUjetsHists> h_badRECO_pt[n_pt-1];
  std::unique_ptr<JECAnalysisPUjetsHists> h_goodpartonRECO_pt[n_pt-1];
  std::unique_ptr<JECAnalysisPUjetsHists> h_badpartonRECO_pt[n_pt-1];

  std::unique_ptr<JECAnalysisPUjetsHists> h_goodRECO_3rd_pt[n_pt-1];
  std::unique_ptr<JECAnalysisPUjetsHists> h_badRECO_3rd_pt[n_pt-1];
  std::unique_ptr<JECAnalysisPUjetsHists> h_goodpartonRECO_3rd_pt[n_pt-1];
  std::unique_ptr<JECAnalysisPUjetsHists> h_badpartonRECO_3rd_pt[n_pt-1];

  std::unique_ptr<JECAnalysisPUjetsHists> h_dataRECO_pt[n_pt-1];
  std::unique_ptr<JECAnalysisPUjetsHists> h_dataRECO_3rd_pt[n_pt-1];

  std::unique_ptr<JECAnalysisHists> h_nocuts, h_sel, h_dijet, h_match, h_final;
  std::unique_ptr<JECAnalysisHists> h_trg40, h_trg60, h_trg80, h_trg140, h_trg200,h_trg260,h_trg320,h_trg400, h_trg450,h_trg500;
  std::unique_ptr<JECAnalysisHists> h_trgHF40, h_trgHF60, h_trgHF80, h_trgHF100, h_trgHF140, h_trgHF160, h_trgHF200, h_trgHF220, h_trgHF260, h_trgHF300, h_trgHF320, h_trgHF400;
  std::unique_ptr<LuminosityHists> h_lumi_nocuts, h_lumi_sel, h_lumi_dijet, h_lumi_match, h_lumi_final;
  std::unique_ptr<LuminosityHists> h_lumi_Trig40, h_lumi_Trig60, h_lumi_Trig80, h_lumi_Trig140, h_lumi_Trig200, h_lumi_Trig260, h_lumi_Trig320, h_lumi_Trig400, h_lumi_Trig450, h_lumi_Trig500;
  std::unique_ptr<LuminosityHists> h_lumi_TrigHF40, h_lumi_TrigHF60, h_lumi_TrigHF80, h_lumi_TrigHF100, h_lumi_TrigHF140, h_lumi_TrigHF160, h_lumi_TrigHF200, h_lumi_TrigHF220, h_lumi_TrigHF260, h_lumi_TrigHF300, h_lumi_TrigHF320, h_lumi_TrigHF400;
  std::unique_ptr<LumiHists> h_monitoring_final;
  std::unique_ptr<JECRunnumberHists> h_runnr_input;
  std::unique_ptr<JECCrossCheckHists> h_input,h_lumisel, h_beforeCleaner,h_afterCleaner,h_2jets,h_beforeJEC,h_afterJEC,h_afterJER,h_afterMET,h_beforeTriggerData,h_afterTriggerData,h_beforeFlatFwd,h_afterFlatFwd,h_afterPtEtaReweight,h_afterLumiReweight,h_afterUnflat,h_afternVts;
  std::unique_ptr<uhh2DiJetJERC::Selection> sel;

  //useful booleans
  bool debug, no_genp;
  bool isMC, JECClosureTest, JERClosureTest, apply_EtaPhi_cut, apply_EtaPhi_HCAL, trigger_central, trigger_fwd, DO_Pu_ReWeighting, apply_lumiweights, apply_L1seed_from_bx1_filter, apply_PUid;
  bool is2016v2, is2016v3, is2017, is2018;
  std::string year;
  bool isAK8, ispuppi;
  string PtBinsTrigger;
  bool ispythia8;
  bool ts;
  string SysType_PU;
  TString dataset_version, JEC_Version, jetLabel;
  JetId Jet_PFID;
  int n_evt;
  bool isThreshold;

  std::map<run_lumi, double> rl2lumi;
  std::map<run_lumi, double> rl2intlumi;
  TBranch * brun ;
  TBranch * blumiblock;
  TBranch * bilumi;
  double integrated_lumi;
  vector<run_lumi> upper_binborders_runnrs;
  vector<double> lumi_in_bins;

  double L1METptThresh;
  double minJetPt, minGenJetPt;
  double qScale_max;
  double eta_thresh_low;
  double eta_thresh_high;

  std::vector<uhh2DiJetJERC::run_lumi_ev>  unprefirableSummary;
  bool useUnprefireable;

  uhh2::GenericEvent::Handle<std::vector<L1Jet>> handle_l1jet_seeds;

};


void AnalysisModule_DiJetTrg::init_JEC(uhh2::Context& ctx){
  std::vector<std::string> JEC_corr,       JEC_corr_A,       JEC_corr_B,       JEC_corr_C,       JEC_corr_D;
  std::vector<std::string> JEC_corr_L1RC,  JEC_corr_A_L1RC,  JEC_corr_B_L1RC,  JEC_corr_C_L1RC,  JEC_corr_D_L1RC;

  #define MAKE_JEC(jecv,jetLabel)                                   \
  if(JEC_Version == #jecv){                                         \
    JEC_corr_A      = JERFiles::jecv##_A_L123_##jetLabel##_DATA;    \
    JEC_corr_B      = JERFiles::jecv##_B_L123_##jetLabel##_DATA;    \
    JEC_corr_C      = JERFiles::jecv##_C_L123_##jetLabel##_DATA;    \
    JEC_corr_D      = JERFiles::jecv##_D_L123_##jetLabel##_DATA;    \
    if (!ispuppi) {                                                 \
      JEC_corr_A_L1RC = JERFiles::jecv##_A_L1RC_##jetLabel##_DATA;  \
      JEC_corr_B_L1RC = JERFiles::jecv##_B_L1RC_##jetLabel##_DATA;  \
      JEC_corr_C_L1RC = JERFiles::jecv##_C_L1RC_##jetLabel##_DATA;  \
      JEC_corr_D_L1RC = JERFiles::jecv##_D_L1RC_##jetLabel##_DATA;  \
    }                                                               \
  }                                                                 \

  #define MAKE_JEC_noRes(jecv,jetLabel)                                 \
  if(JEC_Version == #jecv){                                             \
    JEC_corr_A      = JERFiles::jecv##_A_L123_noRes_##jetLabel##_DATA;  \
    JEC_corr_B      = JERFiles::jecv##_B_L123_noRes_##jetLabel##_DATA;  \
    JEC_corr_C      = JERFiles::jecv##_C_L123_noRes_##jetLabel##_DATA;  \
    JEC_corr_D      = JERFiles::jecv##_D_L123_noRes_##jetLabel##_DATA;  \
    if (!ispuppi) {                                                     \
      JEC_corr_A_L1RC = JERFiles::jecv##_A_L1RC_##jetLabel##_DATA;      \
      JEC_corr_B_L1RC = JERFiles::jecv##_B_L1RC_##jetLabel##_DATA;      \
      JEC_corr_C_L1RC = JERFiles::jecv##_C_L1RC_##jetLabel##_DATA;      \
      JEC_corr_D_L1RC = JERFiles::jecv##_D_L1RC_##jetLabel##_DATA;      \
    }                                                                   \
  }

  #define MAKE_JEC_MC(jecv,jetLabel)                          \
  if(JEC_Version == #jecv){                                   \
    JEC_corr      = JERFiles::jecv##_L123_##jetLabel##_MC;    \
    if (!ispuppi) {                                           \
      JEC_corr_L1RC = JERFiles::jecv##_L1RC_##jetLabel##_MC;  \
    }                                                       \
  }                                                           \


  if(isMC){ //L123 for MC
    if (jetLabel == "AK4CHS") {
      MAKE_JEC_MC(Autumn18_V8, AK4PFchs)
      // else MAKE_JEC_MC(Autumn18_V17, AK4PFchs)
      else MAKE_JEC_MC(Autumn18_V19, AK4PFchs)
      else throw runtime_error("In AnalysisModule_DiJetTrg.cxx: Invalid JEC_Version for deriving residuals on AK4CHS, MC specified ("+JEC_Version+") ");
    } else if (jetLabel == "AK4Puppi") {
      // MAKE_JEC_MC(Autumn18_V17, AK4PFPuppi)
      MAKE_JEC_MC(Autumn18_V19, AK4PFPuppi)
      else throw runtime_error("In AnalysisModule_DiJetTrg.cxx: Invalid JEC_Version for deriving residuals on AK4Puppi, MC specified ("+JEC_Version+") ");
    } else if (jetLabel == "AK8Puppi") {
      MAKE_JEC_MC(Autumn18_V19, AK8PFPuppi)
      else throw runtime_error("In AnalysisModule_DiJetTrg.cxx: Invalid JEC_Version for deriving residuals on AK8Puppi, MC specified ("+JEC_Version+") ");
    } else throw runtime_error("In AnalysisModule_DiJetTrg.cxx: Invalid jetLabel: "+jetLabel+" ");
  } else { //L123 + L2L3Res for Data;
    if (jetLabel == "AK4CHS") {
      if(JECClosureTest){
        std::cout << "JECClosureTest applied" << '\n';
        MAKE_JEC(Autumn18_V8, AK4PFchs)
        else MAKE_JEC(Autumn18_V19, AK4PFchs)
        else throw runtime_error("In AnalysisModule_DiJetTrg.cxx: Invalid JEC_Version for deriving residuals on AK4CHS "+JEC_Version+", DATA specified.");
      } else {
        std::cout << "MAKE_JEC_noRes applied" << '\n';
        MAKE_JEC_noRes(Autumn18_V19, AK4PFchs)
        else throw runtime_error("In AnalysisModule_DiJetTrg.cxx: Invalid JEC_Version for deriving residuals on AK4CHS "+JEC_Version+", DATA specified.");
      }
    } else if (jetLabel == "AK4Puppi") {
      if(JECClosureTest){
        std::cout << "JECClosureTest applied" << '\n';
        MAKE_JEC(Autumn18_V19, AK4PFPuppi)
        else throw runtime_error("In AnalysisModule_DiJetTrg.cxx: Invalid JEC_Version for deriving residuals on AK4Puppi "+JEC_Version+", DATA specified.");
      } else {
        std::cout << "MAKE_JEC_noRes applied" << '\n';
        MAKE_JEC_noRes(Autumn18_V19, AK4PFPuppi)
        else throw runtime_error("In AnalysisModule_DiJetTrg.cxx: Invalid JEC_Version for deriving residuals on AK4Puppi "+JEC_Version+", DATA specified.");
      }
    } else if (jetLabel == "AK8Puppi") {
      if(JECClosureTest){
        std::cout << "JECClosureTest applied" << '\n';
        MAKE_JEC(Autumn18_V19, AK8PFPuppi)
        else throw runtime_error("In AnalysisModule_DiJetTrg.cxx: Invalid JEC_Version for deriving residuals on AK8Puppi "+JEC_Version+", DATA specified.");
      } else {
        std::cout << "MAKE_JEC_noRes applied" << '\n';
        MAKE_JEC_noRes(Autumn18_V19, AK8PFPuppi)
        else throw runtime_error("In AnalysisModule_DiJetTrg.cxx: Invalid JEC_Version for deriving residuals on AK8Puppi "+JEC_Version+", DATA specified.");
      }
    } else throw runtime_error("In AnalysisModule_DiJetTrg.cxx: Invalid jetLabel: "+jetLabel+" ");
  }


  //DATA
  if(!isMC){
    jet_corrector_A.reset(new JetCorrector(ctx, JEC_corr_A, JEC_corr_A_L1RC));
    jet_corrector_B.reset(new JetCorrector(ctx, JEC_corr_B, JEC_corr_B_L1RC));
    jet_corrector_C.reset(new JetCorrector(ctx, JEC_corr_C, JEC_corr_C_L1RC));
    jet_corrector_D.reset(new JetCorrector(ctx, JEC_corr_D, JEC_corr_D_L1RC));
  }
  else if(isMC){//MC : only one version of JECs exists
    jet_corrector_MC.reset(new JetCorrector(ctx, JEC_corr, JEC_corr_L1RC));
  }

}

void AnalysisModule_DiJetTrg::declare_output(uhh2::Context& ctx){
  //Store only vars needed for the dijet analysis
  tt_JER_SM = ctx.declare_event_output<bool>("is_JER_SM");
  tt_gen_pthat = ctx.declare_event_output<float>("gen_pthat");
  tt_gen_PUpthat = ctx.declare_event_output<float>("gen_PUpthat");
  tt_gen_weight = ctx.declare_event_output<float>("gen_weight");
  tt_jet1_pt = ctx.declare_event_output<float>("jet1_pt");
  tt_jet2_pt = ctx.declare_event_output<float>("jet2_pt");
  tt_jet3_pt = ctx.declare_event_output<float>("jet3_pt");
  tt_jet4_pt = ctx.declare_event_output<float>("jet4_pt");
  tt_jet5_pt = ctx.declare_event_output<float>("jet5_pt");
  tt_jet6_pt = ctx.declare_event_output<float>("jet6_pt");
  tt_jet7_pt = ctx.declare_event_output<float>("jet7_pt");
  tt_jet3_eta = ctx.declare_event_output<float>("jet3_eta");
  tt_jet3_phi = ctx.declare_event_output<float>("jet3_phi");
  tt_jet1_ptRaw = ctx.declare_event_output<float>("jet1_ptRaw");
  tt_jet2_ptRaw = ctx.declare_event_output<float>("jet2_ptRaw");
  tt_jet3_ptRaw = ctx.declare_event_output<float>("jet3_ptRaw");
  tt_genjet1_pt = ctx.declare_event_output<float>("genjet1_pt");
  tt_genjet2_pt = ctx.declare_event_output<float>("genjet2_pt");
  tt_genjet3_pt = ctx.declare_event_output<float>("genjet3_pt");
  tt_genjet4_pt = ctx.declare_event_output<float>("genjet4_pt");
  tt_genjet5_pt = ctx.declare_event_output<float>("genjet5_pt");
  tt_genjet6_pt = ctx.declare_event_output<float>("genjet6_pt");
  tt_genjet7_pt = ctx.declare_event_output<float>("genjet7_pt");
  tt_genjet1_eta = ctx.declare_event_output<float>("genjet1_eta");
  tt_genjet2_eta = ctx.declare_event_output<float>("genjet2_eta");
  tt_genjet3_eta = ctx.declare_event_output<float>("genjet3_eta");
  tt_genjet1_phi = ctx.declare_event_output<float>("genjet1_phi");
  tt_genjet2_phi = ctx.declare_event_output<float>("genjet2_phi");
  tt_genjet3_phi = ctx.declare_event_output<float>("genjet3_phi");

  tt_jet1_pt_onoff_Resp = ctx.declare_event_output<float>("jet1_pt_onoff_Resp");
  tt_jet2_pt_onoff_Resp = ctx.declare_event_output<float>("jet2_pt_onoff_Resp");

  tt_probejet_neutEmEF  = ctx.declare_event_output<float>("probejet_neutEmEF");
  tt_probejet_neutHadEF = ctx.declare_event_output<float>("probejet_neutHadEF");
  tt_probejet_chEmEF    = ctx.declare_event_output<float>("probejet_chEmEF");
  tt_probejet_chHadEF   = ctx.declare_event_output<float>("probejet_chHadEF");
  tt_probejet_photonEF  = ctx.declare_event_output<float>("probejet_photonEF");
  tt_probejet_muonEF    = ctx.declare_event_output<float>("probejet_muonEF");

  tt_barreljet_neutEmEF  = ctx.declare_event_output<float>("barreljet_neutEmEF");
  tt_barreljet_neutHadEF = ctx.declare_event_output<float>("barreljet_neutHadEF");
  tt_barreljet_chEmEF    = ctx.declare_event_output<float>("barreljet_chEmEF");
  tt_barreljet_chHadEF   = ctx.declare_event_output<float>("barreljet_chHadEF");
  tt_barreljet_photonEF  = ctx.declare_event_output<float>("barreljet_photonEF");
  tt_barreljet_muonEF    = ctx.declare_event_output<float>("barreljet_muonEF");

  tt_nvertices = ctx.declare_event_output<int>("nvertices");
  tt_pv0Z = ctx.declare_event_output<float>("pv0Z");
  tt_pv0Y = ctx.declare_event_output<float>("pv0Y");
  tt_pv0X = ctx.declare_event_output<float>("pv0X");
  // tt_nGoodvertices = ctx.declare_event_output<int>("nGoodvertices");
  tt_barreljet_eta          = ctx.declare_event_output<float>("barreljet_eta");
  tt_barreljet_phi          = ctx.declare_event_output<float>("barreljet_phi");
  tt_barreljet_pt           = ctx.declare_event_output<float>("barreljet_pt");
  tt_barreljet_ptRaw        = ctx.declare_event_output<float>("barreljet_ptRaw");
  tt_barreljet_ptptcl       = ctx.declare_event_output<float>("barreljet_ptptcl");
  tt_barreljet_statusptcl   = ctx.declare_event_output<int>("barreljet_status_ptcl");
  tt_probejet_eta           = ctx.declare_event_output<float>("probejet_eta");
  tt_probejet_phi           = ctx.declare_event_output<float>("probejet_phi");
  tt_probejet_pt            = ctx.declare_event_output<float>("probejet_pt");
  tt_probejet_ptRaw         = ctx.declare_event_output<float>("probejet_ptRaw");
  tt_probejet_ptptcl        = ctx.declare_event_output<float>("probejet_ptptcl");
  tt_probejet_statusptcl    = ctx.declare_event_output<int>("probejet_status_ptcl");
  tt_probejet_statusptcl    = ctx.declare_event_output<int>("probejet_status_ptcl");
  tt_jet3_ptptcl            = ctx.declare_event_output<float>("jet3_ptptcl");
  tt_jet3_statusptcl        = ctx.declare_event_output<int>("jet3_status_ptcl");
  tt_probejet_dRminParton   = ctx.declare_event_output<float>("probejet_dRminParton");
  tt_barreljet_dRminParton  = ctx.declare_event_output<float>("barreljet_dRminParton");
  tt_pt_ave = ctx.declare_event_output<float>("pt_ave");
  tt_alpha  = ctx.declare_event_output<float>("alpha");
  tt_rel_r  = ctx.declare_event_output<float>("rel_r");
  tt_mpf_r  = ctx.declare_event_output<float>("mpf_r");
  tt_asymmetry = ctx.declare_event_output<float>("asymmetry");
  tt_B = ctx.declare_event_output<float>("B");
  tt_MET = ctx.declare_event_output<float>("MET");
  tt_DeltaMET = ctx.declare_event_output<float>("dMET");
  tt_Bgen = ctx.declare_event_output<float>("genB");
  tt_genMET = ctx.declare_event_output<float>("genMET");
  tt_nPU = ctx.declare_event_output<float>("nPU");
  tt_matchJetId_0 = ctx.declare_event_output<int>("matchJetId_0");
  tt_matchJetId_1 = ctx.declare_event_output<int>("matchJetId_1");
  tt_ev_weight = ctx.declare_event_output<float>("weight");
  tt_jets_pt= ctx.declare_event_output<float>("sum_jets_pt");
  tt_jet_n= ctx.declare_event_output<int>("Njet");
  tt_rho = ctx.declare_event_output<float>("rho");
  tt_partonFlavor = ctx.declare_event_output<int>("partonFlavor");
  tt_flavorBarreljet        = ctx.declare_event_output<int>("flavorBarreljet");
  tt_responseBarreljet      = ctx.declare_event_output<float>("responseBarreljet_genp");
  tt_flavorProbejet         = ctx.declare_event_output<int>("flavorProbejet");
  tt_responseProbejet       = ctx.declare_event_output<float>("responseProbejet_genp");
  tt_flavor3rdjet           = ctx.declare_event_output<int>("flavor3rdjet");
  tt_response3rdjet         = ctx.declare_event_output<int>("response3rdjet");
  tt_flavorLeadingjet       = ctx.declare_event_output<int>("flavorLeadingjet");
  tt_flavorSubleadingjet    = ctx.declare_event_output<int>("flavorSubleadingjet");
  tt_response_leadingjet    = ctx.declare_event_output<float>("leadingjet_response");
  tt_response_Subleadingjet = ctx.declare_event_output<float>("subleadingjet_response");
  tt_inst_lumi = ctx.declare_event_output<float>("instantaneous_lumi");
  tt_integrated_lumi_in_bin = ctx.declare_event_output<float>("integrated_lumi_in_bin");
  tt_lumibin = ctx.declare_event_output<int>("lumibin");
  tt_Nmuon    = ctx.declare_event_output<int>("Nmuon");
  tt_muon_pt  = ctx.declare_event_output<float>("muon_pt");
  tt_ele_pt   = ctx.declare_event_output<float>("electron_pt");
  tt_Nele     = ctx.declare_event_output<int>("Nelectron");
  tt_integrated_lumi = ctx.declare_event_output<float>("integrated_lumi");

  tt_run        = ctx.declare_event_output<int>("run");
  tt_evID       = ctx.declare_event_output<long long>("eventID");
  tt_lumiSec    = ctx.declare_event_output<int>("lumi_sec");
  tt_jet1_l1bx  = ctx.declare_event_output<int>("jet1_l1bx");
  tt_jet2_l1bx  = ctx.declare_event_output<int>("jet1_l2bx");
  tt_jet3_l1bx  = ctx.declare_event_output<int>("jet1_l3bx");

  tt_trigger40  = ctx.declare_event_output<int>("trigger40");
  tt_trigger60  = ctx.declare_event_output<int>("trigger60");
  tt_trigger80  = ctx.declare_event_output<int>("trigger80");
  tt_trigger140 = ctx.declare_event_output<int>("trigger140");
  tt_trigger200 = ctx.declare_event_output<int>("trigger200");
  tt_trigger260 = ctx.declare_event_output<int>("trigger260");
  tt_trigger320 = ctx.declare_event_output<int>("trigger320");
  tt_trigger400 = ctx.declare_event_output<int>("trigger400");
  tt_trigger450 = ctx.declare_event_output<int>("trigger450");
  tt_trigger500 = ctx.declare_event_output<int>("trigger500");

  tt_trigger40_HFJEC  = ctx.declare_event_output<int>("trigger40_HFJEC");
  tt_trigger60_HFJEC  = ctx.declare_event_output<int>("trigger60_HFJEC");
  tt_trigger80_HFJEC  = ctx.declare_event_output<int>("trigger80_HFJEC");
  tt_trigger100_HFJEC = ctx.declare_event_output<int>("trigger100_HFJEC");
  tt_trigger140_HFJEC = ctx.declare_event_output<int>("trigger140_HFJEC");
  tt_trigger160_HFJEC = ctx.declare_event_output<int>("trigger160_HFJEC");
  tt_trigger200_HFJEC = ctx.declare_event_output<int>("trigger200_HFJEC");
  tt_trigger220_HFJEC = ctx.declare_event_output<int>("trigger220_HFJEC");
  tt_trigger260_HFJEC = ctx.declare_event_output<int>("trigger260_HFJEC");
  tt_trigger300_HFJEC = ctx.declare_event_output<int>("trigger300_HFJEC");
  tt_trigger320_HFJEC = ctx.declare_event_output<int>("trigger320_HFJEC");
  tt_trigger400_HFJEC = ctx.declare_event_output<int>("trigger400_HFJEC");

  // MC:
  tt_barrelgenjet_pt  = ctx.declare_event_output<float>("barrelgenjet_pt");
  tt_barrelgenjet_eta = ctx.declare_event_output<float>("barrelgenjet_eta");
  tt_barrelgenjet_phi = ctx.declare_event_output<float>("barrelgenjet_phi");
  tt_probegenjet_pt   = ctx.declare_event_output<float>("probegenjet_pt");
  tt_probegenjet_eta  = ctx.declare_event_output<float>("probegenjet_eta");
  tt_probegenjet_phi  = ctx.declare_event_output<float>("probegenjet_phi");
  tt_gen_pt_ave       = ctx.declare_event_output<float>("gen_pt_ave");
  tt_gen_alpha        = ctx.declare_event_output<float>("gen_alpha");
  tt_gen_asymmetry    = ctx.declare_event_output<float>("gen_asymmetry");
  tt_genjet_n         = ctx.declare_event_output<int>("Ngenjet");
  tt_no_mc_spikes     = ctx.declare_event_output<int>("no_mc_spikes");
  tt_genptcl_n        = ctx.declare_event_output<int>("Nptcl");

  tt_jet1_genID =  ctx.declare_event_output<int>("jet1_genID");
  tt_jet2_genID =  ctx.declare_event_output<int>("jet2_genID");
  tt_jet3_genID =  ctx.declare_event_output<int>("jet3_genID");
  tt_jet4_genID =  ctx.declare_event_output<int>("jet4_genID");
  tt_jet5_genID =  ctx.declare_event_output<int>("jet5_genID");
  tt_jet6_genID =  ctx.declare_event_output<int>("jet6_genID");

  tt_dR_jet3_barreljet = ctx.declare_event_output<float>("dR_jet3_barreljet");
  tt_dR_jet3_probejet = ctx.declare_event_output<float>("dR_jet3_probejet");
  tt_dR_jet3_RECO_GEN = ctx.declare_event_output<float>("dR_jet3_RECO_GEN");
  tt_dR_GenJet_GenParticle_barrel_matched = ctx.declare_event_output<float>("dR_GenJet_GenParticle_barrel_matched");
  tt_dR_GenJet_GenParticle_probe_matched = ctx.declare_event_output<float>("dR_GenJet_GenParticle_probe_matched");
}

void AnalysisModule_DiJetTrg::init_hists(uhh2::Context& ctx){
  h_runnr_input.reset(new JECRunnumberHists(ctx,"Runnr_input"));
  h_input.reset(new JECCrossCheckHists(ctx,"CrossCheck_input"));
  h_lumisel.reset(new JECCrossCheckHists(ctx,"CrossCheck_lumisel"));
  h_beforeCleaner.reset(new JECCrossCheckHists(ctx,"CrossCheck_beforeCleaner"));
  h_afterCleaner.reset(new JECCrossCheckHists(ctx,"CrossCheck_afterCleaner"));
  h_2jets.reset(new JECCrossCheckHists(ctx,"CrossCheck_2jets"));
  h_beforeJEC.reset(new JECCrossCheckHists(ctx,"CrossCheck_beforeJEC"));
  h_afterJEC.reset(new JECCrossCheckHists(ctx,"CrossCheck_afterJEC"));
  h_afterJER.reset(new JECCrossCheckHists(ctx,"CrossCheck_afterJER"));
  h_afterMET.reset(new JECCrossCheckHists(ctx,"CrossCheck_afterMET"));
  h_beforeTriggerData.reset(new JECCrossCheckHists(ctx,"CrossCheck_beforeTriggerData"));
  h_afterTriggerData.reset(new JECCrossCheckHists(ctx,"CrossCheck_afterTriggerData"));
  h_beforeFlatFwd.reset(new JECCrossCheckHists(ctx,"CrossCheck_beforeFlatFwd"));
  h_afterFlatFwd.reset(new JECCrossCheckHists(ctx,"CrossCheck_afterFlatFwd"));
  h_afterPtEtaReweight.reset(new JECCrossCheckHists(ctx,"CrossCheck_afterPtEtaReweight"));
  h_afterLumiReweight.reset(new JECCrossCheckHists(ctx,"CrossCheck_afterLumiReweight"));
  h_afterUnflat.reset(new JECCrossCheckHists(ctx,"CrossCheck_afterUnflat"));
  h_afternVts.reset(new JECCrossCheckHists(ctx,"CrossCheck_afternVts"));
  h_matched_all.reset(new JECAnalysisRecoGenMatchedHistsFractions(ctx,"MatchedRecoGen_all"));
  for(int i=0;i<n_pt-1;i++){
    TString ptname = pt_range[i]; ptname +="_"; ptname +=pt_range[i+1];
    TString histname = "MatchedRecoGen_"; histname+=ptname;
    cout<<histname.Data()<<endl;
    h_matched_pt[i].reset(new JECAnalysisRecoGenMatchedHistsFractions(ctx,histname.Data()));
  }

  h_nocuts.reset(new JECAnalysisHists(ctx,"NoCuts"));
  h_dijet.reset(new JECAnalysisHists(ctx,"diJet"));
  h_match.reset(new JECAnalysisHists(ctx,"JetMatching"));
  h_sel.reset(new JECAnalysisHists(ctx,"Selection"));
  h_final.reset(new JECAnalysisHists(ctx,"Final"));

  h_trg40.reset(new JECAnalysisHists(ctx,"h_trg40"));
  h_trg60.reset(new JECAnalysisHists(ctx,"h_trg60"));
  h_trg80.reset(new JECAnalysisHists(ctx,"h_trg80"));
  h_trg140.reset(new JECAnalysisHists(ctx,"h_trg140"));
  h_trg200.reset(new JECAnalysisHists(ctx,"h_trg200"));
  h_trg260.reset(new JECAnalysisHists(ctx,"h_trg260"));
  h_trg320.reset(new JECAnalysisHists(ctx,"h_trg320"));
  h_trg400.reset(new JECAnalysisHists(ctx,"h_trg400"));
  h_trg450.reset(new JECAnalysisHists(ctx,"h_trg450"));
  h_trg500.reset(new JECAnalysisHists(ctx,"h_trg500"));

  h_trgHF40.reset(new JECAnalysisHists(ctx,"trgHF40"));
  h_trgHF60.reset(new JECAnalysisHists(ctx,"trgHF60"));
  h_trgHF80.reset(new JECAnalysisHists(ctx,"trgHF80"));
  h_trgHF100.reset(new JECAnalysisHists(ctx,"trgHF100"));
  h_trgHF140.reset(new JECAnalysisHists(ctx,"trgHF140"));
  h_trgHF160.reset(new JECAnalysisHists(ctx,"trgHF160"));
  h_trgHF200.reset(new JECAnalysisHists(ctx,"trgHF200"));
  h_trgHF220.reset(new JECAnalysisHists(ctx,"trgHF220"));
  h_trgHF260.reset(new JECAnalysisHists(ctx,"trgHF260"));
  h_trgHF300.reset(new JECAnalysisHists(ctx,"trgHF300"));
  h_trgHF320.reset(new JECAnalysisHists(ctx,"trgHF320"));
  h_trgHF400.reset(new JECAnalysisHists(ctx,"trgHF400"));

  h_lumi_nocuts.reset(new LuminosityHists(ctx,"Lumi_noCuts"));
  h_lumi_sel.reset(new LuminosityHists(ctx,"Lumi_Selection"));
  h_lumi_dijet.reset(new LuminosityHists(ctx,"Lumi_diJet"));
  h_lumi_match.reset(new LuminosityHists(ctx,"Lumi_JetMatching"));
  h_lumi_final.reset(new LuminosityHists(ctx,"Lumi_Final"));
  h_lumi_Trig40.reset(new LuminosityHists(ctx,"Lumi_Trig40"));
  h_lumi_Trig60.reset(new LuminosityHists(ctx,"Lumi_Trig60"));
  h_lumi_Trig80.reset(new LuminosityHists(ctx,"Lumi_Trig80"));
  h_lumi_Trig140.reset(new LuminosityHists(ctx,"Lumi_Trig140"));
  h_lumi_Trig200.reset(new LuminosityHists(ctx,"Lumi_Trig200"));
  h_lumi_Trig260.reset(new LuminosityHists(ctx,"Lumi_Trig260"));
  h_lumi_Trig320.reset(new LuminosityHists(ctx,"Lumi_Trig320"));
  h_lumi_Trig400.reset(new LuminosityHists(ctx,"Lumi_Trig400"));
  h_lumi_Trig450.reset(new LuminosityHists(ctx,"Lumi_Trig450"));
  h_lumi_Trig500.reset(new LuminosityHists(ctx,"Lumi_Trig500"));

  h_lumi_TrigHF40.reset(new LuminosityHists(ctx,"lumi_TrigHF40"));
  h_lumi_TrigHF60.reset(new LuminosityHists(ctx,"lumi_TrigHF60"));
  h_lumi_TrigHF80.reset(new LuminosityHists(ctx,"lumi_TrigHF80"));
  h_lumi_TrigHF100.reset(new LuminosityHists(ctx,"lumi_TrigHF100"));
  h_lumi_TrigHF140.reset(new LuminosityHists(ctx,"lumi_TrigHF140"));
  h_lumi_TrigHF160.reset(new LuminosityHists(ctx,"lumi_TrigHF160"));
  h_lumi_TrigHF200.reset(new LuminosityHists(ctx,"lumi_TrigHF200"));
  h_lumi_TrigHF220.reset(new LuminosityHists(ctx,"lumi_TrigHF220"));
  h_lumi_TrigHF260.reset(new LuminosityHists(ctx,"lumi_TrigHF260"));
  h_lumi_TrigHF300.reset(new LuminosityHists(ctx,"lumi_TrigHF300"));
  h_lumi_TrigHF320.reset(new LuminosityHists(ctx,"lumi_TrigHF320"));
  h_lumi_TrigHF400.reset(new LuminosityHists(ctx,"lumi_TrigHF400"));

  h_monitoring_final.reset(new LumiHists(ctx, "Monitoring_Final"));

  h_goodRECO_all.reset(new JECAnalysisPUjetsHists(ctx,"goodRECO_all"));
  h_badRECO_all.reset(new JECAnalysisPUjetsHists(ctx,"badRECO_all"));
  h_goodpartonRECO_all.reset(new JECAnalysisPUjetsHists(ctx,"goodpartonRECO_all"));
  h_badpartonRECO_all.reset(new JECAnalysisPUjetsHists(ctx,"badpartonRECO_all"));

  h_goodRECO_3rd_all.reset(new JECAnalysisPUjetsHists(ctx,"good3rdRECO_all"));
  h_badRECO_3rd_all.reset(new JECAnalysisPUjetsHists(ctx,"bad3rdRECO_all"));
  h_goodpartonRECO_3rd_all.reset(new JECAnalysisPUjetsHists(ctx,"goodparton3rdRECO_all"));
  h_badpartonRECO_3rd_all.reset(new JECAnalysisPUjetsHists(ctx,"badparton3rdRECO_all"));

  h_dataRECO_all.reset(new JECAnalysisPUjetsHists(ctx,"dataRECO_all"));
  h_dataRECO_3rd_all.reset(new JECAnalysisPUjetsHists(ctx,"data3rdRECO_all"));

  for(int i=0;i<n_pt-1;i++){
    TString ptname = pt_range[i]; ptname +="_"; ptname +=pt_range[i+1];
    TString histname = "goodRECO_"; histname+=ptname;
    h_goodRECO_pt[i].reset(new JECAnalysisPUjetsHists(ctx,histname.Data()));
    histname = "badRECO_"; histname+=ptname;
    h_badRECO_pt[i].reset(new JECAnalysisPUjetsHists(ctx,histname.Data()));
    histname = "goodpartonRECO_"; histname+=ptname;
    h_goodpartonRECO_pt[i].reset(new JECAnalysisPUjetsHists(ctx,histname.Data()));
    histname = "badpartonRECO_"; histname+=ptname;
    h_badpartonRECO_pt[i].reset(new JECAnalysisPUjetsHists(ctx,histname.Data()));

    histname = "good3rdRECO_"; histname+=ptname;
    h_goodRECO_3rd_pt[i].reset(new JECAnalysisPUjetsHists(ctx,histname.Data()));
    histname = "bad3rdRECO_"; histname+=ptname;
    h_badRECO_3rd_pt[i].reset(new JECAnalysisPUjetsHists(ctx,histname.Data()));
    histname = "goodparton3rdRECO_"; histname+=ptname;
    h_goodpartonRECO_3rd_pt[i].reset(new JECAnalysisPUjetsHists(ctx,histname.Data()));
    histname = "badparton3rdRECO_"; histname+=ptname;
    h_badpartonRECO_3rd_pt[i].reset(new JECAnalysisPUjetsHists(ctx,histname.Data()));

    histname = "dataRECO_"; histname+=ptname;
    h_dataRECO_pt[i].reset(new JECAnalysisPUjetsHists(ctx,histname.Data()));
    histname = "data3rdRECO_"; histname+=ptname;
    h_dataRECO_3rd_pt[i].reset(new JECAnalysisPUjetsHists(ctx,histname.Data()));

  }

}

AnalysisModule_DiJetTrg::AnalysisModule_DiJetTrg(uhh2::Context & ctx) {

  sel.reset(new uhh2DiJetJERC::Selection(ctx));
  // cout << "start" << endl;
  // for(auto & kv : ctx.get_all()){
  //   cout << " " << kv.first << " = " << kv.second << endl;
  // }

  no_genp = true;
  dataset_version = ctx.get("dataset_version");
  isMC = (ctx.get("dataset_type") == "MC");
  jetLabel = ctx.get("JetLabel");
  isAK8 = (jetLabel == "AK8CHS" || jetLabel == "AK8Puppi");
  ispuppi = (jetLabel == "AK4Puppi" || jetLabel == "AK8Puppi");
  JEC_Version = ctx.get("JEC_Version");
  JECClosureTest = string2bool(ctx.get("JECClosureTest"));
  JERClosureTest = string2bool(ctx.get("JERClosureTest","false"));
  apply_EtaPhi_cut = string2bool(ctx.get("EtaPhi_cut", "true"));
  apply_EtaPhi_HCAL = string2bool(ctx.get("EtaPhi_HCAL", "true"));
  isThreshold = string2bool(ctx.get("isThreshold", "false"));
  apply_PUid = string2bool(ctx.get("Apply_PUid_3rdjet", "false"));
  cout << "Dataset is " << ((isMC) ? " mc " : " data") << endl;

  apply_L1seed_from_bx1_filter =  (ctx.get("Apply_L1Seed_From_BX1_Filter","false") == "true" && !isMC);
  is2016v2 = (ctx.get("dataset_version").find("2016v2") != std::string::npos);
  is2016v3 = (ctx.get("dataset_version").find("2016v3") != std::string::npos);
  is2017 = (ctx.get("dataset_version").find("2017") != std::string::npos);
  is2018 = (ctx.get("dataset_version").find("2018") != std::string::npos);
  year = ctx.get("year");
  PtBinsTrigger = ctx.get("PtBinsTrigger");

  debug = string2bool(ctx.get("Debug","false"));
  // debug= true;

  if (debug) std::cout << "PtBinsTrigger: " << PtBinsTrigger << " isAK8: " << isAK8 << " ispuppi: " << ispuppi << '\n';
  ispythia8 = ctx.get("MCgenerator", "Pythia8") == "Pythia8";
  useUnprefireable = string2bool(ctx.get("UseUnprefirable","false"));

  if(useUnprefireable){
    cout<<"prepare the list of unprefireable events"<<endl;
    string unprefFile="/nfs/dust/cms/user/karavdia/CMSSW_9_4_3/src/UHH2/TriggersExamination/data/UnprefirableEventList_JetHT_Run2017BtoF.root";
    cout<<"  will use the file "<< unprefFile <<endl;

    TFile* file_FilteredEvents = new TFile(unprefFile.c_str(),"READ","unprefirableEventSummary");
    TTree * filtered_tree = dynamic_cast<TTree*>(file_FilteredEvents->Get("tree"));
    // fetch branches we need
    TBranch * brun = filtered_tree->GetBranch("run");
    TBranch * blumiblock = filtered_tree->GetBranch("lumi");
    TBranch * beventid = filtered_tree->GetBranch("event");

    uhh2DiJetJERC::run_lumi_ev rle;
    brun->SetAddress(&rle.run);
    blumiblock->SetAddress(&rle.lumiblock);
    beventid->SetAddress(&rle.event);

    auto ientries = filtered_tree->GetEntries();
    for(auto ientry = 0l; ientry < ientries; ientry++){
      for(auto b : {brun, blumiblock, beventid}){
        b->GetEntry(ientry);
      }
      unprefirableSummary.push_back(rle);
    }
    cout<<"list of unprefireable events is prepared, found "<<unprefirableSummary.size()<<" events"<<endl;
  }

  if(isMC && !no_genp) cout<<"!!! WARNING, no genparticle are used! !!!"<<endl;

  L1METptThresh = stod(ctx.get("L1METptThresh", "15."));
  eta_thresh_low = stod(ctx.get("eta_thresh_low", "6."));

  if(!isMC) lumi_sel.reset(new LumiSelection(ctx));
  PVCleaner.reset(new PrimaryVertexCleaner());
  /* MET filters */
  if(!isMC) {
    PrimaryVertexId pvid=StandardPrimaryVertexId();
    metfilters_sel.reset(new uhh2::AndSelection(ctx, "metfilters"));
    metfilters_sel->add<TriggerSelection>("goodVertices", "Flag_goodVertices");
    metfilters_sel->add<TriggerSelection>("globalSuperTightHalo2016Filter", "Flag_globalSuperTightHalo2016Filter");
    metfilters_sel->add<TriggerSelection>("HBHENoiseFilter", "Flag_HBHENoiseFilter");
    metfilters_sel->add<TriggerSelection>("HBHENoiseIsoFilter", "Flag_HBHENoiseIsoFilter");
    metfilters_sel->add<TriggerSelection>("EcalDeadCellTriggerPrimitiveFilter", "Flag_EcalDeadCellTriggerPrimitiveFilter");
    metfilters_sel->add<TriggerSelection>("BadPFMuonFilter", "Flag_BadPFMuonFilter");
    metfilters_sel->add<TriggerSelection>("BadChargedCandidateFilter", "Flag_BadChargedCandidateFilter");
    metfilters_sel->add<TriggerSelection>("eeBadScFilter", "Flag_eeBadScFilter"); // Not recommended for MC, but do check
    metfilters_sel->add<TriggerSelection>("ecalBadCalibFilter", "Flag_ecalBadCalibFilter"); // for 2017 and 2018 is always 1. need a EcalBadCalibSelection for the recalculated value.
    metfilters_sel->add<EcalBadCalibSelection>("EcalBadCalibSelection");
    metfilters_sel->add<NPVSelection>("1 good PV",1,-1,pvid);
  }

  init_JEC(ctx);//set up JECs

  if(ispuppi) Jet_PFID = JetPFID(JetPFID::WP_TIGHT_PUPPI);
  else Jet_PFID = JetPFID(JetPFID::WP_TIGHT_CHS);
  jetID.reset(new JetCleaner(ctx, Jet_PFID));
  jetPUid.reset(new JetCleaner(ctx, JetPUid(JetPUid::WP_TIGHT)));

  //remove low pt jets
  minJetPt = stod(ctx.get("minJetPt","10"));
  minGenJetPt = stod(ctx.get("minGenJetPt","15"));
  qScale_max = stod(ctx.get("qScale","1.5"));
  jetcleaner.reset(new JetCleaner(ctx, minJetPt, 5.2));
  genjetcleaner.reset(new GenJetCleaner(ctx, minGenJetPt, 5.2));

  //Lepton cleaner
  const     MuonId muoSR(AndId<Muon>    (MuonID(Muon::CutBasedIdTight),PtEtaCut  (15, 2.4)));
  const ElectronId eleSR(AndId<Electron>(ElectronID_MVA_Fall17_loose_noIso , PtEtaSCCut(15, 2.4)));
  muoSR_cleaner.reset(new     MuonCleaner(muoSR));
  eleSR_cleaner.reset(new ElectronCleaner(eleSR));

  //#############################################  Trigger  #########################################################
  trigger_central = (ctx.get("Trigger_Central") == "true");
  trigger_fwd     = (ctx.get("Trigger_FWD") == "true");
  //    ts  = (ctx.get("Trigger_Single") == "true"); //if true use single jet trigger, if false di jet trigger TODO collapse the SiJet and DiJEt AnalysisModules into one?


  if(!isMC){
    for (std::string mode: {"central", "forward"}) {
      std::string triggerModeName = PtBinsTrigger+"_"+mode;
      for (size_t i = 0; i < pt_indexes.at(triggerModeName).size() ; i++) {
        std::string trg_xml = PtBinsTrigger+"_"+pt_indexes.at(triggerModeName)[i];
        if (isAK8) trg_xml += "_AK8";
        const std::string& trg_name = ctx.get( trg_xml , "NULL");
        if (debug) {
          std::cout << "mode: " << mode << " i: " << i << '\n';
          std::cout << "trg_xml " << trg_xml << '\n';
          std::cout << "trg_name " << trg_name << '\n';
        }
        if (mode=="central") {
          if ( trg_name != "NULL") trigger_sel.emplace_back(new TriggerSelection( trg_name ));
          else trigger_sel.emplace_back(new uhh2::AndSelection(ctx)); // TODO Do we really want this?
        } else if (mode=="forward") {
          if ( trg_name != "NULL") trigger_HFJEC_sel.emplace_back(new TriggerSelection( trg_name ));
          else trigger_HFJEC_sel.emplace_back(new uhh2::AndSelection(ctx));
        } else {throw std::invalid_argument(mode+" is not implemented"); }
      }
    }
  }



  //JER Smearing for corresponding JEC-Version
  if(JERClosureTest && isMC) {
    if(is2018)                jet_resolution_smearer.reset(new GenericJetResolutionSmearer(ctx, "jets", "genjets", JERSmearing::SF_13TeV_Autumn18_RunABCD_V4, "2018/Autumn18_V4_MC_PtResolution_AK4PFchs.txt"));
    if(is2017)                jet_resolution_smearer.reset(new GenericJetResolutionSmearer(ctx, "jets", "genjets", JERSmearing::SF_13TeV_Fall17_V3_RunBCDEF_Madgraph,"2017/Fall17_V3_MC_PtResolution_AK4PFchs.txt"));
    if(is2016v2 || is2016v3)  jet_resolution_smearer.reset(new GenericJetResolutionSmearer(ctx, "jets", "genjets", JERSmearing::SF_13TeV_Summer16_25nsV1,"2016/Summer16_25nsV1_MC_PtResolution_AK4PFchs.txt"));
  }

  //output
  ctx.undeclare_all_event_output();
  declare_output(ctx);//store only varaibles useful for dijet analysis

  // Do pileup reweighting (define it after undeclaring all other variables to keep the weights in the output)
  apply_lumiweights = string2bool(ctx.get("apply_lumiweights","true"));
  apply_lumiweights = apply_lumiweights && isMC;
  if (apply_lumiweights) LumiWeight_module.reset(new MCLumiWeight(ctx));

  DO_Pu_ReWeighting = string2bool(ctx.get("DO_Pu_ReWeighting"));
  DO_Pu_ReWeighting = DO_Pu_ReWeighting && isMC;
  SysType_PU = ctx.get("SysType_PU");
  if (DO_Pu_ReWeighting) pileupSF.reset(new MCPileupReweight(ctx,SysType_PU));

  init_hists(ctx);
  Jet_printer.reset(new JetPrinter("Jet-Printer", 0));
  GenJet_printer.reset(new GenJetPrinter("GenJet-Printer", 0));
  // if(debug && !no_genp) GenParticles_printer.reset(new GenParticlesPrinter(ctx));

  n_evt = 0;

  string lumifile = ctx.get("lumi_file");
  std::unique_ptr<TFile> file(TFile::Open(lumifile.c_str(), "read"));
  TTree * tree = dynamic_cast<TTree*>(file->Get("AnalysisTree"));
  if(!tree){
    throw runtime_error("LuminosityHists: Did not find TTree 'AnalysisTree' in file ;" + lumifile + "'");
  }
  // only fetch branches we really need:
  brun = tree->GetBranch("run");
  blumiblock = tree->GetBranch("luminosityBlock");
  bilumi = tree->GetBranch("intgRecLumi");

  run_lumi rl;
  double ilumi;
  double intlumi_pb = 0;
  brun->SetAddress(&rl.run);
  blumiblock->SetAddress(&rl.lumiblock);
  bilumi->SetAddress(&ilumi);

  //loop over all lumiblocks to save the map between run/lumiblock and stored lumi of the lumiblock (to be divided by 23s)
  auto ientries = tree->GetEntries();
  for(auto ientry = 0l; ientry < ientries; ientry++){
    for(auto b : {brun, blumiblock, bilumi}){
      b->GetEntry(ientry);
    }
    double ilumi_pb = ilumi * 1e-6; // convert units in file (microbarn) to pb.
    intlumi_pb += ilumi_pb;
    rl2lumi.insert(make_pair(rl, ilumi_pb));
    rl2intlumi.insert(make_pair(rl, intlumi_pb));
  }

  double ilumi_current_bin = 0.0;
  run_lumi last_entry;
  for(const auto & rl : rl2lumi){
    ilumi_current_bin += rl.second;
    if(ilumi_current_bin >= 2000){
      upper_binborders_runnrs.push_back(rl.first);
      lumi_in_bins.push_back(ilumi_current_bin - rl.second);
      ilumi_current_bin = ilumi_current_bin - 2000;
    }
    last_entry = rl.first;
  }
  upper_binborders_runnrs.push_back(last_entry); //this is not exactly an UPPER limit because it is equal to the highest possible entry, not greater than it...created exception for this case.
  lumi_in_bins.push_back(ilumi_current_bin);

  handle_l1jet_seeds = ctx.declare_event_input< vector< L1Jet>>("L1Jet_seeds");

  cout<<"end of AnalyseModule Constructor" << '\n';

};

AnalysisModule_DiJetTrg::~AnalysisModule_DiJetTrg() {
}

bool AnalysisModule_DiJetTrg::process(Event & event) {

  //###############################################################
  //
  //Selection Module for DiJetJERC Calculation
  //
  //Select Di-Jet Events
  //Define Barrel and Probe Jet
  //Use possible third Jet to estimate alpha
  //Apply MC-Weights for Reweighting (Second Iteration)
  //
  //###############################################################

  #define ak4jets event.jets

  n_evt++;
  if(debug){
    cout << endl << "++++++++++ NEW EVENT +++++++++" << endl << endl;
    cout << " Evt# "<<event.event<<" Run: "<<event.run<<" " << endl;
    //      cout << "pfparticles.size() = " <<event.pfparticles->size()<<endl;
  }

  // h_nocuts->fill(event);
  h_lumi_nocuts->fill(event);

  // Do pileup reweighting
  if (apply_lumiweights) if(!LumiWeight_module->process(event)) return false;

  // Do pileup reweighting
  if (DO_Pu_ReWeighting) if(!pileupSF->process(event)) return false;

  //Dump Input
  h_input->fill(event);
  //LEPTON selection
  muoSR_cleaner->process(event);
  sort_by_pt<Muon>(*event.muons);
  if(debug )std::cout<<"#muons = "<<event.muons->size()<<std::endl;

  eleSR_cleaner->process(event);
  sort_by_pt<Electron>(*event.electrons);
  if(debug) std::cout<<"#electrons = "<<event.electrons->size()<<std::endl;

  if (event.electrons->size()>0 || event.muons->size()>0) return false; //veto events with leptons

  h_runnr_input->fill(event);
  // PrimaryVertexV Cleaner
  if (!PVCleaner->process(event)) return false;
  h_afternVts->fill(event);

  // CMS-certified luminosity sections
  if(event.isRealData){
    if(!lumi_sel->passes(event)) return false;
    else h_lumisel->fill(event);
  }

  // MET filters
  if(!isMC && !metfilters_sel->passes(event)) return false;

  int event_in_lumibin = -1;
  double fill_event_integrated_lumi = 0;
  double inst_lumi = -1;
  double int_lumi_event = -1;
  if(event.isRealData){
    run_lumi rl_event{event.run, event.luminosityBlock};
    double lumiblock_lumi = rl2lumi[rl_event];
    inst_lumi = lumiblock_lumi/23;
    int_lumi_event = rl2intlumi[rl_event];

    vector<run_lumi>::iterator it;
    if(!(rl_event < upper_binborders_runnrs.back())){
      if(upper_binborders_runnrs.back() < rl_event) throw runtime_error("AnalysisModule_DiJetTrg: run_lumi of event greater than largest bin-border.");
      else it = prev(upper_binborders_runnrs.end()); //force the entries with the highest run_lumi to enter the last bin instead of overflow.
    }
    else it = upper_bound(upper_binborders_runnrs.begin(), upper_binborders_runnrs.end(), rl_event); //find the first entry in the vector of binborders that is greater than rl_event
    event_in_lumibin = distance(upper_binborders_runnrs.begin(), it); //find how many elements of the vector of binborders are smaller than 'it', this is the bin to be filled
    fill_event_integrated_lumi = lumi_in_bins.at(event_in_lumibin);
  }

  h_beforeCleaner->fill(event);

  //############### Jet Cleaner and First Selection (N_Jets >=2) ##############################
  int n_jets_beforeCleaner = ak4jets->size();
  if(debug) cout<<"#jets before clean "<<n_jets_beforeCleaner<<endl;

  //JetID
  int jet_n = ak4jets->size();
  if(jet_n<2) return false;

  jetID->process(event); //TODO FixME: make sure JetID works for AK8
  if(apply_PUid) jetPUid->process(event);
  int n_jets_afterCleaner = ak4jets->size();
  // TODO discard events if not all jets fulfill JetID instead of just discarding single jets
  if(debug) cout<<"#jets after clean "<<n_jets_afterCleaner<<endl;
  sort_by_pt<Jet>(*ak4jets);
  jet_n = ak4jets->size();
  if(jet_n<2) return false;

  //discard events if not all jets fulfill JetID instead of just discarding single jets
  //    if (n_jets_beforeCleaner != n_jets_afterCleaner) return false;
  if(!isMC) h_afterCleaner->fill(event);
  // if(debug) cout<<"ak4jets->at(0).tagname2tag(pileup_loose) = "<<ak4jets->at(0).tagname2tag("pileup_loose")<<endl;
  // if(debug) cout<<"ak4jets->at(0).tagname2tag(pileup_medium) = "<<ak4jets->at(0).tagname2tag("pileup_medium")<<endl;
  // if(debug) cout<<"ak4jets->at(0).tagname2tag(pileup_tight) = "<<ak4jets->at(0).tagname2tag("pileup_tight")<<endl;
  // if(n_jets_afterCleaner>3){
  //   if(debug) cout<<"ak4jets->at(2).PU_id = "<<ak4jets->at(2).pileupID()<<endl;
  //   if(debug) cout<<"ak4jets->at(2).get_tag(pileup_loose) = "<<ak4jets->at(2).get_tag(ak4jets->at(2).tagname2tag("pileup_loose"))<<endl;
  //   if(debug) cout<<"ak4jets->at(2).get_tag(pileup_medium) = "<<ak4jets->at(2).get_tag(ak4jets->at(2).tagname2tag("pileup_medium"))<<endl;
  //   if(debug) cout<<"ak4jets->at(2).get_tag(pileup_tight) = "<<ak4jets->at(2).get_tag(ak4jets->at(2).tagname2tag("pileup_tight"))<<endl;
  //   //PUid ToDo: add flag to config file
  //   //      if(ak4jets->at(2).get_tag(ak4jets->at(2).tagname2tag("pileup_loose"))<1) return false;//TEST
  //   if(apply_PUid)
  // 	if(ak4jets->at(2).get_tag(ak4jets->at(2).tagname2tag("pileup_tight"))<1) return false;//TEST
  // }

  //####################  Select and Apply proper JEC-Versions for every Run ##################

  //    const int jet_n = ak4jets->size();
  h_2jets->fill(event);

  bool apply_MC = false;
  bool apply_A = false;
  bool apply_B = false;
  bool apply_C = false;
  bool apply_D = false;
  bool apply_E = false;
  bool apply_F = false;
  bool apply_G = false;
  bool apply_H = false;

  if(!isMC){
    if(is2018){
      if(event.run <= s_runnr_A_2018)       apply_A = true;
      else if(event.run <= s_runnr_B_2018)  apply_B = true;
      else if(event.run <= s_runnr_C_2018)  apply_C = true;
      else if(event.run <= s_runnr_D_2018)  apply_D = true;
      else throw runtime_error("AnalysisModule: run number not covered by if-statements in 2018 process-routine.");
    }
    if(is2017){
      if(event.run <= s_runnr_B_2017)       apply_B = true;
      else if(event.run <= s_runnr_C_2017)  apply_C = true;
      else if(event.run <= s_runnr_D_2017)  apply_D = true;
      else if(event.run <= s_runnr_E_2017)  apply_E = true;
      else if(event.run <= s_runnr_F_2017)  apply_F = true;
      else throw runtime_error("AnalysisModule: run number not covered by if-statements in 2017 process-routine.");
    }
  }
  else if(isMC){
    apply_MC = true;
  }

  if((apply_A+apply_B+apply_C+apply_D+apply_E+apply_F+apply_G+apply_H+apply_MC) != 1) throw runtime_error("In JERSFModule.cxx: Sum of apply_* when applying JECs is not == 1. Fix this.");

  h_beforeJEC->fill(event);

  if(apply_A) jet_corrector_A->process(event);
  if(apply_B) jet_corrector_B->process(event);
  if(apply_C) jet_corrector_C->process(event);
  if(apply_D) jet_corrector_D->process(event);
  if(apply_E) jet_corrector_E->process(event);
  if(apply_F) jet_corrector_F->process(event);
  if(apply_G) jet_corrector_G->process(event);
  if(apply_H) jet_corrector_H->process(event);
  if(apply_MC)jet_corrector_MC->process(event);

  h_afterJEC->fill(event);

  //#############################################################################################################
  //################################  Apply JER and MET  ########################################################
  if(debug){
    cout<<"After JEC, before JER"<<endl;
    cout << " Evt# "<<event.event<<" Run: "<<event.run<<" " << endl;
    cout<< "MET: "<< event.met->pt() <<endl;
    cout<<"number of jets: "<<ak4jets->size() <<endl;
  }

  //Apply JER to all jet collections
  if(JERClosureTest && isMC) jet_resolution_smearer->process(event);
  jetcleaner->process(event); //remove low pt jets
  jet_n = ak4jets->size();
  if(debug) cout<<"#jets after cleanining low pt jets "<<n_jets_afterCleaner<<endl;
  if(jet_n<2) return false;
  //FixME: do matching to trigger objects instead
  // sort_by_pt<Jet>(*ak4jets);
  // Jet jet1_a = ak4jets->at(0);// leading jet
  // Jet jet2_a = ak4jets->at(1);// sub-leading jet
  // if(jet1_a.eta()!=jet1_b.eta() || jet2_a.eta()!=jet2_b.eta() || jet1_a.phi()!=jet1_b.phi() || jet2_a.phi()!=jet2_b.phi()){
  //   if(debug) cout<<"jet1_a.eta()="<<jet1_a.eta()<<", jet1_b.eta()="<<jet1_b.eta()<<" jet1_a.pt()="<<jet1_a.pt()<<", jet1_b.pt()="<<jet1_b.pt()<<endl;
  //   if(debug) cout<<"jet2_a.eta()="<<jet2_a.eta()<<", jet2_b.eta()="<<jet2_b.eta()<<" jet2_a.pt()="<<jet2_a.pt()<<", jet2_b.pt()="<<jet2_b.pt()<<endl;
  //   return false; //make sure 2 leading jets don't change after JetID, important because trigger selection is based on the first 2 jets
  // }

  if(hypot(event.met->rawCHS_px(),event.met->rawCHS_py())/event.met->uncorr_v4().Pt()>1e4) return false;//remove bad events in Herwig++ sample
  if(debug){
    cout<<"After JER, before MET"<<endl;
    cout << " Evt# "<<event.event<<" Run: "<<event.run<<" " << endl;
    cout<< "MET: PF MET = "<< event.met->pt() <<" uncorr PF MET = "<<event.met->uncorr_v4().Pt()<<" unccor CHS MET = "<<hypot(event.met->rawCHS_px(),event.met->rawCHS_py())<<endl;
  }

  h_afterJER->fill(event);
  float DeltaMET = event.met->pt(); //MET before TypeI correction


  if(eta_thresh_low==1.) eta_thresh_high=2.;
  else if(eta_thresh_low==0.) eta_thresh_high=1.;
  else  if(eta_thresh_low==2.) eta_thresh_high=2.7;
  else  if(eta_thresh_low==2.650) eta_thresh_high=3.139;
  else  if(eta_thresh_low==2.7) eta_thresh_high=3;
  else  if(eta_thresh_low==3.) eta_thresh_high=5.;
  else  if(eta_thresh_low==6.) eta_thresh_high=7.;
  else{
    eta_thresh_low=0.;
    eta_thresh_high=10.;
  }
  if(debug && is2017){
    std::cout<<"jets with pt<"<<L1METptThresh<<" are excluded from MET correction if their eta ["<<eta_thresh_low<<", "<<eta_thresh_high<<"] "<<std::endl;
  }
  //correct MET only AFTER smearing the jets
  bool ischsMET = true;
  if(is2017){//modified MET for 2017 data
    if(apply_B)   jet_corrector_B->correct_met(event,ischsMET,L1METptThresh,  eta_thresh_low, eta_thresh_high);
    if(apply_C)   jet_corrector_C->correct_met(event,ischsMET,L1METptThresh,  eta_thresh_low, eta_thresh_high);
    if(apply_D)   jet_corrector_D->correct_met(event,ischsMET,L1METptThresh,  eta_thresh_low, eta_thresh_high);
    if(apply_E)   jet_corrector_E->correct_met(event,ischsMET,L1METptThresh,  eta_thresh_low, eta_thresh_high);
    if(apply_F)   jet_corrector_F->correct_met(event,ischsMET,L1METptThresh,  eta_thresh_low, eta_thresh_high);
    if(apply_MC)  jet_corrector_MC->correct_met(event,ischsMET,L1METptThresh,  eta_thresh_low, eta_thresh_high);
  } else if (is2018) { //FixME: extend to 2016 case
    if(apply_A)   jet_corrector_A->correct_met(event,ischsMET);
    if(apply_B)   jet_corrector_B->correct_met(event,ischsMET);
    if(apply_C)   jet_corrector_C->correct_met(event,ischsMET);
    if(apply_D)   jet_corrector_D->correct_met(event,ischsMET);
    if(apply_MC)  jet_corrector_MC->correct_met(event,ischsMET);
  }

  h_afterMET->fill(event);
  DeltaMET -= event.met->pt();//= difference between before and after TypeI correction

  if(debug){
    cout << "After MET"<<endl;
    cout<< "MET: "<< event.met->pt() <<" DeltaMET = "<<DeltaMET<<" DeltaMET/MET = "<<DeltaMET/event.met->pt()<<endl;
    if(DeltaMET/event.met->pt()>5.0) cout<<"AAAAAA, DeltaMET/MET is too large!"<<endl;
    cout << " Evt# "<<event.event<<" Run: "<<event.run<<" " << endl;
  }

  //############################################################################################################
  //*****************************************
  // definition od Probe and Barrel:
  //*****************************************

  int jetid_0 = 0;
  int jetid_1 = 1;
  int jetid_2 = 2;

  int jetid_0_last = 0;
  int jetid_1_last = 1;
  int jetid_2_last = 2;

  sort_by_pt<Jet>(*ak4jets);
  Jet* jet1 = &ak4jets->at(0);// leading jet
  Jet* jet2 = &ak4jets->at(1);// sub-leading jet
  float jet1_pt     = jet1->pt();
  float jet2_pt     = jet2->pt();
  auto factor_raw1  = jet1->JEC_factor_raw();
  auto factor_raw2  = jet2->JEC_factor_raw();
  float jet1_ptRaw  = jet1_pt*factor_raw1;
  float jet2_ptRaw  = jet2_pt*factor_raw2;
  float jet3_pt     = 0;
  float jet3_eta    = -100;
  float jet3_phi    = -100;
  auto factor_raw3  = 0;
  float jet3_ptRaw  = 0;

  if(jet_n>2){
    Jet* jet3 = &ak4jets->at(2);
    jet3_pt = jet3->pt();
    jet3_eta = jet3->eta();
    jet3_phi = jet3->phi();
    factor_raw3 = jet3->JEC_factor_raw();
    jet3_ptRaw = jet3_pt*factor_raw3;
  }

  float pt_ave = (jet1_pt + jet2_pt)/2.;
  float alpha = jet3_pt/pt_ave;

  float jet4_pt = (jet_n>3)? (&ak4jets->at(3))->pt(): 0;
  float jet5_pt = (jet_n>4)? (&ak4jets->at(4))->pt(): 0;
  float jet6_pt = (jet_n>5)? (&ak4jets->at(5))->pt(): 0;
  float jet7_pt = (jet_n>6)? (&ak4jets->at(6))->pt(): 0;

  bool isJER_SM = false;
  Jet* jet_barrel = jet1;
  Jet* jet_probe = jet2;
  int numb = (rand() % 2)+1;

  // std::cout << "RANDOMIZE " << numb << '\n';
  // std::cout << jet_barrel->pt() << " " << jet_barrel->eta() << " " << (fabs(jet1->eta())<s_eta_barr) << " " << '\n';
  // std::cout << jet_probe->pt() << " " << jet_probe->eta() << " " << (fabs(jet2->eta())<s_eta_barr) << " " << '\n';

  if ((fabs(jet1->eta())<s_eta_barr) && (fabs(jet2->eta())>s_eta_barr)) { jet_barrel = jet1; jet_probe = jet2; /*std::cout << "B=1 P=2" << '\n';*/}
  else if ((fabs(jet1->eta())>s_eta_barr) && (fabs(jet2->eta())<s_eta_barr)) { jet_barrel = jet2; jet_probe = jet1; /*std::cout << "B=2 P=1" << '\n';*/}
  else if(numb==1) { jet_barrel = jet2; jet_probe = jet1; /*std::cout << "B=2 P=1 for " << numb << '\n';*/}

  float barreljet_pt      = jet_barrel->pt();
  float barreljet_eta     = jet_barrel->eta();
  float barreljet_phi     = jet_barrel->phi();
  auto factor_raw_barrel  = jet_barrel->JEC_factor_raw();
  float barreljet_ptRaw   = barreljet_pt*factor_raw_barrel;
  float probejet_pt       = jet_probe->pt();
  float probejet_eta      = jet_probe->eta();
  float probejet_phi      = jet_probe->phi();
  auto factor_raw_probe   = jet_probe->JEC_factor_raw();
  float probejet_ptRaw    = probejet_pt*factor_raw_probe;

  //Store events in the same eta bin for SM method JER SFs
  for (int i = 0; i < n_eta_bins_JER; i++) {
    if(fabs(barreljet_eta)>=eta_bins_JER[i] && fabs(barreljet_eta)<eta_bins_JER[i+1] && fabs(probejet_eta)>=eta_bins_JER[i] && fabs(probejet_eta)<eta_bins_JER[i+1]) {
      isJER_SM = true;
    }
  }


  float asymmetry = (barreljet_pt - probejet_pt)/(barreljet_pt + probejet_pt);
  float rel_r = (1+asymmetry)/(1-asymmetry);
  // std::cout << "asymmetry=" << asymmetry << '\n';


  ////###############################################  Trigger  ###################################

  //*****************************************
  //
  // Trigger Selection:
  //  Use Central trigger for abs(probejet_eta) < 2.8
  //  Use FWD trigger for abs(probejet_eta) => 2.8
  //  Upper thresholds are applied to make sure
  //  only one trigger contribute in this pT_ave bin
  //
  //*****************************************

  std::map<std::string, bool> pass_trigger, pass_trigger_HFJEC;
  std::map<std::string, double> trg_thresh, trgHF_thresh;

  for (std::string mode: {"central", "forward"}) {
    std::string trg_thr_name = PtBinsTrigger+"_"+mode;
    if (isAK8) trg_thr_name += "_AK8";
    trg_thr_name += "_"+year;
    if (debug) std::cout << "trg_thr_name " << trg_thr_name << " pt_bins: " << pt_trigger_thr.at(trg_thr_name).size() << '\n';
    for (unsigned int i = 0; i < pt_trigger_thr.at(trg_thr_name).size(); i++) {
      std::string trigger = pt_indexes.at(PtBinsTrigger+"_"+mode)[i];
      if (debug) { std::cout << "pass_trigger creation: " << trigger << " index: " << i ; }
      if (mode== "central") {
        pass_trigger.insert({ trigger, false });
        trg_thresh.insert({ trigger, pt_trigger_thr.at(trg_thr_name)[i] });
      } else if (mode== "forward") {
        pass_trigger_HFJEC.insert({ trigger, false });
        trgHF_thresh.insert({ trigger, pt_trigger_thr.at(trg_thr_name)[i] });
      } else {throw std::invalid_argument(mode+" is not implemented"); }
      if (debug) { std::cout << " pt: " << pt_trigger_thr.at(trg_thr_name)[i] << '\n'; }
    }
  }

  if(debug) cout<<"before trigger pass checks\n";
  int n_trig = 0;

  if(event.isRealData){
    float probejet_eta = jet_probe->eta();

    bool eta_cut_bool = abs(probejet_eta) <  eta_cut;
    if(!trigger_fwd) eta_cut_bool = true;

    bool eta_cut_bool_HF = abs(probejet_eta) >=  eta_cut;
    if(!trigger_central) eta_cut_bool_HF = true;

    if (debug) std::cout << "eta_cut_bool " << eta_cut_bool << "\t" << "eta " << std::max(abs(jet1->eta()),abs(jet2->eta())) << "\t" << "trigger_central " << trigger_central<<  "\t" << "trigger_fwd " << trigger_fwd << " eta_cut_bool_HF " << eta_cut_bool_HF << '\n';

    if (eta_cut_bool) {

      for (size_t i = 0; i < pt_indexes.at(PtBinsTrigger+"_central").size() ; i++) {
        std::string trg_xml   = pt_indexes.at(PtBinsTrigger+"_central")[i];
        double pt_min = trg_thresh[trg_xml];
        double pt_max = ((i+1)<pt_indexes.at(PtBinsTrigger+"_central").size()) ? trg_thresh[pt_indexes.at(PtBinsTrigger+"_central")[i+1]]: PT_trigger_max;
        pass_trigger[trg_xml] = trigger_sel[i]->passes(event) && pt_ave>pt_min && pt_ave<pt_max;
        if (debug) { std::cout << "pass_trigger: " << trg_xml << " index: " << i << " pt_min: " << pt_min << " pt_max: " << pt_max << " passed: " << pass_trigger[trg_xml] << '\n'; }
      }
    }
    //ToDo: remove requirement on eta_bool, add one more bin in forward region in

    //FWD Trigger
    if (eta_cut_bool_HF && trigger_fwd ) {

      for (size_t i = 0; i < pt_indexes.at(PtBinsTrigger+"_forward").size() ; i++) {
        std::string trg_xml   = pt_indexes.at(PtBinsTrigger+"_forward")[i];
        double pt_min = trgHF_thresh[trg_xml];
        double pt_max = ((i+1)<pt_indexes.at(PtBinsTrigger+"_forward").size()) ? trgHF_thresh[pt_indexes.at(PtBinsTrigger+"_forward")[i+1]]: PT_trigger_max;
        pass_trigger_HFJEC[trg_xml] = trigger_HFJEC_sel[i]->passes(event) && pt_ave>pt_min && pt_ave<pt_max;
        if (debug) { std::cout << "pass_trigger: " << trg_xml << " index: " << i << " pt_min: " << pt_min << " pt_max: " << pt_max << " passed: " << pass_trigger_HFJEC[trg_xml] << '\n'; }
      }

    }


    //HLT Selection
    // bool pass_trigger = false;
    // if(eta_cut_bool) pass_trigger = (pass_trigger40 || pass_trigger60 || pass_trigger80 || pass_trigger140 || pass_trigger200  || pass_trigger260 || pass_trigger320 || pass_trigger400 || pass_trigger500);
    // else  pass_trigger = (pass_trigger40 || pass_trigger60_HFJEC || pass_trigger80_HFJEC || pass_trigger100_HFJEC || pass_trigger160_HFJEC || pass_trigger220_HFJEC || pass_trigger300_HFJEC);

    bool fired_triggers = false;

    for (auto const& x : eta_cut_bool? pass_trigger: pass_trigger_HFJEC) {
      if(debug) std::cout << "fired_triggers " <<  (eta_cut_bool? "central " : "forward ") <<  x.first << ": " << x.second << std::endl ;
      fired_triggers = fired_triggers|| x.second ;
      if(x.second) n_trig++;
    }

    h_beforeTriggerData->fill(event);

    if(!fired_triggers)	return false;
  }


  if (n_trig>1) throw runtime_error("AnalysisModule_DiJetTrg.cxx: more than 1 trigger fired. To Check.");
  h_afterTriggerData->fill(event);

  double dR_jet3_barreljet = (ak4jets->size()>2)? deltaR(ak4jets->at(2), *jet_barrel): -1;
  double dR_jet3_probejet = (ak4jets->size()>2)? deltaR(ak4jets->at(2), *jet_probe): -1;

  //read or calculated values for dijet events
  float gen_pthat = 0; //pt hat (from QCD simulation)
  float gen_PUpthat = 0;
  float gen_weight = 0;
  if(!event.isRealData){
    gen_weight = event.weight;
    gen_pthat = event.genInfo->qScale();
    gen_PUpthat = event.genInfo->PU_pT_hat_max();
  }
  float nvertices = event.pvs->size();
  float nPU = 0 ;//todo for data?
  if(!event.isRealData) nPU = event.genInfo->pileup_TrueNumInteractions();

  // if (debug) std::cout << "nPU " << nPU << '\n';

  TVector2 pt, met;
  met.Set(event.met->pt() * cos(event.met->phi()),event.met->pt() * sin(event.met->phi()));
  pt.Set(barreljet_pt * cos(barreljet_phi),barreljet_pt* sin(barreljet_phi));
  float mpf_r = 1 + (met.Px()*pt.Px() + met.Py()*pt.Py())/(pt.Px()*pt.Px() + pt.Py()*pt.Py());
  float B = (met.Px()*pt.Px() + met.Py()*pt.Py())/((probejet_pt + barreljet_pt) * sqrt(pt.Px()*pt.Px() + pt.Py()*pt.Py())); //vec_MET*vec_ptbarr/(2ptave*ptbarr)
  float jets_pt = 0;
  for(int i=2;i<jet_n;i++){
    jets_pt += ((Jet*)&ak4jets->at(i))->pt();
  }

  int flavor = 0;
  float onoffDummy =0.;

  double had_n_Efrac = ak4jets->at(0).neutralHadronEnergyFraction();
  double had_ch_Efrac = ak4jets->at(0).chargedHadronEnergyFraction();
  double mu_Efrac = ak4jets->at(0).muonEnergyFraction();
  double ph_Efrac = ak4jets->at(0).photonEnergyFraction();

  //fill the containers
  event.set(tt_run,event.run);
  event.set(tt_evID,event.event);
  event.set(tt_lumiSec,event.luminosityBlock);
  event.set(tt_ev_weight,event.weight);
  event.set(tt_rho,event.rho);
  event.set(tt_JER_SM,isJER_SM);
  event.set(tt_gen_pthat,gen_pthat);
  event.set(tt_gen_PUpthat,gen_PUpthat);
  event.set(tt_gen_weight,gen_weight);
  event.set(tt_jet1_pt,jet1_pt);
  event.set(tt_jet2_pt,jet2_pt);
  event.set(tt_jet3_pt,jet3_pt);
  event.set(tt_jet4_pt,jet4_pt);
  event.set(tt_jet5_pt,jet5_pt);
  event.set(tt_jet6_pt,jet6_pt);
  event.set(tt_jet7_pt,jet7_pt);
  event.set(tt_jet3_eta,jet3_eta);
  event.set(tt_jet3_phi,jet3_phi);
  event.set(tt_jet1_ptRaw,jet1_ptRaw);
  event.set(tt_jet2_ptRaw,jet2_ptRaw);
  event.set(tt_jet3_ptRaw,jet3_ptRaw);
  event.set(tt_jet1_pt_onoff_Resp,onoffDummy);
  event.set(tt_jet2_pt_onoff_Resp,onoffDummy);
  event.set(tt_nvertices,nvertices);
  event.set(tt_pv0Z,(event.pvs->at(0)).z());
  event.set(tt_pv0X,(event.pvs->at(0)).x());
  event.set(tt_pv0Y,(event.pvs->at(0)).y());
  event.set(tt_barreljet_neutEmEF,  jet_barrel->neutralEmEnergyFraction());
  event.set(tt_barreljet_neutHadEF, jet_barrel->neutralHadronEnergyFraction());
  event.set(tt_barreljet_chEmEF,    jet_barrel->chargedEmEnergyFraction());
  event.set(tt_barreljet_chHadEF,   jet_barrel->chargedHadronEnergyFraction());
  event.set(tt_barreljet_photonEF,  jet_barrel->photonEnergyFraction());
  event.set(tt_barreljet_muonEF,    jet_barrel->muonEnergyFraction());
  event.set(tt_probejet_neutEmEF,   jet_probe->neutralEmEnergyFraction());
  event.set(tt_probejet_neutHadEF,  jet_probe->neutralHadronEnergyFraction());
  event.set(tt_probejet_chEmEF,     jet_probe->chargedEmEnergyFraction());
  event.set(tt_probejet_chHadEF,    jet_probe->chargedHadronEnergyFraction());
  event.set(tt_probejet_photonEF,   jet_probe->photonEnergyFraction());
  event.set(tt_probejet_muonEF,     jet_probe->muonEnergyFraction());
  event.set(tt_probejet_pt,probejet_pt);
  event.set(tt_probejet_eta,probejet_eta);
  event.set(tt_probejet_phi,probejet_phi);
  event.set(tt_probejet_ptRaw,probejet_ptRaw);
  event.set(tt_barreljet_pt,barreljet_pt);
  event.set(tt_barreljet_eta,barreljet_eta);
  event.set(tt_barreljet_phi,barreljet_phi);
  event.set(tt_barreljet_ptRaw,barreljet_ptRaw);
  event.set(tt_dR_jet3_barreljet,dR_jet3_barreljet);
  event.set(tt_dR_jet3_probejet,dR_jet3_probejet);
  event.set(tt_pt_ave,pt_ave);
  event.set(tt_alpha,alpha);
  event.set(tt_asymmetry,asymmetry);
  event.set(tt_B,B);
  event.set(tt_MET,event.met->pt());
  event.set(tt_DeltaMET,DeltaMET);
  event.set(tt_rel_r,rel_r);
  event.set(tt_mpf_r,mpf_r);
  event.set(tt_nPU,nPU);
  event.set(tt_matchJetId_0,-10.);
  event.set(tt_matchJetId_1,-10.);
  event.set(tt_jets_pt,jets_pt);
  event.set(tt_jet_n,jet_n);
  event.set(tt_partonFlavor,flavor);
  // event.set(tt_had_n_Efrac,had_n_Efrac);
  // event.set(tt_had_ch_Efrac,had_ch_Efrac);
  // event.set(tt_mu_Efrac,mu_Efrac);
  // event.set(tt_ph_Efrac,ph_Efrac);
  event.set(tt_inst_lumi,inst_lumi);
  event.set(tt_integrated_lumi_in_bin,fill_event_integrated_lumi);
  event.set(tt_lumibin,event_in_lumibin);
  event.set(tt_integrated_lumi,int_lumi_event);


  event.set(tt_trigger40, (pass_trigger.find("trigger40") != pass_trigger.end())? pass_trigger["trigger40"]: 0);
  event.set(tt_trigger60, (pass_trigger.find("trigger60") != pass_trigger.end())? pass_trigger["trigger60"]: 0);
  event.set(tt_trigger80, (pass_trigger.find("trigger80") != pass_trigger.end())? pass_trigger["trigger80"]: 0);
  event.set(tt_trigger140, (pass_trigger.find("trigger140") != pass_trigger.end())? pass_trigger["trigger140"]: 0);
  event.set(tt_trigger200, (pass_trigger.find("trigger200") != pass_trigger.end())? pass_trigger["trigger200"]: 0);
  event.set(tt_trigger260, (pass_trigger.find("trigger260") != pass_trigger.end())? pass_trigger["trigger260"]: 0);
  event.set(tt_trigger320, (pass_trigger.find("trigger320") != pass_trigger.end())? pass_trigger["trigger320"]: 0);
  event.set(tt_trigger400, (pass_trigger.find("trigger400") != pass_trigger.end())? pass_trigger["trigger400"]: 0);
  event.set(tt_trigger450, (pass_trigger.find("trigger450") != pass_trigger.end())? pass_trigger["trigger450"]: 0);
  event.set(tt_trigger500, (pass_trigger.find("trigger500") != pass_trigger.end())? pass_trigger["trigger500"]: 0);
  event.set(tt_trigger40_HFJEC, (pass_trigger_HFJEC.find("trigger40_HFJEC") != pass_trigger_HFJEC.end())? pass_trigger_HFJEC["trigger40_HFJEC"]: 0);
  event.set(tt_trigger60_HFJEC, (pass_trigger_HFJEC.find("trigger60_HFJEC") != pass_trigger_HFJEC.end())? pass_trigger_HFJEC["trigger60_HFJEC"]: 0);
  event.set(tt_trigger80_HFJEC, (pass_trigger_HFJEC.find("trigger80_HFJEC") != pass_trigger_HFJEC.end())? pass_trigger_HFJEC["trigger80_HFJEC"]: 0);
  event.set(tt_trigger100_HFJEC, (pass_trigger_HFJEC.find("trigger100_HFJEC") != pass_trigger_HFJEC.end())? pass_trigger_HFJEC["trigger100_HFJEC"]: 0);
  event.set(tt_trigger140_HFJEC, (pass_trigger_HFJEC.find("trigger140_HFJEC") != pass_trigger_HFJEC.end())? pass_trigger_HFJEC["trigger140_HFJEC"]: 0);
  event.set(tt_trigger160_HFJEC, (pass_trigger_HFJEC.find("trigger160_HFJEC") != pass_trigger_HFJEC.end())? pass_trigger_HFJEC["trigger160_HFJEC"]: 0);
  event.set(tt_trigger200_HFJEC, (pass_trigger_HFJEC.find("trigger200_HFJEC") != pass_trigger_HFJEC.end())? pass_trigger_HFJEC["trigger200_HFJEC"]: 0);
  event.set(tt_trigger220_HFJEC, (pass_trigger_HFJEC.find("trigger220_HFJEC") != pass_trigger_HFJEC.end())? pass_trigger_HFJEC["trigger220_HFJEC"]: 0);
  event.set(tt_trigger260_HFJEC, (pass_trigger_HFJEC.find("trigger260_HFJEC") != pass_trigger_HFJEC.end())? pass_trigger_HFJEC["trigger260_HFJEC"]: 0);
  event.set(tt_trigger300_HFJEC, (pass_trigger_HFJEC.find("trigger300_HFJEC") != pass_trigger_HFJEC.end())? pass_trigger_HFJEC["trigger300_HFJEC"]: 0);
  event.set(tt_trigger320_HFJEC, (pass_trigger_HFJEC.find("trigger320_HFJEC") != pass_trigger_HFJEC.end())? pass_trigger_HFJEC["trigger320_HFJEC"]: 0);
  event.set(tt_trigger400_HFJEC, (pass_trigger_HFJEC.find("trigger400_HFJEC") != pass_trigger_HFJEC.end())? pass_trigger_HFJEC["trigger400_HFJEC"]: 0);

  // Geninfo for MC
  // Start MC

  int genjet_n            = 0;
  float genjet1_pt        = 0;
  float genjet2_pt        = 0;
  float genjet3_pt        = 0;
  float genjet4_pt        = 0;
  float genjet5_pt        = 0;
  float genjet6_pt        = 0;
  float genjet7_pt        = 0;
  float genjet1_eta       = -10;
  float genjet2_eta       = -10;
  float genjet3_eta       = -10;
  float genjet1_phi       = -10;
  float genjet2_phi       = -10;
  float genjet3_phi       = -10;
  float dR_jet3_RECO_GEN  = -1;
  bool no_mc_spikes       = 0;
  float gen_pt_ave        = 0;
  float gen_alpha         = -1;

  if(isMC){
    genjet_n = event.genjets->size();
    sort_by_pt<GenJet>(*event.genjets);
    if (debug) std::cout << "genjet_n " << genjet_n << '\n';
    if(genjet_n<2) return false; // Not sure about this

    genjet1_pt  = event.genjets->at(0).pt();
    genjet1_eta = event.genjets->at(0).eta();
    genjet1_phi = event.genjets->at(0).phi();
    genjet2_pt  = event.genjets->at(1).pt();
    genjet2_eta = event.genjets->at(1).eta();
    genjet2_phi = event.genjets->at(1).phi();
    // barrelgenjet_pt
    if(genjet_n>2) genjet3_pt  = event.genjets->at(2).pt();
    if(genjet_n>2) genjet3_eta = event.genjets->at(2).eta();
    if(genjet_n>2) genjet3_phi = event.genjets->at(2).phi();
    if(genjet_n>2 && jet_n>2) dR_jet3_RECO_GEN =  deltaR(ak4jets->at(2), event.genjets->at(2));
    if(genjet_n>3) genjet4_pt  = event.genjets->at(3).pt();
    if(genjet_n>4) genjet5_pt  = event.genjets->at(4).pt();
    if(genjet_n>5) genjet6_pt  = event.genjets->at(5).pt();
    if(genjet_n>6) genjet7_pt  = event.genjets->at(6).pt();


    // define barrel and probe genjets
    GenJet* genjet1 = &event.genjets->at(0);// leading genjet
    GenJet* genjet2 = &event.genjets->at(1);// sub-leading genjet
    float genjet1_pt = genjet1->pt(); float genjet2_pt = genjet2->pt();
    no_mc_spikes = (genjet1_pt < 1.5*gen_pthat || jet1_pt < 1.5*genjet1_pt); //(pTgen1 < 1.5*gen_pthat || pTreco1 < 1.5* pTgen1) // pthat cut from mikko
    gen_pt_ave = (genjet1_pt + genjet2_pt)/2.;

    gen_alpha = genjet3_pt/pt_ave;

  }

  event.set(tt_genjet_n,genjet_n);
  event.set(tt_genjet1_pt,genjet1_pt);
  event.set(tt_genjet2_pt,genjet2_pt);
  event.set(tt_genjet3_pt,genjet3_pt);
  event.set(tt_genjet4_pt,genjet4_pt);
  event.set(tt_genjet5_pt,genjet5_pt);
  event.set(tt_genjet6_pt,genjet6_pt);
  event.set(tt_genjet7_pt,genjet7_pt);
  event.set(tt_genjet1_eta,genjet1_eta);
  event.set(tt_genjet2_eta,genjet2_eta);
  event.set(tt_genjet3_eta,genjet3_eta);
  event.set(tt_genjet1_phi,genjet1_phi);
  event.set(tt_genjet2_phi,genjet2_phi);
  event.set(tt_genjet3_phi,genjet3_phi);
  event.set(tt_dR_jet3_RECO_GEN,dR_jet3_RECO_GEN);
  event.set(tt_no_mc_spikes, no_mc_spikes);
  event.set(tt_gen_pt_ave,gen_pt_ave);
  event.set(tt_gen_alpha,gen_alpha);

  //##################################################   Advanced Selections   ################################

  sel->SetEvent(event);

  if(debug) {cout << "before 'dijet selection' : " << "\n" << " Evt# "<<event.event<<" Run: "<<event.run<<" " << endl;}

  if(!sel->DiJet()) return false;

  //Pu_pt_hat/pt_hat Selection
  if(!event.isRealData){
    if(!sel->PUpthat()) return false;
    if(!sel->PtaveVsQScale(qScale_max)) return false;//MadGraph 1.5, 1.2 Pythia8 1.2, Herwig++ ?
    if(!sel->PtMC()) return false; // For MC only one Pt threshold
  }
  //PhiEta Region cleaning
  if(apply_EtaPhi_cut && !sel->ApplyHotMap()) return false;
  if(apply_EtaPhi_HCAL && !sel->EtaPhiCleaning()) return false;
  if(!sel->EnergyEtaCut()) return false;
  if(!sel->DiJetAdvanced()) return false;

  h_dijet->fill(event);
  h_lumi_dijet->fill(event);
  h_match->fill(event);
  h_lumi_match->fill(event);
  if(event.isRealData){

    if(pass_trigger.find("trigger40") != pass_trigger.end() && pass_trigger["trigger40"] ) {h_trg40->fill(event); h_lumi_Trig40->fill(event);}
    if(pass_trigger.find("trigger60") != pass_trigger.end() && pass_trigger["trigger60"] ) {h_trg60->fill(event); h_lumi_Trig60->fill(event);}
    if(pass_trigger.find("trigger80") != pass_trigger.end() && pass_trigger["trigger80"] ) {h_trg80->fill(event); h_lumi_Trig80->fill(event);}
    if(pass_trigger.find("trigger140") != pass_trigger.end() && pass_trigger["trigger140"] ) {h_trg140->fill(event); h_lumi_Trig140->fill(event);}
    if(pass_trigger.find("trigger200") != pass_trigger.end() && pass_trigger["trigger200"] ) {h_trg200->fill(event); h_lumi_Trig200->fill(event);}
    if(pass_trigger.find("trigger260") != pass_trigger.end() && pass_trigger["trigger260"] ) {h_trg260->fill(event); h_lumi_Trig260->fill(event);}
    if(pass_trigger.find("trigger320") != pass_trigger.end() && pass_trigger["trigger320"] ) {h_trg320->fill(event); h_lumi_Trig320->fill(event);}
    if(pass_trigger.find("trigger400") != pass_trigger.end() && pass_trigger["trigger400"] ) {h_trg400->fill(event); h_lumi_Trig400->fill(event);}
    if(pass_trigger.find("trigger450") != pass_trigger.end() && pass_trigger["trigger450"] ) {h_trg450->fill(event); h_lumi_Trig450->fill(event);}
    if(pass_trigger.find("trigger500") != pass_trigger.end() && pass_trigger["trigger500"] ) {h_trg500->fill(event); h_lumi_Trig500->fill(event);}
    if(pass_trigger_HFJEC.find("trigger40_HFJEC") != pass_trigger_HFJEC.end() && pass_trigger_HFJEC["trigger40_HFJEC"] ) {h_trgHF40->fill(event); h_lumi_TrigHF40->fill(event);}
    if(pass_trigger_HFJEC.find("trigger60_HFJEC") != pass_trigger_HFJEC.end() && pass_trigger_HFJEC["trigger60_HFJEC"] ) {h_trgHF60->fill(event); h_lumi_TrigHF60->fill(event);}
    if(pass_trigger_HFJEC.find("trigger80_HFJEC") != pass_trigger_HFJEC.end() && pass_trigger_HFJEC["trigger80_HFJEC"] ) {h_trgHF80->fill(event); h_lumi_TrigHF80->fill(event);}
    if(pass_trigger_HFJEC.find("trigger100_HFJEC") != pass_trigger_HFJEC.end() && pass_trigger_HFJEC["trigger100_HFJEC"] ) {h_trgHF100->fill(event); h_lumi_TrigHF100->fill(event);}
    if(pass_trigger_HFJEC.find("trigger140_HFJEC") != pass_trigger_HFJEC.end() && pass_trigger_HFJEC["trigger140_HFJEC"] ) {h_trgHF140->fill(event); h_lumi_TrigHF140->fill(event);}
    if(pass_trigger_HFJEC.find("trigger160_HFJEC") != pass_trigger_HFJEC.end() && pass_trigger_HFJEC["trigger160_HFJEC"] ) {h_trgHF160->fill(event); h_lumi_TrigHF160->fill(event);}
    if(pass_trigger_HFJEC.find("trigger200_HFJEC") != pass_trigger_HFJEC.end() && pass_trigger_HFJEC["trigger200_HFJEC"] ) {h_trgHF200->fill(event); h_lumi_TrigHF200->fill(event);}
    if(pass_trigger_HFJEC.find("trigger220_HFJEC") != pass_trigger_HFJEC.end() && pass_trigger_HFJEC["trigger220_HFJEC"] ) {h_trgHF220->fill(event); h_lumi_TrigHF220->fill(event);}
    if(pass_trigger_HFJEC.find("trigger260_HFJEC") != pass_trigger_HFJEC.end() && pass_trigger_HFJEC["trigger260_HFJEC"] ) {h_trgHF260->fill(event); h_lumi_TrigHF260->fill(event);}
    if(pass_trigger_HFJEC.find("trigger300_HFJEC") != pass_trigger_HFJEC.end() && pass_trigger_HFJEC["trigger300_HFJEC"] ) {h_trgHF300->fill(event); h_lumi_TrigHF300->fill(event);}
    if(pass_trigger_HFJEC.find("trigger320_HFJEC") != pass_trigger_HFJEC.end() && pass_trigger_HFJEC["trigger320_HFJEC"] ) {h_trgHF320->fill(event); h_lumi_TrigHF320->fill(event);}
    if(pass_trigger_HFJEC.find("trigger400_HFJEC") != pass_trigger_HFJEC.end() && pass_trigger_HFJEC["trigger400_HFJEC"] ) {h_trgHF400->fill(event); h_lumi_TrigHF400->fill(event);}
  }

  if(useUnprefireable){
    if(!sel->Unprefirable(unprefirableSummary)) return false;
  }


  // L1 jet seed cleaning
  if(apply_L1seed_from_bx1_filter){
    if(debug) cout << "before the L1 seed filter" << endl;
    if(!sel->L1JetBXcleanSmart()){
      if(debug) cout << "L1 seed filtered" << endl;
      return false;
    }
    if(debug) cout << "after the first L1 seed filter" << endl;
  }

  //get the corresponding L1 Jet and save the bx
  std::vector< L1Jet>* l1jets = &event.get(handle_l1jet_seeds);

  if(debug) cout << "declared L1Jet seeds" << endl;

  int jet1_l1bx, jet2_l1bx, jet3_l1bx;

  unsigned int n_l1jets =l1jets->size();
  if(debug) cout << "l1jets size is "<<n_l1jets<<endl;
  if(n_l1jets>0){
    double dRmin = 100.;
    int dRmin_seed_idx = -1;
    float dR;
    if(debug) cout << "before first L1Jet seeds dR loop" << endl;
    for(unsigned int i = 0; i<n_l1jets; i++){
      dR=uhh2::deltaR(l1jets->at(i),ak4jets->at(0));

      if(dR < dRmin){
        dRmin=dR;
        dRmin_seed_idx = i;
      }
    }
    if( ( l1jets->at(dRmin_seed_idx).pt() / ak4jets->at(0).pt() ) < 0.2 ) jet1_l1bx = -10;
    else jet1_l1bx = l1jets->at(dRmin_seed_idx).bx();
  }
  else jet1_l1bx = 10;

  if(n_l1jets>1){
    double dRmin = 100.;
    int dRmin_seed_idx = -1;
    float dR;
    for(unsigned int i = 0; i<n_l1jets; i++){
      dR=uhh2::deltaR(l1jets->at(i),ak4jets->at(1));

      if(dR < dRmin){
        dRmin=dR;
        dRmin_seed_idx = i;
      }
    }
    if( ( l1jets->at(dRmin_seed_idx).pt() / ak4jets->at(0).pt() ) < 0.2 ) jet2_l1bx = -10;
    else jet2_l1bx = l1jets->at(dRmin_seed_idx).bx();
  }
  else jet2_l1bx = 10;

  if(ak4jets->size()>2){
    if(n_l1jets>2){
      double dRmin = 100.;
      int dRmin_seed_idx = -1;
      float dR;
      for(unsigned int i = 0; i<n_l1jets; i++){
        dR=uhh2::deltaR(l1jets->at(i),ak4jets->at(2));

        if(dR < dRmin){
          dRmin=dR;
          dRmin_seed_idx = i;
        }
      }
      if( ( l1jets->at(dRmin_seed_idx).pt() / ak4jets->at(0).pt() ) < 0.2 ) jet3_l1bx = -10;
      else jet3_l1bx = l1jets->at(dRmin_seed_idx).bx();
    }
    else jet3_l1bx = 10;
  }
  else jet3_l1bx = 10;

  event.set(tt_jet1_l1bx,jet1_l1bx);
  event.set(tt_jet2_l1bx,jet2_l1bx);
  event.set(tt_jet3_l1bx,jet3_l1bx);

  //###############################################################################################

  if (event.get(tt_alpha) < 0.3) {
    h_sel->fill(event);
    h_lumi_sel->fill(event);
  }

  h_final->fill(event);
  h_lumi_final->fill(event);

  h_monitoring_final->fill(event);

  event.set(tt_Nmuon,event.muons->size());
  event.set(tt_muon_pt,(event.muons->size()>0)? event.muons->at(0).pt() : 0.);
  event.set(tt_Nele,event.electrons->size());
  event.set(tt_ele_pt, (event.electrons->size()>0) ? event.electrons->at(0).pt(): 0.);

  if(debug){
    cout<<"-- Event -- "<<endl;
    cout<<" Evt# "<<event.event<<" Run: "<<event.run<<" "<<endl;
    cout<<" Npv = "<<event.get(tt_nvertices)<<" jet_pt_ave = "<<event.get(tt_pt_ave)<<" MET = "<<met.Mod()<<" "<<event.met->pt()<<" "<<event.met->uncorr_pt()<<endl;
    cout<<"MET/pts = "<<event.get(tt_MET)/(event.get(tt_jets_pt)+event.get(tt_barreljet_pt)+event.get(tt_probejet_pt))<<endl;
    cout<<"Probe: "<<event.get(tt_probejet_eta)<<" "<<event.get(tt_probejet_phi)
    <<" "<<event.get(tt_probejet_pt)<<" "<<event.get(tt_probejet_ptRaw)<<endl;
    cout<<" Barrel: "<<event.get(tt_barreljet_eta)<<" "<<event.get(tt_barreljet_phi)
    <<" "<<event.get(tt_barreljet_pt)<<" "<<event.get(tt_barreljet_ptRaw)<<endl;
    cout<<" B="<<event.get(tt_B)<<" A="<<event.get(tt_asymmetry)<<" "<<event.get(tt_rel_r)<<" "<<event.get(tt_mpf_r)<<""<<endl;
    cout<<"N_muons= "<<event.muons->size()<<" N_electrons= "<<event.electrons->size()<<endl;
    if(event.muons->size()>0) cout<<"muon1 pt = "<<event.muons->at(0).pt()<<endl;
    if(event.electrons->size()>0) cout<<"ele1 pt = "<<event.electrons->at(0).pt()<<endl;
    cout<<" "<<event.get(tt_asymmetry)<<" "<<event.get(tt_rel_r)<<" "<<event.get(tt_mpf_r)<<""<<endl;
    cout<<" "<<endl;
    Jet_printer->process(event);
    if (isMC) GenJet_printer->process(event);
    // if (isMC) GenParticles_printer->process(event);
  }

  double dR_GenJet_GenParticle_barrel_matched = -1;
  double dR_GenJet_GenParticle_probe_matched  = 1;
  double response_barreljet = -1;
  double response_probejet  = -1;
  double response3rdjet     = -1;
  int flavor_barreljet      = -1;
  int flavor_probejet       = -1;
  int flavor_leadingjet     = -1;
  int flavor_subleadingjet  = -1;
  int flavor_3rdjet         = -1;
  double barrelgenjet_pt    = 0;
  double barrelgenjet_eta   = -10;
  double barrelgenjet_phi   = -10;
  double probegenjet_pt     = 0;
  double probegenjet_eta    = -10;
  double probegenjet_phi    = -10;
  double gen_asymmetry      = -10;

  double probejet_drminparton   = -1;
  double barreljet_drminparton  = -1;
  double probejet_ptptcl = -1;
  double barreljet_ptptcl = -1;
  double jet3_ptptcl = -1;
  int probejet_statusptcl = -1;
  int barreljet_statusptcl = -1;
  int jet3_statusptcl = -1;

  double response_jet1 = -1;
  double response_jet2 = -1;
  int genptcl_n = 0;
  float Bgen = -1;
  float genMet_pt = -1;
  int jet1_genID = -1;
  int jet2_genID = -1;
  int jet3_genID = -1;
  int jet4_genID = -1;
  int jet5_genID = -1;
  int jet6_genID = -1;

  if(isMC){
    double dr_cut = 0;
    if(jetLabel == "AK4CHS" || jetLabel == "AK4Puppi") dr_cut = 0.4;
    else if (jetLabel == "AK8CHS" || jetLabel == "AK8Puppi") dr_cut = 0.8;
    else throw runtime_error("AnalysisModule_DiJetTrg.cxx: Invalid jet-label specified.");

    //match jets to gen-jets
    int idx_jet_matching_genjet[genjet_n]; //for each genjet: idx of the closest jet with dR<=0.2
    int idx_matched_RecoGenjets[jet_n]; //for each recojet:  idx of the closest genjet with dR<=0.2
    int idx_matched_jets[jet_n]; //for each recojet:  idx of the closest GenParticle matched to a genjet with  dR<=0.4

    for(int i=0; i<genjet_n; i++) idx_jet_matching_genjet[i] = -1;

    for(int i=0; i<jet_n; i++){
      idx_matched_RecoGenjets[i] = -1;
      idx_matched_jets[i] = -1;
    }

    //matching gen- and reco-jets
    if (debug) std::cout << "Start Matching reco-gen" << '\n';
    for(unsigned int i=0; i<event.genjets->size(); i++){
      if (debug) std::cout << "\t genjet" << i << '\n';
      double dR_min = 99999; int idx_matching_jet = -1;
      for(unsigned int j=0; j<ak4jets->size(); j++){
        double dR = deltaR(ak4jets->at(j), event.genjets->at(i));
        if(dR<dR_min){
          dR_min = dR;
          if(dR_min<dr_cut) idx_matching_jet = j;
        }
        if (debug) std::cout << "\t\t recojet" << j << " with dR= " << dR << " idx_matching_jet=" << idx_matching_jet<< '\n';
      }
      idx_jet_matching_genjet[i] = idx_matching_jet;
      idx_matched_RecoGenjets[idx_matching_jet] = i;
      if (debug) std::cout << "\t\t genjet= " << i << " matched with dR_min= " << dR_min << " with reco-jet" << idx_matching_jet<< '\n';
      if(debug) cout << "the jet matching the genjet no. " << i << " is jet no. " << idx_matching_jet << endl;
    }
    /////////////////////

    //match genp to gen-jets
    int idx_j=-1;
    int idx_genp_min = -1;
    for(Particle & genj : *event.genjets){
      idx_j++;
      double dr_min = 99999;
      double dr_cut = 0;
      idx_genp_min = -1;
      if(jetLabel == "AK4CHS" || jetLabel == "AK4Puppi") dr_cut = 0.4; //TEST: for matching to parton use the full cone size
      else if (jetLabel == "AK8CHS" || jetLabel == "AK8Puppi") dr_cut = 0.8; //TEST: for matching to parton use the full cone size
      else throw runtime_error("AnalysisModule_DiJetTrg.cxx: Invalid jet-label specified.");

      int idx_gp = -1;
      double parton_pt_max=15;
      //	  cout<<"Loop over gen particles for matching to GEN jet No."<<idx_j<<endl;
      for(GenParticle & genp: *event.genparticles){
        idx_gp++;
        if(ispythia8){
          if(genp.status()!=23) continue;//Pythia8/Madgraph: only particle with status 23 are final state partons
          double dr = deltaR(genj,genp);
          if(dr < dr_min){
            dr_min = dr;
            idx_genp_min = idx_gp;
            if(debug) cout<<"PYTHIA8 gen jet and gen particle might be matched, status = "<<genp.status()<<" idx_genp_min = "<<idx_genp_min<<" dR="<<dr_min<<endl;
          }
        } else{//Herwig, etc
          if(genp.status()!=11) continue;//Herwig++: only particle with status 11 can contain parton from ME we are interested in
          double parton_pt = genp.pt();
          double dr = deltaR(genj,genp);
          if(debug && dr<0.4) cout<<"parton id = "<<idx_gp<<" with dr = "<<dr<<" parton_pt = "<<parton_pt<<endl;
          if(dr <dr_cut && dr < dr_min && parton_pt>parton_pt_max){
            parton_pt_max=parton_pt;
            dr_min = dr;
            idx_genp_min = idx_gp;
            if(debug) cout<<"HERWIG++ gen jet and gen particle might be matched, status = "<<genp.status()<<" idx_genp_min = "<<idx_genp_min<<" dR="<<dr_min<<endl;
          }
        }
      }
      if(dr_min <= dr_cut && idx_genp_min>-1) {
        if(debug) cout << "genjet " << idx_j << " is matched to genparticle " << idx_genp_min <<" with status="<<event.genparticles->at(idx_genp_min).status()<< " and flavor " << event.genparticles->at(idx_genp_min).partonFlavour()
        << " and pt = "<<event.genparticles->at(idx_genp_min).pt()<< ", within dR = " << dr_min << ". " <<  endl;
        //	    idx_parton_genJet[idx_genp_min] = idx_j;
        if(idx_jet_matching_genjet[idx_j] >= 0) idx_matched_jets[idx_jet_matching_genjet[idx_j]] = idx_genp_min;
      }
    }

    // flavor-quantities

    if(debug && event.genjets->size() <2) cout << "WARNING: GENjets size < 2" << endl;

    //only consider the barreljet, is it leading or sub-leading jet?
    int idx_barreljet = -1;
    if(fabs(jet1->pt() - jet_barrel->pt()) < 0.001) idx_barreljet = 0;
    else if (fabs(jet2->pt() - jet_barrel->pt()) < 0.001) idx_barreljet = 1;
    else throw runtime_error("first two jets are not the barrel jets, how could this happen?");
    if(debug) cout<<"idx_matched_RecoGenjets[0] = "<<idx_matched_RecoGenjets[0]<<endl;
    if(debug) cout<<"idx_matched_RecoGenjets[1] = "<<idx_matched_RecoGenjets[1]<<endl;
    //obtain flavor of the barreljet
    //-1: unmatched, 0: alpha too large, >0: flavor of matching genparticle

    if(idx_matched_RecoGenjets[idx_barreljet] !=-1){//RECO to GEN
      barrelgenjet_pt   = event.genjets->at(idx_matched_RecoGenjets[idx_barreljet]).pt();
      barrelgenjet_eta  = event.genjets->at(idx_matched_RecoGenjets[idx_barreljet]).eta();
      barrelgenjet_phi  = event.genjets->at(idx_matched_RecoGenjets[idx_barreljet]).phi();
      if(debug) cout<<"idx_matched_RecoGenjets[idx_barreljet] = "<<idx_matched_RecoGenjets[idx_barreljet] << " " << barrelgenjet_pt << " " << barrelgenjet_eta <<  " " << barrelgenjet_phi <<endl;
      if(idx_matched_jets[idx_barreljet] != -1){//flavor
        flavor_barreljet      = fabs(event.genparticles->at(idx_matched_jets[idx_barreljet]).partonFlavour());
        response_barreljet    = jet_barrel->pt() / event.genparticles->at(idx_matched_jets[idx_barreljet]).pt();
        barreljet_ptptcl      = event.genparticles->at(idx_matched_jets[idx_barreljet]).pt();
        barreljet_statusptcl  = event.genparticles->at(idx_matched_jets[idx_barreljet]).status();
        dR_GenJet_GenParticle_barrel_matched = deltaR(event.genjets->at(idx_matched_RecoGenjets[idx_barreljet]),event.genparticles->at(idx_matched_jets[idx_barreljet]));
        if(debug) cout << "barreljet is jet no. " << idx_barreljet << " idx_matched_jets[idx_barreljet]="<<idx_matched_jets[idx_barreljet] << ", alpha = " << event.get(tt_alpha) << ", flavor of barreljet = " << flavor_barreljet << endl;
      }
    }

    //also for probe jets
    int idx_probejet = fabs(idx_barreljet - 1);
    if(debug) cout<<"idx_probejet = "<<idx_probejet <<" idx_matched_jets[idx_probejet] = "<<idx_matched_jets[idx_probejet] << " idx_matched_RecoGenjets[idx_probejet] = "<<idx_matched_RecoGenjets[idx_probejet]<<endl;

    //obtain flavor of the probejet
    //-1: unmatched,  >0: flavor of matching genparticle
    if(debug) cout << "probejet is jet no. " << idx_probejet << ", alpha = " << event.get(tt_alpha) << ", flavor of probejet = " << flavor_probejet << endl;

    if(idx_matched_RecoGenjets[idx_probejet] !=-1){//GEN and RECO jets are matched
      probegenjet_pt  = event.genjets->at(idx_matched_RecoGenjets[idx_probejet]).pt();
      probegenjet_eta = event.genjets->at(idx_matched_RecoGenjets[idx_probejet]).eta();
      probegenjet_phi = event.genjets->at(idx_matched_RecoGenjets[idx_probejet]).phi();
      if(debug) cout<<"idx_matched_RecoGenjets[idx_probejet] = "<<idx_matched_RecoGenjets[idx_probejet] << " " << probegenjet_pt << " " << probegenjet_eta <<  " " << probegenjet_phi <<endl;
      if(idx_matched_jets[idx_probejet] != -1){
        flavor_probejet     = fabs(event.genparticles->at(idx_matched_jets[idx_probejet]).partonFlavour());
        response_probejet   = jet_probe->pt() / event.genparticles->at(idx_matched_jets[idx_probejet]).pt();
        probejet_ptptcl     = event.genparticles->at(idx_matched_jets[idx_probejet]).pt();
        probejet_statusptcl = event.genparticles->at(idx_matched_jets[idx_probejet]).status();
        dR_GenJet_GenParticle_probe_matched = deltaR(event.genjets->at(idx_matched_RecoGenjets[idx_probejet]),event.genparticles->at(idx_matched_jets[idx_probejet]));
      }

      //	h_hadrons->fill(event,idx_matched_RecoGenjets[idx_probejet],idx_probejet);
      h_matched_all->fill(event,idx_matched_RecoGenjets[idx_probejet],idx_probejet);//event, gen_id, reco_id
      bool isStop=false;
      for(int jpt=0;jpt<n_pt-1;jpt++){
        if((event.genjets->at(idx_matched_RecoGenjets[idx_probejet]).pt())>pt_bins[jpt] && ((event.genjets->at(idx_matched_RecoGenjets[idx_probejet]).pt())<=pt_bins[jpt+1])){
          h_matched_pt[jpt]->fill(event,idx_matched_RecoGenjets[idx_probejet],idx_probejet);//event, gen_id, reco_id
          isStop = true;
        }
        if(isStop) break;
      }
    }

    // if (debug) std::cout << "MATCH GEN " << std::abs(TVector2::Phi_mpi_pi(barrelgenjet_phi - probegenjet_phi)) << " " << probegenjet_pt << " " << probegenjet_eta <<  " " << probegenjet_phi << " " << barrelgenjet_pt << " " << barrelgenjet_eta <<  " " << barrelgenjet_phi <<endl;
    // if (debug) std::cout << "MATCH jet " << std::abs(TVector2::Phi_mpi_pi(barreljet_phi - probejet_phi)) << " " << probejet_pt << " " << probejet_eta <<  " " << probejet_phi << " " << barreljet_pt << " " << barreljet_eta <<  " " << barreljet_phi <<endl;
    gen_asymmetry = (barrelgenjet_pt - probegenjet_pt)/(barrelgenjet_pt + probegenjet_pt);

    //Fill PUid related histograms
    for(int idx_curr=0;idx_curr<2;idx_curr++){//barrel and leading jets
      if(idx_matched_RecoGenjets[idx_curr] !=-1){//RECO to GEN
        h_goodRECO_all->fill(event,idx_curr);//event, reco_id
        bool isStop=false;
        for(int kpt=0;kpt<n_pt-1;kpt++){
          if((ak4jets->at(idx_curr).pt())>pt_bins[kpt] && ((ak4jets->at(idx_curr).pt())<=pt_bins[kpt+1])){
            h_goodRECO_pt[kpt]->fill(event,idx_curr);//event, reco_id
            isStop = true;
          }
          if(isStop) break;
        }
        if(idx_matched_jets[idx_matched_RecoGenjets[idx_curr]] !=-1) { //parton is matched
          h_goodpartonRECO_all->fill(event,idx_curr);//event, reco_id
          bool isStop=false;
          for(int kpt=0;kpt<n_pt-1;kpt++){
            if((ak4jets->at(idx_curr).pt())>pt_bins[kpt] && ((ak4jets->at(idx_curr).pt())<=pt_bins[kpt+1])){
              h_goodpartonRECO_pt[kpt]->fill(event,idx_curr);//event, reco_id
              isStop = true;
            }
            if(isStop) break;
          }
        } else{//parton is not matched
          h_badpartonRECO_all->fill(event,idx_curr);//event, reco_id
          bool isStop=false;
          for(int kpt=0;kpt<n_pt-1;kpt++){
            if((ak4jets->at(idx_curr).pt())>pt_bins[kpt] && ((ak4jets->at(idx_curr).pt())<=pt_bins[kpt+1])){
              h_badpartonRECO_pt[kpt]->fill(event,idx_curr);//event, reco_id
              isStop = true;
            }
            if(isStop) break;
          }
        }
      } else{//RECO and GEN are not matched
        h_badRECO_all->fill(event,idx_curr);//event, reco_id
        bool isStop=false;
        for(int kpt=0;kpt<n_pt-1;kpt++){
          if((ak4jets->at(idx_curr).pt())>pt_bins[kpt] && ((ak4jets->at(idx_curr).pt())<=pt_bins[kpt+1])){
            h_badRECO_pt[kpt]->fill(event,idx_curr);//event, reco_id
            isStop = true;
          }
          if(isStop) break;
        }
      }
    }
    if(jet_n>2){
      for(int idx_curr=2;idx_curr<3;idx_curr++){//3rd jets
        //	cout<<"Fill PU hists for 3rd jet"<<endl;
        if(idx_matched_RecoGenjets[idx_curr] !=-1){//RECO to GEN
          //	  cout<<"3rd jet IS matched"<<endl;
          h_goodRECO_3rd_all->fill(event,idx_curr);//event, reco_id
          bool isStop=false;
          for(int kpt=0;kpt<n_pt-1;kpt++){
            if((ak4jets->at(idx_curr).pt())>pt_bins[kpt] && ((ak4jets->at(idx_curr).pt())<=pt_bins[kpt+1])){
              h_goodRECO_3rd_pt[kpt]->fill(event,idx_curr);//event, reco_id
              isStop = true;
            }
            if(isStop) break;
          }
          if(idx_matched_jets[idx_matched_RecoGenjets[idx_curr]] !=-1){ //parton is matched
            h_goodpartonRECO_3rd_all->fill(event,idx_curr);//event, reco_id
            bool isStop=false;
            for(int kpt=0;kpt<n_pt-1;kpt++){
              if((ak4jets->at(idx_curr).pt())>pt_bins[kpt] && ((ak4jets->at(idx_curr).pt())<=pt_bins[kpt+1])){
                h_goodpartonRECO_3rd_pt[kpt]->fill(event,idx_curr);//event, reco_id
                isStop = true;
              }
              if(isStop) break;
            }
          } else{//parton is not matched
            h_badpartonRECO_3rd_all->fill(event,idx_curr);//event, reco_id
            bool isStop=false;
            for(int kpt=0;kpt<n_pt-1;kpt++){
              if((ak4jets->at(idx_curr).pt())>pt_bins[kpt] && ((ak4jets->at(idx_curr).pt())<=pt_bins[kpt+1])){
                h_badpartonRECO_3rd_pt[kpt]->fill(event,idx_curr);//event, reco_id
                isStop = true;
              }
              if(isStop) break;
            }
          }
        } else{//RECO_3rd and GEN are not matched
          //	  cout<<"3rd jet is not matched"<<endl;
          h_badRECO_3rd_all->fill(event,idx_curr);//event, reco_id
          bool isStop=false;
          for(int kpt=0;kpt<n_pt-1;kpt++){
            if((ak4jets->at(idx_curr).pt())>pt_bins[kpt] && ((ak4jets->at(idx_curr).pt())<=pt_bins[kpt+1])){
              h_badRECO_3rd_pt[kpt]->fill(event,idx_curr);//event, reco_id
              isStop = true;
            }
            if(isStop) break;
          }
        }
      }
    }

    //find the closest parton to the jet even if not matched (to check dR matching)
    //Code should be the same as matching before!
    // TODO merge with above?

    if(event.genparticles){
      int idx_gp = -1;
      double parton_pt_max=15;
      double dr_min = 1000.;
      //Probe jet
      if(idx_matched_RecoGenjets[idx_probejet]>-1){
        for(GenParticle & genp: *event.genparticles){
          idx_gp++;
          if(ispythia8){
            if(genp.status()!=23) continue;//Pythia8/Madgraph: only particle with status 23 are final state partons
            double dr = deltaR(event.genjets->at(idx_matched_RecoGenjets[idx_probejet]),genp);
            if(dr < dr_min){
              dr_min = dr;
              idx_genp_min = idx_gp;
              if(debug) cout<<"PYTHIA8 gen jet and gen particle might be matched, status = "<<genp.status()<<" idx_genp_min = "<<idx_genp_min<<" dR="<<dr_min<<endl;
            }
          } else{//Herwig, etc
            if(genp.status()!=11) continue;//Herwig++: only particle with status 11 can contain parton from ME we are interested in
            double parton_pt = genp.pt();
            double dr = deltaR(event.genjets->at(idx_matched_RecoGenjets[idx_probejet]),genp);
            if(dr <1.2 && dr < dr_min && parton_pt>parton_pt_max){
              parton_pt_max=parton_pt;
              dr_min = dr;
              idx_genp_min = idx_gp;
              if(debug) cout<<"HERWIG++ gen jet and gen particle might be matched, status = "<<genp.status()<<" idx_genp_min = "<<idx_genp_min<<" dR="<<dr_min<<endl;
            }
          }
        }
        probejet_drminparton = dr_min;
      }

      idx_gp = -1;
      dr_min = 1000.; parton_pt_max=15;

      //Tag jet
      if(idx_matched_RecoGenjets[idx_barreljet]>-1){
        for(GenParticle & genp: *event.genparticles){
          idx_gp++;
          if(ispythia8){
            if(genp.status()!=23) continue;//Pythia8/Madgraph: only particle with status 23 are final state partons
            double dr = deltaR(event.genjets->at(idx_matched_RecoGenjets[idx_barreljet]),genp);
            if(dr < dr_min){
              dr_min = dr;
              idx_genp_min = idx_gp;
              if(debug) cout<<"PYTHIA8 gen jet and gen particle might be matched, status = "<<genp.status()<<" idx_genp_min = "<<idx_genp_min<<" dR="<<dr_min<<endl;
            }
          }
          else{//Herwig, etc
            if(genp.status()!=11) continue;//Herwig++: only particle with status 11 can contain parton from ME we are interested in
            double parton_pt = genp.pt();
            double dr = deltaR(event.genjets->at(idx_matched_RecoGenjets[idx_barreljet]),genp);
            if(dr <1.2 && dr < dr_min && parton_pt>parton_pt_max){
              parton_pt_max=parton_pt;
              dr_min = dr;
              idx_genp_min = idx_gp;
              if(debug) cout<<"HERWIG++ gen jet and gen particle might be matched, status = "<<genp.status()<<" idx_genp_min = "<<idx_genp_min<<" dR="<<dr_min<<endl;
            }
          }
        }
        barreljet_drminparton = dr_min;
      }
    }


    if(debug) {
      cout << "barreljet is jet no. " << idx_barreljet << ", alpha = " << event.get(tt_alpha) << ", flavor of barreljet = " << flavor_barreljet <<", status of barrel jet = "<<barreljet_statusptcl<<" the closest parton with dR = "<<barreljet_drminparton<< endl;
      cout << "probejet is jet no. " << idx_probejet << ", alpha = " << event.get(tt_alpha) << ", flavor of probejet = " << flavor_probejet <<" status of probejet = " << probejet_statusptcl <<" the closest parton with dR = "<<probejet_drminparton<< endl;
      if(probejet_drminparton>1000. || barreljet_drminparton>1000.) cout<<"AAAAAAAAAAAAAAAAA! LOOK!"<<endl;
    }

    //same for leading jet
    //-1: unmatched, 0: alpha too large, >0: flavor of matching genparticle
    if(idx_matched_jets[0] != -1) flavor_leadingjet = fabs(event.genparticles->at(idx_matched_jets[0]).partonFlavour());
    if(debug) cout << "leadingjet is jet no. " << 0 << ", alpha = " << event.get(tt_alpha) << ", flavor of leadingjet = " << flavor_leadingjet << endl;


    //same for subleading jet
    //-1: unmatched, 0: alpha too large, >0: flavor of matching genparticle
    if(idx_matched_jets[1] != -1) flavor_subleadingjet = fabs(event.genparticles->at(idx_matched_jets[1]).partonFlavour());
    if(debug) cout << "subleadingjet is jet no. " << 1 << ", alpha = " << event.get(tt_alpha) << ", flavor of subleadingjet = " << flavor_subleadingjet << endl;

    //same for 3rd jet
    //-1: unmatched, 0: alpha too large, >0: flavor of matching genparticle
    if(jet_n>2){
      if(idx_matched_jets[2] != -1) {
        flavor_3rdjet = fabs(event.genparticles->at(idx_matched_jets[2]).partonFlavour());
        jet3_statusptcl = event.genparticles->at(idx_matched_jets[2]).status();
        jet3_ptptcl = event.genparticles->at(idx_matched_jets[2]).pt();
        response3rdjet = (&ak4jets->at(2))->pt() / event.genparticles->at(idx_matched_jets[2]).pt();
      }
      if(debug) cout << "3rd is jet no. " << idx_matched_jets[2] << ", alpha = " << event.get(tt_alpha) << ", flavor of the 3rd jet = " << flavor_3rdjet <<", status = "<<jet3_statusptcl<< endl;
    }

    //response of leading jet
    //find corresponding genjet
    int idx_corresponding_genjet = -1;
    int idx_corresponding_genjet2 = -1;
    for(int i=0; i<genjet_n; i++){
      if(debug) cout << idx_jet_matching_genjet[i] << endl;
      if(idx_jet_matching_genjet[i] == 0) idx_corresponding_genjet = i;
      if(idx_jet_matching_genjet[i] == 1) idx_corresponding_genjet2 = i;
    }

    if(idx_corresponding_genjet != -1) response_jet1 = ak4jets->at(0).pt() / event.genjets->at(idx_corresponding_genjet).pt();
    if(idx_corresponding_genjet2 != -1) response_jet2 = ak4jets->at(1).pt() / event.genjets->at(idx_corresponding_genjet2).pt();


    for(int idgen=0; idgen<genjet_n; idgen++){
      if (idx_jet_matching_genjet[idgen]==0) jet1_genID = idgen;
      if (idx_jet_matching_genjet[idgen]==1) jet2_genID = idgen;
      if (idx_jet_matching_genjet[idgen]==2) jet3_genID = idgen;
      if (idx_jet_matching_genjet[idgen]==3) jet4_genID = idgen;
      if (idx_jet_matching_genjet[idgen]==4) jet5_genID = idgen;
      if (idx_jet_matching_genjet[idgen]==5) jet6_genID = idgen;
    }

    if(debug) cout<<"N genjets: "<<event.genjets->size()<<endl;
    if(event.genparticles){
      for(GenParticle & genp: *event.genparticles){
        //      if(genp.status()<2) continue;//TEST it does not make sense to look at status 1 particles, since we know nothing about them
        if(genp.pt()>0) genptcl_n++;
      }
    }
    if(debug) cout<<"Number of gen particles with pt>0: "<< genptcl_n<<endl;
    //calculate B from GEN jets and MET
    TVector2 ptgen, genmet;
    if(event.genmet) genmet.Set(event.genmet->pt() * cos(event.genmet->phi()),event.genmet->pt() * sin(event.genmet->phi()));
    else{
      if(debug) cout<<" event.genmet is not availble! "<<endl;
    }
    ptgen.Set(barrelgenjet_pt * cos(barrelgenjet_phi),barrelgenjet_pt* sin(barrelgenjet_phi));
    Bgen = (genmet.Px()*ptgen.Px() + genmet.Py()*ptgen.Py())/((probegenjet_pt + barrelgenjet_pt) * sqrt(ptgen.Px()*ptgen.Px() + ptgen.Py()*ptgen.Py())); //vec_MET*vec_ptbarr/(2ptave*ptbarr)
    genMet_pt = (event.genmet) ? event.genmet->pt() : -1;
    if(debug){
      if(event.genmet){
        cout<<" genMET: pt = "<<event.genmet->pt()<<endl;
        genmet.Print();
      }
      cout<<" ptGen: "<<endl;
      ptgen.Print();
    }
    if(debug) cout<<" B_gen = "<<Bgen<<" B_reco = "<<event.get(tt_B)<<endl;

  } //isMC
  else{//data
    for(unsigned int idx_probejet=0; idx_probejet<2; idx_probejet++){
      h_dataRECO_all->fill(event,idx_probejet);//event, reco_id
      bool isStop=false;
      for(int kpt=0;kpt<n_pt-1;kpt++){
        if((ak4jets->at(idx_probejet).pt())>pt_bins[kpt] && ((ak4jets->at(idx_probejet).pt())<=pt_bins[kpt+1])){
          h_dataRECO_pt[kpt]->fill(event,idx_probejet);//event, reco_id
          isStop = true;
        }
        if(isStop) break;
      }
    }

    if(ak4jets->size()>2){
      h_dataRECO_3rd_all->fill(event,2);//event, reco_id
      bool isStop=false;
      for(int kpt=0;kpt<n_pt-1;kpt++){
        if((ak4jets->at(2).pt())>pt_bins[kpt] && ((ak4jets->at(2).pt())<=pt_bins[kpt+1])){
          h_dataRECO_3rd_pt[kpt]->fill(event,2);//event, reco_id
          isStop = true;
        }
        if(isStop) break;
      }
    }
  }

  event.set(tt_genptcl_n,genptcl_n);
  event.set(tt_Bgen,Bgen);
  event.set(tt_genMET,genMet_pt);
  event.set(tt_flavor3rdjet,flavor_3rdjet);
  event.set(tt_flavorBarreljet,flavor_barreljet);
  event.set(tt_responseBarreljet,response_barreljet);

  event.set(tt_barreljet_ptptcl,barreljet_ptptcl);
  event.set(tt_flavorProbejet,flavor_probejet);
  event.set(tt_responseProbejet,response_probejet);
  event.set(tt_flavorLeadingjet,flavor_leadingjet);
  event.set(tt_flavorSubleadingjet,flavor_subleadingjet);

  event.set(tt_probegenjet_pt,probegenjet_pt);
  event.set(tt_probegenjet_eta,probegenjet_eta);
  event.set(tt_probegenjet_phi,probegenjet_phi);
  event.set(tt_probejet_ptptcl,probejet_ptptcl);
  event.set(tt_barrelgenjet_pt,barrelgenjet_pt);
  event.set(tt_barrelgenjet_phi,barrelgenjet_phi);
  event.set(tt_barrelgenjet_eta,barrelgenjet_eta);
  event.set(tt_gen_asymmetry,gen_asymmetry);

  event.set(tt_barreljet_statusptcl,barreljet_statusptcl);
  event.set(tt_probejet_statusptcl,probejet_statusptcl);
  event.set(tt_jet3_statusptcl,jet3_statusptcl);
  event.set(tt_jet3_ptptcl,jet3_ptptcl);
  event.set(tt_response3rdjet,response3rdjet);
  event.set(tt_probejet_dRminParton,probejet_drminparton);
  event.set(tt_barreljet_dRminParton,barreljet_drminparton);

  event.set(tt_response_leadingjet,response_jet1);
  event.set(tt_response_Subleadingjet,response_jet2);
  event.set(tt_dR_GenJet_GenParticle_barrel_matched, dR_GenJet_GenParticle_barrel_matched);
  event.set(tt_dR_GenJet_GenParticle_probe_matched, dR_GenJet_GenParticle_probe_matched);
  event.set(tt_jet1_genID, jet1_genID);
  event.set(tt_jet2_genID, jet2_genID);
  event.set(tt_jet3_genID, jet3_genID);
  event.set(tt_jet4_genID, jet4_genID);
  event.set(tt_jet5_genID, jet5_genID);
  event.set(tt_jet6_genID, jet6_genID);


  if(debug) {
    cout<<"After full Selection!! Njet="<<jet_n<<endl;
    cout << " Evt# "<<event.event<<" Run: "<<event.run<<" " << endl<<endl;
    cout<<"THIS IS THE END"<<endl;
  }
  return true;
}

// as we want to run the ExampleCycleNew directly with AnalysisModuleRunner,
// make sure the ExampleModule is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(AnalysisModule_DiJetTrg)
