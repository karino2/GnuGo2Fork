# This is only for Android NDK. Use configure-make for PC.

cmake_minimum_required(VERSION 3.5)

project("gnugo2")

add_library(engine
      engine/attdef.c
      engine/dragon.c
      engine/filllib.c
      engine/fuseki.c
      engine/genmove.c
      engine/globals.c
      engine/hash.c
      engine/matchpat.c
      engine/moyo.c
      engine/optics.c
      engine/reading.c
      engine/semeai.c
      engine/sethand.c
      engine/shapes.c
      engine/showbord.c
      engine/utils.c
      engine/worm.c
)

target_compile_definitions(engine PRIVATE HAVE_CONFIG_H BUILDING_GNUGO_ENGINE)
target_include_directories(engine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/android
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/interface
    ${CMAKE_CURRENT_SOURCE_DIR}/patterns
    ${CMAKE_CURRENT_SOURCE_DIR}/sgf
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

add_library(patterns
  patterns/connections.c
  patterns/helpers.c
  patterns/conn.c
  patterns/patterns.c
  patterns/eyes.c
)
target_compile_definitions(patterns PRIVATE BUILDING_GNUGO_ENGINE)
target_include_directories(patterns PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/android
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)


add_library(gnugo2 SHARED android/dummy.c)
target_include_directories(gnugo2 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/android
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/sgf
)


target_link_libraries(gnugo2
                      engine
                      patterns
                      android
                      log)