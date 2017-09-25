#pragma once

#include <string>

#include "file.h"
#include "third_party/jsmn/jsmn.h"

namespace input
{
	enum JsonTokenType
	{
		String,
		Array,
		Null,
		Boolean,
		Number,
		Object,
	};

	class JsonReader
	{
		struct Json
		{
			std::string json_raw;
			jsmntok_t * json_tokens = 0;
			size_t num_json_tokens = 0;
		} json_;

		Sint32 token_index_;

	public:

		JsonReader();

		~JsonReader();

		void ReadJsonFile(const std::string & path);

		void Reset();

		bool NextToken();

		bool PreviousToken();

		JsonTokenType GetTokenType();

		std::string GetString();

		bool GetBoolean();

		double GetNumber();

		inline size_t GetNumTokens() const { return json_.num_json_tokens; }

	private:

		bool TokenEquals(const std::string &value);

	};
}
