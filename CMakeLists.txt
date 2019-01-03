
add_contract(eosio.nft eosio.nft ${CMAKE_CURRENT_SOURCE_DIR}/eosio.nft.cpp)
target_include_directories(eosio.nft.wasm
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(eosio.nft.wasm
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
