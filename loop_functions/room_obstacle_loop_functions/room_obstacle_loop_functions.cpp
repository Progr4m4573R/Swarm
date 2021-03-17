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
      TConfigurationNode& tRoomobstacle = GetNode(t_node, "room_obstacle");
      //https://stackoverflow.com/questions/7430248/creating-a-new-directory-in-c
      
      cwd[PATH_MAX];
      if(getcwd(cwd, sizeof(cwd)) !=NULL){
         printf("Current working directory:%s\n", cwd);
      }else{
         perror("getcwd() error");
      }
      
      time_t now = time(0);
      char* dt = ctime(&now);
      
      
      //should go up two directories and make the performance_folder in the Swarm directory
      working_directory = cwd+../../performance_folder+dt;
      
      
      struct stat st = {0};// checking if the directory already exists

      if (stat(working_directory, &st) == -1){
         mkdir(working_directory, 0700);
      }
      
      
      /* Get the output file name from XML */
      GetNodeAttribute(tRoomobstacle, "output", m_strOutput);
      /* Open the file, erasing its contents */
      m_cOutput.open(m_strOutput.c_str(), std::ios_base::trunc | std::ios_base::out);
      
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
   m_cOutput.close();
   /* Open the file, erasing its contents */
   m_cOutput.open(m_strOutput.c_str(), std::ios_base::trunc | std::ios_base::out);

}

/****************************************/
/****************************************/

void CRoomobstacleLoopFunctions::Destroy() {
   /* Close the file */
   m_cOutput.close();
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
     
      m_cOutput <<"Clock: "<< GetSpace().GetSimulationClock() << ",";
      m_cOutput <<"Robot ID: "<< cFootBot.GetId() << ",";
      m_cOutput <<"Robot position: " <<cPos << std::endl;


      if (Distance(cPos,lightPos)<1){// if the euclidean distance is less than 1 meter in the simulation
      m_cOutput <<"Robot " <<cFootBot.GetId() << ", "<< "Reached the light at position: " << cPos << "Operation took: "<<GetSpace().GetSimulationClock() <<" Seconds" <<std::endl;
      break;
      }
      else{
	m_cOutput << "Robots in hot pursuit......."<<std::endl;
      }
    
  }
  //possible add a time limit and if robots fail to reach goal in that time print out something?
}
/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CRoomobstacleLoopFunctions, "room_obstacle_loop_functions")
