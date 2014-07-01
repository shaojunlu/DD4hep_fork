// $Id$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEANT4STEPHANDLER_H
#define DD4HEP_GEANT4STEPHANDLER_H

// Framework include files
#include "DDG4/Defs.h"

// Geant4 include files
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VTouchable.hh"
#include "G4VSensitiveDetector.hh"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    // Forward declarations;
    class Geant4StepHandler;

    /** @class Geant4StepHandler Geant4SensitiveDetector.h DDG4/Geant4SensitiveDetector.h
     *
     * Tiny helper/utility class to easily access Geant4 step information.
     * Born by lazyness: Avoid typing millions of statements!
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4StepHandler {
    public:
      const G4Step* step;
      G4StepPoint* pre;
      G4StepPoint* post;
      G4Track* track;
      Geant4StepHandler(const G4Step* s)
          : step(s) {
        pre = s->GetPreStepPoint();
        post = s->GetPostStepPoint();
        track = s->GetTrack();
      }
      G4ParticleDefinition* trackDef() const {
        return track->GetDefinition();
      }
      int trkPdgID() const {
        return track->GetDefinition()->GetPDGEncoding();
      }
      static const char* stepStatus(G4StepStatus status);
      const char* preStepStatus() const;
      const char* postStepStatus() const;
      Position prePos() const {
        const G4ThreeVector& p = pre->GetPosition();
        return Position(p.x(), p.y(), p.z());
      }
      Position postPos() const {
        const G4ThreeVector& p = post->GetPosition();
        return Position(p.x(), p.y(), p.z());
      }
      Momentum preMom() const {
        const G4ThreeVector& p = pre->GetMomentum();
        return Momentum(p.x(), p.y(), p.z());
      }
      Momentum postMom() const {
        const G4ThreeVector& p = post->GetMomentum();
        return Momentum(p.x(), p.y(), p.z());
      }
      Momentum trkMom() const {
        const G4ThreeVector& p = track->GetMomentum();
        return Momentum(p.x(), p.y(), p.z());
      }
      double deposit() const  {
	return step->GetTotalEnergyDeposit();
      }
      int trkID() const  {
	return track->GetTrackID();
      }
      double trkEnergy() const  {
	return track->GetTotalEnergy();
      }
      double trkKineEnergy() const  {
	return track->GetKineticEnergy();
      }
      const G4VTouchable* preTouchable() const {
        return pre->GetTouchable();
      }
      const G4VTouchable* postTouchable() const {
        return post->GetTouchable();
      }
      const char* volName(const G4StepPoint* p, const char* undefined = "") const {
        G4VPhysicalVolume* v = volume(p);
        return v ? v->GetName().c_str() : undefined;
      }
      G4VPhysicalVolume* volume(const G4StepPoint* p) const {
        return p->GetTouchableHandle()->GetVolume();
      }
      G4VPhysicalVolume* physvol(const G4StepPoint* p) const {
        return p->GetPhysicalVolume();
      }
      G4LogicalVolume* logvol(const G4StepPoint* p) const {
        G4VPhysicalVolume* pv = physvol(p);
        return pv ? pv->GetLogicalVolume() : 0;
      }
      G4VSensitiveDetector* sd(const G4StepPoint* p) const {
        G4LogicalVolume* lv = logvol(p);
        return lv ? lv->GetSensitiveDetector() : 0;
      }
      const char* sdName(const G4StepPoint* p, const char* undefined = "") const {
        G4VSensitiveDetector* s = sd(p);
        return s ? s->GetName().c_str() : undefined;
      }
      bool isSensitive(const G4LogicalVolume* lv) const {
        return lv ? (0 != lv->GetSensitiveDetector()) : false;
      }
      bool isSensitive(const G4VPhysicalVolume* pv) const {
        return pv ? isSensitive(pv->GetLogicalVolume()) : false;
      }
      bool isSensitive(const G4StepPoint* point) const {
        return point ? isSensitive(volume(point)) : false;
      }
      G4VPhysicalVolume* preVolume() const {
        return volume(pre);
      }
      G4VSensitiveDetector* preSD() const {
        return sd(pre);
      }
      G4VPhysicalVolume* postVolume() const {
        return volume(post);
      }
      G4VSensitiveDetector* postSD() const {
        return sd(post);
      }
      bool firstInVolume() const  {
	return step->IsFirstStepInVolume();
      }
      bool lastInVolume() const  {
	return step->IsLastStepInVolume();
      }
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_GEANT4STEPHANDLER_H