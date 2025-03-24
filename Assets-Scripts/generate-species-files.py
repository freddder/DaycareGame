import requests
import os
import json

max_dex_num = 1008

current_data_version = 1

def load_form_data(form_json):
    form_data = {}

    form_data["type1"] = int(form_json["types"][0]["type"]["url"].split('/')[-2])
    if len(form_json["types"]) > 1:
        form_data["type2"] = int(form_json["types"][1]["type"]["url"].split('/')[-2])
    else:
        form_data["type2"] = 0
    
    ability_count = 1
    form_data["ability1"] = 0
    form_data["ability2"] = 0
    form_data["hiddenAbility"] = 0
    for ability in form_json["abilities"]:
        if ability["is_hidden"]:
            form_data["hiddenAbility"] = int(ability["ability"]["url"].split('/')[-2])
        else:
            form_data[f"ability{ability_count}"] = int(ability["ability"]["url"].split('/')[-2])
            ability_count += 1
    
    form_data["baseHp"] = form_json["stats"][0]["base_stat"]
    form_data["baseAtk"] = form_json["stats"][1]["base_stat"]
    form_data["baseDef"] = form_json["stats"][2]["base_stat"]
    form_data["baseSpAtk"] = form_json["stats"][3]["base_stat"]
    form_data["baseSpDef"] = form_json["stats"][4]["base_stat"]
    form_data["baseSpd"] = form_json["stats"][5]["base_stat"]

    form_data["height"] = form_json["height"]
    form_data["weight"] = form_json["weight"]

    learnset = []
    for move in form_json["moves"]:
        for vgd in move["version_group_details"]:
            if vgd["version_group"]["name"] == "ultra-sun-ultra-moon":
                new_move = {}
                new_move["id"] = int(move["move"]["url"].split('/')[-2])
                new_move["name"] = move["move"]["name"]
                if vgd["move_learn_method"]["name"] == "level-up":
                    new_move["level"] = vgd["level_learned_at"]
                elif vgd["move_learn_method"]["name"] == "egg":
                    new_move["level"] = 0
                elif vgd["move_learn_method"]["name"] == "machine":
                    new_move["level"] = -1
                elif vgd["move_learn_method"]["name"] == "tutor":
                    new_move["level"] = -2
                learnset.append(new_move)
                break
    
    learnset.sort(key=lambda x: x["level"])
    form_data["learnset"] = learnset
    
    return form_data

def create_entry(dex_num):
    if dex_num > max_dex_num:
        return

    dex_str = str(dex_num)
    while len(dex_str) < 4:
        dex_str = "0" + dex_str
    
    folder_path = os.getcwd() + "/" + dex_str
    if not os.path.exists(folder_path):
        os.mkdir(folder_path)
    os.chdir(dex_str)

    pkm_url = f"https://pokeapi.co/api/v2/pokemon/{dex_num}/"
    pkm_response = requests.get(pkm_url)
    pkm_data = pkm_response.json()

    specie_url = f"https://pokeapi.co/api/v2/pokemon-species/{dex_num}/"
    specie_response = requests.get(specie_url)
    specie_data = specie_response.json()

    file_data = { "data_version": current_data_version }

    for name in specie_data["names"]:
        if name["language"]["name"] == "en":
            file_data["name"] = name["name"]
    
    file_data["nationalDexNumber"] = dex_num

    file_data["genderRatio"] = specie_data["gender_rate"]

    file_data["eggGroup1"] = int(specie_data["egg_groups"][0]["url"].split('/')[-2])
    egg_group_2 = 16
    if len(specie_data["egg_groups"]) > 1:
        egg_group_2 = int(specie_data["egg_groups"][1]["url"].split('/')[-2])
    file_data["eggGroup2"] = egg_group_2

    file_data["hatchCycles"] = specie_data["hatch_counter"]

    file_data["catchRate"] = specie_data["capture_rate"]

    file_data["isSpriteGenderBased"] = specie_data["has_gender_differences"]

    file_data["isStatsGenderBased"] = specie_data["varieties"][1]["pokemon"]["name"].split('-')[-1] == "female"

    file_data["defaultForm"] = load_form_data(pkm_data)

    dump = json.dumps(file_data, indent=4)
    with open(f"{dex_num}new.json", 'w') as file:
        file.write(dump)
    
    os.chdir("../")


def main():
    os.chdir("../DaycareGame/assets/pokemon")

    create_entry(445)
    create_entry(678)

    #for i in range(387, 408):
    #    create_entry(i)

if __name__ == "__main__":
    main()