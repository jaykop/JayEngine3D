#pragma once
#include <vector>
#include "Sprite.h"
#include "Vector3.h"

JE_BEGIN

class EmitterBuilder : public ComponentBuilder
{
	friend class AssetManager;

public:

private:

	EmitterBuilder();
	~EmitterBuilder() {};
	EmitterBuilder(const EmitterBuilder& /*_copy*/) = delete;
	void operator=(const EmitterBuilder& /*_copy*/) = delete;

	Component* CreateComponent(Object* _pOwner) const override;

};

class Emitter : public Sprite 
{

	friend class EmitterBuilder;
	friend class GraphicSystem;

	enum ParticleType {PT_NORMAL};

	class Particle {

		friend class Emitter;

	public:

		vec4 m_color;
		vec3 m_position;

		void		SetTexture(const char* _key);
		unsigned	GetTexture();
		
	private:

		Particle(Emitter* _emitter);
		~Particle() {};
		Emitter* m_emitter;
		unsigned m_texture;

		Particle() = delete;
		Particle(const Particle& /*_copy*/) = delete;
		void operator=(const Particle& /*_copy*/) = delete;

	};

	using Particles = std::vector<Particle*>;

public:

	ParticleType	m_type;
	vec4			m_startColor, m_endColor;
	vec3			m_direction, m_velocity, m_position;
	float			m_life;

	void Register() override;

private:

	Emitter(Object* _owner);
	~Emitter();

	void Load(CR_RJValue _data) override;
	void Update(float _dt);
	void NormalUpdate(float _dt);
	void SetQuantity(unsigned _quantity);
	void Refresh(Particle* _particle);

	Particles m_particles;

	Emitter() = delete;
	Emitter(const Emitter& /*_copy*/) = delete;
	void operator=(const Emitter& /*_copy*/) = delete;

};

JE_END