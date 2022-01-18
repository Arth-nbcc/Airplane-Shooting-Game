#include "Entity.h"
#include <cassert>

Entity::Entity(int hitPoints)
	: velocity()
	, hitPoints(hitPoints)
{}

void Entity::setVelocity(sf::Vector2f v)
{
	velocity = v;
}

void Entity::setVelocity(float vx, float vy)
{
	velocity.x = vx;
	velocity.y = vy;
}

void Entity::accelerate(sf::Vector2f velocity)
{
	this->velocity += velocity;
}

void Entity::accelerate(float vx, float vy)
{
	velocity.x += vx;
	velocity.y += vy;
}

sf::Vector2f Entity::getVelocity() const
{
	return velocity;
}

void Entity::angularAccelerate(float av)
{
	angularVelocity += av;
}

int Entity::getHitpoints() const
{
	return hitPoints;
}

void Entity::damage(int points)
{
	assert(points > 0);
	hitPoints -= points;
}

void Entity::repair(int points)
{
	assert(points > 0);
	hitPoints += points;
}

void Entity::destroy()
{
	hitPoints = 0;
}

bool Entity::isDestroyed() const
{
	return hitPoints <= 0;
}

void Entity::remove()
{
	destroy();
}

void Entity::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	move(velocity * dt.asSeconds());
	rotate(angularVelocity * dt.asSeconds());
}


