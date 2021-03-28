#include "GlobalHeader.h"

#include "triangle.h"

#include <iostream>

int Triangle::integerID = 1;

Triangle::Triangle(Vertex v1, Vertex v2, Vertex v3) {
	vertex1 = v1;
	vertex2 = v2;
	vertex3 = v3;

	calculateRGBID();
	area = calculateArea();
}

void Triangle::calculateRGBID() {

	float redValue = integerID % 256;

	int greenRemainingValue = (int)integerID / 256;

	float greenValue = greenRemainingValue % 256;

	int blueRemainingValue = (int)greenRemainingValue / 256;

	float blueValue = blueRemainingValue % 256;

	redValue = redValue / 255.0f;
	greenValue = greenValue / 255.0f;
	blueValue = blueValue / 255.0f;

	glm::vec3 idVector = glm::vec3(redValue, greenValue, blueValue);

	vertex1.rgbID = idVector;
	vertex2.rgbID = idVector;
	vertex3.rgbID = idVector;


	integerID += 10;
}

float Triangle::calculateArea() {
	glm::vec3 v1ToV2;
	glm::vec3 v1ToV3;
	glm::vec3 normalVector;

	float area;

	v1ToV2.x = vertex2.position.x - vertex1.position.x;
	v1ToV2.y = vertex2.position.y - vertex1.position.y;
	v1ToV2.z = vertex2.position.z - vertex1.position.z;

	v1ToV3.x = vertex3.position.x - vertex1.position.x;
	v1ToV3.y = vertex3.position.y - vertex1.position.y;
	v1ToV3.z = vertex3.position.z - vertex1.position.z;

	normalVector.x = v1ToV2.y * v1ToV3.z - v1ToV2.z * v1ToV3.y;
	normalVector.y = v1ToV2.z * v1ToV3.x - v1ToV2.x * v1ToV3.z;
	normalVector.z = v1ToV2.x * v1ToV3.y - v1ToV2.y * v1ToV3.x;

	area = 0.5 * sqrt(normalVector.x * normalVector.x + normalVector.y * normalVector.y + normalVector.z * normalVector.z);

	return area;
}

