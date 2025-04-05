import requests
import os
import json
from PIL import Image

# TODO: find a way to classify if species can change form/variant
# TODO: create a way to describe all possible evolutions and their methods

max_dex_num = 1008
current_data_version = 3
linking_cord_id = 2160

no_variants_override = [
    172, # No spiky ear pichu
    414, # No mothim variants
    716, # Xerneas variants are kinda useless
    493, # Arceus
    649, # Xerneas
    773  # Silvally
]

def get_matching_evo_branch(branch, dex_num):
    
    if int(branch["species"]["url"].split('/')[-2]) == dex_num:
        return branch
    
    for evo_branch in branch["evolves_to"]:
        new_branch = get_matching_evo_branch(evo_branch, dex_num)
        if new_branch != False:
            return new_branch
        
    return False

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

    file_data["gender_ratio"] = specie_data["gender_rate"]

    file_data["egg_group_1"] = int(specie_data["egg_groups"][0]["url"].split('/')[-2])
    egg_group_2 = 0
    if len(specie_data["egg_groups"]) > 1:
        egg_group_2 = int(specie_data["egg_groups"][1]["url"].split('/')[-2])
    file_data["egg_group_2"] = egg_group_2

    file_data["hatch_cycles"] = specie_data["hatch_counter"]

    file_data["catch_rate"] = specie_data["capture_rate"]

    evo_url = specie_data["evolution_chain"]["url"]
    evo_response = requests.get(evo_url)
    evo_data = evo_response.json()

    if specie_data["evolves_from_species"] == None:
        file_data["child_species"] = dex_num
    else:
        if evo_data["baby_trigger_item"] == None:
            file_data["child_species"] = int(evo_data["chain"]["species"]["url"].split('/')[-2])
        else:
            file_data["child_species"] = int(evo_data["chain"]["evolves_to"][0]["species"]["url"].split('/')[-2])
    
    species_evo_branch = get_matching_evo_branch(evo_data["chain"], dex_num)
    evolutions = []
    for new_evo_data in species_evo_branch["evolves_to"]:
        if dex_num == 489: # phione does not evolve
            break
        
        new_evo = {}
        new_evo_details = new_evo_data["evolution_details"][-1]
        new_evo["dex_num"] = int(new_evo_data["species"]["url"].split('/')[-2])
        new_evo["min_level"] = new_evo_details["min_level"]
        new_evo["friendship"] = new_evo_details["min_happiness"] != None
        new_evo["day_time"] = new_evo_details["time_of_day"]

        new_evo["use_item"] = 0
        if new_evo_details["trigger"]["name"] == "use-item":
            new_evo["use_item"] = int(new_evo_details["item"]["url"].split('/')[-2])
        elif new_evo_details["trigger"]["name"] == "trade":
            if new_evo_details["held_item"] != None:
                new_evo["use_item"] = int(new_evo_details["held_item"]["url"].split('/')[-2])
            else:
                new_evo["use_item"] = linking_cord_id

        new_evo["held_item"] = 0
        if new_evo_details["held_item"] != None and new_evo["use_item"] == 0:
            new_evo["held_item"] = int(new_evo_details["held_item"]["url"].split('/')[-2])

        new_evo["gender"] = 0
        if new_evo_details["gender"] != None:
            new_evo["gender"] = new_evo_details["gender"]

        new_evo["known_move"] = 0
        if new_evo_details["known_move"] != None:
            new_evo["known_move"] = int(new_evo_details["known_move"]["url"].split('/')[-2])
        elif new_evo["dex_num"] == 700: # Chose baby-doll-eyes to evolve into sylveon
            new_evo["known_move"] = 608

        if len(new_evo_data["evolution_details"]) > 1:
            print(f"LOOK AT THIS GUY -> {species_name}")
        
        #evolutions.append(new_evo)
    
    # Make sure sylveon is above espeon and umbreon
    if dex_num == 133:
        evolutions.reverse()

    file_data["evolutions"] = evolutions

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

            if dex_num in no_variants_override:
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

    dump = json.dumps(file_data, indent=4)
    with open(f"{dex_str}.json", 'w') as file:
        file.write(dump)
    
    os.chdir("../")


def main():
    os.chdir("../DaycareGame/assets/pokemon/species")

    #create_entry(67)
    #create_entry(93)
    #create_entry(125)
    #create_entry(133)
    #create_entry(265)
    #create_entry(303)
    #create_entry(361)
    #create_entry(412)

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