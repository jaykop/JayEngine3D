#include "Component.h"
#include "Object.h"
#include "JsonParser.h"
#include "ObjectFactory.h"
#include "stringbuffer.h"
#include "istreamwrapper.h"
#include <fstream>
#include <thread>

jeBegin

RJDoc JsonParser::document_;

void JsonParser::ReadFile(const char * directory)
{
	Close();

	std::ifstream read(directory);
	rapidjson::IStreamWrapper toInputStream(read);
	if (document_.ParseStream(toInputStream).HasParseError())
		jeDebugPrint("!JsonParser - Json file has a problem: %s", directory);

}

CR_RJDoc JsonParser::GetDocument() { return document_;}

void JsonParser::Close()
{
	document_.Clear();
}

void JsonParser::LoadObjects()
{
	CR_RJValue object = document_["Object"];

	for (rapidjson::SizeType i = 0; i < object.Size(); ++i) {

		// Check either if object's name
		if (object[i]["Name"].IsString()) {
			//FACTORY::CreateObject(object[i]["Name"].GetString());
			std::thread object_thread(&FACTORY::CreateObject, object[i]["Name"].GetString());
			object_thread.join();

			// Check either if object has any component
			if (object[i].HasMember("Component")) {
				CR_RJValue component = object[i]["Component"];

				// Check either if components have correct type
				for (rapidjson::SizeType j = 0; j < component.Size(); ++j) {

					if (component[j].HasMember("Type")) {
						LoadComponents(component[j]);
						//std::thread component_thread(&LoadComponents, component[j]);
						//component_thread.join();
					}
					else
						jeDebugPrint("!JsonParser - Wrong component type or values.\n");
				}

			} // if (object[i].HasMember("Component")) {

			else
				jeDebugPrint("!JsonParser - No component in this object: %s\n", object[i]["Name"].GetString());

			//FACTORY::AddCreatedObject();
			std::thread register_thread(&FACTORY::AddCreatedObject);
			register_thread.join();
		} // if (object[i]["Name"].IsString()) {

		else 
			jeDebugPrint("!JsonParser - Wrong type of object name.\n");

	} // for (rapidjson::SizeType i = 0; i < object.Size(); ++i) {
}

void JsonParser::LoadComponents(CR_RJValue data)
{
	FACTORY::GetCreatedObject()->AddComponent(data["Type"].GetString());
	Component* found =
		FACTORY::GetCreatedObject()->GetComponent(data["Type"].GetString());

	if (data.HasMember("Values"))
		found->Load(data["Values"]);
}

jeEnd
