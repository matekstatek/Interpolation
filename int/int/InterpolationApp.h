#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "Interp.h"
#include "ResourcePath.hpp"

using namespace std;

class InterpolationApp {

public:
	//Func
	void start();

	//Constructors
	InterpolationApp();
	InterpolationApp(const int dimensions);
	InterpolationApp(const int width, const int height);

private:
	//Func
	void init();
	void exitWithStatus(const int, bool = false);
	sf::Vector2i translateCoordinatesFromRaw(sf::Vector2i raw, sf::Vector2i origin);
	sf::Vector2i translateCoordinatesToRaw(sf::Vector2i offset, sf::Vector2i origin);
	void initializeAxes(sf::VertexArray& axes, sf::Vector2i origin, const int WINDOW_WIDTH, const int WINDOW_HEIGHT);
	void updateInterpolation(vector<sf::Vertex*> sortedPoints, sf::VertexArray& interpolatedPoints, Interp::Type type);
	void insertIntoArray(vector<sf::Vertex*>& userVertexArray, sf::Vector2f newVertex);
	void drawCircle(sf::Vector2f pos, sf::Color color);
	void drawEverything(vector<sf::Drawable*> stuffToDraw, sf::RenderWindow& window);

	//Members
	bool initCalled = false;

	const int DEFAULT_WINDOW_HEIGHT = 750;
	const int DEFAULT_WINDOW_WIDTH = 750;

	const int WINDOW_HEIGHT;
	const int WINDOW_WIDTH;
	const int DEFAULT_ORIGIN_Y = WINDOW_HEIGHT / 2;
	const int DEFAULT_ORIGIN_X = WINDOW_WIDTH / 2;
	const int FONT_SIZE = 16;

	const string DEFAULT_COORDINATES_TEXT = "( , )";
	const string DEFAULT_WINDOW_TITLE = "Interpolation project";

	//Our main window
	sf::RenderWindow window;

	//Will contain all things to be drawn in the window
	vector<sf::Drawable*> drawVector;

	//Represents the font to use in this app
	sf::Font font;
	const string FONT_FILENAME = "/Users/statekmatek/Desktop/int/int/arial.ttf";

	//Represents the coordinates readout in the top left
	sf::Text coordinatesText;

	//Represents the button to switch interpolation methods
	sf::Text switchText;
	sf::FloatRect switchOutline;

	//Represents the origin
	sf::Vector2i origin;

	//Represents the axes to be drawn
	sf::VertexArray axes;

	//Represents all the points the user manually clicked on
	sf::VertexArray userPoints;

	//Represents all points the program calculates to interpolate between the above points 
	sf::VertexArray interpolatedPoints;

	//Represents the currently selected type of interpolation
	Interp::Type currentType;

	//Represents the current interpolation method readout in top right
	sf::Text currentTypeText;

	//Represents the list of vertices chosen by user, but sorted
	vector<sf::Vertex*> sortedVertices;

};
