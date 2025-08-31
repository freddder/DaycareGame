#include "cParticleSpawner.h"

#include "Player.h"

#include "cLinearCongruentialGenerator.h"
#include <time.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

cParticleSpawner::cParticleSpawner(glm::vec3 position, cRenderModel _model, unsigned int maxParticles)
{
	spawnPosition = position;
	isPositionPlayerRelative = false;

	particles.resize(maxParticles);
	timer = 0.f;

	model = _model;

	// do the random
	srand((int)time(0));
	unsigned int init_seed = (rand() % 100) + 1;
	lcgPosX = cLinearCongruentialGenerator(init_seed);

	init_seed = (rand() % 100) + 1;
	lcgPosY = cLinearCongruentialGenerator(init_seed);

	init_seed = (rand() % 100) + 1;
	lcgPosZ = cLinearCongruentialGenerator(init_seed);

	init_seed = (rand() % 100) + 1;
	lcgSpdX = cLinearCongruentialGenerator(init_seed);

	init_seed = (rand() % 100) + 1;
	lcgSpdY = cLinearCongruentialGenerator(init_seed);

	init_seed = (rand() % 100) + 1;
	lcgSpdZ = cLinearCongruentialGenerator(init_seed);

	// Create buffer for particle data (position + timer)
	glGenBuffers(1, &particleBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, particleBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * maxParticles, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

cParticleSpawner::~cParticleSpawner()
{
	particles.clear();

	glDeleteBuffers(1, &particleBufferId);
}

bool cParticleSpawner::SpawnParticle()
{
	sParticle& particle = particles[trackIndex];

	// Particle still exists
	if (particle.timer > 0.f)
		return false;

	double randomPosX;
	double randomPosY;
	double randomPosZ;

	double randomSpdX;
	double randomSpdY;
	double randomSpdZ;

	lcgPosX.get_uniform_draw(randomPosX);
	lcgPosY.get_uniform_draw(randomPosY);
	lcgPosZ.get_uniform_draw(randomPosZ);

	lcgSpdX.get_uniform_draw(randomSpdX);
	lcgSpdY.get_uniform_draw(randomSpdY);
	lcgSpdZ.get_uniform_draw(randomSpdZ);

	//NewValue = (((OldValue - OldMin) * (NewMax - NewMin)) / (OldMax - OldMin)) + NewMin
	glm::vec3 newParticlePosOffset = glm::vec3(
		(((float)randomPosX * (maxPositionOffset.x - minPositionOffset.x)) / (1 - 0)) + minPositionOffset.x,
		(((float)randomPosY * (maxPositionOffset.y - minPositionOffset.y)) / (1 - 0)) + minPositionOffset.y,
		(((float)randomPosZ * (maxPositionOffset.z - minPositionOffset.z)) / (1 - 0)) + minPositionOffset.z
	);

	//NewValue = (((OldValue - OldMin) * (NewMax - NewMin)) / (OldMax - OldMin)) + NewMin
	glm::vec3 newParticleSpdOffset = glm::vec3(
		(((float)randomSpdX * (maxSpeedOffset.x - minSpeedOffset.x)) / (1 - 0)) + minSpeedOffset.x,
		(((float)randomSpdY * (maxSpeedOffset.y - minSpeedOffset.y)) / (1 - 0)) + minSpeedOffset.y,
		(((float)randomSpdZ * (maxSpeedOffset.z - minSpeedOffset.z)) / (1 - 0)) + minSpeedOffset.z
	);

	particle.position = newParticlePosOffset + spawnPosition;
	particle.velocity = newParticleSpdOffset + spawnSpeed;
	particle.timer = 0.f;

	if (isPositionPlayerRelative)
	{
		particle.position += Player::GetPlayerPosition();
	}

	if (trackIndex == particles.size() - 1)
	{
		trackIndex = 0;
	}
	else
	{
		trackIndex++;
	}

	return true;
}

void cParticleSpawner::SpawnParticles(unsigned int numToSpawn)
{
	for (unsigned int i = 0; i < numToSpawn; i++)
	{
		SpawnParticle();
	}
}

void cParticleSpawner::UpdateParticleuffer(unsigned int index)
{
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * index, sizeof(glm::vec4), glm::value_ptr(glm::vec4(particles[index].position, particles[index].timer)));
}

void cParticleSpawner::Update(float deltaTime)
{
	if (spawnRate > 0.f)
	{
		timer += deltaTime;

		if (timer > spawnRate && SpawnParticle())
		{
			timer -= spawnRate;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, particleBufferId);

	for (int i = 0; i < particles.size(); i++)
	{
		if (particles[i].timer < 0.f)
			continue;

		particles[i].timer += deltaTime;

		if (particles[i].timer > particleLifeTime)
		{
			particles[i].timer = -1.f;
			UpdateParticleuffer(i);
			continue;
		}

		// Update position
		particles[i].position += particles[i].velocity * deltaTime;

		// Upadte velocity
		particles[i].velocity += gravity * deltaTime;

		// Update buffer
		UpdateParticleuffer(i);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
