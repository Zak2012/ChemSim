#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <stdint.h>

// class FilePath
// {

// private:
//     std::string m_Path;
// public:
//     FilePath(std::string Path);
//     FilePath operator/(const FilePath &rhs)
//     {
//         std::string Path;
//         if (m_Path.ends_with("/"))
//         {
//             if (rhs.m_Path.starts_with("/"))
//             {
//                 Path = m_Path + rhs.m_Path.substr(1);
//             }
//             else
//             {
//                 Path = m_Path + rhs.m_Path;
//             }
//         }
//         else
//         {
//             if (rhs.m_Path.starts_with("/"))
//             {
//                 Path = m_Path + rhs.m_Path;
//             }
//             else
//             {
//                 Path = m_Path + "/" + rhs.m_Path;
//             }
//         }
//         return FilePath(Path);
//     }
//     FilePath& operator/=(const FilePath &rhs)
//     {
//         if (m_Path.ends_with("/"))
//         {
//             if (rhs.m_Path.starts_with("/"))
//             {
//                 m_Path += rhs.m_Path.substr(1);
//             }
//             else
//             {
//                 m_Path += rhs.m_Path;
//             }
//         }
//         else
//         {
//             if (rhs.m_Path.starts_with("/"))
//             {
//                 m_Path += rhs.m_Path;
//             }
//             else
//             {
//                 m_Path += "/" + rhs.m_Path;
//             }
//         }
//         return *this;
//     }
//     FilePath operator+(const FilePath &rhs)
//     {
//         return FilePath(m_Path + rhs.m_Path);
//     }
//     FilePath& operator+=(const FilePath &rhs)
//     {
//         m_Path += rhs.m_Path;
//         return *this;
//     }
// };
// std::string GetExeFullPath();
std::string fx_ReadFile(std::string Path);
void fx_WriteFile(std::string Path, std::string Str);
void fx_WriteBinaryFile(std::string Path, std::vector<uint8_t> Buffer);
void fx_ApplyEndian(std::vector<uint8_t> &Buffer);
std::vector<uint8_t> fx_ReadBinaryFile(std::string Path);

typedef void (*fx_StateUpdateCallback)(std::string, void*);

class fx_StateStorage
{
private:
    int m_FlushInterval;
    int m_CurInterval = 0;
    struct VariableInfo
    {
        std::string Value;
        std::string Type;
        fx_StateUpdateCallback Update = NULL;
        bool InternalWriteOnly = true;
    };
    std::unordered_map<std::string, VariableInfo> m_Data;
    std::string m_Path;
    std::vector<std::string> m_NameIndex;

    void SaveString(const std::string &Name, const std::string &Value, const std::string &Type);
    void GetString(const std::string &Name, std::string &Value, const std::string &Type);
    std::string ReplaceAll(std::string str, const std::string& from, const std::string& to);
    int Char2Int(char A);
    char Int2Char(int A);
    std::string EscapeString(const std::string &Str);
    std::string UnEscapeString(const std::string &Str);

    template <typename T>
    std::string ParseInt(const T &UnsignedValue, const int TypeSize);
    template <typename T>
    T PrintInt(const std::string &Input, const int TypeSize);

public:
    fx_StateStorage(std::string Path, int FlushInterval = 5);
    ~fx_StateStorage();
    // uint8
    void SetState(const std::string &Name, const uint8_t &Value);
    void GetState(const std::string &Name, uint8_t &Value);
    // uint8[]
    void SetState(const std::string &Name, const std::vector<uint8_t> &Value);
    void GetState(const std::string &Name, std::vector<uint8_t> &Value);
    // int8
    void SetState(const std::string &Name, const int8_t &Value);
    void GetState(const std::string &Name, int8_t &Value);
    // int8[]
    void SetState(const std::string &Name, const std::vector<int8_t> &Value);
    void GetState(const std::string &Name, std::vector<int8_t> &Value);
    // uint16
    void SetState(const std::string &Name, const uint16_t &Value);
    void GetState(const std::string &Name, uint16_t &Value);
    // uint16[]
    void SetState(const std::string &Name, const std::vector<uint16_t> &Value);
    void GetState(const std::string &Name, std::vector<uint16_t> &Value);
    // int16
    void SetState(const std::string &Name, const int16_t &Value);
    void GetState(const std::string &Name, int16_t &Value);
    // int16[]
    void SetState(const std::string &Name, const std::vector<int16_t> &Value);
    void GetState(const std::string &Name, std::vector<int16_t> &Value);
    // uint32
    void SetState(const std::string &Name, const uint32_t &Value);
    void GetState(const std::string &Name, uint32_t &Value);
    // uint32[]
    void SetState(const std::string &Name, const std::vector<uint32_t> &Value);
    void GetState(const std::string &Name, std::vector<uint32_t> &Value);
    // int32
    void SetState(const std::string &Name, const int32_t &Value);
    void GetState(const std::string &Name, int32_t &Value);
    // int32[]
    void SetState(const std::string &Name, const std::vector<int32_t> &Value);
    void GetState(const std::string &Name, std::vector<int32_t> &Value);
    // uint64
    void SetState(const std::string &Name, const uint64_t &Value);
    void GetState(const std::string &Name, uint64_t &Value);
    // uint64[]
    void SetState(const std::string &Name, const std::vector<uint64_t> &Value);
    void GetState(const std::string &Name, std::vector<uint64_t> &Value);
    // int64
    void SetState(const std::string &Name, const int64_t &Value);
    void GetState(const std::string &Name, int64_t &Value);
    // int64[]
    void SetState(const std::string &Name, const std::vector<int64_t> &Value);
    void GetState(const std::string &Name, std::vector<int64_t> &Value);
    // str
    void SetState(const std::string &Name, const std::string &Value);
    void GetState(const std::string &Name, std::string &Value);

    void Flush();
    void Update();
    void UpdateCallback(const std::string &Name, fx_StateUpdateCallback Callback);
    void InternalWriteOnly(const std::string &Name, const bool &Value);

};