#include "UIWidgets.h"

#include <glad/glad.h>

#include "Engine.h"
#include "cUIManager.h"
#include "cCameraManager.h"
#include "cRenderManager.h"

#include "cAnimation.h"

cUIWidget::~cUIWidget()
{
	for (int i = 0; i < children.size(); i++)
	{
		delete children[i];
	}
}

void cUIWidget::Draw()
{
	if (isHidden) return;

	for (int i = 0; i < children.size(); i++)
	{
		children[i]->Draw();
	}
}

void cUIWidget::AddChild(cUIWidget* newChild)
{
	newChild->parent = this;
	children.push_back(newChild);
}

const float cUIWidget::CalculateHeightPixels()
{
	if (!parent) return heightPercent * Manager::camera.SCR_HEIGHT;

	return heightPercent * parent->CalculateHeightPixels();
}

const float cUIWidget::CalculateHeightScreenPercent()
{
	if (!parent) return heightPercent;

	float heightInPixels = parent->CalculateHeightPixels() * heightPercent;
	return heightInPixels / Manager::camera.SCR_HEIGHT;
}

const float cUIWidget::CalculateWidthPixels()
{
	return CalculateHeightPixels() / aspectRatio;
}

const float cUIWidget::CalculateWidthScreenPercent()
{
	return CalculateWidthPixels() / Manager::camera.SCR_WIDTH;
}

const float cUIWidget::CalculateVerticalTranslate()
{
	float parentVerticalTranslation = 0.f;
	float parentHeightPixels = (float)Manager::camera.SCR_HEIGHT;
	if (parent)
	{
		parentVerticalTranslation = parent->CalculateVerticalTranslate();
		parentHeightPixels = parent->CalculateHeightPixels();
	}

	float thisPercentTranslate = verticalTranslate * parentHeightPixels / (float)Manager::camera.SCR_HEIGHT * 2.f; // pixels to translate this 

	// No vertical translating compared to parent
	if (anchor == MIDDLE_LEFT || anchor == MIDDLE_MIDDLE || anchor == MIDDLE_RIGHT)
	{
		return parentVerticalTranslation + thisPercentTranslate;
	}

	float anchorPixelTranslate = parentHeightPixels - CalculateHeightPixels(); // assuming top
	float anchorPercentTranslate = anchorPixelTranslate / (float)Manager::camera.SCR_HEIGHT;

	if (anchor == BOTTOM_LEFT || anchor == BOTTOM_MIDDLE || anchor == BOTTOM_RIGHT) anchorPercentTranslate *= -1.f;

	return parentVerticalTranslation + anchorPercentTranslate + thisPercentTranslate;
}

const float cUIWidget::CalculateHorizontalTranslate()
{
	float parentHorizontalTranslation = 0.f;
	float parentWidthPixels = (float)Manager::camera.SCR_WIDTH;
	if (parent)
	{
		parentHorizontalTranslation = parent->CalculateHorizontalTranslate();
		parentWidthPixels = parent->CalculateWidthPixels();
	}

	float thisPercentTranslate = horizontalTranslate * parentWidthPixels / (float)Manager::camera.SCR_WIDTH * 2.f; // pixels to translate this 

	if (anchor == TOP_MIDDLE || anchor == MIDDLE_MIDDLE || anchor == BOTTOM_MIDDLE)
	{
		return parentHorizontalTranslation + thisPercentTranslate;
	}

	float anchorPixelTranslate = parentWidthPixels - CalculateWidthPixels();
	float anchorPercentTranslate = anchorPixelTranslate / (float)Manager::camera.SCR_WIDTH;

	if (anchor == TOP_LEFT || anchor == MIDDLE_LEFT || anchor == BOTTOM_LEFT) anchorPercentTranslate *= -1.f;

	return parentHorizontalTranslation + anchorPercentTranslate + thisPercentTranslate;
}

void cUIWidget::SetMoveFocus(cUIWidget* to, eDirection dir, bool isViceVersa)
{
	if (!to) return;

	switch (dir)
	{
	case UP:
		focusUp = to;
		break;
	case DOWN:
		focusDown = to;
		break;
	case LEFT:
		focusLeft = to;
		break;
	case RIGHT:
		focusRight = to;
		break;
	default:
		break;
	}

	if (isViceVersa)
	{
		switch (dir)
		{
		case UP:
			to->focusDown = this;
			break;
		case DOWN:
			to->focusUp = this;
			break;
		case LEFT:
			to->focusRight = this;
			break;
		case RIGHT:
			to->focusLeft = this;
			break;
		default:
			break;
		}
	}
}

cUIWidget* cUIWidget::GetAdjecentFocus(eDirection dir)
{
	cUIWidget* adjecent = nullptr;

	switch (dir)
	{
	case UP:
		adjecent = focusUp;
		break;
	case DOWN:
		adjecent = focusDown;
		break;
	case LEFT:
		adjecent = focusLeft;
		break;
	case RIGHT:
		adjecent = focusRight;
		break;
	default:
		break;
	}

	return adjecent;
}

void cUIWidget::EnterFocus()
{
	isFocused = true;
	for (int i = 0; i < children.size(); i++)
	{
		children[i]->EnterFocus();
	}
}

void cUIWidget::LeaveFocus()
{
	isFocused = false;
	for (int i = 0; i < children.size(); i++)
	{
		children[i]->LeaveFocus();
	}
}

void cUIWidget::ConfirmAction()
{
}

cUIImage::~cUIImage()
{
}

void cUIImage::Draw()
{
	cUIWidget::Draw();

	if (textureId == 0) return;

	unsigned int textureIdToUse = !isFocused || hoveredTextureId == 0 ? textureId : hoveredTextureId;

	Manager::render.use("ui");
	
	unsigned int shaderTextureUnit = 0;
	glActiveTexture(shaderTextureUnit + GL_TEXTURE0); // GL_TEXTURE0 = 33984
	glBindTexture(GL_TEXTURE_2D, textureIdToUse);
	std::string shaderVariable = "texture_" + std::to_string(shaderTextureUnit);
	Manager::render.setInt(shaderVariable, shaderTextureUnit);

	// OPTMIZATION: calculate these values once and store them and/or make them into a matrix
	float widthPercent = CalculateWidthScreenPercent();
	float heightPercent = CalculateHeightScreenPercent();
	Manager::render.setFloat("widthPercent", widthPercent);
	Manager::render.setFloat("heightPercent", heightPercent);

	float widthTranslate = CalculateHorizontalTranslate();
	float heightTranslate = CalculateVerticalTranslate();
	Manager::render.setFloat("widthTranslate", widthTranslate);
	Manager::render.setFloat("heightTranslate", heightTranslate);

	if (useScreenSpace)
	{
		float screenAspectRatio = (float)Manager::camera.SCR_HEIGHT / (float)Manager::camera.SCR_WIDTH;
		glm::vec2 rate = glm::vec2(screenSpaceRatio.x / screenAspectRatio, screenSpaceRatio.y);
		Manager::render.setVec2("screenSpaceRatio", rate);
		Manager::render.setVec2("textureTranslate", translate);
	}
	else
	{
		Manager::render.setVec2("screenSpaceRatio", glm::vec2(1.f));
		Manager::render.setVec2("textureTranslate", glm::vec2(0.f));
	}

	Manager::render.setVec3("colorFilter", colorFilter);

	glBindVertexArray(Manager::ui.GetUIQuadVAO());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

cUIText::cUIText()
{
}

cUIText::~cUIText()
{
	if (dataBufferId != 0)
	{
		glDeleteBuffers(1, &dataBufferId);
	}
}

void cUIText::Draw()
{
	unsigned int scrWidth = Manager::camera.SCR_WIDTH;
	unsigned int scrHeight = Manager::camera.SCR_HEIGHT;
	float horizontalTranslation = CalculateHorizontalTranslate();
	float verticalTranslation = CalculateVerticalTranslate();
	float widthPercent = CalculateWidthScreenPercent();
	float heightPercent = CalculateHeightScreenPercent();

	float pixelGlyphRatio = heightPercent * scrHeight * textSizePercent / (float)Manager::ui.GetFontGlyphSize(fontName);

	float finalHorizontalTranslation = horizontalTranslation - widthPercent;
	float finalVerticalTranslation = verticalTranslation + heightPercent;
	glm::vec2 origin = glm::vec2(finalHorizontalTranslation, finalVerticalTranslation);

	Manager::render.use("text");
	Manager::ui.SetupFont(fontName);
	Manager::render.setVec3("color", color);

	Manager::render.setVec2("originOffset", origin);
	Manager::render.setFloat("glyphPixelRatio", pixelGlyphRatio);
	Manager::render.setInt("screenWidth", scrWidth);
	Manager::render.setInt("screenHeight", scrHeight);

	glBindVertexArray(Manager::ui.GetUIQuadVAO());

	// Setup buffer data as vertex atribute
	// (ideally I would want this to be set on VAO creation, but I guess the data needs to be setup before hand... so here it goes)
	glBindBuffer(GL_ARRAY_BUFFER, dataBufferId);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glVertexAttribDivisor(2, 1);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(4 * sizeof(float)));
	glVertexAttribDivisor(3, 1);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0, drawCharCount);

	glBindVertexArray(0);
}

cUIGrid::cUIGrid(unsigned int _width, unsigned int _height)
{
	width = _width;
	height = _height;
	children = std::vector<cUIWidget*>(width * height);
}

void cUIGrid::AssignChildAtIndex(cUIWidget* newChild, unsigned int index)
{
	if (index < 0 || index >= height * width)
		return;

	newChild->anchor = MIDDLE_MIDDLE;
	newChild->heightPercent = 1.f / (float)height;
	
	if (width % 2 == 0)
	{

	}
	else
	{
		horizontalTranslate = (1.f / (float)width) * ((index % 5) - (width / 2));
	}

	children[index] = newChild;
}

cUIWidget* cUIGrid::GetAdjecentFocus(eDirection dir)
{
	int desiredIndex = 0;
	switch (dir)
	{
	case UP:
		desiredIndex = focusIndex - width;
		if (desiredIndex < 0)
			return focusUp;
		break;
	case DOWN:
		desiredIndex = focusIndex + width;
		if (desiredIndex >= width * height)
			return focusDown;
		break;
	case LEFT:
		if (focusIndex % width == 0)
			return focusLeft;
		desiredIndex = focusIndex - 1;
		break;
	case RIGHT:
		if (focusIndex % width == width - 1)
			return focusRight;
		desiredIndex = focusIndex + 1;
		break;
	default:
		break;
	}

	focusIndex = desiredIndex;
	
	return this;
}

void cUIGrid::EnterFocus()
{
	isFocused = true;
	children[focusIndex]->EnterFocus();
	
	// Move position of cursor here
}

void cUIGrid::LeaveFocus()
{
	isFocused = false;
	children[focusIndex]->LeaveFocus();
}
