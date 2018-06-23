#include <algorithm>
#include "GLManager.h"
#include "GraphicSystem.h"
#include "Application.h"
#include "Transform.h"
#include "Model.h"
#include "Light.h"
#include "Camera.h"
#include "Object.h"
#include "MathUtils.h"

jeBegin

using namespace Math;

GraphicSystem::GraphicSystem()
	:System(), m_pMainCamera(nullptr), m_isLight(false), backgroundColor(vec4::ZERO),
	orthoComesFirst(true), screenColor(vec4::ONE), m_width(int(GLM::m_width)),
	m_height(int(GLM::m_height)), m_aniScale(vec3::ZERO), m_aniTranslate(vec3::ZERO),
	m_viewport(mat4()), sobelAmount(0.f), blurSize(0.f), blurAmount(0.f), m_maxLights(16), 
	aliasMode(ALIAS_ALIASED), screenEffect(EFFECT_NONE), m_resolutionScaler(GLM::m_width, GLM::m_height, 1.f)
{}

void GraphicSystem::Load(CR_RJDoc _data)
{
	if (_data.HasMember("Background")) {
		CR_RJValue color = _data["Background"];
		backgroundColor.Set(
			color[0].GetFloat(),
			color[1].GetFloat(),
			color[2].GetFloat(),
			color[3].GetFloat()
		);
	}

	if (_data.HasMember("Screen")) {
		CR_RJValue color = _data["Screen"];
		screenColor.Set(
			color[0].GetFloat(),
			color[1].GetFloat(),
			color[2].GetFloat(),
			color[3].GetFloat()
		);
	}

	if (_data.HasMember("Effect")) {
		CR_RJValue effect = _data["Effect"];
		CR_RJValue type = effect["Type"];

		if (!strcmp("None", type.GetString()))
			screenEffect = EFFECT_NONE;

		else if (!strcmp("Inverse", type.GetString()))
			screenEffect = EFFECT_INVERSE;

		else if (!strcmp("Sobel", type.GetString())) {
			static float s_recommend = 0.005f;
			screenEffect = EFFECT_SOBEL;
			if (effect.HasMember("SobelAmount")) {
				sobelAmount = effect["SobelAmount"].GetFloat();
				if (sobelAmount > s_recommend)
					jeDebugPrint("!GraphicSystem - Recommend to set sobel amount less than %f.\n", s_recommend);
			}
		}
		else if (!strcmp("Blur", type.GetString())) {
			screenEffect = EFFECT_BLUR;
			if (effect.HasMember("BlurAmount"))
				blurAmount = effect["BlurAmount"].GetFloat();
			if (effect.HasMember("BlurSize"))
				blurSize = effect["BlurSize"].GetFloat();
		}
		else
			jeDebugPrint("!GraphicSystem - Wrong type of screen effect.\n");
	}
}

void GraphicSystem::Init()
{
	// If there is no preset camera by user,
	// set the first camera as a main camera.
	if (!m_pMainCamera)
		m_pMainCamera = m_cameras[0];

	for (auto light : m_lights)
		Normalize(light->direction);
}

void GraphicSystem::Update(const float _dt)
{
	RenderToFramebuffer();
	glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);

	UpdatePipelines(_dt);

	RenderToScreen();
}

void GraphicSystem::Close()
{

}

void GraphicSystem::Unload()
{
	m_lights.clear();
	m_models.clear();
	m_cameras.clear();
}

void GraphicSystem::SortModels()
{
	if (orthoComesFirst) {
		std::sort(m_models.begin(), m_models.end(),
			[&](Model* _leftSpt, Model* _rightSpt) -> bool {

			if (_leftSpt->projection == PROJECTION_PERSPECTIVE
				&& _rightSpt->projection == PROJECTION_ORTHOGONAL)
				return true;

			else if (_leftSpt->projection == PROJECTION_ORTHOGONAL
				&& _rightSpt->projection == PROJECTION_PERSPECTIVE)
				return false;

			else
				return false;
		}
		);
	}
}

void GraphicSystem::AddModel(Model* _model)
{
	m_models.push_back(_model);
}

void GraphicSystem::RemoveModel(Model* _model)
{
	for (Models::iterator it = m_models.begin();
		it != m_models.end(); ++it) {
		if ((*it)->GetOwner()->GetId() == _model->GetOwner()->GetId()) {
			m_models.erase(it);
			break;
		}
	}
}

int GraphicSystem::GetWidth() const
{
	return m_width;
}

int	GraphicSystem::GetHeight() const
{
	return m_height;
}

void GraphicSystem::SetMainCamera(Camera* _camera)
{
	m_pMainCamera = _camera;
}

Camera* GraphicSystem::GetMainCamera() const
{
	return m_pMainCamera;
}

void GraphicSystem::AddCamera(Camera* _camera)
{
	m_cameras.push_back(_camera);
	if (m_cameras.size())
		m_pMainCamera = m_cameras[0];
}

void GraphicSystem::RemoveCamera(Camera* _camera)
{
	for (Cameras::iterator it = m_cameras.begin();
		it != m_cameras.end(); ++it) {
		if ((*it)->GetOwner()->GetId() == _camera->GetOwner()->GetId()) {
			m_cameras.erase(it);
			break;
		}
	}
}

void GraphicSystem::AddLight(Light * _light)
{
	if (m_lights.size() < m_maxLights)
		m_lights.push_back(_light);

	else
		jeDebugPrint("!GraphicSystem: JEngine cannot support the number of lights more than %d.", m_maxLights);
}

void GraphicSystem::RemoveLight(Light * _light)
{
	for (Lights::iterator it = m_lights.begin();
		it != m_lights.end(); ++it) {
		if ((*it)->GetOwner()->GetId() == _light->GetOwner()->GetId()) {
			m_lights.erase(it);
			break;
		}
	}
}

void GraphicSystem::StartAntialiasing()
{
	//Start alias mode
	switch (aliasMode)
	{
	case ALIAS_ALIASED:
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_POINT_SMOOTH);
		glDisable(GL_POLYGON_SMOOTH);
		glDisable(GL_MULTISAMPLE);
		break;

	case ALIAS_ANTIALIASED:
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_POLYGON_SMOOTH);
		glDisable(GL_MULTISAMPLE);
		break;

	case ALIAS_MULTISAMPLE:
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_POINT_SMOOTH);
		glDisable(GL_POLYGON_SMOOTH);
		glEnable(GL_MULTISAMPLE);
		break;
	}
}

void GraphicSystem::EndAntialiasing()
{
	//End alias mode
	switch (aliasMode)
	{
	case ALIAS_ANTIALIASED:
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_POLYGON_SMOOTH);
		break;

	case ALIAS_MULTISAMPLE:
		glDisable(GL_MULTISAMPLE);
		break;
	}
}

void GraphicSystem::Ray(Model* /*_model*/, Transform* /*_transform*/)
{
	//if (_model->GetOwnerId() != _transform->GetOwnerId())
	//	jeDebugPrint("!The owners of model and transform are not identical.\n");

	//static mat4 s_translate, s_scale, s_rotation,
	//	s_viewport, s_projection;
	//static vec4 s_final, s_position4;
	//static vec3 s_position3;

	//s_position3 = _transform->position;
	//s_position4.Set(s_position3.x, s_position3.y, s_position3.z, 1.f);
	//s_translate = mat4::Translate(s_position3);
	//s_scale = mat4::Scale(_transform->scale);
	//s_rotation = mat4::Rotate(_transform->rotation, _transform->rotationAxis);
	//
	//if (_model->projection == PROJECTION_PERSPECTIVE) {
	//	s_projection = m_perspective;
	//	s_viewport = mat4::LookAt(
	//		m_pMainCamera->position, m_pMainCamera->m_target, m_pMainCamera->m_up);
	//}

	//else {	// PROJECTION_ORTHOGONAL
	//	s_projection = m_orthogonal;
	//	s_viewport.SetIdentity();
	//	s_viewport = mat4::Scale(m_resolutionScaler);
	//}

	//s_final = s_projection * s_viewport * (s_scale * s_rotation * s_translate) * s_position4;

	//// TODO...
	//// http://goguri.tistory.com/entry/3D-%ED%94%BC%ED%82%B9
}

jeEnd
