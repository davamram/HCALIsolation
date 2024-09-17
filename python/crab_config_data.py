from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = 'EGamma_crab_2022C'
config.General.workArea = 'crab_projects'

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'Data_cfg.py'
config.JobType.allowUndistributedCMSSW = True
config.JobType.maxJobRuntimeMin = 180

config.section_("Data")
config.Data.inputDataset='/EGamma/Run2022C-PromptReco-v1/MINIAOD'
config.Data.splitting = 'FileBased'
#config.Data.inputBlocks=[ '/DYto2L_M-50_TuneCP5_13p6TeV_pythia8/Run3Winter24Reco-KeepSi_133X_mcRun3_2024_realistic_v8-v2/GEN-SIM-RECO#002a5782-ba88-4f5b-868e-e06f40060440' ]
config.Data.unitsPerJob=1
config.Data.publication = True
config.Data.outputDatasetTag = 'EGamma_Run2022C'
config.Data.lumiMask = '../Corrections/Cert_Collisions2022_355100_362760_Golden.json'

config.Data.outLFNDirBase = '/store/user/damram/'

config.section_("Site")
config.Site.storageSite = 'T3_FR_IPNL'
config.Site.blacklist = ['T3_UK_ScotGrid_GLA', 'T1_IT_CNAF']
