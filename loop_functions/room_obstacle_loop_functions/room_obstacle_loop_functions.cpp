#include "room_obstacle_loop_functions.h"
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
#include <controllers/footbot_roomobstacle/footbot_roomobstacle.h>

/****************************************/
/****************************************/

CRoomobstacleLoopFunctions::CRoomobstacleLoopFunctions() 
{

}

/****************************************/
/****************************************/

void CRoomobstacleLoopFunctions::Init(TConfigurationNode& t_node) {
   try {
      TConfigurationNode& tRoomobstacle = GetNode(t_node, "room_obstacle");



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



      /* Get handle to foot-bot entity */
      CFootBotEntity& cFootBot = *any_cast<CFootBotEntity*>(it->second);

      /* Get the position of the foot-bot on the ground as a CVector2 */
      CVector2 cPos;
      cPos.Set(cFootBot.GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
               cFootBot.GetEmbodiedEntity().GetOriginAnchor().Position.GetY());

     /*Get handle to light entity*/
     m_pcLight& cLight = *any_cast<m_pcLight*>(it->second);
     /*get the position of the light*/	
     CVector2 lightPos;
     lightPos.set(cLight.GetEmbodiedEntity().GetOriginAnchor().Position.GetX()
                  cLight.GetEmbodiedEntity().GetOriginAnchor().Position.GetY());
     
      m_cOutput <<"Clock: "<< GetSpace().GetSimulationClock() << ",";
      m_cOutput <<"Robot ID: "<< cFootBot.GetId() << ",";
      m_cOutput <<"Robot position: " <<cPos << std::endl;
      m_cOutput <<"Light position: "<< lightPos << std::end1;


   for(CSpace::TMapPerType::iterator it = m_cFootbots.begin();
       it != m_cFootbots.end();
       ++it)
   {
      
      if (cPos.GetX() && cPos.GetY() <= lightPos.GetX() && lightPos.GetY()){
      m_cOutput <<"Robot " <<cFootBot.GetID() << ", "<< "Reached the light at position: " << cPos << "Operation took: print time taken here" <<std::endl;
      }
      else{
	m_cOutput << "Robots in hot pursuit"<<std::endl;
      }
   }
    
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CRoomobstacleLoopFunctions, "room_obstacle_loop_functions")
