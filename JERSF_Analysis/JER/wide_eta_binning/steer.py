import sys
import os
import time
import numpy as np

sys.path.append("/nfs/dust/cms/user/matthies/AndreaMalara/PersonalCode/")
from parallelise import *

def getLabel(sample):
    if sample == "A":
        LABEL_LUMI_INV_FB = "[MC 102X] Run2018A 14.00 fb^{-1}"
    elif sample == "B":
        LABEL_LUMI_INV_FB = "[MC 102X] Run2018B 7.10 fb^{-1}"
    elif sample == "C":
        LABEL_LUMI_INV_FB = "[MC 102X] Run2018C 6.94 fb^{-1}"
    elif sample == "D":
        LABEL_LUMI_INV_FB = "[MC 102X] Run2018D 31.93 fb^{-1}"
    elif sample == "ABC":
        LABEL_LUMI_INV_FB = "[MC 102X] Run2018 28.04 fb^{-1}"
    elif sample == "ABCD":
        LABEL_LUMI_INV_FB = "[MC 102X] Run2018 59.97 fb^{-1}"
    else:
        LABEL_LUMI_INV_FB = "[MC 102X] (2018)"
    return LABEL_LUMI_INV_FB


def main_function(gaustails=False, shiftForPLI="central", gaustail_num = 0.985):
    outdir = out_path+pattern+QCDsample+"/"+run+"/"
    shiftForPLI_num = 0.0
    ref_shift = 3
    if "barrel_check" in extraText:
        ref_shift = int(extraText[-2])
    if gaustails:
        outdir = out_path+newJECVersion+"/"+newJetLabel+"/gaustails_"+str(gaustail_num)+"/"+QCDsample+"/"+run+"/"
    if shiftForPLI=="up":
        outdir = out_path+newJECVersion+"/"+newJetLabel+"/PLI/up/"+QCDsample+"/"+run+"/"
        shiftForPLI_num = 0.25
    if shiftForPLI=="down":
        outdir = out_path+newJECVersion+"/"+newJetLabel+"/PLI/down/"+QCDsample+"/"+run+"/"
        shiftForPLI_num = -0.25
    print "outdir ", outdir
    if os.path.isdir(outdir):
        for el in sorted(os.listdir(outdir)):
            cmd = "rm -fr %s" % (outdir+el)
            a = os.system(cmd)
    else:
        os.makedirs(outdir)
    programm ="mainRun"
    # if "AK8" in outdir: programm += "AK8"
    cmd = "cp %s.cxx %s" % (programm, outdir)
    a = os.system(cmd)
    cmd = "cp functions.C %s" % (outdir)
    a = os.system(cmd)
    cmd = "cp /nfs/dust/cms/user/matthies/AndreaMalara/PersonalCode/tdrstyle_all.C %s" % (outdir)
    a = os.system(cmd)
    os.chdir(outdir)
    os.makedirs("pdfy")
    os.makedirs("pdfy/MCTruth")
    os.makedirs("pdfy/SFs")
    os.makedirs("pdfy/NSC_SFs")
    os.makedirs("pdfy/JERs")
    os.makedirs("pdfy/widths")
    os.makedirs("pdfy/maps")
    os.makedirs("ClosureTest")
    os.makedirs("output")
    os.makedirs("output/asymmetries")
    # print pattern+run
    temp_time=time.time()
    # f = open("log.txt",'w')
    MC_type = '\\"MC\\"'
    data_type = '\\"Data\\"'
    trigger_type = '\\"'+study[:-1]+'\\"'
    # cmd = 'root -l -b -q "%s%s.cxx(false, %s, %s, %s, %s , %s, %s, %s, %s, %s, %s)" >> log.txt &' % (outdir, programm, MC_file, Data_file, LABEL_LUMI_INV_FB, MC_type, data_type, trigger_type, '\\"'+outdir+'\\"', gaustail_num, shiftForPLI_num, ref_shift)
    # cmd = 'root -l -b -q "%s%s.cxx(false, %s, %s, %s, %s , %s, %s, %s, %s, %s, %s)" >> log.txt  ' % (outdir, programm, MC_file, Data_file, LABEL_LUMI_INV_FB, MC_type, data_type, trigger_type, '\\"'+outdir+'\\"', gaustail_num, shiftForPLI_num, ref_shift)
    cmd = 'root -l -b -q "%s%s.cxx(false, %s, %s, %s, %s , %s, %s, %s, %s, %s, %s)"' % (outdir, programm, MC_file, Data_file, LABEL_LUMI_INV_FB, MC_type, data_type, trigger_type, '\\"'+outdir+'\\"', gaustail_num, shiftForPLI_num, ref_shift)
    print "cmd", cmd
    a = os.system(cmd)
    print ("time needed: "+str((time.time()-temp_time))+" s")
    os.chdir(common_path)




source_path = "/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/UHH2/DiJetJERC/JERSF_Analysis/hist_preparation/"
common_path = "/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/UHH2/DiJetJERC/JERSF_Analysis/JER/wide_eta_binning/"


samples = ["A", "B", "C", "D", "ABC", "ABCD"]
samples = ["D", "ABC", "ABCD"]
samples = ["D"]
#QCDSamples = ["QCD_Flat", "QCD_Flat2018"]
QCDSamples = ["QCDHT"]
JetLabels=["AK4CHS", "AK8Puppi", "AK4Puppi"]
JetLabels=["AK8Puppi"]
JECVersions=["Autumn18_V19"]
dirs = ["", "up", "down"]
studies = ["MergeL2Res/"]
# systematics=["", "PU", "JEC", "alpha", "JER"]
systematics=["", "PU", "JEC", "alpha"]
systematics=[""]
#systematics=["", "JEC"]


for extraText in [""]:
    for study in studies:
        out_path  = common_path+"file/"+study+extraText
        for newJECVersion in JECVersions:
            for newJetLabel in JetLabels:
                for syst in systematics:
                    for dir in dirs:
                        if syst == "JER" and dir != "":
                          continue
                        if syst == "JER" and dir == "":
                          dir = "nominal"
                        if (syst == "" and dir != "") or (syst == "alpha" and dir != "") or ((syst != "" and syst != "alpha") and dir == ""):
                            continue
                        pattern = newJECVersion+"/"+newJetLabel+"/"+syst+"/"+dir+"/"
                        if syst == "":
                            pattern = newJECVersion+"/"+newJetLabel+"/standard/"
                        if syst == "alpha":
                            pattern = newJECVersion+"/"+newJetLabel+"/"+syst+"/"
                        print "pattern ", pattern
                        for QCDsample in QCDSamples:
                            for sample in samples:
                                run = "Run"+sample
                                LABEL_LUMI_INV_FB=getLabel(sample)
                                LABEL_LUMI_INV_FB = '\\"'+LABEL_LUMI_INV_FB+'\\"'
                                MC_file   = '\\"'+source_path+"MC/wide_eta_bin/file/"+study+pattern.replace("/standard","")+QCDsample+extraText+"/histograms_mc_incl_full.root"+'\\"'
                                Data_file = '\\"'+source_path+"data/wide_eta_bin/file/"+study+pattern.replace("/standard","")+run+extraText+"/histograms_data_incl_full.root"+'\\"'
                                print MC_file, Data_file
                                if not os.path.isfile(str(MC_file.replace("\\","").strip("\""))) or not os.path.isfile(str(Data_file.replace("\\","").strip("\""))):
                                    continue
                                # print MC_file, Data_file
                                main_function(gaustails=False)
                                # if syst == "":
                                  # main_function(gaustails=False, shiftForPLI="up")
                                  # main_function(gaustails=False, shiftForPLI="down")
                                  # main_function(gaustails=True, shiftForPLI="central")
                                  # main_function(gaustails=True, shiftForPLI="central", gaustail_num = 0.95)
                                    ## for gaustail_num in np.arange(0.8,1.0,0.005):
                                    ##    main_function(gaustails=True, shiftForPLI="central", gaustail_num=gaustail_num)
