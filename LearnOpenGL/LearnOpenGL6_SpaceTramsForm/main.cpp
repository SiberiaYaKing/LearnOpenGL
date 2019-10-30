


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

// ����
//float vertices[] = {
//	//     ---- λ�� ----      - �������� -
//		 0.5f,  0.5f, 0.0f,    1.0f, 1.0f,   // ����
//		 0.5f, -0.5f, 0.0f,    1.0f, 0.0f,   // ����
//		-0.5f, -0.5f, 0.0f,    0.0f, 0.0f,   // ����
//		-0.5f,  0.5f, 0.0f,    0.0f, 1.0f    // ����
//};
//unsigned int indices[] = { // ע��������0��ʼ! 
//	0, 1, 3, // ��һ��������
//	1, 2, 3  // �ڶ���������
//};

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
	//IBO = vids[2];  //������û��ibo
	texture1= loadTexture("container.jpg", "jpg");
	texture2= loadTexture("awesomeface.png", "png");

	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);  

	glEnable(GL_DEPTH_TEST); //������Ȳ���

	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window)) {
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

		ourShader.setFloat("mixValue", mixValue);

		for (unsigned int i = 0; i < 10; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 view = glm::mat4(1.0f);
			glm::mat4 projection = glm::mat4(1.0f);

			//model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));  //�����ε�model
			//model = glm::rotate(model, (float)glfwGetTime()*glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));  //�������������model
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f*(i + 1)*glfwGetTime();
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			view = glm::translate(view, glm::vec3(0, 0, -3.0f));
			projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

			ourShader.setMat4("model", model);
			ourShader.setMat4("view", view);
			ourShader.setMat4("projection", projection);

			glDrawArrays(GL_TRIANGLES, 0, 36);  //��������
		}
	
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,0);   //������

		//��鲢�����¼�����������
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &IBO);  //������û��ibo

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

	//������û��ibo
	//glGenBuffers(1, &vids[2]);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vids[2]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

