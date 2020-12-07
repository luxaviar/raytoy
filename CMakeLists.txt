#---------------------------------------------------------------------------------------------------
# CMake Build Configuration for the Ray Tracing Weekend Series
#
# See README.md for guidance.
#---------------------------------------------------------------------------------------------------

cmake_minimum_required ( VERSION 3.1.0 )

project(raytoy  LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

include_directories(src)
# include_directories(src/common)

# Source
set(COMMON_ALL
  src/common/image.cpp
  src/common/transform.cpp
  src/concurrent/thread_pool.cpp
  src/math/random.cpp
  src/hittable.cpp
)

# Executables
add_executable(random_sphere src/example/random_sphere.cpp ${COMMON_ALL})
add_executable(motion_blur src/example/motion_blur.cpp ${COMMON_ALL})
add_executable(checker src/example/checker.cpp ${COMMON_ALL})
add_executable(perlin src/example/perlin.cpp ${COMMON_ALL})
add_executable(image src/example/image_texture.cpp ${COMMON_ALL})
add_executable(light src/example/simple_light.cpp ${COMMON_ALL})
add_executable(cornell src/example/cornell.cpp ${COMMON_ALL})
add_executable(cornell_smoke src/example/cornell_smoke.cpp ${COMMON_ALL})
add_executable(cornell_aluminum src/example/cornell_aluminum.cpp ${COMMON_ALL})
add_executable(cornell_glass src/example/cornell_glass.cpp ${COMMON_ALL})
add_executable(final src/example/final.cpp ${COMMON_ALL})

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    TARGET_LINK_LIBRARIES(random_sphere pthread)
    TARGET_LINK_LIBRARIES(motion_blur pthread)
    TARGET_LINK_LIBRARIES(checker pthread)
    TARGET_LINK_LIBRARIES(perlin pthread)
    TARGET_LINK_LIBRARIES(image pthread)
    TARGET_LINK_LIBRARIES(light pthread)
    TARGET_LINK_LIBRARIES(cornell pthread)
    TARGET_LINK_LIBRARIES(cornell_smoke pthread)
    TARGET_LINK_LIBRARIES(cornell_aluminum pthread)
    TARGET_LINK_LIBRARIES(cornell_glass pthread)
    TARGET_LINK_LIBRARIES(final pthread)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

