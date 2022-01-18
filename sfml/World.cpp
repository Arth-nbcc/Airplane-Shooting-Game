#include "World.h"
#include "Utility.h"
#include "Pickup.h"
#include "Projectile.h"
#include <algorithm>
#include "ParticleNode.h"

#include <SFML/Graphics/RenderWindow.hpp>

World::World(sf::RenderTarget& outputTarget, FontHolder_t& fonts)
	: target(outputTarget)
	, worldView(outputTarget.getDefaultView())
	, textures()
	, fonts(fonts)
	, sceneGraph()
	, sceneLayers()
	, commands()
	, worldBounds(0.f, 0.f, worldView.getSize().x, 4000.f)
	, spawnPosition(worldView.getSize().x / 2.f
		, worldBounds.height - worldView.getSize().y / 2.f)
	, scrollSpeed(-50.f)
	, playerAircraft(nullptr)
{
	sceneTexture.create(target.getSize().x, target.getSize().y);
	loadTextures();
	buildScene();

	worldView.setCenter(spawnPosition);
}

void World::update(sf::Time dt)
{
	playerAircraft->setVelocity(0.f, 0.f);

	//scroll the world
	worldView.move(0.f, scrollSpeed * dt.asSeconds());

	destroyEntitiesOutOfView();
	guideMissiles();

	//apply all the commands
	while (!commands.isEmpty())
	{
		sceneGraph.onCommand(commands.pop(), dt);
	}

	adaptPlayerVelocity();

	//create new enimies as spawnPoints come into range
	spawnEnemies();

	handleCollisions();

	sceneGraph.removeWrecks();

	sceneGraph.update(dt, commands);
	adaptPlayerPosition();
}

void World::draw()
{
	if (PostEffect::isSupported())
	{
		sceneTexture.clear();
		sceneTexture.setView(worldView);
		sceneTexture.draw(sceneGraph);
		sceneTexture.display();
		bloomEffect.apply(sceneTexture, target);
	}
	else
	{
		target.setView(worldView);
		target.draw(sceneGraph);
	}
}

CommandQueue& World::getCommandQueue()
{
	return commands;
}

bool World::hasAlivePlayer() const
{
	return !playerAircraft->isDestroyed();
}

bool World::hasPlayerReachedEnd() const
{
	return !worldBounds.contains(playerAircraft->getPosition());
}

void World::loadTextures()
{
	textures.load(TextureID::Entities, "Media/Textures/Entities.png");

	textures.load(TextureID::Jungle, "Media/Textures/Jungle.png");
	textures.load(TextureID::FinishLine, "Media/Textures/FinishLine.png");
	textures.load(TextureID::Particle, "Media/Textures/Particle.png");
	textures.load(TextureID::Explosion, "Media/Textures/Explosion.png");
}

void World::buildScene()
{
	//set up layer nodes
	for (std::size_t i = 0; i < LayerCount; ++i) {

		Category::type category = (i == LowerAir) ? Category::SceneAirLayer : Category::None;

		SceneNode::Ptr layer(new SceneNode(category));
		sceneLayers[i] = layer.get();
		sceneGraph.attachChild(std::move(layer));
	}

	//prepare tiled background
	sf::Texture& texture = textures.get(TextureID::Jungle);
	texture.setRepeated(true);

	//background
	sf::IntRect textureRect(worldBounds);
	auto backgroundSprite = std::make_unique<SpriteNode>(texture, textureRect);
	backgroundSprite->setPosition(worldBounds.left, worldBounds.top);
	sceneLayers[Background]->attachChild(std::move(backgroundSprite));

	// Add the finish line to the scene
	sf::Texture& finishTexture = textures.get(TextureID::FinishLine);
	std::unique_ptr<SpriteNode> finishSprite(new SpriteNode(finishTexture));
	finishSprite->setPosition(0.f, -76.f);
	sceneLayers[Background]->attachChild(std::move(finishSprite));

	//add smoke particle node to the scene
	std::unique_ptr<ParticleNode>smokeNode(new ParticleNode(Particle::Type::Smoke, textures));
	sceneLayers[LowerAir]->attachChild(std::move(smokeNode));

	//add propellant particle node to the scene
	std::unique_ptr<ParticleNode>propellantNode(new ParticleNode(Particle::Type::Propellant, textures));
	sceneLayers[LowerAir]->attachChild(std::move(propellantNode));

	//add players aircraft
	auto leader = std::make_unique<Aircraft>(Aircraft::Type::Eagle, textures, fonts);
	playerAircraft = leader.get();
	playerAircraft->setPosition(spawnPosition);
	playerAircraft->setVelocity(100.f, scrollSpeed);
	sceneLayers[UpperAir]->attachChild(std::move(leader));

	//add enimies to spawn
	addEnemies();
}

void World::adaptPlayerPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds(worldView.getCenter() - worldView.getSize() / 2.f, worldView.getSize());
	const float borderDistance = 40.f;

	sf::Vector2f position = playerAircraft->getPosition();
	position.x = std::max(position.x, viewBounds.left + borderDistance);
	position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
	position.y = std::max(position.y, viewBounds.top + borderDistance);
	position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
	playerAircraft->setPosition(position);
}

void World::adaptPlayerVelocity()
{
	sf::Vector2f velocity = playerAircraft->getVelocity();

	if (velocity.x != 0.f && velocity.y != 0.f)
		playerAircraft->setVelocity(velocity / std::sqrt(2.f));


	playerAircraft->accelerate(0.f, scrollSpeed);

}

void World::spawnEnemies()
{
	while (!enemySpawnPoints.empty() && enemySpawnPoints.back().y > getBattlefiend().top)
	{
		auto spawn = enemySpawnPoints.back();
		std::unique_ptr<Aircraft> enemy(new Aircraft(spawn.type, textures, fonts));
		enemy->setPosition(spawn.x, spawn.y);
		enemy->setRotation(180.f);
		sceneLayers[UpperAir]->attachChild(std::move(enemy));
		enemySpawnPoints.pop_back();
	}
}

void World::addEnemies()
{
	// spawn position x,y is relative to original player spawn pint
	/*addEnemy(Aircraft::Type::Raptor, -256.f, 200.f);
	addEnemy(Aircraft::Type::Raptor, 0.f, 200.f);
	addEnemy(Aircraft::Type::Raptor, 256.f, 200.f);

	addEnemy(Aircraft::Type::Avenger, -70.f, 600.f);
	addEnemy(Aircraft::Type::Avenger, 70.f, 600.f);

	addEnemy(Aircraft::Type::Raptor, -256.f, 700.f);
	addEnemy(Aircraft::Type::Raptor, 0.f, 700.f);
	addEnemy(Aircraft::Type::Raptor, 256.f, 700.f);

	addEnemy(Aircraft::Type::Avenger, -70.f, 900.f);
	addEnemy(Aircraft::Type::Avenger, 70.f, 900.f);*/

	for (float i = 200.f; i < 4000.f; i += 200.f)
	{
		auto type = (randomInt(2) == 0) ? Aircraft::Type::Avenger : Aircraft::Type::Raptor;
		addEnemy(type, -200.f, i);
		addEnemy(type, 0.f, i);
		addEnemy(type, 200.f, i);
	}

	std::sort(enemySpawnPoints.begin(), enemySpawnPoints.end(),
		[](SpawnPoint lhs, SpawnPoint rhs)
		{
			return lhs.y < rhs.y;
		});
}

void World::addEnemy(Aircraft::Type type, float relX, float relY)
{
	SpawnPoint spawn(type, spawnPosition.x + relX, spawnPosition.y - relY);
	enemySpawnPoints.push_back(spawn);
}

sf::FloatRect World::getBattlefiend() const
{
	return sf::FloatRect(worldView.getCenter() - worldView.getSize() / 2.f, worldView.getSize());
}

sf::FloatRect World::getViewBounds() const
{
	sf::FloatRect bounds = getViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;

	return bounds;
};

void World::guideMissiles()
{
	//build a list of active enemies
	Command enemyCollector;
	enemyCollector.category = Category::EnemyAircraft;
	enemyCollector.action = derivedAction<Aircraft>([this](Aircraft& enemy, sf::Time dt)
		{
			if (!enemy.isDestroyed())
				activeEnemies.push_back(&enemy);
		});

	Command missileGuider;
	missileGuider.category = Category::type::AlliedProjectile;
	missileGuider.action = derivedAction<Projectile>([this](Projectile& missile, sf::Time dt)
		{
			//ignore bullets
			if (!missile.isGuided())
				return;

			float minDistance = std::numeric_limits<float>::max();
			Aircraft* closestEnemy = nullptr;

			for (Aircraft* e : activeEnemies)
			{
				auto d = distance(missile, *e);
				if (d < minDistance)
				{
					minDistance = d;
					closestEnemy = e;
				}
			}
			if (closestEnemy)
				missile.guideTowards(closestEnemy->getWorldPosition());
		});

	commands.push(enemyCollector);
	commands.push(missileGuider);

	activeEnemies.clear();
}

bool matchesCategories(SceneNode::Pair& colliders, Category::type type1, Category::type type2)
{
	unsigned int category1 = colliders.first->getCategory();
	unsigned int category2 = colliders.second->getCategory();

	//make sure first pair has category type1 and second has type2
	if (type1 & category1 && type2 & category2)
	{
		return true;
	}
	else if (type1 & category2 && type2 & category1)
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}

void World::handleCollisions()
{
	std::set<SceneNode::Pair> collisionPairs;
	sceneGraph.checkSceneCollision(sceneGraph, collisionPairs);

	for (auto pair : collisionPairs)
	{
		if (matchesCategories(pair, Category::type::PlayerAircraft, Category::type::EnemyAircraft))
		{
			auto& player = static_cast<Aircraft&>(*(pair.first));
			auto& enemy = static_cast<Aircraft&>(*(pair.second));

			player.damage(enemy.getHitpoints());
			enemy.destroy();
		}
		else if (matchesCategories(pair, Category::type::PlayerAircraft, Category::type::Pickup))
		{
			auto& player = static_cast<Aircraft&>(*(pair.first));
			auto& pickup = static_cast<Pickup&>(*(pair.second));

			pickup.apply(player);
			pickup.destroy();
		}
		else if (matchesCategories(pair, Category::type::PlayerAircraft, Category::type::EnemyProjectile) ||
			matchesCategories(pair, Category::type::EnemyAircraft, Category::type::AlliedProjectile))
		{
			auto& aircraft = static_cast<Aircraft&>(*(pair.first));
			auto& projectile = static_cast<Projectile&>(*(pair.second));

			aircraft.damage(projectile.getDamage());
			projectile.destroy();
		}
	}
}

void World::destroyEntitiesOutOfView()
{
	Command command;
	command.category = Category::SpaceJunk;

	command.action = derivedAction<Entity>([this](Entity& e, sf::Time dt)
		{
			if (!getBattlefiend().intersects(e.getBoundingRect()))
				e.remove();
		});

	commands.push(command);
}
