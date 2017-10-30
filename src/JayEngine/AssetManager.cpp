#include "GL\glew.h"
#include "lodepng.h"
#include "State.h"
#include "Application.h"
#include "AssetManager.h"
#include "JsonParser.h"
#include "StateManager.h"

JE_BEGIN

// Declare static member variables
ASSET::AudioMap		ASSET::m_audioMap;
ASSET::StateMap		ASSET::m_stateMap;
ASSET::TextureMap	ASSET::m_textureMap;
ASSET::ArchetypeMap	ASSET::m_archetypeMap;

void AssetManager::Load()
{
	// Load states
	JSON::ReadFile("../src/resource/register/state.json");
	const rapidjson::Value& states = JSON::GetDocument()["State"];
	for (rapidjson::SizeType i = 0; i < states.Size(); ++i) 
		STATE::PushState(states[i]["directory"].GetString(), states[i]["key"].GetString());

	// Load images
	JSON::ReadFile("../src/resource/register/asset.json");
	const rapidjson::Value& textures = JSON::GetDocument()["Texture"];
	for (rapidjson::SizeType i = 0; i < textures.Size(); ++i) 
		LoadImage(textures[i]["directory"].GetString(), textures[i]["key"].GetString());
}

void AssetManager::Unload()
{
	// Clear audio map
	//for (auto audio : m_audioMap) {
	//	if (audio.second) {
	//		delete audio.second;
	//		audio.second = nullptr;
	//	}
	//}

	// Clear texture map
	m_textureMap.clear();

	//// Clear archetype map
	//for (auto archetype : m_archetypeMap) {
	//	if (archetype.second) {
	//		delete archetype.second;
	//		archetype.second = nullptr;
	//	}
	//}
}

State* AssetManager::GetState(const char *_key)
{
	auto found = m_stateMap.find(_key);
	if (found != m_stateMap.end())
		return found->second;

	JE_DEBUG_PRINT("Cannot find such name of state resource: %s.\n", _key);
	return nullptr;
}

Audio* AssetManager::GetAudio(const char *_key)
{
	auto found = m_audioMap.find(_key);
	if (found != m_audioMap.end())
		return found->second;

	JE_DEBUG_PRINT("Cannot find such name of audio resource: %s.\n", _key);
	return nullptr;
}

unsigned AssetManager::GetTexture(const char *_key)
{
	auto found = m_textureMap.find(_key);
	if (found != m_textureMap.end())
		return found->second;

	JE_DEBUG_PRINT("Cannot find such name of texture resource: %s.\n", _key);
	return 0;
}

Archetype* AssetManager::GetArchetype(const char *_key)
{
	auto found = m_archetypeMap.find(_key);
	if (found != m_archetypeMap.end())
		return found->second;

	JE_DEBUG_PRINT("Cannot find such name of archetype resource: %s.\n", _key);
	return nullptr;
}

//void AssetManager::LoadState(const char* _path, const char* _stateKey)
//{
//	STATE::PushState(_path, _stateKey);
//}

void AssetManager::LoadAudio(const char* /*_path*/, const char* /*_audioKey*/)
{
	// TODO
	// load audio assets
}

void AssetManager::LoadImage(const char *_path, const char *_textureKey)
{
	unsigned	newImage;
	Image		image;
	unsigned	width, height;
	unsigned	error = lodepng::decode(image, width, height, _path);

	if (error)
		JE_DEBUG_PRINT("decoder error %d: %s.\n", error, lodepng_error_text(error));

	// Enable the texture for OpenGL.
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &newImage);
	glBindTexture(GL_TEXTURE_2D, newImage);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	m_textureMap.insert(TextureMap::value_type(
		_textureKey, newImage));
	
}

void AssetManager::LoadArchetype(const char* /*_path*/, const char* /*_archetypeKey*/)
{
	// TODO
	// load archetpye assets
}

JE_END
