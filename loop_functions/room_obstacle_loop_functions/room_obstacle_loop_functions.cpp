#include "room_obstacle_loop_functions.h"
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
#include <controllers/footbot_roomobstacle/footbot_roomobstacle.h>
#include <argos3/plugins/simulator/entities/light_entity.h>


//Making  directory to write my performance.txt to
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
/****************************************/
/****************************************/

//Get working directory so i can make random folder relative to computer
#include <stdio.h>
#include <limits.h>

//Get the Date and time for my file name
#include <iostream>
#include <ctime>
#include <vector>
#include <string>
using namespace std;
CRoomobstacleLoopFunctions::CRoomobstacleLoopFunctions() 
{

}

/****************************************/
/****************************************/

void CRoomobstacleLoopFunctions::Init(TConfigurationNode& t_node) {
   try {
      // Get random seed
      TConfigurationNode& configuration_tree = CSimulator::GetInstance().GetConfigurationRoot();
      TConfigurationNode& framework_node = GetNode(configuration_tree, "framework");
      TConfigurationNode& experiment_node = GetNode(framework_node, "experiment");
      GetNodeAttribute(experiment_node, "random_seed", random_seed);
      
      //Set individual scenario result time for when to display the resulf of the robots reaching the goal
      GetNodeAttribute(experiment_node, "time_limit", time_limit);

      TConfigurationNode& tRoomobstacle = GetNode(t_node, "room_obstacle");      
      
      /* Get the output file name from XML */
      GetNodeAttribute(tRoomobstacle, "output_filename", output_filename);
      GetNodeAttribute(tRoomobstacle, "output_folder", output_folder);

      std::ostringstream oss;
      oss << "output/" << output_folder << "/" << output_filename << "_" << random_seed << ".txt";
      output_filename = oss.str();

      printf("Writing to file: %s\n", output_filename.c_str());

      /* Open the file, erasing its contents */
      output_file.open(output_filename.c_str(), std::ios_base::trunc | std::ios_base::out);
      
      /* Get how long it takes each bot to reach a destination */
      /* Get the id of each bot */
      /*get the start position and end position of each bot*/
	
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("Error parsing loop functions!", ex);
   }
}

/****************************************/
/****************************************/

void CRoomobstacleLoopFunctions::Reset() {
   /* Close the file */
   output_file.close();
   /* Open the file, erasing its contents */
   output_file.open(output_filename.c_str(), std::ios_base::trunc | std::ios_base::out);

}

/****************************************/
/****************************************/

void CRoomobstacleLoopFunctions::Destroy() {
   /* Close the file */
   output_file.close();
}

/****************************************/
/****************************************/


void CRoomobstacleLoopFunctions::PreStep() {

   std::vector<string> goal;

   CSpace::TMapPerType& m_cFootbots = GetSpace().GetEntitiesByType("foot-bot");
   
     /*Get handle to light entity*/
     CLightEntity& light = dynamic_cast<CLightEntity&>(GetSpace().GetEntity("light"));
     /*get the position of the light*/	
     CVector2 lightPos;
     lightPos.Set(light.GetPosition().GetX(),
                  light.GetPosition().GetY());

   for(CSpace::TMapPerType::iterator it = m_cFootbots.begin();
       it != m_cFootbots.end();
       ++it)                                                         
   {
      /* Get handle to foot-bot entity */
      CFootBotEntity& cFootBot = *any_cast<CFootBotEntity*>(it->second);

      /* Get the position of the foot-bot on the ground as a CVector2 */
      CVector2 cPos;
      cPos.Set(cFootBot.GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
               cFootBot.GetEmbodiedEntity().GetOriginAnchor().Position.GetY());
      
      output_file <<GetSpace().GetSimulationClock() << ","; // simulation clock
      output_file << cFootBot.GetId() << ",";// robot id
	   output_file << Distance(cPos,lightPos) << ",";    // robot distance to target
      output_file <<cPos.GetX() << "," <<cPos.GetY() << ",";// robot x and y position
      output_file << (Distance(cPos, lightPos) < 1) << std::endl; // Add a 0 column in the final column to indicate whether a particualr robot is currently within 1 metre of the light source., 0 is flse, 1 is true

      //TIMELIMIT MUST NOT EXCEED MAX SIMULATION TIME (EXPERIMENT LENGTH)
      if (Distance(cPos,lightPos)<1 && GetSpace().GetSimulationClock() < time_limit){// if the euclidean distance is less than 1 meter in the simulation while the time limit has not been reached, if time limit is exceeded then swarm has been unsuccessful in the time given.
      
            if (std::find(goal.begin(), goal.end(), cFootBot.GetId()) != goal.end()){ 
               //If the robot has already reached the end then do nothing
            }
            else{
               goal.push_back(cFootBot.GetId());// robots have just readched the end, add then to goal reached vector                                                                               
            //output_file << cFootBot.GetId() << " has reached the goal" << std::endl;
            //output_file <<"Robot " <<cFootBot.GetId() << ", "<< "Reached the light at position: " << cPos << " Operation took: "<<GetSpace().GetSimulationClock() <<" Seconds" <<std::endl;
            //CRoomobstacleLoopFunctions::Destroy();

            if(goal.size() == m_cFootbots.size()){//if all robots have reached the end the simulation is successful.onedri
               cout << "All robots reached objective at: " << GetSpace().GetSimulationClock() << std::endl;
            }             
         }
      } 
         //possible add a time limit and if robots fail to reach goal in that time print out something?       
   }
}
/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CRoomobstacleLoopFunctions, "room_obstacle_loop_functions")
