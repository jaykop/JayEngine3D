#include "Model.h"
#include "Object.h"
#include "Material.h"

#ifdef  jeUseBuiltInAllocator
#include "MemoryAllocator.h"
#endif

jeBegin
jeDefineComponentBuilder(Material);

Material::Material(Object* _pOwner)
	:Component(_pOwner), m_diffuse(0), 
	m_specular(0), m_shininess(1.f)
{
	// Connect to sprite's pointer
	if (_pOwner->HasComponent<Sprite>()
		&& (_pOwner->GetComponent<Sprite>()->m_status & Sprite::HAS_MATERIAL)
			== Sprite::HAS_MATERIAL) {
		_pOwner->GetComponent<Sprite>()->m_material = this;
		_pOwner->GetComponent<Sprite>()->m_status |= Sprite::HAS_MATERIAL;
	}

	else if (_pOwner->HasComponent<Model>()
		&& (_pOwner->GetComponent<Model>()->m_status & Sprite::HAS_MATERIAL)
		== Sprite::HAS_MATERIAL) {
		_pOwner->GetComponent<Model>()->m_material = this;
		_pOwner->GetComponent<Sprite>()->m_status |= Sprite::HAS_MATERIAL;
	}

	else
		jeDebugPrint("!Material - This object has no sprite componnet: %s\n", _pOwner->GetName().c_str());
}

Material::~Material()
{
	// Turn off the toggle
	if (GetOwner()->HasComponent<Sprite>()) {
		GetOwner()->GetComponent<Sprite>()->m_material = nullptr;
		GetOwner()->GetComponent<Sprite>()->m_status &= ~Sprite::HAS_MATERIAL;
	}
}

void Material::operator=(const Material & _copy)
{
	m_diffuse = _copy.m_diffuse;
	m_specular = _copy.m_specular;
	m_shininess = _copy.m_shininess;
	
	if (GetOwner()->HasComponent<Sprite>()
		&& (GetOwner()->GetComponent<Sprite>()->m_status & Sprite::HAS_MATERIAL)
		== Sprite::HAS_MATERIAL) {
		GetOwner()->GetComponent<Sprite>()->m_material = this;
		GetOwner()->GetComponent<Sprite>()->m_status |= Sprite::HAS_MATERIAL;
	}
	else if (GetOwner()->HasComponent<Model>()
		&& (GetOwner()->GetComponent<Model>()->m_status & Sprite::HAS_MATERIAL)
		== Sprite::HAS_MATERIAL) {
		GetOwner()->GetComponent<Model>()->m_material = this;
		GetOwner()->GetComponent<Sprite>()->m_status |= Sprite::HAS_MATERIAL;
	}

}

void Material::Load(CR_RJValue _data)
{
	if (_data.HasMember("Diffuse")) {
		CR_RJValue diffuse = _data["Diffuse"];
		m_diffuse = diffuse.GetInt();
	}

	if (_data.HasMember("Specular")) {
		CR_RJValue specular = _data["Specular"];
		m_specular = specular.GetInt();
	}
	
	if (_data.HasMember("Shininess")) {
		CR_RJValue shininess = _data["Shininess"];
		m_shininess = shininess.GetFloat();
	}
}

void Material::EditorUpdate(const float /*_dt*/)
{
	// TODO
}

jeEnd
