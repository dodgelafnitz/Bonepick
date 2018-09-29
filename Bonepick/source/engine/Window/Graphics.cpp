#include <Windows.h>
#undef ERROR
#undef min
#undef max

#include "engine/Window/Graphics.h"

#include "engine/Utility/Debug.h"

#pragma warning(push)
#pragma warning(disable : 4312)
#pragma warning(disable : 4456)
#pragma warning(disable : 4457)

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x) ASSERT(x)
#include "external/stb_image.h"

#pragma warning(pop)

#define ENABLE_SHAPED_ASCII 0

namespace
{
  unsigned const AsciiPixelWidth = 3;

  //############################################################################
  struct AsciiShapeInfo
  {
    unsigned primaryIndex   : 5;
    unsigned secondaryIndex : 5;
    unsigned key            : 9;
    unsigned strong         : 1;
  };

  //############################################################################
  COLORREF GetApplyColorFromColor(Color color)
  {
    return RGB(color.r, color.g, color.b);
  }

  //############################################################################
  Color GetColorFromApplyColor(COLORREF color)
  {
    return Color(GetRValue(color), GetGValue(color), GetBValue(color));
  }

  //############################################################################
  unsigned GetColorDist(Color const & color0, Color const & color1)
  {
    IVec4 diff = IVec4(color0) - color1;

    return VectorSum(diff * diff);
  }

  //############################################################################
  WORD GetAttributesFromColors(unsigned foregroundColorIndex,
    unsigned backgroundColorIndex)
  {
    ASSERT(foregroundColorIndex < NumberOfColors);
    ASSERT(backgroundColorIndex < NumberOfColors);

    ASSERT(WORD(foregroundColorIndex | (backgroundColorIndex << 4)) ==
      WORD(
        (foregroundColorIndex & 0x1 ? FOREGROUND_BLUE      : 0) |
        (foregroundColorIndex & 0x2 ? FOREGROUND_GREEN     : 0) |
        (foregroundColorIndex & 0x4 ? FOREGROUND_RED       : 0) |
        (foregroundColorIndex & 0x8 ? FOREGROUND_INTENSITY : 0) |
        (backgroundColorIndex & 0x1 ? BACKGROUND_BLUE      : 0) |
        (backgroundColorIndex & 0x2 ? BACKGROUND_GREEN     : 0) |
        (backgroundColorIndex & 0x4 ? BACKGROUND_RED       : 0) |
        (backgroundColorIndex & 0x8 ? BACKGROUND_INTENSITY : 0)));

    return WORD(foregroundColorIndex | (backgroundColorIndex << 4));
  }

  //############################################################################
  Grid<unsigned> GetMatchedColorsForImage(
    Color * colors, unsigned count, Image const & image)
  {
    Grid<unsigned> imageColors(image.GetWidth(), image.GetHeight());

    ASSERT(count > 0);

    for (unsigned i = 0; i < image.GetHeight(); ++i)
    {
      for (unsigned j = 0; j < image.GetWidth(); ++j)
      {
        Color const & color = image.GetValue(j, i);

        byte closestColor = 0;
        unsigned closestDist = GetColorDist(colors[0], color);

        for (byte k = 1; k < count; ++k)
        {
          unsigned dist = GetColorDist(colors[k], color);
          if (closestDist > dist)
          {
            closestDist = dist;
            closestColor = k;
          }
        }

        imageColors.SetValue(j, i, closestColor);
      }
    }

    return imageColors;
  }

  //############################################################################
  void GetShapeColorInfoFromSegment(
    AsciiShapeInfo & shapeInfo, Grid<unsigned> const & segment)
  {
    unsigned colors[NumberOfColors] = { 0 };

    for (unsigned i = 0; i < segment.GetCount(); ++i)
    {
      ASSERT(segment[i] < NumberOfColors);

      ++colors[segment[i]];
    }

    unsigned const NonValue = 16;

    shapeInfo.primaryIndex   = NonValue;
    shapeInfo.secondaryIndex = NonValue;

    for (unsigned i = 0; i < NumberOfColors; ++i)
    {
      if (shapeInfo.primaryIndex == NonValue ||
        colors[shapeInfo.primaryIndex] < colors[i])
      {
        shapeInfo.secondaryIndex = shapeInfo.primaryIndex;
        shapeInfo.primaryIndex = i;
      }
      else if (shapeInfo.secondaryIndex == NonValue ||
        colors[shapeInfo.secondaryIndex] < colors[i])
      {
        shapeInfo.secondaryIndex = i;
      }
    }
  }

  //############################################################################
  void GetShapeKeyInfoFromSegment(
    AsciiShapeInfo & shapeInfo, Grid<unsigned> const & segment,
    unsigned colorIndex)
  {
    shapeInfo.key = 0;
    int strongIndicator = 0;

    for (unsigned i = 0; i < segment.GetCount(); ++i)
    {
      shapeInfo.key <<= 1;

      if (segment[i] == colorIndex)
      {
        shapeInfo.key |= 0x1;
        ++strongIndicator;
      }
      else
        --strongIndicator;

      shapeInfo.strong = (strongIndicator > 0);
    }
  }

  //############################################################################
  AsciiShapeInfo GetShapeFromSegment(Grid<unsigned> const & segment)
  {
    ASSERT(segment.GetCount() == AsciiPixelWidth * AsciiPixelWidth);

    AsciiShapeInfo shape;

    GetShapeColorInfoFromSegment(shape, segment);
    GetShapeKeyInfoFromSegment(shape, segment, shape.primaryIndex);

    return shape;
  }

  //############################################################################
  unsigned GetBitCount(unsigned value)
  {
    unsigned map = 1;
    unsigned count = 0;

    while (value)
    {
      if (value & map)
        ++count;

      value &= ~map;

      map <<= 1;
    }

    return count;
  }

  //############################################################################
  unsigned GetBinaryDist(unsigned val0, unsigned val1)
  {
    return GetBitCount(val0 ^ val1);
  }

  //############################################################################
  char GetCharacterFromShapeKey(unsigned key, bool & invert)
  {
    struct CharacterResultInfo
    {
      unsigned shape;
      char character;
    };

    static const CharacterResultInfo info[] =
    {
      { 0b000000000, ' ' },
      { 0b010111101, 'A' },
      { 0b010111111, 'A' },
      { 0b011100011, 'C' },
      { 0b110101110, 'D' },
      { 0b101111101, 'H' },
      { 0b111010111, 'I' },
      { 0b111010110, 'J' },
      { 0b101110101, 'K' },
      { 0b100100111, 'L' },
      { 0b010101010, 'O' },
      { 0b111111100, 'P' },
      { 0b010101011, 'Q' },
      { 0b111110101, 'R' },
      { 0b111010010, 'T' },
      { 0b101101111, 'U' },
      { 0b101101010, 'V' },
      { 0b101010101, 'X' },
      { 0b101010010, 'Y' },
      { 0b100100100, '(' },
      { 0b010010010, '|' },
      { 0b001001001, ')' },
      { 0b111100111, '[' },
      { 0b111001111, ']' },
      { 0b010100010, '<' },
      { 0b001010001, '<' },
      { 0b110001110, '>' },
      { 0b100010100, '>' },
      { 0b010001010, '>' },
      { 0b010111010, '+' },
      { 0b001010100, '/' },
      { 0b100010001, '\\' },
      { 0b000101010, 'v' },
      { 0b101010000, 'v' },
      { 0b010101000, '^' },
      { 0b000010101, '^' },
      { 0b000111000, '-' },
      { 0b000000111, '_' },
      { 0b100000000, '\'' },
      { 0b010000000, '\'' },
      { 0b001000000, '\'' },
      { 0b000100000, '-' },
      { 0b000010000, '-' },
      { 0b000001000, '-' },
      { 0b000000100, '.' },
      { 0b000000010, '.' },
      { 0b000000001, '.' },
    };

    unsigned closestIndex = 0;
    unsigned closestDist = GetBinaryDist(info[0].shape, key);
    bool     closestIsInverted = false;

    for (unsigned i = 1; i < sizeof(info) / sizeof(*info); ++i)
    {
      unsigned dist = GetBinaryDist(info[i].shape, key);

      if (dist < closestDist)
      {
        closestDist = dist;
        closestIndex = i;
      }

      if (closestDist == 0)
        break;
    }

    for (unsigned i = 0; i < sizeof(info) / sizeof(*info); ++i)
    {
      unsigned dist = GetBinaryDist(0b111111111 ^ info[i].shape, key);

      if (dist < closestDist)
      {
        closestDist = dist;
        closestIndex = i;
        closestIsInverted = true;
      }

      if (closestDist == 0)
        break;
    }

    invert = closestIsInverted;
    return info[closestIndex].character;
  }

  //############################################################################
  AsciiImageData AsciiImageDataFromShape(AsciiShapeInfo const & shape)
  {
    bool invert;
    char character = GetCharacterFromShapeKey(shape.key, invert);

    return AsciiImageData(
      character,
      byte(invert ? shape.secondaryIndex : shape.primaryIndex),
      byte(invert ? shape.primaryIndex : shape.secondaryIndex));
  }

  //############################################################################
  AsciiImageData GetAsciiDataForImageColors(
    unsigned xPos, unsigned yPos, Grid<unsigned> const & imageColors)
  {
    Grid<unsigned> segment = imageColors.GetSubGrid(
      xPos * AsciiPixelWidth, yPos * AsciiPixelWidth,
      AsciiPixelWidth, AsciiPixelWidth);

    AsciiShapeInfo shapeInfo = GetShapeFromSegment(segment);

    return AsciiImageDataFromShape(shapeInfo);
  }
}

//##############################################################################
Color::Color(void) :
  r(0), g(0), b(0), a(255)
{}

//##############################################################################
Color::Color(
  byte r, byte g, byte b) :
  r(r), g(g), b(b), a(255)
{}

//##############################################################################
Color::Color(
  byte r, byte g, byte b, byte a) :
  r(r), g(g), b(b), a(a)
{}

//##############################################################################
Color::Color(std::uint32_t colorCode) :
  r((colorCode >> 0)  & 0xff),
  g((colorCode >> 8)  & 0xff),
  b((colorCode >> 16) & 0xff),
  a((colorCode >> 24) & 0xff)
{}

//##############################################################################
byte * Color::begin(void)
{
  return &r;
}

//##############################################################################
byte * Color::end(void)
{
  return &r + Size;
}

//##############################################################################
byte const * Color::begin(void) const
{
  return &r;
}

//##############################################################################
byte const * Color::end(void) const
{
  return &r + Size;
}

/*
GetStdHandle
SetConsoleTextAttribute
SetConsoleCursorPosition
GetConsoleScreenBufferInfoEx
PeekConsoleInput
SetConsoleWindowInfo
WriteConsoleOutput
*/

//##############################################################################
AsciiImage::AsciiImage(UVec2 const & size) :
  Grid(size)
{}

//##############################################################################
AsciiImage::AsciiImage(UVec2 const & size, AsciiImageData const & value) :
  Grid(size, value)
{}

//##############################################################################
AsciiImage::AsciiImage(UVec2 const & size, AsciiImageData const * buffer) :
  Grid(size, buffer)
{}

//##############################################################################
AsciiImage::AsciiImage(unsigned width, unsigned height) :
  AsciiImage(UVec2(width, height))
{}

//##############################################################################
AsciiImage::AsciiImage(unsigned width, unsigned height,
  AsciiImageData const & value) :
  AsciiImage(UVec2(width, height), value)
{}

//##############################################################################
AsciiImage::AsciiImage(unsigned width, unsigned height, AsciiImageData
  const * buffer) :
  AsciiImage(UVec2(width, height), buffer)
{}

//##############################################################################
AsciiImage::AsciiImage(unsigned width, unsigned height, Image & image) :
  AsciiImage(UVec2(width, height))
{
#if ENABLE_SHAPED_ASCII
  Image scaledImage =
    image.Scaled(width * AsciiPixelWidth, height * AsciiPixelWidth);
#else
  Image scaledImage =
    image.Scaled(width, height);
#endif

  Color colors[NumberOfColors];
  GetColorPalette(colors, NumberOfColors, 0);

  Grid<unsigned> imageColors =
    GetMatchedColorsForImage(colors, NumberOfColors, scaledImage);

  for (unsigned i = 0; i < GetHeight(); ++i)
  {
    for (unsigned j = 0; j < GetWidth(); ++j)
    {
#if ENABLE_SHAPED_ASCII
      SetValue(j, i, GetAsciiDataForImageColors(j, i, imageColors));
#else
      SetValue(j, i, AsciiImageData(' ', 0, byte(imageColors.GetValue(j, i))));
#endif
    }
  }
}

//##############################################################################
void AsciiImage::Render(void) const
{
  HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

  CONSOLE_SCREEN_BUFFER_INFOEX screenBufferInfo;
  screenBufferInfo.cbSize = sizeof(screenBufferInfo);

  GetConsoleScreenBufferInfoEx(consoleHandle, &screenBufferInfo);

  if (screenBufferInfo.dwSize.X != SHORT(GetWidth()) ||
    screenBufferInfo.dwSize.Y != SHORT(GetHeight()) ||
    screenBufferInfo.srWindow.Left != 0 ||
    screenBufferInfo.srWindow.Top != 0 || 
    screenBufferInfo.srWindow.Right != SHORT(GetWidth() - 1) ||
    screenBufferInfo.srWindow.Bottom != SHORT(GetHeight() - 1))
  {
    screenBufferInfo.dwSize.X = SHORT(GetWidth());
    screenBufferInfo.dwSize.Y = SHORT(GetHeight());
    screenBufferInfo.srWindow.Left = 0;
    screenBufferInfo.srWindow.Top = 0;
    screenBufferInfo.srWindow.Right = SHORT(GetWidth());
    screenBufferInfo.srWindow.Bottom = SHORT(GetHeight());

    SetConsoleScreenBufferInfoEx(consoleHandle, &screenBufferInfo);
  }

  CONSOLE_CURSOR_INFO cursorInfo;
  GetConsoleCursorInfo(consoleHandle, &cursorInfo);

  if (cursorInfo.bVisible)
  {
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
  }

  std::vector<CHAR_INFO> info(GetCount());

  for (unsigned i = 0; i < GetCount(); ++i)
  {
    info[i].Attributes = GetAttributesFromColors(GetValue(i).foregroundColorIndex,
      GetValue(i).backgroundColorIndex);

    info[i].Char.AsciiChar = GetValue(i).character;
  }

  COORD const size = { SHORT(GetWidth()), SHORT(GetHeight()) };
  COORD const topLeft = { 0, 0 };
  SMALL_RECT window = { 0, 0, SHORT(GetWidth() - 1), SHORT(GetHeight() - 1) };

  SetConsoleCursorPosition(consoleHandle, topLeft);
  WriteConsoleOutput(consoleHandle, info.data(), size, topLeft, &window);
}

//##############################################################################
Image::Image(std::string const & filename)
{
  Load(filename);
}

//##############################################################################
Image::Image(UVec2 const & size) :
  Grid(size)
{}

//##############################################################################
Image::Image(UVec2 const & size, Color const & color) :
  Grid(size, color)
{}

//##############################################################################
Image::Image(UVec2 const & size, Color const * buffer) :
  Grid(size, buffer)
{}

//##############################################################################
Image::Image(unsigned width, unsigned height) :
  Grid(width, height)
{}

//##############################################################################
Image::Image(unsigned width, unsigned height, Color const & color) :
  Grid(width, height, color)
{}

//##############################################################################
Image::Image(unsigned width, unsigned height, Color const * buffer) :
  Grid(width, height, buffer)
{}

//##############################################################################
Image & Image::Scale(unsigned width, unsigned height)
{
  *this = Scaled(width, height);

  return *this;
}

//##############################################################################
Image Image::Scaled(unsigned width, unsigned height) const
{
  bool const recurse = width < GetWidth() / 2 || height < GetHeight() / 2;

  if (recurse)
  {
    unsigned nextWidth = std::max(width, GetWidth() / 2);
    unsigned nextHeight = std::max(height, GetHeight() / 2);

    return Scaled(nextWidth, nextHeight).Scaled(width, height);
  }

  Image image(width, height);

  for (unsigned i = 0; i < height; ++i)
  {
    float yOrig = float(i) * (float(GetHeight() - 1)) / float(height - 1);

    for (unsigned j = 0; j < width; ++j)
    {
      float xOrig = float(j) * (float(GetWidth() - 1)) / float(width - 1);

      image.GetValue(j, i) = GetInterpolatedValue(xOrig, yOrig);
    }
  }

  return image;
}

//##############################################################################
bool Image::Load(std::string const & filename)
{
  int width;
  int height;
  int composition;

  stbi_uc * data =
    stbi_load(filename.c_str(), &width, &height, &composition, STBI_rgb);

  bool success = data;

  if (success)
  {
    switch (composition)
    {
    case STBI_grey:
      Resize(width, height);
      for (int i = 0; i < width * height; ++i)
      {
        Color color(
          data[i],
          data[i],
          data[i],
          255);

        SetValue(i, color);
      }
      break;
    case STBI_grey_alpha:
      Resize(width, height);
      for (int i = 0; i < width * height; ++i)
      {
        Color color(
          data[i * 2],
          data[i * 2],
          data[i * 2],
          data[i * 2 + 1]);

        SetValue(i, color);
      }
      break;
    case STBI_rgb:
      Resize(width, height);
      for (int i = 0; i < width * height; ++i)
      {
        Color color(
          data[i * 3 + 0],
          data[i * 3 + 1],
          data[i * 3 + 2],
          255);

        SetValue(i, color);
      }
      break;
    case STBI_rgb_alpha:
    {
      Color const * buffer = reinterpret_cast<Color const *>(data);
      *this = Image(width, height, buffer);
    }
      break;
    default:
      success = false;
      break;
    }
  }

  stbi_image_free(data);
  return success;
}

//##############################################################################
AsciiImageData::AsciiImageData(void) :
  character(' '),
  foregroundColorIndex(0),
  backgroundColorIndex(0)
{}

//##############################################################################
AsciiImageData::AsciiImageData(char character) :
  character(character),
  foregroundColorIndex(0),
  backgroundColorIndex(0)
{}

//##############################################################################
AsciiImageData::AsciiImageData(byte foregroundColorIndex,
  byte backgroundColorIndex) :
  character(' '),
  foregroundColorIndex(foregroundColorIndex),
  backgroundColorIndex(backgroundColorIndex)
{}

//##############################################################################
AsciiImageData::AsciiImageData(char character, byte foregroundColorIndex,
  byte backgroundColorIndex) :
  character(character),
  foregroundColorIndex(foregroundColorIndex),
  backgroundColorIndex(backgroundColorIndex)
{}

//##############################################################################
void SetColorPalette(Color const * colors, unsigned colorCount, unsigned offset)
{
  ASSERT(offset < NumberOfColors);
  ASSERT(offset + colorCount <= NumberOfColors);

  HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

  CONSOLE_SCREEN_BUFFER_INFOEX screenBufferInfo;
  screenBufferInfo.cbSize = sizeof(screenBufferInfo);
  GetConsoleScreenBufferInfoEx(consoleHandle, &screenBufferInfo);

  for (unsigned i = 0; i < colorCount; ++i)
    screenBufferInfo.ColorTable[offset + i] = GetApplyColorFromColor(colors[i]);

  SetConsoleScreenBufferInfoEx(consoleHandle, &screenBufferInfo);
}

//##############################################################################
void GetColorPalette(Color * colors, unsigned colorCount, unsigned offset)
{
  ASSERT(offset < NumberOfColors);
  ASSERT(offset + colorCount <= NumberOfColors);

  HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

  CONSOLE_SCREEN_BUFFER_INFOEX screenBufferInfo;
  screenBufferInfo.cbSize = sizeof(screenBufferInfo);
  GetConsoleScreenBufferInfoEx(consoleHandle, &screenBufferInfo);

  for (unsigned i = 0; i < colorCount; ++i)
    colors[i] = GetColorFromApplyColor(screenBufferInfo.ColorTable[offset + i]);
}
