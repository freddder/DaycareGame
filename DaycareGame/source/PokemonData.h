#pragma once
#include <string>
#include <map>
#include <vector>
//#include <array>

// TODO: see if this is better
//typedef std::array<char, 12> Name;

//Name to_name(const std::string& string);
//std::string to_string(const Name& name);

namespace Pokemon
{
	const static unsigned int BASE_SHINY_ODDS = 8192; // 2^13

	enum eType
	{
		NORMAL = 0,
		FIGHTING,
		FLYING,
		POISON,
		GROUND,
		ROCK,
		BUG,
		GHOST,
		STEEL,
		FIRE,
		WATER,
		GRASS,
		ELECTRIC,
		PSYCHIC,
		ICE,
		DRAGON,
		DARK,
		FAIRY,
		NO_TYPE,
		TYPE_ENUM_COUNT
	};

	const static char* Type_Strings[] =
	{
		"Normal",
		"Fighting",
		"Flying",
		"Poison",
		"Ground",
		"Rock",
		"Bug",
		"Ghost",
		"Steel",
		"Fire",
		"Water",
		"Grass",
		"Electric",
		"Psychic",
		"Ice",
		"Dragon",
		"Dark",
		"Fairy",
		"No Type"
	};

	enum eEggGroup
	{
		EGG_MONSTER,
		EGG_WATER_1,
		EGG_BUG,
		EGG_FLYING,
		EGG_FIELD,
		EGG_FAIRY,
		EGG_GRASS,
		EGG_HUMAN_LIKE,
		EGG_WATER_3,
		EGG_MINERAL,
		EGG_AMORPHOUS,
		EGG_WATER_2,
		EGG_DITTO,
		EGG_DRAGON,
		EGG_NO_EGGS_DISCOVERED,
		EGG_NO_EGG_GROUP,
		EGG_ENUM_COUNT
	};

	const static char* EggGroup_Strings[] =
	{
		"Monster",
		"Water 1",
		"Bug",
		"Flying",
		"Field",
		"Fairy",
		"Grass",
		"Human Like",
		"Water 3",
		"Mineral",
		"Amorphous",
		"Water 2",
		"Ditto",
		"Dragon",
		"No Eggs Discovered",
		"No Egg Group"
	};

	enum eGender
	{
		MALE,
		FEMALE,
		NO_GENDER
	};

	enum eNature
	{
		HARDY,		// +Atk   -Atk
		LONELY,		// +Atk   -Def
		ADAMANT,	// +Atk   -SpAtk
		NAUGHTY,	// +Atk   -SpDef
		BRAVE,		// +Atk   -Spd
		BOLD,		// +Def   -Atk
		DOCILE,		// +Def   -Def
		IMPISH,		// +Def   -SpAtk
		LAX,		// +Def   -SpDef
		RELAXED,	// +Def   -Spd
		MODEST,		// +SpAtk -Atk
		MILD,		// +SpAtk -Def
		BASHFUL,	// +SpAtk -SpAtk
		RASH,		// +SpAtk -SpDef
		QUIET,		// +SpAtk -Spd
		CALM,		// +SpDef -Atk
		GENTLE,		// +SpDef -Def
		CAREFUL,	// +SpDef -SpAtk
		QUIRKY,		// +SpDef -SpDef
		SASSY,		// +SpDef -Spd
		TIMID,		// +Spd	  -Atk
		HASTY,		// +Spd	  -Def
		JOLLY,		// +Spd	  -SpAtk
		NAIVE,		// +Spd	  -SpDef
		SERIOUS		// +Spd	  -Spd
	};

	struct sStats
	{
		sStats() : 
			hp{ 0 },
			atk{ 0 },
			spAtk{ 0 },
			def{ 0 },
			spDef{ 0 },
			spd{ 0 }
		{}

		unsigned int hp		: 8;	// Health
		unsigned int atk	: 8;	// Attack
		unsigned int spAtk	: 8;	// Special Attack
		unsigned int def	: 8;	// Defence
		unsigned int spDef	: 8;	// Special Defence
		unsigned int spd	: 8;	// Speed

		int Total()
		{
			return hp + atk + spAtk + def + spDef + spd;
		}
	};

	static bool IsNationalDexNumberValid(int nationalDexNumber)
	{
		return nationalDexNumber > 0 && nationalDexNumber <= 1008;
	}

	enum eMoveClass
	{
		PHYSICAL,
		SPECIAL,
		STATUS
	};

	//enum eMoveTargetType
	//{
	//	a lot more than I thought... luckly not necessary for this game
	//};

	struct sMove
	{
		unsigned int id = 0;
		eType type = NO_TYPE;
		eMoveClass damageClass = PHYSICAL;

		unsigned int power = 0;
		unsigned int pp = 0; // Power points
		unsigned int accuracy = 100;
		//int priority = 0;
		//eMoveTargetType targetType;
	};

	struct sForm
	{
		sStats baseStats;

		eType type1 = NORMAL;
		eType type2 = NO_TYPE;
		
		unsigned int ability1 = 0;
		unsigned int ability2 = 0;
		unsigned int hiddenAbility = 0;

		float height = 0.f; // in meters
		float weight = 0.f; // in kilograms

		float battleSpriteHeightSize = 1.f;
		int battleFrontSpriteFrameCount = 1;
		int battleBackSpriteFrameCount = 1;

		// Learnset as a pair of int (level) and unsigned int (move id)
		std::vector<std::pair<int, unsigned int>> learnset;
	};

	struct sSpeciesData
	{
		std::string name;
		int nationalDexNumber = 0;

		int genderRatio = 50; // chance to be male (0 to 100 and <0 for genderless) TODO: change to eigths
		eEggGroup eggGroup1 = EGG_NO_EGGS_DISCOVERED;
		eEggGroup eggGroup2 = EGG_NO_EGG_GROUP;
		int hatchCycles = 0; // Not sure what this means, but its related to the amount of steps to hatch

		float catchRate; // Not sure how this will work either

		bool isFormGenderBased = false; // Use an alternate form if female. Will only be used a few times (ex: Meowstic, Indeedee)
		bool isSpriteGenderBased = false; // Change sprite if its female (doesn't matter if isStatsGenderBased is true)

		sForm defaultForm;
		std::map<std::string, sForm> alternateForms;
	};

	const static unsigned int JSON_DATA_VERSION = 1;
	std::string MakeDexNumberFolderName(const int nationalDexNumber);
	void SaveSpecieData(const int nationalDexNumber, const sSpeciesData& data);
	void LoadSpecieData(const int nationalDexNumber, sSpeciesData& data);
	void LoadFormData(const int nationalDexNumber, sForm& form, const std::string& formName = "");

	//enum eSpawnType
	//{
	//	TALL_GRASS,
	//	ST_WATER,
	//	CAVE,
	//	ST_ENUM_COUNT
	//};

	//struct sSpawnData
	//{
	//	int nationalDexNumber = 0;
	//	std::string formName = "";

	//	// These should be the same as the species data
	//	int genderRatio = 50;
	//	bool isFormGenderBased = false; // Use an alternate form if female. Will only be used a few times (ex: Meowstic, Indeedee)
	//	bool isSpriteGenderBased = false; // Change sprite if its female (doesn't matter if isStatsGenderBased is true)

	//	eSpawnType spawnType = TALL_GRASS;
	//	int spawnChance = 0; // Number of "entries"

	//	int minLevel;
	//	int maxLevel;
	//};

	struct sRoamingPokemonData // Minimal data for rendering sprite in overworld
	{
		int nationalDexNumber = 0;
		std::string formName;

		int level = 0;
		eGender gender = NO_GENDER;
		bool isShiny = false;

		bool isFormGenderBased = false;
		bool isSpriteGenderBased = false;

		const std::string MakeRoamingTextureName();
	};
	//sRoamingPokemonData GenerateRoamingPokemonData(const sSpawnData& spawnData);

	struct sIndividualData : public sRoamingPokemonData // Individual data (outside of battle)
	{
		unsigned int maxHealth = 0;
		unsigned int currHealth = 0;

		unsigned int expToNextLevel = 0;
		unsigned int currExp = 0;

		unsigned int abilityId = 0;
		eNature nature;

		unsigned int move1 :10;
		unsigned int move2 :10;
		unsigned int move3 :10;
		unsigned int move4 :10;

		sStats IVs; // Individial values
		sStats EVs; // Effort values

		unsigned int item = 0;
		std::string nickname = "";

		const std::string MakeIconTextureName();
		const std::string MakeBattleTextureName(bool isFront = true);
		//void LoadFormFromSpeciesData();
	};
	sIndividualData GenerateIndividualPokemonData(int nationalDexId);

	//struct sBattleData : public sIndividualData // Individual data (in battle)
	//{
	//	sStats statChanges;
	//};
	//sBattleData GeneratePokemonBattleData(const sRoamingPokemonData& roamingData); // For wild encounters

}