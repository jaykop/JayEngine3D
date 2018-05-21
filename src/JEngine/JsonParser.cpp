#include "Component.h"
#include "JsonParser.h"
#include "ObjectFactory.h"
#include "stringbuffer.h"
#include "ImguiManager.h"
#include "istreamwrapper.h"
#include <fstream>

jeBegin

RJDoc JsonParser::m_document;

void JsonParser::ReadFile(const char * _dir)
{
	Close();

	std::ifstream read(_dir);
	rapidjson::IStreamWrapper toInputStream(read);
	m_document.ParseStream(toInputStream);
}

CR_RJDoc JsonParser::GetDocument()
{
	return m_document;
}

void JsonParser::Close()
{
	m_document.Clear();
}

void JsonParser::LoadObjects()
{
	CR_RJValue object = m_document["Object"];

	for (rapidjson::SizeType i = 0; i < object.Size(); ++i) {

		// Check either if object's name
		if (object[i]["Name"].IsString()) {
			FACTORY::CreateObject(object[i]["Name"].GetString());

			// Check either if object has any component
			if (object[i].HasMember("Component")) {
				CR_RJValue component = object[i]["Component"];

				// Check either if components have correct type
				for (rapidjson::SizeType j = 0; j < component.Size(); ++j) {

					if (component[j].HasMember("Type"))
						LoadComponents(component[j]);
					else
						jeDebugPrint("!JsonParser - Wrong component type or values.\n");

				} // for (rapidjson::SizeType j = 0; j < component.Size(); ++j) {
			} // if (object[i].HasMember("Component")) {

			else
				jeDebugPrint("!JsonParser - No component in this object: %s\n", object[i]["Name"].GetString());

			FACTORY::AddCreatedObject();
		} // if (object[i]["Name"].IsString()) {

		else // if (object[i]["Name"].IsString()) {
			jeDebugPrint("!JsonParser - Wrong type of object name.\n");

	} // for (rapidjson::SizeType i = 0; i < object.Size(); ++i) {
}

void JsonParser::LoadComponents(CR_RJValue _data)
{
	std::string a = _data["Type"].GetString();
		FACTORY::GetCreatedObject()->AddComponent(_data["Type"].GetString());
		Component* found = 
			FACTORY::GetCreatedObject()->GetComponent(_data["Type"].GetString());
		if (_data.HasMember("Values"))
			found->Load(_data["Values"]);
}

jeEnd
