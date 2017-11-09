#pragma once
#include "AssetManager.h"
#include "ComponentManager.h"

/////////////////////////////////////////////////////////////////////////
// customize component headers
//////////////////////////////////////////////////////////////////////////
#include "SampleLogic.h"

JE_BEGIN

void AssetManager::RegisterUserAppInfo()
{
	// Set directories
	ASSET::m_initDirectory.assign("resource/register/initData.json");
	ASSET::m_stateDirectory.assign("resource/register/state.json");
	ASSET::m_assetDirectory.assign("resource/register/asset.json");
	ASSET::m_archeDirectory.assign("resource/register/archetype.json");

	// Load customized components
	JE_ADD_COMPONENT(SampleLogic);

}

JE_END