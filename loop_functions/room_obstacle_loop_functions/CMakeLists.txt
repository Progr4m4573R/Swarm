link_directories(${CMAKE_BINARY_DIR}/controllers/footbot_roomobstacle)
set(room_obstacle_loop_functions_SOURCES room_obstacle_loop_functions.cpp)

if(ARGOS_COMPILE_QTOPENGL)
  set(room_obstacle_loop_functions_SOURCES
    ${room_obstacle_loop_functions_SOURCES}
    )
endif(ARGOS_COMPILE_QTOPENGL)

add_library(room_obstacle_loop_functions MODULE ${room_obstacle_loop_functions_SOURCES})
target_link_libraries(room_obstacle_loop_functions
  footbot_roomobstacle
  argos3core_simulator
  argos3plugin_simulator_dynamics2d
  argos3plugin_simulator_entities
  argos3plugin_simulator_footbot
  argos3plugin_simulator_genericrobot
  argos3plugin_simulator_media)

if(ARGOS_COMPILE_QTOPENGL)
  target_link_libraries(room_obstacle_loop_functions argos3plugin_simulator_qtopengl)
endif(ARGOS_COMPILE_QTOPENGL)
