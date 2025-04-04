import requests
import os
import json
from PIL import Image

max_dex_num = 1008
current_data_version = 3

# TODO: find a way to classify if species can change form/variant
# TODO: create a way to describe all possible evolutions and their methods

def load_form_data(form_json):
    form_data = {}
    	
    split_name = form_json["name"].split('-', 1)
    if len(split_name) > 1:
        form_data["name"] = split_name[-1]
    else:
        form_data["name"] = ""

    form_data["type_1"] = int(form_json["types"][0]["type"]["url"].split('/')[-2])
    if len(form_json["types"]) > 1:
        form_data["type_2"] = int(form_json["types"][1]["type"]["url"].split('/')[-2])
    else:
        form_data["type_2"] = 0
    
    ability_count = 1
    form_data["ability_1"] = 0
    form_data["ability_2"] = 0
    form_data["hidden_ability"] = 0
    for ability in form_json["abilities"]:
        if ability["is_hidden"]:
            form_data["hidden_ability"] = int(ability["ability"]["url"].split('/')[-2])
        else:
            form_data[f"ability_{ability_count}"] = int(ability["ability"]["url"].split('/')[-2])
            ability_count += 1
    
    form_data["base_hp"] = form_json["stats"][0]["base_stat"]
    form_data["base_atk"] = form_json["stats"][1]["base_stat"]
    form_data["base_def"] = form_json["stats"][2]["base_stat"]
    form_data["base_spAtk"] = form_json["stats"][3]["base_stat"]
    form_data["base_spDef"] = form_json["stats"][4]["base_stat"]
    form_data["base_spd"] = form_json["stats"][5]["base_stat"]

    form_data["height"] = form_json["height"]
    form_data["weight"] = form_json["weight"]

    front_sprite_url = form_json["sprites"]["versions"]["generation-v"]["black-white"]["animated"]["front_default"]
    back_sprite_url = form_json["sprites"]["versions"]["generation-v"]["black-white"]["animated"]["back_default"]
    if front_sprite_url is None:
        form_data["front_sprite_count"] = 1
        form_data["back_sprite_count"] = 1
    else:
        response = requests.get(front_sprite_url, stream=True)
        img = Image.open(response.raw)
        form_data["front_sprite_count"] = img.n_frames

        response = requests.get(back_sprite_url, stream=True)
        img = Image.open(response.raw)
        form_data["back_sprite_count"] = img.n_frames

    moves = []
    for move in form_json["moves"]:
        for vgd in move["version_group_details"]:
            if vgd["version_group"]["name"] == "ultra-sun-ultra-moon":
                move_id = int(move["move"]["url"].split('/')[-2])
                move_learn_method = vgd["move_learn_method"]["name"]
                move_level = 0
                if move_learn_method == "level-up":
                    move_level = vgd["level_learned_at"]
                    if move_level == 0: # when evolving
                        move_level = -3
                elif move_learn_method == "machine":
                    move_level = 0
                elif move_learn_method == "egg":
                    move_level = -1
                elif move_learn_method == "tutor":
                    move_level = -2

                moves.append((move_level, move_id))
                break
    moves.sort(key=lambda tup: tup[0])
    learnset = []
    for move in moves:
        learnset.extend(move)

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

    specie_url = f"https://pokeapi.co/api/v2/pokemon-species/{dex_num}/"
    specie_response = requests.get(specie_url)
    specie_data = specie_response.json()

    species_name = specie_data["name"]
    print(species_name)

    file_data = { "data_version": current_data_version }

    for name in specie_data["names"]:
        if name["language"]["name"] == "en":
            file_data["name"] = name["name"]
            break
    
    file_data["national_dex_number"] = dex_num

    if specie_data["evolves_from_species"] == None:
        file_data["child_species"] = dex_num
    else:
        evo_url = specie_data["evolution_chain"]["url"]
        evo_response = requests.get(evo_url)
        evo_data = evo_response.json()

        if evo_data["baby_trigger_item"] == None:
            file_data["child_species"] = int(evo_data["chain"]["species"]["url"].split('/')[-2])
        else:
            file_data["child_species"] = int(evo_data["chain"]["evolves_to"][0]["species"]["url"].split('/')[-2])

    file_data["gender_ratio"] = specie_data["gender_rate"]

    file_data["egg_group_1"] = int(specie_data["egg_groups"][0]["url"].split('/')[-2])
    egg_group_2 = 0
    if len(specie_data["egg_groups"]) > 1:
        egg_group_2 = int(specie_data["egg_groups"][1]["url"].split('/')[-2])
    file_data["egg_group_2"] = egg_group_2

    file_data["hatch_cycles"] = specie_data["hatch_counter"]

    file_data["catch_rate"] = specie_data["capture_rate"]

    file_data["is_sprite_gender_based"] = specie_data["has_gender_differences"]

    if len(specie_data["varieties"]) > 1:
        file_data["is_stats_gender_based"] = specie_data["varieties"][1]["pokemon"]["name"].split('-')[-1] == "female"
    else:
        file_data["is_stats_gender_based"] = False

    alternate_forms = []
    variants = []
    for form in specie_data["varieties"]:
        url = form["pokemon"]["url"]
        form_response = requests.get(url)
        form_data = form_response.json()

        splits = form["pokemon"]["name"].split('-')
        if form["is_default"]:
            file_data["default_form"] = load_form_data(form_data)

            # No spiky ear pichu
            if dex_num == 172:
                continue

            # No mothim variants
            if dex_num == 414:
                continue

            # Xerneas variants are kinda useless
            if dex_num == 716:
                continue

            # Not supporting arceus, genesect or silvally
            if dex_num == 493 or dex_num == 649 or dex_num == 773:
                continue

            # Correct mr mime and mime jr name
            if dex_num == 122 or dex_num == 439:
                file_data["default_form"]["name"] = ""

            for variant in form_data["forms"]:
                if variant["name"] != form_data["name"]:
                    variants.append(variant["name"].split('-', 1)[-1])
        elif "mega" not in splits and "totem" not in splits and "gmax" not in splits: # dont include megas, totems or gmax
            # Fuck pikachu and its 30 fukcing pointless forms
            if dex_num == 25:
                continue

            # Not supporting galarian or paldean forms (alolan should be fine)
            if "galar" in splits or "paldea" in splits or "hisui" in splits:
                continue

            alternate_forms.append(load_form_data(form_data))
            
    file_data["alternate_forms"] = alternate_forms
    file_data["variants"] = variants

    # some possible overrites
    # - rename mr mime and mime jr
    # - clear mothim variants

    dump = json.dumps(file_data, indent=4)
    with open(f"{dex_str}.json", 'w') as file:
        file.write(dump)
    
    os.chdir("../")


def main():
    os.chdir("../DaycareGame/assets/pokemon/species")

    #create_entry(150)
    #create_entry(406)
    #create_entry(315)
    #create_entry(407)
    #create_entry(413)
    #create_entry(445)
    #create_entry(678)
    #create_entry(665)

    for i in range(1, 810):
        create_entry(i)

    # Apparently largest form name is alcremie-ruby-cream-strawberry-sweet with 5 words (yikes)
    #largest_length = 0
    #for i in range(10001, 10503):
    #    url = f"https://pokeapi.co/api/v2/pokemon-form/{i}/"
    #    response = requests.get(url)
    #    data = response.json()
    #
    #    name = data["name"]
    #    length = len(data["name"].split('-'))
    #    if length > largest_length:
    #        largest_length = length
    #        print(f"{length}: {name}")


if __name__ == "__main__":
    main()