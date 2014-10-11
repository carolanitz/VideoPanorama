#include "painter.hpp"

// Shader sources
const GLchar* vertexSource = R"(
    #version 300 es
    in vec2 position;
    in vec2 texcoord;
    out vec2 Texcoord;
    void main() {
      Texcoord = texcoord;
      gl_Position = vec4(position, 0.0, 1.0);
    }
)";
const GLchar* fragmentSource = R"(
    #version 300 es
    precision mediump float;
    in vec2 Texcoord;
    out vec4 outColor;
    uniform sampler2D image1;
    uniform sampler2D image2;
    void main() {
       outColor = mix(texture(image1, Texcoord), texture(image2, Texcoord), 0.5);
    }
)";


Painter::Painter()
{

}

Painter::~Painter()
{

}

void Painter::updateImage1(cv::Mat image)
{
  m_image1 = image;
}

void Painter::updateImage2(cv::Mat image)
{
  m_image2 = image;
}

void Painter::updateHomography1(cv::Mat H)
{

}

void Painter::updateHomography2(cv::Mat H)
{

}

void checkErrors() {

  GLenum errCode;
  const GLubyte *errString;

  if ((errCode = glGetError()) != GL_NO_ERROR) {
    printf("OpenGL Error: 0x%x\n", errCode);
  }

}

void Painter::setupOpenGL(int w, int h)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  // Create Vertex Array Object
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Create a Vertex Buffer Object and copy the vertex data to it
  GLuint vbo;
  glGenBuffers(1, &vbo);

  GLfloat vertices[] = {
    // Position   Texcoords
    -0.5f,  0.5f, 0.0f, 0.0f, // Top-left
     0.5f,  0.5f, 1.0f, 0.0f, // Top-right
     0.5f, -0.5f, 1.0f, 1.0f, // Bottom-right
    -0.5f, -0.5f, 0.0f, 1.0f  // Bottom-left
  };

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Create an element array
  GLuint ebo;
  glGenBuffers(1, &ebo);

  GLuint elements[] = {
    0, 1, 2,
    2, 3, 0
  };

  checkErrors();


  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

  // Create and compile the vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  glCompileShader(vertexShader);

  GLint status;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);

  if (status == GL_FALSE)
  {
    GLint infoLogLength;
    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);

    GLchar* strInfoLog = new GLchar[infoLogLength + 1];
    glGetShaderInfoLog(vertexShader, infoLogLength, NULL, strInfoLog);

    printf("Compilation error in shader: %s\n", strInfoLog);
    delete[] strInfoLog;
  }

  checkErrors();

  // Create and compile the fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);

  if (status == GL_FALSE)
  {
    GLint infoLogLength;
    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);

    GLchar* strInfoLog = new GLchar[infoLogLength + 1];
    glGetShaderInfoLog(fragmentShader, infoLogLength, NULL, strInfoLog);

    printf("Compilation error in shader: %s\n", strInfoLog);
    delete[] strInfoLog;
  }

  checkErrors();

  // Link the vertex and fragment shader into a shader program
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glBindFragDataLocation(shaderProgram, 0, "outColor");
  glLinkProgram(shaderProgram);


  GLint isLinked = 0;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);
  if(isLinked == GL_FALSE)
  {
    GLint maxLength = 0;
    glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

    //The maxLength includes the NULL character
    std::vector<GLchar> infoLog(maxLength);
    glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &infoLog[0]);

    printf("Error: %s\n", &infoLog[0]);

    //The program is useless now. So delete it.
    glDeleteProgram(shaderProgram);

    //Provide the infolog in whatever manor you deem best.
    //Exit with failure.
    exit(3);
  }


  glUseProgram(shaderProgram);

  checkErrors();

  // Specify the layout of the vertex data
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

  GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
  glEnableVertexAttribArray(texAttrib);
  glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

  // Load textures
  glGenTextures(2, m_textures);

  int width, height;
  unsigned char* image;


  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_textures[0]);
  glUniform1i(glGetUniformLocation(shaderProgram, "image1"), 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_textures[1]);
  glUniform1i(glGetUniformLocation(shaderProgram, "image2"), 1);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  checkErrors();
}

void Painter::draw()
{
  std::lock_guard<std::mutex> lock(m_mutex);

  if (!m_image1.empty())
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textures[0]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_image1.size().width, m_image1.size().height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_image1.data);
  }

  if (!m_image2.empty())
  {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_image2.size().width, m_image2.size().height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_image2.data);
  }

  // Clear the screen to black
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw a rectangle from the 2 triangles using 6 indices
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  checkErrors();

}

void Painter::cleanupOpenGL()
{
  std::lock_guard<std::mutex> lock(m_mutex);
}
