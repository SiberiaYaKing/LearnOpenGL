#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
using namespace std;

//GLFW:创建OpenGL上下文，定义窗口参数以及处理用户输入
//GLAD:管理不同平台下OpenGL的函数指针


float vertices[] = {
	// 位置              // 颜色
	 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
	-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
	 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
};

//相连三角形
//float vertices[] = {
//	// first triangle
//	-0.9f, -0.5f, 0.0f,  // left 
//	-0.0f, -0.5f, 0.0f,  // right
//	-0.45f, 0.5f, 0.0f,  // top 
//	// second triangle
//	 0.0f, -0.5f, 0.0f,  // left
//	 0.9f, -0.5f, 0.0f,  // right
//	 0.45f, 0.5f, 0.0f   // top 
//};

////矩形顶点
//float vertices[] = {
//	0.5f, 0.5f, 0.0f,   // 右上角
//	0.5f, -0.5f, 0.0f,  // 右下角
//	-0.5f, -0.5f, 0.0f, // 左下角
//	-0.5f, 0.5f, 0.0f   // 左上角
//};
//
////矩形索引
//unsigned int indices[] = { // 注意索引从0开始! 
//	0, 1, 3, // 第一个三角形
//	1, 2, 3  // 第二个三角形
//};


inline int initWindow(GLFWwindow*&);
void processInput(GLFWwindow*);
inline void loadVertex(unsigned int &);
inline void loadShader(unsigned int &);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource = "#version 330 core\n"
"layout(location=0) in vec3 aPos;\n"
"layout(location=1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos,1.0);\n"
"	ourColor = aColor;\n"
"}\0";
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor,1.0);\n"
"}\0";



int main() {
	GLFWwindow* window = NULL;
	if (initWindow(window) == -1) return -1;

	unsigned int VAO, shaderProgram;
	loadVertex(VAO);
	loadShader(shaderProgram);

	//渲染循环
	while (!glfwWindowShouldClose(window)) {
		//清空颜色缓冲
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  //状态设置
		glClear(GL_COLOR_BUFFER_BIT);          //状态使用

		//输入处理
		processInput(window);

		//渲染指令
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//线框模式，默认为实体模式
		glUseProgram(shaderProgram);  //激活着色器程序
		////随时间sin变化的绿色分量值
		//float greenValue = (sin(glfwGetTime()) / 2.0f) + 0.5f;
		////查询uniform在着色器的位置值
		//int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		////设置uniform的值
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);   //画三角形
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,0);  //画矩形

		//检查并调用事件，交换缓冲
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//释放相关资源(终止渲染)
	glfwTerminate();
	return 0;
}

int initWindow(GLFWwindow*& window) {
	//初始化GLFW库，设置版本号，核心模式
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//mac os 需要以下代码
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//创建glfw窗口
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hello OpenGL", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}

	//当窗口大小调整的时候，渲染的视口也做相应调整
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
	});
	//当前上下文
	glfwMakeContextCurrent(window);

	//初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}
	return 0;
}

//处理键鼠输入
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

//载入顶点数据
void loadVertex(unsigned int& VAO) {
	//VAO理解为顶点属性集合(顶点的空间坐标，顶点的纹理坐标，顶点的颜色)
	glGenVertexArrays(1, &VAO); //生成vao对象
	glBindVertexArray(VAO);    //绑定vao

	//VBO理解为顶点包含的属性集合，顶点的空间坐标（顶点1坐标值，顶点2坐标值，顶点3坐标值）
	unsigned int VBO;
	glGenBuffers(1, &VBO);    //生成vbo对象
	glBindBuffer(GL_ARRAY_BUFFER, VBO);  //绑定vbo对象并设置其缓冲类型
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);   //将vob缓冲复制到显存

	////IBO理解为顶点集合的索引集合
	//unsigned int IBO;
	//glGenBuffers(1, &IBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//告诉显卡，值的结构
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //设置顶点缓冲数据的解析方式
	glEnableVertexAttribArray(0);     //启动解析
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


}

//载入着色器代码
void loadShader(unsigned int& shaderProgram) {
	//顶点着色器
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); //创建顶点着色器
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);  //为顶点着色器设置源码
	glCompileShader(vertexShader);  //编译顶点着色器

	//片元着色器
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//将着色器对象链接到着色器程序
	shaderProgram = glCreateProgram();  //创建着色器程序对象
	glAttachShader(shaderProgram, vertexShader);     //附加着色器
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);                 //链接
	glDeleteShader(vertexShader);				  //删除着色器对象
	glDeleteShader(fragmentShader);


}


