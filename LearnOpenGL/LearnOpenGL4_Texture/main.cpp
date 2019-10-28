

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.h>

#include <iostream>
#include <math.h>

#include "stb_image.h"
using namespace std;


float vertices[] = {
	//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
};
//float vertices[] = {
//	// positions          // colors           // 纹理坐标变为0-2
//	 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f, // top right
//	 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f, // bottom right
//	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
//	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f  // top left 
//};
//float vertices[] = {
//	// positions          // colors           // 纹理坐标缩小，显示中间部分
//	 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.55f, 0.55f, // top right
//	 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.55f, 0.45f, // bottom right
//	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.45f, 0.45f, // bottom left
//	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.45f, 0.55f  // top left 
//};
unsigned int indices[] = { // 注意索引从0开始! 
	0, 1, 3, // 第一个三角形
	1, 2, 3  // 第二个三角形
};
float mixValue = 0.2f;


inline int initWindow(GLFWwindow*&);
void processInput(GLFWwindow*);
inline void loadVertex(unsigned int &);
inline void loadTexture(unsigned int &, const char*, const char*);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


int main() {
	GLFWwindow* window = NULL;
	if (initWindow(window) == -1) return -1;

	Shader ourShader("shader.vs", "shader.fs");

	unsigned int VAO,texture1,texture2;
	loadVertex(VAO);
	loadTexture(texture1,"container.jpg","jpg");
	loadTexture(texture2, "awesomeface.png","png");

	//// texture 1
	//// ---------
	//glGenTextures(1, &texture1);
	//glBindTexture(GL_TEXTURE_2D, texture1);
	//// set the texture wrapping parameters
	////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); // note that we set the container wrapping method to GL_CLAMP_TO_BORDER
	////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	////float borderColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	////glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//// set texture filtering parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//// load image, create texture and generate mipmaps
	//int width, height, nrChannels;
	//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	//unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
	//if (data)
	//{
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}
	//else
	//{
	//	std::cout << "Failed to load texture" << std::endl;
	//}
	//stbi_image_free(data);
	//// texture 2
	//// ---------
	//glGenTextures(1, &texture2);
	//glBindTexture(GL_TEXTURE_2D, texture2);
	//// set the texture wrapping parameters
	////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// we want to repeat the awesomeface pattern so we kept it at GL_MIRRORED_REPEAT
	////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//// set texture filtering parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//// load image, create texture and generate mipmaps
	//data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
	//if (data)
	//{
	//	// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}
	//else
	//{
	//	std::cout << "Failed to load texture" << std::endl;
	//}
	//stbi_image_free(data);

	ourShader.use();
	glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0); //手动设置
	ourShader.setInt("texture2", 1);  //或使用着色器类设置

	//渲染循环
	while (!glfwWindowShouldClose(window)) {
		//清空颜色缓冲
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  //状态设置
		glClear(GL_COLOR_BUFFER_BIT);          //状态使用

		//输入处理
		processInput(window);

		//渲染指令
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		ourShader.setFloat("mixValue", mixValue);

		ourShader.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);   //画三角形

		//检查并调用事件，交换缓冲
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//glDeleteVertexArrays(1, &VAO);
	//glDeleteTextures(1, &texture1);
	//glDeleteTextures(1, &texture2);

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
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixValue += 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
		if (mixValue >= 1.0f)
			mixValue = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixValue -= 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
		if (mixValue <= 0.0f)
			mixValue = 0.0f;
	}
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

	//IBO理解为顶点集合的索引集合
	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//告诉显卡顶点属性的结构，让显卡解析
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8* sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	/*glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);*/
}


void loadTexture(unsigned int &texture,const char* texPath,const char* texType) {
	glGenTextures(1, &texture);
	//绑定纹理，设置纹理类型
	glBindTexture(GL_TEXTURE_2D, texture);
	//为当前绑定的纹理对象设置wrap,filter模式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//加载并生成纹理
	int width, height, nrChannels;  //宽度，高度，颜色通道数
	stbi_set_flip_vertically_on_load(true);  //反转纹理
	unsigned char *data = stbi_load(texPath, &width, &height, &nrChannels, 0);
	if (data) {
		if(texType=="jpg")
			//载入纹理数据到纹理对象
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if (texType == "png")
			//载入纹理数据到纹理对象
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		//生成mipmap
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	//释放纹理数据内存
	stbi_image_free(data);
}

