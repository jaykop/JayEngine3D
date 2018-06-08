#include "Light.h"
#include "SystemManager.h"
#include "GLManager.h"

#ifdef  jeUseBuiltInAllocator
#include "MemoryAllocator.h"
#endif

jeBegin
jeDefineComponentBuilder(Light);

Light::Light(Object * _pOwner)
	:Component(_pOwner), color(vec4::ONE),
	ambient(vec4::ONE), diffuse(vec4::ONE),
	specular(vec4::ONE), position(vec3(0.f, 0.f, 1.f)),
	direction(vec3::ZERO), constant(0.f), linear(0.f),
	quadratic(0.f), cutOff(0.f), outerCutOff(0.f),
	projection(PROJECTION_PERSPECTIVE),
	sfactor(GL_SRC_ALPHA), dfactor(GL_ONE_MINUS_SRC_ALPHA)
{}

Light::~Light()
{
	SYSTEM::GetGraphicSystem()->RemoveLight(this);
}

void Light::operator=(const Light & _copy)
{
	color.Set(_copy.color);
	ambient.Set(_copy.ambient); 
	diffuse.Set(_copy.diffuse);
	specular.Set(_copy.specular); 
	position.Set(_copy.position);
	direction.Set(_copy.direction); 
	constant = _copy.constant; 
	linear = _copy.linear;
	quadratic = _copy.quadratic;
	cutOff = _copy.cutOff;
	outerCutOff = _copy.outerCutOff;
	projection = _copy.projection;
}

void Light::Register()
{
	SYSTEM::GetGraphicSystem()->AddLight(this);
}

void Light::Load(CR_RJValue _data)
{
	if (_data.HasMember("Type")) {
		CR_RJValue loadedType = _data["Type"];
		if (!strcmp(loadedType.GetString(), "Normal"))
			m_type = NORMALLIGHT;
		else if (!strcmp(loadedType.GetString(), "Directional"))
			m_type = DIRECTIONALLIGHT;
		else if (!strcmp(loadedType.GetString(), "Spot"))
			m_type = SPOTLIGHT;
		else if (!strcmp(loadedType.GetString(), "Point"))
			m_type = POINTLIGHT;
	}

	if (_data.HasMember("CutOff")) {
		CR_RJValue loadedCutOff = _data["CutOff"];
		cutOff = loadedCutOff.GetFloat();
	}

	if (_data.HasMember("OuterCutOff")) {
		CR_RJValue loadedOutCutOff = _data["OuterCutOff"];
		outerCutOff = loadedOutCutOff.GetFloat();
	}

	if (_data.HasMember("Constant")) {
		CR_RJValue loadedConstant = _data["Constant"];
		constant = loadedConstant.GetFloat();
	}

	if (_data.HasMember("Linear")) {
		CR_RJValue loadedlinear = _data["Linear"];
		linear = loadedlinear.GetFloat();
	}

	if (_data.HasMember("Quadratic")) {
		CR_RJValue loadedQuadratic = _data["Quadratic"];
		quadratic = loadedQuadratic.GetFloat();
	}

	if (_data.HasMember("Direction")) {
		CR_RJValue loadedDirection = _data["Direction"];
		direction.Set(loadedDirection[0].GetFloat(), loadedDirection[1].GetFloat(), loadedDirection[2].GetFloat());
	}

	if (_data.HasMember("Color")) {
		CR_RJValue loadedColor = _data["Color"];
		color.Set(loadedColor[0].GetFloat(), loadedColor[1].GetFloat(), loadedColor[2].GetFloat(), loadedColor[3].GetFloat());
	}

	if (_data.HasMember("Ambient")) {
		CR_RJValue loadedAmbient = _data["Ambient"];
		ambient.Set(loadedAmbient[0].GetFloat(), loadedAmbient[1].GetFloat(), loadedAmbient[2].GetFloat(), loadedAmbient[3].GetFloat());
	}
	
	if (_data.HasMember("Diffuse")) {
		CR_RJValue loadedDiffuse = _data["Diffuse"];
		diffuse.Set(loadedDiffuse[0].GetFloat(), loadedDiffuse[1].GetFloat(), loadedDiffuse[2].GetFloat(), loadedDiffuse[3].GetFloat());
	}
	
	if (_data.HasMember("Specular")) {
		CR_RJValue loadedSpecular = _data["Specular"];
		specular.Set(loadedSpecular[0].GetFloat(), loadedSpecular[1].GetFloat(), loadedSpecular[2].GetFloat(), loadedSpecular[3].GetFloat());
	}
	
	if (_data.HasMember("Position")) {
		CR_RJValue loadedPosition = _data["Position"];
		position.Set(loadedPosition[0].GetFloat(), loadedPosition[1].GetFloat(), loadedPosition[2].GetFloat());
	}
}

void Light::EditorUpdate(const float /*_dt*/)
{
	// TODO
}

jeEnd
