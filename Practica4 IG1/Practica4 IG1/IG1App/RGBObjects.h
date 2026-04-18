#pragma once
#include "Entity.h"

class RGBTriangle : public EntityWithColors {
public:
	explicit RGBTriangle(GLdouble r, GLfloat Radio);
	virtual void update();
};

class RGBRectangle : public EntityWithColors {
public:
	explicit RGBRectangle(GLdouble w, GLdouble h);
	void render(const glm::mat4& modelViewMat) const override;
};

class RGBCube : public EntityWithColors
{
public:
	explicit RGBCube(GLdouble length);
};


