#!/usr/bin/env python

from createConfigFiles import *

def cont_event(paths ="./submittedJobs/" , JECVersions_Data=["Autumn18_V4"], JetLabels=["AK4CHS"], systematics=["", "PU", "JEC", "JER"],extratext=""):
    count = 0
    for newJECVersion in JECVersions_Data:
        for newJetLabel in JetLabels:
            for sys in systematics:
                for dir in ["", "up", "down"]:
                    if sys == "" and dir != "":
                        continue
                    if sys == "JER" and dir != "":
                        continue
                    if sys == "JER" and dir == "":
                        dir = "nominal"
                    path = paths+newJECVersion+"/"+newJetLabel+extratext+"/"+sys+"/"+dir+"/"
                    for sample in sorted(os.listdir(path)):
                        if not ".xml" in sample:
                            continue
                        count += 1
    return count

@timeit
def condor_control(original_dir ="./SubmittedJobs/" , JECVersions_Data=["Autumn18_V4"], JetLabels=["AK4CHS"], systematics=["", "PU", "JEC", "JER"], internal_option="-l", processes=[], extratext=""):
    count = 0
    for newJECVersion in JECVersions_Data:
        for newJetLabel in JetLabels:
            for sys in systematics:
                for dir in ["", "up", "down"]:
                    if sys == "" and dir != "":
                        continue
                    if sys == "JER" and dir != "":
                        continue
                    if sys == "JER" and dir == "":
                        dir = "nominal"
                    path = original_dir+newJECVersion+"/"+newJetLabel+extratext+"/"+sys+"/"+dir+"/"
                    for sample in sorted(os.listdir(path)):
                        if not ".xml" in sample:
                            continue
 			if all(not control in sample for control in processes): continue
                        count += 1
                        all_events = cont_event(original_dir, JECVersions_Data, JetLabels, systematics,extratext)
                        print "Already completed "+str(count)+" out of "+str(all_events)+" jobs --> "+str(float(count)/float(all_events)*100)+"%."
                        os.chdir(original_dir)
                        os.chdir(path)
                        if internal_option:
                            command = ['sframe_batch.py', internal_option, sample]
                        else:
                            command = ['sframe_batch.py', sample]
                        process = subprocess.Popen(command)
                        process.wait()
                        if internal_option == "-a":
                            time.sleep(5)
                        os.chdir(original_dir)


from createConfigFiles import *
@timeit
def delete_workdir(original_dir ="./SubmittedJobs/" , JECVersions_Data=["Autumn18_V4", "Autumn18_V4"], JetLabels=["AK4CHS", "AK8Puppi"], systematics=["", "PU", "JEC", "JER"],extratext=""):
    add_name = original_dir[original_dir.find("SubmittedJobs")+len("SubmittedJobs"):-1]
    for sample in ["DATA", "QCD"]:
        for newJECVersion in JECVersions_Data:
            for newJetLabel in JetLabels:
                for sys in systematics:
                    for dir in ["", "up", "down"]:
                        if sys == "" and dir != "":
                            continue
                        if sys == "JER" and dir != "":
                            continue
                    	if sys == "JER" and dir == "":
                       	    dir = "nominal"
                        path = userPathSframeOutput+outdir+add_name+"_"+sample+"/"+newJECVersion+"/"+newJetLabel+extratext+"/"+sys+"/"+dir+"/"
                        if os.path.isdir(path):
                            for workdir in sorted(os.listdir(path)):
                                if "workdir" in workdir:
                                    cmd = "rm -fr %s" % (path+workdir)
				    print cmd
                                    a = os.system(cmd)
                        path = original_dir+newJECVersion+"/"+newJetLabel+extratext+"/"+sys+"/"+dir+"/"
                        if os.path.isdir(path):
                            for workdir in sorted(os.listdir(path)):
                                if "workdir" in workdir:
                                    cmd = "rm -fr %s" % (path+workdir)
				    print cmd
                                    a = os.system(cmd)




def main_program(option="", internal_option="", processes=[], JECVersions_Data=[], JECVersions_MC=[], JetLabels=[], systematics=[], original_dir="./SubmittedJobs/", original_file="JER2018.xml", isMB=False, test_trigger=False, isThreshold=False, isLowPt=False, isL1Seed=False, isECAL=False, extratext=""):
    if option == "new":
        createConfigFiles(processes, JECVersions_Data, JECVersions_MC, JetLabels, systematics, original_dir, original_file, outdir, isMB, test_trigger, isThreshold,isLowPt,isL1Seed,isECAL,extratext)
    elif option == "remove" or option == "delete":
        delete_workdir(original_dir, JECVersions_Data, JetLabels, systematics, extratext)
    else:
        condor_control(original_dir, JECVersions_Data, JetLabels, systematics, internal_option, processes, extratext)



##################################################
#                                                #
#                   MAIN Program                 #
#                                                #
##################################################

try:
    option = sys.argv[1]
except:
    option = ""

if option == "resubmit":
    internal_option = "-r"
elif option == "submit":
    internal_option = "-s"
elif option == "add" or option == "merge":
    internal_option = "-f"
elif option == "list":
    internal_option = "-l"
elif option == "new":
    internal_option = ""
elif option == "remove" or option == "delete":
    internal_option = ""
else:
    internal_option = ""


QCD_process= []
QCD_process.append("QCD_Flat2018")
QCD_process.append("QCD_Flat")

QCD_process.append("QCDHT50to100")
QCD_process.append("QCDHT100to200")
QCD_process.append("QCDHT200to300")
QCD_process.append("QCDHT300to500")
QCD_process.append("QCDHT500to700")
QCD_process.append("QCDHT700to1000")
QCD_process.append("QCDHT1000to1500")
QCD_process.append("QCDHT1500to2000")
QCD_process.append("QCDHT2000toInf")

Data_process= []
Data_process.append("DATA_RunA_2018")
Data_process.append("DATA_RunB_2018")
Data_process.append("DATA_RunC_2018")
Data_process.append("DATA_RunD_2018")

processes = QCD_process+Data_process

# JECVersions_Data = ["Autumn18_V4"]
# JetLabels = ["AK4CHS", "AK8Puppi"]
# systematics = ["", "PU", "JEC", "JER"]

userPathSframeOutput="/nfs/dust/cms/user/matthies/102X/sframe_all/"
original_file = "DiJetJERC_DiJetHLT.xml"
outdir = "DiJetJERC_DiJetHLT"
original_dir_ = os.getcwd()

JECVersions_Data = ["Autumn18_V19"]
JECVersions_MC   = ["Autumn18_V19"]
JetLabels = ["AK4CHS","AK8Puppi", "AK4Puppi"]
JetLabels = ["AK4CHS"]
systematics = ["", "PU", "JEC", "JER"]
systematics = ["", "PU", "JEC"]
# systematics = [""]

isLowPt = False
isMB = False
test_trigger = False
isThreshold = False
isL1Seed = False
isECAL = False
extratext = ""
original_dir = original_dir_
original_dir += "/SubmittedJobs/"
main_program(option, internal_option, processes, JECVersions_Data, JECVersions_MC, JetLabels, systematics, original_dir, original_file, isMB=isMB, test_trigger=test_trigger, isThreshold=isThreshold, isLowPt=isLowPt, isL1Seed=isL1Seed, isECAL=isECAL, extratext=extratext)

