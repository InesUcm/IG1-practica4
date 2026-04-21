#include "IG1App.h"
#include "Scene.h"
#include "Shader.h"
#include "Entity.h"
#include <iostream>

using namespace std;

IG1App IG1App::s_ig1app;

void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam)
{
	const char* prefix = (type == GL_DEBUG_TYPE_ERROR)
		? "\x1b[31m[ERROR]\x1b[0m "
		: "\x1b[33m[WARNING]\x1b[0m ";
	cout << prefix << message << endl;
}

void IG1App::close()
{
	cout << "Closing GLFW...\n";
	glfwSetWindowShouldClose(mWindow, true);
}

void IG1App::run()
{
	if (mWindow == 0) init();

	while (!glfwWindowShouldClose(mWindow)) {
		if (mNeedsRedisplay) {
			display();
			mNeedsRedisplay = false;
		}
		if (mUpdateEnabled || mUpdateFrame) {
			double currentTime = glfwGetTime();
			if (currentTime >= mNextUpdate) {
				mScenes[mCurrentScene]->update();
				mNeedsRedisplay = true;
				mNextUpdate += FRAME_DURATION;
			}
			double timeout = mNextUpdate - currentTime;
			if (timeout < 0.0) timeout = 0.0;
			glfwWaitEventsTimeout(timeout);
			mUpdateFrame = false;
		}
		else {
			glfwWaitEvents();
			mNextUpdate = glfwGetTime();
		}
	}
	destroy();
}

void IG1App::init()
{
	iniWinOpenGL();
	mViewPort = new Viewport(mWinW, mWinH);
	mCamera = new Camera(mViewPort);

	mScenes.push_back(new Scene);   // 0
	mScenes.push_back(new Scene1);  // 1
	mScenes.push_back(new Scene2);  // 2
	mScenes.push_back(new Scene3);  // 3
	mScenes.push_back(new Scene4);  // 4
	mScenes.push_back(new Scene5);  // 5
	mScenes.push_back(new Scene6);  // 6
	mScenes.push_back(new Scene7);  // 7
	mScenes.push_back(new Scene8);  // 8

	mCamera->set2D();
	for (Scene* scene : mScenes) scene->init();
	mScenes[mCurrentScene]->load();
}

void IG1App::iniWinOpenGL()
{
	cout << "Starting GLFW...\n";
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	mWindow = glfwCreateWindow(mWinW, mWinH, "IG1App", nullptr, nullptr);
	if (!mWindow) { glfwTerminate(); throw std::logic_error("cannot create GLFW window"); }

	glfwMakeContextCurrent(mWindow);

	if (GLenum err = glewInit(); err != GLEW_OK) {
		glfwTerminate();
		throw logic_error("Error GLEW: "s + reinterpret_cast<const char*>(glewGetErrorString(err)));
	}

	glfwSetWindowSizeCallback(mWindow, s_resize);
	glfwSetCharCallback(mWindow, s_key);
	glfwSetKeyCallback(mWindow, s_specialkey);
	glfwSetWindowRefreshCallback(mWindow, s_display);
	glfwSetMouseButtonCallback(mWindow, s_mouse);
	glfwSetCursorPosCallback(mWindow, s_motion);
	glfwSetScrollCallback(mWindow, s_mouseWheel);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0u, 0, GL_TRUE);
	glDebugMessageCallback(debugCallback, nullptr);

	cout << glGetString(GL_VERSION) << '\n';
	cout << glGetString(GL_VENDOR) << '\n';
}

void IG1App::destroy()
{
	for (Scene* scene : mScenes) delete scene;
	mScenes.clear();
	delete mCamera;   mCamera = nullptr;
	delete mViewPort; mViewPort = nullptr;
	Shader::freeAll();
	glfwTerminate();
}

void IG1App::display() const
{
	// ── FIX FONDO: setClearColor() antes de glClear ──────────────────────────
	// Cada escena declara su propio color de fondo en setClearColor().
	// Llamándolo aquí, justo antes del glClear, el color correcto se aplica
	// en el mismo frame, sin esperar al siguiente.
	mScenes[mCurrentScene]->setClearColor();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!m2Vistas) {
		mCamera->getViewport()->setPos(0, 0);
		mCamera->getViewport()->setSize(mWinW, mWinH);
		mScenes[mCurrentScene]->render(*mCamera);
	}
	else {
		int centro = mWinH / 4;

		mCamera->set3D();
		mCamera->getViewport()->setPos(0, centro);
		mCamera->getViewport()->setSize(mWinW / 2, mWinH / 2);
		mCamera->upload();
		mScenes[mCurrentScene]->render(*mCamera);

		mCamera->setCenital();
		mCamera->getViewport()->setPos(mWinW / 2, centro);
		mCamera->getViewport()->setSize(mWinW / 2, mWinH / 2);
		mCamera->upload();
		mScenes[mCurrentScene]->render(*mCamera);
	}
	glfwSwapBuffers(mWindow);
}

void IG1App::resize(int newWidth, int newHeight)
{
	mWinW = newWidth;
	mWinH = newHeight;
	mViewPort->setSize(newWidth, newHeight);
	mCamera->setSize(mViewPort->width(), mViewPort->height());
}

void IG1App::key(unsigned int key)
{
	bool need_redisplay = true;
	GLdouble step = 10.0;

	switch (key) {
	case '+': mCamera->setScale(+0.01); break;
	case '-': mCamera->setScale(-0.01); break;
	case 'l':
		static_cast<Scene4*>(mScenes[4])->setCapture();
		mCamera->set3D();
		break;
	case 'o':
		static_cast<Scene4*>(mScenes[4])->setCapture();
		mCamera->set2D();
		break;
	case 'u': mUpdateFrame = true;            break;
	case 'U': mUpdateEnabled = !mUpdateEnabled; break;
	case 'a': mCamera->moveLR(-step); break;
	case 'd': mCamera->moveLR(+step); break;
	case 'w': mCamera->moveUD(+step); break;
	case 's': mCamera->moveUD(-step); break;
	case 'W': mCamera->moveFB(+step); break;
	case 'S': mCamera->moveFB(-step); break;
	case 'p': mCamera->changePrj();   break;
	case 'k': m2Vistas = !m2Vistas;  break;
	case 'N': ColorMaterialEntity::toggleShowNormals(); break;
	case 'f':
		if (mCurrentScene == 8)
			static_cast<Scene8*>(mScenes[8])->rotate();
		break;
	case 'g':
		if (mCurrentScene == 8)
			static_cast<Scene8*>(mScenes[8])->orbit();
		break;
	default:
		if (key >= '0' && key <= '9') {
			if (changeScene(key - '0')) break;
			cout << "[NOTE] No existe la escena " << char(key) << ".\n";
		}
		need_redisplay = false;
		break;
	}

	if (need_redisplay) mNeedsRedisplay = true;
}

void IG1App::specialkey(int key, int scancode, int action, int mods)
{
	if (action == GLFW_RELEASE) return;
	bool need_redisplay = true;
	GLfloat angle = 1.0f;

	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(mWindow, true);
		break;
	case GLFW_KEY_RIGHT:
		static_cast<Scene4*>(mScenes[4])->setCapture();
		(mods == GLFW_MOD_CONTROL) ? mCamera->rollReal(-angle) : mCamera->yawReal(-angle);
		break;
	case GLFW_KEY_LEFT:
		static_cast<Scene4*>(mScenes[4])->setCapture();
		(mods == GLFW_MOD_CONTROL) ? mCamera->rollReal(+angle) : mCamera->yawReal(+angle);
		break;
	case GLFW_KEY_UP:
		static_cast<Scene4*>(mScenes[4])->setCapture();
		mCamera->pitchReal(+angle);
		break;
	case GLFW_KEY_DOWN:
		static_cast<Scene4*>(mScenes[4])->setCapture();
		mCamera->pitchReal(-angle);
		break;
	default:
		need_redisplay = false;
		break;
	}
	if (need_redisplay) mNeedsRedisplay = true;
}

bool IG1App::changeScene(size_t sceneNr)
{
	if (sceneNr >= mScenes.size()) return false;
	if (sceneNr != mCurrentScene) {
		mScenes[mCurrentScene]->unload();
		mCurrentScene = sceneNr;
		mScenes[mCurrentScene]->load();
	}
	return true;
}

void IG1App::mouse(int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		mMouseButt = button;
		glfwGetCursorPos(mWindow, &mMouseCoord.x, &mMouseCoord.y);
	}
	else if (action == GLFW_RELEASE) {
		mMouseButt = -1; // limpiar al soltar para que motion() no siga actuando
	}
}

void IG1App::motion(double x, double y)
{
	if (mMouseButt == -1) return; // no hay botón pulsado

	glm::dvec2 mp = mMouseCoord - glm::dvec2(x, y);
	mMouseCoord = glm::dvec2(x, y);

	if (mMouseButt == GLFW_MOUSE_BUTTON_LEFT) {
		// Click izquierdo: órbita. mp.x controla el ángulo, mp.y la altura.
		mCamera->orbit(mp.x * 0.5, mp.y * 0.5);
	}
	else if (mMouseButt == GLFW_MOUSE_BUTTON_RIGHT) {
		// Click derecho: paneo lateral y vertical
		mCamera->moveLR(mp.x * 0.5);
		mCamera->moveUD(mp.y * 0.5);
	}

	mNeedsRedisplay = true;
}

void IG1App::mouseWheel(double dx, double dy)
{
	int ctrlLeft = glfwGetKey(mWindow, GLFW_KEY_LEFT_CONTROL);
	int ctrlRight = glfwGetKey(mWindow, GLFW_KEY_RIGHT_CONTROL);

	if (ctrlLeft == GLFW_PRESS || ctrlRight == GLFW_PRESS)
		mCamera->setScale(dy * 0.05);
	else
		mCamera->moveFB(dy * 10.0);

	mNeedsRedisplay = true;
}