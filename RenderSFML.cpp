#include <SFML/Graphics.hpp>
#include <iostream>

#include "RenderSFML.h"

sf::Vector2i const mySFML::screenDimensions(800, 600);

mySFML::mySFML() {

	if (!font.loadFromFile("font.ttf")) {
		Error = "Could not load font file!";
		return;
	}

	text.setFont(font);
	
	text.setFillColor(sf::Color::White);
	text.setCharacterSize(15);

	text.setPosition(445, 50);

	// Create pal "cursor"
	palCursor.setSize(sf::Vector2f(16 * graphics.BlockDimension.x, graphics.BlockDimension.y));
	palCursor.setFillColor(sf::Color::Transparent);
	palCursor.setOutlineThickness(5);
	palCursor.setOutlineColor(sf::Color::Red);

	palCursor.setPosition(graphics.BlockPosition.x, graphics.BlockPosition.y);

	palText.setString("Current pal number : 0");
	palText.setFont(font);
	palText.setCharacterSize(12);
	palText.setFillColor(sf::Color::Red);
	palText.setPosition(graphics.BlockPosition.x, graphics.BlockPosition.y - 30);

	infoText = "No GFX File loaded.\n\n";
	text.setString(infoText);
}

void mySFML::OpenGFXFile(std::string Filename) {

	// Open Graphics file
	if (!graphics.open(Filename)) {
		Error = "Error loading GFX file!";
		return;
	}

	bool dot = false;
	size_t i = Filename.length()-1;
	GFXFileName = "";

	while (Filename[i] != '\\') {
		if (dot) GFXFileName = Filename[i] + GFXFileName;
		else if (Filename[i] == '.') dot = true;
		i--;
	}

	sf::Sprite tempSprite;

	// Draw image
	texture.loadFromImage(graphics.draw(curPal));
	tempSprite.setTexture(texture);
	tempSprite.setScale(sf::Vector2f(ImageScale, ImageScale)); // Make image bigger

	sprite = tempSprite;

	// Text
	infoText = "Press 'S' to save an image.\n\nPress Up/Down to change palette colors.\n\n";
	if (sprite.getGlobalBounds().height > screenDimensions.y) infoText += "Press Pg Up/Pg Down to scroll GFX.";
	text.setString(infoText);


}


void mySFML::OpenPalFile(std::string Filename) {

	// Open palette file
	if (!graphics.openPal(Filename)) {
		Error = "Error loading palette file!\n";
		return;
	}

	palCursor.setPosition(graphics.BlockPosition.x, graphics.BlockPosition.y);
	curPal = 0;
	palText.setString("Current pal number : 0");

	if (graphics.BGcolor.r+graphics.BGcolor.b+graphics.BGcolor.g >= TOO_WHITE) text.setFillColor(sf::Color::Black);
	else text.setFillColor(sf::Color::White);

	texture.update(graphics.draw(curPal));
}

void mySFML::Update(sf::RenderWindow& window)
{

	window.clear(graphics.BGcolor);
	graphics.drawPal(window); // Draw palette
	window.draw(palCursor);
	window.draw(text);
	window.draw(sprite);
	window.draw(palText);
	window.display();

}

void mySFML::PageGFX(bool direction) {

	if (!direction && sprite.getPosition().y < 0)
		sprite.move(0, scrollSpeed);
	else if (direction && (sprite.getPosition().y*-1) < screenDimensions.y && sprite.getGlobalBounds().height > screenDimensions.y)
		sprite.move(0, -scrollSpeed);
}

void mySFML::SPressed(bool press) {
	
	if(press) text.setString(infoText); 
	else if (graphics.savetoImage(GFXFileName)) 
		text.setString(infoText + "\n\n\nImage saved succesfully!");
	else
		text.setString(infoText + "\n\n\nError: couldn't save image!");
}

void mySFML::HandlePalCursor(bool direction) {

	if (direction && curPal < 240 * 3) {
		curPal += 16 * 3;
		palCursor.move(0, graphics.BlockDimension.y);
	}
	else if (!direction && curPal)
	{
		curPal -= 16 * 3;
		palCursor.move(0, -graphics.BlockDimension.y);
	}
	else return;

	// Update the graphics
	std::string hex[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F" };
	palText.setString("Current pal number: " + hex[curPal / (16 * 3)]);
	texture.update(graphics.draw(curPal));


	if (graphics.BGcolor.r + graphics.BGcolor.b + graphics.BGcolor.g >= TOO_WHITE) text.setFillColor(sf::Color::Black);
	else text.setFillColor(sf::Color::White);
}

