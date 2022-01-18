#pragma once

#include <SFML/Graphics/Sprite.hpp>

#include "Entity.h"
#include "ResourceIdentifier.h"
#include "Command.h"
#include "CommandQueue.h"
#include "Projectile.h"
#include "Animation.h"
#include <SFML/Graphics/Sprite.hpp>

class TextNode;

class Aircraft : public Entity
{
public:
	enum class Type
	{
		Eagle,
		Raptor,
		Avenger
	};

public:
	Aircraft(Type type, const TextureHolder_t& textures, const FontHolder_t& fonts);

	virtual void		 drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;
	void				 updateRollAnimation();
	virtual unsigned int getCategory() const override;

	void			     fire();
	void				 launchMissile();

	bool				 isAllied() const;

	void				 increaseFireRate();
	void				 increaseFireSpread();
	void				 collectMissiles(unsigned int count);

	virtual sf::FloatRect		getBoundingRect() const override;

	void				 remove() override;
	bool				 isMarkedForRemoval() const override;

private:
	virtual void		 updateCurrent(sf::Time dt, CommandQueue& commands) override;

	void				 updateTexts();
	void				 updateMovementPattern(sf::Time dt);

	void				 checkProjectileLaunch(sf::Time dt, CommandQueue& commands);
	void				 checkPickupDrop(CommandQueue& commands);

	void				 createBulletes(SceneNode& node, const TextureHolder_t& textures) const;
	void				 createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder_t& textures) const;

	void				 createPickup(SceneNode& node, const TextureHolder_t& textures) const;

private:
	Type				 type;
	sf::Sprite			 sprite;

	Animation			 explosion;
	bool				 showExplosion;
	bool				 hasSpawnPickup;

	int					 missileAmmo;
	int					 fireRateLevel;
	int					 spreadLevel;

	size_t				 directionIndex;
	float				 travellDistance;

	Command				 fireCommand;
	Command				 missileCommand;
	Command				 dropPickupCommand;

	sf::Time			 fireCountDown;

	bool				 isFiring;
	bool				 isLaunchingMissile;
	bool				 _isMarkedForRemoval;

	TextNode* healthDisplay;
	TextNode* missileDisplay;
};

