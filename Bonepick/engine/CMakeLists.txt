project(BonepickEngine C CXX)

add_library(BonepickEngine STATIC)

target_include_directories(BonepickEngine
PUBLIC
  .
)

target_sources(BonepickEngine
PUBLIC
  io/ascii/Graphics.h
  io/ascii/Input.h
  io/common/Graphics.h
  io/common/Input.h
  io/window/Graphics.h
  io/window/Input.h
  system/Entity.h
  system/EntityManager.h
  utility/Bitset.h
  utility/Blob.h
  utility/CommandOptions.h
  utility/containers/Array.h
  utility/containers/DumbVariant.h
  utility/containers/External.h
  utility/containers/Grid.h
  utility/containers/Optional.h
  utility/containers/Shared.h
  utility/containers/SortedArray.h
  utility/containers/Tuple.h
  utility/containers/Variant.h
  utility/DataLayout.h
  utility/Debug.h
  utility/Macros.h
  utility/math/MathConstants.h
  utility/math/Vector.h
  utility/math/VectorMath.h
  utility/Random.h
  utility/String.h
  utility/TemplateTools.h
  utility/Token.h
  utility/Typedefs.h
PRIVATE
  io/ascii/Graphics.cpp
  io/ascii/Input.cpp
  io/common/Graphics.cpp
  io/common/Input.cpp
  io/window/Graphics.cpp
  io/window/Input.cpp
  utility/Bitset.cpp
  utility/Blob.cpp
  utility/CommandOptions.cpp
  utility/DataLayout.cpp
  utility/Debug.cpp
  utility/Random.cpp
  utility/String.cpp
  utility/Token.cpp
)
