add_library(insura_domain STATIC)

target_sources(insura_domain
  PRIVATE
    client.cpp
    policy.cpp
    interaction.cpp
)

target_include_directories(insura_domain
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
