#include <string>
#include <map>
#include <cassert>

#include "Menu.h"

Menu::Menu(std::shared_ptr<hl::Drawer> drawer, std::shared_ptr<Input> input) : IRenderable(drawer)
{
	m_input = input;

	LPD3DXFONT font;
	assert(SUCCEEDED(D3DXCreateFontW(drawer->GetDevice(), 20, 0, FW_NORMAL, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas", &font)));
	m_font.reset(font);

	LPD3DXLINE line;
	assert(SUCCEEDED(D3DXCreateLine(drawer->GetDevice(), &line)));
	m_line.reset(line);
}

const ChoiceMenuItem::MenuChoice& ChoiceMenuItem::Next()
{
	auto end = std::end(m_choices);
	auto it = std::find_if(std::begin(m_choices), end, [this](const auto& choice) {
		return choice.id == m_selected_choice.id;
	});
	if (it != end)
	{
		m_selected_choice = ++it == end ? m_choices.front() : *it;
		if (m_set_callback)
			m_set_callback(m_selected_choice.id);
	}
	return m_selected_choice;
}

const ChoiceMenuItem::MenuChoice& ChoiceMenuItem::Previous()
{
	auto end = std::rend(m_choices);
	auto it = std::find_if(std::rbegin(m_choices), end, [this](const auto& choice) {
		return choice.id == m_selected_choice.id;
	});
	if (it != end)
	{
		m_selected_choice = ++it == end ? m_choices.back() : *it;
		if (m_set_callback)
			m_set_callback(m_selected_choice.id);
	}
	return m_selected_choice;
}

void Menu::Draw()
{
	assert(m_selected_item.get() != nullptr);

	if (m_input->WasPressed(Input::Button::Special))
		m_visible = !m_visible;

	if (!m_visible)
		return;

	// update state
	{
		if (m_input->WasPressed(Input::Button::Up))
		{
			// move selected item up
			auto end = std::rend(m_menu_items);
			auto it = std::find(std::rbegin(m_menu_items), end, m_selected_item);
			if (it != end)
			{
				it = std::find_if(++it, end, [](const auto& item) { return item->IsSelectable(); });
				if (it != end)
					m_selected_item = *it;
			}
		}
		else if (m_input->WasPressed(Input::Button::Down))
		{
			// move selected item down
			auto end = std::end(m_menu_items);
			auto it = std::find(std::begin(m_menu_items), end, m_selected_item);
			if (it != end)
			{
				it = std::find_if(++it, end, [](const auto& item) { return item->IsSelectable(); });
				if (it != end)
					m_selected_item = *it;
			}
		}
		else if (m_input->WasPressed(Input::Button::Left))
		{
			// cycle choices left
			if (auto choice_item = dynamic_cast<ChoiceMenuItem*>(m_selected_item.get()))
				choice_item->Previous();
		}
		else if (m_input->WasPressed(Input::Button::Right))
		{
			// cycle choices right
			if (auto choice_item = dynamic_cast<ChoiceMenuItem*>(m_selected_item.get()))
				choice_item->Next();
		}

		if (m_input->WasPressed(Input::Button::Enter))
		{
			if (auto bool_item = dynamic_cast<BooleanMenuItem*>(m_selected_item.get())) 
				bool_item->Toggle();
			else if (auto action_item = dynamic_cast<ActionMenuItem*>(m_selected_item.get()))
				action_item->DoAction();
			else if (auto choice_item = dynamic_cast<ChoiceMenuItem*>(m_selected_item.get()))
				choice_item->Next();
		}
	}

	std::map<MenuItem*, std::string> values;
	for (auto& item : m_menu_items)
	{
		auto bool_item = dynamic_cast<BooleanMenuItem*>(item.get());
		if (bool_item)
		{
			values.emplace(item.get(), bool_item->IsOn() ? "ON" : "OFF");
			continue;
		}
	}

	// calculate menu bounds
	const auto margin = 12;
	const auto name_value_seperator = 32;
	auto name_width = 0;
	auto height = 0;
	// name
	for (auto& item : m_menu_items)
	{
		RECT rect { 0, 0, 0, 0 };
		height += m_font->DrawTextA(nullptr, item->GetText().c_str(), item->GetText().length(), &rect, DT_CALCRECT, 0);
		if (rect.right > name_width)
			name_width = rect.right;
	}
	// value
	auto value_width = 0;
	for (auto& value : values)
	{
		RECT rect{ 0, 0, 0, 0 };
		m_font->DrawTextA(nullptr, value.second.c_str(), value.second.length(), &rect, DT_CALCRECT, 0);
		if (rect.right > value_width)
			value_width = rect.right;
	}
	auto width = name_width + value_width + margin * 2 + name_value_seperator;
	height += margin * 2;

	// draw background
	D3DVIEWPORT9 viewport;
	m_drawer->GetDevice()->GetViewport(&viewport);

	auto menu_pos = POINTFLOAT{ viewport.Width / 2.0f - width / 2.0f, viewport.Height / 4.0f };
	auto menu_text_pos = POINTFLOAT{ menu_pos.x + margin, menu_pos.y + margin };
	auto menu_value_pos = POINTFLOAT{ menu_text_pos.x + name_width + name_value_seperator, menu_text_pos.y };
	m_drawer->DrawRectFilled(menu_pos.x, menu_pos.y, static_cast<float>(width), static_cast<float>(height), 0xFF'27'28'22);

	const auto line_width = 8;
	constexpr auto half_line = line_width / 2;
	// background outline
	D3DXVECTOR2 background_verts[5] = {
		{ menu_pos.x - half_line, menu_pos.y - half_line },
		{ menu_pos.x + width + half_line, menu_pos.y - half_line },
		{ menu_pos.x + width + half_line, menu_pos.y + height + half_line },
		{ menu_pos.x - half_line, menu_pos.y + height + half_line },
		{ menu_pos.x - half_line, menu_pos.y - half_line },
	};
	m_line->SetWidth(static_cast<float>(line_width));
	m_line->Draw(&background_verts[0], ARRAYSIZE(background_verts), 0x55'44'44'44);

	// draw text
	auto y = static_cast<LONG>(menu_text_pos.y);
	for (auto& item : m_menu_items)
	{
		// draw name
		RECT rect{ static_cast<LONG>(menu_text_pos.x), y, 0, 0 };
		auto h = m_font->DrawTextA(nullptr, item->GetText().c_str(), item->GetText().length(), &rect, DT_NOCLIP, item == m_selected_item ? 0xFFFD971F : 0xfff8f8f2);

		// draw value
		auto it = values.find(item.get());
		if (it != values.end())
		{
			auto str = (*it).second; // how2 not copy
			auto color = 0xfff8f8f2;
			if (str == "ON")
				color = 0xFF'A6'E2'2E;
			else if (str == "OFF")
				color = 0xFF'F9'26'72;
			rect = { static_cast<LONG>(menu_value_pos.x), y, 0, 0 };
			m_font->DrawTextA(nullptr, str.c_str(), str.length(), &rect, DT_NOCLIP, color);
		}

		y += h;
	}
}
