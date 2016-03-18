/**
 * Created by Jian Chen
 * @since  2016.03.16
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "json.hpp"
#include <cstdlib>
#include <chen/tool/num.hpp>

using namespace chen;

// -----------------------------------------------------------------------------
// json
json::json(std::nullptr_t)
{

}

json::json(const json &o)
: _type(o._type)
, _data(o._data)
{

}

json::json(json &&o)
: _type(o._type)
, _data(o._data)
{

}

json::json(const chen::json::object &v)
: _type(JsonType::Object)
, _data(v)
{

}

json::json(chen::json::object &&v)
: _type(JsonType::Object)
, _data(chen::json::object(v))
{

}

json::json(const chen::json::array &v)
: _type(JsonType::Array)
, _data(v)
{

}

json::json(chen::json::array &&v)
: _type(JsonType::Array)
, _data(chen::json::array(v))
{

}

json::json(double v)
: _type(JsonType::Number)
, _data(v)
{

}

json::json(int v)
: _type(JsonType::Number)
, _data(static_cast<double>(v))
{

}

json::json(const std::string &v)
: _type(JsonType::String)
, _data(std::string(v))
{

}

json::json(std::string &&v)
: _type(JsonType::String)
, _data(std::string(v))
{

}

json::json(const char *v)
: _type(JsonType::String)
, _data(std::string(v))
{

}

json::json(bool v)
: _type(v ? JsonType::True : JsonType::False)
, _data(v)
{

}

// copy & move
json& json::operator=(const json &o)
{
    if (this == &o)
        return *this;

    this->clear();

    this->_type = o._type;
    this->_data = o._data;

    return *this;
}

json& json::operator=(json &&o)
{
    if (this == &o)
        return *this;

    this->clear();

    this->_type = o._type;
    this->_data = o._data;

    return *this;
}

// decode
void json::decode(const std::string &text)
{

}

// encode
std::string json::encode(std::size_t space, std::uint32_t option) const
{
    std::string out;
    std::size_t indent = 0;
    this->encode(out, space, indent, option);
    return out;
}

// clear
void json::clear()
{
    this->_type = JsonType::Null;
    this->_data.clear();
}

// type
chen::json::JsonType json::type() const
{
    return this->_type;
}

// check
bool json::isObject() const
{
    return this->_type == JsonType::Object;
}

bool json::isArray() const
{
    return this->_type == JsonType::Array;
}

bool json::isNumber() const
{
    return this->_type == JsonType::Number;
}

bool json::isString() const
{
    return this->_type == JsonType::String;
}

bool json::isTrue() const
{
    return this->_type == JsonType::True;
}

bool json::isFalse() const
{
    return this->_type == JsonType::False;
}

bool json::isNull() const
{
    return this->_type == JsonType::Null;
}

bool json::isBool() const
{
    return (this->_type == JsonType::True) || (this->_type == JsonType::False);
}

// value
chen::json::object json::asObject() const
{
    static chen::json::object object;

    switch (this->_type)
    {
        case JsonType::Object:
            return this->_data;

        case JsonType::Array:
        case JsonType::Number:
        case JsonType::String:
        case JsonType::True:
        case JsonType::False:
        case JsonType::Null:
            return object;
    }
}

chen::json::array json::asArray() const
{
    static chen::json::array array;

    switch (this->_type)
    {
        case JsonType::Object:
            return array;

        case JsonType::Array:
            return this->_data;

        case JsonType::Number:
        case JsonType::String:
        case JsonType::True:
        case JsonType::False:
        case JsonType::Null:
            return array;
    }
}

double json::asNumber() const
{
    switch (this->_type)
    {
        case JsonType::Object:
        case JsonType::Array:
            return 0.0;

        case JsonType::Number:
            return static_cast<double>(this->_data);

        case JsonType::String:
        {
            std::string d = this->_data;
            return std::atof(d.c_str());
        }

        case JsonType::True:
            return 1.0;

        case JsonType::False:
        case JsonType::Null:
            return 0.0;
    }
}

int json::asInteger() const
{
    switch (this->_type)
    {
        case JsonType::Object:
        case JsonType::Array:
            return 0;

        case JsonType::Number:
            return static_cast<int>(static_cast<double>(this->_data));

        case JsonType::String:
        {
            std::string d = this->_data;
            return std::atoi(d.c_str());
        }

        case JsonType::True:
            return 1;

        case JsonType::False:
        case JsonType::Null:
            return 0;
    }
}

std::string json::asString() const
{
    switch (this->_type)
    {
        case JsonType::Object:
        case JsonType::Array:
            return "";

        case JsonType::Number:
        {
            double d = static_cast<double>(this->_data);
            std::int64_t i = static_cast<std::int64_t>(d);
            return (d - i == 0) ? num::str(i) : num::str(d);
        }

        case JsonType::String:
            return this->_data;

        case JsonType::True:
            return "true";

        case JsonType::False:
            return "false";

        case JsonType::Null:
            return "null";
    }
}

bool json::asBool() const
{
    switch (this->_type)
    {
        // treat object as true, like javascript
        case JsonType::Object:
        case JsonType::Array:
            return true;

        case JsonType::Number:
            return static_cast<double>(this->_data) != 0.0;

        case JsonType::String:
        {
            std::string d = this->_data;
            return !d.empty();
        }

        case JsonType::True:
            return true;

        case JsonType::False:
        case JsonType::Null:
            return false;
    }
}

// encode helper
void json::encode(std::string &out, std::size_t space, std::size_t &indent, std::uint32_t option) const
{
    switch (this->_type)
    {
        case JsonType::Object:
            return this->encode(this->asObject(), out, space, indent, option);

        case JsonType::Array:
            return this->encode(this->asArray(), out, space, indent, option);

        case JsonType::Number:
            return this->encode(this->asNumber(), out, option);

        case JsonType::String:
            return this->encode(this->asString(), out, option);

        case JsonType::True:
        case JsonType::False:
            return this->encode(this->asBool(), out, option);

        case JsonType::Null:
            return this->encode(nullptr, out, option);
    }
}

// encode specific
void json::encode(const chen::json::object &v, std::string &out, std::size_t space, std::size_t &indent, std::uint32_t option) const
{
    out.append("{");

    if (space && !v.empty())
    {
        indent += space;
        out.append("\n");
    }

    auto beg = v.begin();
    auto end = v.end();

    for (auto it = beg; it != end; ++it)
    {
        auto &key = it->first;
        auto &val = it->second;

        if (it != beg)
        {
            out.append(",");

            if (space)
                out.append("\n");
        }

        if (indent)
            out.append(indent, ' ');

        out.append("\"" + key + "\":");

        if (space)
            out.append(" ");

        val.encode(out, space, indent, option);
    }

    if (space && !v.empty())
    {
        indent -= space;
        out.append("\n");
        out.append(indent, ' ');
    }

    out.append("}");
}

void json::encode(const chen::json::array &v, std::string &out, std::size_t space, std::size_t &indent, std::uint32_t option) const
{
    out.append("[");

    if (space && !v.empty())
    {
        indent += space;
        out.append("\n");
    }

    for (std::size_t i = 0, len = v.size(); i < len; ++i)
    {
        if (i)
        {
            out.append(",");

            if (space)
                out.append("\n");
        }

        if (indent)
            out.append(indent, ' ');

        v[i].encode(out, space, indent, option);
    }

    if (space && !v.empty())
    {
        indent -= space;
        out.append("\n");
        out.append(indent, ' ');
    }

    out.append("]");
}

void json::encode(double v, std::string &out, std::uint32_t option) const
{
    out.append(num::str(v));
}

void json::encode(const std::string &v, std::string &out, std::uint32_t option) const
{
    // todo handle escape
    out.append("\"" + v + "\"");
}

void json::encode(bool v, std::string &out, std::uint32_t option) const
{
    out.append(num::str(v));
}

void json::encode(std::nullptr_t, std::string &out, std::uint32_t option) const
{
    out.append("null");
}

// parse & stringify
chen::json json::parse(const std::string &text)
{
    chen::json json;
    json.decode(text);
    return json;
}

std::string json::stringify(const chen::json &json, std::size_t space, std::uint32_t option)
{
    return json.encode(space, option);
}