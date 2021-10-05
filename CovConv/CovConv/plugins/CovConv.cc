// -*- C++ -*-
//
// Package:    CovConv/CovConv
// Class:      CovConv
//
/**\class CovConv CovConv.cc CovConv/CovConv/plugins/CovConv.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Jean-Roch Vlimant
//         Created:  Mon, 04 Oct 2021 08:07:44 GMT
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

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/Common/interface/Association.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include <TTree.h>

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.

using reco::TrackCollection;

class CovConv : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit CovConv(const edm::ParameterSet&);
  ~CovConv();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void beginJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;

  // ----------member data ---------------------------
  edm::EDGetTokenT<edm::View<reco::PFCandidate> > PfToken_;
  edm::EDGetTokenT<edm::Association<pat::PackedCandidateCollection> > pf2pToken_;

  TTree * covTree_;
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
CovConv::CovConv(const edm::ParameterSet& iConfig)
  : PfToken_(consumes<edm::View<reco::PFCandidate>>(iConfig.getParameter<edm::InputTag>("pfCand"))),
    pf2pToken_(consumes<edm::Association<pat::PackedCandidateCollection>>(iConfig.getParameter<edm::InputTag>("pf2packed"))) {
  usesResource("TFileService");
  edm::Service<TFileService> fs;
  covTree_ = fs->make<TTree>("covTree","ntuple for pfcandidate / packed candidate covariance matrix studies");
  double dummy;
  covTree_->Branch("pf_par", &dummy, "pf_par[5]/D");
  covTree_->Branch("pf_cov", &dummy,"pf_cov[25]/D");
  covTree_->Branch("packed_par", &dummy, "packed_par[5]/D");
  covTree_->Branch("packed_cov", &dummy,"packed_cov[25]/D");
  
}

CovConv::~CovConv() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  //
  // please remove this method altogether if it would be left empty
}

//
// member functions
//

// ------------ method called for each event  ------------
void CovConv::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;
  auto& pfcands = iEvent.get(PfToken_);
  auto& pf2packed = iEvent.get(pf2pToken_);
  
  uint pfs=0;
  uint no_pf_track=0;
  uint no_packed=0;
  uint no_packed_track=0;
  pfs = pfcands.size();
  for (size_t i = 0 ; i < pfcands.size(); i++){
    auto pfcand = pfcands.refAt(i);
    if (!pfcand->trackRef()){
      //std::cout<<"a PF candidate with no track associated"<<std:endl;
      no_pf_track++;
      continue;
    }
    //qoverp, lambda, phi, dxy, dsz
    const reco::TrackBase::ParameterVector & par = pfcand->trackRef()->parameters();
    const reco::TrackBase::CovarianceMatrix & cov = pfcand->trackRef()->covariance();

    //find the matching packed candidate, so that we have a handle on the packed covariance matrix
    auto packed = pf2packed[pfcand];
    if (!packed) {
      no_packed++;
      //std::cout<<"missing associated packed candidate"<<std::endl;
      continue;
    }
    if (!packed->hasTrackDetails()){
      //std::cout<<"a pf candidate, with an associated packed candidate, that does not have track details!"<<std::endl;
      no_packed_track++;
      continue;
    }
    auto pTrack = packed->pseudoTrack();
    const reco::TrackBase::ParameterVector & packed_par = pTrack.parameters();
    const reco::TrackBase::CovarianceMatrix & packed_cov = pTrack.covariance();

    //you may fill the info in an ntuple
    covTree_->SetBranchAddress("pf_par", const_cast<double*>(par.Array()));
    covTree_->SetBranchAddress("packed_par", const_cast<double*>(packed_par.Array()));
    covTree_->SetBranchAddress("pf_cov",const_cast<double*>(cov.Array()));
    covTree_->SetBranchAddress("packed_cov",const_cast<double*>(packed_cov.Array()));
    //std::cout<<"filling one pf-packed pair"<<std::endl;
    covTree_->Fill();
  }
  
  std::cout<<pfs<<" PF, "
	   <<no_pf_track<<" without tracks, "
	   <<no_packed<<" not associated with packed, "
	   <<no_packed_track<<" with no packed track details"
	   <<std::endl;
}

// ------------ method called once each job just before starting event loop  ------------
void CovConv::beginJob() {
  // please remove this method if not needed
}

// ------------ method called once each job just after ending the event loop  ------------
void CovConv::endJob() {
  // please remove this method if not needed
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void CovConv::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(CovConv);
