#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

class Interp {

public:
	enum Type { Linear = 0, Cosine = 1, Cubic = 2 };
	static void interpolateBetweenPoints(sf::Vertex v1, sf::Vertex v2, sf::VertexArray& pointArray, Interp::Type type);
	static void interpolateBetweenPointsCubic(sf::Vertex v1, sf::Vertex v2, sf::Vertex v3, sf::Vertex v4, sf::VertexArray& pointArray);

	static double linear(double y1, double y2, double mu);
	static double cosine(double y1, double y2, double mu);
	static double cubic(double y0, double y1, double y2, double y3, double mu);

	static double linearExtrapolate(double x1, double y1, double x2, double y2, double x);
	static double linearExtrapolate(sf::Vertex v1, sf::Vertex v2, double x);
	static void linearExtrapolate(sf::Vertex v1, sf::Vertex v2, double x, sf::Vertex& extrap);

	static std::string getTypeString(Interp::Type type);

};
