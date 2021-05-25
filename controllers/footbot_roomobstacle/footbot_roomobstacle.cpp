/* Include the controller definition */
#include "footbot_roomobstacle.h"
#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/core/utility/logging/argos_log.h>

//-----------------------------------------added to  simuale vectors?-----------------------------------------------
/* 2D vector definition */
#include <argos3/core/utility/math/vector2.h>
//------------------------------------------------------------------------------------------------------------------

/****************************************/

CFootBotNavigation::CFootBotNavigation() :
   m_pcWheels(NULL),
   m_pcProximity(NULL),
   m_pcLight(NULL),
   m_pcLEDs(NULL),
   m_pcCamera(NULL),
   m_cAlpha(10.0f),
   m_pcRNG(NULL),//Added to create random vector
   m_fDelta(0.5f),
   m_fWheelVelocity(2.5f),
   m_cGoStraightAngleRange(-ToRadians(m_cAlpha),
                           ToRadians(m_cAlpha)) {}

void CFootBotNavigation::SWheelTurningParams::Init(TConfigurationNode& t_node) {
   try {
      TurningMechanism = NO_TURN;
      CDegrees cAngle;
      GetNodeAttribute(t_node, "hard_turn_angle_threshold", cAngle);
      HardTurnOnAngleThreshold = ToRadians(cAngle);
      GetNodeAttribute(t_node, "soft_turn_angle_threshold", cAngle);
      SoftTurnOnAngleThreshold = ToRadians(cAngle);
      GetNodeAttribute(t_node, "no_turn_angle_threshold", cAngle);
      NoTurnAngleThreshold = ToRadians(cAngle);
      GetNodeAttribute(t_node, "max_speed", MaxSpeed);
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("Error initializing controller wheel turning parameters.", ex);
   }
}

/****************************************/
/****************************************/

void CFootBotNavigation::SFlockingInteractionParams::Init(TConfigurationNode& t_node) {
   try {
      GetNodeAttribute(t_node, "target_distance", TargetDistance);
      GetNodeAttribute(t_node, "gain", Gain);
      GetNodeAttribute(t_node, "exponent", Exponent);
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("Error initializing controller flocking parameters.", ex);
   }
}

/****************************************/
/****************************************/

/*
 * This function is a generalization of the Lennard-Jones potential
 */
Real CFootBotNavigation::SFlockingInteractionParams::GeneralizedLennardJones(Real f_distance) {
   Real fNormDistExp = ::pow(TargetDistance / f_distance, Exponent);
   return -Gain / f_distance * (fNormDistExp * fNormDistExp - fNormDistExp);
}

/****************************************/
/****************************************/


void CFootBotNavigation::Init(TConfigurationNode& t_node) {
   /*
    * Get sensor/actuator handles
    *
    * The passed string (ex. "differential_steering") corresponds to the XML tag of the
    * device whose handle we want to have. For a list of allowed values, type at the
    * command prompt:
    *
    * $ argos3 -q actuators
    *
    * to have a list of all the possible actuators, or
    *
    * $ argos3 -q sensors
    *
    * to have a list of all the possible sensors.
    *
    * NOTE: ARGoS creates and initializes actuators and sensors internally, on the basis of
    *       the lists provided the configuration file at the
    *       <controllers><footbot_diffusion><actuators> and
    *       <controllers><footbot_diffusion><sensors> sections. If you forgot to
    *       list a device in the XML and then you request it here, an error occurs.
    */
   m_pcWheels = GetActuator<CCI_DifferentialSteeringActuator          >("differential_steering");
   m_pcLight  = GetSensor  <CCI_FootBotLightSensor                    >("footbot_light");
   m_pcLEDs   = GetActuator<CCI_LEDsActuator                          >("leds");
   m_pcCamera = GetSensor  <CCI_ColoredBlobOmnidirectionalCameraSensor>("colored_blob_omnidirectional_camera");
   //-------------------------------added to allow the robots to see----------------------------
   m_pcProximity = GetSensor  <CCI_FootBotProximitySensor      >("footbot_proximity");
   //---------------------------Addint random vector to enable the robots to move when stuck---------------------

   /*
    * Parse the config file
    */
   //------------------------------ added to help the robots move straight by default-------------------------- 
   GetNodeAttributeOrDefault(t_node, "alpha", m_cAlpha, m_cAlpha);
   m_cGoStraightAngleRange.Set(-ToRadians(m_cAlpha), ToRadians(m_cAlpha));
   GetNodeAttributeOrDefault(t_node, "delta", m_fDelta, m_fDelta);
   GetNodeAttributeOrDefault(t_node, "velocity", m_fWheelVelocity, m_fWheelVelocity); 
   //--------------------------------------------------------------------------------------------------------------
   try {
      /* Wheel turning */
      m_sWheelTurningParams.Init(GetNode(t_node, "wheel_turning"));
      /* Flocking-related */
      m_sFlockingParams.Init(GetNode(t_node, "flocking"));
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("Error parsing the controller parameters.", ex);
   }

   /*
    * Other init stuff
    */
   /* Enable camera filtering */
   m_pcCamera->Enable();
   /* Set beacon color to all red to be visible for other robots */
   m_pcLEDs->SetSingleColor(12, CColor::RED);
   m_pcRNG = CRandom::CreateRNG("argos");
}

/****************************************/
/****************************************/

void CFootBotNavigation::ControlStep() {
   //If target is seen 
   SetWheelSpeedsFromVector(VectorToLight() + FlockingVector() + ObstacleVector());

}

/****************************************/
/****************************************/

CVector2 CFootBotNavigation::VectorToLight() {
   /* Get light readings */
   const CCI_FootBotLightSensor::TReadings& tReadings = m_pcLight->GetReadings();
   /* Calculate a normalized vector that points to the closest light */
   CVector2 cAccum;
   for(size_t i = 0; i < tReadings.size(); ++i) {
      cAccum += CVector2(tReadings[i].Value, tReadings[i].Angle);
   }
   if(cAccum.Length() > 0.0f) {
      /* Make the vector long as 1/4 of the max speed */
      cAccum.Normalize();
      cAccum *= 0.25f * m_sWheelTurningParams.MaxSpeed;
   }
   return cAccum;
}

/****************************************/
/****************************************/



/****************************************/
/****************************************/

CVector2 CFootBotNavigation::FlockingVector() {


   /* Get the camera readings */
   const CCI_ColoredBlobOmnidirectionalCameraSensor::SReadings& sReadings = m_pcCamera->GetReadings();
   /* Go through the camera readings to calculate the flocking interaction vector */
   if(! sReadings.BlobList.empty()) {
      CVector2 cAccum;
      Real fLJ;
      size_t unBlobsSeen = 0;
	
      for(size_t i = 0; i < sReadings.BlobList.size(); ++i) {

         /*
          * The camera perceives the light as a yellow blob
          * The robots have their red beacon on
          * So, consider only red blobs
          * In addition: consider only the closest neighbors, to avoid
          * attraction to the farthest ones. Taking 180% of the target
          * distance is a good rule of thumb.
          */
         if(sReadings.BlobList[i]->Color == CColor::RED &&
            sReadings.BlobList[i]->Distance < m_sFlockingParams.TargetDistance * 1.80f) {
            
            
            /*
             * Take the blob distance and angle
             * With the distance, calculate the Lennard-Jones interaction force
             * Form a 2D vector with the interaction force and the angle
             * Sum such vector to the accumulator
             */
            /* Calculate LJ */
            fLJ = m_sFlockingParams.GeneralizedLennardJones(sReadings.BlobList[i]->Distance);
            /* Sum to accumulator */
            cAccum += CVector2(fLJ,
                               sReadings.BlobList[i]->Angle);
            /* Increment the blobs seen counter */
            ++unBlobsSeen;
         }
      }
      if(unBlobsSeen > 0) {
         /* Divide the accumulator by the number of blobs seen */
         cAccum /= unBlobsSeen;
         /* Clamp the length of the vector to the max speed */
         if(cAccum.Length() > m_sWheelTurningParams.MaxSpeed) {
            cAccum.Normalize();
            cAccum *= m_sWheelTurningParams.MaxSpeed;
         }
         return cAccum;
      }
      else
         return CVector2();
   }
   else {
      return CVector2();
   }
}

/************************Preventing swarm from getting separated and stuck by enabling individual avoidance and deadlock counter****************/


CVector2 CFootBotNavigation::ObstacleVector() {
   /* Get readings from proximity sensor */
   const CCI_FootBotProximitySensor::TReadings& tProxReads = m_pcProximity->GetReadings();
   
   /* Sum them together */
   CVector2 cAccumulator;

   for(size_t i = 0; i < tProxReads.size(); ++i) {
      cAccumulator += CVector2(tProxReads[i].Value, tProxReads[i].Angle);
   }

   if(cAccumulator.Length() == 0)
      return CVector2(1, 0); // Drive forward if no obstacles detected

   /* Average based on number of sensors */
   cAccumulator /= tProxReads.size();

   /* Normalise the vector */
   cAccumulator.Normalize();

   /* Invert vector to point away from detected obstacles */
   cAccumulator *= -1;

   /* Add a random vector to help escape deadlock situations  */
   cAccumulator += CVector2(m_pcRNG->Gaussian(0.5), m_pcRNG->Uniform(CRadians::UNSIGNED_RANGE));

   /* Increase the influence of the vector */
   cAccumulator *= 60;

   return cAccumulator;
}

/****************************************/


void CFootBotNavigation::SetWheelSpeedsFromVector(const CVector2& c_heading) {

//---------------------------------added to allow robot to avoid obstacles-----------------------------
   /* Get readings from proximity sensor */
   const CCI_FootBotProximitySensor::TReadings& tProxReads = m_pcProximity->GetReadings();
   /* Sum them together */
   CVector2 cAccumulator;
   for(size_t i = 0; i < tProxReads.size(); ++i) {
      cAccumulator += CVector2(tProxReads[i].Value, tProxReads[i].Angle);
   }
   cAccumulator /= tProxReads.size();
   /* If the angle of the vector is small enough and the closest obstacle
    * is far enough, continue going straight, otherwise curve a little
    */
   CRadians cAngle = cAccumulator.Angle();

    if(m_cGoStraightAngleRange.WithinMinBoundIncludedMaxBoundIncluded(cAngle) &&
      cAccumulator.Length() < m_fDelta){// need one more conditionn usch as blob seen   
      
      //--------------------------------------------------Follow the flocking vector------------------------------------------------------------------------	

      //std::cout <<"Flocking to target..."<< std::endl;
      /* Get the heading angle */
      CRadians cHeadingAngle = c_heading.Angle().SignedNormalize();
      /* Get the length of the heading vector */
      Real fHeadingLength = c_heading.Length();
      /* Clamp the speed so that it's not greater than MaxSpeed */
      Real fBaseAngularWheelSpeed = Min<Real>(fHeadingLength, m_sWheelTurningParams.MaxSpeed);
      /* State transition logic */
      if(m_sWheelTurningParams.TurningMechanism == SWheelTurningParams::HARD_TURN) {
         if(Abs(cHeadingAngle) <= m_sWheelTurningParams.SoftTurnOnAngleThreshold) {
            m_sWheelTurningParams.TurningMechanism = SWheelTurningParams::SOFT_TURN;
         }
      }
      if(m_sWheelTurningParams.TurningMechanism == SWheelTurningParams::SOFT_TURN) {
         if(Abs(cHeadingAngle) > m_sWheelTurningParams.HardTurnOnAngleThreshold) {
            m_sWheelTurningParams.TurningMechanism = SWheelTurningParams::HARD_TURN;
         }
         else if(Abs(cHeadingAngle) <= m_sWheelTurningParams.NoTurnAngleThreshold) {
            m_sWheelTurningParams.TurningMechanism = SWheelTurningParams::NO_TURN;
         }
      }
      if(m_sWheelTurningParams.TurningMechanism == SWheelTurningParams::NO_TURN) {
         if(Abs(cHeadingAngle) > m_sWheelTurningParams.HardTurnOnAngleThreshold) {
            m_sWheelTurningParams.TurningMechanism = SWheelTurningParams::HARD_TURN;
         }
         else if(Abs(cHeadingAngle) > m_sWheelTurningParams.NoTurnAngleThreshold) {
            m_sWheelTurningParams.TurningMechanism = SWheelTurningParams::SOFT_TURN;
         }
      }
      /* Wheel speeds based on current turning state */
      Real fSpeed1, fSpeed2;
      switch(m_sWheelTurningParams.TurningMechanism) {
         case SWheelTurningParams::NO_TURN: {
            /* Just go straight */
            fSpeed1 = fBaseAngularWheelSpeed;
            fSpeed2 = fBaseAngularWheelSpeed;
            break;
         }
         case SWheelTurningParams::SOFT_TURN: {
            /* Both wheels go straight, but one is faster than the other */
            Real fSpeedFactor = (m_sWheelTurningParams.HardTurnOnAngleThreshold - Abs(cHeadingAngle)) / m_sWheelTurningParams.HardTurnOnAngleThreshold;
            fSpeed1 = fBaseAngularWheelSpeed - fBaseAngularWheelSpeed * (1.0 - fSpeedFactor);
            fSpeed2 = fBaseAngularWheelSpeed + fBaseAngularWheelSpeed * (1.0 - fSpeedFactor);
            break;
         }
         case SWheelTurningParams::HARD_TURN: {
            /* Opposite wheel speeds */
            fSpeed1 = -m_sWheelTurningParams.MaxSpeed;
            fSpeed2 =  m_sWheelTurningParams.MaxSpeed;
            break;
         }
      }
      /* Apply the calculated speeds to the appropriate wheels */
      Real fLeftWheelSpeed, fRightWheelSpeed;
      if(cHeadingAngle > CRadians::ZERO) {
         /* Turn Left */
         fLeftWheelSpeed  = fSpeed1;
         fRightWheelSpeed = fSpeed2;
      }
      else {
         /* Turn Right */
         fLeftWheelSpeed  = fSpeed2;
         fRightWheelSpeed = fSpeed1;
      }
      /* Finally, set the wheel speeds */
      m_pcWheels->SetLinearVelocity(fLeftWheelSpeed, fRightWheelSpeed);

   }
      //------------------------------------------------------------------------------------------
   else {
      /* Turn, depending on the sign of the angle */
      if(cAngle.GetValue() > 0.0f) {
	 m_pcWheels->SetLinearVelocity(m_fWheelVelocity, 0.0f);
      }
      else {
	 m_pcWheels->SetLinearVelocity(0.0f, m_fWheelVelocity);
      }
   }
}

//---------------------------------------------------------------------------------------	

/****************************************/
/****************************************/

/*
 * This statement notifies ARGoS of the existence of the controller.
 * It binds the class passed as first argument to the string passed as second argument.
 * The string is then usable in the XML configuration file to refer to this controller.
 * When ARGoS reads that string in the XML file, it knows which controller class to instantiate.
 * See also the XML configuration files for an example of how this is used.
 */
REGISTER_CONTROLLER(CFootBotNavigation, "footbot_navigation_controller")
