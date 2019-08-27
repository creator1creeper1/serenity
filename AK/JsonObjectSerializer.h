#pragma once

#include <AK/JsonArraySerializer.h>
#include <AK/JsonValue.h>

namespace AK {

template<typename Builder>
class JsonObjectSerializer {
public:
    explicit JsonObjectSerializer(Builder& builder)
        : m_builder(builder)
    {
        m_builder.append('{');
    }

    JsonObjectSerializer(const JsonObjectSerializer&) = delete;
    JsonObjectSerializer(JsonObjectSerializer&&) = delete;

    ~JsonObjectSerializer()
    {
        if (!m_finished)
            finish();
    }

    void add(const StringView& key, const JsonValue& value)
    {
        begin_item(key);
        value.serialize(m_builder);
    }

    JsonArraySerializer<Builder> add_array(const StringView& key)
    {
        begin_item(key);
        return JsonArraySerializer(m_builder);
    }

    JsonObjectSerializer<Builder> add_object(const StringView& key)
    {
        begin_item(key);
        return JsonObjectSerializer(m_builder);
    }

    void finish()
    {
        ASSERT(!m_finished);
        m_finished = true;
        m_builder.append('}');
    }

private:
    void begin_item(const StringView& key)
    {
        if (!m_empty)
            m_builder.append(',');
        m_empty = false;

        m_builder.append('"');
        m_builder.append(key);
        m_builder.append("\":");
    }

    Builder& m_builder;
    bool m_empty { true };
    bool m_finished { false };
};

template<typename Builder>
JsonObjectSerializer<Builder> JsonArraySerializer<Builder>::add_object()
{
    begin_item();
    return JsonObjectSerializer(m_builder);
}

}

using AK::JsonObjectSerializer;
