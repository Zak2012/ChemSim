#include "File.hpp"

#include <string>
#include <fstream>
#include <streambuf>
#include <filesystem>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>
#include <iostream>

#include <tinyxml2/tinyxml2.h>

// #include <windows.h>

// std::wstring s2ws(const std::string& str)
// {
//     using convert_typeX = std::codecvt_utf8<wchar_t>;
//     std::wstring_convert<convert_typeX, wchar_t> converterX;

//     return converterX.from_bytes(str);
// }

// std::string ws2s(const std::wstring& wstr)
// {
//     using convert_typeX = std::codecvt_utf8<wchar_t>;
//     std::wstring_convert<convert_typeX, wchar_t> converterX;

//     return converterX.to_bytes(wstr);
// }

// std::string GetExeFullPath()
// {
//     char path[MAX_PATH];
//     GetModuleFileNameA(NULL, path, MAX_PATH);
//     return std::string(path);
// }

std::string fx_ReadFile(std::string Path)
{
    std::ifstream infile(Path);
    if ( infile.fail() )
    {
        throw std::runtime_error("File " + Path + " not found");
    }

    return std::string((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
}

void fx_WriteFile(std::string Path, std::string Str)
{
    std::ofstream Out(Path, std::ios::trunc);
    if ( Out.fail() )
    {
        throw std::runtime_error("File " + Path + " not found");
    }
    Out << Str;
    Out.close();
}

void fx_WriteBinaryFile(std::string Path, std::vector<uint8_t> Buffer)
{
    std::ofstream Out(Path, std::ios::binary);
    if ( Out.fail() )
    {
        throw std::runtime_error("File " + Path + " not found");
    }
    Out.write((const char *)Buffer.data(), (int)Buffer.size());
    Out.close();
}

void fx_ApplyEndian(std::vector<uint8_t> &Buffer)
{
    unsigned int endian_test = 1;
    int big_endian = 0;
    if (((char *)(&endian_test))[0] != big_endian)
    {
        std::reverse(Buffer.begin(), Buffer.end());
    }
}

std::vector<uint8_t> fx_ReadBinaryFile(std::string Path)
{
    unsigned int filesize = std::filesystem::file_size(Path);
    unsigned char *Inpout = new unsigned char[filesize];

    std::ifstream In(Path, std::ios::binary);
    if ( In.fail() )
    {
        throw std::runtime_error("File " + Path + " not found");

    }
    In.read((char *)Inpout, filesize);
    In.close();
    std::vector<uint8_t> Buffer(Inpout, Inpout + filesize);
    delete Inpout;
    return Buffer;
}

int fx_StateStorage::Char2Int(char A)
{
    if (A == '0')
    {
        return 0;
    }
    else if (A == '1')
    {
        return 1;
    }
    else if (A == '2')
    {
        return 2;
    }
    else if (A == '3')
    {
        return 3;
    }
    else if (A == '4')
    {
        return 4;
    }
    else if (A == '5')
    {
        return 5;
    }
    else if (A == '6')
    {
        return 6;
    }
    else if (A == '7')
    {
        return 7;
    }
    else if (A == '8')
    {
        return 8;
    }
    else if (A == '9')
    {
        return 9;
    }
    else if (A == 'A' || A == 'a')
    {
        return 10;
    }
    else if (A == 'B' || A == 'b')
    {
        return 11;
    }
    else if (A == 'C' || A == 'c')
    {
        return 12;
    }
    else if (A == 'D' || A == 'd')
    {
        return 13;
    }
    else if (A == 'E' || A == 'e')
    {
        return 14;
    }
    else if (A == 'F' || A == 'f')
    {
        return 15;
    }
    else
    {
        throw std::runtime_error("Invalid Hex");
    }
}

char fx_StateStorage::Int2Char(int A)
{
    static const char HexRef[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    if (A > 15)
    {
        throw std::out_of_range("Int2Char");
    }
    return HexRef[A];
}

fx_StateStorage::fx_StateStorage(std::string Path, int FlushInterval)
{
    m_Path = Path;
    m_FlushInterval = FlushInterval;
    if (!std::filesystem::exists(Path))
    {
        std::ofstream MkFile(m_Path);
        MkFile.close();
    }
    std::string FileContent = fx_ReadFile(m_Path);
    m_Data.clear();
    tinyxml2::XMLDocument doc;
    doc.Parse(FileContent.c_str());
    for(tinyxml2::XMLElement* e = doc.FirstChildElement("var"); e != NULL; e = e->NextSiblingElement("var"))
    {
        VariableInfo Info;
        Info.Value = UnEscapeString(std::string(e->Attribute("value")));
        Info.Type = std::string(e->Attribute("type"));
        std::string VarName = std::string(e->Attribute("name"));
        m_Data.insert({VarName,Info});
        m_NameIndex.push_back(VarName);
    }
}

fx_StateStorage::~fx_StateStorage()
{
    Flush();
}

std::string fx_StateStorage::ReplaceAll(std::string str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

std::string fx_StateStorage::EscapeString(const std::string &Str)
{
    std::string Result = Str;
    // Result = ReplaceAll(Result, "\\", "\\\\");
    Result = ReplaceAll(Result, "\a", "\\a");
    Result = ReplaceAll(Result, "\b", "\\b");
    Result = ReplaceAll(Result, "\f", "\\f");
    Result = ReplaceAll(Result, "\n", "\\n");
    Result = ReplaceAll(Result, "\r", "\\r");
    Result = ReplaceAll(Result, "\t", "\\t");
    Result = ReplaceAll(Result, "\v", "\\v");
    Result = ReplaceAll(Result, "\'", "\\'");
    Result = ReplaceAll(Result, "\"", "\\\"");
    Result = ReplaceAll(Result, "\?", "\\\?");
    return Result;
}

std::string fx_StateStorage::UnEscapeString(const std::string &Str)
{
    std::string Result = Str;
    Result = ReplaceAll(Result, "\\a", "\a");
    Result = ReplaceAll(Result, "\\b", "\b");
    Result = ReplaceAll(Result, "\\f", "\f");
    Result = ReplaceAll(Result, "\\n", "\n");
    Result = ReplaceAll(Result, "\\r", "\r");
    Result = ReplaceAll(Result, "\\t", "\t");
    Result = ReplaceAll(Result, "\\v", "\v");
    Result = ReplaceAll(Result, "\\'", "\'");
    Result = ReplaceAll(Result, "\\\"", "\"");
    Result = ReplaceAll(Result, "\\\?", "\?");
    // Result = ReplaceAll(Result, "\\\\", "\\");
    return Result;
}

void fx_StateStorage::SaveString(const std::string &Name, const std::string &Value, const std::string &Type)
{
    
    if (m_FlushInterval > 0)
    {
        m_CurInterval++;
    }

    if (!m_Data.contains(Name))
    {
        m_NameIndex.push_back(Name);
    }
    VariableInfo Info;
    Info.Value = Value;
    Info.Type = Type;

    m_Data.insert_or_assign(Name, Info);
    if (m_CurInterval >= m_FlushInterval && m_FlushInterval > 0)
    {
        m_CurInterval = 0;
        Flush();
    }
}

void fx_StateStorage::GetString(const std::string &Name, std::string &Value, const std::string &Type)
{
    if (!m_Data.contains(Name))
    {
        throw std::out_of_range("fx_StateStorage::GetState");
    }
    if (!(m_Data.at(Name).Type == Type))
    {
        throw std::runtime_error(std::string("Expected `") + Name + std::string("` to have type `")  + Type + std::string("` but instead got `") + m_Data.at(Name).Type + "`");
    }
    Value = m_Data.at(Name).Value;
}

template<typename T>
std::string fx_StateStorage::ParseInt(const T &UnsignedValue, const int TypeSize)
{
    int Mem = TypeSize * 8;
    std::string Output;
    const int TotalBytes = (TypeSize *2) + 2;
    Output.resize(TotalBytes);
    Output[0] = '\\';
    Output[1] = 'x';
    for (int i = 2; i < TotalBytes - 1; i++)
    {
        Mem -= 4;
        Output[i] = Int2Char(UnsignedValue >> Mem);
    }
    Output[TotalBytes - 1] = Int2Char(UnsignedValue & 0x0F);
    return Output;
}

template<typename T>
T fx_StateStorage::PrintInt(const std::string &Input, const int TypeSize)
{
    int Mem = TypeSize * 8;
    const int TotalBytes = (TypeSize *2) + 2;
    T Value;
    Value = Char2Int(Input.back());
    for (int i = 2; i < (TotalBytes - 1); i++)
    {
        Mem -= 4;
        Value += Char2Int(Input[i]) << Mem;
    }
    return Value;
}

void fx_StateStorage::SetState(const std::string &Name, const uint8_t &Value)
{
    std::string Output = ParseInt<uint8_t>(Value, sizeof(uint8_t));
    SaveString(Name, Output, "uint8");
}

void fx_StateStorage::GetState(const std::string &Name, uint8_t &Value)
{
    std::string Output;
    GetString(Name, Output, "uint8");
    Value = PrintInt<uint8_t>(Output, sizeof(uint8_t));
}

void fx_StateStorage::SetState(const std::string &Name, const std::vector<uint8_t> &Value)
{
    std::stringstream Output;
    for (unsigned int i = 0; i < Value.size(); i++)
    {
        Output << ParseInt<uint8_t>(Value[i], sizeof(uint8_t));
    }
    SaveString(Name, Output.str(), "uint8[]");
}

void fx_StateStorage::GetState(const std::string &Name, std::vector<uint8_t> &Value)
{
    std::string Output;
    GetString(Name, Output, "uint8[]");
    const int TotalBytes = (sizeof(uint8_t) *2) + 2;
    Value.resize(Output.size() / TotalBytes);
    for (unsigned int i = 0; i < Value.size(); i++)
    {
        Value[i] = PrintInt<uint8_t>(Output.substr(i * TotalBytes, (i + 1) * TotalBytes), sizeof(uint8_t));
    }
}

void fx_StateStorage::SetState(const std::string &Name, const int8_t &Value)
{
    uint8_t PosValue = Value;
    if (Value < 0)
    {
        PosValue = -Value;
        PosValue = (~PosValue + 1);
    }
    std::string Output = ParseInt<uint8_t>(PosValue, sizeof(uint8_t));
    SaveString(Name, Output, "int8");
}

void fx_StateStorage::GetState(const std::string &Name, int8_t &Value)
{
    std::string Output;
    GetString(Name, Output, "int8");
    Value = PrintInt<uint8_t>(Output, sizeof(uint8_t));
}

void fx_StateStorage::SetState(const std::string &Name, const std::vector<int8_t> &Value)
{
    std::stringstream Output;
    for (unsigned int i = 0; i < Value.size(); i++)
    {
        uint8_t PosValue = Value[i];
        if (Value[i] < 0)
        {
            PosValue = -Value[i];
            PosValue = (~PosValue + 1);
        }
        Output << ParseInt<uint8_t>(PosValue, sizeof(uint8_t));
    }
    SaveString(Name, Output.str(), "int8[]");
}

void fx_StateStorage::GetState(const std::string &Name, std::vector<int8_t> &Value)
{
    std::string Output;
    GetString(Name, Output, "int8[]");
    const int TotalBytes = (sizeof(uint8_t) *2) + 2;
    Value.resize(Output.size() / TotalBytes);
    for (unsigned int i = 0; i < Value.size(); i++)
    {
        Value[i] = PrintInt<uint8_t>(Output.substr(i * (TotalBytes - 2), (i + 1) * (TotalBytes - 2)), sizeof(uint8_t));
    }
}

void fx_StateStorage::SetState(const std::string &Name, const uint16_t &Value)
{
    std::string Output = ParseInt<uint16_t>(Value, sizeof(uint16_t));
    SaveString(Name, Output, "uint16");
}

void fx_StateStorage::GetState(const std::string &Name, uint16_t &Value)
{
    std::string Output;
    GetString(Name, Output, "uint16");
    Value = PrintInt<uint16_t>(Output, sizeof(uint16_t));
}

void fx_StateStorage::SetState(const std::string &Name, const std::vector<uint16_t> &Value)
{
    std::stringstream Output;
    for (unsigned int i = 0; i < Value.size(); i++)
    {
        Output << ParseInt<uint16_t>(Value[i], sizeof(uint16_t));
    }
    SaveString(Name, Output.str(), "uint16[]");
}

void fx_StateStorage::GetState(const std::string &Name, std::vector<uint16_t> &Value)
{
    std::string Output;
    GetString(Name, Output, "uint16[]");
    const int TotalBytes = (sizeof(uint16_t) *2) + 2;
    Value.resize(Output.size() / TotalBytes);
    for (unsigned int i = 0; i < Value.size(); i++)
    {
        Value[i] = PrintInt<uint16_t>(Output.substr(i * (TotalBytes - 2), (i + 1) * (TotalBytes - 2)), sizeof(uint16_t));
    }
}

void fx_StateStorage::SetState(const std::string &Name, const int16_t &Value)
{
    uint16_t PosValue = Value;
    if (Value < 0)
    {
        PosValue = -Value;
        PosValue = (~PosValue + 1);
    }
    std::string Output = ParseInt<int16_t>(PosValue, sizeof(uint16_t));
    SaveString(Name, Output, "int16");
}

void fx_StateStorage::GetState(const std::string &Name, int16_t &Value)
{
    std::string Output;
    GetString(Name, Output, "int16");
    Value = PrintInt<uint16_t>(Output, sizeof(uint16_t));
}

void fx_StateStorage::SetState(const std::string &Name, const std::vector<int16_t> &Value)
{
    std::stringstream Output;
    for (unsigned int i = 0; i < Value.size(); i++)
    {
        uint16_t PosValue = Value[i];
        if (Value[i] < 0)
        {
            PosValue = -Value[i];
            PosValue = (~PosValue + 1);
        }
        Output << ParseInt<uint16_t>(PosValue, sizeof(uint16_t));
    }
    SaveString(Name, Output.str(), "int16[]");
}

void fx_StateStorage::GetState(const std::string &Name, std::vector<int16_t> &Value)
{
    std::string Output;
    GetString(Name, Output, "int16[]");
    const int TotalBytes = (sizeof(uint16_t) *2) + 2;
    Value.resize(Output.size() / TotalBytes);
    for (unsigned int i = 0; i < Value.size(); i++)
    {
        Value[i] = PrintInt<uint16_t>(Output.substr(i * (TotalBytes - 2), (i + 1) * (TotalBytes - 2)), sizeof(uint16_t));
    }
}

void fx_StateStorage::SetState(const std::string &Name, const uint32_t &Value)
{
    std::string Output = ParseInt<uint32_t>(Value, sizeof(uint32_t));
    SaveString(Name, Output, "uint32");
}

void fx_StateStorage::GetState(const std::string &Name, uint32_t &Value)
{
    std::string Output;
    GetString(Name, Output, "uint32");
    Value = PrintInt<uint32_t>(Output, sizeof(uint32_t));
}

void fx_StateStorage::SetState(const std::string &Name, const std::vector<uint32_t> &Value)
{
    std::stringstream Output;
    for (unsigned int i = 0; i < Value.size(); i++)
    {
        Output << ParseInt<uint32_t>(Value[i], sizeof(uint32_t));
    }
    SaveString(Name, Output.str(), "uint32[]");
}

void fx_StateStorage::GetState(const std::string &Name, std::vector<uint32_t> &Value)
{
    std::string Output;
    GetString(Name, Output, "uint32[]");
    const int TotalBytes = (sizeof(uint32_t) *2) + 2;
    Value.resize(Output.size() / TotalBytes);
    for (unsigned int i = 0; i < Value.size(); i++)
    {
        Value[i] = PrintInt<uint32_t>(Output.substr(i * (TotalBytes - 2), (i + 1) * (TotalBytes - 2)), sizeof(uint32_t));
    }
}

void fx_StateStorage::SetState(const std::string &Name, const int32_t &Value)
{
    uint32_t PosValue = Value;
    if (Value < 0)
    {
        PosValue = -Value;
        PosValue = (~PosValue + 1);
    }
    std::string Output = ParseInt<uint8_t>(PosValue, sizeof(uint32_t));
    SaveString(Name, Output, "int8");
}

void fx_StateStorage::GetState(const std::string &Name, int32_t &Value)
{
    std::string Output;
    GetString(Name, Output, "int32");
    Value = PrintInt<uint32_t>(Output, sizeof(uint32_t));
}

void fx_StateStorage::SetState(const std::string &Name, const std::vector<int32_t> &Value)
{
    std::stringstream Output;
    for (unsigned int i = 0; i < Value.size(); i++)
    {
        uint32_t PosValue = Value[i];
        if (Value[i] < 0)
        {
            PosValue = -Value[i];
            PosValue = (~PosValue + 1);
        }
        Output << ParseInt<uint32_t>(PosValue, sizeof(uint32_t));
    }
    SaveString(Name, Output.str(), "int32[]");
}

void fx_StateStorage::GetState(const std::string &Name, std::vector<int32_t> &Value)
{
    std::string Output;
    GetString(Name, Output, "int32[]");
    const int TotalBytes = (sizeof(uint32_t) *2) + 2;
    Value.resize(Output.size() / TotalBytes);
    for (unsigned int i = 0; i < Value.size(); i++)
    {
        Value[i] = PrintInt<uint32_t>(Output.substr(i * (TotalBytes - 2), (i + 1) * (TotalBytes - 2)), sizeof(uint32_t));
    }
}

void fx_StateStorage::SetState(const std::string &Name, const uint64_t &Value)
{
    std::string Output = ParseInt<uint64_t>(Value, sizeof(uint64_t));
    SaveString(Name, Output, "uint64");
}

void fx_StateStorage::GetState(const std::string &Name, uint64_t &Value)
{
    std::string Output;
    GetString(Name, Output, "uint64");
    Value = PrintInt<uint16_t>(Output, sizeof(uint64_t));
}

void fx_StateStorage::SetState(const std::string &Name, const std::vector<uint64_t> &Value)
{
    std::stringstream Output;
    for (unsigned int i = 0; i < Value.size(); i++)
    {
        Output << ParseInt<uint16_t>(Value[i], sizeof(uint64_t));
    }
    SaveString(Name, Output.str(), "uint64[]");
}

void fx_StateStorage::GetState(const std::string &Name, std::vector<uint64_t> &Value)
{
    std::string Output;
    GetString(Name, Output, "uint64[]");
    const int TotalBytes = (sizeof(uint64_t) *2) + 2;
    Value.resize(Output.size() / TotalBytes);
    for (unsigned int i = 0; i < Value.size(); i++)
    {
        Value[i] = PrintInt<uint64_t>(Output.substr(i * (TotalBytes - 2), (i + 1) * (TotalBytes - 2)), sizeof(uint64_t));
    }
}

void fx_StateStorage::SetState(const std::string &Name, const int64_t &Value)
{
    uint64_t PosValue = Value;
    if (Value < 0)
    {
        PosValue = -Value;
        PosValue = (~PosValue + 1);
    }
    std::string Output = ParseInt<uint64_t>(PosValue, sizeof(uint64_t));
    SaveString(Name, Output, "int64");
}

void fx_StateStorage::GetState(const std::string &Name, int64_t &Value)
{
    std::string Output;
    GetString(Name, Output, "int64");
    Value = PrintInt<uint64_t>(Output, sizeof(uint64_t));
}

void fx_StateStorage::SetState(const std::string &Name, const std::vector<int64_t> &Value)
{
    std::stringstream Output;
    for (unsigned int i = 0; i < Value.size(); i++)
    {
        uint64_t PosValue = Value[i];
        if (Value[i] < 0)
        {
            PosValue = -Value[i];
            PosValue = (~PosValue + 1);
        }
        Output << ParseInt<uint64_t>(PosValue, sizeof(uint64_t));
    }
    SaveString(Name, Output.str(), "int64[]");
}

void fx_StateStorage::GetState(const std::string &Name, std::vector<int64_t> &Value)
{
    std::string Output;
    GetString(Name, Output, "int64[]");
    const int TotalBytes = (sizeof(uint64_t) *2) + 2;
    Value.resize(Output.size() / TotalBytes);
    for (unsigned int i = 0; i < Value.size(); i++)
    {
        Value[i] = PrintInt<uint64_t>(Output.substr(i * (TotalBytes - 2), (i + 1) * (TotalBytes - 2)), sizeof(uint64_t));
    }
}

void fx_StateStorage::SetState(const std::string &Name, const std::string &Value)
{
    SaveString(Name, Value, "str");
}

void fx_StateStorage::GetState(const std::string &Name, std::string &Value)
{
    GetString(Name, Value, "str");
}

void fx_StateStorage::Flush()
{
    std::stringstream XMLcontent;
    for (const auto &x : m_NameIndex)
    {
        XMLcontent << "<var name=\"" << x << "\" value=\"" << EscapeString(m_Data.at(x).Value) << "\" type=\"" << m_Data.at(x).Type  << "\"/>\n";
    }
    fx_WriteFile(m_Path, XMLcontent.str());
}

void fx_StateStorage::Update()
{
    // std::unordered_map<std::string, VariableInfo> DataCopy;
    // DataCopy = m_Data;
    // m_Data.clear();
    std::string FileContent = fx_ReadFile(m_Path);
    tinyxml2::XMLDocument doc;
    doc.Parse(FileContent.c_str());
    for(tinyxml2::XMLElement* e = doc.FirstChildElement("var"); e != NULL; e = e->NextSiblingElement("var"))
    {
        std::string Name = std::string(e->Attribute("name"));
        if (m_Data[Name].InternalWriteOnly)
        {
            continue;
        }

        VariableInfo Info;
        Info.Value = UnEscapeString(std::string(e->Attribute("value")));
        Info.Type = std::string(e->Attribute("type"));

        if (m_Data[Name].Value != Info.Value)
        {
            m_Data[Name].Value = Info.Value;
            if (m_Data[Name].Update != NULL)
            {
                m_Data[Name].Update(Name, this);
            }
        }
    }

}

void fx_StateStorage::UpdateCallback(const std::string &Name, fx_StateUpdateCallback Callback)
{
    m_Data[Name].Update = Callback;
}

void fx_StateStorage::InternalWriteOnly(const std::string &Name, const bool &Value)
{
    m_Data[Name].InternalWriteOnly = Value;
}
