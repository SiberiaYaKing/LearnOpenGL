#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
using namespace std;

//GLFW:����OpenGL�����ģ����崰�ڲ����Լ������û�����
//GLAD:����ͬƽ̨��OpenGL�ĺ���ָ��


float vertices[] = {
	// λ��              // ��ɫ
	 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // ����
	-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // ����
	 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // ����
};

//����������
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

////���ζ���
//float vertices[] = {
//	0.5f, 0.5f, 0.0f,   // ���Ͻ�
//	0.5f, -0.5f, 0.0f,  // ���½�
//	-0.5f, -0.5f, 0.0f, // ���½�
//	-0.5f, 0.5f, 0.0f   // ���Ͻ�
//};
//
////��������
//unsigned int indices[] = { // ע��������0��ʼ! 
//	0, 1, 3, // ��һ��������
//	1, 2, 3  // �ڶ���������
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

	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window)) {
		//�����ɫ����
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  //״̬����
		glClear(GL_COLOR_BUFFER_BIT);          //״̬ʹ��

		//���봦��
		processInput(window);

		//��Ⱦָ��
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//�߿�ģʽ��Ĭ��Ϊʵ��ģʽ
		glUseProgram(shaderProgram);  //������ɫ������
		////��ʱ��sin�仯����ɫ����ֵ
		//float greenValue = (sin(glfwGetTime()) / 2.0f) + 0.5f;
		////��ѯuniform����ɫ����λ��ֵ
		//int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		////����uniform��ֵ
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);   //��������
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,0);  //������

		//��鲢�����¼�����������
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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
}

//���붥������
void loadVertex(unsigned int& VAO) {
	//VAO���Ϊ�������Լ���(����Ŀռ����꣬������������꣬�������ɫ)
	glGenVertexArrays(1, &VAO); //����vao����
	glBindVertexArray(VAO);    //��vao

	//VBO���Ϊ������������Լ��ϣ�����Ŀռ����꣨����1����ֵ������2����ֵ������3����ֵ��
	unsigned int VBO;
	glGenBuffers(1, &VBO);    //����vbo����
	glBindBuffer(GL_ARRAY_BUFFER, VBO);  //��vbo���������仺������
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);   //��vob���帴�Ƶ��Դ�

	////IBO���Ϊ���㼯�ϵ���������
	//unsigned int IBO;
	//glGenBuffers(1, &IBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//�����Կ���ֵ�Ľṹ
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //���ö��㻺�����ݵĽ�����ʽ
	glEnableVertexAttribArray(0);     //��������
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


}

//������ɫ������
void loadShader(unsigned int& shaderProgram) {
	//������ɫ��
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); //����������ɫ��
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);  //Ϊ������ɫ������Դ��
	glCompileShader(vertexShader);  //���붥����ɫ��

	//ƬԪ��ɫ��
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//����ɫ���������ӵ���ɫ������
	shaderProgram = glCreateProgram();  //������ɫ���������
	glAttachShader(shaderProgram, vertexShader);     //������ɫ��
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);                 //����
	glDeleteShader(vertexShader);				  //ɾ����ɫ������
	glDeleteShader(fragmentShader);


}


