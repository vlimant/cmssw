// -*- C++ -*-
//
// Package:    EventFilter/ScoutingRawPacker
// Class:      ScoutingRawPacker
// 
/**\class ScoutingRawPacker ScoutingRawPacker.cc EventFilter/ScoutingRawPacker/plugins/ScoutingRawPacker.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Jean-Roch Vlimant
//         Created:  Wed, 04 Feb 2015 09:56:24 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/PatCandidates/interface/Jet.h"


//
// class declaration
//

class ScoutingRawPacker : public edm::EDProducer {
   public:
      explicit ScoutingRawPacker(const edm::ParameterSet&);
      ~ScoutingRawPacker();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() override;
      virtual void produce(edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;
      
      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------

  //edm::InputTag jetITag_;
  edm::EDGetTokenT<pat::JetCollection> jetToken_;
  unsigned int fedID_;
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
ScoutingRawPacker::ScoutingRawPacker(const edm::ParameterSet& iConfig)
{

  edm::InputTag jetITag_ = iConfig.getParameter<edm::InputTag>("jets");
  jetToken_ = consumes<pat::JetCollection>(jetITag_);
  fedID_ = iConfig.getParameter<unsigned int>("fedID");
  
  produces<FEDRawDataCollection>();
}


ScoutingRawPacker::~ScoutingRawPacker()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
ScoutingRawPacker::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std;

  Handle<pat::JetCollection> jetsH;
  iEvent.getByToken(jetToken_, jetsH);
  const pat::JetCollection & jets = *jetsH.product();
  
  auto_ptr<FEDRawDataCollection> productRawData(new FEDRawDataCollection);
  FEDRawData& rawdata = productRawData->FEDData(fedID_);

  unsigned int nVar = 2;   // things can be done much more elegantly with configuration based ready string parser
  std::size_t sof = sizeof(float);
  unsigned int nJets = jets.size();

  unsigned int size_in_b = 1+1+nVar*sof*nJets;
  unsigned int fed_size = ((size_in_b/8)+1)*8;

  // convert in a multiple of 8
  rawdata.resize( fed_size );
  cout <<"size in bytes "<<size_in_b<<endl
       <<"fed allocation:"<<fed_size<<endl;

  unsigned char* writePtr = rawdata.data();
  *writePtr = (float)sof;
  writePtr+=sof;
  *writePtr = (float)nJets;
  writePtr+=sof;

  //the name of the game is to tranform jets quantities into "unsigned char"
  for (uint ij=0;ij!=nJets;++ij){
    *(float*)writePtr = jets[ij].pt();
    writePtr+=sof;
    *(float*)writePtr = jets[ij].eta();
    writePtr+=sof;

    cout<<"<--"<<ij<<"] offset: "<< writePtr - rawdata.data() << " pt: "<<jets[ij].pt()<<" eta: "<<jets[ij].eta()<<endl;
  }
  
  iEvent.put(productRawData);
}

// ------------ method called once each job just before starting event loop  ------------
void 
ScoutingRawPacker::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
ScoutingRawPacker::endJob() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
ScoutingRawPacker::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
ScoutingRawPacker::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
ScoutingRawPacker::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
ScoutingRawPacker::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
ScoutingRawPacker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(ScoutingRawPacker);
