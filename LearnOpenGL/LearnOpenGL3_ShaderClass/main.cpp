#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.h>

#include <iostream>
#include <math.h>

using namespace std;




float vertices[] = {
	// 位置              // 颜色
	 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
	-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
	 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
};



inline int initWindow(GLFWwindow*&);
void processInput(GLFWwindow*);
inline void loadVertex(unsigned int &);
inline void loadShader(unsigned int &);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;






int main() {
	GLFWwindow* window = NULL;
	if (initWindow(window) == -1) return -1;

	unsigned int VAO;
	loadVertex(VAO);
	
	Shader ourShader("shader.vs","shader.fs");

	//渲染循环
	while (!glfwWindowShouldClose(window)) {
		//清空颜色缓冲
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  //状态设置
		glClear(GL_COLOR_BUFFER_BIT);          //状态使用

		//输入处理
		processInput(window);

		//渲染指令
		ourShader.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);   //画三角形

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


