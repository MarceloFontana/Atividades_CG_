#include <iostream>
#include <string>
#include <assert.h>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include "../Exericio8/stb_image.h"
using namespace std;

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

int carregarTextura(string caminho);

// Protótipos das funções
int setupShader();
void processInput(glm::vec3& position, glm::vec3& scale);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 2000, HEIGHT = 1400;

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar* vertexShaderSource = "#version 450\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"layout(location = 2) in vec2 tex_coord;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec4 finalColor;\n"
"out vec2 texCoord;\n"
"void main()\n"
"{\n"
"gl_Position = projection * view * model * vec4(position, 1.0);\n"
"finalColor = vec4(color, 1.0);\n"
"texCoord = vec2(tex_coord.x, 1 - tex_coord.y);\n"
"}\0";

// Código fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar* fragmentShaderSource = "#version 450\n"
"in vec4 finalColor;\n"
"in vec2 texCoord;\n"
"out vec4 color;\n"
"uniform sampler2D tex_buffer;\n"
"void main()\n"
"{\n"
"color = texture(tex_buffer, texCoord);\n"
"}\n\0";

bool rotateX = false, rotateY = false, rotateZ = false;
bool moveXPos = false, moveXNeg = false;
bool moveYPos = false, moveYNeg = false;
bool moveZPos = false, moveZNeg = false;
bool scaleUp = false, scaleDown = false;

int loadSimpleOBJ(string filepath, int& nVerts, glm::vec3 color = glm::vec3(1.0, 0.0, 1.0));


int main()
{
    // Inicialização da GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Criação da janela GLFW
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola 3D -- Marcelo Luiz Fontana!", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Fazendo o registro da função de callback para a janela GLFW
    glfwSetKeyCallback(window, key_callback);

    // GLAD: carrega todos os ponteiros de funções da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Obtendo as informações de versão
    const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
    const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported " << version << std::endl;

    // Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Compilando e buildando o programa de shader
    GLuint shaderID = setupShader();

    // Gerando um buffer simples, com a geometria de um triângulo
    int nVerts;
    GLuint VAO = loadSimpleOBJ("cube.obj", nVerts, glm::vec3(0.0, 1.0, 1.0));


    GLuint texID = carregarTextura("Cube.png");

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(0.3f);

    glUseProgram(shaderID);
    glEnable(GL_DEPTH_TEST);

    // Setando a matriz de projeção
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    GLint projectionLoc = glGetUniformLocation(shaderID, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Setando a matriz de visualização
    glm::mat4 view = glm::lookAt(glm::vec3(1.5f, 1.5f, 1.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    GLint viewLoc = glGetUniformLocation(shaderID, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // Loop da aplicação - "game loop"
    while (!glfwWindowShouldClose(window))
    {
        // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
        glfwPollEvents();
        processInput(position, scale);

        // Limpa o buffer de cor
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // cor de fundo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float angle = (GLfloat)glfwGetTime() * 50.0f;

        // Configurações para o primeiro cubo
        glm::mat4 model1 = glm::mat4(1.0f);
        model1 = glm::translate(model1, position);
        model1 = glm::scale(model1, scale);
        if (rotateX)
        {
            model1 = glm::rotate(model1, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        else if (rotateY)
        {
            model1 = glm::rotate(model1, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else if (rotateZ)
        {
            model1 = glm::rotate(model1, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
        }

        GLint modelLoc = glGetUniformLocation(shaderID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model1));


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);
             
        // Desenha o primeiro cubo
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Desenha o segundo cubo
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);

        // Troca os buffers da tela
        glfwSwapBuffers(window);
    }

    // Pede pra OpenGL desalocar os buffers
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &shaderID);
    // Finaliza a execução da GLFW, limpando os recursos alocados por ela
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

    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
        moveYPos = true;
    if (key == GLFW_KEY_W && action == GLFW_RELEASE)
        moveYPos = false;

    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
        moveYNeg = true;
    if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        moveYNeg = false;

    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
        moveXNeg = true;
    if (key == GLFW_KEY_A && action == GLFW_RELEASE)
        moveXNeg = false;

    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
        moveXPos = true;
    if (key == GLFW_KEY_D && action == GLFW_RELEASE)
        moveXPos = false;

    if (key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT))
        moveZPos = true;
    if (key == GLFW_KEY_I && action == GLFW_RELEASE)
        moveZPos = false;

    if (key == GLFW_KEY_J && (action == GLFW_PRESS || action == GLFW_REPEAT))
        moveZNeg = true;
    if (key == GLFW_KEY_J && action == GLFW_RELEASE)
        moveZNeg = false;

    if (key == GLFW_KEY_U && (action == GLFW_PRESS || action == GLFW_REPEAT))
        scaleUp = true;
    if (key == GLFW_KEY_U && action == GLFW_RELEASE)
        scaleUp = false;

    if (key == GLFW_KEY_P && (action == GLFW_PRESS || action == GLFW_REPEAT))
        scaleDown = true;
    if (key == GLFW_KEY_P && action == GLFW_RELEASE)
        scaleDown = false;
}

void processInput(glm::vec3& position, glm::vec3& scale)
{
    if (moveXPos)
        position.x += 0.01f;
    if (moveXNeg)
        position.x -= 0.01f;
    if (moveYPos)
        position.y += 0.01f;
    if (moveYNeg)
        position.y -= 0.01f;
    if (moveZPos)
        position.z += 0.01f;
    if (moveZNeg)
        position.z -= 0.01f;
    if (scaleUp)
        scale *= 1.01f;
    if (scaleDown)
        scale *= 0.99f;
}

int setupShader()
{
    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Checando erros de compilação (exibição via log no terminal)
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Checando erros de compilação (exibição via log no terminal)
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // Linkando os shaders e criando o identificador do programa de shader
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Checando por erros de linkagem
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int loadSimpleOBJ(string filepath, int& nVerts, glm::vec3 color)
{
    vector <glm::vec3> vertices;
    vector <GLuint> indices;
    vector <glm::vec2> texCoords;
    vector <glm::vec3> normals;
    vector <GLfloat> vbuffer;

    ifstream inputFile;
    inputFile.open(filepath.c_str());
    if (inputFile.is_open())
    {
        char line[100];
        string sline;



        while (!inputFile.eof())
        {
            inputFile.getline(line, 100);
            sline = line;

            string word;

            istringstream ssline(line);
            ssline >> word;

            //cout << word << " ";
            if (word == "v")
            {
                glm::vec3 v;

                ssline >> v.x >> v.y >> v.z;

                vertices.push_back(v);
            }
            if (word == "vt")
            {
                glm::vec2 vt;

                ssline >> vt.s >> vt.t;

                texCoords.push_back(vt);
            }
            if (word == "vn")
            {
                glm::vec3 vn;

                ssline >> vn.x >> vn.y >> vn.z;

                normals.push_back(vn);
            }
            if (word == "f")
            {
                string tokens[3];

                ssline >> tokens[0] >> tokens[1] >> tokens[2];

                for (int i = 0; i < 3; i++)
                {
                    //Recuperando os indices de v
                    int pos = tokens[i].find("/");
                    string token = tokens[i].substr(0, pos);
                    int index = atoi(token.c_str()) - 1;
                    indices.push_back(index);

                    vbuffer.push_back(vertices[index].x);
                    vbuffer.push_back(vertices[index].y);
                    vbuffer.push_back(vertices[index].z);
                    vbuffer.push_back(color.r);
                    vbuffer.push_back(color.g);
                    vbuffer.push_back(color.b);

                    //Recuperando os indices de vts
                    tokens[i] = tokens[i].substr(pos + 1);
                    pos = tokens[i].find("/");
                    token = tokens[i].substr(0, pos);
                    index = atoi(token.c_str()) - 1;

                    vbuffer.push_back(texCoords[index].s);
                    vbuffer.push_back(texCoords[index].t);

                    //Recuperando os indices de vns
                    tokens[i] = tokens[i].substr(pos + 1);
                    index = atoi(tokens[i].c_str()) - 1;

                    vbuffer.push_back(normals[index].x);
                    vbuffer.push_back(normals[index].y);
                    vbuffer.push_back(normals[index].z);
                }
            }

        }

    }
    else
    {
        cout << "Problema ao encontrar o arquivo " << filepath << endl;
    }
    inputFile.close();

    GLuint VBO, VAO;

    nVerts = vbuffer.size() / 11; //Provisório

    //Geração do identificador do VBO
    glGenBuffers(1, &VBO);

    //Faz a conexão (vincula) do buffer como um buffer de array
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //Envia os dados do array de floats para o buffer da OpenGl
    glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(GLfloat), vbuffer.data(), GL_STATIC_DRAW);

    //Geração do identificador do VAO (Vertex Array Object)
    glGenVertexArrays(1, &VAO);

    // Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
    // e os ponteiros para os atributos 
    glBindVertexArray(VAO);

    //Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
    // Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
    // Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
    // Tipo do dado
    // Se está normalizado (entre zero e um)
    // Tamanho em bytes 
    // Deslocamento a partir do byte zero 

    //Atributo posição (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    //Atributo cor (r, g, b)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    //Atributo coordenada de textura (s, t)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    //Atributo normal do vértice (x, y, z)
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);


    // Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
    // atualmente vinculado - para que depois possamos desvincular com segurança
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
    glBindVertexArray(0);

    return VAO;


}

int carregarTextura(string caminho) {
    GLuint texID;

    // Gera o identificador da textura na memória 
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    //Ajusta os parâmetros de wrapping e filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Carregamento da imagem
    int width, height, nrChannels;
    unsigned char* data = stbi_load(caminho.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        if (nrChannels == 3) //jpg, bmp
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else //png
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;
}

