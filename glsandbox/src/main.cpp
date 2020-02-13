#include <SDL2/SDL.h>

#include <exception>
#include <GL/glew.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

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
        0.5f, -0.5f, 0.0f
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
    glBindBuffer(GL_ARRAY_BUFFER, colorsVboID);

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
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    const GLchar* vertexShaderSrc =
        "attribute vec3 a_Position;" \
        "attribute vec4 a_Color;   " \
        "                          "  
        "varying vec4 v_Color;     " \
        "							  " \
        "void main()                " \
        "{						  " \
        " gl_Position = vec4(a_Position, 1.0);" \
        " v_Color = a_Color;           "\
        "}                          ";

    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID, 1, &vertexShaderSrc, NULL);
    glCompileShader(vertexShaderID);
    GLint success = 0;
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        throw std::exception();
    }

    const GLchar* fragmentShaderSrc =
        "varying vec4 v_Color;    "\
        "void main()              "\
        "{                        "\
        " gl_FragColor = v_Color;"\
        "}";

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

    bool quit = false;

    float r = 0, g = 0, b = 0;
    bool r1 = false, g1 = false, b1 = false;

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

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(programID);

        glBindVertexArray(vaoId);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);
        glUseProgram(0);

        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}