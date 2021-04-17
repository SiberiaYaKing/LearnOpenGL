#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <exception>

class OpenGLWindowException :public std::exception {
public:
	virtual const char* what()const throw() { return "OpenGLWindow Exception!!"; }
};

class InitWindowException :public OpenGLWindowException {
public:
	const char* what()const throw() {
		return "OpenGLWindow Exception!! Failed to create GLFW window\n";
	}
};

class InitGLADException :public OpenGLWindowException {
public:
	const char* what()const throw() {
		return "OpenGLWindow Exception!! Failed to initialize GLAD\n";
	}
};

class OpenGLWindow {
private:
	GLFWwindow *window = nullptr;
	void(*inputProcessor)(const OpenGLWindow& ) = nullptr;
public:
	void initWindow(unsigned int width,unsigned int height,const std::string &name);
	void setCursorDisable() const;
	void setCursorEnable() const;
	void setCursorPosCallback(GLFWcursorposfun cbfun);
	void setScrollCallback(GLFWscrollfun cbfun);
	void swapBuffersAndPollEvents();
	GLFWwindow* getWindowPtr()const;
	bool isWindowClosed() const;
	~OpenGLWindow();
	GLFWwindow* operator &() const {
		return window;
	}
	void closeWindow() const {
		glfwSetWindowShouldClose(window, true);
	}
	void setInputProcessor(void(*processor)(const OpenGLWindow&)) {
		inputProcessor = processor;
	}
	void processInput() const{
		if(window && inputProcessor)
			inputProcessor(*this);
	}
	void getWindowSize(int *width,int *height) const {
		if(window) glfwGetWindowSize(window, width, height);
	}
public:
	static float deltaTime;
	static float calculateDeltaTime() {
		static float lastFrame = 0.0f;
		float currentFrame = glfwGetTime();
		float deltaTime = currentFrame-lastFrame;
		lastFrame = currentFrame;
		OpenGLWindow::deltaTime = deltaTime;
		return deltaTime;
	}
	//static void openglClearDefaultFrameBuffer(GLfloat r,GLfloat g,GLfloat b,GLfloat a) {
	//	glClearColor(r,g,b,a);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//}
	static float lastX; 
	static float lastY;
};

float OpenGLWindow::deltaTime = 0.0f;
float OpenGLWindow::lastX = 0.0f;
float OpenGLWindow::lastY = 0.0f;

void OpenGLWindow::initWindow(unsigned int width, unsigned int height, const std::string &name) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		throw InitWindowException();
	}
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
	});

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		throw InitGLADException();
	}
	lastX = (float)width / 2;
	lastY = (float)height / 2;
}

OpenGLWindow::~OpenGLWindow() {
	glfwTerminate();
}

void OpenGLWindow::setCursorDisable() const{
	if(window) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void OpenGLWindow::setCursorEnable() const {
	if (window) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void OpenGLWindow::setCursorPosCallback(GLFWcursorposfun cbfun)  {
	if (window) glfwSetCursorPosCallback(window, cbfun);
}

void OpenGLWindow::setScrollCallback(GLFWscrollfun cbfun)  {
	if (window) glfwSetScrollCallback(window, cbfun);
}

bool OpenGLWindow::isWindowClosed() const {
	if (window) return glfwWindowShouldClose(window);
	else false;
}

void OpenGLWindow::swapBuffersAndPollEvents()  {
	if (window) {
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

GLFWwindow* OpenGLWindow::getWindowPtr()const {
	return window;
}


