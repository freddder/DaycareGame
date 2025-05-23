#include "Engine.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>
#include "cCameraManager.h"
#include "cLightManager.h"
#include "cAnimationManager.h"
#include "cRenderManager.h"
#include "cMapManager.h"
#include "cSceneManager.h"
#include "cUIManager.h"
#include "cInputManager.h"

#include "PokemonData.h"

#include "Player.h"
#include "cPlayerEntity.h"
#include "cTamedRoamingPokemon.h"
#include "cNPCCharacter.h"

GLFWwindow* window;
float deltaTime = 0.f;
float lastFrame = 0.f;

static bool isFullscreen = false;

static int searchNationalDexNumber = 0;
static Pokemon::sSpeciesData selectedSpecies;
static eEnvironmentWeather selectedWeather = SNOW;

static Pokemon::sIndividualData pkm1;
static Pokemon::sIndividualData pkm2;
static Pokemon::sIndividualData child;

const char* resolutions[] = {
    "2560x1400",
    "1920x1080",
    "1600x900",
    "1280x720",
    "900x900",
};

void SetWindowResolution(int width, int height)
{
    Manager::camera.SCR_WIDTH = width;
    Manager::camera.SCR_HEIGHT = height;
    glfwSetWindowSize(window, width, height);
}

void InitializeImgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.WantCaptureMouse = false;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void RenderImgui();
void RenderFormData(Pokemon::sForm& form);
void RenderIndividualData(Pokemon::sIndividualData& data);
void RenderEvolutionData(Pokemon::sEvolution& data);

void ShutdownImgui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

namespace Manager
{
    cCameraManager camera;
    cLightManager light;
    cAnimationManager animation;
    cRenderManager render;
    cMapManager map;
    cSceneManager scene;
    cUIManager ui;
    cInputManager input;
}

namespace Engine
{
    eGameMode currGameMode = eGameMode::MAP;
    cEntity* currInteractingEntity = nullptr;

    // camera
    float lastX = 1200 / 2.0f;
    float lastY = 640 / 2.0f;
    bool firstMouse = true;

	bool InitializeGLFW()
	{
        // glfw: initialize and configure
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        // glfw window creation
        window = glfwCreateWindow(1280, 720, "Magik", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetKeyCallback(window, key_callback);

        // tell GLFW to capture our mouse
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        // glad: load all OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return false;
        }

        return true;
	}

    void StartUpManagers()
    {
        //cCamera::GetInstance();
        //cLightManager::GetInstance();
        //cAnimationManager::GetInstance();
        //cRenderManager::GetInstance();
        //cMapManager::GetInstance();
        //cSceneManager::GetInstance();
        //cUIManager::GetInstance();

        Manager::light.Startup();

        Manager::render.Startup();

        Player::playerChar = new cPlayerEntity();
        Manager::camera.targetPosRef = Player::GetPlayerPositionRef();

        Pokemon::sIndividualData partner;
        partner.nationalDexNumber = 445;
        partner.gender = Pokemon::FEMALE;
        partner.isShiny = true;
        //partner.form.battleBackSpriteFrameCount = 48; // hard coded for now
        partner.level = 98;
        //partner.LoadFormFromSpeciesData();
        Player::AddPartyMember(partner);

        Pokemon::sSpeciesData followerSpecieData;
        Pokemon::LoadSpecieData(Player::party[0].nationalDexNumber, followerSpecieData);
        Manager::render.LoadRoamingPokemonSpecieTextures(followerSpecieData);

        // Not being added to tamed roaming vector on scene manager
        Player::playerPartner = std::make_shared<cTamedRoamingPokemon>(partner, glm::vec3(0));
        Player::playerChar->SetFollower(Player::playerPartner.get());

        Manager::map.Startup();

        Manager::render.LoadSpriteSheet("AsymetricalNPC_1.png", 3, 4);
        std::shared_ptr<cNPCCharacter> npc = Manager::scene.SpawnNPC("AsymetricalNPC_1.png", glm::vec3(0.f, 0.f, 20.f));

        Manager::input.Startup();

        Manager::ui.Startup();
    }

    void ShutdownManagers()
    {
        //cCamera::DestroyInstance();
        //cLightManager::DestroyInstance();
        //cRenderManager::DestroyInstance();
        //cMapManager::DestroyInstance();
        //cSceneManager::DestroyInstance();
        //cUIManager::DestroyInstance();
        //cAnimationManager::DestroyInstance();

        Manager::light.Shutdown();

        Manager::map.Shutdown();

        Manager::render.Shutdown();

        Manager::scene.Shutdown();

        Manager::ui.Shutdown();

        delete Player::playerChar;

        // TODO: I think there is one sprite model not properly deleting. Investigate later
    }

    void GameLoop(bool renderDebugInfo)
    {
        if (renderDebugInfo) InitializeImgui();

        while (!glfwWindowShouldClose(window))
        {
            // per-frame time logic
            float currentFrame = (float)glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            // input
            Manager::input.Process(deltaTime);

            Manager::animation.Process(deltaTime);

            Manager::scene.Process(deltaTime);

            Manager::render.DrawFrame();

            if (renderDebugInfo) RenderImgui();

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        if (renderDebugInfo) ShutdownImgui();
    }

    void ShutdownGLFW()
    {
        //glfw: terminate, clearing all previously allocated GLFW resources.
        glfwTerminate();
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        Manager::input.UpdateInput(key, action);
    }

    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and 
        // height will be significantly larger than specified on retina displays.
        //glViewport(0, 0, width, height);

        Manager::camera.SCR_WIDTH = width;
        Manager::camera.SCR_HEIGHT = height;
    }


    // glfw: whenever the mouse moves, this callback is called
    void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
    {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {

            float xpos = static_cast<float>(xposIn);
            float ypos = static_cast<float>(yposIn);

            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

            lastX = xpos;
            lastY = ypos;

            Manager::camera.ProcessMouseMovement(xoffset, yoffset);
        }
    }

    // glfw: whenever the mouse scroll wheel scrolls, this callback is called
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        //camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}

void RenderImgui()
{
    // Imgui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //ImGui::ShowDemoWindow();

    static int currentSelectedResolutionIndex = 3;

    ImGui::Begin("Debug");
    ImGui::Text("FPS: %f", (1.f / deltaTime));
    if (ImGui::Button(isFullscreen ? "Window" : "Fullscreen"))
    {
        if (isFullscreen) // set windowed
        {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
            glfwSetWindowPos(window, mode->width / 7, mode->height / 7);

            std::string resolution = resolutions[currentSelectedResolutionIndex];
            size_t pos = resolution.find('x');
            int width = stoi(resolution.substr(0, pos));
            int height = stoi(resolution.substr(pos + 1));
            SetWindowResolution(width, height);
        }
        else // set borlerless window (fullscreen)
        {
            glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
            glfwSetWindowPos(window, 0, 0);
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowSize(window, mode->width, mode->height);
        }

        isFullscreen = !isFullscreen;
    }

    if (isFullscreen) ImGui::BeginDisabled();

    // Pass in the preview value visible before opening the combo (it could be anything)
    std::string resolutionPreviewValue = std::to_string(Manager::camera.SCR_WIDTH) + "x" + std::to_string(Manager::camera.SCR_HEIGHT);
    float aspectRatio = round(((float)Manager::camera.SCR_WIDTH / (float)Manager::camera.SCR_HEIGHT) * 100.f) / 100.f;
    ImGui::SameLine(); ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.7f);
    if (ImGui::BeginCombo(std::to_string(aspectRatio).c_str(), resolutionPreviewValue.c_str()))
    {
        for (int n = 0; n < IM_ARRAYSIZE(resolutions); n++)
        {
            const bool is_selected = (currentSelectedResolutionIndex == n);
            if (ImGui::Selectable(resolutions[n], is_selected) && !isFullscreen)
            {
                currentSelectedResolutionIndex = n;
                std::string newResolution = resolutions[n];
                size_t pos = newResolution.find('x');
                int width = stoi(newResolution.substr(0, pos));
                int height = stoi(newResolution.substr(pos + 1));

                SetWindowResolution(width, height);
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (isFullscreen) ImGui::EndDisabled();

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Camera"))
    {
        float* cameraPosition[3];
        cameraPosition[0] = &Manager::camera.position.x;
        cameraPosition[1] = &Manager::camera.position.y;
        cameraPosition[2] = &Manager::camera.position.z;

        ImGui::DragFloat3("Position", *cameraPosition);
        ImGui::Checkbox("Player Cam", &Manager::camera.usePlayerCamera);
        ImGui::DragFloat("FOV", &Manager::camera.FOV);
        ImGui::DragFloat("Distance", &Manager::camera.targetDistance);
        ImGui::DragFloat("Angle", &Manager::camera.targetAngle);
    }

    if (ImGui::CollapsingHeader("Enviornment"))
    {
        if (ImGui::BeginCombo("Weather", Weather_Strings[selectedWeather]))
        {
            for (int n = 0; n < eEnvironmentWeather::ENUM_COUNT; n++)
            {
                const bool is_selected = (selectedWeather == n);
                if (ImGui::Selectable(Weather_Strings[n], is_selected))
                {
                    selectedWeather = static_cast<eEnvironmentWeather>(n);
                    Manager::scene.SetWeather(selectedWeather);
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::DragFloat("Wind Speed", &Manager::scene.windSpeed, 0.01f, 0.01f, 10.f);

        if (ImGui::BeginTabBar("Tabs"))
        {
            if (ImGui::BeginTabItem("Light"))
            {
                float* position[3];
                position[0] = &Manager::light.lights[0].position.x;
                position[1] = &Manager::light.lights[0].position.y;
                position[2] = &Manager::light.lights[0].position.z;

                float* colors[3];
                colors[0] = &Manager::light.lights[0].diffuse.r;
                colors[1] = &Manager::light.lights[0].diffuse.g;
                colors[2] = &Manager::light.lights[0].diffuse.b;

                int* shadowSmooth = &Manager::light.shadowSampleRadius;

                ImGui::ColorEdit3("Color", *colors);
                ImGui::DragFloat3("Position", *position);
                ImGui::DragInt("Smoothing", shadowSmooth);
                //ImGui::Image((void*)(intptr_t)Manager::render.GetDepthMapId(), ImVec2(200, 200));
                ImGui::Image((void*)(intptr_t)Manager::ui.GetFontTextureId("Truth And Ideals-Normal.ttf"), ImVec2(200, 200));

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Fog"))
            {
                float* fogColor[3];
                fogColor[0] = &Manager::scene.fogColor.r;
                fogColor[1] = &Manager::scene.fogColor.g;
                fogColor[2] = &Manager::scene.fogColor.b;

                ImGui::ColorEdit3("Color", *fogColor);
                ImGui::DragFloat("Density", &Manager::scene.fogDensity, 0.005f, 0.f);
                ImGui::DragFloat("Gradient", &Manager::scene.fogGradient, 0.03f);

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }

    if (ImGui::CollapsingHeader("Data"))
    {
        ImGui::Text("Search National Dex");
        ImGui::PushItemWidth(50); ImGui::SameLine();
        ImGui::DragInt(":", &searchNationalDexNumber, 1, 0, 1008);

        ImGui::SameLine();
        if (ImGui::Button("Load Species Data"))
        {
            selectedSpecies = Pokemon::sSpeciesData();
            Pokemon::LoadSpecieData(searchNationalDexNumber, selectedSpecies);
        }

        if (selectedSpecies.nationalDexNumber != 0)
        {
            std::string title = selectedSpecies.name.str() + " #" + std::to_string(selectedSpecies.nationalDexNumber);
            ImGui::Text(title.c_str());

            if (ImGui::BeginTable("table1", 2, ImGuiTableFlags_SizingFixedFit))
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                ImGui::Image((void*)(intptr_t)Manager::render.GetDepthMapId(), ImVec2(120, 90));

                ImGui::TableNextColumn();

                ImGui::DragInt("Gender ratio", &selectedSpecies.genderRatio, 1, -1, 8);
                if (ImGui::IsItemHovered()) ImGui::SetTooltip("Chance to be female in eights (-1 for genderless)");

                if (selectedSpecies.genderRatio != -1)
                {
                    ImGui::Checkbox("Are stats gender based", &selectedSpecies.isFormGenderBased);
                }

                if (ImGui::BeginCombo("Egg Group 1", Pokemon::EggGroup_Strings[selectedSpecies.eggGroup1]))
                {
                    for (int n = 0; n < Pokemon::eEggGroup::EGG_ENUM_COUNT; n++)
                    {
                        const bool is_selected = (selectedSpecies.eggGroup1 == n);
                        if (ImGui::Selectable(Pokemon::EggGroup_Strings[n], is_selected) && n != Pokemon::EGG_NO_EGG_GROUP)
                        {
                            selectedSpecies.eggGroup1 = static_cast<Pokemon::eEggGroup>(n);
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                if (ImGui::BeginCombo("Egg Group 2", Pokemon::EggGroup_Strings[selectedSpecies.eggGroup2]))
                {
                    for (int n = 0; n < Pokemon::eEggGroup::EGG_ENUM_COUNT; n++)
                    {
                        const bool is_selected = (selectedSpecies.eggGroup2 == n);
                        if (ImGui::Selectable(Pokemon::EggGroup_Strings[n], is_selected))
                        {
                            selectedSpecies.eggGroup2 = static_cast<Pokemon::eEggGroup>(n);
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                ImGui::EndTable();
            }

            if (ImGui::TreeNode("Evolutions"))
            {
                for (unsigned int i = 0; i < selectedSpecies.evolutions.size(); i++)
                {
                    ImGui::PushID(selectedSpecies.evolutions[i].dexId);
                    RenderEvolutionData(selectedSpecies.evolutions[i]);
                    ImGui::PopID();
                    ImGui::Separator();
                }
                ImGui::TreePop();
            }

            ImGui::Separator();

            ImGui::Text("Default form");
            RenderFormData(selectedSpecies.defaultForm);
            ImGui::Separator();

            for (unsigned int i = 0; i < selectedSpecies.alternateForms.size(); i++)
            {
                std::string formTitle = selectedSpecies.alternateForms[i].name.str() + " form";
                ImGui::Text(formTitle.c_str());
                ImGui::PushID(formTitle.c_str());
                RenderFormData(selectedSpecies.alternateForms[i]);
                ImGui::PopID();
                ImGui::Separator();
            }

            if (ImGui::Button("Save Species Data"))
            {
                Pokemon::SaveSpecieData(selectedSpecies.nationalDexNumber, selectedSpecies);
            }
        }
    }

    if (ImGui::CollapsingHeader("Breeding Test"))
    {
        static int pkm1Dex;
        ImGui::DragInt("Parent1 Dex Num", &pkm1Dex, 1, 0, 1008);
        pkm1.nationalDexNumber = pkm1Dex;
        ImGui::SameLine();
        if (ImGui::Button("Randomize"))
        {
            pkm1.nationalDexNumber = rand() % 809 + 1;
            pkm1Dex = pkm1.nationalDexNumber;
        }

        static int pkm2Dex;
        ImGui::DragInt("Parent2 Dex Num", &pkm2Dex, 1, 0, 1008);
        pkm2.nationalDexNumber = pkm2Dex;
        ImGui::SameLine();
        if (ImGui::Button("Rand0mize"))
        {
            pkm2.nationalDexNumber = rand() % 809 + 1;
            pkm2Dex = pkm2.nationalDexNumber;
        }

        if (pkm1.nationalDexNumber != 0)
        {
            ImGui::PushID("juh1");
            RenderIndividualData(pkm1);
            ImGui::PopID();
            ImGui::Separator();
        }

        if (pkm2.nationalDexNumber != 0)
        {
            ImGui::PushID("juh2");
            RenderIndividualData(pkm2);
            ImGui::PopID();
            ImGui::Separator();
        }

        if (pkm1.nationalDexNumber != 0 && pkm2.nationalDexNumber != 0)
        {
            if (ImGui::Button("B R E E D"))
            {
                child = Pokemon::sIndividualData();
                if (!Pokemon::GenerateChild(pkm1, pkm2, child))
                    child.nationalDexNumber = 0;
            }
            ImGui::Separator();
        }

        if (child.nationalDexNumber != 0)
        {
            RenderIndividualData(child);
        }
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void RenderFormData(Pokemon::sForm& form)
{
    ImGui::PushItemWidth(100);
    if (ImGui::BeginCombo("Type 1", Pokemon::Type_Strings[form.type1]))
    {
        for (int n = 0; n < Pokemon::eType::TYPE_ENUM_COUNT; n++)
        {
            const bool is_selected = (form.type1 == n);
            if (ImGui::Selectable(Pokemon::Type_Strings[n], is_selected) && n != Pokemon::NO_TYPE)
            {
                form.type1 = static_cast<Pokemon::eType>(n);
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    if (ImGui::BeginCombo("Type 2", Pokemon::Type_Strings[form.type2]))
    {
        for (int n = 0; n < Pokemon::eType::TYPE_ENUM_COUNT; n++)
        {
            const bool is_selected = (form.type2 == n);
            if (ImGui::Selectable(Pokemon::Type_Strings[n], is_selected))
            {
                form.type2 = static_cast<Pokemon::eType>(n);
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    int hp = form.baseStats.hp;
    int atk = form.baseStats.atk;
    int def = form.baseStats.def;
    int spAtk = form.baseStats.spAtk;
    int spDef = form.baseStats.spDef;
    int spd = form.baseStats.spd;

    ImGui::DragInt("HP", &hp, 1, 1, 150);
    ImGui::DragInt("Attack", &atk, 1, 1, 150);
    ImGui::DragInt("Defence", &def, 1, 1, 150);
    ImGui::DragInt("Special Attack", &spAtk, 1, 1, 150);
    ImGui::DragInt("Special Defence", &spDef, 1, 1, 150);
    ImGui::DragInt("Speed", &spd, 1, 1, 150);

    form.baseStats.hp = hp;
    form.baseStats.atk = atk;
    form.baseStats.def = def;
    form.baseStats.spAtk = spAtk;
    form.baseStats.spDef = spDef;
    form.baseStats.spd = spd;

    ImGui::DragInt("Height(dm)", &form.height, 1, 1, 250);
    ImGui::SameLine();
    ImGui::DragInt("Weight(hg)", &form.weight, 1, 1, 10000);

    if (ImGui::TreeNode("Learnset"))
    {
        ImGui::BeginTable("", 2, ImGuiTableFlags_Borders);
        for (unsigned int i = 0; i < form.learnset.size(); i++)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            if (form.learnset[i].first == -3)
                ImGui::Text("Evo");
            else if (form.learnset[i].first == -2)
                ImGui::Text("Tutor");
            else if (form.learnset[i].first == -1)
                ImGui::Text("Egg");
            else if (form.learnset[i].first == 0)
                ImGui::Text("TM");
            else
                ImGui::Text(std::to_string(form.learnset[i].first).c_str());

            ImGui::TableNextColumn();
            ImGui::Text(std::to_string(form.learnset[i].second).c_str());
            ImGui::TableNextRow();
        }

        ImGui::EndTable();
        ImGui::TreePop();
    }
}

void RenderIndividualData(Pokemon::sIndividualData& data)
{
    std::string title = "DEX: " + std::to_string(data.nationalDexNumber);
    ImGui::Text(title.c_str());

    std::string form = "FORM: " + data.formOrVariantName.str();
    ImGui::Text(form.c_str());

    ImGui::Checkbox("Shiny", &data.isShiny);

    if (ImGui::BeginCombo("Nature", Pokemon::Natures_Strings[data.nature]))
    {
        for (int n = 0; n < Pokemon::eNature::NATURE_COUNT; n++)
        {
            const bool is_selected = (data.nature == n);
            if (ImGui::Selectable(Pokemon::Natures_Strings[n], is_selected))
            {
                data.nature = static_cast<Pokemon::eNature>(n);
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (ImGui::BeginCombo("Gender", Pokemon::Gender_Strings[data.gender]))
    {
        for (int n = 0; n < Pokemon::eGender::GENDER_ENUM_COUNT; n++)
        {
            const bool is_selected = (data.gender == n);
            if (ImGui::Selectable(Pokemon::Gender_Strings[n], is_selected))
            {
                data.gender = static_cast<Pokemon::eGender>(n);
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    int move1 = data.move1;
    int move2 = data.move2;
    int move3 = data.move3;
    int move4 = data.move4;
    ImGui::DragInt("Move 1", &move1);
    ImGui::DragInt("Move 2", &move2);
    ImGui::DragInt("Move 3", &move3);
    ImGui::DragInt("Move 4", &move4);
    data.move1 = move1;
    data.move2 = move2;
    data.move3 = move3;
    data.move4 = move4;
}

void RenderEvolutionData(Pokemon::sEvolution& data)
{
    int dexNum = data.dexId;
    ImGui::DragInt("Dex Num", &dexNum, 1, 0, 810);
    data.dexId = dexNum;

    ImGui::DragInt("Min Level", &data.minLevel, 1, 0, 100);
    ImGui::Checkbox("Firendship", &data.friendship);

    ImGui::DragInt("Use Item", &data.useItem, 1, 0, 10500);
    ImGui::DragInt("Held Item", &data.heldItem, 1, 0, 10500);

    int moveKnown = data.knownMoveId;
    ImGui::DragInt("Move Known", &moveKnown, 1, 0, 10500);
    data.knownMoveId = moveKnown;
    
    if (ImGui::BeginCombo("Day Time", Pokemon::DayTyme_Strings[data.dayTime]))
    {
        for (int n = 0; n < Pokemon::eDayTime::DAY_TIME_ENUM_COUNT; n++)
        {
            const bool is_selected = (data.dayTime == n);
            if (ImGui::Selectable(Pokemon::DayTyme_Strings[n], is_selected))
            {
                data.dayTime = static_cast<Pokemon::eDayTime>(n);
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (ImGui::BeginCombo("Gender", Pokemon::Gender_Strings[data.gender]))
    {
        for (int n = 0; n < Pokemon::eGender::GENDER_ENUM_COUNT; n++)
        {
            const bool is_selected = (data.gender == n);
            if (ImGui::Selectable(Pokemon::Gender_Strings[n], is_selected))
            {
                data.gender = static_cast<Pokemon::eGender>(n);
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}
