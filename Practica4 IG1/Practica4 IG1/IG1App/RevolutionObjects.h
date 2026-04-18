#pragma once
#include "Entity.h"

class Torus : public SingleColorEntity
{
public:
	explicit Torus(GLdouble R, GLdouble r, GLuint nPoints = 40, GLuint nSamples = 40);
};

