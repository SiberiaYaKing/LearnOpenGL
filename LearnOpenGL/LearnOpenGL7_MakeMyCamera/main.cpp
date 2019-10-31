

#include <camera.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.h>

#include <iostream>
#include <math.h>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

//立方体
float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

//10个立方体的位置
glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

////格拉姆--施密特正交化求观察空间轴向
//glm::vec3 cameraPos = glm::vec3(0, 0, 3); //相机位置
//glm::vec3 cameraTarget = glm::vec3(0, 0, 0);  //相机目标
//glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);  //相机Z轴方向
//glm::vec3 up = glm::vec3(0, 1, 0);   //世界空间上方向
//glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection)); //相机X轴方向
//glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);   //相机Y轴方向

////相机参数
//glm::vec3 cameraPos = glm::vec3(0, 0, 3);  
//glm::vec3 cameraFront = glm::vec3(0, 0, -1);  
//glm::vec3 cameraUp = glm::vec3(0, 1, 0);  
//float fov = 45;
////俯仰角(pitch)和偏航角(yaw)
//float pitch = 0, yaw = -90; //设成-90是为了与相机初始化朝向一致

//使用自定义的相机类
Camera myCam(glm::vec3(0, 0, 3));

//固定时间差
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//窗口长宽
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//上一帧的鼠标坐标
float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;
float sensitivity = 0.05f;  //鼠标灵敏度值
bool firstMouse = true; //解决相机跳跃问题



inline int initWindow(GLFWwindow*&);
void processInput(GLFWwindow*);
inline unsigned int* loadVertex();
inline unsigned int & loadTexture(const char*, const char*);
//glm::mat4 my_loockAt_matrix(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp);

int main() {
	GLFWwindow* window = NULL;
	if (initWindow(window) == -1) return -1;

	Shader ourShader("shader.vs", "shader.fs");

	unsigned int VAO,VBO,IBO,texture1,texture2;
	unsigned int * vids =loadVertex();
	VAO = vids[0];
	VBO = vids[1];
	texture1= loadTexture("container.jpg", "jpg");
	texture2= loadTexture("awesomeface.png", "png");

	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);  

	glEnable(GL_DEPTH_TEST); //开启深度测试

	//渲染循环
	while (!glfwWindowShouldClose(window)) {
		//每帧计算新的时间差
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//清空颜色缓冲和深度缓冲
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);          

		//输入处理
		processInput(window);

		//渲染指令
		//绑定纹理坐标
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glBindVertexArray(VAO);

		ourShader.use();

		//float radius = 10;
		//float camX = sin(glfwGetTime())*radius;
		//float camZ = cos(glfwGetTime())*radius; //计算相机在圆上的位置	
		//view = glm::lookAt(glm::vec3(camX, 0, camZ), cameraTarget, up);  //构建lookat矩阵
		//glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		//glm::mat4 view = my_loockAt_matrix(myCam.Position, myCam.Position+myCam.Front, myCam.Up);
		glm::mat4 view = myCam.GetViewMatrix();
		ourShader.setMat4("view", view);

		glm::mat4 projection = glm::perspective(glm::radians(myCam.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);


		for (unsigned int i = 0; i < 10; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f*(i + 1)*glfwGetTime();
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			ourShader.setMat4("model", model);
			
			glDrawArrays(GL_TRIANGLES, 0, 36);  //画立方体
		}

		//检查并调用事件，交换缓冲
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

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
	//隐藏光标并让光标留在当前窗口
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//监听鼠标移动事件
	glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos) {
		//解决镜头一开始跳很远
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
		
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		myCam.ProcessMouseMovement(xoffset, yoffset);
		//xoffset *= sensitivity;
		//yoffset *= sensitivity;

		//yaw += xoffset;
		//pitch += yoffset;

		//if (pitch > 89)pitch = 89;
		//if (pitch < -89)pitch = -89;

		//glm::vec3 front;  //相机旋转会不断改变前方向，需要每帧计算
		//front.x = cos(glm::radians(pitch))*cos(glm::radians(yaw));// pitch与yaw是角度制，三角函数需要弧度制为参数
		//front.y = sin(glm::radians(pitch));
		//front.z = cos(glm::radians(pitch))*sin(glm::radians(yaw));
		//cameraFront = glm::normalize(front);
	});

	//当前上下文
	glfwMakeContextCurrent(window);

	//监听鼠标滚轮事件
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		myCam.ProcessMouseScroll(yoffset);
	});

	//初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}
	return 0;
}

//处理键鼠输入
void processInput(GLFWwindow* window) {
	//float cameraSpeed = 5*deltaTime;
	myCam.MovementSpeed = 5;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		//cameraPos += cameraSpeed * cameraFront;
		myCam.ProcessKeyboard(FORWARD, deltaTime);
	if(glfwGetKey(window,GLFW_KEY_S)==GLFW_PRESS)
		//cameraPos -= cameraSpeed * cameraFront;
		myCam.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		//cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp))*cameraSpeed;
		myCam.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		//cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp))*cameraSpeed;
		myCam.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
}

//载入顶点数据
unsigned int* loadVertex() {
	static unsigned int vids[3];

	//VAO理解为顶点属性集合(顶点的空间坐标，顶点的纹理坐标，顶点的颜色)
	glGenVertexArrays(1, &vids[0]); //生成vao对象
	glBindVertexArray(vids[0]);    //绑定vao

	//VBO理解为顶点包含的属性集合，顶点的空间坐标（顶点1坐标值，顶点2坐标值，顶点3坐标值）;
	glGenBuffers(1, &vids[1]);    //生成vbo对象
	glBindBuffer(GL_ARRAY_BUFFER, vids[1]);  //绑定vbo对象并设置其缓冲类型
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);   //将vob缓冲复制到显存

	//告诉显卡顶点属性的结构，让显卡解析
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	return vids;

}


unsigned int & loadTexture(const char* texPath, const char* texType) {
	static unsigned int texture;

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
		if (texType == "jpg")
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
	return texture;
}

//自己计算lookAt矩阵
//glm::mat4 my_loockAt_matrix(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
//	//格拉姆--施密特正交化 求摄像机三轴
//	glm::vec3 zaxis = glm::normalize(position - target);
//	glm::vec3 xaxis = glm::normalize(glm::cross(glm::normalize(up),zaxis));
//	glm::vec3 yaxis = glm::normalize(glm::cross(zaxis, xaxis));
//
//	//构建矩阵, 注意glm的矩阵是列优先矩阵！！
//	glm::mat4 translation = glm::mat4(1.0f);
//	translation[3][0] = -position.x;
//	translation[3][1] = -position.y;
//	translation[3][2] = -position.z;
//	glm::mat4 rotation = glm::mat4(1.0f);
//	rotation[0][0] = xaxis.x;
//	rotation[1][0] = xaxis.y;
//	rotation[2][0] = xaxis.z;
//	rotation[0][1] = yaxis.x;
//	rotation[1][1] = yaxis.y;
//	rotation[2][1] = yaxis.z;
//	rotation[0][2] = zaxis.x;
//	rotation[1][2] = zaxis.y;
//	rotation[2][2] = zaxis.z;
//
//	return rotation * translation;
//}



