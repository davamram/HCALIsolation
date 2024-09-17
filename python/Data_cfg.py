import FWCore.ParameterSet.Config as cms

# List of file (for the time crab is not working)
with open("../listSamples/EGamma_Run2022G-PromptReco-v1_MINIAOD.txt", "r") as f:
    file_list = f.readlines()
file_list_root = [f'root://cmsxrootd.fnal.gov/{file}' for file in file_list]

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.Geometry.GeometryExtended2023Reco_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')

process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False),
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
                                # replace 'myfile.root' with the source file you want to use
                                fileNames = cms.untracked.vstring(
            #2022
            #'root://cmsxrootd.fnal.gov//store/data/Run2022C/EGamma/MINIAOD/27Jun2023-v1/2530000/00c6799c-81af-4e31-bf0a-724f9a82d9f1.root'
            #2022
            *file_list_root,

                )
                            )

process.TFileService = cms.Service("TFileService",
                                       fileName = cms.string('data_run2022GminiAod_reducedEgamma.root')
                                   )
process.MessageLogger.cerr.threshold = 'DEBUG'
process.MessageLogger.cerr.FwkJob = cms.untracked.PSet(
    limit = cms.untracked.int32(-1)
)

process.demo = cms.EDAnalyzer('ConeOptimisation',
PFCands = cms.InputTag("packedPFCandidates"),
electrons = cms.InputTag("slimmedElectrons"),
photons = cms.InputTag("slimmedPhotons"),
hbheInput = cms.InputTag("reducedEgamma","reducedHBHEHits","RECO"),
HLTriggerResults = cms.InputTag("TriggerResults","","HLT"),
genInfo = cms.InputTag("generator","","SIM"),
xsec = cms.double(1),
# 2024 : B : 0.130 / C : 7.238 / D : 7.957 / E : 11.319 / F : 25.790 / G : 5.477
# 2022 : B :  0.09656 / C : 5.0104 / D : 2.9700 / E : 5.8070 / F : 17.7819 / G : 3.0828
lumi = cms.double(1),
isData = cms.bool(True),
trigName = cms.string("HLT_DoubleEle25_CaloIdL_MW_v7"),
sfName = cms.string("root://lyoeos.in2p3.fr//eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/SF/egammaEffi.txt_EGM2D.root")
                              )

process.p = cms.Path(process.demo)