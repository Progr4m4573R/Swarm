#ifndef roomobstacle_LOOP_FUNCTIONS_H
#define roomobstacle_LOOP_FUNCTIONS_H
#include <string>
#include <argos3/core/simulator/loop_functions.h>

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

   std::string m_strOutput;
   std::ofstream m_cOutput;
   string working_directory = "";
   char cwd = '';
};

#endif


