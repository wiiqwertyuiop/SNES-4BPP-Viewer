#include <SFML/Graphics.hpp>
#include <fstream>

#include<iostream>

#include "GFX.h"

// Open Palette file
bool GFX::openPal(std::string Filename) {

	std::ifstream file(Filename, std::ios::in | std::ios::binary);

	if (!file.is_open()) return 1;

	file.seekg(0, std::ios::beg);
	file.read((char *)pal, PaletteSize);
	file.close();

	BGcolor = sf::Color(sf::Color(pal[0], pal[1], pal[2]));
	return true;
}

sf::Vector2f const GFX::BlockDimension(22, 22);
sf::Vector2f const GFX::BlockPosition(442, 242);

// Draw Palette colors to screen
void GFX::drawPal(sf::RenderWindow& window) {

	for (int i = 0; i < PaletteSize; i += 3) {

		sf::VertexArray vArray(sf::PrimitiveType::Quads, 4);

		unsigned short int x = (i / 3) & 0x0F;
		unsigned short int y = ((i / 3) & 0xF0) >> 4;

		vArray[0].position = sf::Vector2f((x * BlockDimension.x) + BlockPosition.x, (y * BlockDimension.y) + BlockPosition.y);
		vArray[1].position = sf::Vector2f((x * BlockDimension.x + BlockDimension.x) + BlockPosition.x, (y * BlockDimension.y) + BlockPosition.y);
		vArray[2].position = sf::Vector2f((x * BlockDimension.x + BlockDimension.x) + BlockPosition.x, (y * BlockDimension.y + BlockDimension.y) + BlockPosition.y);
		vArray[3].position = sf::Vector2f((x * BlockDimension.x) + BlockPosition.x, (y * BlockDimension.y + BlockDimension.y) + BlockPosition.y);

		vArray[0].color = sf::Color(pal[i], pal[i + 1], pal[i + 2]);
		vArray[1].color = vArray[0].color;
		vArray[2].color = vArray[0].color;
		vArray[3].color = vArray[0].color;

		window.draw(vArray);
	}

}

// Open GFX file
bool GFX::open(std::string Filename) {

	std::ifstream File(Filename, std::ios::binary | std::ios::ate);

	delete[] GFXFile;

	if (!File.is_open()) return false;

	// Get file size
	int ActualFileSize = (int)File.tellg();
	GFXFileSize = ActualFileSize;

	// If the filesize isn't a multiple of 0x200, expand our GFX File so it is
	if (GFXFileSize % 0x200) GFXFileSize += (0x200 - (ActualFileSize % 0x200));

	// Calculate width and height of image
	width = (GFXFileSize % 0x200) / 4;
	height = ((GFXFileSize / 0x200) * 8);

	// Limit the width of the image to 128 if too big (or zero)
	if (!width || GFXFileSize > 0x200) width = 128;

	// Minimum height is 8 pixels
	if (!height) height = 8;

	// Read file into buffer
	File.seekg(0, File.beg);
	GFXFile = new char[GFXFileSize];
	File.read(GFXFile, ActualFileSize);
	File.close();

	return true;
}

// Draw GFX to screen
sf::Image GFX::draw(int palToUse) {

	// Create an array of pixels
	sf::Uint8 * pixels = new sf::Uint8[width * height * 4];

	int curPixel = 0;	// Keep track of which pixel we are drawing 

	for (int i = 0; (i + 0x11) < GFXFileSize; i += 2) {

		// Go to next bytes after we reach the end of the "line"
		if (!(i % 0x10) && i) 
			i += 0x10;

		// Read data
		unsigned short int byteOne = GFXFile[i];
		unsigned short int byteTwo = GFXFile[i + 1];
		unsigned short int byteThree = GFXFile[i + 0x10];
		unsigned short int byteFour = GFXFile[i + 0x11];

		/*
			Pixels are drawn line by line by in a 8x8 block,
			then moving on to the next block after we have finished one:

		 curPixel
			0     |    0  --------
			1	  |   128 --------
			2	  |   256 --------
			3	  |   etc.
			4
			5
			6
			7
			(move horizontaly to next block then)

		*/

		// Set us up for a row of pixels
		int pixelPos = width * (curPixel % 8);

		// Move horizontally 8 pixels after we draw an 8x8 block
		pixelPos += (curPixel / 8) * 8;

		// After we draw an entire 8x8 row of pixels, move us down to the next row
		pixelPos += (curPixel / 128) * (128 * 7); // (0 = 1st row, 1 = 2nd row, etc.)

		pixelPos *= 4; // pixel data is 4 bytes long (RGBA)

		if (pixelPos + 8 > (width*height * 4)) break;

		// Place pixels
		for (unsigned short int j = 0b10000000; j > 0; j >>= 1) {

			unsigned short int color = 0;

			if (byteOne&j) color = 1;
			if (byteTwo&j) color |= 0b00000010;
			if (byteThree&j) color |= 0b00000100;
			if (byteFour&j) color |= 0b00001000;
			color *= 3;
			color += palToUse;

			pixels[pixelPos] = pal[color];	// R
			pixelPos++;

			pixels[pixelPos] = pal[color + 1];	// G
			pixelPos++;

			pixels[pixelPos] = pal[color + 2];	// B
			pixelPos++;

			pixels[pixelPos] = 255;		// A
			pixelPos++;

		}

		// Go to next pixel
		curPixel++;
	}

	// Set window background color
	BGcolor = sf::Color(sf::Color(pal[palToUse], pal[palToUse + 1], pal[palToUse + 2]));

	// create image
	image.create(width, height, pixels);
	delete[] pixels;

	return image;
}

bool GFX::savetoImage(std::string outName) {
	
	std::string Filename = (outName + ".bmp");
	return image.saveToFile(Filename);
}

GFX::~GFX() {
	delete[] GFXFile;
}

