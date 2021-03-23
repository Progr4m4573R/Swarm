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
/*Things to add*/

//check if a robot has reached a destination 
//check if the robot state has changed/ change a robot state after they reach a destination

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
     
      // output_file <<"Clock: "<< GetSpace().GetSimulationClock() << ",";
      // output_file <<"Robot ID: "<< cFootBot.GetId() << ",";
      // output_file <<"Robot position: " <<cPos << std::endl;
      
      output_file << GetSpace().GetSimulationClock() << ",";
      output_file << cFootBot.GetId() << ",";
      output_file <<cPos.GetX() << "," <<cPos.GetY() << std::endl;


      if (Distance(cPos,lightPos)<1){// if the euclidean distance is less than 1 meter in the simulation
      output_file <<"Robot " <<cFootBot.GetId() << ", "<< "Reached the light at position: " << cPos << "Operation took: "<<GetSpace().GetSimulationClock() <<" Seconds" <<std::endl;
      break;
      }
      else{
	      //output_file << "Robots in hot pursuit......."<<std::endl;
      }
    
  }
  //possible add a time limit and if robots fail to reach goal in that time print out something?
}
/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CRoomobstacleLoopFunctions, "room_obstacle_loop_functions")
