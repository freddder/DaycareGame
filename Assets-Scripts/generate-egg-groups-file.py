import requests
import os
import json

current_data_version = 2

egg_incense_exceptions = [
    113,    # Chancey
    122,    # Mr. Mime
    143,    # Snorlax
    183,    # Marill
    185,    # Sudowoodo
    202,    # Wobbuffet
    226,    # Mantine
    315,    # Roselia
    358,    # Chimecho
]

def get_egg_group_data(group_id):

    url = f"https://pokeapi.co/api/v2/egg-group/{group_id}/"
    response = requests.get(url)
    data = response.json()

    group = {
        "id": group_id,
        "name": data["name"]
    }

    namee = data["name"]
    print(f"\nGROUP - {namee}")

    species = []
    for specie in data["pokemon_species"]:
        dex_num = int(specie["url"].split('/')[-2])

        url = f"https://pokeapi.co/api/v2/pokemon-species/{dex_num}/"
        response = requests.get(url)
        specie_data = response.json()

        specie_name = specie_data["name"]
        if specie_data["evolves_from_species"] == None or dex_num in egg_incense_exceptions:
            print(f"{specie_name} - yup")
            species.append(dex_num)
        else:
            print(f"{specie_name} - nop")
    
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