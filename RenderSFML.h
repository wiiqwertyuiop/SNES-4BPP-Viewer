#pragma once

#include "GFX.h"

// Color that is too white for text (so text changes color)
#define TOO_WHITE 0x2A0

class mySFML {

	const float ImageScale = 3; // Resize GFX image
	const float scrollSpeed = 30; // Scroll speed if GFX don't the fit screen

	sf::Font font;
	sf::Texture texture;
	sf::Sprite sprite;
	sf::Text text;

	GFX graphics;
	int curPal = 0; // Current palette we are using
	std::string infoText;

	sf::Text palText;
	sf::RectangleShape palCursor;

	sf::Vector2i static const screenDimensions;

public:

	std::string GFXFileName;
	std::string Error;

	mySFML();
	void OpenGFXFile(std::string);
	void OpenPalFile(std::string);
	void Update(sf::RenderWindow&);
	void PageGFX(bool);
	void SPressed(bool);
	void HandlePalCursor(bool);
};

