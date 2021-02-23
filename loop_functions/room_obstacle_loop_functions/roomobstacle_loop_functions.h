#ifndef roomobstacle_LOOP_FUNCTIONS_H
#define roomobstacle_LOOP_FUNCTIONS_H

#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/simulator/entity/floor_entity.h>
#include <argos3/core/utility/math/range.h>
#include <argos3/core/utility/math/rng.h>

using namespace argos;

class CRoomobstacleLoopFunctions : public CLoopFunctions {

public:

   CRoomobstacleLoopFunctions();
   virtual ~CRoomobstacleLoopFunctions() {}

   virtual void Init(TConfigurationNode& t_tree);
   virtual void Reset();
   virtual void Destroy();

   virtual void PreStep();

private:
   CRange<Real> m_cRoomobstacleArenaSideX, m_cRoomobstacleArenaSideY;

   CRandom::CRNG* m_pcRNG;

   std::string m_strOutput;
   std::ofstream m_cOutput;

};

#endif
