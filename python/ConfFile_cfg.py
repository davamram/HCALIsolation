import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")


process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False),
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
                                # replace 'myfile.root' with the source file you want to use
                                fileNames = cms.untracked.vstring(
            'root://cmsxrootd.fnal.gov//store/mc/Run3Summer22EEMiniAODv3/DYToLL_M-4To50_TuneCP5_13p6TeV-pythia8/MINIAODSIM/Poisson70KeepRAW_124X_mcRun3_2022_realistic_postEE_v1-v1/2550000/51335a09-3cfd-417a-b526-8560627e60f5.root',
            'root://cmsxrootd.fnal.gov//store/mc/Run3Summer22EEMiniAODv3/DYto2E_M-50_NNPDF31_TuneCP5_13p6TeV-powheg-pythia8/MINIAODSIM/124X_mcRun3_2022_realistic_postEE_v1-v3/2810000/01ce9762-375b-48c7-95e2-aeb0bc814de7.root' 
                )
                            )

process.TFileService = cms.Service("TFileService",
                                       fileName = cms.string('histodemo.root')
                                   )

process.demo = cms.EDAnalyzer('ConeOptimisation',
PFCands = cms.InputTag("packedPFCandidates"),
electrons = cms.InputTag("slimmedElectrons"),
photons = cms.InputTag("slimmedPhotons"),
hbheInput = cms.InputTag("reducedEgamma" ,  "reducedHBHEHits" ,  "PAT")
                              )

process.p = cms.Path(process.demo)