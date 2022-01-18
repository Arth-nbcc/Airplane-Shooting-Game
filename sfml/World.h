#pragma once
#include "ResourceHolder.h"
#include "ResourceIdentifier.h"
#include "SceneNode.h"
#include "SpriteNode.h"
#include "Aircraft.h"
#include "CommandQueue.h"
#include "Command.h"
#include "BloomEffect.h"

#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <array>

namespace sf {
	class RenderTarget;
}

class World
{
public:
	World(const World&) = delete;

	explicit						  World(sf::RenderTarget& outputTarget, FontHolder_t& fonts);

	void							  update(sf::Time dt);
	void							  draw();

	CommandQueue& getCommandQueue();

	bool							  hasAlivePlayer() const;
	bool							  hasPlayerReachedEnd() const;

private:

	void							  loadTextures();
	void							  buildScene();


	void							  adaptPlayerPosition();
	void							  adaptPlayerVelocity();

	void							  spawnEnemies();
	void							  addEnemies();
	void							  addEnemy(Aircraft::Type type, float relX, float relY);

	sf::FloatRect					  getBattlefiend() const;
	sf::FloatRect					  getViewBounds() const;

	void							  guideMissiles();
	void							  handleCollisions();
	void							  destroyEntitiesOutOfView();


private:

	enum Layer
	{
		Background = 0,
		LowerAir,
		UpperAir,
		LayerCount
	};

	struct SpawnPoint
	{
		SpawnPoint(Aircraft::Type type, float x, float y) : type(type), x(x), y(y) {}

		Aircraft::Type			type;
		float					x;
		float					y;
	};

private:
	sf::RenderTarget& target;
	sf::RenderTexture				   sceneTexture;

	sf::View						   worldView;
	TextureHolder_t					   textures;
	FontHolder_t& fonts;
	SceneNode						   sceneGraph;
	std::array<SceneNode*, LayerCount> sceneLayers;
	CommandQueue					   commands;

	sf::FloatRect					   worldBounds;

	sf::Vector2f					   spawnPosition;
	float							   scrollSpeed;
	Aircraft* playerAircraft;

	std::vector<SpawnPoint>			   enemySpawnPoints;
	std::vector<Aircraft*>			   activeEnemies;

	BloomEffect						   bloomEffect;
};

