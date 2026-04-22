#include "Shader.h"
#include "Camera.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_access.hpp>

using namespace glm;

Camera::Camera(Viewport* vp)
	: mViewMat(1.0)
	, mProjMat(1.0)
	, xRight(vp->width() / 2.0)
	, xLeft(-xRight)
	, yTop(vp->height() / 2.0)
	, yBot(-yTop)
	, mViewPort(vp)
{
	setPM();
	setAxes();
}

void Camera::uploadVM() const
{
	// Sube la matriz de vista a todos los shaders
	Shader::setUniform4All("modelView", mViewMat);

	// Apt. 58: la dirección de la luz en coordenadas mundiales es (-1, -1.5, -1.25)
	// (valor exacto del enunciado). Se transforma a coordenadas de vista multiplicando
	// por mViewMat. w=0 porque es una dirección, no un punto (la traslación no afecta).
	glm::vec4 lightDirWorld = glm::vec4(-1.0f, -1.5f, -1.25f, 0.0f);
	glm::vec4 lightDirView = glm::normalize(mViewMat * lightDirWorld);
	Shader* ls = Shader::get("simple_light");
	if (ls) {
		ls->use();
		ls->setUniform("lightDir", lightDirView);
	}
}

void Camera::setVM()
{
	mViewMat = lookAt(mEye, mLook, mUp);
	setAxes();
	setPM();
}

void Camera::set2D()
{
	mEye = { 0, 0, 500 };
	mLook = { 0, 0, 0 };
	mUp = { 0, 1, 0 };
	mRadio = 500.0; mAng = 90.0;
	setVM();
}

void Camera::set3D()
{
	mEye = { 500, 500, 500 };
	mLook = { 0, 10, 0 };
	mUp = { 0, 1, 0 };
	mRadio = 707.1; mAng = 45.0;
	setVM();
}

void Camera::setCenital()
{
	mEye = { 0, 500, 0 };
	mLook = { 0, 0, 0 };
	mUp = { 0, 0, -1 };
	mRadio = 500.0; mAng = 90.0;
	setVM();
}

void Camera::pitch(GLfloat a)
{
	mViewMat = rotate(mViewMat, radians(a), vec3(1.0f, 0.0f, 0.0f));
	setAxes();
}
void Camera::yaw(GLfloat a)
{
	mViewMat = rotate(mViewMat, radians(a), vec3(0.0f, 1.0f, 0.0f));
	setAxes();
}
void Camera::roll(GLfloat a)
{
	mViewMat = rotate(mViewMat, radians(a), vec3(0.0f, 0.0f, 1.0f));
	setAxes();
}

void Camera::setSize(GLdouble xw, GLdouble yh)
{
	xRight = xw / 2.0; xLeft = -xRight;
	yTop = yh / 2.0; yBot = -yTop;
	setPM();
}

void Camera::setScale(GLdouble s)
{
	mScaleFact -= s;
	if (mScaleFact < 0.0) mScaleFact = 0.01;
	setPM();
}

void Camera::setPM()
{
	if (bOrto) {
		mProjMat = ortho(
			xLeft * mScaleFact, xRight * mScaleFact,
			yBot * mScaleFact, yTop * mScaleFact,
			mNearVal, mFarVal);
	}
	else {
		// Ratio: mNearVal / distancia(ojo, look) — corrección de la profesora
		GLdouble dist = glm::length(mEye - mLook);
		GLdouble ratio = mNearVal / dist;
		mProjMat = frustum(
			xLeft * mScaleFact * ratio, xRight * mScaleFact * ratio,
			yBot * mScaleFact * ratio, yTop * mScaleFact * ratio,
			mNearVal, mFarVal);
	}
}

void Camera::uploadPM() const { Shader::setUniform4All("projection", mProjMat); }

void Camera::upload() const
{
	mViewPort->upload();
	uploadVM();
	uploadPM();
}

void Camera::setAxes()
{
	mRight = row(mViewMat, 0);
	mUpward = row(mViewMat, 1);
	mFront = -row(mViewMat, 2);
}

void Camera::moveLR(GLfloat cs)
{
	mEye += mRight * (float)cs;
	mLook += mRight * (float)cs;
	setVM();
}
void Camera::moveFB(GLfloat cs)
{
	mEye += mFront * (float)cs;
	mLook += mFront * (float)cs;
	setVM();
}
void Camera::moveUD(GLfloat cs)
{
	mEye += mUpward * (float)cs;
	mLook += mUpward * (float)cs;
	setVM();
}

void Camera::changePrj() { bOrto = !bOrto; setPM(); }

// pitchReal: rota el vector focus y mUp alrededor de mRight (eje lateral de la cámara)
void Camera::pitchReal(GLfloat cs)
{
	glm::vec3 focus = mLook - mEye;
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(cs), mRight);
	focus = glm::vec3(rot * glm::vec4(focus, 0.0f));
	mLook = mEye + focus;
	mUp = glm::vec3(rot * glm::vec4(mUp, 0.0f));
	setVM();
}

// yawReal: rota el vector focus y mUp alrededor de mUpward (eje vertical de la cámara)
void Camera::yawReal(GLfloat cs)
{
	glm::vec3 focus = mLook - mEye;
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(cs), mUpward);
	focus = glm::vec3(rot * glm::vec4(focus, 0.0f));
	mLook = mEye + focus;
	mUp = glm::vec3(rot * glm::vec4(mUp, 0.0f));
	setVM();
}

void Camera::rollReal(GLfloat cs)
{
	mUp = glm::vec3(rotate(glm::mat4(1.0f), glm::radians(cs), mFront) * glm::vec4(mUp, 0.0f));
	setVM();
}

// orbit: mueve el ojo alrededor de mLook manteniendo mUp = (0,1,0)
void Camera::orbit(GLdouble incAng, GLdouble incY)
{
	mAng += incAng;
	mEye.x = mLook.x + (float)(cos(radians(mAng)) * mRadio);
	mEye.z = mLook.z - (float)(sin(radians(mAng)) * mRadio);
	mEye.y += (float)incY;
	mUp = { 0, 1, 0 };
	setVM();
}