import FWCore.ParameterSet.Config as cms
import subprocess
# To have the full list starting with a path
# command = 'xrdfs root://cmsxrootd.fnal.gov/ ls /store/relval/CMSSW_12_4_12/EGamma/RECO/124X_dataRun3_Prompt_v10_RelVal_2022C-v1/00000'
# file_list = subprocess.check_output(command, shell=True, text=True).strip().split('\n')

# To use the path in a text file (cosume way less time)
with open("listDatav15C.txt", "r") as f:
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
                                *file_list_root,
                                #'file:/gridgroup/cms/amram/EPR/CMSSW_12_4_15/src/ConeOptimisation/ConeOptimisation/data/DATA/test/00c6799c-81af-4e31-bf0a-724f9a82d9f1.root',
                )
                            )

process.TFileService = cms.Service("TFileService",
                                       fileName = cms.string('Data_eraE.root')
                                   )

process.demo = cms.EDAnalyzer('ConeOptimisation',
PFCands = cms.InputTag("particleFlow"),
electrons = cms.InputTag("gedGsfElectrons", "", "reRECO"),
photons = cms.InputTag("particleFlow", "photons", "RECO"),
hbheInput = cms.InputTag("hbhereco","","reRECO"),
HLTriggerResults = cms.InputTag("TriggerResults","","HLT"),
xsec = cms.double(1),
lumi = cms.double(1),
isData = cms.bool(True),
trigName = cms.string("HLT_DoubleEle25_CaloIdL_MW_v5")
                              )

process.p = cms.Path(process.demo)
