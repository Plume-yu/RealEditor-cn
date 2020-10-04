#include "TextureProcessor.h"

#include <Tera/ALog.h>
#include <Tera/FStream.h>

#include <nvtt/nvtt.h>
#include <FreeImage.h>

#include <ppl.h>
#include <algorithm>

#include "DDS.h"

// freeimage raii container
struct FreeImageHolder {
  FreeImageHolder(bool initContext)
    : ctx(initContext)
  {
    if (ctx)
    {
      FreeImage_Initialise();
    }
  }

  FIBITMAP* bmp = nullptr;
  FIMEMORY* mem = nullptr;
  bool ctx = false;

  ~FreeImageHolder()
  {
    FreeImage_Unload(bmp);
    FreeImage_CloseMemory(mem);
    if (ctx)
    {
      FreeImage_DeInitialise();
    }
  }
};

// nvtt handler
struct TPOutputHandler : public nvtt::OutputHandler {
  static const int32 MaxMipCount = 16;

  TPOutputHandler()
  {
    memset(Mips, 0, sizeof(Mips));
  }

  struct Mip {
    int32 SizeX = 0;
    int32 SizeY = 0;

    int32 Size = 0;
    int32 LastOffset = 0;
    void* Data = nullptr;
  };

  ~TPOutputHandler() override
  {
    for (int32 idx = 0; idx < MaxMipCount; ++idx)
    {
      free(Mips[idx].Data);
    }
  }

  void beginImage(int size, int width, int height, int depth, int face, int miplevel) override
  {
    if (miplevel >= MaxMipCount)
    {
      Ok = false;
      return;
    }
    Mip& mip = Mips[MipsCount];
    mip.Size = size;
    mip.SizeX = width;
    mip.SizeY = height;
    mip.Data = malloc(size);
    DBreakIf(depth != 1);
    MipsCount++;
  }

  bool writeData(const void* data, int size) override
  {
    if (!Ok || !MipsCount)
    {
      return Ok;
    }
    Mip& mip = Mips[MipsCount - 1];
    if (mip.LastOffset + size > mip.Size)
    {
      if (mip.Size)
      {
        if (void* tmp = realloc(mip.Data, mip.LastOffset + size))
        {
          mip.Size = mip.LastOffset + size;
          mip.Data = tmp;
        }
        else
        {
          Ok = false;
        }
      }
      else
      {
        mip.Data = malloc(size);
        mip.Size = size;
      }
    }
    if (Ok)
    {
      memcpy((char*)mip.Data + mip.LastOffset, data, size);
      mip.LastOffset += size;
    }
    return Ok;
  }

  virtual void endImage() override
  {}

  Mip Mips[MaxMipCount];
  int32 MipsCount = 0;
  bool Ok = true;
};

nvtt::MipmapFilter MipFilterTypeToNvtt(MipFilterType type)
{
  switch (type)
  {
  case MipFilterType::Box:
    return nvtt::MipmapFilter_Box;
  case MipFilterType::Triangle:
    return nvtt::MipmapFilter_Triangle;
  case MipFilterType::Kaiser:
  default:
    return nvtt::MipmapFilter_Kaiser;
  }
}

nvtt::WrapMode AddressModeToNvtt(TextureAddress mode)
{
  switch (mode)
  {
  case TA_Clamp:
    return nvtt::WrapMode_Clamp;
  case TA_Mirror:
    return nvtt::WrapMode_Mirror;
  case TA_Wrap:
  default:
    return nvtt::WrapMode_Repeat;
    break;
  }
}

bool TextureProcessor::Process()
{
  if (InputPath.empty())
  {
    if (!InputData || !InputDataSize)
    {
      Error = "Texture Processor: no input data";
      return false;
    }
    
    if (OutputPath.empty())
    {
      if (!OutputData || !OutputDataSize)
      {
        Error = "Texture Processor: no output specified";
        return false;
      }
      return BytesToBytes();
    }
    return BytesToFile();
  }
  if (OutputPath.empty())
  {
    return FileToBytes();
  }
  Error = "Texture Processor: no output specified";
  return false;
}

bool TextureProcessor::BytesToFile()
{
  nvtt::Surface surface;
  bool hasAlpha = true;
  if (OutputFormat == TCFormat::DDS)
  {
    return BytesToDDS();
  }
  if (InputFormat == TCFormat::DXT1 || InputFormat == TCFormat::DXT3 || InputFormat == TCFormat::DXT5)
  {
    nvtt::Format fmt = nvtt::Format_Count;
    if (InputFormat == TCFormat::DXT1)
    {
      fmt = nvtt::Format_DXT1;
      hasAlpha = false;
    }
    else if (InputFormat == TCFormat::DXT3)
    {
      fmt = nvtt::Format_DXT3;
    }
    else if (InputFormat == TCFormat::DXT5)
    {
      fmt = nvtt::Format_DXT5;
    }
    LogI("Texture Processor: Setting surface image");
    if (!surface.setImage2D(fmt, nvtt::Decoder_D3D9, InputDataSizeX, InputDataSizeY, InputData))
    {
      Error = "Texture Processor: failed to create input surface (";
      Error += "DXT1:" + std::to_string(InputDataSizeX) + "x" + std::to_string(InputDataSizeY) + ")";
      return false;
    }
  }
  else if (InputFormat == TCFormat::RGBA8)
  {
    if (!surface.setImage(nvtt::InputFormat_BGRA_8UB, InputDataSizeX, InputDataSizeY, 1, InputData))
    {
      Error = "Texture Processor: failed to create input surface (";
      Error += "RGBA8:" + std::to_string(InputDataSizeX) + "x" + std::to_string(InputDataSizeY) + ")";
      return false;
    }
  }
  else
  {
    // TODO: export G8 as a luminance R8
    // TODO: export A8R8G8B8
    Error = "Texture Processor: unsupported input " + std::to_string((int)InputFormat);
    return false;
  }

  if (surface.isNull() || !surface.width() || !surface.height())
  {
    Error = "Texture Processor: NVTT failed to create the surface!";
    return false;
  }

  surface.flipY();
  nvtt::CompressionOptions compressionOptions;
  compressionOptions.setFormat(hasAlpha ? nvtt::Format_RGBA : nvtt::Format_RGB);
  compressionOptions.setPixelFormat(hasAlpha ? 32 : 24, 0xFF0000, 0xFF00, 0xFF, hasAlpha ? 0xFF000000 : 0);

  nvtt::OutputOptions outputOptions;
  outputOptions.setSrgbFlag(SRGB);

  TPOutputHandler ohandler;
  outputOptions.setOutputHandler(&ohandler);

  LogI("Texture Processor: Decompress DXT data");
  nvtt::Context ctx;
  if (!ctx.compress(surface, 0, 0, compressionOptions, outputOptions))
  {
    Error = "Texture Processor: Failed to decompress texture to RGBA!";
    return false;
  }

  if (!ohandler.MipsCount)
  {
    Error = "Texture Processor: Failed to decompress DXT texture to RGBA!";
    return false;
  }

  if (!ohandler.Mips[0].Size)
  {
    Error = "Texture Processor: NVTT failed to decompress the texture!";
    return false;
  }

  LogI("Texture Processor: Preparing to convert the buffer to FreeImage format");
  FreeImageHolder holder(true);
  holder.bmp = FreeImage_Allocate(ohandler.Mips[0].SizeX, ohandler.Mips[0].SizeY, hasAlpha ? 32 : 24);
  memcpy(FreeImage_GetBits(holder.bmp), ohandler.Mips[0].Data, ohandler.Mips[0].Size);
  holder.mem = FreeImage_OpenMemory();

  LogI("Texture Processor: Convert the buffer to FreeImage format");
  if (OutputFormat == TCFormat::TGA)
  {
    if (!FreeImage_SaveToMemory(FIF_TARGA, holder.bmp, holder.mem, 0))
    {
      Error = "Texture Processor: Failed to create a FreeImage(TARGA:" + std::to_string(hasAlpha ? 32 : 24) + ")";
      return false;
    }
  }
  else if (OutputFormat == TCFormat::PNG)
  {
    if (!FreeImage_SaveToMemory(FIF_PNG, holder.bmp, holder.mem, 0))
    {
      Error = "Texture Processor: Failed to create a FreeImage(PNG:" + std::to_string(hasAlpha ? 32 : 24) + ")";
      return false;
    }
  }
  else
  {
    Error = "Texture Processor: Unsupported IO combination BytesToFile(\"" + std::to_string((int)InputFormat) + "\"" + std::to_string((int)InputFormat) + ")";
    return false;
  }

  LogI("Texture Processor: Preparing to save the buffer");
  DWORD memBufferSize = 0;
  unsigned char* memBuffer = nullptr;
  if (!FreeImage_AcquireMemory(holder.mem, &memBuffer, &memBufferSize))
  {
    Error = "Texture Processor: Failed to acquire a memory buffer";
    return false;
  }

  if (!memBufferSize)
  {
    Error = "Texture Processor: FreeImageLib failed to acquire memory!";
    return false;
  }

  LogI("Saving data...");
  FWriteStream s(OutputPath);
  if (!s.IsGood())
  {
    Error = "Texture Processor: Failed to create a write stream to \"" + OutputPath + "\"";
    return false;
  }
  s.SerializeBytes(memBuffer, (FILE_OFFSET)memBufferSize);
  if (!s.IsGood())
  {
    Error = "Texture Processor: Failed to write data to the stream at: \"" + OutputPath + "\"";
    return false;
  }
  return true;
}

bool TextureProcessor::BytesToDDS()
{
  DDS::DDSHeader header;
  header.D3D9.dwWidth = InputDataSizeX;
  header.D3D9.dwHeight = InputDataSizeY;
  header.D3D9.dwPitchOrLinearSize = std::max(1, ((InputDataSizeX + 3) / 4)) * (InputFormat == TCFormat::DXT1 ? 8 : 16);
  switch (InputFormat)
  {
  case TextureProcessor::TCFormat::DXT1:
    header.D3D10.dxgiFormat = DDS::DXGI_FORMAT_BC1_TYPELESS;
    break;
  case TextureProcessor::TCFormat::DXT3:
    header.D3D10.dxgiFormat = DDS::DXGI_FORMAT_BC2_TYPELESS;
    break;
  case TextureProcessor::TCFormat::DXT5:
    header.D3D10.dxgiFormat = DDS::DXGI_FORMAT_BC3_TYPELESS;
    break;
  default:
    Error = "Texture Processor: pixel format is not supported!";
    return false;
  }

  FWriteStream s(OutputPath);
  s << header;
  s.SerializeBytes(InputData, InputDataSize);
  if (!s.IsGood())
  {
    Error = "Texture Processor: failed to save the file!";
    return false;
  }
  return true;
}

bool TextureProcessor::BytesToBytes()
{
  Error = "Texture Processor: B2B conversion unsupported!";
  return false;
}

bool TextureProcessor::FileToBytes()
{
  FreeImageHolder holder(true);
  FREE_IMAGE_FORMAT fmt;
  if (InputFormat == TCFormat::PNG)
  {
    fmt = FIF_PNG;
  }
  else if (InputFormat == TCFormat::TGA)
  {
    fmt = FIF_TARGA;
  }
  else if (InputFormat == TCFormat::DDS)
  {
    return DDSToBytes();
  }
  else
  {
    Error = "Texture Processor: Input format \"" + std::to_string((int)InputFormat) + "\" is not supported!";
    return false;
  }
  
  holder.bmp = FreeImage_LoadU(fmt, A2W(InputPath).c_str());
  if (FreeImage_GetBPP(holder.bmp) != 32)
  {
    holder.bmp = FreeImage_ConvertTo32Bits(holder.bmp);
  }

  Alpha = FreeImage_IsTransparent(holder.bmp);
  uint8* data = (uint8*)FreeImage_GetBits(holder.bmp);

  nvtt::Surface surface;
  surface.setWrapMode(AddressModeToNvtt(AddressX));
  nvtt::Format outFmt = nvtt::Format_Count;

  if (OutputFormat == TCFormat::DXT1)
  {
    outFmt = Alpha ? nvtt::Format_DXT1a : nvtt::Format_DXT1;
  }
  else if (OutputFormat == TCFormat::DXT3)
  {
    outFmt = nvtt::Format_DXT3;
  }
  else if (OutputFormat == TCFormat::DXT5)
  {
    outFmt = nvtt::Format_DXT5;
  }

  if (!surface.setImage(nvtt::InputFormat_BGRA_8UB, FreeImage_GetWidth(holder.bmp), FreeImage_GetHeight(holder.bmp), 1, data))
  {
    Error = "Texture Processor: failed to create input surface (";
    Error += std::to_string(FreeImage_GetWidth(holder.bmp)) + "x" + std::to_string(FreeImage_GetHeight(holder.bmp)) + ")";
    FreeImage_Unload(holder.bmp);
    return false;
  }

  if (Normal)
  {
    surface.setNormalMap(true);
  }
  surface.setAlphaMode(Alpha ? nvtt::AlphaMode_Transparency : nvtt::AlphaMode_None);
  surface.flipY();
  

  nvtt::CompressionOptions compressionOptions;
  compressionOptions.setFormat(outFmt);
  compressionOptions.setQuality(nvtt::Quality_Production);
  if (outFmt == nvtt::Format_DXT3)
  {
    compressionOptions.setQuantization(false, true, false);
  }
  else if (outFmt == nvtt::Format_DXT1a)
  {
    compressionOptions.setQuantization(false, true, true, 127);
  }

  TPOutputHandler ohandler;
  nvtt::OutputOptions outputOptions;
  outputOptions.setOutputHandler(&ohandler);
  nvtt::Context context;
  
  int32 idx = 0;
  int32 sizeX = FreeImage_GetWidth(holder.bmp);
  int32 sizeY = FreeImage_GetHeight(holder.bmp);

  const int32 minX = 4; // TODO: replace with PF block width & height
  const int32 minY = minX;
  OutputDataSize = 0;

  if (sizeX < minX || sizeY < minY)
  {
    Error = "Texture Processor: failed to compress the bitmap. The image is too small!";
    return false;
  }

  while (sizeX >= minX && sizeY >= minY && idx < TPOutputHandler::MaxMipCount)
  {
    if (!context.compress(surface, 0, idx, compressionOptions, outputOptions))
    {
      Error = "Texture Processor: failed to compress the bitmap.";
      return false;
    }

    OutputDataSize += ohandler.Mips[idx].Size;

    if (GenerateMips)
    {
      if (!surface.buildNextMipmap(MipFilterTypeToNvtt(MipFilter), minX))
      {
        break;
      }
      sizeX = surface.width();
      sizeY = surface.height();
      idx++;
    }
    else
    {
      break;
    }
  }

  if (!(OutputMipCount = ohandler.MipsCount))
  {
    Error = "Texture Processor: failed to compress the bitmap. No mipmaps were generated.";
    return false;
  }
  if (!OutputDataSize)
  {
    Error = "Texture Processor: NVTT encoding error!";
    return false;
  }

  OutputData = malloc(OutputDataSize);
  
  int32 offset = 0;
  for (int32 idx = 0; idx < OutputMipCount; ++idx)
  {
    TPOutputHandler::Mip& mip = ohandler.Mips[idx];
    memcpy((uint8*)OutputData + offset, mip.Data, mip.Size);
    OutputMips.push_back({mip.SizeX, mip.SizeY, mip.Size, (uint8*)OutputData + offset });
    offset += ohandler.Mips[idx].Size;
  }
  return true;
}

bool TextureProcessor::DDSToBytes()
{
  FReadStream s = FReadStream(InputPath);
  if (!s.IsGood())
  {
    Error = "Texture Processor: Failed to open the texture file!";
    return false;
  }
  
  DDS::DDSHeader header;
  try
  {
    s << header;
  }
  catch (const std::exception& e)
  {
    Error = e.what();
    return false;
  }
  EPixelFormat fmt = header.GetPixelFormat();
  if (fmt == PF_Unknown)
  {
    switch (header.D3D10.dxgiFormat)
    {
    case DDS::DXGI_FORMAT_BC7_TYPELESS:
    case DDS::DXGI_FORMAT_BC7_UNORM:
    case DDS::DXGI_FORMAT_BC7_UNORM_SRGB:
      // BC7 is a default format in NVTE. Show more specific error to make sure that user understands what's wrong.
      Error = "Texture Processor: The input file has BC7 format. Tera does not support it!\nPlease, use BC1(DXT1) or BC3(DXT5) format when saving your DDS file.";
      return false;
    }
    Error = "Texture Processor: The input file has unsupported pixel format.\nPlease, use BC1(DXT1) or BC3(DXT5) format when saving your DDS file.";
    return false;
  }
  else if (fmt == PF_DXT1)
  {
    OutputFormat = TCFormat::DXT1;
  }
  else if (fmt == PF_DXT3)
  {
    OutputFormat = TCFormat::DXT3;
  }
  else if (fmt == PF_DXT5)
  {
    OutputFormat = TCFormat::DXT5;
    Alpha = true;
  }
  FILE_OFFSET mipSize = header.CalculateMipmapSize();
  if (!mipSize)
  {
    Error = "Texture Processor: Failed to calculate texture size.";
    return false;
  }

  OutputData = malloc(mipSize);
  s.SerializeBytes(OutputData, mipSize);
  OutputDataSize = mipSize;
  OutputMipCount = 1;
  OutputMips.clear();
  OutputMips.push_back({header.GetWidth(), header.GetHeight(), OutputDataSize, OutputData});
  return true;
}
