#include "json.h"

#include "file.h"

using namespace input;

JsonReader::JsonReader() :
	token_index_(0)
{
}

JsonReader::~JsonReader()
{
	if (json_.json_tokens)
	{
		delete json_.json_tokens;
	}
}

void JsonReader::ReadJsonFile(const std::string & path)
{

	if (json_.json_tokens)
	{
		delete json_.json_tokens;
	}

	jsmn_parser json_parser;
	jsmn_init(&json_parser);

	// Read the raw data
	FileReader reader;
	json_.json_raw = reader.ReadTextFile(path);

	// Get number of JSON tokens
	int num_tokens = jsmn_parse(&json_parser,
		json_.json_raw.c_str(),
		json_.json_raw.size(),
		nullptr, 0);

	if (num_tokens < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_INPUT, "Failed to parse JSON file %s.", path.c_str());
	}

	// Reset token position
	jsmn_init(&json_parser);

	if (num_tokens >= 1)
	{
		json_.num_json_tokens = num_tokens;
		json_.json_tokens = new jsmntok_t[json_.num_json_tokens];

		jsmn_parse(&json_parser,
			json_.json_raw.c_str(),
			json_.json_raw.size(),
			json_.json_tokens,
			static_cast<unsigned int>(json_.num_json_tokens));
	}
}

void JsonReader::Reset()
{
	token_index_ = 0;
}

bool JsonReader::NextToken()
{
	bool result = false;

	if (token_index_ + 1 < json_.num_json_tokens)
	{
		++token_index_;

		result = true;
	}

	return result;
}

bool JsonReader::PreviousToken()
{
	bool result = false;

	if (token_index_ - 1 > 0)
	{
		token_index_--;

		result = true;
	}

	return result;
	
}

JsonTokenType JsonReader::GetTokenType()
{

	JsonTokenType type;

	switch (json_.json_tokens[token_index_].type)
	{
	case JSMN_ARRAY: { type = JsonTokenType::Array; } break;
	case JSMN_OBJECT: { type = JsonTokenType::Object; } break;
	case JSMN_STRING: { type = JsonTokenType::String; } break;
	case JSMN_UNDEFINED: { type = JsonTokenType::Null; } break;
	case JSMN_PRIMITIVE:
	{

		if (TokenEquals("null"))
		{
			type = JsonTokenType::Null;
		}
		else if (TokenEquals("true") || TokenEquals("false"))
		{
			type = JsonTokenType::Boolean;
		}
		else
		{
			type = JsonTokenType::Number;
		}

	} break;
	}


	return JsonTokenType();
}

std::string JsonReader::GetString()
{
	std::string value;
	value.assign(json_.json_raw.c_str() + json_.json_tokens[token_index_].start,
		json_.json_tokens[token_index_].end - json_.json_tokens[token_index_].start);

	return value;
}

bool JsonReader::GetBoolean()
{
	bool value;

	if (TokenEquals("true"))
	{
		value = true;
	}
	else if (TokenEquals("false"))
	{
		value = false;
	}

	return value;
}

double JsonReader::GetNumber()
{
	return std::strtod(json_.json_raw.c_str() + json_.json_tokens[token_index_].start, 0);
}

bool JsonReader::TokenEquals(const std::string & value)
{
	jsmntok_t * token = &json_.json_tokens[token_index_];
	return ((int)SDL_strlen(value.c_str()) == token->end - token->start) &&
		(SDL_strncmp(json_.json_raw.c_str() + token->start, value.c_str(), token->end - token->start) == 0);
}