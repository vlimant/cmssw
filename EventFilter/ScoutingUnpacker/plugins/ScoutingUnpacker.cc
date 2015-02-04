// -*- C++ -*-
//
// Package:    EventFilter/ScoutingUnpacker
// Class:      ScoutingUnpacker
// 
/**\class ScoutingUnpacker ScoutingUnpacker.cc EventFilter/ScoutingUnpacker/plugins/ScoutingUnpacker.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Jean-Roch Vlimant
//         Created:  Wed, 04 Feb 2015 11:13:42 GMT
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

//
// class declaration
//

class ScoutingUnpacker : public edm::EDProducer {
   public:
      explicit ScoutingUnpacker(const edm::ParameterSet&);
      ~ScoutingUnpacker();

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
  edm::EDGetTokenT<FEDRawDataCollection> rawToken_;
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
ScoutingUnpacker::ScoutingUnpacker(const edm::ParameterSet& iConfig)
{
  edm::InputTag rawITag_ = iConfig.getParameter<edm::InputTag>("raw");
  rawToken_ = consumes<FEDRawDataCollection>(rawITag_);
  fedID_ = iConfig.getParameter<unsigned int>("fedID");
}


ScoutingUnpacker::~ScoutingUnpacker()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
ScoutingUnpacker::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace std;
   Handle<FEDRawDataCollection> rawH;
   iEvent.getByToken(rawToken_, rawH);

   const FEDRawDataCollection & raw = *rawH.product();
 
   const FEDRawData& rawdata = raw.FEDData(fedID_);

   const unsigned char* readPtr = rawdata.data();

   std::size_t sof = (float)*readPtr;
   readPtr+= sof;
   unsigned int nJets = (float)*readPtr;
   readPtr+=sof;
   cout<<"sof "<<sof<<endl
       <<"njets"<<nJets<<endl;

   float pt,eta;
   for (uint ij=0;ij!=nJets;++ij){
     pt = *(float*)readPtr;
     readPtr+=sof;
     eta = *(float*)readPtr;
     readPtr+=sof;

     cout<<"-->"<<ij<<"] offset: "<<readPtr- rawdata.data() << " pt: "<<pt<<" eta: "<<eta<<endl;
   }
}

// ------------ method called once each job just before starting event loop  ------------
void 
ScoutingUnpacker::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
ScoutingUnpacker::endJob() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
ScoutingUnpacker::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
ScoutingUnpacker::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
ScoutingUnpacker::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
ScoutingUnpacker::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
ScoutingUnpacker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(ScoutingUnpacker);
