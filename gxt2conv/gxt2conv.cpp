#include "gxt2conv.h"
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <windows.h> // MultiByteToWideChar, WideCharToMultiByte

// -----------------------
// 文字コード変換関数
// -----------------------
static std::wstring Utf8ToWstring(const std::string& utf8str)
{
    if (utf8str.empty()) return L"";

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), (int)utf8str.size(), nullptr, 0);
    if (size_needed == 0) return L"";

    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), (int)utf8str.size(), &wstr[0], size_needed);
    return wstr;
}

static std::string WstringToUtf8(const std::wstring& wstr)
{
    if (wstr.empty()) return "";

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    if (size_needed == 0) return "";

    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &str[0], size_needed, nullptr, nullptr);
    return str;
}

// -----------------------
// コンストラクタ・デストラクタ
// -----------------------
Gxt2File::Gxt2File()
    : m_languageCode(0x0409), m_endian(ENDIAN_UNKNOWN)
{
}

Gxt2File::~Gxt2File()
{
}

void Gxt2File::clear()
{
    m_entries.clear();
    m_languageCode = 0x0409;
    m_endian = ENDIAN_UNKNOWN;
}

void Gxt2File::setHashKeyMap(const std::unordered_map<uint32_t, std::wstring>& map)
{
    m_hashKeyMap = map;

    // m_entries のキーをハッシュ文字列から文字列キーに置換
    std::map<std::wstring, std::wstring> newEntries;
    for (const auto& p : m_entries)
    {
        const std::wstring& oldKey = p.first;
        const std::wstring& value = p.second;

        if (oldKey.rfind(L"0x", 0) == 0 && oldKey.length() == 10) {
            try {
                uint32_t hash = static_cast<uint32_t>(std::stoul(oldKey.substr(2), nullptr, 16));
                auto it = m_hashKeyMap.find(hash);
                if (it != m_hashKeyMap.end()) {
                    newEntries[it->second] = value;
                    continue;
                }
            }
            catch (...) {}
        }
        newEntries[oldKey] = value;
    }
    m_entries = std::move(newEntries);
}

// -----------------------
// load - gxt2ファイル読み込み
// -----------------------
bool Gxt2File::load(const std::wstring& filePath)
{
    clear();

    std::ifstream file(filePath, std::ios::binary);
    if (!file)
        return false;

    uint32_t magic = 0;
    file.read(reinterpret_cast<char*>(&magic), sizeof(magic));

    if (magic == GXT2_MAGIC_LE)
        m_endian = ENDIAN_LITTLE;
    else if (magic == GXT2_MAGIC_BE)
        m_endian = ENDIAN_BIG;
    else
        return false;

    uint32_t entryCount = 0;
    file.read(reinterpret_cast<char*>(&entryCount), sizeof(entryCount));
    if (m_endian == ENDIAN_BIG)
        entryCount = swap_uint32(entryCount);

    struct RawEntry {
        uint32_t hash;
        uint32_t offset;
    };

    std::vector<RawEntry> rawEntries(entryCount);
    for (uint32_t i = 0; i < entryCount; ++i) {
        file.read(reinterpret_cast<char*>(&rawEntries[i].hash), sizeof(uint32_t));
        file.read(reinterpret_cast<char*>(&rawEntries[i].offset), sizeof(uint32_t));
        if (m_endian == ENDIAN_BIG) {
            rawEntries[i].hash = swap_uint32(rawEntries[i].hash);
            rawEntries[i].offset = swap_uint32(rawEntries[i].offset);
        }
    }

    uint32_t magic2 = 0;
    uint32_t dataLength = 0;
    file.read(reinterpret_cast<char*>(&magic2), sizeof(magic2));
    file.read(reinterpret_cast<char*>(&dataLength), sizeof(dataLength));
    if (m_endian == ENDIAN_BIG) {
        magic2 = swap_uint32(magic2);
        dataLength = swap_uint32(dataLength);
    }
    if (magic2 != GXT2_MAGIC_LE && magic2 != GXT2_MAGIC_BE)
        return false;

    uint32_t heapStart = static_cast<uint32_t>(file.tellg());
    if (heapStart > dataLength)
        return false;

    uint32_t heapLength = dataLength - heapStart;
    std::vector<char> stringHeap(heapLength);
    file.read(stringHeap.data(), heapLength);
    if (!file)
        return false;

    for (const auto& e : rawEntries)
    {
        uint32_t offset = e.offset - heapStart;
        if (offset >= heapLength)
            continue;

        const char* strPtr = stringHeap.data() + offset;
        std::string utf8str(strPtr);
        std::wstring wstr = Utf8ToWstring(utf8str);

        wchar_t buf[11]; // L"0x12345678"
        swprintf(buf, 11, L"0x%08X", e.hash);

        m_entries[buf] = wstr;
    }

    // ここでキー置換処理
    if (!m_hashKeyMap.empty()) {
        setHashKeyMap(m_hashKeyMap);
    }

    return true;
}

// -----------------------
// save - gxt2ファイル書き込み（リトルエンディアン）
// -----------------------
bool Gxt2File::save(const std::wstring& filePath) const
{
    std::ofstream file(filePath, std::ios::binary);
    if (!file) return false;

    file.write(reinterpret_cast<const char*>(&GXT2_MAGIC_LE), sizeof(uint32_t));

    uint32_t entryCount = static_cast<uint32_t>(m_entries.size());
    file.write(reinterpret_cast<const char*>(&entryCount), sizeof(entryCount));

    struct RawEntry {
        uint32_t hash;
        uint32_t offset;
    };

    std::vector<RawEntry> rawEntries(entryCount);
    std::vector<std::string> utf8Strings;
    uint32_t offsetBase = 4 /*magic*/ + 4 /*entryCount*/ + entryCount * sizeof(RawEntry) + 8 /*magic2 + dataLength*/;
    uint32_t currentOffset = offsetBase;

    size_t idx = 0;
    for (const auto& p : m_entries) {
        uint32_t hash = 0;

        // もしキーが "0x12345678" の形式なら16進値を利用
        if (p.first.rfind(L"0x", 0) == 0 && p.first.length() == 10) {
            try {
                hash = static_cast<uint32_t>(std::stoul(p.first.substr(2), nullptr, 16));
            }
            catch (...) {
                std::hash<std::wstring> hasher;
                hash = static_cast<uint32_t>(hasher(p.first));
            }
        }
        else {
            // 文字列キーなのでハッシュ値を計算
            std::hash<std::wstring> hasher;
            hash = static_cast<uint32_t>(hasher(p.first));
        }

        rawEntries[idx].hash = hash;
        rawEntries[idx].offset = currentOffset;

        std::string utf8str = WstringToUtf8(p.second);
        utf8Strings.push_back(utf8str);
        currentOffset += static_cast<uint32_t>(utf8str.size() + 1);
        idx++;
    }

    for (const auto& e : rawEntries) {
        file.write(reinterpret_cast<const char*>(&e.hash), sizeof(e.hash));
        file.write(reinterpret_cast<const char*>(&e.offset), sizeof(e.offset));
    }

    file.write(reinterpret_cast<const char*>(&GXT2_MAGIC_LE), sizeof(uint32_t));
    uint32_t dataLength = currentOffset;
    file.write(reinterpret_cast<const char*>(&dataLength), sizeof(dataLength));

    for (const auto& s : utf8Strings) {
        file.write(s.c_str(), s.size() + 1);
    }

    return file.good();
}

// -----------------------
// load_txt - TXTから読み込み
// フォーマットは「キー=値」1行1エントリ、UTF-8テキストファイル想定
// -----------------------
bool Gxt2File::load_txt(const std::wstring& filePath)
{
    clear();

    std::ifstream file(filePath, std::ios::binary);
    if (!file)
        return false;

    // ファイルサイズ取得
    file.seekg(0, std::ios::end);
    size_t fileSize = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(fileSize);
    file.read(buffer.data(), fileSize);
    if (!file)
        return false;

    // UTF-8 → wstring全体変換
    std::string utf8str(buffer.begin(), buffer.end());
    std::wistringstream wiss(Utf8ToWstring(utf8str));

    std::wstring line;
    while (std::getline(wiss, line)) {
        // コメントや空行は無視
        if (line.empty() || line[0] == L'#') continue;

        auto pos = line.find(L'=');
        if (pos == std::wstring::npos) continue;

        std::wstring key = line.substr(0, pos);
        std::wstring value = line.substr(pos + 1);

        m_entries[key] = value;
    }

    return true;
}

// -----------------------
// save_txt - TXTに保存
// フォーマットは「キー=値」1行1エントリ、UTF-8テキストファイルで出力
// -----------------------
bool Gxt2File::save_txt(const std::wstring& filePath) const
{
    std::ofstream file(filePath, std::ios::binary);
    if (!file)
        return false;

    for (const auto& p : m_entries) {
        // キー=値 のUTF-8書き込み
        std::string keyUtf8 = WstringToUtf8(p.first);
        std::string valUtf8 = WstringToUtf8(p.second);

        file.write(keyUtf8.data(), keyUtf8.size());
        file.write("=", 1);
        file.write(valUtf8.data(), valUtf8.size());
        file.write("\r\n", 2);
    }

    return true;
}

// -----------------------
// 以下getter/setter、C API実装は省略なし
// -----------------------
void Gxt2File::setLanguageCode(uint32_t langCode)
{
    m_languageCode = langCode;
}

uint32_t Gxt2File::languageCode() const
{
    return m_languageCode;
}

int Gxt2File::entryCount() const
{
    return static_cast<int>(m_entries.size());
}

void Gxt2File::addEntry(const std::wstring& key, const std::wstring& text)
{
    m_entries[key] = text;
}

bool Gxt2File::removeEntry(const std::wstring& key)
{
    return m_entries.erase(key) > 0;
}

bool Gxt2File::contains(const std::wstring& key) const
{
    return m_entries.find(key) != m_entries.end();
}

Entry Gxt2File::entry(const std::wstring& key) const
{
    Entry e;
    auto it = m_entries.find(key);
    if (it != m_entries.end()) {
        e.key = it->first;
        e.text = it->second;
    }
    return e;
}

std::vector<Entry> Gxt2File::entries() const
{
    std::vector<Entry> ret;
    for (const auto& p : m_entries)
        ret.push_back({ p.first, p.second });
    return ret;
}

// C API
extern "C" {
    GXT2CONV_API Gxt2File* createGxt2File() { return new Gxt2File(); }
    GXT2CONV_API void destroyGxt2File(Gxt2File* obj) { delete obj; }
    GXT2CONV_API bool gxt2_load(Gxt2File* obj, const wchar_t* filePath) {
        return obj && filePath && obj->load(filePath);
    }
    GXT2CONV_API bool gxt2_save(Gxt2File* obj, const wchar_t* filePath) {
        return obj && filePath && obj->save(filePath);
    }
    GXT2CONV_API bool gxt2_load_txt(Gxt2File* obj, const wchar_t* filePath) {
        return obj && filePath && obj->load_txt(filePath);
    }
    GXT2CONV_API bool gxt2_save_txt(Gxt2File* obj, const wchar_t* filePath) {
        return obj && filePath && obj->save_txt(filePath);
    }

    GXT2CONV_API void gxt2_setLanguageCode(Gxt2File* obj, uint32_t langCode) {
        if (obj) obj->setLanguageCode(langCode);
    }
    GXT2CONV_API uint32_t gxt2_languageCode(Gxt2File* obj) {
        return obj ? obj->languageCode() : 0;
    }

    GXT2CONV_API int gxt2_entryCount(Gxt2File* obj) {
        return obj ? obj->entryCount() : 0;
    }

    GXT2CONV_API void gxt2_addEntry(Gxt2File* obj, const wchar_t* key, const wchar_t* text) {
        if (obj && key && text) obj->addEntry(key, text);
    }

    GXT2CONV_API bool gxt2_removeEntry(Gxt2File* obj, const wchar_t* key) {
        return obj && key && obj->removeEntry(key);
    }

    GXT2CONV_API bool gxt2_contains(Gxt2File* obj, const wchar_t* key) {
        return obj && key && obj->contains(key);
    }
}