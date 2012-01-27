/*
 *  viewer/Driver.h
 *  avida-core
 *
 *  Created by David on 10/28/10.
 *  Copyright 2010-2011 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#ifndef AvidaViewerDriver_h
#define AvidaViewerDriver_h

#include "apto/core.h"
#include "avida/core/Feedback.h"
#include "avida/core/Genome.h"
#include "avida/core/WorldDriver.h"
#include "avida/data/Recorder.h"

class cWorld;


namespace Avida {
  namespace Viewer {

    // Class Declarations
    // --------------------------------------------------------------------------------------------------------------  
    
    class Map;
    class Listener;


    // Enumerations
    // --------------------------------------------------------------------------------------------------------------  
    
    enum DriverPauseState {
      DRIVER_PAUSED,
      DRIVER_UNPAUSED
    };

    
    // Driver Definition
    // --------------------------------------------------------------------------------------------------------------  
    
    class Driver : public Apto::Thread, public Avida::WorldDriver
    {
    private:
      struct InjectGenomeInfo;
      
    private:
      cWorld* m_world;
      World* m_new_world;
      
      mutable Apto::Mutex m_mutex;
      Apto::ConditionVariable m_pause_cv;
      DriverPauseState m_pause_state;
      bool m_started;
      bool m_done;
      bool m_paused;
      
      Update m_pause_at;
      
      Avida::DriverCallback m_callback;
      
      Map* m_map;
      
      Apto::Set<Listener*> m_listeners;

      class StdIOFeedback : public Avida::Feedback
      {
        void Error(const char* fmt, ...);
        void Warning(const char* fmt, ...);
        void Notify(const char* fmt, ...);
      } m_feedback;
      
      Apto::List<InjectGenomeInfo*, Apto::DL> m_inject_queue;

      
    public:
      LIB_EXPORT Driver(cWorld* world, World* new_world);
      LIB_EXPORT ~Driver();
      
      LIB_EXPORT static Driver* InitWithDirectory(const Apto::String& dir);
      
      LIB_EXPORT inline World* GetWorld() { return m_new_world; }
      LIB_EXPORT inline cWorld* GetOldWorld() { return m_world; }
      
      LIB_EXPORT bool HasStarted() const { return m_started; }
      LIB_EXPORT void PauseAt(Update update) { m_pause_at = update; }
      LIB_EXPORT DriverPauseState GetPauseState() const { return m_pause_state; }
      LIB_EXPORT bool IsPaused() const { return m_paused; }
      LIB_EXPORT bool HasFinished() const { return m_done; }
      LIB_EXPORT void Resume();
      
      LIB_EXPORT void InjectGenomeAt(GenomePtr genome, int x, int y);
      LIB_EXPORT bool HasPendingInjects() const;

      LIB_EXPORT void AttachListener(Listener* listener);
      LIB_EXPORT void DetachListener(Listener* listener);

      LIB_EXPORT void AttachRecorder(Data::RecorderPtr recorder, bool concurrent_update = false);
      LIB_EXPORT void DetachRecorder(Data::RecorderPtr recorder);
      
      
      // Hacks to get things working
      LIB_EXPORT int CurrentUpdate() const;
      LIB_EXPORT int NumOrganisms() const;
      
      LIB_EXPORT int WorldX();
      LIB_EXPORT int WorldY();
      LIB_EXPORT bool SetWorldSize(int x, int y);
      
      LIB_EXPORT double MutationRate();
      LIB_EXPORT void SetMutationRate(double rate);
      LIB_EXPORT int PlacementMode();
      LIB_EXPORT void SetPlacementMode(int mode);
      LIB_EXPORT int RandomSeed();
      LIB_EXPORT void SetRandomSeed(int seed);

      LIB_EXPORT double ReactionValue(const Apto::String& name);
      LIB_EXPORT void SetReactionValue(const Apto::String& name, double value);
      
      // WorldDriver Protocol
      // ------------------------------------------------------------------------------------------------------------  
      
    public:
      // Actions
      LIB_EXPORT void Pause();
      LIB_EXPORT void Finish();
      LIB_EXPORT void Abort(AbortCondition condition);
      
      // Facilities
      LIB_EXPORT Avida::Feedback& Feedback() { return m_feedback; }

      // Callback methods
      LIB_EXPORT void RegisterCallback(DriverCallback callback);
      
      
      // Apto::Thread Interface
      // ------------------------------------------------------------------------------------------------------------  
      
    protected:
      LIB_LOCAL void Run();
      
      
      // Private Implementation Details
      // ------------------------------------------------------------------------------------------------------------  
      
    private:
      struct InjectGenomeInfo
      {
        GenomePtr genome;
        int x;
        int y;
        
        LIB_LOCAL inline InjectGenomeInfo(GenomePtr in_genome, int in_x, int in_y) : genome(in_genome), x(in_x), y(in_y) { ; }
      };
      
    };

  };
};

#endif
