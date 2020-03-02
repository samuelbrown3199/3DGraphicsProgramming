#include <SDL2/SDL.h>

#include <exception>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

// Needs to be defined before the include in exactly
// one compilation unit.
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

const GLchar* vertexShaderSrc =
"attribute vec3 a_Position;" \
"attribute vec4 a_TexCoord;   " \
"                          "
"varying vec4 v_TexCoord;     " \
"							  " \
"uniform mat4 u_Projection;		"\
"uniform mat4 u_Model;			"\
"								"\
"void main()                " \
"{						  " \
" gl_Position = u_Projection * u_Model * vec4(a_Position, 1.0);" \
" v_TexCoord = a_TexCoord;           "\
"}                          ";

const GLchar* fragmentShaderSrc =
"uniform sampler2D u_Texture;" \
"varying vec4 v_TexCoord;    "\
"void main()              "\
"{                        "\
" vec4 tex = texture2D(u_Texture, v_TexCoord);"
" gl_FragColor = tex;"\
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
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,

        -0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
    };
    /*const GLfloat colors[] = 
    {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f
    };*/
    const GLfloat textureCoords[] =
    {
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f
    };

    //load the texture
    int w = 0;
    int h = 0;
    unsigned char* data = stbi_load("image.png", &w, &h, NULL, 4);
    if (!data)
    {
        throw std::exception();
    }

    //create and bind a texture
    GLuint textureID = 0;
    glGenTextures(1, &textureID);

    if (!textureID)
    {
        throw std::exception();
    }
    glBindTexture(GL_TEXTURE_2D, textureID);
    //upload the image daya to the bound texture unit in the GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //free the loaded data because we now have a copy on the GPU
    free(data);
    //generate mipmap so the texture can be mapped correctly
    glGenerateMipmap(GL_TEXTURE_2D);
    //unbind the texture becase we are done operating on it
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint positionsVboID = 0;
    glGenBuffers(1, &positionsVboID);

    if (!positionsVboID)
    {
        throw std::exception();
    }
    glBindBuffer(GL_ARRAY_BUFFER, positionsVboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

   /* GLuint colorsVboID = 0;
    glGenBuffers(1, &colorsVboID);

    if (!colorsVboID)
    {
        throw std::exception();
    }

    glBindBuffer(GL_ARRAY_BUFFER, colorsVboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);*/

    GLuint textureVboID = 0;
    glGenBuffers(1, &textureVboID);
    if (!textureVboID)
    {
        throw std::exception();
    }
    glBindBuffer(GL_ARRAY_BUFFER, textureVboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);
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

    glBindBuffer(GL_ARRAY_BUFFER, textureVboID);
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
    glBindAttribLocation(programID, 1, "a_TexCoord");

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

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

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

        glClearColor(0.1f, 0.65f, 0.65f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programID);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glBindTexture(GL_TEXTURE_2D, textureID);
        glBindVertexArray(vaoId);
        glDrawArrays(GL_TRIANGLES, 0, (sizeof(positions) / sizeof(positions[0])) / 3);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0, 0, -7.5f));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glDrawArrays(GL_TRIANGLES, 0, 3);

		projection = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT, 0.0f, 1.0f);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(100, WINDOW_HEIGHT - 100, 0));
		model = glm::scale(model, glm::vec3(100, 100, 1));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindTexture(GL_TEXTURE, 0);
        glBindVertexArray(0);
        glUseProgram(0);

        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}