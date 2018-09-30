#ifndef ENGINE_WINDOW_GRAPHICS_H
#define ENGINE_WINDOW_GRAPHICS_H

#include <vector>

#include "engine/Utility/Containers/Grid.h"
#include "engine/Utility/Math/Vector.h"
#include "engine/Utility/Typedefs.h"

//##############################################################################
unsigned const NumberOfColors = 16;

//##############################################################################
struct Color;
class Image;
struct AsciiImageData;
class AsciiImage;

//##############################################################################
struct Color : public VectorBase<byte, 4, Color>
{
  Color(void);
  Color(byte r, byte g, byte b);
  Color(byte r, byte g, byte b, byte a);
  Color(std::uint32_t colorCode);

  template <typename U, typename UDerived>
  Color(VectorBase<U, 3, UDerived> const & vec);

  template <typename U, typename UDerived>
  Color(VectorBase<U, 4, UDerived> const & vec);

  byte * begin(void);
  byte * end(void);
  byte const * begin(void) const;
  byte const * end(void) const;

  byte r;
  byte g;
  byte b;
  byte a;
};

//##############################################################################
class Image : public Grid<Color>
{
public:
  Image(void) = default;
  Image(Image const & image) = default;
  Image(Image && image) = default;
  Image(std::string const & filename);
  Image(UVec2 const & size);
  Image(UVec2 const & size, Color const & color);
  Image(UVec2 const & size, Color const * buffer);
  Image(unsigned width, unsigned height);
  Image(unsigned width, unsigned height, Color const & color);
  Image(unsigned width, unsigned height, Color const * buffer);

  Image & operator =(Image const & image) = default;
  Image & operator =(Image && image) = default;

  Image & Scale(unsigned width, unsigned height);
  Image Scaled(unsigned width, unsigned height) const;

  bool Load(std::string const & filename);
};

//##############################################################################
struct AsciiImageData
{
  AsciiImageData(void);
  AsciiImageData(char character);
  AsciiImageData(byte foregroundColorIndex, byte backgroundColorIndex);
  AsciiImageData(char character, byte foregroundColorIndex,
    byte backgroundColorIndex);

  char character;
  byte foregroundColorIndex : 4;
  byte backgroundColorIndex : 4;
};

//##############################################################################
class AsciiImage : public Grid<AsciiImageData>
{
public:
  AsciiImage(void) = default;
  AsciiImage(AsciiImage const & asciiImage) = default;
  AsciiImage(AsciiImage && asciiImage) = default;
  AsciiImage(UVec2 const & size);
  AsciiImage(UVec2 const & size, AsciiImageData const & value);
  AsciiImage(UVec2 const & size, AsciiImageData const * buffer);
  AsciiImage(unsigned width, unsigned height);
  AsciiImage(unsigned width, unsigned height, AsciiImageData const & value);
  AsciiImage(unsigned width, unsigned height, AsciiImageData const * buffer);
  AsciiImage(unsigned width, unsigned height, Image & image);

  AsciiImage & operator =(AsciiImage const & asciiImage) = default;
  AsciiImage & operator =(AsciiImage && asciiImage) = default;

  void Render(void) const;
};

//##############################################################################
void SetColorPalette(Color const * colors, unsigned colorCount,
  unsigned offset);

//##############################################################################
void GetColorPalette(Color * colors, unsigned colorCount, unsigned offset);

//##############################################################################
template <typename U, typename UDerived>
Color::Color(VectorBase<U, 3, UDerived> const & vec) :
  r(vec[0]),
  g(vec[1]),
  b(vec[2]),
  a(255)
{}

//##############################################################################
template <typename U, typename UDerived>
Color::Color(VectorBase<U, 4, UDerived> const & vec) :
  r(vec[0]),
  g(vec[1]),
  b(vec[2]),
  a(vec[3])
{}

#endif
