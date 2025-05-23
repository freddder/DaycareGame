#include "cSceneManager.h"
#include "cWildRoamingPokemon.h"
#include "cTamedRoamingPokemon.h"
#include "cNPCCharacter.h"
#include "Player.h"

#include <time.h>

#include "Engine.h"
#include "cMapManager.h"
#include "cRenderManager.h"
#include "cInputManager.h"
#include "CanvasFactory.h"

cSceneManager::cSceneManager()
{
	currWeather = NONE;

	fogDensity = 0.f;
	fogGradient = 0.1f;
	windSpeed = 0.25f;
}

cSceneManager::~cSceneManager()
{
}

void cSceneManager::Shutdown()
{
	if (weatherParticleSpawner)
		delete weatherParticleSpawner;

	particleSpawners.clear();

	//roamingWildPokemon.clear();
}

void cSceneManager::SetWeather(eEnvironmentWeather newWeather)
{
	if (newWeather == currWeather) return;

	if ((currWeather == SNOW || currWeather == HAIL || currWeather == SNOWSTORM) // snow transition
		&& (newWeather == SNOW || newWeather == HAIL || newWeather == SNOWSTORM))
	{
		if (newWeather == SNOW)
		{

		}
		else if (newWeather == HAIL)
		{

		}
	}
	else if ((currWeather == RAIN || currWeather == HEAVYRAIN) // rain transition
		&& (newWeather == RAIN || newWeather == HEAVYRAIN))
	{



	}
	else // no transition
	{
		if (weatherParticleSpawner)
		{
			delete weatherParticleSpawner;
			weatherParticleSpawner = nullptr;
		}

		if (newWeather == NONE)
		{
			fogDensity = 0.f;
			fogGradient = 0.1f;
		}
		else if (newWeather == SNOW)
		{
			fogDensity = 0.057f;
			fogGradient = 0.57f;
			fogColor = glm::vec3(0.89f, 0.89f, 0.89f);

			cRenderModel prtcl;
			prtcl.meshName = "ParticleHolder.obj";
			prtcl.shaderName = "snow";
			prtcl.textureName = "SnowFlake3.png";
			prtcl.scale = glm::vec3(0.6f);

			weatherParticleSpawner = new cParticleSpawner(glm::vec3(0.f, 13.f, 0.f), prtcl, 500);
			weatherParticleSpawner->minPositionOffset = glm::vec3(-20.f, 0.f, -20.f);
			weatherParticleSpawner->maxPositionOffset = glm::vec3(20.f, 0.f, 20.f);
			weatherParticleSpawner->isPositionPlayerRelative = true;
			weatherParticleSpawner->spawnSpeed = glm::vec3(0.f, -3.f, 0.f);
			weatherParticleSpawner->spawnRate = 0.05f;
			weatherParticleSpawner->particleLifeTime = 10.f;
		}
		else if (newWeather == HAIL)
		{
			fogDensity = 0.022f;
			fogGradient = 0.48f;
			fogColor = glm::vec3(0.89f, 0.89f, 0.89f);


		}
		else if (newWeather == SNOWSTORM)
		{

		}
		else if (newWeather == RAIN)
		{

		}
		else if (newWeather == HEAVYRAIN)
		{

		}
		else if (newWeather == SANDSTORM)
		{

		}
		else if (newWeather == LEAVES)
		{

		}
	}

	currWeather = newWeather;
}

//void cSceneManager::LoadSpawnData(const int nationalDexId, const int minLevel, const int maxLevel, const Pokemon::eSpawnType spawnType, const int spawnChance, const std::string formName)
//{
//	for (int i = 0; i < loadedSpawnData.size(); i++)
//	{
//		if (loadedSpawnData[i].nationalDexNumber == nationalDexId && loadedSpawnData[i].formName == formName)
//		{
//			return; // Already loaded
//		}
//	}
//
//	Pokemon::sSpeciesData specieData;
//	Pokemon::LoadSpecieData(nationalDexId, specieData);
//
//	// Load overworld sprite texture
//	Manager::render.LoadRoamingPokemonSpecieTextures(specieData);
//
//	Pokemon::sSpawnData spawnData;
//	spawnData.nationalDexNumber = nationalDexId;
//	spawnData.formName = formName;
//	spawnData.minLevel = minLevel;
//	spawnData.maxLevel = maxLevel;
//	spawnData.spawnType = spawnType;
//	spawnData.spawnChance = spawnChance;
//	spawnData.genderRatio = specieData.genderRatio;
//	spawnData.isFormGenderBased = specieData.isFormGenderBased;
//	spawnData.isSpriteGenderBased = specieData.isSpriteGenderBased;
//
//	loadedSpawnData.push_back(spawnData);
//}

//std::shared_ptr<cWildRoamingPokemon> cSceneManager::SpawnRandomWildPokemon()
//{
//	if (loadedSpawnData.size() == 0) return nullptr;
//
//	// Pick a random spawn data
//	int randIndex = rand() % loadedSpawnData.size();
//	Pokemon::sSpawnData spawnData = loadedSpawnData[randIndex];
//	std::shared_ptr<cWildRoamingPokemon> spawnedWildPokemon = nullptr;
//
//	// Find a suitable tile
//	if (spawnData.spawnType == Pokemon::TALL_GRASS)
//	{
//		glm::vec3 spawnTilePos;
//		sTile* spawnTile = Manager::map.GetRandomSpawnTile(spawnTilePos);
//
//		if (spawnTile)
//			spawnedWildPokemon = SpawnWildPokemon(spawnData, spawnTilePos, spawnTile);
//	}
//	
//	return spawnedWildPokemon;
//}

//std::shared_ptr<cWildRoamingPokemon> cSceneManager::SpawnWildPokemon(const Pokemon::sSpawnData& spawnData, glm::vec3 tileLocation, sTile* spawnTile)
//{
//	if (!spawnTile) return nullptr;
//
//	Pokemon::sRoamingPokemonData roamingData = Pokemon::GenerateRoamingPokemonData(spawnData);
//
//	std::shared_ptr<cWildRoamingPokemon> newWildPokemon = std::make_shared<cWildRoamingPokemon>(roamingData, tileLocation);
//	roamingWildPokemon.push_back(newWildPokemon);
//
//	spawnTile->entity = newWildPokemon.get();
//
//	return newWildPokemon;
//}

//void cSceneManager::DespawnWildPokemon(cWildRoamingPokemon* pokemonToDespawn)
//{
//	for (int i = 0; roamingWildPokemon.size(); i++)
//	{
//		if (roamingWildPokemon[i].get() == pokemonToDespawn)
//		{
//			roamingWildPokemon.erase(roamingWildPokemon.begin() + i);
//			return;
//		}
//	}
//}

std::shared_ptr<cTamedRoamingPokemon> cSceneManager::SpawnTamedPokemon(Pokemon::sRoamingPokemonData& pokemonData, glm::vec3 tileLocation)
{
	sTile* spawnTile = Manager::map.GetTile(tileLocation);
	if (!spawnTile || spawnTile->entity != nullptr) return nullptr;

	std::shared_ptr<cTamedRoamingPokemon> newTamedPokemon = std::make_shared<cTamedRoamingPokemon>(pokemonData, tileLocation);
	roamingTamedPokemon.push_back(newTamedPokemon);

	spawnTile->entity = newTamedPokemon.get();

	return newTamedPokemon;
}

std::shared_ptr<cNPCCharacter> cSceneManager::SpawnNPC(std::string textureName, glm::vec3 tileLocation)
{
	sTile* spawnTile = Manager::map.GetTile(tileLocation);
	if (!spawnTile || spawnTile->entity != nullptr) return nullptr;

	std::shared_ptr<cNPCCharacter> newNPC = std::make_shared<cNPCCharacter>(textureName, tileLocation);
	npcs.push_back(newNPC);

	spawnTile->entity = newNPC.get();

	return newNPC;
}

void cSceneManager::ChangeScene(const std::string newSceneDescFile, const int entranceNumUsed)
{
	// Things this should do (not ordered):
	// - unload current map model
	// - unload map textures
	// - unload map data
	// - despawn entities
	// - unload despawn data
	// - move player and follower to appropriate place
	// - remove render objects from vector

	// TODO: despawn all entities and remove all particle spawners

	Manager::map.UnloadMap();

	Manager::render.UnloadModels();
	Manager::render.UnloadTextures();

	//loadedSpawnData.clear();
	//roamingWildPokemon.clear();
	roamingTamedPokemon.clear();

	Manager::map.LoadMap(newSceneDescFile, entranceNumUsed);

	// TEMP
	Pokemon::sSpeciesData followerSpecieData;
	Pokemon::LoadSpecieData(Player::party[0].nationalDexNumber, followerSpecieData);
	Manager::render.LoadRoamingPokemonSpecieTextures(followerSpecieData);

	//// TEMP
	//// TODO: find a good place to seed the rand
	//srand((int)time(0));
	//Manager::scene.SpawnRandomWildPokemon();
	//Manager::scene.SpawnRandomWildPokemon();
	//Manager::scene.SpawnRandomWildPokemon();
	//Manager::scene.SpawnRandomWildPokemon();
	//Manager::scene.SpawnRandomWildPokemon();
}

//void cSceneManager::EnterWildEncounter(const Pokemon::sRoamingPokemonData& roamingPokemonData, cWildRoamingPokemon* roamingEntity)
//{
//	enemyBattleData = Pokemon::GeneratePokemonBattleData(roamingPokemonData);
//	std::string enemyTextureName = enemyBattleData.MakeBattleTextureName();
//	float enemySpriteAspectRatio = Manager::render.LoadPokemonBattleSpriteSheet(enemyBattleData);
//
//	Pokemon::sBattleData playerBattleData = (Pokemon::sBattleData&)Player::party[0];
//	std::string playerTextureName = playerBattleData.MakeBattleTextureName(false);
//	float playerSpriteAspectRatio = Manager::render.LoadPokemonBattleSpriteSheet(playerBattleData, false);
//
//	Manager::map.opponentSpriteModel->SetSpriteData(enemyTextureName, enemyBattleData.form.battleSpriteHeightSize, enemySpriteAspectRatio, enemyBattleData.form.battleFrontSpriteFrameCount);
//	Manager::map.playerSpriteModel->SetSpriteData(playerTextureName, playerBattleData.form.battleSpriteHeightSize, playerSpriteAspectRatio, playerBattleData.form.battleBackSpriteFrameCount);
//
//	Manager::ui.AddCanvas(new cBattleCanvas());
//	DespawnWildPokemon(roamingEntity);
//
//	Engine::currGameMode = eGameMode::BATTLE;
//	Manager::input.ChangeInputState(MENU_NAVIGATION);
//}

//void cSceneManager::CatchWildPokemon()
//{
//	if (Engine::currGameMode != eGameMode::BATTLE) return;
//
//	if (enemyBattleData.nationalDexNumber == 0) return;
//
//	Player::AddPartyMember(enemyBattleData);
//	ExitEncounter();
//}

//void cSceneManager::ExitEncounter()
//{
//	if (Engine::currGameMode != eGameMode::BATTLE) return;
//
//	Engine::currGameMode = eGameMode::MAP;
//	Manager::input.ChangeInputState(OVERWORLD_MOVEMENT);
//
//	Manager::ui.RemoveCanvas();
//
//	Manager::map.opponentSpriteModel->ClearSpriteData();
//	Manager::map.playerSpriteModel->ClearSpriteData();
//
//	enemyBattleData = Pokemon::sBattleData();
//}

std::shared_ptr<cParticleSpawner> cSceneManager::CreateParticleSpawner(glm::vec3 position, cRenderModel model, unsigned int maxParticles)
{
	std::shared_ptr<cParticleSpawner> newSpawner = std::make_shared<cParticleSpawner>(position, model, maxParticles);
	particleSpawners.push_back(newSpawner);

	return newSpawner;
}

void cSceneManager::Process(float deltaTime)
{
	if (weatherParticleSpawner)
	{
		weatherParticleSpawner->Update(deltaTime);
	}

	for (int i = 0; i < particleSpawners.size(); i++)
	{
		particleSpawners[i]->Update(deltaTime);
	}
}
