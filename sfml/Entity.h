#pragma once

#include "SceneNode.h"


class Entity : public SceneNode
{
public:
	explicit		Entity(int hitPoints);

	void			setVelocity(sf::Vector2f v);
	void			setVelocity(float vx, float vy);
	void			accelerate(sf::Vector2f velocity);
	void			accelerate(float vx, float vy);
	sf::Vector2f	getVelocity() const;

	void			angularAccelerate(float av);
	int				getHitpoints() const;
	void			damage(int points);
	void			repair(int points);
	void			destroy();
	virtual bool	isDestroyed() const;
	virtual void	remove();


protected:
	virtual void	updateCurrent(sf::Time dt, CommandQueue& commands) override;

private:
	sf::Vector2f	velocity;
	float			angularVelocity{ 0.f };

	int hitPoints;

};