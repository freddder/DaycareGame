#pragma once
#include <string>
#include <vector>

typedef char Name[12];

bool CompareName(const Name& name, const std::string& str);

namespace Pokemon
{
	const static unsigned int BASE_SHINY_ODDS = 8192; // 2^13

	enum eType
	{
		NO_TYPE,
		NORMAL,
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
		TYPE_ENUM_COUNT
	};

	const static char* Type_Strings[] =
	{
		"No Type",
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
	};

	enum eEggGroup
	{
		EGG_NO_EGG_GROUP,
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
		EGG_ENUM_COUNT
	};

	const static char* EggGroup_Strings[] =
	{
		"No Egg Group",
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
	};

	enum eGender
	{
		NO_GENDER,
		FEMALE,
		MALE,
		GENDER_ENUM_COUNT
	};

	const static char* Gender_Strings[] =
	{
		"No Gender",
		"Female",
		"Male",
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
		SERIOUS,	// +Spd	  -Spd
		NATURE_COUNT
	};

	const static char* Natures_Strings[] =
	{
		"Hardy",
		"Lonely",
		"Adamant",
		"Naughty",
		"Brave",
		"Bold",
		"Docile",
		"Impish",
		"Lax",
		"Relaxed",
		"Modest",
		"Mild",
		"Bashful",
		"Rash",
		"Quiet",
		"Calm",
		"Gentle",
		"Careful",
		"Quirky",
		"Sassy",
		"Timid",
		"Hasty",
		"Jolly",
		"Naive",
		"Serious"
	};

	enum eDayTime
	{
		NO_DAY_TIME,
		DAY,
		DUSK,
		NIGHT,
		DAY_TIME_ENUM_COUNT
	};

	const static char* DayTyme_Strings[] =
	{
		"No Day Time",
		"Day",
		"Dusk",
		"Night"
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

	struct sEvolution
	{
		unsigned int dexId = 0;
		int minLevel = 0;
		bool friendship = false;
		eDayTime dayTime = NO_DAY_TIME;
		int heldItem = 0;
		int useItem = 0;
		unsigned int knownMoveId = 0;
		eGender gender = NO_GENDER;
	};

	struct sForm
	{
		Name name = "";

		sStats baseStats;

		eType type1 = NORMAL;
		eType type2 = NO_TYPE;
		
		unsigned int ability1 = 0;
		unsigned int ability2 = 0;
		unsigned int hiddenAbility = 0;

		int height = 0; // in decimeters (/10 for meters)
		int weight = 0; // in hectograms (/10 for kilograms)

		unsigned int frontSpriteFrameCount = 1;
		unsigned int backSpriteFrameCount = 1;

		// Learnset as a pair of int (level) and unsigned int (move id)
		// -3 = when evolving
		// -2 = tutor
		// -1 = egg move
		//  0 = TM
		std::vector<std::pair<int, unsigned int>> learnset;
	};

	struct sSpeciesData
	{
		Name name = "";
		unsigned int nationalDexNumber = 0;
		unsigned int childSpecies = 0;

		int genderRatio = 4; // in eigths (8 = only female; 0 = only male; -1 = genderless)
		eEggGroup eggGroup1 = EGG_NO_EGGS_DISCOVERED;
		eEggGroup eggGroup2 = EGG_NO_EGG_GROUP;
		int hatchCycles = 0; // Not sure what this means, but its related to the amount of steps to hatch

		int catchRate; // Not sure how this will work either

		bool isFormGenderBased = false; // Use an alternate form if female. Will only be used a few times (ex: Meowstic, Indeedee)
		bool isSpriteGenderBased = false; // Change sprite if its female (doesn't matter if isStatsGenderBased is true)
		bool isFormSwitchable = false;
		bool isBornFormRandom = false;

		std::vector<sEvolution> evolutions;

		sForm defaultForm;
		std::vector<sForm> alternateForms;
	};

	const static unsigned int JSON_DATA_VERSION = 4;
	std::string MakeDexNumberFolderName(const int nationalDexNumber);
	void SaveSpecieData(const int nationalDexNumber, const sSpeciesData& data);
	void LoadSpecieData(const int nationalDexNumber, sSpeciesData& data);
	void LoadFormData(const int nationalDexNumber, sForm& form, const Name& formName);
	void GetAllIdsFromEggGroup(eEggGroup group, std::vector<unsigned int>& output);

	struct sRoamingPokemonData // Minimal data for rendering sprite in overworld
	{
		unsigned int nationalDexNumber = 0;
		Name formName = "";

		int level = 0;
		eGender gender = NO_GENDER;
		bool isShiny = false;

		bool isFormGenderBased = false;
		bool isSpriteGenderBased = false;

		std::string MakeRoamingTextureName() const;
		std::string MakeIconTextureName() const;
	};

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
		Name nickname = "";

		std::string MakeBattleTextureName(bool isFront = true) const;
		//void LoadFormFromSpeciesData();
		bool HasMove(const unsigned int moveId) const;
		void LearnEggMove(const unsigned int moveId);

	};
	sIndividualData GenerateIndividualPokemonData(int nationalDexId);
	bool GenerateChild(const sIndividualData& parent1, const sIndividualData& parent2, sIndividualData& child);
}