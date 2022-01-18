#include "Aircraft.h"
#include "ResourceHolder.h"
#include "Utility.h"
#include "Category.h"
#include "DataTables.h"
#include "TextNode.h"
#include "Pickup.h"


#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <memory>
#include <stdexcept>

//this is called Anonymus namespace
namespace {
	const std::map<Aircraft::Type, AircraftData> TABLE = initalizeAircraftData();
}


Aircraft::Aircraft(Type type, const TextureHolder_t& textures, const FontHolder_t& fonts)
	: Entity(TABLE.at(type).hitPoints)
	, type(type)
	, sprite(textures.get(TABLE.at(type).texture), TABLE.at(type).textureRect)
	, explosion(textures.get(TextureID::Explosion))
	, showExplosion(true)
	, hasSpawnPickup(false)
	, missileAmmo(5)
	, fireRateLevel(1)
	, spreadLevel(1)
	, directionIndex(0)
	, travellDistance(0)
	, fireCommand()
	, missileCommand()
	, fireCountDown(sf::Time::Zero)
	, isFiring(false)
	, isLaunchingMissile(false)
	, _isMarkedForRemoval(false)
	, healthDisplay(nullptr)
	, missileDisplay(nullptr)
{
	explosion.setFrameSize(sf::Vector2i(256, 256));
	explosion.setNumFrames(16);
	explosion.setDuration(sf::seconds(1));
	centerOrigin(explosion);

	centerOrigin(sprite);

	//fireCommand
	fireCommand.category = Category::SceneAirLayer;
	fireCommand.action = [this, &textures](SceneNode& node, sf::Time)
	{
		this->createBulletes(node, textures);
	};

	//missileCommand
	missileCommand.category = Category::SceneAirLayer;
	missileCommand.action = [this, &textures](SceneNode& node, sf::Time)
	{
		this->createProjectile(node, Projectile::Type::Missile, 0.f, 0.5f, textures);
	};

	//dropPickup
	dropPickupCommand.category = Category::SceneAirLayer;
	dropPickupCommand.action = [this, &textures](SceneNode& node, sf::Time)
	{
		this->createPickup(node, textures);
	};

	std::unique_ptr<TextNode> health(new TextNode(fonts, ""));
	healthDisplay = health.get(); ///< raw pointer to healthDisplay node
	attachChild(std::move(health));

	if (getCategory() == Category::PlayerAircraft)
	{
		std::unique_ptr<TextNode> missiles(new TextNode(fonts, ""));
		missiles->setPosition(0, 70);
		missileDisplay = missiles.get();
		attachChild(std::move(missiles));
	}

	updateTexts();
}

void Aircraft::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (isDestroyed() && showExplosion)
		target.draw(explosion, states);
	else
		target.draw(sprite, states);
}

unsigned int Aircraft::getCategory() const
{
	switch (type)
	{
	case Type::Eagle:
		return Category::PlayerAircraft;

	default:
		return Category::EnemyAircraft;
	}
}

void Aircraft::fire()
{
	//only ships with fire interval != 0 are able to fire
	if (TABLE.at(type).fireInterval != sf::Time::Zero)
	{
		isFiring = true;
	}
}

void Aircraft::launchMissile()
{
	if (missileAmmo > 0) {
		isLaunchingMissile = true;
		--missileAmmo;
	}
}

bool Aircraft::isAllied() const
{
	return type == Type::Eagle;
}

void Aircraft::increaseFireRate()
{
	if (fireRateLevel < 10) {
		++fireRateLevel;
	}
}

void Aircraft::increaseFireSpread()
{
	if (spreadLevel < 3) {
		++spreadLevel;
	}
}

void Aircraft::collectMissiles(unsigned int count)
{
	missileAmmo += count;
}

sf::FloatRect Aircraft::getBoundingRect() const
{
	return getWorldTransform().transformRect(sprite.getGlobalBounds());
}

void Aircraft::remove()
{
	_isMarkedForRemoval = true;
	Entity::remove();
	showExplosion = false;
}

bool Aircraft::isMarkedForRemoval() const
{
	return _isMarkedForRemoval;
}


void Aircraft::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	updateTexts();

	//check if aircraft has been destroyed
	if (isDestroyed())
	{
		//check for pickup drop
		checkPickupDrop(commands);
		explosion.update(dt);
		return;
	}
	//
	//check if bullets or missiles re fired
	checkProjectileLaunch(dt, commands);
	updateMovementPattern(dt);	//follow autopilot

	updateRollAnimation();
	updateTexts();

	Entity::updateCurrent(dt, commands);
}

void Aircraft::updateRollAnimation()
{
	//update roll animation
	if (TABLE.at(type).hasRollAnimation)
	{
		sf::IntRect textureRect = TABLE.at(type).textureRect;

		//roll left: Texture rect offset once
		if (getVelocity().x < 0.f)
			textureRect.left += textureRect.width;

		//roll right: Texture rect offset twice
		else if (getVelocity().x > 0.f)
			textureRect.left += 2 * textureRect.width;

		sprite.setTextureRect(textureRect);
	}
}

void Aircraft::updateTexts()
{
	if (isDestroyed())
	{
		healthDisplay->setText("");
		return;
	}

	healthDisplay->setText(std::to_string(getHitpoints()) + " HP");
	healthDisplay->setPosition(0.f, 50.f);
	healthDisplay->setRotation(-getRotation());

	if (missileDisplay)
	{
		if (missileAmmo == 0)
			missileDisplay->setText("");
		else
			missileDisplay->setText("M: " + std::to_string(missileAmmo));
	}
}

void Aircraft::updateMovementPattern(sf::Time dt)
{
	const std::vector<Direction>& directions = TABLE.at(type).directions;

	if (!directions.empty())
	{
		float distanceToTravel = directions.at(directionIndex).distance;
		if (travellDistance > distanceToTravel)
		{
			//go to next leg
			directionIndex = ++directionIndex % directions.size();
			travellDistance = 0;
		}

		travellDistance += TABLE.at(type).speed * dt.asSeconds();

		//set velocity from autopilot
		float dirAngle = directions[directionIndex].angle + 90.f;
		float vx = TABLE.at(type).speed * gex::cos(dirAngle);
		float vy = TABLE.at(type).speed * gex::sin(dirAngle);
		setVelocity(vx, vy);
	}
}

void Aircraft::checkProjectileLaunch(sf::Time dt, CommandQueue& commands)
{
	//enemy try to fire all the time
	if (!isAllied()) {
		fire();
	}

	//check for bullet fire
	if (isFiring && fireCountDown <= sf::Time::Zero)
	{
		//interval expired: We can fire a new bullet
		commands.push(fireCommand);
		fireCountDown += TABLE.at(type).fireInterval / (fireRateLevel + 1.f);

	}
	else if (fireCountDown > sf::Time::Zero) {
		//interval not expired: Decrease it further
		fireCountDown -= dt;
	}

	//check for missile launch
	if (isLaunchingMissile) {
		commands.push(missileCommand);
	}

	isLaunchingMissile = false;
	isFiring = false;
}

void Aircraft::checkPickupDrop(CommandQueue& commands)
{
	if (!isAllied() && randomInt(2) == 0 && !hasSpawnPickup) //and randomInt(3) == 0)

		commands.push(dropPickupCommand);

	hasSpawnPickup = true;

}

void Aircraft::createBulletes(SceneNode& node, const TextureHolder_t& textures) const
{
	Projectile::Type type = isAllied() ? Projectile::Type::AlliedBullet : Projectile::Type::EnemyBullet;

	switch (spreadLevel)
	{
	case 1:
		createProjectile(node, type, 0.0f, 0.5f, textures);
		break;

	case 2:
		createProjectile(node, type, -0.33f, 0.33f, textures);
		createProjectile(node, type, +0.33f, 0.33f, textures);
		break;

	case 3:
		createProjectile(node, type, -0.5f, 0.33f, textures);
		createProjectile(node, type, +0.0f, 0.5f, textures);
		createProjectile(node, type, +0.5f, 0.33f, textures);
		break;
	}
}

void Aircraft::createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder_t& textures) const
{
	std::unique_ptr<Projectile>projectile(new Projectile(type, textures));

	sf::Vector2f offset(xOffset * sprite.getGlobalBounds().width, yOffset * sprite.getGlobalBounds().width);
	sf::Vector2f velocity(0, projectile->getMaxSpeed());

	float sign = isAllied() ? -1.f : +1.f;
	projectile->setPosition(getWorldPosition() + offset * sign);
	projectile->setVelocity(velocity * sign);
	projectile->setRotation(getRotation() * sign);
	node.attachChild(std::move(projectile));

}

void Aircraft::createPickup(SceneNode& node, const TextureHolder_t& textures) const
{
	auto type = static_cast<Pickup::Type>(randomInt(static_cast<int>(Pickup::Type::Count)));

	std::unique_ptr<Pickup>pickup(new Pickup(type, textures));
	pickup->setPosition(getWorldPosition());
	pickup->setVelocity(0.f, 1.f);
	node.attachChild(std::move(pickup));

}


