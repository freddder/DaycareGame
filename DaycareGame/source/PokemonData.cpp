#include "PokemonData.h"

#include <fstream>

#include <rapidjson/filereadstream.h>
#include "rapidjson/filewritestream.h"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <iostream>

const std::string PKM_DATA_PATH = "assets/pokemon/";

bool CompareName(const Name& name, const std::string& str)
{
	for (unsigned int i = 0; i < str.length(); i++)
	{
		if (name[i] != str[i])
			return false;

		if (name[i] == '\0' && str[i] == '\0')
			return true;
	}

	return true;
}

namespace Pokemon
{
	bool OpenPokemonDataFile(rapidjson::Document& doc, const int nationalDexNumber);
	void LoadFormData(rapidjson::Value& formObject, sForm& form);

	std::string MakeDexNumberFolderName(const int nationalDexNumber)
	{
		std::string dexNumberString = std::to_string(nationalDexNumber);
		while (dexNumberString.length() < 4)
		{
			dexNumberString = "0" + dexNumberString;
		}

		return dexNumberString;
	}

	void SaveSpecieData(const int nationalDexNumber, const sSpeciesData& data)
	{
		if (!IsNationalDexNumberValid(nationalDexNumber)) return;
	
		std::string dexNumberString = MakeDexNumberFolderName(nationalDexNumber);
	
		FILE* fp = 0;
		fopen_s(&fp, (PKM_DATA_PATH + "species/" + dexNumberString + "/" + dexNumberString + ".json").c_str(), "wb");
		if (fp == 0)
		{
			std::cout << "Failed to create file" << std::endl;
			return;
		}
	
		char writeBuffer[65536];
		rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
	
		rapidjson::Document d;
		rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
	
		d.SetObject();

		d.AddMember("data_version", JSON_DATA_VERSION, allocator);
		rapidjson::Value name(data.name, allocator);
		d.AddMember("name", name, allocator);
		d.AddMember("national_dex_number", data.nationalDexNumber, allocator);
		d.AddMember("child_species", data.childSpecies, allocator);

		d.AddMember("gender_ratio", data.genderRatio, allocator);
		d.AddMember("egg_group_1", data.eggGroup1, allocator);
		d.AddMember("egg_group_2", data.eggGroup2, allocator);
		d.AddMember("hatch_cycles", data.hatchCycles, allocator);

		d.AddMember("catch_rate", data.catchRate, allocator);

		d.AddMember("is_sprite_gender_based", data.isSpriteGenderBased, allocator);
		d.AddMember("is_stats_gender_based", data.isFormGenderBased, allocator);

		rapidjson::Value defaultForm(rapidjson::kObjectType);

		rapidjson::Value formName(data.defaultForm.name, allocator);
		defaultForm.AddMember("name", formName, allocator);

		defaultForm.AddMember("type_1", data.defaultForm.type1, allocator);
		defaultForm.AddMember("type_2", data.defaultForm.type2, allocator);

		defaultForm.AddMember("ability_1", data.defaultForm.ability1, allocator);
		defaultForm.AddMember("ability_2", data.defaultForm.ability2, allocator);
		defaultForm.AddMember("hidden_ability", data.defaultForm.hiddenAbility, allocator);

		defaultForm.AddMember("base_hp", data.defaultForm.baseStats.hp, allocator);
		defaultForm.AddMember("base_atk", data.defaultForm.baseStats.atk, allocator);
		defaultForm.AddMember("base_spAtk", data.defaultForm.baseStats.spAtk, allocator);
		defaultForm.AddMember("base_def", data.defaultForm.baseStats.def, allocator);
		defaultForm.AddMember("base_spDef", data.defaultForm.baseStats.spDef, allocator);
		defaultForm.AddMember("base_spd", data.defaultForm.baseStats.spd, allocator);

		defaultForm.AddMember("height", data.defaultForm.height, allocator);
		defaultForm.AddMember("weight", data.defaultForm.weight, allocator);

		rapidjson::Value defaultLearnset(rapidjson::kArrayType);
		for (unsigned int i = 0; i < data.defaultForm.learnset.size(); i++)
		{
			defaultLearnset.PushBack(data.defaultForm.learnset[i].first, allocator);
			defaultLearnset.PushBack(data.defaultForm.learnset[i].second, allocator);
		}
		defaultForm.AddMember("learnset", defaultLearnset, allocator);

		d.AddMember("default_form", defaultForm, allocator);

		rapidjson::Value alternateForms(rapidjson::kArrayType);
		for (unsigned int i = 0; i < data.alternateForms.size(); i++)
		{
			rapidjson::Value newAlternateForm(rapidjson::kObjectType);
			rapidjson::Value alternateFormName(data.alternateForms[i].name, allocator);
			newAlternateForm.AddMember("name", alternateFormName, allocator);

			newAlternateForm.AddMember("type_1", data.alternateForms[i].type1, allocator);
			newAlternateForm.AddMember("type_2", data.alternateForms[i].type2, allocator);

			newAlternateForm.AddMember("ability_1", data.alternateForms[i].ability1, allocator);
			newAlternateForm.AddMember("ability_2", data.alternateForms[i].ability2, allocator);
			newAlternateForm.AddMember("hidden_ability", data.alternateForms[i].hiddenAbility, allocator);

			newAlternateForm.AddMember("base_hp", data.alternateForms[i].baseStats.hp, allocator);
			newAlternateForm.AddMember("base_atk", data.alternateForms[i].baseStats.atk, allocator);
			newAlternateForm.AddMember("base_spAtk", data.alternateForms[i].baseStats.spAtk, allocator);
			newAlternateForm.AddMember("base_def", data.alternateForms[i].baseStats.def, allocator);
			newAlternateForm.AddMember("base_spDef", data.alternateForms[i].baseStats.spDef, allocator);
			newAlternateForm.AddMember("base_spd", data.alternateForms[i].baseStats.spd, allocator);

			newAlternateForm.AddMember("height", data.alternateForms[i].height, allocator);
			newAlternateForm.AddMember("weight", data.alternateForms[i].weight, allocator);

			rapidjson::Value alternateLearnset(rapidjson::kArrayType);
			for (unsigned int j = 0; j < data.alternateForms[i].learnset.size(); j++)
			{
				alternateLearnset.PushBack(data.alternateForms[i].learnset[j].first, allocator);
				alternateLearnset.PushBack(data.alternateForms[i].learnset[j].second, allocator);
			}
			newAlternateForm.AddMember("learnset", alternateLearnset, allocator);

			alternateForms.PushBack(newAlternateForm, allocator);
		}
		d.AddMember("alternate_forms", alternateForms, allocator);
	
		rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
		d.Accept(writer);
	
		fclose(fp);	
	}
	
	void LoadSpecieData(const int nationalDexNumber, sSpeciesData& data)
	{
		rapidjson::Document d;
		if (!OpenPokemonDataFile(d, nationalDexNumber))
		{
			data.nationalDexNumber = 0;
			return;
		}

		if (d["data_version"].GetInt() != JSON_DATA_VERSION)
		{
			std::cout << "WARNING: loader function has a different version as json data" << std::endl;
		}

		strcpy_s(data.name, d["name"].GetString());
		data.nationalDexNumber = d["national_dex_number"].GetUint();
		data.childSpecies = d["child_species"].GetUint();

		data.genderRatio = d["gender_ratio"].GetInt();
		data.eggGroup1 = static_cast<eEggGroup>(d["egg_group_1"].GetInt());
		data.eggGroup2 = static_cast<eEggGroup>(d["egg_group_2"].GetInt());
		data.hatchCycles = d["hatch_cycles"].GetInt();

		data.catchRate = d["catch_rate"].GetInt();
		data.isSpriteGenderBased = d["is_sprite_gender_based"].GetBool();
		data.isFormGenderBased = d["is_stats_gender_based"].GetBool();

		rapidjson::Value& defaultFormObject = d["default_form"];
		LoadFormData(defaultFormObject, data.defaultForm);

		rapidjson::Value& alternateForms = d["alternate_forms"];
		for (unsigned int i = 0; i < alternateForms.Size(); i++)
		{
			data.alternateForms.emplace_back();
			rapidjson::Value& newFormObject = alternateForms[i];
			LoadFormData(newFormObject, data.alternateForms.back());
		}
	}

	void LoadFormData(const int nationalDexNumber, sForm& form, const Name& formName)
	{
		rapidjson::Document d;
		if (!OpenPokemonDataFile(d, nationalDexNumber))
		{
			return;
		}

		if (d["data_version"].GetInt() != JSON_DATA_VERSION)
		{
			std::cout << "WARNING: loader function has a different version as json data" << std::endl;
		}

		if (d["is_stats_gender_based"].GetBool() && formName == "f") // load "female" form
		{
			rapidjson::Value& alternateForms = d["alternate_forms"];
			for (unsigned int i = 0; i < alternateForms.Size(); i++)
			{
				if (alternateForms[i]["name"].GetString() == "female")
				{
					LoadFormData(alternateForms[i], form);
					break;
				}
			}
		}
		else if (formName != "" && formName != "f") // load custom form
		{
			rapidjson::Value& alternateForms = d["alternate_forms"];
			for (unsigned int i = 0; i < alternateForms.Size(); i++)
			{
				if (alternateForms[i]["name"].GetString() == formName)
				{
					LoadFormData(alternateForms[i], form);
					break;
				}
			}
		}
		else // load default form
		{
			rapidjson::Value& defaultFormObject = d["default_form"];
			LoadFormData(defaultFormObject, form);
		}
	}

	void GetAllIdsFromEggGroup(eEggGroup group, std::vector<unsigned int>& output)
	{
		rapidjson::Document d;
		FILE* fp = 0;
		fopen_s(&fp, (PKM_DATA_PATH + "egg_groups.json").c_str(), "rb"); // non-Windows use "r"
		if (fp == 0) return; // File doesn't exists

		// OPTIMIZATION: best buffer size might be different
		char readBuffer[4096];
		rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
		d.ParseStream(is);

		fclose(fp);

		if (d["data_version"].GetInt() != JSON_DATA_VERSION)
		{
			std::cout << "WARNING: loader function has a different version as json data" << std::endl;
		}

		rapidjson::Value& groups = d["groups"];
		for (unsigned int i = 0; i < groups.Size(); i++)
		{
			if (groups[i]["id"].GetUint() != group)
				continue;

			for (unsigned int j = 0; j < groups[i]["species"].Size(); j++)
			{
				output.push_back(groups[i]["species"][j].GetUint());
			}

			break;
		}
	}

	// TODO: have a more versitile way to create battle ready pokemon & incorporate into entering wild encounters and spawn data
	sIndividualData GenerateIndividualPokemonData(int nationalDexId)
	{
		//if (!IsNationalDexNumberValid(nationalDexId)) return;

		return sIndividualData();
	}

	bool GenerateChild(const sIndividualData& parent1, const sIndividualData& parent2, sIndividualData& child)
	{
		sSpeciesData parent1species;
		LoadSpecieData(parent1.nationalDexNumber, parent1species);
		sSpeciesData parent2species;
		LoadSpecieData(parent2.nationalDexNumber, parent2species);

		// Check gender
		if (parent1.gender == parent2.gender)
		{
			if (parent1.gender != NO_GENDER)
				return false;

			// Both genderless, so only if one is a Ditto
			if (!((parent1species.eggGroup1 == EGG_DITTO && parent2species.eggGroup1 != EGG_NO_EGGS_DISCOVERED) ||
				parent2species.eggGroup1 == EGG_DITTO && parent1species.eggGroup1 != EGG_NO_EGGS_DISCOVERED))
				return false;
		}

		// At least one is no eggs discovered
		if (parent1species.eggGroup1 == EGG_NO_EGGS_DISCOVERED || parent2species.eggGroup1 == EGG_NO_EGGS_DISCOVERED)
			return false;

		// Both are ditto
		if (parent1species.eggGroup1 == EGG_DITTO && parent2species.eggGroup1 == EGG_DITTO)
			return false;

		eEggGroup groupMatch1 = EGG_NO_EGG_GROUP, groupMatch2 = EGG_NO_EGG_GROUP;
		if (parent1species.eggGroup1 == EGG_DITTO || parent2species.eggGroup1 == EGG_DITTO) // either is ditto
		{
			sSpeciesData& nonDitto = parent1species;
			if (parent1species.eggGroup1 == EGG_DITTO)
				nonDitto = parent2species;

			groupMatch1 = nonDitto.eggGroup1;
			groupMatch2 = nonDitto.eggGroup2;
		}
		else // Non are no eggs discovered or ditto
		{
			if (parent1species.eggGroup1 == parent2species.eggGroup1 || parent1species.eggGroup1 == parent2species.eggGroup2)
				groupMatch1 = parent1species.eggGroup1;

			if (parent1species.eggGroup2 == parent2species.eggGroup1 || parent1species.eggGroup2 == parent2species.eggGroup2)
				groupMatch2 = parent1species.eggGroup2;
		}

		if (groupMatch1 == EGG_NO_EGG_GROUP && groupMatch2 == EGG_NO_EGG_GROUP)
			return false;

		// Roll for species
		int chance = rand() % 100;
		int chilSpecieId = 0;
		if (chance <= 40)
		{
			// do one
			chilSpecieId = parent1species.childSpecies;
		}
		else if (chance <= 80)
		{
			// do another
			chilSpecieId = parent2species.childSpecies;
		}
		else
		{
			// do random from egg group
			std::vector<unsigned int> possibleSpecies;
			if (groupMatch1 != EGG_NO_EGG_GROUP)
				GetAllIdsFromEggGroup(groupMatch1, possibleSpecies);
			if (groupMatch2 != EGG_NO_EGG_GROUP)
				GetAllIdsFromEggGroup(groupMatch2, possibleSpecies);

			chilSpecieId = possibleSpecies[rand() % possibleSpecies.size()];
		}		

		sSpeciesData childSpecie;
		LoadSpecieData(chilSpecieId, childSpecie);

		// Check if parents current moves can be learned as egg moves
		child.nationalDexNumber = childSpecie.nationalDexNumber;
		child.formName; // TODO

		child.level = 1;
		child.gender; // TODO
		child.isShiny = rand() % BASE_SHINY_ODDS == 0;

		child.isFormGenderBased = childSpecie.isFormGenderBased;
		child.isSpriteGenderBased = childSpecie.isSpriteGenderBased;

		child.expToNextLevel; // TODO
		child.abilityId; // TODO
		child.nature = static_cast<eNature>(rand() % NATURE_COUNT);

		child.IVs.hp = rand() % 32;
		child.IVs.atk = rand() % 32;
		child.IVs.def = rand() % 32;
		child.IVs.spAtk = rand() % 32;
		child.IVs.spDef = rand() % 32;
		child.IVs.spd = rand() % 32;

		// Teach it lv 1 moves

		// Randomise IVs

		return true;
	}

	const std::string sRoamingPokemonData::MakeRoamingTextureName()
	{
		std::string textureName = std::to_string(nationalDexNumber);

		if (gender == FEMALE && (isSpriteGenderBased || isFormGenderBased))
			textureName = textureName + "_f";
		else if (!CompareName(formName, "")) // There is no case where both will be true
			textureName = textureName + "_" + formName;

		if (isShiny)
			textureName = textureName + "_s";

		textureName = textureName + ".png";

		return textureName;
	}

	const std::string sRoamingPokemonData::MakeIconTextureName()
	{
		std::string textureName = std::to_string(nationalDexNumber);

		if (gender == FEMALE && isFormGenderBased)
			textureName = textureName + "_f";
		else if (!CompareName(formName, "")) // There is no case where both will be true
			textureName = textureName + "_" + formName;

		textureName += "_ico";

		if (isShiny)
			textureName = textureName + "_s";

		textureName = textureName + ".png";

		return textureName;
	}

	const std::string sIndividualData::MakeBattleTextureName(bool isFront)
	{
		std::string textureName = std::to_string(nationalDexNumber);

		if (gender == FEMALE && (isSpriteGenderBased || isFormGenderBased))
			textureName = textureName + "_f";
		else if (formName != "") // There is no case where both will be true
			textureName = textureName + "_" + formName;

		if (isFront)
			textureName = textureName + "_bf";
		else
			textureName = textureName + "_bb";

		if (isShiny)
			textureName = textureName + "_s";

		textureName = textureName + ".png";

		return textureName;
	}

	bool OpenPokemonDataFile(rapidjson::Document& doc, const int nationalDexNumber)
	{
		if (!IsNationalDexNumberValid(nationalDexNumber)) return false;

		std::string dexNumberString = MakeDexNumberFolderName(nationalDexNumber);

		FILE* fp = 0;
		fopen_s(&fp, (PKM_DATA_PATH + "species/" + dexNumberString + "/" + dexNumberString + ".json").c_str(), "rb"); // non-Windows use "r"
		if (fp == 0) return false; // File doesn't exists

		// OPTIMIZATION: best buffer size might be different
		char readBuffer[4096];
		rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

		doc.ParseStream(is);

		fclose(fp);
		return true;
	}

	void LoadFormData(rapidjson::Value& formObject, sForm& form)
	{
		strcpy_s(form.name, formObject["name"].GetString());

		form.type1 = static_cast<eType>(formObject["type_1"].GetInt());
		form.type2 = static_cast<eType>(formObject["type_2"].GetInt());

		form.ability1 = formObject["ability_1"].GetUint();
		form.ability2 = formObject["ability_2"].GetUint();
		form.hiddenAbility = formObject["hidden_ability"].GetUint();

		form.baseStats.hp = formObject["base_hp"].GetUint();
		form.baseStats.atk = formObject["base_atk"].GetUint();
		form.baseStats.def = formObject["base_def"].GetUint();
		form.baseStats.spAtk = formObject["base_spAtk"].GetUint();
		form.baseStats.spDef = formObject["base_spDef"].GetUint();
		form.baseStats.spd = formObject["base_spd"].GetUint();

		form.height = formObject["height"].GetInt();
		form.weight = formObject["weight"].GetInt();

		form.frontSpriteFrameCount = formObject["front_sprite_count"].GetUint();
		form.backSpriteFrameCount = formObject["back_sprite_count"].GetUint();

		rapidjson::Value& learnset = formObject["learnset"];
		for (unsigned int i = 0; i < learnset.Size(); i += 2)
		{
			int level = learnset[i].GetInt();
			unsigned int id = learnset[i + 1].GetUint();
			form.learnset.push_back(std::pair<int, unsigned>(level, id));
		}
	}
}