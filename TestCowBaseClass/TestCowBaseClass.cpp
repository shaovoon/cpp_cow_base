#include <iostream>
#include <string>
#include <memory>
#include "COWBase.h"

struct TextBoxImpl
{
    std::string m_Title;
    int m_Color;
};

class TextBox : private COWBase<TextBoxImpl>
{
public:
    TextBox()
    {
        construct();
    }
	TextBox(const std::string& title, int color)
	{
		construct();
        ptr()->m_Title = title;
        ptr()->m_Color = color;
	}
    int GetColor() const { return ptr()->m_Color; }
	const std::string& GetTitle() const { return ptr()->m_Title; }

    void SetColor(int color)
    {
        clone_if_needed();
        ptr()->m_Color = color;
    }
	void SetTitle(const std::string& title)
	{
		clone_if_needed();
		ptr()->m_Title = title;
	}
    void Display()
    {
		std::cout << "use_count:" << use_count() << ", Title: " << ptr()->m_Title << ", Color: " << ptr()->m_Color << "\n";
    }
};

int main()
{
    TextBox a("Hello", 1);
    a.Display();

    TextBox b = a;
    b.Display();

    
    b.SetColor(2);
	std::cout << "\nAfter setting color:2 on b\n";
	b.Display();

	b.SetTitle("world");
	std::cout << "\nAfter setting title:world on b\n";
	b.Display();

    return 0;
}