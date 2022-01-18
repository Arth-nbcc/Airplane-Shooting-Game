#pragma once
#include "ResourceIdentifier.h"
#include "Aircraft.h"
#include "Projectile.h"
#include "Pickup.h"
#include "Particle.h"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Color.hpp>

#include <vector>
#include <functional>
#include <map>


struct Direction
{
	Direction(float a, float d) : angle(a), distance(d) {}
	float angle;
	float distance;
};


struct AircraftData
{
	int						hitPoints;
	float					speed;
	TextureID				texture;
	sf::IntRect				textureRect;
	sf::Time				fireInterval;
	std::vector<Direction>	directions;
	bool					hasRollAnimation{ false };

};

struct ProjectileData
{
	int				damage;
	float			speed;
	TextureID		texture;
	sf::IntRect		textureRect;
};

struct PickupData
{
	std::function<void(Aircraft&)>		action;
	TextureID							texture;
	sf::IntRect							textureRect;
};

struct ParticleData
{
	sf::Color				color;
	sf::Time				lifetime;
};

// functions to fill data tables
std::map<Aircraft::Type, AircraftData>	initalizeAircraftData();
std::map<Projectile::Type, ProjectileData> initializeProjectileData();
std::map<Pickup::Type, PickupData> initializePickupData();
std::map<Particle::Type, ParticleData> initializeParticleData();
