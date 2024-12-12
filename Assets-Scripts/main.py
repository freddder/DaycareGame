import requests
import os
from PIL import Image
from pprint import pprint

max_dex_num = 1008

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
    
    url = f"https://pokeapi.co/api/v2/pokemon/{dex_num}/"
    response = requests.get(url)
    data = response.json()

    icon_url = data["sprites"]["versions"]["generation-viii"]["icons"]["front_default"]
    icon = requests.get(icon_url)
    with open(f"{dex_num}_ico.png", 'wb') as file:
        file.write(icon.content)

    battle_sprite_url = data["sprites"]["versions"]["generation-v"]["black-white"]["animated"]["front_default"]
    if battle_sprite_url is None:
        battle_sprite_url = data["sprites"]["versions"]["generation-v"]["black-white"]["front_default"]
        battle_sprite = requests.get(battle_sprite_url)
        with open(f"{dex_num}_bf.png", 'wb') as file:
            file.write(battle_sprite.content)
    else:
        battle_sprite = requests.get(battle_sprite_url)
        gif_file_name = "temp.gif"
        with open(gif_file_name, 'wb') as file:
            file.write(battle_sprite.content)

        battle_sprite_gif = Image.open(gif_file_name)
        sprite_sheet = Image.new("RGBA", (battle_sprite_gif.size[0] * battle_sprite_gif.n_frames, battle_sprite_gif.size[1]))

        for frame in range(0, battle_sprite_gif.n_frames):
            battle_sprite_gif.seek(frame)
            extracted_frame = battle_sprite_gif.resize(battle_sprite_gif.size)
            position = (frame * battle_sprite_gif.size[0], 0)
            sprite_sheet.paste(extracted_frame, position)

        sprite_sheet.save("result.png")

    os.remove(gif_file_name)
    

def main():
    os.chdir("../DaycareGame/assets/pokemon")

    #create_entry(132)
    create_entry(445)
    #create_entry(681)

if __name__ == "__main__":
    main()