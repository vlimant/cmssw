import FWCore.ParameterSet.Config as cms

process = cms.Process("OWNPARTICLES")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        '/store/relval/CMSSW_7_4_0_pre6/RelValTTbar_13/MINIAODSIM/PU50ns_MCRUN2_74_V0-v3/00000/7097E44B-90AB-E411-B2F4-003048FFCC2C.root'
    )
)

process.myProducerLabel = cms.EDProducer('ScoutingRawPacker',
                                         jets = cms.InputTag('slimmedJets'),
                                         fedID = cms.uint32(145)
                                         )


process.myReader = cms.EDProducer('ScoutingUnpacker',
                                  raw = cms.InputTag('myProducerLabel'),
                                  fedID = cms.uint32(145)
                                  )

process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string('myOutputFile.root'),
                               outputCommands = cms.untracked.vstring("drop *",
                                                                      "keep *_slimmedJets_*_*",
                                                                      "keep *_myProducerLabel_*_*")
)

  
process.p = cms.Path(process.myProducerLabel + process.myReader)

process.e = cms.EndPath(process.out)
