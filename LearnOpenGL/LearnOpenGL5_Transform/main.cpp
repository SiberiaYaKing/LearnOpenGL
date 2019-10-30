


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


float vertices[] = {
	//     ---- λ�� ----       ---- ��ɫ ----     - �������� -
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // ����
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // ����
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // ����
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // ����
};
unsigned int indices[] = { // ע��������0��ʼ! 
	0, 1, 3, // ��һ��������
	1, 2, 3  // �ڶ���������
};
float mixValue = 0.2f;


inline int initWindow(GLFWwindow*&);
void processInput(GLFWwindow*);
inline unsigned int* loadVertex();
inline unsigned int & loadTexture(const char*, const char*);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


int main() {
	GLFWwindow* window = NULL;
	if (initWindow(window) == -1) return -1;

	Shader ourShader("shader.vs", "shader.fs");

	unsigned int VAO,VBO,IBO,texture1,texture2;
	unsigned int * vids =loadVertex();
	VAO = vids[0];
	VBO = vids[1];
	IBO = vids[2];
	texture1= loadTexture("container.jpg", "jpg");
	texture2= loadTexture("awesomeface.png", "png");

	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);  //��ʹ����ɫ��������

	//glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
	//glm::mat4 trans;
	//trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
	//vec = trans * vec;
	//std::cout << vec.x << vec.y << vec.z << endl;

	//glm::mat4 trans;
	//trans = glm::rotate(trans, glm::radians(90.f), glm::vec3(0.0, 0.0, 1.0));
	//trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
	//glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(trans));

	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window)) {
		//�����ɫ����
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  //״̬����
		glClear(GL_COLOR_BUFFER_BIT);          //״̬ʹ��

		//���봦��
		processInput(window);

		//��Ⱦָ��
		//����������
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glBindVertexArray(VAO);

		ourShader.setFloat("mixValue", mixValue);

		//����任��˳��͵��õ�˳�����෴��
		glm::mat4 trans=glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(0.5, -0.5, 0.0));
		trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));  //��Z����ת
		unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);   //��������

		trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(-0.5f, 0.5f, 0.0f));
		float scaleAmount = sin(glfwGetTime());
		trans = glm::scale(trans, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//��鲢�����¼�����������
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);

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
	//��ǰ������
	glfwMakeContextCurrent(window);

	//��ʼ��GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}
	return 0;
}

//�����������
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

	//IBO���Ϊ���㼯�ϵ���������
	glGenBuffers(1, &vids[2]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vids[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//�����Կ��������ԵĽṹ�����Կ�����
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
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

