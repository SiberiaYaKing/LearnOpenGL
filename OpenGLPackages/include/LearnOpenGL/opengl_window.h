#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <exception>
//Òþ²Ø¿ØÖÆÌ¨´°¿Ú
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

class InitWindowException :public std::exception {
public:
	const char* what()const throw() {
		return "Failed to create GLFW window\n";
	}
};

class InitGLADException :public std::exception {
public:
	const char* what()const throw() {
		return "Failed to initialize GLAD\n";
	}
};

class OpenGLWindow {
private:
	GLFWwindow *window;
public:
	void initWindow(unsigned int width,unsigned int height,const std::string &name);
	void setCursorDisable();
	void setCursorEnable();
	void setCursorPosCallback(GLFWcursorposfun cbfun);
	void setScrollCallback(GLFWscrollfun cbfun);
	void swapBuffersAndPollEvents();
	GLFWwindow* getWindowPtr()const;
	bool isWindowClosed() const;
	~OpenGLWindow();
	GLFWwindow* operator &() const {
		return window;
	}
};

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
}

OpenGLWindow::~OpenGLWindow() {
	glfwTerminate();
}

void OpenGLWindow::setCursorDisable() {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void OpenGLWindow::setCursorEnable() {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void OpenGLWindow::setCursorPosCallback(GLFWcursorposfun cbfun)  {
	glfwSetCursorPosCallback(window, cbfun);
}

void OpenGLWindow::setScrollCallback(GLFWscrollfun cbfun)  {
	glfwSetScrollCallback(window, cbfun);
}

bool OpenGLWindow::isWindowClosed() const {
	return glfwWindowShouldClose(window);
}

void OpenGLWindow::swapBuffersAndPollEvents()  {
	glfwSwapBuffers(window);
	glfwPollEvents();
}

GLFWwindow* OpenGLWindow::getWindowPtr()const {
	return window;
}


