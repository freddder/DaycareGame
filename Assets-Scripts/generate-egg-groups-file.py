import requests
import os
import json

current_data_version = 1

def get_egg_group_data(group_id):

    url = f"https://pokeapi.co/api/v2/egg-group/{group_id}/"
    response = requests.get(url)
    data = response.json()

    group = {
        "id": group_id,
        "name": data["name"]
    }

    species = []
    for specie in data["pokemon_species"]:
        species.append(int(specie["url"].split('/')[-2]))
    
    group["species"] = species

    return group

def main():
    os.chdir("../DaycareGame/assets/pokemon")
    file_data = { "data_version": current_data_version }

    groups = []
    for i in range(1, 16):
        new_group = get_egg_group_data(i)
        groups.append(new_group)
    
    file_data["groups"] = groups
    
    dump = json.dumps(file_data, indent=4)
    with open("egg_groups.json", 'w') as file:
        file.write(dump)
    
    os.chdir("../")


if __name__ == "__main__":
    main()