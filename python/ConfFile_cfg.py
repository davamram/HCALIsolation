import FWCore.ParameterSet.Config as cms
import subprocess

# command = 'xrdfs root://cmsxrootd.fnal.gov/ ls /store/mc/Run3Winter22DR/DYToLL_M-50_TuneCP5_13p6TeV-pythia8/GEN-SIM-RECO/L1TPU0to99FEVT_122X_mcRun3_2021_realistic_v9-v2/2830000/'
# file_list = subprocess.check_output(command, shell=True, text=True).strip().split('\n')
with open("listMC.txt", "r") as f:
    file_list = f.readlines()
file_list_root = [f'root://cmsxrootd.fnal.gov/{file}' for file in file_list]
file_list_reduced = file_list_root

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.Geometry.GeometryExtended2021Reco_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')

process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False),
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
                                # replace 'myfile.root' with the source file you want to use
                                fileNames = cms.untracked.vstring(
            # 'root://cmsxrootd.fnal.gov//store/mc/Run3Summer22EEMiniAODv3/DYto2E_M-50_NNPDF31_TuneCP5_13p6TeV-powheg-pythia8/MINIAODSIM/124X_mcRun3_2022_realistic_postEE_v1-v3/2810000/01ce9762-375b-48c7-95e2-aeb0bc814de7.root',
            # 'root://cmsxrootd.fnal.gov//store/mc/Run3Summer22EEMiniAODv3/DYto2E_M-50_NNPDF31_TuneCP5_13p6TeV-powheg-pythia8/MINIAODSIM/124X_mcRun3_2022_realistic_postEE_v1-v3/2810000/004716f9-0288-41fe-b342-d3372331a3cb.root',
            # 'root://cmsxrootd.fnal.gov//store/mc/Run3Summer22DRPremix/DYJetsToLL_M-50_TuneCP5_13p6TeV-madgraphMLM-pythia8/AODSIM/124X_mcRun3_2022_realistic_v12-v2/30000/c5c81d33-b5da-4b23-a1ba-ccff81b9820d.root'
            #'root://cmsxrootd.fnal.gov//store/mc/Run3Summer22EEMiniAODv3/GJet_PT-40_DoubleEMEnriched_MGG-80_TuneCP5_13p6TeV_pythia8/MINIAODSIM/124X_mcRun3_2022_realistic_postEE_v1-v2/2540000/014da2df-dad1-4d23-8042-9880b61a9f0d.root'
            #'root://cmsxrootd.fnal.gov//store/mc/Run3Winter24Reco/DYJetsToLL_M-50_TuneCP5_13p6TeV-madgraphMLM-pythia8/GEN-SIM-RECO/NoPU_Pilot_133X_mcRun3_2024_realistic_v6-v2/50000/7ee999b2-ade6-4158-b40b-b4aa9dbf26d5.root'
            #'file:/gridgroup/cms/amram/EPR/CMSSW_12_4_15/src/ConeOptimisation/ConeOptimisation/data/RECO/7ee999b2-ade6-4158-b40b-b4aa9dbf26d5.root'
            # 'root://cmsxrootd.fnal.gov//store/mc/Run3Winter22DR/DYToLL_M-50_TuneCP5_13p6TeV-pythia8/GEN-SIM-RECO/L1TPU0to99FEVT_122X_mcRun3_2021_realistic_v9-v2/2830000/*.root',
            # 'root://cmsxrootd.fnal.gov//store/mc/Run3Winter22DR/DYToLL_M-50_TuneCP5_13p6TeV-pythia8/GEN-SIM-RECO/L1TPU0to99FEVT_122X_mcRun3_2021_realistic_v9-v2/2830000/04e68ee9-6bb9-49de-9bbd-cadc2f57ed75.root',
            # 'root://cmsxrootd.fnal.gov//store/mc/Run3Winter22DR/DYToLL_M-50_TuneCP5_13p6TeV-pythia8/GEN-SIM-RECO/L1TPU0to99FEVT_122X_mcRun3_2021_realistic_v9-v2/2830000/068063d7-73f2-48ad-b6a9-d6ce24af7da2.root',
            # 'root://cmsxrootd.fnal.gov//store/mc/Run3Winter22DR/DYToLL_M-50_TuneCP5_13p6TeV-pythia8/GEN-SIM-RECO/L1TPU0to99FEVT_122X_mcRun3_2021_realistic_v9-v2/2830000/06b02f02-1bd5-469b-9f7f-48b0cf7147be.root',
            # 'root://cmsxrootd.fnal.gov//store/mc/Run3Winter22DR/DYToLL_M-50_TuneCP5_13p6TeV-pythia8/GEN-SIM-RECO/L1TPU0to99FEVT_122X_mcRun3_2021_realistic_v9-v2/2830000/07f5f6ff-1e57-42d4-ba4d-711f8f1fd669.root',
            # 'root://cmsxrootd.fnal.gov//store/mc/Run3Winter22DR/DYToLL_M-50_TuneCP5_13p6TeV-pythia8/GEN-SIM-RECO/L1TPU0to99FEVT_122X_mcRun3_2021_realistic_v9-v2/2830000/0898b79c-5b4b-418d-8077-8738c68916f2.root',
            # 'root://cmsxrootd.fnal.gov//store/mc/Run3Winter22DR/DYToLL_M-50_TuneCP5_13p6TeV-pythia8/GEN-SIM-RECO/L1TPU0to99FEVT_122X_mcRun3_2021_realistic_v9-v2/2830000/0b052acc-d6ef-4eae-b237-ad66747b9e8f.root',
            # 'root://cmsxrootd.fnal.gov//store/mc/Run3Winter22DR/DYToLL_M-50_TuneCP5_13p6TeV-pythia8/GEN-SIM-RECO/L1TPU0to99FEVT_122X_mcRun3_2021_realistic_v9-v2/2830000/0c883325-971c-4093-91e9-24ceed4616e7.root'
                                    *file_list_reduced,
                #'root://cmsxrootd.fnal.gov//store/mc/Run3Winter22DR/DYToLL_M-50_TuneCP5_13p6TeV-pythia8/GEN-SIM-RECO/L1TPU0to99FEVT_122X_mcRun3_2021_realistic_v9-v2/2830000/ffdb35b2-7b9f-4b95-b290-cdde95f1d65b.root'
                )
                            )

process.TFileService = cms.Service("TFileService",
                                       fileName = cms.string('test.root')
                                   )

process.demo = cms.EDAnalyzer('ConeOptimisation',
PFCands = cms.InputTag("particleFlow"),
electrons = cms.InputTag("gedGsfElectrons", "", "RECO"),
photons = cms.InputTag("particleFlow", "photons", "RECO"),
hbheInput = cms.InputTag("hbhereco","","RECO"),
HLTriggerResults = cms.InputTag("TriggerResults","","HLT"),
genInfo = cms.InputTag("generator","","SIM"),
genparticles = cms.InputTag("genParticles", "", "HLT"),
xsec = cms.double(5572),
lumi = cms.double(17.7819),
isData = cms.bool(False),
trigName = cms.string("HLT_DoubleEle25_CaloIdL_MW_v4")
                              )

process.p = cms.Path(process.demo)