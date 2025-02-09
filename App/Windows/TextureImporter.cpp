#include "TextureImporter.h"
#include "../App.h"
#include "../Windows/ProgressWindow.h"
#include "../Windows/REDialogs.h"

#include <Tera/FStream.h>
#include <Tera/UTexture.h>

#include <Tera/Utils/ALog.h>
#include <Tera/Utils/TextureUtils.h>

#include <wx/statline.h>
#include <wx/filename.h>

enum ControlElementId {
  Format = wxID_HIGHEST + 1,
  Normal,
  SRGB,
  MipGen,
  MipFilter,
  AddressX,
  AddressY
};

inline int PixelFormatToWx(EPixelFormat fmt)
{
  switch (fmt)
  {
  case PF_DXT1:
    return 0;
  case PF_DXT3:
    return 1;
  default:
  case PF_DXT5:
    return 2;
  case PF_A8R8G8B8:
    return 3;
  case PF_G8:
    return 4;
  }
}

inline EPixelFormat WxToPixelFormat(int fmt)
{
  switch (fmt)
  {
  case 0:
    return PF_DXT1;
  case 1:
    return PF_DXT3;
  case 3:
    return PF_A8R8G8B8;
  case 4:
    return PF_G8;
  default:
  case 2:
    return PF_DXT5;
  }
}

inline int TextureAddressToWx(TextureAddress address)
{
  switch (address)
  {
  case TA_Wrap:
    return 0;
  case TA_Clamp:
    return 1;
  case TA_Mirror:
    return 2;
  case TA_MAX:
  default:
    return -1;
  }
}

inline TextureAddress WxToTextureAddress(int address)
{
  switch (address)
  {
  case 0:
    return TA_Wrap;
  case 1:
    return TA_Clamp;
  case 2:
    return TA_Mirror;
  case TA_MAX:
  default:
    return TA_MAX;
  }
}

wxString TextureImporterOptions::LoadImageDialog(wxWindow* parent)
{
  return IODialog::OpenTextureDialog(parent);
}


wxString TextureImporterOptions::SaveImageDialog(wxWindow* parent, const wxString& defaultFileName)
{
  return IODialog::SaveTextureDialog(parent, defaultFileName);
}

EPixelFormat TextureImporterOptions::GetDDSPixelFormat(const wxString& ddsPath)
{
  FReadStream s(ddsPath.ToStdWstring());
  if (!s.IsGood())
  {
    return PF_Unknown;
  }
  DDS::DDSHeader header;
  s << header;
  return header.GetPixelFormat();
}

TextureImporterOptions::TextureImporterOptions(wxWindow* parent, EPixelFormat fmt, bool bNormal, bool bSRGB, TextureAddress addressX, TextureAddress addressY)
  : WXDialog(parent, wxID_ANY, wxT("导入选项"), wxDefaultPosition, wxSize(552, 595))
{
  SetSize(FromDIP(GetSize()));
  SetSizeHints(wxDefaultSize, wxDefaultSize);

  wxBoxSizer* bSizer1;
  bSizer1 = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer* bSizer11;
  bSizer11 = new wxBoxSizer(wxVERTICAL);

  wxStaticText* m_staticText1;
  m_staticText1 = new wxStaticText(this, wxID_ANY, wxT("格式化"), wxDefaultPosition, wxDefaultSize, 0);
  m_staticText1->Wrap(-1);
  m_staticText1->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString));

  bSizer11->Add(m_staticText1, 0, wxALL, FromDIP(5));

  wxPanel* m_panel1;
  m_panel1 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME | wxTAB_TRAVERSAL);
  wxBoxSizer* bSizer2;
  bSizer2 = new wxBoxSizer(wxHORIZONTAL);

  wxStaticText* m_staticText2;
  m_staticText2 = new wxStaticText(m_panel1, wxID_ANY, wxT("选择要将图像转换为的像素格式。根据经验，当图像没有透明度时，请使用“DXT1”，否则您可以选择“DXT5”."), wxDefaultPosition, wxDefaultSize, 0);
  m_staticText2->Wrap(FromDIP(400));
  bSizer2->Add(m_staticText2, 0, wxALL, FromDIP(5));

  wxArrayString PixelFormatChoices;
  PixelFormatChoices.Add("DXT1");
  PixelFormatChoices.Add("DXT3");
  PixelFormatChoices.Add("DXT5");
  PixelFormatChoices.Add("A8R8G8B8");
  PixelFormatChoices.Add("G8");
  PixelFormat = new wxChoice(m_panel1, ControlElementId::Format, wxDefaultPosition, wxDefaultSize, PixelFormatChoices, 0);
  PixelFormat->SetSelection(PixelFormatToWx(fmt));
  bSizer2->Add(PixelFormat, 1, wxALIGN_CENTER_VERTICAL | wxALL, FromDIP(5));


  m_panel1->SetSizer(bSizer2);
  m_panel1->Layout();
  bSizer2->Fit(m_panel1);
  bSizer11->Add(m_panel1, 0, wxEXPAND | wxALL, FromDIP(5));

  wxStaticText* m_staticText3;
  m_staticText3 = new wxStaticText(this, wxID_ANY, wxT("颜色"), wxDefaultPosition, wxDefaultSize, 0);
  m_staticText3->Wrap(-1);
  m_staticText3->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString));

  bSizer11->Add(m_staticText3, 0, wxALL, FromDIP(5));

  wxPanel* m_panel2;
  m_panel2 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME | wxTAB_TRAVERSAL);
  wxBoxSizer* bSizer3;
  bSizer3 = new wxBoxSizer(wxVERTICAL);

  Normal = new wxCheckBox(m_panel2, ControlElementId::Normal, wxT("法线贴图"), wxDefaultPosition, wxDefaultSize, 0);
  bSizer3->Add(Normal, 0, wxALL, FromDIP(5));
  Normal->SetValue(bNormal);
  Normal->Enable(!bSRGB);

  wxStaticText* m_staticText7;
  m_staticText7 = new wxStaticText(m_panel2, wxID_ANY, wxT("法线贴图的压缩方式不同。如果要导入法线贴图，请选中此选项."), wxDefaultPosition, wxDefaultSize, 0);
  m_staticText7->Wrap(FromDIP(515));
  bSizer3->Add(m_staticText7, 0, wxALL, FromDIP(5));

  wxStaticLine* m_staticline1;
  m_staticline1 = new wxStaticLine(m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
  bSizer3->Add(m_staticline1, 0, wxEXPAND | wxALL, FromDIP(5));

  SRGB = new wxCheckBox(m_panel2, ControlElementId::SRGB, wxT("sRGB"), wxDefaultPosition, wxDefaultSize, 0);
  bSizer3->Add(SRGB, 0, wxALL, FromDIP(5));
  SRGB->SetValue(bSRGB);
  SRGB->Enable(!bNormal);

  wxStaticText* m_staticText8;
  m_staticText8 = new wxStaticText(m_panel2, wxID_ANY, wxT("如果您的图像使用sRGB颜色空间，请选中此项。此选项不适用于法线贴图."), wxDefaultPosition, wxDefaultSize, 0);
  m_staticText8->Wrap(-1);
  bSizer3->Add(m_staticText8, 0, wxALL, FromDIP(5));


  m_panel2->SetSizer(bSizer3);
  m_panel2->Layout();
  bSizer3->Fit(m_panel2);
  bSizer11->Add(m_panel2, 0, wxEXPAND | wxALL, FromDIP(5));

  wxStaticText* m_staticText14;
  m_staticText14 = new wxStaticText(this, wxID_ANY, wxT("Mipmaps"), wxDefaultPosition, wxDefaultSize, 0);
  m_staticText14->Wrap(-1);
  m_staticText14->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString));

  bSizer11->Add(m_staticText14, 0, wxALL, FromDIP(5));

  wxPanel* m_panel9;
  m_panel9 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME | wxTAB_TRAVERSAL);
  wxBoxSizer* bSizer14;
  bSizer14 = new wxBoxSizer(wxVERTICAL);

  GenMips = new wxCheckBox(m_panel9, ControlElementId::MipGen, wxT("生成mipmaps"), wxDefaultPosition, wxDefaultSize, 0);
  GenMips->SetValue(false);
  GenMips->Enable(HasAVX2());
  bSizer14->Add(GenMips, 0, wxALL, FromDIP(5));

  wxStaticText* m_staticText20;
  m_staticText20 = new wxStaticText(m_panel9, wxID_ANY, wxT("在导入过程中生成mipmaps可节省游戏过程中的GPU时间."), wxDefaultPosition, wxDefaultSize, 0);
  m_staticText20->Wrap(-1);
  bSizer14->Add(m_staticText20, 0, wxALL, FromDIP(5));

  wxBoxSizer* bSizer20;
  bSizer20 = new wxBoxSizer(wxHORIZONTAL);

  wxStaticText* m_staticText15;
  m_staticText15 = new wxStaticText(m_panel9, wxID_ANY, wxT("ipmap在远处的对象上渲染，以降低GPU成本并在纹理加载过程中进行渲染."), wxDefaultPosition, wxDefaultSize, 0);
  m_staticText15->Wrap(FromDIP(350));
  bSizer20->Add(m_staticText15, 0, wxALL, FromDIP(5));

  wxStaticText* m_staticText16;
  m_staticText16 = new wxStaticText(m_panel9, wxID_ANY, wxT("方法:"), wxDefaultPosition, wxDefaultSize, 0);
  m_staticText16->Wrap(-1);
  m_staticText16->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString));

  bSizer20->Add(m_staticText16, 0, wxALIGN_CENTER_VERTICAL | wxALL, FromDIP(5));

  wxArrayString MipGenMethodChoices;
  MipGenMethodChoices.Add("Box");
  MipGenMethodChoices.Add("Triangle");
  MipGenMethodChoices.Add("Kaiser");
  MipFilter = new wxChoice(m_panel9, ControlElementId::MipFilter, wxDefaultPosition, wxDefaultSize, MipGenMethodChoices, 0);
  MipFilter->SetSelection(2);
  MipFilter->Enable(GenMips->GetValue());
  bSizer20->Add(MipFilter, 1, wxALIGN_CENTER_VERTICAL | wxALL, FromDIP(5));


  bSizer14->Add(bSizer20, 1, wxEXPAND, 0);


  m_panel9->SetSizer(bSizer14);
  m_panel9->Layout();
  bSizer14->Fit(m_panel9);
  bSizer11->Add(m_panel9, 1, wxEXPAND | wxALL, FromDIP(5));


  wxStaticText* m_staticText9;
  m_staticText9 = new wxStaticText(this, wxID_ANY, wxT("地址模式"), wxDefaultPosition, wxDefaultSize, 0);
  m_staticText9->Wrap(-1);
  m_staticText9->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString));

  bSizer11->Add(m_staticText9, 0, wxALL, FromDIP(5));

  wxPanel* m_panel3;
  m_panel3 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME | wxTAB_TRAVERSAL);
  wxBoxSizer* bSizer4;
  bSizer4 = new wxBoxSizer(wxHORIZONTAL);

  wxStaticText* m_staticText10;
  m_staticText10 = new wxStaticText(m_panel3, wxID_ANY, wxT("地址模式定义了游戏的纹理采样器行为。一般来说，除非你知道自己在做什么，否则你不想改变这一点。."), wxDefaultPosition, wxDefaultSize, 0);
  m_staticText10->Wrap(FromDIP(330));
  bSizer4->Add(m_staticText10, 0, wxALIGN_CENTER_VERTICAL | wxALL, FromDIP(5));

  wxPanel* m_panel4;
  m_panel4 = new wxPanel(m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
  wxBoxSizer* bSizer5;
  bSizer5 = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer* bSizer6;
  bSizer6 = new wxBoxSizer(wxHORIZONTAL);

  wxStaticText* m_staticText11;
  m_staticText11 = new wxStaticText(m_panel4, wxID_ANY, wxT("坐标 X:"), wxDefaultPosition, wxDefaultSize, 0);
  m_staticText11->Wrap(-1);
  bSizer6->Add(m_staticText11, 0, wxALIGN_CENTER_VERTICAL | wxALL, FromDIP(5));

  wxArrayString AddressXChoices;
  AddressXChoices.Add("Wrap");
  AddressXChoices.Add("Clamp");
  AddressXChoices.Add("Mirror");
  AddressX = new wxChoice(m_panel4, ControlElementId::AddressX, wxDefaultPosition, wxDefaultSize, AddressXChoices, 0);
  AddressX->SetSelection(TextureAddressToWx(addressX));
  AddressX->Enable(false);
  bSizer6->Add(AddressX, 1, wxALIGN_CENTER_VERTICAL | wxALL, FromDIP(5));


  bSizer5->Add(bSizer6, 0, wxEXPAND, 0);

  wxBoxSizer* bSizer61;
  bSizer61 = new wxBoxSizer(wxHORIZONTAL);

  wxStaticText* m_staticText111;
  m_staticText111 = new wxStaticText(m_panel4, wxID_ANY, wxT("坐标 Y:"), wxDefaultPosition, wxDefaultSize, 0);
  m_staticText111->Wrap(-1);
  bSizer61->Add(m_staticText111, 0, wxALIGN_CENTER_VERTICAL | wxALL, FromDIP(5));

  wxArrayString AddressYChoices;
  AddressYChoices.Add("Wrap");
  AddressYChoices.Add("Clamp");
  AddressYChoices.Add("Mirror");
  AddressY = new wxChoice(m_panel4, ControlElementId::AddressY, wxDefaultPosition, wxDefaultSize, AddressYChoices, 0);
  AddressY->SetSelection(TextureAddressToWx(addressY));
  AddressY->Enable(false);
  bSizer61->Add(AddressY, 1, wxALIGN_CENTER_VERTICAL | wxALL, FromDIP(5));


  bSizer5->Add(bSizer61, 0, wxEXPAND, 0);


  m_panel4->SetSizer(bSizer5);
  m_panel4->Layout();
  bSizer5->Fit(m_panel4);
  bSizer4->Add(m_panel4, 1, wxALL, 0);


  m_panel3->SetSizer(bSizer4);
  m_panel3->Layout();
  bSizer4->Fit(m_panel3);
  bSizer11->Add(m_panel3, 0, wxEXPAND | wxALL, FromDIP(5));


  bSizer1->Add(bSizer11, 1, wxEXPAND, 0);

  wxPanel* m_panel5;
  m_panel5 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
  wxBoxSizer* bSizer10;
  bSizer10 = new wxBoxSizer(wxHORIZONTAL);

  wxPanel* m_panel6;
  m_panel6 = new wxPanel(m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
  bSizer10->Add(m_panel6, 1, wxEXPAND | wxALL, FromDIP(5));

  ImportButton = new wxButton(m_panel5, wxID_OK, wxT("导入"), wxDefaultPosition, wxDefaultSize, 0);
  bSizer10->Add(ImportButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, FromDIP(5));
  ImportButton->SetFocus();

  CancelButton = new wxButton(m_panel5, wxID_CANCEL, wxT("取消"), wxDefaultPosition, wxDefaultSize, 0);
  bSizer10->Add(CancelButton, 0, wxALIGN_CENTER_VERTICAL | wxALL, FromDIP(5));

  m_panel5->SetSizer(bSizer10);
  m_panel5->Layout();
  bSizer10->Fit(m_panel5);
  bSizer1->Add(m_panel5, 1, wxEXPAND | wxALL, FromDIP(5));


  SetSizer(bSizer1);
  Layout();

  Centre(wxBOTH);
}

EPixelFormat TextureImporterOptions::GetPixelFormat() const
{
  return WxToPixelFormat(PixelFormat->GetSelection());
}

TextureAddress TextureImporterOptions::GetAddressX() const
{
  return WxToTextureAddress(AddressX->GetSelection());
}

TextureAddress TextureImporterOptions::GetAddressY() const
{
  return WxToTextureAddress(AddressY->GetSelection());
}

MipFilterType TextureImporterOptions::GetMipFilter() const
{
  return (MipFilterType)MipFilter->GetSelection();
}

bool TextureImporterOptions::IsNormal() const
{
  return Normal->GetValue();
}

bool TextureImporterOptions::IsSRGB() const
{
  return SRGB->GetValue();
}

bool TextureImporterOptions::GetGenerateMips() const
{
  return GenMips->GetValue();
}

void TextureImporterOptions::OnNormalClick(wxCommandEvent&)
{
  SRGB->Enable(!Normal->GetValue());
}

void TextureImporterOptions::OnGenMipsClicked(wxCommandEvent&)
{
  MipFilter->Enable(GenMips->GetValue());
}

void TextureImporterOptions::OnSRGBClick(wxCommandEvent&)
{
  Normal->Enable(!SRGB->GetValue());
}

wxBEGIN_EVENT_TABLE(TextureImporterOptions, WXDialog)
EVT_CHECKBOX(ControlElementId::Normal, TextureImporterOptions::OnNormalClick)
EVT_CHECKBOX(ControlElementId::MipGen, TextureImporterOptions::OnGenMipsClicked)
EVT_CHECKBOX(ControlElementId::SRGB, TextureImporterOptions::OnSRGBClick)
wxEND_EVENT_TABLE()

TextureImporter::TextureImporter(wxWindow* parent, class UTexture2D* texture, bool createMode)
  : Parent(parent)
  , Texture(texture)
  , CreateMode(createMode)
{}

bool TextureImporter::Run()
{
  wxString path = CustomPath;
  if (path.empty())
  {
    path = TextureImporterOptions::LoadImageDialog(Parent);
  }
  if (path.empty())
  {
    LogI("Import canceled by user.");
    return false;
  }

  bool isNormal = Texture->CompressionSettings == TC_Normalmap ||
    Texture->CompressionSettings == TC_NormalmapAlpha ||
    Texture->CompressionSettings == TC_NormalmapUncompressed ||
    Texture->CompressionSettings == TC_NormalmapBC5;

  wxString extension;
  wxFileName::SplitPath(path, nullptr, nullptr, nullptr, &extension);
  extension.MakeLower();

  TextureProcessor::TCFormat outputFormat = TextureProcessor::TCFormat::None;
  TextureProcessor::TCFormat inFormat = TextureProcessor::TCFormat::None;

  TextureImporterOptions importer(Parent, Texture->Format, isNormal, Texture->SRGB, Texture->AddressX, Texture->AddressY);
  if (extension != wxT("dds"))
  {
    if (extension == wxT("png"))
    {
      inFormat = TextureProcessor::TCFormat::PNG;
    }
    else if (extension == wxT("tga"))
    {
      inFormat = TextureProcessor::TCFormat::TGA;
    }

    if (importer.ShowModal() != wxID_OK)
    {
      LogI("Import canceled by user.");
      return false;
    }

    switch (importer.GetPixelFormat())
    {
    case PF_DXT1:
      outputFormat = TextureProcessor::TCFormat::DXT1;
      break;
    case PF_DXT3:
      outputFormat = TextureProcessor::TCFormat::DXT3;
      break;
    case PF_DXT5:
      outputFormat = TextureProcessor::TCFormat::DXT5;
      break;
    case PF_A8R8G8B8:
      outputFormat = TextureProcessor::TCFormat::ARGB8;
      break;
    case PF_G8:
      outputFormat = TextureProcessor::TCFormat::G8;
      break;
    default:
    {
      std::string errmsg = std::string("Format ") + PixelFormatToString(Texture->Format).String() + " is not supported!";
      LogE(errmsg.c_str());
      REDialog::Error(errmsg);
      return false;
    }
    }
  }
  else
  {
    inFormat = TextureProcessor::TCFormat::DDS;
    if (Texture->Format == PF_Unknown)
    {
      FReadStream s(path.ToStdWstring());
      DDS::DDSHeader header;
      s << header;
      switch (header.GetPixelFormat())
      {
      case PF_DXT1:
      case PF_DXT3:
      case PF_DXT5:
        outputFormat = TextureProcessor::TCFormat::DXT;
        break;
      case PF_A8R8G8B8:
        outputFormat = TextureProcessor::TCFormat::ARGB8;
        break;
      case PF_G8:
        outputFormat = TextureProcessor::TCFormat::G8;
        break;
      default:
      {
        std::string errmsg = std::string("Format ") + PixelFormatToString(header.GetPixelFormat()).String() + " is not supported!\nUse one of the following formats: DXT1-5(BC1-3), ARGB8, G8(R8).";
        LogE(errmsg.c_str());
        REDialog::Error(errmsg);
        return false;
      }
      }
    }
    else
    {
      switch (Texture->Format)
      {
      case PF_DXT1:
      case PF_DXT3:
      case PF_DXT5:
        outputFormat = TextureProcessor::TCFormat::DXT;
        break;
      case PF_A8R8G8B8:
        outputFormat = TextureProcessor::TCFormat::ARGB8;
        break;
      case PF_G8:
        outputFormat = TextureProcessor::TCFormat::G8;
        break;
      default:
      {
        std::string errmsg = std::string("Format ") + PixelFormatToString(Texture->Format).String() + " is not supported!";
        LogE(errmsg.c_str());
        REDialog::Error(errmsg);
        return false;
      }
      }
    }
  }

  TextureProcessor processor(inFormat, outputFormat);
  processor.SetInputPath(W2A(path.ToStdWstring()));
  processor.SetSrgb(importer.IsSRGB());
  processor.SetNormal(importer.IsNormal());
  processor.SetAddressX(importer.GetAddressX());
  processor.SetAddressY(importer.GetAddressY());
  processor.SetGenerateMips(importer.GetGenerateMips());
  processor.SetMipFilter(importer.GetMipFilter());

  ProgressWindow progress(Parent, "Please wait...");
  progress.SetCurrentProgress(-1);
  progress.SetCanCancel(false);
  progress.SetActionText("Processing the image");
  progress.Layout();

  std::thread([&processor, &progress] {
    bool result = false;
    try
    {
      result = processor.Process();
    }
    catch (const std::exception& e)
    {
      result = false;
      LogE(e.what());
    }
    catch (const char* msg)
    {
      result = false;
      std::string err = std::string("Failed to process the image: ") + msg;
      LogE(err.c_str());
    }
    SendEvent(&progress, UPDATE_PROGRESS_FINISH, result);
  }).detach();

  if (!progress.ShowModal())
  {
    REDialog::Error(processor.GetError());
    return false;
  }

  EPixelFormat resultFormat = importer.GetPixelFormat();
  if (extension == wxT("dds"))
  {
    switch (processor.GetOutputFormat())
    {
    case TextureProcessor::TCFormat::DXT1:
      resultFormat = PF_DXT1;
      break;
    case TextureProcessor::TCFormat::DXT3:
      resultFormat = PF_DXT3;
      break;
    case TextureProcessor::TCFormat::DXT5:
      resultFormat = PF_DXT5;
      break;
    }
  }

  TextureTravaller travaller;
  travaller.SetFormat(resultFormat);
  travaller.SetSRGB(importer.IsSRGB());
  travaller.SetAddressX(importer.GetAddressX());
  travaller.SetAddressY(importer.GetAddressY());
  travaller.SetIsNew(CreateMode);

  if (importer.IsNormal())
  {
    travaller.SetLODGroup(TEXTUREGROUP_WorldNormalMap);
  }
  travaller.SetCompression(importer.IsNormal() ? processor.GetAlpha() ? TC_NormalmapAlpha : TC_Normalmap : TC_Default);

  const auto& mips = processor.GetOutputMips();
  for (const auto mip : mips)
  {
    travaller.AddMipMap(mip.SizeX, mip.SizeY, mip.Size, mip.Data);
  }

  if (!travaller.Visit(Texture))
  {
    REDialog::Error(travaller.GetError());
    return false;
  }

  return true;
}