import sys
import os
import time

sys.path.append("/nfs/dust/cms/user/matthies/AndreaMalara/PersonalCode/")
from parallelise import *

def main_program(path="", list_path="", out_path="", JECVersions=[], JetLabels=[], systematics=[], samples=[], barrel_check = 0):
  list_path_=list_path[:-1]+path[path.find(inputdir)+len(inputdir):path.find("DATA")-1]+"/"
  out_path_=out_path[:-1]+path[path.find(inputdir)+len(inputdir):path.find("DATA")-1]+"/"
  for newJECVersion in JECVersions:
    for newJetLabel in JetLabels:
      for sys in set(systematics):
        if sys == "alpha":
          alpha_cut = 10
        else:
          alpha_cut = 15
        for dir in ["", "up", "down"]:
          if sys == "JER" and dir != "":
            continue
          if sys == "JER" and dir == "":
            dir = "nominal"
            print sys, dir
          if (sys == "" and dir != "") or (sys == "alpha" and dir != "") or ((sys != "" and sys != "alpha") and dir == ""):
            continue
          pattern = newJECVersion+"/"+newJetLabel+"/"+sys+"/"+dir+"/"
          if sys == "" or sys == "alpha":
            pattern = newJECVersion+"/"+newJetLabel+"/"
          source_path = path+pattern
          if not os.path.isdir(source_path):
            continue
          if sys == "alpha":
            pattern = newJECVersion+"/"+newJetLabel+"/"+sys+"/"
          if not os.path.isdir(list_path_+pattern):
            os.makedirs(list_path_+pattern)
          for sample in samples:
            run_list = list_path_+pattern+"file_DataRun"+sample+".txt"
            with open(run_list, "w") as outputfile:
              for writable in sorted(os.listdir(source_path)):
                if not os.path.isfile(source_path+writable):
                  continue
                if not "ECAL" in sample and "ECAL" in writable:
                  continue
                for el in sample:
                  if "ECAL" in sample:
                    el = sample
                  if "Run"+el in writable and ".root" in writable:
                    outputfile.write(source_path+writable+"\n")
                  if "ECAL" in sample:
                    break
            if not os.path.isfile(run_list):
              continue
            outdir = out_path_+pattern+"Run"+sample+"/"
            if not os.path.isdir(outdir):
              os.makedirs(outdir)
            print "RUNNING ON ", run_list
            temp_time=time.time()
            # cmd = "cp MySelector_full_Single.C MySelector.C"
            cmd = "cp MySelector_full_DiJet.C MySelector.C"
            a = os.system(cmd)
            cmd = 'sed -i -e """s/jet_thr=15/jet_thr=%s/g" MySelector.C' % (alpha_cut)
            a = os.system(cmd)
            if study == "LowPtJets":
              cmd = 'sed -i -e """s/pt_bins_Si/pt_bins_MB/g" MySelector.C'
              a = os.system(cmd)
            if barrel_check>0:
              if barrel_check == 1:
                cmd = 'sed -i -e """s/s_eta_barr/0.522/g" MySelector.C'
                outdir = out_path_+pattern+"Run"+sample+"_barrel_check_1/"
              if barrel_check == 2:
                cmd = 'sed -i -e """s/s_eta_barr/0.783/g" MySelector.C'
                outdir = out_path_+pattern+"Run"+sample+"_barrel_check_2/"
              if barrel_check == 3:
                cmd = 'sed -i -e """s/s_eta_barr/1.131/g" MySelector.C'
                outdir = out_path_+pattern+"Run"+sample+"_barrel_check_3/"
              a = os.system(cmd)
              if not os.path.isdir(outdir):
                os.makedirs(outdir)
            cmd = "cp Makefile %s" % (outdir)
            a = os.system(cmd)
            cmd = "cp *.C %s" % (outdir)
            a = os.system(cmd)
            cmd = "cp *.h %s" % (outdir)
            a = os.system(cmd)
            os.chdir(outdir)
            time.sleep(3)
            cmd = "make clean"
            a = os.system(cmd)
            cmd = "make"
            a = os.system(cmd)
            logfilename = "log.txt"
            f = open(logfilename,'w')
            cmd = './Analysis.x %s >> log.txt &' % (run_list)
            command = [outdir+"Analysis.x", run_list, outdir]
            list_processes.append(command)
            list_logfiles.append(outdir+"log.txt")
            f.close()
            os.chdir(common_path+"wide_eta_bin/")
            print ("time needed: "+str((time.time()-temp_time))+" s")


common_path = "/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/UHH2/DiJetJERC/JERSF_Analysis/hist_preparation/data/"
study = "StandardPtBins"
study = "MergeL2Res"

list_path   = common_path+"lists/"+study+"/"
out_path    = common_path+"wide_eta_bin/file/"+study+"/"
os.chdir(common_path+"wide_eta_bin/")

inputdir = "DiJetJERC_DiJetHLT"

sframe_ = "/nfs/dust/cms/user/matthies/102X/sframe_all/"

list_processes = []
list_logfiles = []
for el in [""]:
  path = sframe_+inputdir+el+"_DATA/"
  samples = ["A", "B", "C", "D", "ABC", "ABCD"]
  samples = ["ABC", "D", "ABCD"]
  # samples = ["ABC"]
  # samples = ["A"]
  JECVersions = ["Autumn18_V19"]
  JetLabels = ["AK4CHS", "AK8Puppi", "AK4Puppi"]
  # systematics = ["", "alpha","PU", "JEC", "JER"]
  systematics = ["", "alpha","PU", "JEC"]
  # systematics = [""]
  main_program(path, list_path, out_path, JECVersions, JetLabels, systematics, samples)

print len(list_processes)

for i in list_processes:
  print i

parallelise(list_processes, 10, list_logfiles)
