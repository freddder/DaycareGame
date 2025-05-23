#include "Player.h"
#include "cPlayerEntity.h"
#include "cTamedRoamingPokemon.h"
#include "PokemonData.h"

#include "Engine.h"
#include "cMapManager.h"

namespace Player 
{
    cPlayerEntity* playerChar;
    std::shared_ptr<cTamedRoamingPokemon> playerPartner;

    glm::vec3 GetPlayerPosition()
    {
        return playerChar->GetModelPosition();
    }

    glm::vec3* GetPlayerPositionRef()
    {
        return &playerChar->GetModelPositionRef();
    }

    Pokemon::sIndividualData party[6];

    void AddPartyMember(const Pokemon::sIndividualData& newPartyMember)
    {
        for (int i = 0; i < 6; i++)
        {
            if (party[i].nationalDexNumber == 0)
            {
                party[i] = newPartyMember;
                break;
            }
        }
    }

    void SwitchPartyMembers(const int member1, const int member2)
    {
        if (member1 < 1 || member1 > 6 ||
            member2 < 1 || member2 > 6 ||
            member1 == member2)
            return;

        if (party[member1 - 1].nationalDexNumber == 0 || party[member2 - 1].nationalDexNumber == 0)
            return;

        Pokemon::sIndividualData& switchMember1 = party[member1 - 1];
        Pokemon::sIndividualData& switchMember2 = party[member2 - 1];

        Pokemon::sIndividualData temp = switchMember1;
        switchMember1 = switchMember2;
        switchMember2 = temp;

        if (member1 == 1 || member2 == 1) // change the following tamed pokemon sprite
        {
            playerPartner.get()->UpdateRoamingData(party[0]);
        }
    }

    void AttemptInteract()
    {
        glm::ivec3 desiredPos = playerChar->position;

        if (playerChar->currFacingDir == eDirection::UP)
            desiredPos.x += 1;
        else if (playerChar->currFacingDir == eDirection::DOWN)
            desiredPos.x -= 1;
        else if (playerChar->currFacingDir == eDirection::LEFT)
            desiredPos.z -= 1;
        else if (playerChar->currFacingDir == eDirection::RIGHT)
            desiredPos.z += 1;

        sTile* facingTile = Manager::map.GetTile(desiredPos);
        if (facingTile && facingTile->entity)
        {
            facingTile->entity->PromptInteract();
        }
    }
}
