#pragma once

#include "cUIManager.h"

#include <memory>
#include "cAnimation.h"

class cMenuButtonWidget : public cUIImage
{
public:
	cMenuButtonWidget(cUICanvas* canvas, std::string text, std::string iconFileName);
	virtual ~cMenuButtonWidget() {};
};

class cOverworldCanvas : public cUICanvas
{
	cMenuButtonWidget* pokedexButton;
	cMenuButtonWidget* pokemonButton;
	cMenuButtonWidget* bagButton;
	cMenuButtonWidget* optionsButton;
	cMenuButtonWidget* saveButton;

public:
	cOverworldCanvas();
	virtual ~cOverworldCanvas() {};

	virtual void ConfirmAction();
	virtual void CancelAction();
};

//class cHealthBar : public cUIImage
//{
//	cUIImage* hbContent;
//public:
//	cHealthBar(cUICanvas* canvas, float initialRatio);
//	virtual ~cHealthBar() {};
//
//	void UpdateHealthBar(float healthPercent);
//};

//class cPlayerBattleInfo : public cUIWidget
//{
//	cHealthBar* playerHb;
//public:
//	cPlayerBattleInfo(cUICanvas* canvas);
//	virtual ~cPlayerBattleInfo() {};
//
//	void UpdatePlayerInfo();
//};

//class cEnemyBattleInfo : public cUIWidget
//{
//	cHealthBar* enemyHb;
//public:
//	cEnemyBattleInfo(cUICanvas* canvas);
//	virtual ~cEnemyBattleInfo() {};
//
//	void UpdateEnemyInfo();
//};
//
//class cBattleCanvas : public cUICanvas
//{
//	cUIImage* fightButton;
//	cUIImage* pokemonButton;
//	cUIImage* bagButton;
//	cUIImage* runButton;
//
//	cPlayerBattleInfo* pbi;
//	cEnemyBattleInfo* ebi;
//
//public:
//	cBattleCanvas();
//	virtual ~cBattleCanvas() {};
//
//	virtual void ConfirmAction();
//	virtual void CancelAction();
//};

class cPartyMemberButton : public cUIImage
{
public:
	cPartyMemberButton(cUICanvas* canvas, int memberNum);
	virtual ~cPartyMemberButton() {};
};

class cAnimatedBackground : public cUIImage
{
	std::shared_ptr<cVec2Animation> scroll;
public:
	cAnimatedBackground(cUICanvas* canvas);
	virtual ~cAnimatedBackground();
};

class cPartyCanvas : public cUICanvas
{
	cPartyMemberButton* member1 = nullptr;
	cPartyMemberButton* member2 = nullptr;
	cPartyMemberButton* member3 = nullptr;
	cPartyMemberButton* member4 = nullptr;
	cPartyMemberButton* member5 = nullptr;
	cPartyMemberButton* member6 = nullptr;

	int memberSelected = 0;

public:
	cPartyCanvas();
	virtual ~cPartyCanvas() {};

	virtual void ConfirmAction();
	virtual void CancelAction();
};

class cDialogCanvas : public cUICanvas
{
	cUIText* textWidget;
	cUIImage* optionsWidget;
	cUIWidget* optionsContainer;
	std::vector<cUIText*> options;
	unsigned int hoveredOptionIndex = 0;

public:
	cDialogCanvas();
	virtual ~cDialogCanvas() {};

	virtual void ConfirmAction() override;
	virtual void MoveAction(const eDirection dir) override;

	void UpdateText(const std::string& newText);
	void ShowOptions(const std::vector<std::string>& options);
};