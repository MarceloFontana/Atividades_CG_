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

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Protótipos das funções
int setupShader();
int setupGeometry();
void processInput(glm::vec3& position, glm::vec3& scale);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 2000, HEIGHT = 1400;

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar* vertexShaderSource = "#version 450\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec4 finalColor;\n"
"void main()\n"
"{\n"
"gl_Position = projection * view * model * vec4(position, 1.0);\n"
"finalColor = vec4(color, 1.0);\n"
"}\0";

// Código fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar* fragmentShaderSource = "#version 450\n"
"in vec4 finalColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = finalColor;\n"
"}\n\0";

bool rotateX = false, rotateY = false, rotateZ = false;
bool moveXPos = false, moveXNeg = false;
bool moveYPos = false, moveYNeg = false;
bool moveZPos = false, moveZNeg = false;
bool scaleUp = false, scaleDown = false;

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
    GLuint VAO = setupGeometry();

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

        // Desenha o primeiro cubo
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Configurações para o segundo cubo
        glm::mat4 model2 = glm::translate(model1, glm::vec3(1.5f, 0.0f, 0.0f)); // Desloca o segundo cubo
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

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

int setupGeometry()
{
    GLfloat vertices[] = {
       
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  

       
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  

         -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  
          0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  
          0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  
          0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  
         -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  
         -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  

         -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  
          0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  
          0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  
          0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  
         -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  
         -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f   
    };

    GLuint VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}


