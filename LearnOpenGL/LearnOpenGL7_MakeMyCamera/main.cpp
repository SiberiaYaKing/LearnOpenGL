

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

//������
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

//10���������λ��
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

////����ķ--ʩ������������۲�ռ�����
//glm::vec3 cameraPos = glm::vec3(0, 0, 3); //���λ��
//glm::vec3 cameraTarget = glm::vec3(0, 0, 0);  //���Ŀ��
//glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);  //���Z�᷽��
//glm::vec3 up = glm::vec3(0, 1, 0);   //����ռ��Ϸ���
//glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection)); //���X�᷽��
//glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);   //���Y�᷽��

////�������
//glm::vec3 cameraPos = glm::vec3(0, 0, 3);  
//glm::vec3 cameraFront = glm::vec3(0, 0, -1);  
//glm::vec3 cameraUp = glm::vec3(0, 1, 0);  
//float fov = 45;
////������(pitch)��ƫ����(yaw)
//float pitch = 0, yaw = -90; //���-90��Ϊ���������ʼ������һ��

//ʹ���Զ���������
Camera myCam(glm::vec3(0, 0, 3));

//�̶�ʱ���
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//���ڳ���
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//��һ֡���������
float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;
float sensitivity = 0.05f;  //���������ֵ
bool firstMouse = true; //��������Ծ����



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

	glEnable(GL_DEPTH_TEST); //������Ȳ���

	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window)) {
		//ÿ֡�����µ�ʱ���
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//�����ɫ�������Ȼ���
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);          

		//���봦��
		processInput(window);

		//��Ⱦָ��
		//����������
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glBindVertexArray(VAO);

		ourShader.use();

		//float radius = 10;
		//float camX = sin(glfwGetTime())*radius;
		//float camZ = cos(glfwGetTime())*radius; //���������Բ�ϵ�λ��	
		//view = glm::lookAt(glm::vec3(camX, 0, camZ), cameraTarget, up);  //����lookat����
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
			
			glDrawArrays(GL_TRIANGLES, 0, 36);  //��������
		}

		//��鲢�����¼�����������
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	//�ͷ������Դ(��ֹ��Ⱦ)
	glfwTerminate();
	return 0;
}

int initWindow(GLFWwindow*& window) {
	//��ʼ��GLFW�⣬���ð汾�ţ�����ģʽ
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//mac os ��Ҫ���´���
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//����glfw����
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hello OpenGL", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}

	//�����ڴ�С������ʱ����Ⱦ���ӿ�Ҳ����Ӧ����
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
	});
	//���ع�겢�ù�����ڵ�ǰ����
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//��������ƶ��¼�
	glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos) {
		//�����ͷһ��ʼ����Զ
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

		//glm::vec3 front;  //�����ת�᲻�ϸı�ǰ������Ҫÿ֡����
		//front.x = cos(glm::radians(pitch))*cos(glm::radians(yaw));// pitch��yaw�ǽǶ��ƣ����Ǻ�����Ҫ������Ϊ����
		//front.y = sin(glm::radians(pitch));
		//front.z = cos(glm::radians(pitch))*sin(glm::radians(yaw));
		//cameraFront = glm::normalize(front);
	});

	//��ǰ������
	glfwMakeContextCurrent(window);

	//�����������¼�
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		myCam.ProcessMouseScroll(yoffset);
	});

	//��ʼ��GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}
	return 0;
}

//�����������
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

//���붥������
unsigned int* loadVertex() {
	static unsigned int vids[3];

	//VAO���Ϊ�������Լ���(����Ŀռ����꣬������������꣬�������ɫ)
	glGenVertexArrays(1, &vids[0]); //����vao����
	glBindVertexArray(vids[0]);    //��vao

	//VBO���Ϊ������������Լ��ϣ�����Ŀռ����꣨����1����ֵ������2����ֵ������3����ֵ��;
	glGenBuffers(1, &vids[1]);    //����vbo����
	glBindBuffer(GL_ARRAY_BUFFER, vids[1]);  //��vbo���������仺������
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);   //��vob���帴�Ƶ��Դ�

	//�����Կ��������ԵĽṹ�����Կ�����
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	return vids;

}


unsigned int & loadTexture(const char* texPath, const char* texType) {
	static unsigned int texture;

	glGenTextures(1, &texture);
	//������������������
	glBindTexture(GL_TEXTURE_2D, texture);
	//Ϊ��ǰ�󶨵������������wrap,filterģʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//���ز���������
	int width, height, nrChannels;  //��ȣ��߶ȣ���ɫͨ����
	stbi_set_flip_vertically_on_load(true);  //��ת����
	unsigned char *data = stbi_load(texPath, &width, &height, &nrChannels, 0);
	if (data) {
		if (texType == "jpg")
			//�����������ݵ��������
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if (texType == "png")
			//�����������ݵ��������
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		//����mipmap
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	//�ͷ����������ڴ�
	stbi_image_free(data);
	return texture;
}

//�Լ�����lookAt����
//glm::mat4 my_loockAt_matrix(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
//	//����ķ--ʩ���������� �����������
//	glm::vec3 zaxis = glm::normalize(position - target);
//	glm::vec3 xaxis = glm::normalize(glm::cross(glm::normalize(up),zaxis));
//	glm::vec3 yaxis = glm::normalize(glm::cross(zaxis, xaxis));
//
//	//��������, ע��glm�ľ����������Ⱦ��󣡣�
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



