import FWCore.ParameterSet.Config as cms

# List of file (for the time crab is not working)
with open("../listSamples/DYto2L_M-50_TuneCP5_13p6TeV_pythia8_Run3Winter24MiniAOD-KeepSi_133X_mcRun3_2024_realistic_v8-v2_MINIAODSIM.txt", "r") as f:
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
            # 'root://cmsxrootd.fnal.gov//store/mc/Run3Summer22EEMiniAODv3/DYto2E_M-50_NNPDF31_TuneCP5_13p6TeV-powheg-pythia8/MINIAODSIM/124X_mcRun3_2022_realistic_postEE_v1-v3/2810000/01ce9762-375b-48c7-95e2-aeb0bc814de7.root',
            # 'root://cmsxrootd.fnal.gov//store/mc/Run3Summer22EEMiniAODv3/DYto2E_M-50_NNPDF31_TuneCP5_13p6TeV-powheg-pythia8/MINIAODSIM/124X_mcRun3_2022_realistic_postEE_v1-v3/2810000/004716f9-0288-41fe-b342-d3372331a3cb.root',
            # 'root://cmsxrootd.fnal.gov//store/mc/Run3Summer22DRPremix/DYJetsToLL_M-50_TuneCP5_13p6TeV-madgraphMLM-pythia8/AODSIM/124X_mcRun3_2022_realistic_v12-v2/30000/c5c81d33-b5da-4b23-a1ba-ccff81b9820d.root'
            # 'root://cmsxrootd.fnal.gov//store/mc/Run3Summer22EEMiniAODv3/GJet_PT-40_DoubleEMEnriched_MGG-80_TuneCP5_13p6TeV_pythia8/MINIAODSIM/124X_mcRun3_2022_realistic_postEE_v1-v2/2540000/014da2df-dad1-4d23-8042-9880b61a9f0d.root'
            #'root://cmsxrootd.fnal.gov//store/mc/Run3Winter24Reco/DYJetsToLL_M-50_TuneCP5_13p6TeV-madgraphMLM-pythia8/GEN-SIM-RECO/NoPU_Pilot_133X_mcRun3_2024_realistic_v6-v2/50000/7ee999b2-ade6-4158-b40b-b4aa9dbf26d5.root'
            # 'root://cmsxrootd.fnal.gov//store/mc/Run3Winter24MiniAOD/DYTo2L_MLL-50_TuneCP5_13p6TeV_pythia8/MINIAODSIM/KeepSi_133X_mcRun3_2024_realistic_v8-v2/2560000/001d128f-6406-4358-bee3-785f92b40734.root'
            #'file:../mcsmaple/01ce9762-375b-48c7-95e2-aeb0bc814de7.root'
            #'root://cmsxrootd.fnal.gov//store/mc/RunIISummer20UL18MiniAODv2/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUForTRK_106X_upgrade2018_realistic_v16_L1v1-v2/50000/92DB666B-9E0A-374D-9EDE-2C614BAEC1F2.root'
            #2024
            #'root://cmsxrootd.fnal.gov//store/mc/Run3Winter24MiniAOD/DYto2L_M-50_TuneCP5_13p6TeV_pythia8/MINIAODSIM/KeepSi_133X_mcRun3_2024_realistic_v8-v2/2540000/00d74fac-f2a0-48a4-82c3-543b8ec1e324.root'
            #2022
            #'root://cmsxrootd.fnal.gov//store/mc/Run3Summer22EEMiniAODv3/DYto2E_M-50_NNPDF31_TuneCP5_13p6TeV-powheg-pythia8/MINIAODSIM/124X_mcRun3_2022_realistic_postEE_v1-v3/2810000/004716f9-0288-41fe-b342-d3372331a3cb.root'
            *file_list_root,

                )
                            )

process.TFileService = cms.Service("TFileService",
                                       fileName = cms.string('mc2024_run2022GminiAod_reducedEgamma.root')
                                   )
process.MessageLogger.cerr.threshold = 'DEBUG'
process.MessageLogger.cerr.FwkJob = cms.untracked.PSet(
    limit = cms.untracked.int32(-1)
)

process.demo = cms.EDAnalyzer('ConeOptimisation',
PFCands = cms.InputTag("packedPFCandidates"),
electrons = cms.InputTag("slimmedElectrons"),
photons = cms.InputTag("slimmedPhotons"),
hbheInput = cms.InputTag("reducedEgamma","reducedHBHEHits","PAT"),
HLTriggerResults = cms.InputTag("TriggerResults","","HLT"),
genInfo = cms.InputTag("generator","","SIM"),
xsec = cms.double(5572),
# 2024 : B : 0.130 / C : 7.238 / D : 7.957 / E : 11.319 / F : 25.790 / G : 5.477
# 2022 : B :  0.09656 / C : 5.0104 / D : 2.9700 / E : 5.8070 / F : 17.7819 / G : 3.0828
lumi = cms.double(3.0828),
isData = cms.bool(False),
trigName = cms.string("HLT_DoubleEle25_CaloIdL_MW_v9"),
sfName = cms.string("root://lyoeos.in2p3.fr//eos/lyoeos.in2p3.fr/grid/cms/store/user/damram/SF/egammaEffi.txt_EGM2D.root")
                              )

process.p = cms.Path(process.demo)