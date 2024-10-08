import FWCore.ParameterSet.Config as cms

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
            'root://cmsxrootd.fnal.gov//store/mc/Run3Winter24Reco/DYJetsToLL_M-50_TuneCP5_13p6TeV-madgraphMLM-pythia8/GEN-SIM-RECO/NoPU_Pilot_133X_mcRun3_2024_realistic_v6-v2/50000/7ee999b2-ade6-4158-b40b-b4aa9dbf26d5.root'
                )
                            )

process.TFileService = cms.Service("TFileService",
                                       fileName = cms.string('histodemo.root')
                                   )

process.demo = cms.EDAnalyzer('ConeOptimisation',
PFCands = cms.InputTag("packedPFCandidates"),
electrons = cms.InputTag("slimmedElectrons"),
photons = cms.InputTag("slimmedPhotons"),
hbheInput = cms.InputTag("reducedEgamma","reducedHBHEHits","PAT")
                              )

process.p = cms.Path(process.demo)