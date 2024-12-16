#include "Pasture.h"

#include "CanvasFactory.h"
#include "Engine.h"
#include "cCameraManager.h"

cPastureCanvas::cPastureCanvas()
{
    cAnimatedBackground* background = new cAnimatedBackground(this);
    background->textureId = LoadUITexture("background.png");
    background->aspectRatio = (float)Manager::camera.SCR_HEIGHT / (float)Manager::camera.SCR_WIDTH;
    background->useScreenSpace = true;
    background->screenSpaceRatio = glm::vec2(30.f);

    cUIGrid* grid = new cUIGrid(6, 5);
    grid->heightPercent = 0.9f;

    for (int i = 0; i < 30; i++)
    {
        cUIImage* icon = new cUIImage();

        grid->AssignChildAtIndex(icon, i);
    }

    anchoredWidgets.push_back(grid);
    anchoredWidgets.push_back(background);
}
