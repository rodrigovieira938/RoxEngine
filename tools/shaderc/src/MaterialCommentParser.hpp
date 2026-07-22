#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cctype>

enum class BlendMode
{
    Opaque,
    AlphaBlend,
    Additive,
    Multiply,
    Premultiplied
};

enum class CullMode
{
    None,
    Front,
    Back
};

enum class DepthTestMode
{
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always
};

inline const char* toString(BlendMode m)
{
    switch (m)
    {
        case BlendMode::Opaque:        return "opaque";
        case BlendMode::AlphaBlend:    return "alpha_blend";
        case BlendMode::Additive:      return "additive";
        case BlendMode::Multiply:      return "multiply";
        case BlendMode::Premultiplied: return "premultiplied";
    }
    return "";
}

inline const char* toString(CullMode m)
{
    switch (m)
    {
        case CullMode::None:  return "none";
        case CullMode::Front: return "front";
        case CullMode::Back:  return "back";
    }
    return "";
}

inline const char* toString(DepthTestMode m)
{
    switch (m)
    {
        case DepthTestMode::Never:        return "never";
        case DepthTestMode::Less:         return "less";
        case DepthTestMode::Equal:        return "equal";
        case DepthTestMode::LessEqual:    return "less_equal";
        case DepthTestMode::Greater:      return "greater";
        case DepthTestMode::NotEqual:     return "not_equal";
        case DepthTestMode::GreaterEqual: return "greater_equal";
        case DepthTestMode::Always:       return "always";
    }
    return "";
}

struct PermutationDef
{
    std::string name;
    std::vector<std::string> values;
};

struct MaterialTag
{
    std::string key;
    std::string value;

    bool isFlag() const { return value.empty(); }
};

struct ParseError
{
    int line = 0;
    std::string directive;
    std::string message;
};

struct MaterialFileMeta
{
    std::string name;
    std::string domain;

    std::optional<BlendMode> blend;
    std::optional<CullMode> cull;
    std::optional<DepthTestMode> depthTest;
    std::optional<bool> depthWrite;

    std::vector<PermutationDef> permutations;
    std::vector<MaterialTag> tags;

    std::vector<std::string> unknownDirectives;

    std::vector<ParseError> errors;

    bool ok() const { return errors.empty(); }
};

namespace detail
{
    inline bool isIdentChar(char c)
    {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
    }

    inline std::string trim(const std::string& s)
    {
        size_t start = 0;
        size_t end = s.size();
        while (start < end && std::isspace(s[start])) ++start;
        while (end > start && std::isspace(s[end - 1])) --end;
        return s.substr(start, end - start);
    }

    inline std::string toLower(const std::string& s)
    {
        std::string out = s;
        for (char& c : out) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return out;
    }

    // Splits on any run of whitespace. "a   b\tc" -> ["a", "b", "c"]
    inline std::vector<std::string> splitWhitespace(const std::string& s)
    {
        std::vector<std::string> out;
        size_t i = 0;
        size_t n = s.size();
        while (i < n)
        {
            while (i < n && std::isspace(s[i])) ++i;
            if (i >= n) break;
            size_t start = i;
            while (i < n && !std::isspace(s[i])) ++i;
            out.push_back(s.substr(start, i - start));
        }
        return out;
    }

    // Splits on a single delimiter char, trimming each piece.
    inline std::vector<std::string> splitChar(const std::string& s, char delim)
    {
        std::vector<std::string> out;
        size_t start = 0;
        for (size_t i = 0; i <= s.size(); ++i)
        {
            if (i == s.size() || s[i] == delim)
            {
                out.push_back(trim(s.substr(start, i - start)));
                start = i + 1;
            }
        }
        return out;
    }

    // Splits on the FIRST occurrence of delim only.
    inline bool splitOnce(const std::string& s, char delim,
                           std::string& left, std::string& right)
    {
        size_t pos = s.find(delim);
        if (pos == std::string::npos) return false;
        left = trim(s.substr(0, pos));
        right = trim(s.substr(pos + 1));
        return true;
    }

    // Splits a source blob into lines without touching <sstream>.
    inline std::vector<std::string> splitLines(const std::string& source)
    {
        std::vector<std::string> lines;
        size_t start = 0;
        for (size_t i = 0; i <= source.size(); ++i)
        {
            if (i == source.size() || source[i] == '\n')
            {
                std::string line = source.substr(start, i - start);
                if (!line.empty() && line.back() == '\r')
                    line.pop_back();
                lines.push_back(line);
                start = i + 1;
            }
        }
        return lines;
    }
}

namespace detail
{
    inline bool tryParseBlendMode(const std::string& value, BlendMode& out)
    {
        std::string v = toLower(trim(value));
        if (v == "opaque")            { out = BlendMode::Opaque; return true; }
        if (v == "alpha_blend")       { out = BlendMode::AlphaBlend; return true; }
        if (v == "additive")          { out = BlendMode::Additive; return true; }
        if (v == "multiply")          { out = BlendMode::Multiply; return true; }
        if (v == "premultiplied")     { out = BlendMode::Premultiplied; return true; }
        return false;
    }

    inline bool tryParseCullMode(const std::string& value, CullMode& out)
    {
        std::string v = toLower(trim(value));
        if (v == "none")  { out = CullMode::None; return true; }
        if (v == "front") { out = CullMode::Front; return true; }
        if (v == "back")  { out = CullMode::Back; return true; }
        return false;
    }

    inline bool tryParseDepthTestMode(const std::string& value, DepthTestMode& out)
    {
        std::string v = toLower(trim(value));
        if (v == "never")         { out = DepthTestMode::Never; return true; }
        if (v == "less")          { out = DepthTestMode::Less; return true; }
        if (v == "equal")         { out = DepthTestMode::Equal; return true; }
        if (v == "less_equal")    { out = DepthTestMode::LessEqual; return true; }
        if (v == "greater")       { out = DepthTestMode::Greater; return true; }
        if (v == "not_equal")     { out = DepthTestMode::NotEqual; return true; }
        if (v == "greater_equal") { out = DepthTestMode::GreaterEqual; return true; }
        if (v == "always")        { out = DepthTestMode::Always; return true; }
        return false;
    }

    inline bool tryParseBool(const std::string& value, bool& out)
    {
        std::string v = toLower(trim(value));
        if (v == "true" || v == "1" || v == "on" || v == "yes")  { out = true; return true; }
        if (v == "false" || v == "0" || v == "off" || v == "no") { out = false; return true; }
        return false;
    }
}

// -----------------------------------------------------------------------
// Directive-line parsing
// -----------------------------------------------------------------------

namespace detail
{
    // Given a full source line, checks whether it is a "// @directive value"
    // comment line. On success, fills directive/value and returns true.
    inline bool parseDirectiveLine(const std::string& rawLine,
                                    std::string& directive,
                                    std::string& value)
    {
        std::string line = trim(rawLine);
        if (!line.starts_with("//")) return false;

        size_t i = 2;
        size_t n = line.size();
        while (i < n && std::isspace(line[i])) ++i;

        if (i >= n || line[i] != '@') return false;
        ++i; // skip '@'

        size_t directiveStart = i;
        while (i < n && isIdentChar(line[i])) ++i;
        if (i == directiveStart) return false; // bare "@" with no name

        directive = line.substr(directiveStart, i - directiveStart);
        value = trim(line.substr(i));
        return true;
    }

    // "USE_NORMAL_MAP = 0, 1" -> name="USE_NORMAL_MAP", values=["0","1"]
    inline PermutationDef parsePermutationValue(const std::string& value)
    {
        PermutationDef def;
        std::string name, rest;
        if (splitOnce(value, '=', name, rest))
        {
            def.name = name;
            def.values = splitChar(rest, ',');
        }
        else
        {
            def.name = trim(value);
        }
        return def;
    }

    // "category=metal transparent=false lowpoly" ->
    //   {category, metal}, {transparent, false}, {lowpoly, ""}
    inline std::vector<MaterialTag> parseTagValue(const std::string& value)
    {
        std::vector<MaterialTag> tags;
        for (const std::string& token : splitWhitespace(value))
        {
            MaterialTag tag;
            std::string key, val;
            if (splitOnce(token, '=', key, val))
            {
                tag.key = key;
                tag.value = val;
            }
            else
            {
                tag.key = token;
                tag.value.clear();
            }
            tags.push_back(std::move(tag));
        }
        return tags;
    }
}

inline MaterialFileMeta parseMaterialComments(const std::string& source)
{
    using namespace detail;

    MaterialFileMeta meta;
    std::vector<std::string> lines = splitLines(source);

    for (size_t lineIndex = 0; lineIndex < lines.size(); ++lineIndex)
    {
        int lineNumber = static_cast<int>(lineIndex) + 1;

        std::string directive, value;
        if (!parseDirectiveLine(lines[lineIndex], directive, value))
            continue;

        if (directive == "material")
        {
            meta.name = value;
        }
        else if (directive == "domain")
        {
            meta.domain = value;
        }
        else if (directive == "blend")
        {
            BlendMode mode;
            if (tryParseBlendMode(value, mode))
            {
                meta.blend = mode;
            }
            else
            {
                meta.errors.push_back({lineNumber, directive,
                    "invalid value '" + value + "' for @blend "
                    "(expected: opaque, alpha_blend, additive, multiply, premultiplied)"});
            }
        }
        else if (directive == "cull")
        {
            CullMode mode;
            if (tryParseCullMode(value, mode))
            {
                meta.cull = mode;
            }
            else
            {
                meta.errors.push_back({lineNumber, directive,
                    "invalid value '" + value + "' for @cull "
                    "(expected: none, front, back)"});
            }
        }
        else if (directive == "depth_test")
        {
            DepthTestMode mode;
            if (tryParseDepthTestMode(value, mode))
            {
                meta.depthTest = mode;
            }
            else
            {
                meta.errors.push_back({lineNumber, directive,
                    "invalid value '" + value + "' for @depth_test "
                    "(expected: never, less, equal, less_equal, greater, "
                    "not_equal, greater_equal, always)"});
            }
        }
        else if (directive == "depth_write")
        {
            bool flag;
            if (tryParseBool(value, flag))
            {
                meta.depthWrite = flag;
            }
            else
            {
                meta.errors.push_back({lineNumber, directive,
                    "invalid value '" + value + "' for @depth_write "
                    "(expected: true, false, on, off, yes, no, 1, 0)"});
            }
        }
        else if (directive == "permutation")
        {
            PermutationDef def = parsePermutationValue(value);
            if (def.name.empty())
            {
                meta.errors.push_back({lineNumber, directive,
                    "@permutation requires a name"});
            }
            else
            {
                meta.permutations.push_back(std::move(def));
            }
        }
        else if (directive == "tag")
        {
            if (value.empty())
            {
                meta.errors.push_back({lineNumber, directive,
                    "@tag requires at least one key=value or flag token"});
            }
            else
            {
                std::vector<MaterialTag> parsed = parseTagValue(value);
                meta.tags.insert(meta.tags.end(), parsed.begin(), parsed.end());
            }
        }
        else
        {
            meta.unknownDirectives.push_back(directive);
            meta.errors.push_back({lineNumber, directive,
                "unknown directive '@" + directive + "'"});
        }
    }

    return meta;
}