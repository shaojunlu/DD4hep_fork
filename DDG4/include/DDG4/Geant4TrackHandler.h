// $Id: Geant4TrackHandler.h 683 2013-08-08 16:18:20Z markus.frank $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4TRACKHANDLER_H
#define DD4HEP_DDG4_GEANT4TRACKHANDLER_H

// Framework include files
#include "DDG4/Defs.h"

// Geant4 include files
#include "G4Track.hh"
#include "G4TrajectoryPoint.hh"
#include "G4VTouchable.hh"
#include "G4VSensitiveDetector.hh"
#include "G4ParticleDefinition.hh"
#include "G4VProcess.hh"

#include <stdexcept>

// Forward declarations
class G4VTouchableHandle;
class G4VUserTrackInformation;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    // Forward declarations;
    class Geant4TrackHandler;

    /** @class Geant4TrackHandler Geant4SensitiveDetector.h DDG4/Geant4SensitiveDetector.h
     *
     * Tiny helper/utility class to easily access Geant4 track information.
     * Born by lazyness: Avoid typing millions of statements!
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4TrackHandler {
    public:
      typedef G4VUserTrackInformation Info;
      typedef G4ReferenceCountedHandle<G4VTouchable> Touchable;
      /// Reference to the track object
      const G4Track* track;
      /// Initializing constructor
      Geant4TrackHandler(const G4Track* t)
          : track(t) {
	/// Should test here if the track pointer is valid to avoind any later trouble
	if ( 0 == t )  {
	  throw std::runtime_error("Geant4TrackHandler: NULL pointer passed to constructor!");
	}
      }
      const char* statusName() const    {
	switch( track->GetTrackStatus() )  {
	case fAlive:                    return "Alive";
	case fStopButAlive:             return "StopButAlive";
	case fStopAndKill:              return "StopAndKill";
	case fKillTrackAndSecondaries:  return "KillTrackAndSecondaries";
	case fSuspend:                  return "Suspend";
	case fPostponeToNextEvent:      return "PostponeToNextEvent";
	default:                        return "UNKNOWN";
	}
      }

      /// Conversion to G4Track
      operator const G4Track*() const {
        return track;
      }
      /// Track's identifier
      int id() const  {
	return track->GetTrackID();
      }
      /// Track's parent identifier
      int parent()  const  {
	return track->GetParentID();
      }
      /// Track's particle definition
      G4ParticleDefinition* trackDef() const {
        return track->GetDefinition();
      }
      /// Track's particle name
      const std::string& name()  const  {
	return trackDef()->GetParticleName();
      }
      /// Track's position
      const G4ThreeVector& position() const {
        return track->GetPosition();
      }
      /// Track's vertex position, where the track was created
      const G4ThreeVector& vertex() const {
        return track->GetVertexPosition();
      }
      /// Track global time 
      double globalTime() const  {
	return track->GetGlobalTime();
      }
      /// Track proper time 
      double properTime() const  {
	return track->GetProperTime();
      }
      /// Track's energy
      double energy() const {
        return track->GetTotalEnergy();
      }
      /// Track's kinetic energy
      double kineticEnergy() const {
        return track->GetKineticEnergy();
      }
      /// Track velocity
      double velocity() const {
        return track->GetVelocity();
      }
      /// Track length
      double length() const {
        return track->GetTrackLength();
      }
      /// Track time
      double time() const {
        return track->GetGlobalTime();
      }
      /// Physical (original) volume of the track
      G4VPhysicalVolume* vol() const {
        return track->GetVolume();
      }
      /// Next physical volume of the track
      G4VPhysicalVolume* nextVol() const {
        return track->GetNextVolume();
      }
      /// Logical volume of the origine vertex
      const G4LogicalVolume* vertexVol() const {
        return track->GetLogicalVolumeAtVertex();
      }
      /// Touchable of the track
      const Touchable& touchable() const {
        return track->GetTouchableHandle();
      }
      /// Next touchable of the track
      const Touchable& nextTouchable() const {
        return track->GetNextTouchableHandle();
      }
      /// Physical process of the track generation
      const G4VProcess* creatorProcess() const {
        return track->GetCreatorProcess();
      }
      /// Physical process of the track generation
      const std::string creatorName() const {
	const G4VProcess* p = creatorProcess();
        if ( p ) return p->GetProcessName();
	return "";
      }
      /// User information block
      Info* userInfo() const {
        return track->GetUserInformation();
      }
      /// Specific user information block
      template <typename T> T* info() const {
        return (T*) userInfo();
      }
      /// Step information
      const G4Step* step() const {
        return track->GetStep();
      }
      /// Step number
      G4int stepNumber() const {
        return track->GetCurrentStepNumber();
      }
      /// Access the PDG particle identification
      int pdgID() const {
        G4ParticleDefinition* def = trackDef();
        return def ? def->GetPDGEncoding() : 0;
      }
    };

  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_DDG4_GEANT4TRACKHANDLER_H