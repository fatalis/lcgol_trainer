#pragma once

#include "Input.h"
#include "IRenderable.h"
#include "Util.h"

class MenuItem
{
public:
	explicit MenuItem(const std::string& text)
	{
		m_text = text;
		m_selectable = false;
	}
	virtual ~MenuItem() { };

	const std::string& GetText() const
	{
		return m_text;
	}

	bool IsSelectable() const
	{
		return m_selectable;
	}

protected:
	bool m_selectable;
	std::string m_text;
};

class BooleanMenuItem : public MenuItem
{
public:
	BooleanMenuItem(const std::string& text,
	                const std::function<void(bool on)>& set_callback = nullptr,
	                const std::function<bool()>& get_callback = nullptr)
	    : MenuItem(text)
	{
		m_on = false;
		m_set_callback = set_callback;
		m_get_callback = get_callback;
		m_selectable = true;
	}

	bool IsOn() const
	{
		return m_get_callback ? m_get_callback() : m_on;
	}

	bool Toggle()
	{
		m_on = !this->IsOn();
		if (m_set_callback)
			m_set_callback(m_on);
		return m_on;
	}

private:
	bool m_on;
	std::function<void(bool on)> m_set_callback;
	std::function<bool()> m_get_callback;
};

class ActionMenuItem : public MenuItem
{
public:
	ActionMenuItem(const std::string& text,
		const std::function<void()>& callback)
		: MenuItem(text)
	{
		m_callback = callback;
		m_selectable = true;
	}

	void DoAction() const
	{
		m_callback();
	}

private:
	std::function<void()> m_callback;
};

class ChoiceMenuItem : public MenuItem
{
public:
	struct MenuChoice
	{
		int id;
		std::string name;
	};

	ChoiceMenuItem(const std::string& text, const std::vector<MenuChoice>& choices, const std::function<void(int choice)>& set_callback = nullptr,
		const std::function<int()>& get_callback = nullptr) : MenuItem(text), m_choices(choices)
	{
		m_selectable = true;
		m_selected_choice = m_choices.front();
		m_set_callback = set_callback;
		m_get_callback = get_callback;
	}

	int GetChoice() const
	{
		return m_get_callback ? m_get_callback() : m_selected_choice.id;
	}

	const MenuChoice& Next();
	const MenuChoice& Previous();

private:
	MenuChoice m_selected_choice;
	const std::vector<MenuChoice> m_choices;
	std::function<void(int choice)> m_set_callback;
	std::function<int()> m_get_callback;
};

class Menu : public IRenderable
{
public:
	void Draw() override;

protected:
	Menu(std::shared_ptr<hl::Drawer> drawer, std::shared_ptr<Input> input);
	std::vector<std::shared_ptr<MenuItem>> m_menu_items;
	std::shared_ptr<MenuItem> m_selected_item;
	std::string m_name = "Menu";
	bool m_visible = false;

private:
	std::shared_ptr<Input> m_input;
	std::unique_ptr<ID3DXFont, Util::COMDeleter> m_font; // TODO: test com deleter
	std::unique_ptr<ID3DXLine, Util::COMDeleter> m_line;
};
