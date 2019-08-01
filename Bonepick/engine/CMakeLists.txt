project(BonepickEngine C CXX)

add_library(BonepickEngine STATIC)

set_property(TARGET BonepickEngine PROPERTY FOLDER engine)
set_property(TARGET BonepickEngine PROPERTY CMAKE_CXX_STANDARD 17)
set_property(TARGET BonepickEngine PROPERTY CXX_STANDARD_REQUIRED ON)
target_compile_features(BonepickEngine PUBLIC cxx_std_17)

target_link_libraries(BonepickEngine
PRIVATE
  stb
)

target_include_directories(BonepickEngine
PUBLIC
  .
)

set(inc
  ${CMAKE_CURRENT_SOURCE_DIR}/io/ascii/Graphics.h
  ${CMAKE_CURRENT_SOURCE_DIR}/io/ascii/Input.h
  ${CMAKE_CURRENT_SOURCE_DIR}/io/common/Graphics.h
  ${CMAKE_CURRENT_SOURCE_DIR}/io/common/Input.h
  ${CMAKE_CURRENT_SOURCE_DIR}/io/window/Graphics.h
  ${CMAKE_CURRENT_SOURCE_DIR}/io/window/Input.h
  ${CMAKE_CURRENT_SOURCE_DIR}/system/Entity.h
  ${CMAKE_CURRENT_SOURCE_DIR}/system/EntityManager.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/Bitset.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/Blob.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/CommandOptions.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/containers/Array.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/containers/DumbVariant.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/containers/External.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/containers/Grid.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/containers/Optional.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/containers/Shared.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/containers/SortedArray.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/containers/Tuple.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/containers/Variant.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/DataLayout.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/Debug.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/Macros.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/math/MathConstants.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/math/Vector.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/math/VectorMath.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/Random.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/String.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/TemplateTools.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/Token.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/Typedefs.h
)

set(src
  ${CMAKE_CURRENT_SOURCE_DIR}/io/ascii/Graphics.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/io/ascii/Input.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/io/common/Graphics.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/io/common/Input.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/io/window/Graphics.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/io/window/Input.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/Bitset.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/Blob.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/CommandOptions.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/DataLayout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/Debug.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/Random.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/String.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utility/Token.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX inc FILES ${inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX src FILES ${src})

target_sources(BonepickEngine
PUBLIC
  ${inc}
PRIVATE
  ${src}
)
