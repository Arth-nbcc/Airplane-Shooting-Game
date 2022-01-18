#include "DataTables.h"

std::map<Aircraft::Type, AircraftData> initalizeAircraftData()
{
	std::map<Aircraft::Type, AircraftData> data;

	//Eagle data
	data[Aircraft::Type::Eagle].hitPoints = 100;
	data[Aircraft::Type::Eagle].speed = 200.f;
	data[Aircraft::Type::Eagle].texture = TextureID::Entities;
	data[Aircraft::Type::Eagle].textureRect = sf::IntRect(0, 0, 48, 64);
	data[Aircraft::Type::Eagle].fireInterval = sf::seconds(1);
	data[Aircraft::Type::Eagle].hasRollAnimation = true;

	//Raptor data
	data[Aircraft::Type::Raptor].hitPoints = 100;
	data[Aircraft::Type::Raptor].speed = 80.f;
	data[Aircraft::Type::Raptor].texture = TextureID::Entities;
	data[Aircraft::Type::Raptor].textureRect = sf::IntRect(144, 0, 84, 64);
	data[Aircraft::Type::Raptor].fireInterval = sf::Time::Zero;

	//Raptor data auto pilot
	data[Aircraft::Type::Raptor].directions.push_back(Direction(+80.f, 80.f));
	data[Aircraft::Type::Raptor].directions.push_back(Direction(0.f, 160.f));
	data[Aircraft::Type::Raptor].directions.push_back(Direction(-80.f, 160.f));
	data[Aircraft::Type::Raptor].directions.push_back(Direction(0.f, 160.f));
	data[Aircraft::Type::Raptor].directions.push_back(Direction(+80.f, 80.f));

	//Avenger data
	data[Aircraft::Type::Avenger].hitPoints = 100;
	data[Aircraft::Type::Avenger].speed = 150.f;
	data[Aircraft::Type::Avenger].texture = TextureID::Entities;
	data[Aircraft::Type::Avenger].textureRect = sf::IntRect(228, 0, 60, 59);
	data[Aircraft::Type::Avenger].fireInterval = sf::seconds(2);

	//Avenger auto pilot
	data[Aircraft::Type::Avenger].directions.push_back(Direction(+70.f, 150.f));
	data[Aircraft::Type::Avenger].directions.push_back(Direction(0.f, 25.f));
	data[Aircraft::Type::Avenger].directions.push_back(Direction(-70.f, 300.f));
	data[Aircraft::Type::Avenger].directions.push_back(Direction(0.f, 25.f));
	data[Aircraft::Type::Avenger].directions.push_back(Direction(+70.f, 150.f));


	return data;
}


std::map<Projectile::Type, ProjectileData> initializeProjectileData()
{
	std::map<Projectile::Type, ProjectileData> data;

	data[Projectile::Type::AlliedBullet].damage = 10;
	data[Projectile::Type::AlliedBullet].speed = 300.f;
	data[Projectile::Type::AlliedBullet].texture = TextureID::Entities;
	data[Projectile::Type::AlliedBullet].textureRect = sf::IntRect(175, 64, 3, 14);

	data[Projectile::Type::EnemyBullet].damage = 10;
	data[Projectile::Type::EnemyBullet].speed = 300.f;
	data[Projectile::Type::EnemyBullet].texture = TextureID::Entities;
	data[Projectile::Type::EnemyBullet].textureRect = sf::IntRect(175, 64, 3, 14);

	data[Projectile::Type::Missile].damage = 200;
	data[Projectile::Type::Missile].speed = 150.f;
	data[Projectile::Type::Missile].texture = TextureID::Entities;
	data[Projectile::Type::Missile].textureRect = sf::IntRect(160, 64, 15, 32);

	return data;
}


std::map<Pickup::Type, PickupData> initializePickupData()
{
	std::map<Pickup::Type, PickupData> data;

	data[Pickup::Type::HealthRefill].texture = TextureID::Entities;
	data[Pickup::Type::HealthRefill].textureRect = sf::IntRect(0, 64, 40, 40);
	data[Pickup::Type::HealthRefill].action = [](Aircraft& a) {a.repair(25); };

	data[Pickup::Type::MissileRefill].texture = TextureID::Entities;
	data[Pickup::Type::MissileRefill].textureRect = sf::IntRect(40, 64, 40, 40);
	data[Pickup::Type::MissileRefill].action = [](Aircraft& a) {a.collectMissiles(3); };

	data[Pickup::Type::FireSpread].texture = TextureID::Entities;
	data[Pickup::Type::FireSpread].textureRect = sf::IntRect(80, 64, 40, 40);
	data[Pickup::Type::FireSpread].action = [](Aircraft& a) {a.increaseFireSpread(); };

	data[Pickup::Type::FireRate].texture = TextureID::Entities;
	data[Pickup::Type::FireRate].textureRect = sf::IntRect(120, 64, 40, 40);
	data[Pickup::Type::FireRate].action = [](Aircraft& a) {a.increaseFireRate(); };

	return data;
}

std::map<Particle::Type, ParticleData> initializeParticleData()
{
	std::map<Particle::Type, ParticleData> data;

	data[Particle::Type::Propellant].color = sf::Color(255, 255, 50);
	data[Particle::Type::Propellant].lifetime = sf::seconds(0.6f);

	data[Particle::Type::Smoke].color = sf::Color(50, 50, 50);
	data[Particle::Type::Smoke].lifetime = sf::seconds(4.f);

	return data;
}