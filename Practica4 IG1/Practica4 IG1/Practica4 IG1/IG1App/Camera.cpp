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
	Shader::setUniform4All("modelView", mViewMat); 
}

void Camera::setVM()
{
	mViewMat = lookAt(mEye, mLook, mUp); // glm::lookAt defines the view matrix
	setAxes();
}

void Camera::set2D()
{
	mEye = { 0, 0, 500 };
	mLook = { 0, 0, 0 };
	mUp = { 0, 1, 0 };
	//damos valor a mRadio y mAng
	mRadio = 500.0;
	mAng = 90.0;

	setVM();
}

void Camera::set3D()
{
	mEye = { 500, 500, 500 };
	mLook = { 0, 10, 0 };
	mUp = { 0, 1, 0 };
	//damos valor a mRadio y mAng
	mRadio = 707.1;
	mAng = 45.0;

	setVM();
}

void Camera::setCenital()
{
	//colocamos el ojo en el eje Y
	mEye = { 0, 500, 0 };
	mLook = { 0, 0, 0 };
	mUp = { 0, 0, -1 };

	mRadio = 500.0;
	mAng = 90.0;

	setVM();
}

void Camera::pitch(GLfloat a) 
{ 
	mViewMat = rotate(mViewMat, radians(a), vec3(1.0f, 0.0f, 0.0f)); 
	// glm::rotate returns mViewMat * rotationMatrix
	setAxes();
}
void Camera::yaw(GLfloat a) 
{ 
	mViewMat = rotate(mViewMat, radians(a), vec3(0.0f, 1.0f, 0.0f)); 
	// glm::rotate returns mViewMat * rotationMatrix
	setAxes();
}
void Camera::roll(GLfloat a) 
{
	mViewMat = rotate(mViewMat, radians(a), vec3(0.0f, 0.0f, 1.0f)); 
	// glm::rotate returns mViewMat * rotationMatrix
	setAxes();
}

void Camera::setSize(GLdouble xw, GLdouble yh)
{
	xRight = xw / 2.0;
	xLeft = -xRight;
	yTop = yh / 2.0;
	yBot = -yTop;
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
	//mira si la proyeccion es ortogonal o en perspectiva
	if (bOrto) {//  if orthogonal projection
		mProjMat = ortho(
			xLeft * mScaleFact,
			xRight * mScaleFact,
			yBot * mScaleFact,
			yTop * mScaleFact,
			mNearVal,
			mFarVal);
		// glm::ortho defines the orthogonal projection matrix
	}
	else {//proyeccion en perspectiva
		// Dibudimos los límites por 500 para que se vea en grande la perspectiva
		mProjMat = frustum(
			(xLeft / 500.0) * mScaleFact,
			(xRight / 500.0) * mScaleFact,
			(yBot / 500.0) * mScaleFact,
			(yTop / 500.0) * mScaleFact,
			mNearVal,
			mFarVal);
	}
}

void Camera::uploadPM() const 
{ 
	Shader::setUniform4All("projection", mProjMat); 
}

void Camera::upload() const
{
	mViewPort->upload();
	uploadVM();
	uploadPM();
}

void Camera::setAxes() {
	mRight = row(mViewMat, 0);
	mUpward = row(mViewMat, 1);
	mFront = -row(mViewMat, 2);
}

void Camera::moveLR(GLfloat cs) {
	mEye += mRight * (float)cs;
	mLook += mRight * (float)cs;
	setVM(); // Reconstruye la matriz con los nuevos puntos y llama a setAxes()
}
void Camera::moveFB(GLfloat cs) {
	mEye += mFront * (float)cs;
	mLook += mFront * (float)cs;
	setVM();
}
void Camera::moveUD(GLfloat cs) {
	mEye += mUpward * (float)cs;
	mLook += mUpward * (float)cs;
	setVM();
}

void Camera::changePrj(){
	bOrto = !bOrto;//cambia la perspectiva
	setPM(); 
}

void Camera::pitchReal(GLfloat cs)
{
	//Calculamos el vector que va del ojo al objetivo
	glm::vec3 focus = mLook - mEye;
	//Lo rotamos usando el eje derecho de la cámara
	focus = rotate(glm::mat4(1.0f), glm::radians(cs), mRight) * glm::vec4(focus, 0.0f);
	//El nuevo mLook es la posición del ojo + el nuevo vector rotado
	mLook = mEye + focus;
	setVM();
}
void Camera::yawReal(GLfloat cs)
{
	glm::vec3 focus = mLook - mEye;
	focus = rotate(glm::mat4(1.0f), glm::radians(cs), mUpward) * glm::vec4(focus, 0.0f);
	mLook = mEye + focus;
	setVM();
}
void Camera::rollReal(GLfloat cs)
{
	mUp = rotate(glm::mat4(1.0f), glm::radians(cs), mFront) * glm::vec4(mUp, 0.0f);
	setVM();
}

void Camera::orbit(GLdouble incAng, GLdouble incY) {
	mAng += incAng;
	mEye.x = mLook.x + cos(radians(mAng)) * mRadio;
	mEye.z = mLook.z - sin(radians(mAng)) * mRadio;
	mEye.y += incY;
	setVM();
}