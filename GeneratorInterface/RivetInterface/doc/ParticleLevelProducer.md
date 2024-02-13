The `ParticleLevelProducer` module uses the `Rivet::RivetAnalysis` object to interpret the `HepMC::GenEvent` stored in __src__ `edm::HepMCProduct` into `reco::GenParticleCollection` products for neutrinos, photons, "consts" and "tags", into `reco::GenJetCollection` products for leptons, jets, fatjets, and into a `reco::METCollection` product for mets.
The exact definition of the content of the output collection is defined by **RivetAnalysis**:

