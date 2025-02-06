#include "Pasture.h"

#include "CanvasFactory.h"
#include "Engine.h"
#include "cCameraManager.h"
#include "PokemonData.h"

#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>

cPastureCanvas::cPastureCanvas()
{
    rapidjson::Document doc;
    FILE* fp = 0;
    fopen_s(&fp, "assets/save_data/pastures.json", "rb"); // non-Windows use "r"
    if (fp == 0) return; // File doesn't exists
    char readBuffer[4096]; // OPTIMIZATION: best buffer size might be different
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    doc.ParseStream(is);
    fclose(fp);

    if (!doc.HasMember("data")) return;

    cUIGrid* grid = new cUIGrid(6, 5);
    grid->heightPercent = 0.9f;
    grid->aspectRatio = (3.f * 5.f) / (4.f * 6.f);

    rapidjson::Value& data = doc["data"];
    for (unsigned int i = 0; i < data.Size(); i++)
    {
        if (data[i].IsNull()) continue;

        unsigned int dexNum = data[i].GetUint();
        std::string iconFileName = std::to_string(dexNum) + "_ico.png";

        cUIImage* icon = new cUIImage();
        icon->aspectRatio = 3.f / 4.f;
        icon->textureId = LoadUITexture(iconFileName, "assets/pokemon/" + Pokemon::MakeDexNumberFolderName(dexNum) + "/");

        grid->AssignChildAtIndex(icon, i);
    }

    cAnimatedBackground* background = new cAnimatedBackground(this);
    background->textureId = LoadUITexture("background.png");
    background->aspectRatio = (float)Manager::camera.SCR_HEIGHT / (float)Manager::camera.SCR_WIDTH;
    background->useScreenSpace = true;
    background->screenSpaceRatio = glm::vec2(30.f); 

    anchoredWidgets.push_back(grid);
    anchoredWidgets.push_back(background);

    defaultFocus = grid;
}
