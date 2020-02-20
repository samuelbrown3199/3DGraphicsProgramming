#include <SDL2/SDL.h>

#include <exception>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

const GLchar* vertexShaderSrc =
"attribute vec3 a_Position;" \
"attribute vec4 a_Color;   " \
"                          "
"varying vec4 v_Color;     " \
"							  " \
"uniform mat4 u_Projection;		"\
"uniform mat4 u_Model;			"\
"								"\
"void main()                " \
"{						  " \
" gl_Position = u_Projection * u_Model * vec4(a_Position, 1.0);" \
" v_Color = a_Color;           "\
"}                          ";

const GLchar* fragmentShaderSrc =
"varying vec4 v_Color;    "\
"void main()              "\
"{                        "\
" gl_FragColor = v_Color;"\
"}";



int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        throw std::exception();
    }

    SDL_Window* window = SDL_CreateWindow("OpenGL Template",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

    if (!SDL_GL_CreateContext(window))
    {
        throw std::exception();
    }
    if (glewInit() != GLEW_OK)
    {
        throw std::exception();
    }

    const GLfloat positions[] =
    {
        0.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,

        1.0f, 0.5f, 0.0f,
        0.0f, -0.5f, 0.0f,
        1.5f, -0.5f, 0.0f
    };

    const GLfloat colors[] = 
    {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f
    };

    GLuint positionsVboID = 0;
    glGenBuffers(1, &positionsVboID);

    if (!positionsVboID)
    {
        throw std::exception();
    }
    glBindBuffer(GL_ARRAY_BUFFER, positionsVboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint colorsVboID = 0;
    glGenBuffers(1, &colorsVboID);

    if (!colorsVboID)
    {
        throw std::exception();
    }

    glBindBuffer(GL_ARRAY_BUFFER, colorsVboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint vaoId = 0;
    glGenVertexArrays(1, &vaoId);
    if (!vaoId)
    {
        throw std::exception();
    }
    glBindVertexArray(vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, positionsVboID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, colorsVboID);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID, 1, &vertexShaderSrc, NULL);
    glCompileShader(vertexShaderID);
    GLint success = 0;
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        throw std::exception();
    }

    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID, 1, &fragmentShaderSrc, NULL);
    glCompileShader(fragmentShaderID);
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        throw std::exception();
    }

    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glBindAttribLocation(programID, 0, "a_Position");
    glBindAttribLocation(programID, 1, "a_Color");

    glLinkProgram(programID);
    glGetProgramiv(programID, GL_LINK_STATUS, &success);

    if (!success)
    {
        throw std::exception();
    }

    glDetachShader(programID, vertexShaderID);
    glDeleteShader(vertexShaderID);
    glDetachShader(programID, fragmentShaderID);
    glDeleteShader(fragmentShaderID);

	GLint modelLoc = glGetUniformLocation(programID, "u_Model");
	GLint projectionLoc = glGetUniformLocation(programID, "u_Projection");

    bool quit = false;
	float angle = 0;

    while (!quit)
    {
        SDL_Event event = { 0 };

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
        }

		int width = 0;
		int height = 0;
		SDL_GetWindowSize(window, &width, &height);
		glViewport(0, 0, width, height);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_HEIGHT / (float)WINDOW_WIDTH, 0.1f, 100.0f);
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0, 0, -2.5f));
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));

		angle += 1.0f;

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(programID);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(vaoId);
        glDrawArrays(GL_TRIANGLES, 0, 3);

		projection = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT, 0.0f, 1.0f);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(100, WINDOW_HEIGHT - 100, 0));
		model = glm::scale(model, glm::vec3(100, 100, 1));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);
        glUseProgram(0);

        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}