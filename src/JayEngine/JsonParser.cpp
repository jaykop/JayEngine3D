#include "Object.h"
#include "Component.h"
#include "JsonParser.h"
#include "ObjectFactory.h"

#include "stringbuffer.h"
#include "FileReadStream.h"

JE_BEGIN

rapidjson::Document JsonParser::m_document;

void JsonParser::ReadFile(const char * _dir)
{
	m_document.Clear();
	FILE* pFile;
	fopen_s(&pFile, _dir, "rb");
	char buffer[65536];
	rapidjson::FileReadStream read(pFile, buffer, sizeof(buffer));
	m_document.ParseStream<0, rapidjson::UTF8<>, rapidjson::FileReadStream>(read);
}

const rapidjson::Document & JsonParser::GetDocument()
{
	return m_document;
}

void JsonParser::LoadObjects()
{
	const rapidjson::Value& object = m_document["Object"];

	for (rapidjson::SizeType i = 0; i < object.Size(); ++i) {

		const rapidjson::Value& component = object[i]["Component"];

		if (component[i]["Name"].IsString()) {
			FACTORY::CreateObject(component[i]["Name"].GetString());

			for (rapidjson::SizeType j = 0; j < component.Size(); ++j)
				LoadComponents(component[j]);
		}

		else
			JE_DEBUG_PRINT("Wrong type of object name.\n");
	}
}

void JsonParser::LoadComponents(const rapidjson::Value& _data)
{
	if (_data["Type"].IsString()) {
		FACTORY::GetCreatedObject()->AddComponent(_data["Type"].GetString());
		Component* found = 
			FACTORY::GetCreatedObject()->GetComponent(_data["Type"].GetString());
		found->Load(_data["Values"]);
	}

	else
		JE_DEBUG_PRINT("Wrong type of component name.\n");
}

JE_END
