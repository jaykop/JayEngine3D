#include "Model.h"
#include "GLManager.h"
#include "Material.h"
#include "Transform.h"
#include "AssetManager.h"
#include "SystemManager.h"
#include "Object.h"
#include "Mesh.h"

#ifdef  jeUseBuiltInAllocator
#include "MemoryAllocator.h"
#endif

jeBegin
jeDefineComponentBuilder(Model);

Model::Model(Object* _pOwner)
	:Component(_pOwner), m_drawMode(GL_TRIANGLES), color(vec4::ONE), projection(PROJECTION_PERSPECTIVE), m_mainTex(0),
	m_pTransform(nullptr), m_culled(false), m_pMaterial(nullptr), sfactor(GL_SRC_ALPHA),
	dfactor(GL_ONE_MINUS_SRC_ALPHA), m_pAnimation(nullptr), m_hiddenStatus(0x0000), m_pInherited(nullptr)
{}

void Model::Register()
{
	SYSTEM::pGraphic_->AddModel(this);
	if (GetOwner()->HasComponent<Transform>())
		m_pTransform = GetOwner()->GetComponent<Transform>();
}

void Model::AddMesh(Mesh* pMesh)
{
	return meshes_.push_back(pMesh);
}

void Model::RemoveMesh(unsigned index)
{
	unsigned count = 0;
	for (auto it = meshes_.begin();
		it != meshes_.end(); ++it, ++count) {
		if (count == index)
			meshes_.erase(it);
	}
}

Mesh* Model::GetMesh(unsigned index) const
{
	return meshes_.at(index);
}

unsigned Model::GetMeshCount() const
{
	return unsigned(meshes_.size());
}

void Model::SetParentToFollow(Object* _pObj)
{
	if (_pObj->HasComponent<Transform>()) {
		m_pInherited = _pObj->GetComponent<Transform>();
		status |= Model::IS_INHERITED;
	}
	else
		jeDebugPrint("!Model - Object to be parent does not habe transform component!: %s\n", _pObj->GetName().c_str());
}

void Model::AddTexture(const char *_key)
{
	auto found = m_textureMap.find(_key);
	if (found != m_textureMap.end())
		jeDebugPrint("!Model - Existing texture: %s.\n", _key);

	else {
		unsigned newTexture = ASSET::GetTexture(_key);

		if (m_textureMap.empty())
			m_mainTex = newTexture;

		m_textureMap.insert(
			TextureMap::value_type(
				_key, newTexture));
	}
}

void Model::RemoveTexture(const char *_key)
{
	m_textureMap.erase(_key);
}

void Model::SetCurrentTexutre(const char *_key)
{
	m_mainTex = GetTexutre(_key);
}

unsigned Model::GetCurrentTexutre() const
{
	return m_mainTex;
}

unsigned Model::GetTexutre(const char *_key)
{
	auto found = m_textureMap.find(_key);
	if (found != m_textureMap.end())
		return found->second;

	jeDebugPrint("!Model - No such name of enrolled texture: %s.\n", _key);
	return 0;
}

Model::~Model()
{
	// Remove textures
	m_textureMap.clear();
	
	// Clear mesh container
	for (auto mesh : meshes_) {
		if (!mesh->builtIn_) {
			delete mesh;
			mesh = nullptr;
		}
	}
	meshes_.clear();

	SYSTEM::pGraphic_->RemoveModel(this);
}

void Model::operator=(const Model & _copy)
{
	color.Set(_copy.color);
	projection = _copy.projection,
	m_mainTex = _copy.m_mainTex;
	m_pTransform = GetOwner()->GetComponent<Transform>();
	m_culled = _copy.m_culled;
	m_pMaterial = GetOwner()->GetComponent<Material>();
	m_hiddenStatus = _copy.m_hiddenStatus;
}

void Model::Load(CR_RJValue _data)
{
	if (_data.HasMember("Mesh")
		&& _data["Mesh"].GetString())
	{
		std::string meshType = _data["Mesh"].GetString();
		if (!strcmp(meshType.c_str(), "Point"))
			AddMesh(GLM::pMesh_[GLM::SHAPE_POINT]);

		else if (!strcmp(meshType.c_str(), "Rect"))
			AddMesh(GLM::pMesh_[GLM::SHAPE_RECT]);

		else if (!strcmp(meshType.c_str(), "CrossRect"))
			AddMesh(GLM::pMesh_[GLM::SHAPE_CROSSRECT]);

		else if (!strcmp(meshType.c_str(), "Cube"))
			AddMesh(GLM::pMesh_[GLM::SHAPE_CUBE]);

		else if (!strcmp(meshType.c_str(), "Tetrahedron"))
			AddMesh(GLM::pMesh_[GLM::SHAPE_TETRAHEDRON]);

		else /*if (!strcmp(meshType.c_str(), "Custom"))*/ {
			Mesh* pMesh = ASSET::LoadObjFile(meshType.c_str());
			GraphicSystem::DescribeVertex(pMesh);
			AddMesh(pMesh);
		}
	}
	else 
		AddMesh(GLM::pMesh_[GLM::SHAPE_RECT]);

	if (_data.HasMember("DrawMode")
		&& _data["DrawMode"].IsString()) {
		
		std::string drawType = _data["DrawMode"].GetString();
		if (!strcmp(drawType.c_str(), "Triangles"))
			m_drawMode = GL_TRIANGLES;

		else if (!strcmp(drawType.c_str(), "Triangle_Strip"))
			m_drawMode = GL_TRIANGLE_STRIP;

		else if (!strcmp(drawType.c_str(), "Triangle_Fan")) 
			m_drawMode = GL_TRIANGLE_FAN;

		else if (!strcmp(drawType.c_str(), "Lines")) 
			m_drawMode = GL_LINES;

		else if (!strcmp(drawType.c_str(), "Line_Strip")) 
			m_drawMode = GL_LINE_STRIP;
	}

	if (_data.HasMember("Flip")
		&& _data["Flip"].GetBool())
		status |= IS_FLIPPED;

	if (_data.HasMember("Color")) {
		CR_RJValue loadedColor = _data["Color"];
		color.Set(loadedColor[0].GetFloat(), loadedColor[1].GetFloat(),
			loadedColor[2].GetFloat(), loadedColor[3].GetFloat());
	}

	if (_data.HasMember("Projection")) {
		CR_RJValue loadedProjection = _data["Projection"];

		if (!strcmp("Perspective", loadedProjection.GetString()))
			projection = PROJECTION_PERSPECTIVE;

		else if (!strcmp("Orthogonal", loadedProjection.GetString()))
			projection = PROJECTION_ORTHOGONAL;

		else
			jeDebugPrint("!Model - Wrong projection type: %s\n", loadedProjection.GetString());
	}

	if (_data.HasMember("Texture")) {
		CR_RJValue loadedTexture = _data["Texture"];
		AddTexture(loadedTexture.GetString());
	}

	if (_data.HasMember("Bilboard")
		&& _data["Bilboard"].GetBool())
		status |= IS_BILBOARD;
}

void Model::EditorUpdate(const float /*dt*/)
{
	// TODO
}

jeEnd

