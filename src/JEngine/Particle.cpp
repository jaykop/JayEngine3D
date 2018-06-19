#include "Particle.h"
#include "GLManager.h"
#include "Transform.h"
#include "SystemManager.h"
#include "Random.h"
#include "Object.h"
#include "MathUtils.h"
#include "Mesh.h"
#include "AssetManager.h"

#ifdef  jeUseBuiltInAllocator
#include "MemoryAllocator.h"
#endif

jeBegin
jeDefineComponentBuilder(Emitter);

using namespace Math;

const unsigned	Emitter::m_maxSize = 1000;

Emitter::Particle::Particle(Emitter* _emitter)
	: m_pEmitter(_emitter), dead(false)
{
	static Transform* s_pTransform;
	s_pTransform = m_pEmitter->GetOwner()->GetComponent<Transform>();
	
	life		= Random::GetRandomFloat(0.f, m_pEmitter->life);
	velocity	= Random::GetRandVec3(vec3::ZERO, m_pEmitter->velocity);
	position	= s_pTransform->position;
	rotation	= Random::GetRandomFloat(0.f, 360.f);
	direction	= Random::GetRandVec3(-m_pEmitter->direction, m_pEmitter->direction);
	Normalize(direction);
	color.Set(m_pEmitter->m_startColor);
	
	static float s_rotationSpeed = m_pEmitter->rotationSpeed;

	if (m_pEmitter->type == PARTICLE_EXPLODE)
		hidden = false;
	else
		hidden = true;

	if (s_rotationSpeed)
		rotationSpeed = Random::GetRandomFloat(0., s_rotationSpeed);

	if (m_pEmitter->is2d)
		direction.z = 0.f;
}

void Emitter::Particle::operator=(const Particle & _copy)
{
	static Transform* s_pTransform;
	s_pTransform = m_pEmitter->GetOwner()->GetComponent<Transform>();

	dead = _copy.dead;
	life = _copy.life;
	velocity.Set(_copy.velocity);
	position.Set(s_pTransform->position);
	rotation = _copy.rotation;
	direction.Set(_copy.direction);
	Normalize(direction);
	color.Set(_copy.color);
	hidden = _copy.hidden;
	rotationSpeed = _copy.rotationSpeed;
	direction.z = _copy.direction.z;;
}

void Emitter::Particle::Refresh()
{
	static Transform* s_pTransform;
	s_pTransform = m_pEmitter->GetOwner()->GetComponent<Transform>();

	rotation = Random::GetRandomFloat(0.f, 360.f);
	rotationSpeed = Random::GetRandomFloat(0., m_pEmitter->rotationSpeed);

	life = Random::GetRandomFloat(0.f, m_pEmitter->life);
	color.Set(m_pEmitter->m_startColor);

	if (m_pEmitter->type == Emitter::PARTICLE_NORMAL) {

		position = s_pTransform->position;
		hidden = false;
		direction = Random::GetRandVec3(-m_pEmitter->direction, m_pEmitter->direction);
	}

	else if (m_pEmitter->type == PARTICLE_EXPLODE) {

		// No more particle to update,
		// turn off the active toggle
		if (m_pEmitter->size == m_pEmitter->m_deadCount)
			m_pEmitter->active = false;

		else if (!dead) {

			// Ready for next update
			position = s_pTransform->position;
			direction = Random::GetRandVec3(-m_pEmitter->direction, m_pEmitter->direction);

			// Set dead and add number
			dead = true;
			hidden = true;
			m_pEmitter->m_deadCount++;
		}
	}

	else if (m_pEmitter->type == Emitter::PARTICLE_WIDE) {

		direction.y = -1;
		hidden = false;

		static vec3 s_position, s_range;
		s_range = m_pEmitter->range;
		s_position = s_pTransform->position;
		position.x = Random::GetRandomFloat(s_position.x - s_range.x, s_position.x + s_range.x);
		position.y = Random::GetRandomFloat(s_position.y - s_range.y, s_position.y + s_range.y);
		position.z = Random::GetRandomFloat(s_position.z - s_range.z, s_position.z + s_range.z);

		life = Random::GetRandomFloat(0.f, m_pEmitter->life);
		color.Set(m_pEmitter->m_startColor);

	}

	if (m_pEmitter->is2d)
		direction.z = 0.f;
}

Emitter::Emitter(Object* _pOwner)
	:Model(_pOwner), m_startColor(vec3::ONE), m_changeColor(true),
	m_endColor(vec3::ZERO), life(1.f), type(PARTICLE_NORMAL), is2d(false),
	direction(vec3::ZERO), velocity(vec3::ZERO), active(true), colorDiff(vec3::ZERO),
	m_deadCount(0), pointSize(0.f), range(vec3::ZERO), size(0), rotationSpeed(0.f)
{
	sfactor = GL_SRC_ALPHA;
	dfactor = GL_ONE;
	m_hiddenStatus |= IS_EMITTER;
}

Emitter::~Emitter() 
{
	// Clear particles
	for (auto particle : m_particles) {
		delete particle;
		particle = nullptr;
	}

	SYSTEM::GetGraphicSystem()->RemoveModel(this);
}

void Emitter::operator=(const Emitter & _copy)
{
	m_startColor.Set(_copy.m_startColor);
	m_changeColor = _copy.m_changeColor;
	m_endColor.Set(_copy.m_endColor);
	life = _copy.life;
	type = _copy.type;
	is2d = _copy.is2d;
	direction.Set(_copy.direction);
	velocity.Set(_copy.velocity);
	active = _copy.active;
	m_deadCount = _copy.m_deadCount;
	pointSize = _copy.pointSize;
	range.Set(_copy.range);
	size = _copy.size;
	colorDiff.Set(_copy.colorDiff);
	rotationSpeed = _copy.rotationSpeed;

	SetQuantity(size);
	auto copy = _copy.m_particles.begin();
	for (auto particle = m_particles.begin(); particle != m_particles.end();
		particle++, copy++) 
		(*particle) = (*copy);
	
}

void Emitter::Register()
{
	SYSTEM::GetGraphicSystem()->AddModel(this);
	if (GetOwner()->HasComponent<Transform>()) 
		m_pTransform = GetOwner()->GetComponent<Transform>();
}

void Emitter::ManualRefresh()
{
	m_deadCount = 0;
	for (auto particle : m_particles)
		particle->Refresh();
}

void Emitter::Load(CR_RJValue _data)
{
	if (_data.HasMember("Mesh")
		&& _data["Mesh"].GetString())
	{
		std::string meshType = _data["Mesh"].GetString();
		if (!strcmp(meshType.c_str(), "Point")) {
			m_pMeshes = Mesh::CreatePoint();
			m_pMeshes->m_shape = Mesh::MESH_POINT;
		}
		else if (!strcmp(meshType.c_str(), "Rect")) {
			m_pMeshes = Mesh::CreateRect();
			m_pMeshes->m_shape = Mesh::MESH_RECT;
		}
		else if (!strcmp(meshType.c_str(), "CrossRect")) {
			m_pMeshes = Mesh::CreateCrossRect();
			m_pMeshes->m_shape = Mesh::MESH_CROSSRECT;
		}
		else if (!strcmp(meshType.c_str(), "Cube")) {
			m_pMeshes = Mesh::CreateCube();
			m_pMeshes->m_shape = Mesh::MESH_CUBE;
		}
		else if (!strcmp(meshType.c_str(), "Tetrahedron")) {
			m_pMeshes = Mesh::CreateTetrahedron();
			m_pMeshes->m_shape = Mesh::MESH_TETRAHEDRON;
		}
		else /*if (!strcmp(meshType.c_str(), "Custom"))*/ {
			m_pMeshes = ASSET::LoadObj(meshType.c_str());
			m_pMeshes->m_shape = Mesh::MESH_NONE;
			m_pMeshes->CreateCustomObject();
		}
	}
	else {
		m_pMeshes = Mesh::CreateRect();
		m_pMeshes->m_shape = Mesh::MESH_RECT;
	}

	if (_data.HasMember("Active"))
		active = _data["Active"].GetBool();

	if (_data.HasMember("Bilboard")
		&& _data["Bilboard"].GetBool())
		status |= Model::IS_BILBOARD;

	if (_data.HasMember("Iis2d"))
		is2d = _data["Iis2d"].GetBool();

	if (_data.HasMember("Projection")) {
		CR_RJValue loadedProjection = _data["Projection"];

		if (!strcmp("Perspective", loadedProjection.GetString())) {
			projection = PROJECTION_PERSPECTIVE;
		}

		else if (!strcmp("Orhtogonal", loadedProjection.GetString())) {
			projection = PROJECTION_PERSPECTIVE;
		}
	}

	if (_data.HasMember("Texture")) {
		CR_RJValue loadedTexture = _data["Texture"];
		AddTexture(loadedTexture.GetString());
	}

	if (_data.HasMember("Life")) 
		life = _data["Life"].GetFloat();

	if (_data.HasMember("StartColor") 
		&& _data.HasMember("EndColor")) {

		CR_RJValue loadedStartColor = _data["StartColor"],
			loadedEndColor = _data["EndColor"];

		m_startColor.Set(loadedStartColor[0].GetFloat(),
			loadedStartColor[1].GetFloat(),
			loadedStartColor[2].GetFloat());

		m_endColor.Set(loadedEndColor[0].GetFloat(),
			loadedEndColor[1].GetFloat(),
			loadedEndColor[2].GetFloat());

		SetColors(m_startColor, m_endColor);
	}

	if (_data.HasMember("Type")) {
		CR_RJValue loadedType = _data["Type"];
		
		if (!strcmp(loadedType.GetString(), "Normal"))
			type = PARTICLE_NORMAL;
		else if (!strcmp(loadedType.GetString(), "Wide"))
			type = PARTICLE_WIDE;
		else if (!strcmp(loadedType.GetString(), "Explosion"))
			type = PARTICLE_EXPLODE;
	}

	if (_data.HasMember("Range")) {
		CR_RJValue loadedRange = _data["Range"];
		range.Set(loadedRange[0].GetFloat(),
			loadedRange[1].GetFloat(),
			loadedRange[2].GetFloat());
	}

	if (_data.HasMember("Direction")) {
		CR_RJValue loadedDirection = _data["Direction"];
		direction.Set(loadedDirection[0].GetFloat(),
			loadedDirection[1].GetFloat(),
			loadedDirection[2].GetFloat());
	}
	
	if (_data.HasMember("Velocity")) {
		CR_RJValue loadedVelocity = _data["Velocity"];
		velocity.Set(loadedVelocity[0].GetFloat(),
			loadedVelocity[1].GetFloat(),
			loadedVelocity[2].GetFloat());
	}

	if (_data.HasMember("Quantity"))
		SetQuantity(_data["Quantity"].GetUint());

	if (_data.HasMember("RotationSpeed"))
		rotationSpeed = _data["RotationSpeed"].GetFloat();

	if (_data.HasMember("PointSize"))
		pointSize = _data["PointSize"].GetFloat();
}

void Emitter::SetQuantity(unsigned _quantity)
{
	if (m_particles.empty()) {

		if (m_maxSize < _quantity) {
			_quantity = m_maxSize;
			jeDebugPrint("!Emitter - The quantity of particle must be less than 1000.\n");
		}

		for (unsigned i = 0; i < _quantity; ++i)
			m_particles.push_back(new Particle(this));
		size = _quantity;
	}

	else
		jeDebugPrint("!Emitter - Already allocated.\n");
}

void Emitter::SetColors(const vec3& _start, const vec3& _end)
{
	m_startColor = _start, m_endColor = _end;
	colorDiff = (m_endColor - m_startColor) / life;

	// If the idff is zero, no need to add diff
	if (colorDiff == vec3::ZERO)
		m_changeColor = false;

	else
		m_changeColor = true;
}

void Emitter::Refresh(Particle *_particle)
{
	_particle->position = m_pTransform->position;
	_particle->color = m_startColor;

}

void Emitter::EditorUpdate(const float /*_dt*/)
{
	// TODO
}

jeEnd
