#include "Interp.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <iostream>
#include <cstdio>

using namespace std;

void Interp::interpolateBetweenPoints(sf::Vertex v1, sf::Vertex v2, sf::VertexArray& pointArray, Interp::Type type) {

	float distance = abs(v1.position.x - v2.position.x);
	float step = 1 / (distance);

	
	printf("Interpolating (%d, %d) and (%d, %d) with step %f (distance = %f)\n", 
		(int)v1.position.x, 
		(int)v1.position.y, 
		(int)v2.position.x,
		(int)v2.position.y,
		step,
		distance);
		
	double (*interpFunc)(double, double, double) = NULL;

	switch (type) {

		case Interp::Cosine:
			interpFunc = Interp::cosine;
		break;

		case Interp::Linear:
			interpFunc = Interp::linear;
		break;

		default:
			cout << "Error! Could not find interpolation function!" << endl;
			return;

	}

	float x_val = v1.position.x;
	float y_val = 0;

	//Here we generate the rest of the interpolated points
	for (float i = 0; i <= 1; i += step) {

		x_val++;
		y_val = (float)interpFunc(v1.position.y, v2.position.y, i);

		//printf("(%d, %d)\n", x_val, y_val);

		pointArray.append(sf::Vertex(sf::Vector2f(x_val, y_val), sf::Color::Black));

	}

}

void Interp::interpolateBetweenPointsCubic(sf::Vertex v0, sf::Vertex v1, sf::Vertex v2, sf::Vertex v3, sf::VertexArray& pointArray) {

	float distance = abs(v1.position.x - v2.position.x);
	float step = 1 / (distance);
	float x_val = v1.position.x;
	float y_val = 0;

	//printf("Interpolating between (%d, %d) and (%d, %d)\n", (int)v1.position.x, (int)v1.position.y, (int)v2.position.x, (int)v2.position.y);

	for (float i = 0; i <= 1; i += step) {

		x_val++;
		y_val = cubic(v0.position.y, v1.position.y, v2.position.y, v3.position.y, i);

		//printf("(%d,%d)\n", (int)x_val, (int)y_val);

		pointArray.append(sf::Vertex(sf::Vector2f(x_val, y_val), sf::Color::Black));

	}

}

double Interp::linear(double y1, double y2, double mu) {

	return (y1 * (1 - mu) + y2 * mu);

};

double Interp::cosine(double y1, double y2, double mu) {

	double mu2 = (1 - cos(mu * M_PI)) / 2;
	return (y1 * (1 - mu2) + y2 * mu2);

}

double Interp::cubic(double y0, double y1, double y2, double y3, double mu) {

	double a0, a1, a2, a3, mu2;

	mu2 = mu*mu;
	a0 = y3 - y2 - y0 + y1;
	a1 = y0 - y1 - a0;
	a2 = y2 - y0;
	a3 = y1;

	return(a0*mu*mu2 + a1*mu2 + a2*mu + a3);

}

std::string Interp::getTypeString(Interp::Type type) {

	const string const types[3] = {"Linear", "Cosine", "Cubic"};
	return types[type];

}

double Interp::linearExtrapolate(double x1, double y1, double x2, double y2, double x) {

	return y1 + ((x-x1)/(x2-x1))*(y2-y1);

}

double Interp::linearExtrapolate(sf::Vertex v1, sf::Vertex v2, double x) {

	return linearExtrapolate(v1.position.x, v1.position.y, v2.position.x, v2.position.y, x);

}

void Interp::linearExtrapolate(sf::Vertex v1, sf::Vertex v2, double x, sf::Vertex& extrap) {

	extrap.position.x = x;
	extrap.position.y = linearExtrapolate(v1, v2, x);

}
