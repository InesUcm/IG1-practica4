#pragma once
#include "Entity.h"


class Cube : public SingleColorEntity
{
public:
	explicit Cube(GLdouble length);
	void render(const glm::mat4& modelViewMat) const override;
};

class BoxOutline : public EntityWithTexture
{
protected:
	Texture* mTexture2;
public:
	explicit BoxOutline(GLdouble length);
	virtual ~BoxOutline() override;
	void render(const glm::mat4& modelViewMat) const override;
};

class Star3D : public EntityWithTexture
{
public:
	explicit Star3D(GLdouble re, GLuint np, GLdouble h);
	void render(const glm::mat4& modelViewMat) const override;
	void update() override;
};

class GlassParapet : public EntityWithTexture
{
public:
	explicit GlassParapet(GLdouble length);
	void render(const glm::mat4& modelViewMat) const override;
};