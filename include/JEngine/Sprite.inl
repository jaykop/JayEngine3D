#pragma once
#include "Sprite.h"

JE_BEGIN

template<class EffectType>
inline void Sprite::AddEffect() {

	VisualEffect::VEType veType;
	const char* effectName = typeid(EffectType).name();
	ConvertVEType(effectName, veType);

	auto found = m_effects.find(veType);
	if (found != m_effects.end())
		JE_DEBUG_PRINT("*Sprite: Existing effet - %s\n", effectName);

	else {
		m_effects.insert(
			Effects::value_type(veType,
				new EffectType(this, veType)));
	}
}

template<class EffectType>
inline EffectType* Sprite::GetEffect() {

	VisualEffect::VEType veType;
	const char* effectName = typeid(EffectType).name();
	ConvertVEType(effectName, veType);

	auto found = m_effects.find(veType);
	if (found != m_effects.end())
		return static_cast<EffectType*>(found->second);

	else {
		JE_DEBUG_PRINT("*Sprite: No such name of enrolled effect - %s\n", effectName);
		return nullptr;
	}
}

template<class EffectType>
inline void Sprite::RemoveEffect() {

	VisualEffect::VEType veType;
	const char* effectName = typeid(EffectType).name();
	ConvertVEType(effectName, veType);

	auto found = m_effects.find(type);
	if (found == m_effects.end())
		JE_DEBUG_PRINT("*Sprite: No such name of enrolled effect - %s\n", effectName);

	else {
		delete found->second;
		found->second = nullptr;
		m_effects.erase(found);
	}
}

template<class EffectType>
inline bool Sprite::HasEffect() {

	VisualEffect::VEType veType;
	const char* effectName = typeid(EffectType).name();
	ConvertVEType(effectName, veType);

	auto found = m_effects.find(veType);
	if (found != m_effects.end())
		return true;

	else {
		JE_DEBUG_PRINT("*Sprite: No such name of enrolled effect - %s\n", effectName);
		return false;
	}
}

JE_END