#include "Steering.h"
#include "CustomLogicHeader.h"
#include "Random.h"

jeBegin

using namespace Math;

// Helper functions
void Truncate(float& value, float max)
{
	if (max < value)
		value = max;
}

// Steering functions body
vec3 Steering::Seek(const vec3& _targetPos)
{
	if (m_transform) {
		Transform *transform = GetOwner()->GetComponent<Transform>();
		vec3 desiredVel = (_targetPos - transform->position_) * maxSpeed;

		return desiredVel - velocity;
	}

	return vec3::ZERO;
}

vec3 Steering::Flee(const vec3& _targetPos)
{
	if (GetOwner()->HasComponent<Transform>()) {
		Transform *transform = GetOwner()->GetComponent<Transform>();
		vec3 desiredVel = (transform->position_ - _targetPos) * maxSpeed;

		return desiredVel - velocity;
	}
	return vec3::ZERO;
}

vec3 Steering::Arrive(const vec3& _targetPos)
{
	vec3 toTarget = _targetPos - m_transform->position_;

	float distance = GetLength(toTarget);

	if (distance > 0) {

		//
		static const float decelerationTweaker = 0.3f;

		// Get new speed and modify it
		float s_speed = distance / (deceleration * decelerationTweaker);
		Truncate(s_speed, maxSpeed);

		// 
		vec3 desiredVelocity = toTarget * s_speed / distance;

		return (desiredVelocity - velocity);
	}

	return vec3::ZERO;
}

vec3 Steering::Evade(const Steering* _pursuer)
{
	vec3 toPursuer = _pursuer->m_transform->position_ - m_transform->position_;

	float lookAheadTime = GetLength(toPursuer) / (maxSpeed + GetLength(_pursuer->velocity));

	// now seek to the pridicted future position of the evader
	return Flee(_pursuer->m_transform->position_ + _pursuer->velocity * lookAheadTime);
}

vec3 Steering::Pursuit(const Steering* _evader)
{
	// If the evader is ahead and facing the agent then we can just seek
	// for the evader's current position
	vec3 toEvader = _evader->m_transform->position_ - m_transform->position_;

	float relativeHeading = DotProduct(heading, _evader->heading);

	if (DotProduct(toEvader, heading) > 0
		&& (relativeHeading < -0.95)) // acos(0.95)=18 degs
		return Seek(_evader->m_transform->position_);

	// Otherwise, this object predict where the evader will be.

	// the look-ahead time is propotional to the distanc between the evader
	// and the pursuer - and is inversly porpotional to the sum of the agent's velocities
	float lookAheadTime = GetLength(toEvader) / (maxSpeed + GetLength(_evader->velocity));

	// now seek to the pridicted future position of the evader
	return Seek(_evader->m_transform->position_ + _evader->velocity * lookAheadTime);
}

vec3 Steering::Wander()
{
	// Get the random direction from current wander target position

	// Get a direction to go next frame first
	// Get random degree
	static float randomDegree = RAND::GetRandomFloat(0, 360.F);
	static const float room = 45.f;
	static const float wanderForce = 100.f;
	// Get near random degree from last one
	randomDegree = RAND::GetRandomFloat(randomDegree - room, randomDegree + room);
	float randomRadian = Math::DegToRad(randomDegree);
	// Get random point on the circle
	vec3 nextPointToGo;
	nextPointToGo.Set(cosf(randomRadian), sinf(randomRadian), 0.f);
	Normalize(nextPointToGo);
	nextPointToGo *= wanderRadius;

	// Set the wandar center
	vec3 wanderCenter;
	wanderCenter.Set(m_transform->position_);
	wanderCenter += heading * (m_transform->scale_.x / 2.f + wanderRadius);
	wanderCenter.z = zPos;
	// Put the circle on the correct position
	circleTransform->position_.Set(wanderCenter);
	// Set the wander target position
	wanderTarget.Set(
		wanderCenter.x + nextPointToGo.x, wanderCenter.y + nextPointToGo.y, zPos);
	// Put the haircross on the correct position 
	targetTransform->position_.Set(wanderTarget);

	// Return the velocity
	return GetNormalize(wanderTarget - m_transform->position_) * wanderForce;
}

vec3 Steering::Avoid()
{
	GetIntersectedObstalces();
	
	return vec3::ZERO;
}

vec3 Steering::Calculate()
{
	// Refresh the force
	steeringForce.SetZero();

	// Force to add to steering force
	vec3 force;

	// case: Seek
	switch (m_behavior) {
	case seek:
		force = Seek(targetTransform->position_);
		break;

	case flee:
		force = Flee(targetTransform->position_);
		break;

	case arrive:
		force = Arrive(targetTransform->position_);
		break;

	case pursuit:
		force = Pursuit(m_evader);
		break;

	case evade:
		force = Evade(m_pursuer);
		break;

	case wander:
		force = Wander();
		break;

	case obstacle_avoidance:
		force = Seek(targetTransform->position_);
		break;

	default:
		break;
	}

	if (!AccumulateForce(force))
		return steeringForce;

	return steeringForce;
}

vec3 Steering::GetPosOriginatedToDitection(Transform* _transform)
{
	vec3 diff = _transform->position_ - GetWorldDetection();
	float degree = DegToRad(detectionTransform->rotation_ + m_transform->rotation_);
	vec3 vector(cosf(degree), sinf(degree));

	float length = GetLength(diff);
	float angle = DegToRad(GetAngle(GetNormalize(vector), GetNormalize(diff)));

	float newX = length * cosf(angle), newY = length * sinf(angle);
	return vec3(newX, newY, -1.f);
}

vec3 Steering::GetWorldDetection()
{
	vec4 output;

	vec3 detectionPos = detectionTransform->position_;
	vec4 detectionPos4(detectionPos.x, detectionPos.y, detectionPos.z, 1.f);

	vec3 parentPos = m_transform->position_;
	vec3 parentScale = m_transform->scale_;

	mat4 parent = Translate(parentPos) 
		* Rotate(DegToRad(m_transform->rotation_), m_transform->rotationAxis_)
		* Scale(parentScale);

	output = detectionPos4 * parent;

	return vec3(output.x, output.y, output.z);
}

void Steering::GetIntersectedObstalces()
{
	m_intersected.clear();

	for (auto obstacle : m_obstacles) {

		static const vec2 detectionScale(m_transform->scale_.x * detectionTransform->scale_.x,
			m_transform->scale_.y * detectionTransform->scale_.y);

		Transform *obstacleTransform = obstacle->GetComponent<Transform>();

		vec3 scale = obstacleTransform->scale_;
		vec3 position = GetPosOriginatedToDitection(obstacleTransform);

		// Obstacles' boundaries
		vec2 obstacleUp, obstacleDown, obstacleLeft, obstacleRight;
		obstacleUp.Set(position.x, position.y + scale.y);
		obstacleDown.Set(position.x, position.y -scale.y);
		obstacleLeft.Set(position.x -scale.x, position.y);
		obstacleRight.Set(position.x + scale.x, position.y);

		// Box boudaries
		vec2 up, down, left, right;
		up.Set(0.f, detectionScale.y);
		down.Set(0.f, -detectionScale.y);
		left.Set(-detectionScale.x, 0.f);
		right.Set(detectionScale.x, 0.f);

		//// Check intersection
		//bool intersected = !(obstacleUp.y < down.y) 
		//	&& !(obstacleDown.y > up.y)
		//	&& !(obstacleRight.x < left.x)
		//	&& !(obstacleLeft.x > right.x);

		//// Enroll the intersected ones
		//if (intersected) {
		//	jeDebugPrint("%s\n", obstacle->GetName().c_str());
		//	m_intersected.push_back(obstacle);
		//}
	}
}

bool Steering::AccumulateForce(const vec3& forceToAdd)
{
	// Get the current magnitude
	float magnitudeSoFar = GetLength(steeringForce);

	// Get the remaining magnitude
	float magnitudeRemaining = maxForce - magnitudeSoFar;

	// If there is no more remaining magnitude to add,
	// return false
	if (magnitudeRemaining <= 0.f) return false;

	// Otherwise, get force to add
	float magnitudeToAdd = GetLength(forceToAdd);

	// If that valuse is lower than capable remaining magnitude,
	// just add it
	if (magnitudeToAdd < magnitudeRemaining)
		steeringForce += forceToAdd;

	// If not, modify its length and add it
	else
		steeringForce += GetNormalize(forceToAdd) * maxForce;

	return true;
}

jeEnd