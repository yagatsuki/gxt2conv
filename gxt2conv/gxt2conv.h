#ifndef GXT2CONV_H
#define GXT2CONV_H

#include <cstdint>
#include <unordered_map>
#include <string>
#include <vector>
#include <map>

#if defined(_WIN32) && defined(GXT2CONV_EXPORTS)
#define GXT2CONV_API __declspec(dllexport)
#elif defined(_WIN32)
#define GXT2CONV_API __declspec(dllimport)
#else
#define GXT2CONV_API
#endif

struct Entry {
    std::wstring key;
    std::wstring text;
};

constexpr uint32_t MAKE_MAGIC(char a, char b, char c, char d) {
    return (static_cast<uint32_t>(a) << 24) |
        (static_cast<uint32_t>(b) << 16) |
        (static_cast<uint32_t>(c) << 8) |
        (static_cast<uint32_t>(d));
}

constexpr uint32_t GXT2_MAGIC_LE = MAKE_MAGIC('G', 'X', 'T', '2');
constexpr uint32_t GXT2_MAGIC_BE = MAKE_MAGIC('2', 'T', 'X', 'G');

inline uint32_t swap_uint32(uint32_t val) {
    return ((val >> 24) & 0x000000FF) |
        ((val >> 8) & 0x0000FF00) |
        ((val << 8) & 0x00FF0000) |
        ((val << 24) & 0xFF000000);
}

enum Endian {
    ENDIAN_UNKNOWN = 0,
    ENDIAN_LITTLE,
    ENDIAN_BIG
};

class Gxt2File {
public:
    Gxt2File();
    ~Gxt2File();

    void clear();

    void setHashKeyMap(const std::unordered_map<uint32_t, std::wstring>& map);

    bool load(const std::wstring& filePath);
    bool save(const std::wstring& filePath) const;

    bool load_txt(const std::wstring& filePath);
    bool save_txt(const std::wstring& filePath) const;

    void setLanguageCode(uint32_t langCode);
    uint32_t languageCode() const;

    int entryCount() const;
    void addEntry(const std::wstring& key, const std::wstring& text);
    bool removeEntry(const std::wstring& key);
    bool contains(const std::wstring& key) const;
    Entry entry(const std::wstring& key) const;
    std::vector<Entry> entries() const;

private:
    std::unordered_map<uint32_t, std::wstring> m_hashKeyMap; // ハッシュ→文字列キー変換マップ
    uint32_t m_languageCode;
    Endian m_endian;
    std::map<std::wstring, std::wstring> m_entries;
};

extern "C" {

    GXT2CONV_API Gxt2File* createGxt2File();
    GXT2CONV_API void destroyGxt2File(Gxt2File* obj);

    GXT2CONV_API bool gxt2_load(Gxt2File* obj, const wchar_t* filePath);
    GXT2CONV_API bool gxt2_save(Gxt2File* obj, const wchar_t* filePath);

    GXT2CONV_API bool gxt2_load_txt(Gxt2File* obj, const wchar_t* filePath);
    GXT2CONV_API bool gxt2_save_txt(Gxt2File* obj, const wchar_t* filePath);

    GXT2CONV_API void gxt2_setLanguageCode(Gxt2File* obj, uint32_t langCode);
    GXT2CONV_API uint32_t gxt2_languageCode(Gxt2File* obj);

    GXT2CONV_API int gxt2_entryCount(Gxt2File* obj);
    GXT2CONV_API void gxt2_addEntry(Gxt2File* obj, const wchar_t* key, const wchar_t* text);
    GXT2CONV_API bool gxt2_removeEntry(Gxt2File* obj, const wchar_t* key);
    GXT2CONV_API bool gxt2_contains(Gxt2File* obj, const wchar_t* key);
}

#endif // GXT2CONV_H
