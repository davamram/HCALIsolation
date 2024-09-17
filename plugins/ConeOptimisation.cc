// -*- C++ -*-
//
// Package:    ConeOptimisation/ConeOptimisation
// Class:      ConeOptimisation
//
/**\class ConeOptimisation ConeOptimisation.cc ConeOptimisation/ConeOptimisation/plugins/ConeOptimisation.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  David AMRAM
//         Created:  Mon, 02 Oct 2023 13:10:19 GMT
//
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include <vector>
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TH1.h"
#include "TH2.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "TLorentzVector.h"
#include "DataFormats/PatCandidates/interface/Photon.h"

#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateIsolation.h"

#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"

#include "DataFormats/HcalRecHit/interface/HBHERecHit.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"

#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloTopology/interface/HcalTopology.h"
#include "Geometry/HcalCommonData/interface/HcalDDDRecConstants.h"
#include "Geometry/HcalTowerAlgo/interface/HcalGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/Records/interface/HcalRecNumberingRecord.h"
#include "FWCore/Utilities/interface/ESGetToken.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include <iostream>
//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.
namespace {
  // This template function finds whether theCandidate is in thefootprint
  // // collection. It is templated to be able to handle both reco and pat
  // // photons (from AOD and miniAOD, respectively).
  //
  template <class T, class U>
  bool isInFootprint(const T& thefootprint, const U& theCandidate) {
    for (auto itr = thefootprint.begin(); itr != thefootprint.end(); ++itr) {
      if (itr->key() == theCandidate.key())
        return true;
    }
    return false;
  }
  //This function is needed because pfNoPileUpCandidates have changed keys,
  ////and thus the solution is to use sourceCandidatePtr(0)
  // // This function *shouldn't be used for packedCandidate*
  template <class T, class U>
  bool isInFootprintAlternative(const T& thefootprint, const U& theCandidate) {
    for (auto itr = thefootprint.begin(); itr != thefootprint.end(); ++itr) {
      if (itr->key() == theCandidate->sourceCandidatePtr(0).key())
        return true;
    }
    return false;
  }

}  // namespace

double scaleToEt(const double &eta) { return std::sin(2. * std::atan(std::exp(-eta))); }

// Following : https://github.com/cms-sw/cmssw/blob/master/RecoEgamma/ElectronIdentification/python/Identification/cutBasedElectronID_Winter22_122X_V1_cff.py
// Since cutBasedElectronID_Winter22_122X_V1 is not available in the Data file i'm looking at
bool passesTightID(const pat::Electron& el, const CaloGeometry* geometry) {
    // Retrieve supercluster eta and energy
    float scEta = el.superCluster()->eta();
    float scEt = el.superCluster()->energy() * std::sin(el.superCluster()->position().theta());

    // Define working points for barrel (EB) and endcap (EE)
    if (fabs(scEta) < 1.479) {  // EB (Barrel)
        if (el.full5x5_sigmaIetaIeta() >= 0.0101) return false;
        if (fabs(el.deltaEtaSuperClusterTrackAtVtx()) >= 0.00411) return false;
        if (fabs(el.deltaPhiSuperClusterTrackAtVtx()) >= 0.116) return false;
        float hOverE = el.hcalOverEcal() - 1.16 * el.superCluster()->eta() - 0.0422;
        if (hOverE >= 0.02) return false;
        if (el.dr03TkSumPt() / scEt >= 0.0388 + 0.535 * el.pt()) return false;
        if (fabs(1.0 / el.ecalEnergy() - 1.0 / el.trackMomentumAtVtx().R()) >= 0.023) return false;
        if (el.gsfTrack()->hitPattern().numberOfLostHits(reco::HitPattern::MISSING_INNER_HITS) > 1) return false;
    } else {  // EE (Endcap)
        if (el.full5x5_sigmaIetaIeta() >= 0.027) return false;
        if (fabs(el.deltaEtaSuperClusterTrackAtVtx()) >= 0.00938) return false;
        if (fabs(el.deltaPhiSuperClusterTrackAtVtx()) >= 0.164) return false;
        float hOverE = el.hcalOverEcal() - 0.5 * el.superCluster()->eta() - 0.262;
        if (hOverE >= 0.02) return false;
        if (el.dr03TkSumPt() / scEt >= 0.0544 + 0.519 * el.pt()) return false;
        if (fabs(1.0 / el.ecalEnergy() - 1.0 / el.trackMomentumAtVtx().R()) >= 0.018) return false;
        if (el.gsfTrack()->hitPattern().numberOfLostHits(reco::HitPattern::MISSING_INNER_HITS) > 1) return false;
    }

    // Conversion veto is already applied elsewhere, or we can add it here if needed.
    return true;
}


using reco::TrackCollection;

class ConeOptimisation : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit ConeOptimisation(const edm::ParameterSet&);
  ~ConeOptimisation() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void beginJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;

  // ----------member data ---------------------------
edm::EDGetToken PFCands_;
edm::EDGetToken electronsToken_;
edm::EDGetToken photonsToken_;
edm::EDGetToken hbheToken_;
edm::EDGetToken genInfo_;
edm::EDGetToken triggerResultsToken_;
edm::ESGetToken<CaloGeometry, CaloGeometryRecord> caloGeometryEventToken_;
TH1D *_histoPfIso;
TH1D *_histoPfAodIso;
TH1D *_histoHcalIsodr03;
TH1D *_histoHcalIsodr02;
TH1D *_histoHcalIsodr022;
TH1D *_histoHcalIsodr024;
TH1D *_histoHcalIsodr026;
TH1D *_histoHcalIsodr028;
TH1D *_histoHcalIsodr03Treshold;
TH1D *_histoHcalAodIso;
TH1D *_histoRecHit;
TH1D *_histoMissingRecHit;
TH1D *_histoWrongIso;

// SF
TH2F* electronScaleFactorHist_;

// Config var
double xsec;
double lumi;
bool isData;
std::string trigName;
std::string sfName;
double sow;

// Test for the thresholds
float minHB = 10;
float minHE1 = 10;
float minHE2 = 10;

CaloGeometry const *geometry = nullptr;
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  edm::ESGetToken<SetupData, SetupRecord> setupToken_;
#endif
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
ConeOptimisation::ConeOptimisation(const edm::ParameterSet& iConfig)
    : PFCands_(consumes<std::vector<pat::PackedCandidate>>(iConfig.getParameter<edm::InputTag>("PFCands"))),
      electronsToken_(consumes<std::vector<pat::Electron>>(iConfig.getParameter<edm::InputTag>("electrons"))),
      photonsToken_(consumes<std::vector<pat::Photon>>(iConfig.getParameter<edm::InputTag>("photons"))),
      hbheToken_(consumes<edm::SortedCollection<HBHERecHit,edm::StrictWeakOrdering<HBHERecHit>>>(iConfig.getParameter<edm::InputTag>("hbheInput"))),
      triggerResultsToken_ (consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag> ("HLTriggerResults"))),
      caloGeometryEventToken_{esConsumes<CaloGeometry, CaloGeometryRecord>()},
      xsec(iConfig.getParameter<double>("xsec")),
      lumi(iConfig.getParameter<double>("lumi")),
      isData(iConfig.getParameter<bool>("isData")),
      trigName(iConfig.getParameter<std::string>("trigName")),
      sfName(iConfig.getParameter<std::string>("sfName")){

        if(!isData) genInfo_ = (consumes<GenEventInfoProduct>(iConfig.getParameter<edm::InputTag> ("genInfo")));
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  setupDataToken_ = esConsumes<SetupData, SetupRecord>();
#endif
  //now do what ever initialization is needed
  edm::Service<TFileService> fs;
  _histoPfIso = fs->make<TH1D>("PfNeutralIso" , "PfNeutralIso" , 50 , 0 , 15);
  _histoPfAodIso = fs->make<TH1D>("PfNeutralAodIso" , "PfNeutralAodIso" , 50 , 0 , 15);
  _histoHcalIsodr03 = fs->make<TH1D>("HcalTowerIsodr03" , "HcalTowerIsodr03" , 50 , 0 , 15);
  _histoHcalIsodr02 = fs->make<TH1D>("HcalTowerIsodr02" , "HcalTowerIsodr02" , 50 , 0 , 15);
  _histoHcalIsodr022 = fs->make<TH1D>("HcalTowerIsodr022" , "HcalTowerIsodr022" , 50 , 0 , 15);
  _histoHcalIsodr024 = fs->make<TH1D>("HcalTowerIsodr024" , "HcalTowerIsodr024" , 50 , 0 , 15);
  _histoHcalIsodr026 = fs->make<TH1D>("HcalTowerIsodr026" , "HcalTowerIsodr026" , 50 , 0 , 15);
  _histoHcalIsodr028 = fs->make<TH1D>("HcalTowerIsodr028" , "HcalTowerIsodr028" , 50 , 0 , 15);
  _histoHcalIsodr03Treshold = fs->make<TH1D>("HcalIsoTreshold" , "HcalIsoTreshold" , 50 , 0 , 15);
  _histoHcalAodIso = fs->make<TH1D>("HcalNeutralAodIso" , "HcalNeutralAodIso" , 50 , 0 , 15);
  _histoRecHit = fs->make<TH1D>("NofRecHit" , "NofRecHit" , 100 , 0 , 100);
  _histoMissingRecHit = fs->make<TH1D>("MissingRecHit", "MissingRecHit", 2, 0, 2);
  _histoWrongIso = fs->make<TH1D>("WrongIso", "MissingRecHit", 50, 0, 5);

  // Scale factor
  TFile* file = TFile::Open(sfName.c_str());
  if (file && !file->IsZombie()) {
    electronScaleFactorHist_ = dynamic_cast<TH2F*>(file->Get("EGamma_SF2D"));
    if (!electronScaleFactorHist_) {
      throw cms::Exception("FileError") << "Failed to load histogram for electron scale factors!";
    }
  } else {
    throw cms::Exception("FileError") << "Failed to open the file containing electron scale factors!";
  }

}

ConeOptimisation::~ConeOptimisation() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  //
  // please remove this method altogether if it would be left empty
}

//
// member functions
//

// ------------ method called for each event  ------------
void ConeOptimisation::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;
  using namespace std;


  edm::Handle<vector<pat::PackedCandidate>> PFCands;
  iEvent.getByToken(PFCands_, PFCands);
  edm::Handle<std::vector<pat::Electron>> electrons;
  iEvent.getByToken(electronsToken_, electrons);
  edm::Handle<std::vector<pat::Photon>> photons;
  iEvent.getByToken(photonsToken_, photons);
  edm::Handle<edm::SortedCollection<HBHERecHit,edm::StrictWeakOrdering<HBHERecHit>>> hbheHits;
  iEvent.getByToken(hbheToken_, hbheHits);
  edm::Handle<GenEventInfoProduct> genInfo;
  if(!isData) iEvent.getByToken(genInfo_, genInfo);
  edm::Handle<edm::TriggerResults> triggerResults;
  iEvent.getByToken(triggerResultsToken_, triggerResults);
  geometry = &iSetup.getData(caloGeometryEventToken_);

  double w = 1;
  if (!isData) {
    w = genInfo->weight();
    sow += w;
  }
  else{
    sow=1;
  }

  // Selection for data comparaison
  const edm::TriggerNames& triggerNames = iEvent.triggerNames(*triggerResults);
  bool trig = false;
  for (auto const &triggerName : triggerNames.triggerNames()) {
    if (triggerName == trigName && triggerResults->accept(triggerNames.triggerIndex(triggerName))){
      trig = true;
    }
  }
  if(!trig) return;

  std::vector<TLorentzVector> Elp4;
  std::vector<pat::Electron> goodEle;
  for (const auto& el : *electrons) {
    TLorentzVector p4;
    // Electron ID
    // const std::vector<std::pair<std::string, float>>& electronIDs = el.electronIDs();
    // for (const auto& id : electronIDs) {
    //     std::cout << "ID: " << id.first << " Value: " << id.second << std::endl;
    // }
    // if(!el.electronID("cutBasedElectronID-RunIIIWinter22-V1-tight")) continue;

    if (!passesTightID(el, geometry)) continue;

    // Cut for recHits storage
    float scEt = el.superCluster()->energy()*std::sin(el.superCluster()->position().theta());
    if(el.et() < 30 || scEt < 30) continue;
    // Cuts for data
    auto scEta = el.superCluster()->eta();
    if(fabs(scEta)>2.4 || (fabs(scEta)<1.56 && fabs(scEta)>1.44)) continue;
    p4.SetPtEtaPhiE(el.pt(), el.eta(), el.phi(), el.energy());
    Elp4.push_back(p4);
    goodEle.push_back(el);


  }

  // Cuts for DY
  if(goodEle.size()!=2) return;
  if(goodEle[0].charge() * goodEle[1].charge() >= 0) return;
  if((Elp4[0]+Elp4[1]).M()<50) return;

  for (const auto& el : goodEle) {
    double PfNeutralIso=0;
    double HcalTowerIsodr03=0;
    double HcalTowerIsodr02=0;
    double HcalTowerIsodr022=0;
    double HcalTowerIsodr024=0;
    double HcalTowerIsodr026=0;
    double HcalTowerIsodr028=0;
    double HcalTowerIsodr03WithThreshold=0;

    float eta = el.eta();
    float pt = el.pt();
    float scaleFactor = 1.0;

    if(!isData){
      if (electronScaleFactorHist_) {
        int binX = electronScaleFactorHist_->GetXaxis()->FindBin(eta);
        int binY = electronScaleFactorHist_->GetYaxis()->FindBin(pt);
        scaleFactor = electronScaleFactorHist_->GetBinContent(binX, binY);
        w = genInfo->weight()*scaleFactor;

      }
    }
    

    
    for (const auto& pfcand : *PFCands) {
      bool inFootprint = isInFootprint((edm::RefVector<pat::PackedCandidateCollection>)el.associatedPackedPFCandidates(), (edm::Ptr<pat::PackedCandidate>&)pfcand);
      if(inFootprint) continue;
      if (reco::deltaR2(el, pfcand) < 0.3*0.3){
        if(abs(pfcand.pdgId())==130) PfNeutralIso+=pfcand.et();
      }
    }
    for (const auto& hbhe : *hbheHits) {
      // Get eta and phi
      HcalDetId cell(hbhe.id());
      const HcalGeometry *cellGeometry = dynamic_cast<const HcalGeometry *>(geometry->getSubdetectorGeometry(cell));
      auto position = cellGeometry->getPosition(cell);
      double heta = position.eta();
      double hphi = position.phi();
      double hbhe_et = hbhe.energy()*scaleToEt(abs(heta));
      double dr2 = reco::deltaR2(el.superCluster()->eta(), el.superCluster()->phi(), heta, hphi);
      if (dr2 < 0.3*0.3 && dr2 > 0.15*0.15) HcalTowerIsodr03+=hbhe_et;
      if (dr2 < 0.2*0.2 && dr2 > 0.15*0.15) HcalTowerIsodr02+=hbhe_et;
      if (dr2 < 0.22*0.22 && dr2 > 0.15*0.15) HcalTowerIsodr022+=hbhe_et;
      if (dr2 < 0.24*0.24 && dr2 > 0.15*0.15) HcalTowerIsodr024+=hbhe_et;
      if (dr2 < 0.26*0.26 && dr2 > 0.15*0.15) HcalTowerIsodr026+=hbhe_et;
      if (dr2 < 0.28*0.28 && dr2 > 0.15*0.15) HcalTowerIsodr028+=hbhe_et;

      // Apply the cuts on hbhehits following https://github.com/cms-sw/cmssw/pull/23540
      HcalDetId hcalDetId = hbhe.id();
      if (hcalDetId.subdet() == HcalBarrel) {
        if(hbhe_et < minHB) minHB = hbhe_et;
        if(hcalDetId.depth()==1 && hbhe_et < 0.4) continue;
        if(hcalDetId.depth()!=1 && hbhe_et < 0.3) continue;
      } else if (hcalDetId.subdet() == HcalEndcap) {
          if(hbhe_et < minHE1 && hcalDetId.depth()==1) minHE1 = hbhe_et;
          if(hbhe_et < minHE2 && hcalDetId.depth()!=1) minHE2 = hbhe_et;
          if(hcalDetId.depth()==1 && hbhe_et<0.1) continue;
          if(hcalDetId.depth()!=1 && hbhe_et<0.2) continue;
      } else {
          std::cout << "Something is wrong" << std::endl;
      }
      if (dr2 < 0.3*0.3 && dr2 > 0.15*0.15) HcalTowerIsodr03WithThreshold+=hbhe_et;
    }
    _histoWrongIso->Fill(el.dr03HcalTowerSumEt(0)-HcalTowerIsodr03, w);
    //if(hbheHits->size()!=0) std::cout<<"size :"<<hbheHits->size()<<std::endl;
    //if(fabs(scEta)<1.44) continue;
    _histoPfIso->Fill(PfNeutralIso, w);
    _histoHcalIsodr03->Fill(HcalTowerIsodr03, w);
    _histoHcalIsodr02->Fill(HcalTowerIsodr02, w);
    _histoHcalIsodr022->Fill(HcalTowerIsodr022, w);
    _histoHcalIsodr024->Fill(HcalTowerIsodr024, w);
    _histoHcalIsodr026->Fill(HcalTowerIsodr026, w);
    _histoHcalIsodr028->Fill(HcalTowerIsodr028, w);
    _histoHcalIsodr03Treshold->Fill(HcalTowerIsodr03WithThreshold, w);
    // electron
    _histoHcalAodIso->Fill(el.dr03HcalTowerSumEt(0), w);
    _histoPfAodIso->Fill(el.pfIsolationVariables().sumNeutralHadronEt, w);
    //photon
    // _histoHcalAodIso->Fill(el.hcalTowerSumEtConeDR03(0));
    // _histoPfAodIso->Fill(el.neutralHadronIso());
    // for photon : ph.neutralHadronIso()
  }
  if(electrons->size() != 0 && hbheHits->size()==0){
    _histoMissingRecHit->Fill(1);
  }
  else if (electrons->size() != 0 && hbheHits->size()!= 0) _histoMissingRecHit->Fill(0);
  _histoRecHit->Fill(hbheHits->size());

#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  // if the SetupData is always needed
  auto setup = iSetup.getData(setupToken_);
  // if need the ESHandle to check if the SetupData was there or not
  auto pSetup = iSetup.getHandle(setupToken_);
#endif
}

// ------------ method called once each job just before starting event loop  ------------
void ConeOptimisation::beginJob() {
  // please remove this method if not needed
  sow=0;
}

// ------------ method called once each job just after ending the event loop  ------------
void ConeOptimisation::endJob() {
  std::cout<<"min HB/HE1/HE2 : "<<minHB<<" / "<<minHE1<<" / "<<minHE2<<std::endl;
  _histoHcalIsodr03->Scale(xsec*lumi/sow);
  _histoHcalIsodr02->Scale(xsec*lumi/sow);
  _histoHcalIsodr022->Scale(xsec*lumi/sow);
  _histoHcalIsodr024->Scale(xsec*lumi/sow);
  _histoHcalIsodr026->Scale(xsec*lumi/sow);
  _histoHcalIsodr028->Scale(xsec*lumi/sow);
  _histoHcalIsodr03Treshold->Scale(xsec*lumi/sow);
  _histoHcalAodIso->Scale(xsec*lumi/sow);
  // please remove this method if not needed
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void ConeOptimisation::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addWithDefaultLabel(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(ConeOptimisation);
