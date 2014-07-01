// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/Primitives.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4HitCollection.h"
#include "DDG4/Geant4Output2ROOT.h"
// Geant4 include files
#include "G4HCofThisEvent.hh"

// ROOT include files
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

using namespace DD4hep::Simulation;
using namespace DD4hep;
using namespace std;

/// Standard constructor
Geant4Output2ROOT::Geant4Output2ROOT(Geant4Context* context, const string& nam)
    : Geant4OutputAction(context, nam), m_file(0), m_tree(0) {
  declareProperty("Section", m_section = "EVENT");
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Output2ROOT::~Geant4Output2ROOT() {
  InstanceCount::decrement(this);
  if (m_file) {
    TDirectory::TContext context(m_file);
    m_tree->Write();
    m_file->Close();
    m_tree = 0;
    deletePtr (m_file);
  }
}

/// Create/access tree by name
TTree* Geant4Output2ROOT::section(const std::string& nam) {
  Sections::const_iterator i = m_sections.find(nam);
  if (i == m_sections.end()) {
    TDirectory::TContext context(m_file);
    TTree* t = new TTree(nam.c_str(), ("Geant4 " + nam + " information").c_str());
    m_sections.insert(make_pair(nam, t));
    return t;
  }
  return (*i).second;
}

/// Callback to store the Geant4 run information
void Geant4Output2ROOT::beginRun(const G4Run* run) {
  if (!m_file && !m_output.empty()) {
    TDirectory::TContext context(TDirectory::CurrentDirectory());
    m_file = TFile::Open(m_output.c_str(), "RECREATE", "DD4hep Simulation data");
    if (m_file->IsZombie()) {
      deletePtr (m_file);
      throw runtime_error("Failed to open ROOT output file:'" + m_output + "'");
    }
    m_tree = section("EVENT");
  }
  Geant4OutputAction::beginRun(run);
}

/// Fill single EVENT branch entry (Geant4 collection data)
int Geant4Output2ROOT::fill(const std::string& nam, const ComponentCast& type, void* ptr) {
  if (m_file) {
    TBranch* b = 0;
    Branches::const_iterator i = m_branches.find(nam);
    if (i == m_branches.end()) {
      TClass* cl = TBuffer::GetClass(type.type);
      if (cl) {
        b = m_tree->Branch(nam.c_str(), cl->GetName(), (void*) 0);
        b->SetAutoDelete(false);
        m_branches.insert(make_pair(nam, b));
      }
      else {
        throw runtime_error("No ROOT TClass object availible for object type:" + typeName(type.type));
      }
    }
    else {
      b = (*i).second;
    }
    Long64_t evt = b->GetEntries(), nevt = b->GetTree()->GetEntries(), num = nevt - evt;
    if (nevt > evt) {
      b->SetAddress(0);
      while (num > 0) {
        b->Fill();
        --num;
      }
    }
    b->SetAddress(&ptr);
    int nbytes = b->Fill();
    if (nbytes < 0) {
      throw runtime_error("Failed to write ROOT collection:" + nam + "!");
    }
    return nbytes;
  }
  return 0;
}

/// Commit data at end of filling procedure
void Geant4Output2ROOT::commit(OutputContext<G4Event>& ctxt) {
  if (m_file) {
    TObjArray* a = m_tree->GetListOfBranches();
    Long64_t evt = m_tree->GetEntries() + 1;
    Int_t nb = a->GetEntriesFast();
    /// Fill NULL pointers to all branches, which have less entries than the Event branch
    for (Int_t i = 0; i < nb; ++i) {
      TBranch* br_ptr = (TBranch*) a->UncheckedAt(i);
      Long64_t br_evt = br_ptr->GetEntries();
      if (br_evt < evt) {
        Long64_t num = evt - br_evt;
        br_ptr->SetAddress(0);
        while (num > 0) {
          br_ptr->Fill();
          --num;
        }
      }
    }
    m_tree->SetEntries(evt);
  }
  Geant4OutputAction::commit(ctxt);
}

/// Callback to store each Geant4 hit collection
void Geant4Output2ROOT::saveCollection(OutputContext<G4Event>& /* ctxt */, G4VHitsCollection* collection) {
  Geant4HitCollection* coll = dynamic_cast<Geant4HitCollection*>(collection);
  std::string hc_nam = collection->GetName();
  std::vector<void*> hits;
  if (coll) {
    coll->getHitsUnchecked(hits);
    fill(hc_nam, coll->vector_type(), &hits);
  }
}