#include <iostream>
#include <string>
#include <assert.h>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

#include "Shader.h"

// Prot�tipo da fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Prot�tipos das fun��es
int setupGeometry();

struct Vertex {
	float x, y, z, r, g, b;
};

// Dimens�es da janela (pode ser alterado em tempo de execu��o)
const GLuint WIDTH = 1000, HEIGHT = 1000;

bool rotateX=false, rotateY=false, rotateZ=false;
int verticesSize = 0;

std::vector<std::string> splitString(const std::string& input, char delimiter) {
	std::vector<std::string> tokens;
	std::istringstream iss(input);
	std::string token;

	while (std::getline(iss, token, delimiter)) {
		tokens.push_back(token);
	}

	return tokens;
}

vector<float> parseObjToVertices(const std::string& filename) {
	std::ifstream file(filename); // Open the file for reading
	vector<float> uniqueVertices;
	vector<float> uniqueTextures;
	vector<float> uniqueNormals;
	vector<float> vertices;

	if (file.is_open()) {
		std::string line;

		while (std::getline(file, line)) {
			std::vector<std::string> row = splitString(line, ' ');

			bool isVertice = row[0] == "v";
			bool isFace = row[0] == "f";
			bool isTexture = row[0] == "vt";
			bool isNormal = row[0] == "vn";

			if (isVertice) {
				float x = std::stof(row[1]), y = std::stof(row[2]), z = std::stof(row[3]);

				uniqueVertices.push_back(x);
				uniqueVertices.push_back(y);
				uniqueVertices.push_back(z);
			}

			if (isTexture) {
				float x = std::stof(row[1]), y = std::stof(row[2]);

				uniqueTextures.push_back(x);
				uniqueTextures.push_back(y);
			}

			if (isNormal) {
				float x = std::stof(row[1]), y = std::stof(row[2]), z = std::stof(row[3]);

				uniqueNormals.push_back(x);
				uniqueNormals.push_back(y);
				uniqueNormals.push_back(z);
			}

			if (isFace) {
				std::vector<std::string> x = splitString(row[1], '/'), y = splitString(row[2], '/'), z = splitString(row[3], '/');

				int v1 = stoi(x[0]) - 1, v2 = stoi(y[0]) - 1, v3 = stoi(z[0]) - 1;
				int t1 = stoi(x[1]) - 1, t2 = stoi(y[1]) - 1, t3 = stoi(z[1]) - 1;
				int n1 = stoi(x[2]) - 1, n2 = stoi(y[2]) - 1, n3 = stoi(z[2]) - 1;

				//vertice 1
				vertices.push_back(uniqueVertices[v1 * 3]);
				vertices.push_back(uniqueVertices[v1 * 3 + 1]);
				vertices.push_back(uniqueVertices[v1 * 3 + 2]);

				//cor
				vertices.push_back(1.0);
				vertices.push_back(0.0);
				vertices.push_back(1.0);

				//textura
				vertices.push_back(uniqueTextures[t1 * 2]);
				vertices.push_back(uniqueTextures[t1 * 2 + 1]);

				//normal
				vertices.push_back(uniqueNormals[n1 * 3]);
				vertices.push_back(uniqueNormals[n1 * 3 + 1]);
				vertices.push_back(uniqueNormals[n1 * 3 + 2]);

				//vertice 2
				vertices.push_back(uniqueVertices[v2 * 3]);
				vertices.push_back(uniqueVertices[v2 * 3 + 1]);
				vertices.push_back(uniqueVertices[v2 * 3 + 2]);

				//cor
				vertices.push_back(1.0);
				vertices.push_back(0.0);
				vertices.push_back(1.0);

				//textura
				vertices.push_back(uniqueTextures[t2 * 2]);
				vertices.push_back(uniqueTextures[t2 * 2 + 1]);

				//normal
				vertices.push_back(uniqueNormals[n2 * 3]);
				vertices.push_back(uniqueNormals[n2 * 3 + 1]);
				vertices.push_back(uniqueNormals[n2 * 3 + 2]);

				//vertice 3
				vertices.push_back(uniqueVertices[v3 * 3]);
				vertices.push_back(uniqueVertices[v3 * 3 + 1]);
				vertices.push_back(uniqueVertices[v3 * 3 + 2]);

				//cor
				vertices.push_back(1.0);
				vertices.push_back(0.0);
				vertices.push_back(1.0);

				//textura
				vertices.push_back(uniqueTextures[t3 * 2]);
				vertices.push_back(uniqueTextures[t3 * 2 + 1]);

				//normal
				vertices.push_back(uniqueNormals[n3 * 3]);
				vertices.push_back(uniqueNormals[n3 * 3 + 1]);
				vertices.push_back(uniqueNormals[n3 * 3 + 2]);
			}
		}

		file.close();

		return vertices;
	}
	else {
		std::cout << "Unable to open the file: " << filename << std::endl;
	}
}

string parseMtlToFile(const std::string& filename) {
	std::ifstream file(filename);

	if (file.is_open()) {
		std::string line;

		while (std::getline(file, line)) {
			std::vector<std::string> row = splitString(line, ' ');

			if (row.size() == 0) {
				continue;
			}

			bool isTexturePath = row[0].compare("map_Kd") == 0;

			if (isTexturePath) {
				return row[1];
			}
		}

		std::cout << "Unable to find texture path in MTL file." << std::endl;
	}
	else {
		std::cout << "Unable to open the file: " << filename << std::endl;
	}
}

// Fun��o MAIN
int main()
{
	// Inicializa��o da GLFW
	glfwInit();

	// Cria��o da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Tarefa M4 -- Guilherme", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da fun��o de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d fun��es da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informa��es de vers�o
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	std::cout << "Renderer: " << renderer << endl;
	std::cout << "OpenGL version supported " << version << endl;

	// Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Compilando e buildando o programa de shader
	//GLuint shaderID = setupShader();
	Shader shader("../shaders/shaders.vs", "../shaders/shaders.fs");

	// Gerando um buffer simples, com a geometria de um tri�ngulo
	GLuint VAO = setupGeometry();

	glUseProgram(shader.ID);

	//Matriz de view -- posição e orientação da câmera
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	shader.setMat4("view", value_ptr(view));

	//Matriz de projeção perspectiva - definindo o volume de visualização (frustum)
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	shader.setMat4("projection", glm::value_ptr(projection));

	glm::mat4 model = glm::mat4(1);

	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	shader.setMat4("model", glm::value_ptr(model));

	shader.setFloat("ka", 0.4);
	shader.setFloat("kd", 0.5);
	shader.setFloat("ks", 0.5);
	shader.setFloat("q", 10);

	//Definindo as propriedades da fonte de luz
	shader.setVec3("lightPos", -2.0f, 10.0f, 3.0f);
	shader.setVec3("lightColor", 1.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	// Loop da aplica��o - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as fun��es de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		float angle = (GLfloat)glfwGetTime();

		model = glm::mat4(1); 
		if (rotateX)
		{
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
			
		}
		else if (rotateY)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

		}
		else if (rotateZ)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));

		}

		model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
		shader.setMat4("model", glm::value_ptr(model));
		
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, verticesSize);

		// Chamada de desenho - drawcall
		// CONTORNO - GL_LINE_LOOP
		
		//glDrawArrays(GL_LINE_STRIP, 0, verticesSize);
		glBindVertexArray(0);

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execu��o da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		rotateX = true;
		rotateY = false;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = true;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = false;
		rotateZ = true;
	}
}

int setupGeometry()
{
	vector<float> vertices = parseObjToVertices("../models/SuzanneTriTextured.obj");

	verticesSize = vertices.size();

	GLuint VBO, VAO;

	//Gera��o do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conex�o (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	//Gera��o do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v�rtices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	
	//Atributo posi��o (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo textura
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(2);

	//Atributo normal
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}



