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


#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"


#include <iostream>
#include "TTree.h"
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
edm::EDGetToken electronsToken_;
edm::EDGetToken photonsToken_;
edm::EDGetToken hbheToken_;
edm::EDGetToken genParticlesToken_;
edm::EDGetToken triggerResultsToken_;
edm::EDGetToken genInfo_;
edm::ESGetToken<CaloGeometry, CaloGeometryRecord> caloGeometryEventToken_;

TH1D *_histoPfIso;
TH1D *_histoPfAodIso;
TH1D *_histodr03HcalIso;
TH1D *_histodr03HcalIsoTreshold;
TH1D *_histoHcalAodIso;
TH1D *_histodr04HcalIso;
TH1D *_histodr04HcalIsoTreshold;
TH1D *_histoMll;
TH1D *_histoRecHit;
TH1D *_histoMissingRecHit;
double xsec;
double lumi;
bool isData;
std::string trigName;
double sow;
TTree *outputTree;
double dr03HcalNeutralIso;
double dr03HcalNeutralIsoWithThreshold;
double dr04HcalNeutralIso;
double dr04HcalNeutralIsoWithThreshold;
double HcalAodIso;
double mll;
TLorentzVector ele;
bool isTrue;

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
    : electronsToken_(consumes<std::vector<reco::GsfElectron>>(iConfig.getParameter<edm::InputTag>("electrons"))),
      photonsToken_(consumes<edm::ValueMap<edm::Ptr<reco::PFCandidate>>>(iConfig.getParameter<edm::InputTag>("photons"))),
      hbheToken_(consumes<edm::SortedCollection<HBHERecHit,edm::StrictWeakOrdering<HBHERecHit>>>(iConfig.getParameter<edm::InputTag>("hbheInput"))),
      triggerResultsToken_ (consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag> ("HLTriggerResults"))),
      caloGeometryEventToken_{esConsumes<CaloGeometry, CaloGeometryRecord>()},
      xsec(iConfig.getParameter<double>("xsec")),
      lumi(iConfig.getParameter<double>("lumi")),
      isData(iConfig.getParameter<bool>("isData")),
      trigName(iConfig.getParameter<std::string>("trigName")){
        
        if(!isData) genInfo_ = (consumes<GenEventInfoProduct>(iConfig.getParameter<edm::InputTag> ("genInfo")));
        if(!isData) genParticlesToken_ = (consumes<std::vector<reco::GenParticle>>(iConfig.getParameter<edm::InputTag>("genparticles")));
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  setupDataToken_ = esConsumes<SetupData, SetupRecord>();
#endif
  //now do what ever initialization is needed
  edm::Service<TFileService> fs;
  _histoPfIso = fs->make<TH1D>("PfNeutralIso" , "PfNeutralIso" , 50 , 0 , 15);
  _histoPfAodIso = fs->make<TH1D>("PfNeutralAodIso" , "PfNeutralAodIso" , 50 , 0 , 15);
  _histodr03HcalIso = fs->make<TH1D>("dr03HcalNeutralIso" , "dr03HcalNeutralIso" , 180 , 0 , 15);
  _histodr04HcalIso = fs->make<TH1D>("dr04HcalNeutralIso" , "dr04HcalNeutralIso" , 180 , 0 , 15);
  _histodr03HcalIsoTreshold = fs->make<TH1D>("dr03HcalIsoTreshold" , "dr03HcalIsoTreshold" , 180 , 0 , 15);
  _histodr04HcalIsoTreshold = fs->make<TH1D>("dr04HcalIsoTreshold" , "dr04HcalIsoTreshold" , 180 , 0 , 15);
  _histoHcalAodIso = fs->make<TH1D>("HcalNeutralAodIso" , "HcalNeutralAodIso" , 180 , 0 , 15);
  _histoMll = fs->make<TH1D>("Mll" , "Mll" , 180 , 0 , 180);
  _histoRecHit = fs->make<TH1D>("NofRecHit" , "NofRecHit" , 100 , 0 , 100);
  _histoMissingRecHit = fs->make<TH1D>("MissingRecHit", "MissingRecHit", 2, 0, 2);
  outputTree = fs->make<TTree>("outputTree", "Output Tree");
  outputTree->Branch("dr03HcalNeutralIso", &dr03HcalNeutralIso);
  outputTree->Branch("dr03HcalNeutralIsoWithThreshold", &dr03HcalNeutralIsoWithThreshold);
  outputTree->Branch("dr04HcalNeutralIso", &dr04HcalNeutralIso);
  outputTree->Branch("dr04HcalNeutralIsoWithThreshold", &dr04HcalNeutralIsoWithThreshold);
  outputTree->Branch("HcalAodIso", &HcalAodIso);
  outputTree->Branch("ele", &ele);
  outputTree->Branch("isTrue", &isTrue);

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


  edm::Handle<std::vector<reco::GsfElectron>> electrons;
  iEvent.getByToken(electronsToken_, electrons);
  edm::Handle<edm::ValueMap<edm::Ptr<reco::PFCandidate>>> photons;
  iEvent.getByToken(photonsToken_, photons);
  edm::Handle<edm::SortedCollection<HBHERecHit,edm::StrictWeakOrdering<HBHERecHit>>> hbheHits;
  iEvent.getByToken(hbheToken_, hbheHits);
  edm::Handle<GenEventInfoProduct> genInfo;
  edm::Handle<std::vector<reco::GenParticle>> genParticles;
  if(!isData){  
    iEvent.getByToken(genParticlesToken_, genParticles);
    iEvent.getByToken(genInfo_, genInfo);
  }
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

  const edm::TriggerNames& triggerNames = iEvent.triggerNames(*triggerResults);
  bool trig = false;
  for (auto const &triggerName : triggerNames.triggerNames()) {
    //std::cout<<triggerName<<std::endl;
    if (triggerName == trigName && triggerResults->accept(triggerNames.triggerIndex(triggerName))){
      trig = true;
      //std::cout<<"Trigger passed"<<std::endl;
    }
  }
  if(!trig) return;

  // Electron selection
  std::vector<TLorentzVector> Elp4;
  std::vector<reco::GsfElectron> goodEle;
  isTrue=false;
  for (const auto& el : *electrons) {
    TLorentzVector p4;
    reco::GenParticle genEl;
    float distance = -1;
    float scEta = el.superCluster()->eta();
    if(el.pt()<30) continue;
    if(fabs(scEta)>2.4 || (fabs(scEta)<1.56 && fabs(scEta)>1.44)) continue;
    goodEle.push_back(el);
    p4.SetPtEtaPhiE(el.pt(), el.eta(), el.phi(), el.energy());
    Elp4.push_back(p4);
    if(!isData){
      for (const auto& genParticle : *genParticles) {
        if (distance==-1 || (abs(genParticle.pt() - el.pt()) + abs(genParticle.eta() - el.eta()) + abs(genParticle.phi() - el.phi()) < distance)){
          genEl = genParticle;
          distance = abs(genParticle.pt() - el.pt()) + abs(genParticle.eta() - el.eta()) + abs(genParticle.phi() - el.phi());
        }
      }
    }
  if(!isData){
    if(abs(genEl.pdgId())==11){
      isTrue=true;
    }
  }
  }

  if(goodEle.size()!=2) return;
  if((Elp4[0]+Elp4[1]).M()<50) return;

  std::vector<TLorentzVector> p4Ele;
  for (const auto el : goodEle) {
    double PfNeutralIso=0;
    dr03HcalNeutralIso=0;
    dr03HcalNeutralIsoWithThreshold=0;
    dr04HcalNeutralIso=0;
    dr04HcalNeutralIsoWithThreshold=0;
    ele.SetPtEtaPhiE(el.pt(), el.eta(), el.phi(), el.energy());
    p4Ele.push_back(ele);
    float scEt = el.superCluster()->energy()*std::sin(el.superCluster()->position().theta());
    float scEta = el.superCluster()->eta();
    float scPhi = el.superCluster()->phi();
    for (const auto& hbhe : *hbheHits) {
      // Get eta and phi
      HcalDetId cell(hbhe.id());
      const HcalGeometry *cellGeometry = dynamic_cast<const HcalGeometry *>(geometry->getSubdetectorGeometry(cell));
      auto position = cellGeometry->getPosition(cell);
      double heta = position.eta();
      double hphi = position.phi();
      double hbhe_et = hbhe.energy()*scaleToEt(abs(heta));
      double dr2 = reco::deltaR2(scEta, scPhi, heta, hphi);
      if (dr2 < 0.3*0.3 && dr2 > 0.15*0.15) dr03HcalNeutralIso+=hbhe_et;
      if (dr2 < 0.4*0.4 && dr2 > 0.15*0.15) dr04HcalNeutralIso+=hbhe_et;

      // Apply the cuts on hbhehits following https://github.com/cms-sw/cmssw/pull/23540
      HcalDetId hcalDetId = hbhe.id();
      if (hcalDetId.subdet() == HcalBarrel) {
        if(hbhe_et < 0.8) continue;
      } else if (hcalDetId.subdet() == HcalEndcap) {
          if(hcalDetId.depth()==1 && hbhe_et<0.1) continue;
          if(hcalDetId.depth()!=1 && hbhe_et<0.2) continue;
      } else {
          std::cout << "Something is wrong" << std::endl;
      }
      if (dr2 < 0.3*0.3 && dr2 > 0.15*0.15) dr03HcalNeutralIsoWithThreshold+=hbhe_et;
      if (dr2 < 0.4*0.4 && dr2 > 0.15*0.15) dr04HcalNeutralIsoWithThreshold+=hbhe_et;
    }
    _histodr03HcalIso->Fill(dr03HcalNeutralIso, w);
    _histodr03HcalIsoTreshold->Fill(dr03HcalNeutralIsoWithThreshold, w);
    _histodr04HcalIso->Fill(dr04HcalNeutralIso, w);
    _histodr04HcalIsoTreshold->Fill(dr04HcalNeutralIsoWithThreshold, w);
    // electron
    HcalAodIso = el.dr03HcalTowerSumEt(0);
    _histoHcalAodIso->Fill(HcalAodIso, w);
    //photon
    // _histoHcalAodIso->Fill(el.hcalTowerSumEtConeDR03(0));
    // _histoPfAodIso->Fill(el.neutralHadronIso());
    // for photon : ph.neutralHadronIso()
    outputTree->Fill();
  }
  TLorentzVector diEle=p4Ele[0]+p4Ele[1];
  mll=diEle.M();
  _histoMll->Fill(mll);
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
  // please remove this method if not needed
  std::cout<<"histoHcalAodIso integral : "<<_histoHcalAodIso->Integral()<<std::endl;
  std::cout<<"Scalling with factor : "<<xsec*lumi/sow<<std::endl;
  _histodr03HcalIso->Scale(xsec*lumi/sow);
  _histodr03HcalIsoTreshold->Scale(xsec*lumi/sow);
  _histodr04HcalIso->Scale(xsec*lumi/sow);
  _histodr04HcalIsoTreshold->Scale(xsec*lumi/sow);
  _histoHcalAodIso->Scale(xsec*lumi/sow);
  _histoMll->Scale(xsec*lumi/sow);
  std::cout<<"sow is : "<<sow<<std::endl;
  std::cout<<"histoHcalAodIso integral : "<<_histoHcalAodIso->Integral()<<std::endl;
  outputTree->Write();
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
