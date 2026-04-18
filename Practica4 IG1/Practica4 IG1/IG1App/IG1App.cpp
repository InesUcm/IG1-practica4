#include "IG1App.h"
#include "Scene.h"
#include "Shader.h"
#include <iostream>

using namespace std;

IG1App IG1App::s_ig1app;

// Print OpenGL errors and warnings
void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam)
{
	const char* prefix = (type == GL_DEBUG_TYPE_ERROR)
		? "\x1b[31m[ERROR]\x1b[0m "
		: "\x1b[33m[WARNING]\x1b[0m ";
	cout << prefix << message << endl;
}

void
IG1App::close()
{
	cout << "Closing GLFW...\n";
	glfwSetWindowShouldClose(mWindow, true);
}

void
IG1App::run()// enters the main event processing loop
{
	if (mWindow == 0)
		init();

	while (!glfwWindowShouldClose(mWindow)) {

		// Redisplay the window if needed
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
			// Stop and wait for new events
			glfwWaitEvents();
			mNextUpdate = glfwGetTime();
		}
	}

	destroy();
}

void
IG1App::init()
{
	// create an OpenGL Context
	iniWinOpenGL();

	// create the scene after creating the context
	// allocate memory and resources
	mViewPort = new Viewport(mWinW, mWinH);
	mCamera = new Camera(mViewPort);

	// Registrar todas las escenas
	mScenes.push_back(new Scene);
	mScenes.push_back(new Scene1); 
	mScenes.push_back(new Scene2);  
	mScenes.push_back(new Scene3);
	mScenes.push_back(new Scene4); 
	mScenes.push_back(new Scene5);

	mCamera->set2D();
	for (Scene* scene : mScenes)
		scene->init();

	mScenes[mCurrentScene]->load();
}

void
IG1App::iniWinOpenGL()
{ // Initialization
	cout << "Starting GLFW...\n";
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	mWindow = glfwCreateWindow(mWinW, mWinH, "IG1App", nullptr, nullptr);
	if (mWindow == nullptr) {
		glfwTerminate();
		throw std::logic_error("cannot create GLFW window");
	}

	glfwMakeContextCurrent(mWindow);

	// Initialize OpenGL extension library
	if (GLenum err = glewInit(); err != GLEW_OK) {
		glfwTerminate();
		throw logic_error("Error al cargar extensiones GLEW: "s +
			reinterpret_cast<const char*>(glewGetErrorString(err)));
	}

	// Callback registration
	glfwSetWindowSizeCallback(mWindow, s_resize);
	glfwSetCharCallback(mWindow, s_key);
	glfwSetKeyCallback(mWindow, s_specialkey);
	glfwSetWindowRefreshCallback(mWindow, s_display);

	// Error message callback (all messages)
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0u, 0, GL_TRUE);
	glDebugMessageCallback(debugCallback, nullptr);

	cout << glGetString(GL_VERSION) << '\n';
	cout << glGetString(GL_VENDOR) << '\n';
}

void
IG1App::destroy()
{
	for (Scene* scene : mScenes)
		delete scene;
	mScenes.clear();

	delete mCamera;   
	mCamera = nullptr;
	delete mViewPort; 
	mViewPort = nullptr;

	// evita los memory leaks de shaders
	Shader::freeAll();

	glfwTerminate();
}

void
IG1App::display() const
{ // double buffering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// clears the back buffer

	//modo de vista normal
	if (!m2Vistas) {
		//en grande y en el centro de la pantalla
		mCamera->getViewport()->setPos(0, 0);
		mCamera->getViewport()->setSize(mWinW, mWinH);

		mScenes[mCurrentScene]->render(*mCamera);// uploads the viewport and camera to the GPU
	}

	//modo de vista doble
	else {
		int centro = mWinH / 4; //altura central de la pantalla

		//vista en 3D normal
		mCamera->set3D();
		mCamera->getViewport()->setPos(0, centro); //que se vea en el centro 
		mCamera->getViewport()->setSize(mWinW / 2, mWinH / 2); //mitad de ancho

		mCamera->upload();
		mScenes[mCurrentScene]->render(*mCamera);

		//vista cenital
		mCamera->setCenital();
		mCamera->getViewport()->setPos(mWinW / 2, centro); //empieza en la mitad de la pantalla
		mCamera->getViewport()->setSize(mWinW / 2, mWinH / 2); //mitad de ancho

		mCamera->upload();
		mScenes[mCurrentScene]->render(*mCamera);
	}
	glfwSwapBuffers(mWindow); // swaps the front and back buffer
}

void
IG1App::resize(int newWidth, int newHeight)
{
	mWinW = newWidth;
	mWinH = newHeight;
	// Resize Viewport to the new window size
	mViewPort->setSize(newWidth, newHeight);
	// Resize Scene Visible Area such that the scale is not modified
	mCamera->setSize(mViewPort->width(), mViewPort->height());
}

void
IG1App::key(unsigned int key)
{
	bool need_redisplay = true;

	GLdouble step = 10.0;//velocidad de movimiento de la camara

	switch (key) {
	case '+':
		mCamera->setScale(+0.01); // zoom in  (increases the scale)
		break;
	case '-':
		mCamera->setScale(-0.01); // zoom out (decreases the scale)
		break;
	case 'l':
		static_cast<Scene4*>(mScenes[4])->setCapture();//hay que capturar la foto de nuevo
		mCamera->set3D();
		break;
	case 'o':
		static_cast<Scene4*>(mScenes[4])->setCapture();//hay que capturar la foto de nuevo
		mCamera->set2D();
		break;
	case 'u':
		mUpdateFrame = true;
		break;
	case 'U':
		mUpdateEnabled = !mUpdateEnabled;
		break;
		//Mueve la camara de derecha a izquierda
	case 'a':
		mCamera->moveLR(-step);
		break;
	case 'd':
		mCamera->moveLR(step);
		break;
		//Mueve la camara de arriba a abajo
	case 'w':
		mCamera->moveUD(step);
		break;
	case 's':
		mCamera->moveUD(-step);
		break;
		//Mueve la camara del frente a atras
	case 'W':
		mCamera->moveFB(step);
		break;
	case 'S':
		mCamera->moveFB(-step);
		break;
		//Cambia la proyeccion
	case 'p':
		mCamera->changePrj();
		break;
	case 'k':
		m2Vistas = !m2Vistas; //Alterna el modo
		break;
	default:
		if (key >= '0' && key <= '9') {
			if (changeScene(key - '0')) break;
			cout << "[NOTE] No existe la escena " << char(key) << ".\n";
		}
		need_redisplay = false;
		break;
	}

	if (need_redisplay)
		mNeedsRedisplay = true;
}

void
IG1App::specialkey(int key, int scancode, int action, int mods)
{
	// Only interested in press events
	if (action == GLFW_RELEASE) return;

	bool need_redisplay = true;
	GLfloat angle = 1.0f; //angulo de rotacion de la camara

	// Handle keyboard input
	// (key reference: https://www.glfw.org/docs/3.4/group__keys.html)
	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(mWindow, true);// stops main loop
		break;
	case GLFW_KEY_RIGHT:
		static_cast<Scene4*>(mScenes[4])->setCapture(); //hay que capturar la foto de nuevo
		if (mods == GLFW_MOD_CONTROL)
			mCamera->rollReal(-angle); // rotates -1 on the X axis
		else
			mCamera->yawReal(-angle); // rotates 1 on the X axis
		break;
	case GLFW_KEY_LEFT:
		static_cast<Scene4*>(mScenes[4])->setCapture();//hay que capturar la foto de nuevo
		if (mods == GLFW_MOD_CONTROL)
			mCamera->rollReal(angle); // rotates 1 on the Y axis
		else
			mCamera->yawReal(angle); // rotate -1 on the Y axis
		break;
	case GLFW_KEY_UP:
		static_cast<Scene4*>(mScenes[4])->setCapture();//hay que capturar la foto de nuevo
		mCamera->pitchReal(angle); // rotates 1 on the Z axis
		break;
	case GLFW_KEY_DOWN:
		static_cast<Scene4*>(mScenes[4])->setCapture();//hay que capturar la foto de nuevo
		mCamera->pitchReal(-angle); // rotates -1 on the Z axis
		break;
	default:
		need_redisplay = false;
		break;
	}

	if (need_redisplay)
		mNeedsRedisplay = true;
}

bool
IG1App::changeScene(size_t sceneNr)
{
	// Check whether the scene exists
	if (sceneNr >= mScenes.size())
		return false;

	// Change only if a different scene
	if (sceneNr != mCurrentScene) {
		mScenes[mCurrentScene]->unload();
		mCurrentScene = sceneNr;
		mScenes[mCurrentScene]->load();
	}

	return true;
}

void 
IG1App::mouse(int button, int action, int mods)
{
	//guardamos que boton se ha pulsado
	mMouseButt = button; 
	//guardamos la posicion del raton
	glfwGetCursorPos(mWindow, &mMouseCoord.x, &mMouseCoord.y);
}

void 
IG1App::motion(double x, double y)
{
	glm::dvec2 mp = mMouseCoord - glm::dvec2(x, y);
	//guardamos la posicion
	mMouseCoord = glm::dvec2(x, y);

	if (mMouseButt == GLFW_MOUSE_BUTTON_LEFT) {
		//orbitamos la camara
		mCamera->orbit(mp.x * 0.05, mp.y);
	}
	else if (mMouseButt == GLFW_MOUSE_BUTTON_RIGHT) {
		//desplazamos la camara
		mCamera->moveLR(mp.x);
		mCamera->moveUD(mp.y);
	}

	mNeedsRedisplay = true;
}

void 
IG1App::mouseWheel(double dx, double dy)
{
	int ctrlLeft = glfwGetKey(mWindow, GLFW_KEY_LEFT_CONTROL);
	int ctrlRight = glfwGetKey(mWindow, GLFW_KEY_RIGHT_CONTROL);

	//si se esta pulsando ctrl, cambiamos la escala
	if (ctrlLeft == GLFW_PRESS || ctrlRight == GLFW_PRESS) {
		mCamera->setScale(dy);
	}
	//si no, la camara se mueve
	else {
		mCamera->moveFB(dy);
	}

	mNeedsRedisplay = true;
}