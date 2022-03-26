
#include "Rendering.h"
#include "Exports/paf/View.h"
#include "../Core/Helpers.h"
#include "../Utilities/System.h"
#include <vsh/stdc.h>

CRender g_Render;

void CRender::OnUpdate()
{
	if (!g_Helpers.system_plugin)
		return;

	if (!g_Helpers.page_autooff_guide)
	{
		DestroyPlanesAndTexts();
		return;
	}

	paf::PhPlane* phPlane = GetPlane(0);

	if (phPlane)
	{
		if (!phPlane->IsAttached())
		{
			DestroyPlanesAndTexts();
			return;
		}
	}
	else
	{
		CreatePlanesAndTexts();
		return;
	}

	m_CurrentPlane = 0;
	m_CurrentText = 0;

	ClearPlanes();
	ClearTexts();
}

void CRender::Rectangle(vec2 position, vec2 size, Align horizontalAlign, Align verticalAlign, Color color, const std::string& texture, float angle)
{
	paf::PhPlane* phPlane = GetPlane(m_CurrentPlane);
	if (!phPlane)
		return;

	if (!phPlane->IsAttached())
		return;

	phPlane->SetPosition(position)
		.SetSize(size)
		.SetColor(color)
		.SetStyle(paf::PhWidget::Anchor, int(horizontalAlign | verticalAlign))
		.SetRotation(angle, false)
		.SetSystemTexture(texture);

	m_CurrentPlane++;
}

void CRender::RectangleGradient(vec2 position, vec2 size, Align horizontalAlign, Align verticalAlign, Color colorLeft, Color colorRight, float angle)
{
	Rectangle(position, size, horizontalAlign, verticalAlign, colorLeft, "tex_optionmenu_game");
	Rectangle(position, size, horizontalAlign, verticalAlign, colorRight, "tex_optionmenu_game", 180);
}

// Untested
void CRender::Line(vec2 from, vec2 to, float thickness, Color color)
{
	float width, height, hypotenuse, x, y, angle;

	width = to.x - from.x;
	height = to.y - from.y;
	hypotenuse = stdc::sqrtf(width * width + height * height);

	x = from.x + ((width - hypotenuse) / 2);
	y = from.y + (height / 2) - (thickness / 2);
	angle = Math::degrees(atan2f(height, width));

	Rectangle(vec2(x, y), vec2(hypotenuse, thickness), Left, Top, color, "", angle);
}

void CRender::Text(const std::wstring& text, vec2 position, float height, Align horizontalAlign, Align verticalAlign, Color color, float angle)
{
	paf::PhText* phText = GetText(m_CurrentText);
	if (!phText)
		return;

	if (!phText->IsAttached())
		return;

	position.y += (height * 0.15);

	phText->SetText(text)
		.SetTextHeight(height * 0.8)
		.SetPosition(position)
		.SetColor(color)
		.SetStyle(paf::PhWidget::Anchor, int(horizontalAlign | verticalAlign))
		.SetStyle(paf::PhWidget::TextAlignment, int(horizontalAlign))
		.SetRotation(angle, false);

	m_CurrentText++;
}

void CRender::Text(const std::string& text, vec2 position, float height, Align horizontalAlign, Align verticalAlign, Color color, float angle)
{
	paf::PhText* phText = GetText(m_CurrentText);
	if (!phText)
		return;

	if (!phText->IsAttached())
		return;

	position.y += (height * 0.15);

	phText->SetText(text)
		.SetTextHeight(height * 0.8)
		.SetPosition(position)
		.SetColor(color)
		.SetStyle(paf::PhWidget::Anchor, int(horizontalAlign | verticalAlign))
		.SetStyle(paf::PhWidget::TextAlignment, int(horizontalAlign))
		.SetRotation(angle, false);

	m_CurrentText++;
}

paf::PhPlane* CRender::CreatePlane(const std::string& widgetName)
{
	if (!g_Helpers.page_autooff_guide)
		return nullptr;

	paf::PhPlane* phPlane = new paf::PhPlane(g_Helpers.page_autooff_guide);
	phPlane->SetName(widgetName)
		.SetColor(Color());
	return phPlane;
}

paf::PhText* CRender::CreateText(const std::string& widgetName)
{
	if (!g_Helpers.page_autooff_guide)
		return nullptr;

	paf::PhText* phText = new paf::PhText(g_Helpers.page_autooff_guide);
	phText->SetName(widgetName)
		.SetColor(Color())
		.SetStyle(0x13, 0x70);
	return phText;
}

paf::PhPlane* CRender::GetPlane(int index)
{
	if (m_PlaneList.empty())
		return nullptr;

	if (index > MAX_PLANES || index < 0)
		return nullptr;

	return m_PlaneList[index];
}

paf::PhText* CRender::GetText(int index)
{
	if (m_TextList.empty())
		return nullptr;

	if (index > MAX_TEXTS || index < 0)
		return nullptr;

	return m_TextList[index];
}

void CRender::CreatePlanesAndTexts()
{
	Math::srand(Timers::GetTimeNow());
	for (int i = 0; i < MAX_PLANES; i++)
	{
		// Using a randomized name add compatibility with other menus if they use the same name template
		paf::PhPlane* phPlane = CreatePlane(stdc::va("CRenderPlane_%i", Math::rand()));
		if (!phPlane)
			break;

		m_PlaneList.push_back(phPlane);
	}

	Math::srand(Timers::GetTimeNow());
	for (int i = 0; i < MAX_TEXTS; i++)
	{
		paf::PhText* phText = CreateText(stdc::va("CRenderText_%i", Math::rand()));
		if (!phText)
			break;

		m_TextList.push_back(phText);
	}
}

void CRender::DestroyPlanesAndTexts()
{
	for (int i = 0; i < MAX_PLANES; i++)
	{
		paf::PhPlane* phPlane = GetPlane(i);
		if (!phPlane)
			continue;

		delete phPlane;
	}

	for (int i = 0; i < MAX_TEXTS; i++)
	{
		paf::PhText* phText = GetText(i);
		if (!phText)
			continue;

		delete phText;
	}

	m_PlaneList.clear();
	m_TextList.clear();
}

void CRender::ClearPlanes()
{
	for (int i = 0; i < MAX_PLANES; i++)
	{
		paf::PhPlane* phPlane = GetPlane(i);
		if (!phPlane)
			continue;

		if (!phPlane->IsAttached())
			continue;

		phPlane->SetColor(Color());
	}
}

void CRender::ClearTexts()
{
	for (int i = 0; i < MAX_TEXTS; i++)
	{
		paf::PhText* phText = GetText(i);
		if (!phText)
			continue;

		if (!phText->IsAttached())
			continue;

		phText->SetColor(Color());
	}
}