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

   int random_seed;
   int time_limit;
   std::string output_folder;
   std::string output_filename;
   std::ofstream output_file;

};

#endif


