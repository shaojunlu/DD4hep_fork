// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include <algorithm>
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4Kernel.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

/// Intializing constructor
Geant4Run::Geant4Run(const G4Run* run)
: ObjectExtensions(typeid(Geant4Run)), m_run(run)
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Run::~Geant4Run()   {
  InstanceCount::decrement(this);
}

/// Intializing constructor
Geant4Event::Geant4Event(const G4Event* evt) 
: ObjectExtensions(typeid(Geant4Event)), m_event(evt)  
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Event::~Geant4Event()  {
  InstanceCount::decrement(this);
}

/// Default constructor
Geant4Context::Geant4Context(Geant4Kernel* kernel)
  : m_kernel(kernel), m_run(0), m_event(0) {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Context::~Geant4Context() {
  // Do not delete run and event structures here. This is done outside in the framework
  InstanceCount::decrement(this);
}

/// Set the geant4 run reference
void Geant4Context::setRun(Geant4Run* new_run)    {
  m_run = new_run;
}

/// Access the geant4 run -- valid only between BeginRun() and EndRun()!
Geant4Run& Geant4Context::run()  const   {
  if ( m_run ) return *m_run;
  invalidHandleError<Geant4Run>();
  return *m_run;
}

/// Set the geant4 event reference
void Geant4Context::setEvent(Geant4Event* new_event)   {
  m_event = new_event;
}

/// Access the geant4 event -- valid only between BeginEvent() and EndEvent()!
Geant4Event& Geant4Context::event()  const   {
  if ( m_event ) return *m_event;
  invalidHandleError<Geant4Event>();
  return *m_event;
}

/// Access to detector description
Geometry::LCDD& Geant4Context::lcdd() const {
  return m_kernel->lcdd();
}

/// Create a user trajectory
G4VTrajectory* Geant4Context::createTrajectory(const G4Track* /* track */) const {
  string err = DD4hep::format("Geant4Kernel", "createTrajectory: Purely virtual method. requires overloading!");
  DD4hep::printout(DD4hep::FATAL, "Geant4Kernel", "createTrajectory: Purely virtual method. requires overloading!");
  throw runtime_error(err);
}

/// Access the tracking manager
G4TrackingManager* Geant4Context::trackMgr() const {
  return m_kernel->trackMgr();
}

/// Access to the main run action sequence from the kernel object
Geant4RunActionSequence& Geant4Context::runAction() const {
  return m_kernel->runAction();
}

/// Access to the main event action sequence from the kernel object
Geant4EventActionSequence& Geant4Context::eventAction() const {
  return m_kernel->eventAction();
}

/// Access to the main stepping action sequence from the kernel object
Geant4SteppingActionSequence& Geant4Context::steppingAction() const {
  return m_kernel->steppingAction();
}

/// Access to the main tracking action sequence from the kernel object
Geant4TrackingActionSequence& Geant4Context::trackingAction() const {
  return m_kernel->trackingAction();
}

/// Access to the main stacking action sequence from the kernel object
Geant4StackingActionSequence& Geant4Context::stackingAction() const {
  return m_kernel->stackingAction();
}

/// Access to the main generator action sequence from the kernel object
Geant4GeneratorActionSequence& Geant4Context::generatorAction() const {
  return m_kernel->generatorAction();
}

/// Access to the main generator action sequence from the kernel object
Geant4SensDetSequences& Geant4Context::sensitiveActions() const {
  return m_kernel->sensitiveActions();
}