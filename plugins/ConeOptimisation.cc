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
TH1D *_histoPfIso;
TH1D *_histoPfAodIso;
TH1D *_histoHcalIso;
TH1D *_histoHcalAodIso;
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
      hbheToken_(consumes<edm::SortedCollection<HBHERecHit,edm::StrictWeakOrdering<HBHERecHit>>>(iConfig.getParameter<edm::InputTag>("hbheInput"))){
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  setupDataToken_ = esConsumes<SetupData, SetupRecord>();
#endif
  //now do what ever initialization is needed
  edm::Service<TFileService> fs;
  _histoPfIso = fs->make<TH1D>("PfNeutralIso" , "PfNeutralIso" , 180 , 0 , 15);
  _histoPfAodIso = fs->make<TH1D>("PfNeutralAodIso" , "PfNeutralAodIso" , 180 , 0 , 15);
  _histoHcalIso = fs->make<TH1D>("HcalNeutralIso" , "HcalNeutralIso" , 180 , 0 , 15);
  _histoHcalAodIso = fs->make<TH1D>("HcalNeutralAodIso" , "HcalNeutralAodIso" , 180 , 0 , 15);

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

  for (const auto& el : *electrons) {
    double PfNeutralIso=0;
    double HcalNeutralIso=0;
    // auto scEta = el.superCluster()->eta();
    //if(el.pt()<25 || fabs(scEta)>2.4 || ( fabs(scEta)<1.56 && fabs(scEta)>1.44)) continue;
    for (const auto& pfcand : *PFCands) {
      bool inFootprint = isInFootprint((edm::RefVector<pat::PackedCandidateCollection>)el.associatedPackedPFCandidates(), (edm::Ptr<pat::PackedCandidate>&)pfcand);
      if(inFootprint) continue;
      if ( reco::deltaR2(el, pfcand) < 0.3*0.3){
        if(abs(pfcand.pdgId())==130) PfNeutralIso+=pfcand.et();
      }
    }
    for (const auto& hbhe : *hbheHits) {
      if ( reco::deltaR2(el.superCluster()->eta(), el.superCluster()->phi(), hbhe.id().ieta(), hbhe.id().iphi()) < 0.3*0.3 && reco::deltaR2(el.superCluster()->eta(), el.superCluster()->phi(), hbhe.id().ieta(), hbhe.id().iphi()) > 0.15*0.15) HcalNeutralIso+=hbhe.energy()*scaleToEt(hbhe.id().ieta());
    }
    //if(hbheHits->size()!=0) std::cout<<"size :"<<hbheHits->size()<<std::endl;
    //if(fabs(scEta)<1.44) continue;
    _histoPfIso->Fill(PfNeutralIso);
    _histoHcalIso->Fill(HcalNeutralIso);
    _histoHcalAodIso->Fill(el.dr03HcalTowerSumEt());
    _histoPfAodIso->Fill(el.pfIsolationVariables().sumNeutralHadronEt);
    // for photon : ph.neutralHadronIso()
  }
  if(electrons->size() != 0 && hbheHits->size()==0){
    std::cout<<"SIZE : hbhe : "<<hbheHits->size()<<" electron : "<<electrons->size()<<std::endl;
    for(const auto& el : *electrons){
      std::cout<<"Iso : "<<el.dr03HcalTowerSumEt()<<std::endl;
    }
  }

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
}

// ------------ method called once each job just after ending the event loop  ------------
void ConeOptimisation::endJob() {
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
