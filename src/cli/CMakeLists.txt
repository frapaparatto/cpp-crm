add_library(insura_cli STATIC)

target_link_libraries(insura_cli
PRIVATE
  insura_domain
  insura_service
)

target_sources(insura_cli
PRIVATE
  menu.cpp
  client_view.cpp
  policy_view.cpp
  interaction_view.cpp
  application.cpp
)



