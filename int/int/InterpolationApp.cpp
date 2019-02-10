#include "InterpolationApp.h"
#include <iostream>

//Default constructor
InterpolationApp::InterpolationApp() : InterpolationApp(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT){}

//One int passed
InterpolationApp::InterpolationApp(const int dimensions) : InterpolationApp(dimensions, dimensions) {}

//Two ints passed
//Both other constructors call this one. Init should only be called once
InterpolationApp::InterpolationApp(const int width, const int height) : WINDOW_HEIGHT(height), WINDOW_WIDTH(width) { init(); }

void InterpolationApp::init() {

	//If this object has already been initialized, just return
	if (initCalled)
		return;

	//Load the font we need
	if (!font.loadFromFile(FONT_FILENAME))
	{

		cout << "Could not load font" << endl;
		exitWithStatus(EXIT_FAILURE, true);

	}

	//Initialize the coordinate locator
	coordinatesText = sf::Text(DEFAULT_COORDINATES_TEXT, font, FONT_SIZE);
	coordinatesText.setFillColor(sf::Color::Black);
	drawVector.push_back(&coordinatesText);

	switchText = sf::Text("Switch mode", font, FONT_SIZE);
	switchText.setFillColor(sf::Color::Black);
	switchText.setPosition(WINDOW_WIDTH / 2 - (switchText.getLocalBounds().width / 2), 0);
	drawVector.push_back(&switchText);

	switchOutline = switchText.getGlobalBounds();

	//Initialize the default origin
	origin = sf::Vector2i(DEFAULT_ORIGIN_X, DEFAULT_ORIGIN_Y);

	//Initialize the axes
	axes = sf::VertexArray(sf::Lines, 4);
	initializeAxes(axes, origin, WINDOW_WIDTH, WINDOW_HEIGHT);
	drawVector.push_back(&axes);

	//Initialize the vertex array for points
	userPoints = sf::VertexArray(sf::Points);
	interpolatedPoints = sf::VertexArray(sf::LineStrip);
	//drawVector.push_back(&userPoints);
	drawVector.push_back(&interpolatedPoints);

	currentType = Interp::Linear;

	currentTypeText = sf::Text(Interp::getTypeString(currentType), font, FONT_SIZE);
	currentTypeText.setFillColor(sf::Color::Black);
	currentTypeText.setPosition(WINDOW_WIDTH * 0.985 - currentTypeText.getLocalBounds().width, 0);
	drawVector.push_back(&currentTypeText);

	initCalled = true;

}

void InterpolationApp::start() {

	//Open our window
	sf::RenderWindow window(sf::VideoMode(WINDOW_HEIGHT, WINDOW_WIDTH), DEFAULT_WINDOW_TITLE);

	while (window.isOpen()) {

		sf::Event event;
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed) {

				//cout << "User closed the window" << endl;
				window.close();

			}
			else if (event.type == sf::Event::MouseLeft) {

				//When the mouse leaves the window
				coordinatesText.setString(DEFAULT_COORDINATES_TEXT);

			}
			else if (event.type == sf::Event::MouseMoved) {

				string coordinates_string, x, y = "";

				//Get mouse position
				//Convert the coordinates so that they reflect the chosen origin
				sf::Vector2i translatedMousePosition = translateCoordinatesFromRaw(sf::Mouse::getPosition(window), origin);

				x = to_string(translatedMousePosition.x);
				y = to_string(translatedMousePosition.y);

				coordinates_string = "(" + x + ", " + y + ")";
				coordinatesText.setString(coordinates_string);

			}
			else if (event.type == sf::Event::MouseButtonPressed) {

				//Add the point the user clicked

				sf::Vector2f mouseClickLocation = (sf::Vector2f)sf::Mouse::getPosition(window);

				//If the click is inside the switch text...
				if (switchOutline.contains(mouseClickLocation)) {

					//Swap the interpolation type
					switch (currentType) {

					case Interp::Linear:
						currentType = Interp::Cosine;
						break;

					case Interp::Cosine:
						if (sortedVertices.size() < 4)
							currentType = Interp::Linear;
						else
							currentType = Interp::Cubic;
						break;

					case Interp::Cubic:
						currentType = Interp::Linear;
						break;

					}

					currentTypeText.setString(Interp::getTypeString(currentType));

					//Update the stuff
					updateInterpolation(sortedVertices, interpolatedPoints, currentType);

					//Continue the window loop
					continue;

				}

				//Otherwise add a new point to the graph
				sf::Vector2f newVertexPosition = mouseClickLocation;

				//userPoints.append(sf::Vertex(newVertexPosition, sf::Color::Red));

				const int circleRadius = 1;
				const int numPoints = 10;

				drawCircle(newVertexPosition, sf::Color::Red);

				insertIntoArray(sortedVertices, newVertexPosition);
				updateInterpolation(sortedVertices, interpolatedPoints, currentType);

			}

		}

		//Clear the window
		window.clear(sf::Color::White);

		//Draw everything
		drawEverything(drawVector, window);

		//Tell the window to finally display everything
		window.display();

	}

	exitWithStatus(EXIT_SUCCESS);

}

void InterpolationApp::exitWithStatus(const int EXIT_STATUS, bool wait){

	if (wait)
	{

		cout << "Press enter to exit..." << endl;
		getchar();

	}

	exit(EXIT_STATUS);

}

sf::Vector2i InterpolationApp::translateCoordinatesFromRaw(sf::Vector2i raw, sf::Vector2i origin){

	sf::Vector2i translated;

	translated.x = raw.x - origin.x;
	translated.y = origin.y - raw.y;

	return translated;
}

sf::Vector2i InterpolationApp::translateCoordinatesToRaw(sf::Vector2i offset, sf::Vector2i origin){

	sf::Vector2i raw;

	raw.x = offset.x + origin.x;
	raw.y = origin.y - offset.y;

	return raw;
}

void InterpolationApp::initializeAxes(sf::VertexArray & axes, sf::Vector2i origin, const int WINDOW_WIDTH, const int WINDOW_HEIGHT){

	axes[0].position = sf::Vector2f((float)origin.x, 0.0);
	axes[1].position = sf::Vector2f((float)origin.x, (float)WINDOW_HEIGHT);

	axes[2].position = sf::Vector2f(0, (float)origin.y);
	axes[3].position = sf::Vector2f((float)WINDOW_WIDTH, (float)origin.y);

	for (register int i = 0; i < 4; i++)
		axes[i].color = sf::Color::Black;
}

void InterpolationApp::updateInterpolation(vector<sf::Vertex*> sortedPoints, sf::VertexArray & interpolatedPoints, Interp::Type type){

	if (sortedPoints.empty())
		return;

	interpolatedPoints.clear();

	//Do something else entirely if it's cubic
	if (type == Interp::Cubic) {

		sf::VertexArray temp;

		//First and last segments need to be cosine.
		//printf("Cosine interp between 0 and 1\n");
		Interp::interpolateBetweenPoints(*sortedPoints[0], *sortedPoints[1], temp, Interp::Cosine);
		sf::Vertex pointAfterFirst = temp[0];

		//We need to extrapolate one point at the start and one at the end.
		//Start one is easy, since we already have the point we need
		//But to get the end one, we need to calculate the slope in a different area
		
		Interp::interpolateBetweenPoints(*sortedPoints[sortedPoints.size() - 2], *sortedPoints[sortedPoints.size() - 1], temp, Interp::Cosine);

		sf::Vertex pointBeforeLast = temp[temp.getVertexCount()-1];
		
		//drawCircle(pointBeforeLast.position, sf::Color::Blue);
		//drawCircle(pointAfterFirst.position, sf::Color::Blue);

		Interp::interpolateBetweenPointsCubic(*sortedPoints[0], pointAfterFirst, *sortedPoints[1], *sortedPoints[2], interpolatedPoints);

		sf::Vertex v0, v1, v2, v3;

		for (unsigned int i = 0; i < sortedPoints.size() - 3; i++) {

			//printf("Cubic interp between %d and %d\n", i + 1, i + 2);

			v0 = *sortedPoints[i];
			v1 = *sortedPoints[i + 1];
			v2 = *sortedPoints[i + 2];
			v3 = *sortedPoints[i + 3];

			Interp::interpolateBetweenPointsCubic(v0, v1, v2, v3, interpolatedPoints);

		}

		//printf("Cosine interp between %d and %d\n", sortedPoints.size() - 2, sortedPoints.size() - 1);
		//Interp::interpolateBetweenPoints(*sortedPoints[sortedPoints.size() - 2], *sortedPoints[sortedPoints.size() - 1], interpolatedPoints, Interp::Cosine);
		Interp::interpolateBetweenPointsCubic(*sortedPoints[sortedPoints.size()-3], 
			*sortedPoints[sortedPoints.size()-2], 
			pointBeforeLast, 
			*sortedPoints[sortedPoints.size()-1], 
			interpolatedPoints);

		return;

	}


	sf::Vertex v1, v2;

	//Get every pair of adjacent vertices
	for (unsigned int i = 0; i < sortedPoints.size() - 1; i++) {

		v1 = *(sortedPoints[i]);
		v2 = *(sortedPoints[i + 1]);

		Interp::interpolateBetweenPoints(v1, v2, interpolatedPoints, type);

	}

}

void InterpolationApp::drawCircle(sf::Vector2f pos, sf::Color color) {

	const int radius = 1;
	const int numPoints = 10;

	//newVertexPosition.x-(circleRadius/2), newVertexPosition.y-(circleRadius/2)

	sf::CircleShape* toDraw = new sf::CircleShape(radius, numPoints);
	toDraw->setPosition(pos.x - (radius / 2), pos.y - (radius / 2));
	toDraw->setFillColor(color);
	toDraw->setOutlineColor(sf::Color::Black);
	toDraw->setOutlineThickness(1);

	drawVector.push_back(toDraw);

}

void InterpolationApp::insertIntoArray(vector<sf::Vertex*>& userVertexArray, sf::Vector2f newVertex){

	//First create the actual vertex object
	sf::Vertex* newVert = new sf::Vertex(newVertex, sf::Color::Black);

	//cout << "Inserting new vertice with position x: " << newVertex.x << " and y: " << newVertex.y << endl;

	if (userVertexArray.empty()) {

		//cout << "Array currently empty, adding vertex" << endl;

		//If it's empty, just add it
		userVertexArray.push_back(newVert);

	}
	else {

		//Otherwise find the appropriate place to insert it
		//Needs to be sorted by x coordinate

		//cout << "Starting iterator..." << endl;

		for (vector<sf::Vertex*>::iterator it = userVertexArray.begin();
			it != userVertexArray.end();
			it++) {

			//If the new vertex is higher
			if (newVertex.x < (*it)->position.x) {

				//cout << "Inserting vertex at position " << it - userVertexArray.begin() << endl;
				userVertexArray.insert(it, newVert);
				return;

			}

		}

		//Otherwise, just add it to the end
		//cout << "Inserting vertex at the end" << endl;
		userVertexArray.push_back(newVert);

	}
}

void InterpolationApp::drawEverything(vector<sf::Drawable*> stuffToDraw, sf::RenderWindow & window){

	for (vector<sf::Drawable*>::iterator it = stuffToDraw.begin(); it != stuffToDraw.end(); it++)
		window.draw(**it);

}