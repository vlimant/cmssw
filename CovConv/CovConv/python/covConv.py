import FWCore.ParameterSet.Config as cms

ntuple = cms.EDAnalyzer("CovConv",
                        pfCand = cms.InputTag("particleFlow"),
                        pf2packed = cms.InputTag("packedPFCandidates"))
TFileService = cms.Service("TFileService",
                           fileName = cms.string("covConv.root"))

NTUPLE_Step = cms.Path(ntuple)
