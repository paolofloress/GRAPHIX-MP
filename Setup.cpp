#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <string>
#include <iostream>

#include "Model3D.h"
#include "camera.h" // camera
#include "light.h"
#include "shader_m.h" // source: learnopengl "multiple lights"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

// settings
float screenWidth = 750.f;
float screenHeight = 750.f;
float movespeed = 0.2f;

// object rotation
float theta_x = 0.0f;
float theta_y = 0.0f;
float theta_z = 0.0f;

//old pitch and yaw
float oldPitch = 0;
float oldYaw = -90;

// 1st or 3rd, 1st = 0, 3rd = 1
bool perspControl = 0;
// camera (perspective or ortho)
bool cameraControl = 0;


// camera
OrthoCamera orthoCam(glm::vec3(0.0f, 10.f, 10.0f));
PerspectiveCamera persCam(glm::vec3(0.0f, 0.0f, 10.0f));

float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;

glm::vec4 vec(1.0f, 1.0f, 1.0f, 1.0f);

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//modval
float modx = 0;
float mody = 0;
float modz = -5.f;
float rotx = 0;
float roty = 0;
float rotz = 0;

// lighting
int cycleIntensity = 0;
float lightIntensity = 0.3f;

std::vector<Model3D> models;

glm::mat4 projection_matrix;
glm::mat4 view_matrix;

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(screenWidth, screenHeight, "Anthony Nocom", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    stbi_set_flip_vertically_on_load(true);

    Shader lightingShader("Shaders/sample.vert", "Shaders/MP_Light.frag");
    Shader skyboxShader("Shaders/skybox.vert", "Shaders/skybox.frag");

    // ^^^^^^^^^^^^^ skyboxShader basically does the same thing as the
    // ^^^^^^^^^^^^^ commented lines below, note the string names

    //// shader skybox
    //std::fstream skyboxVertSrc("Shaders/skybox.vert");
    //std::stringstream skyboxVertBuff;
    //skyboxVertBuff << skyboxVertSrc.rdbuf();
    //std::string skyboxVertS = skyboxVertBuff.str();
    //const char* sky_v = skyboxVertS.c_str();

    //std::fstream skyboxFragSrc("Shaders/skybox.frag");
    //std::stringstream skyboxFragBuff;
    //skyboxFragBuff << skyboxFragSrc.rdbuf();
    //std::string skyboxFragS = skyboxFragBuff.str();
    //const char* sky_f = skyboxFragS.c_str();

    //GLuint vertexShaderSkybox = glCreateShader(GL_VERTEX_SHADER);
    //glShaderSource(vertexShaderSkybox, 1, &sky_v, NULL);
    //glCompileShader(vertexShaderSkybox);

    //GLuint fragShaderSkybox = glCreateShader(GL_FRAGMENT_SHADER);
    //glShaderSource(fragShaderSkybox, 1, &sky_f, NULL);
    //glCompileShader(fragShaderSkybox);

    //GLuint skyboxShaderProg = glCreateProgram();
    //glAttachShader(skyboxShaderProg, vertexShaderSkybox);
    //glAttachShader(skyboxShaderProg, fragShaderSkybox);
    //glLinkProgram(skyboxShaderProg);

    //glDeleteShader(vertexShaderSkybox);
    //glDeleteShader(fragShaderSkybox);

    // TEXTURE 1, SHIP
    int img_width, img_height, color_channels;
    unsigned char* tex_bytes = stbi_load("3D/metal.jpg", // might be different for us
        &img_width,
        &img_height,
        &color_channels,
        0);

    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        img_width,
        img_height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        tex_bytes
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes);
    // TEXTURE 2, FISH (starfish)
    int img_width2, img_height2, color_channels2;
    unsigned char* tex_bytes2 = stbi_load("3D/starfish.jpg", // might be different for us
        &img_width2,
        &img_height2,
        &color_channels2,
        0);

    GLuint texture2;
    glGenTextures(1, &texture2);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        img_width2,
        img_height2,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        tex_bytes2
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes2);
    // TEXTURE 3, FISH1
    int img_width3, img_height3, color_channels3;
    unsigned char* tex_bytes3 = stbi_load("3D/fish.jpg", // might be different for us
        &img_width3,
        &img_height3,
        &color_channels3,
        0);

    GLuint texture3;
    glGenTextures(1, &texture3);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture3);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        img_width3,
        img_height3,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        tex_bytes3
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes3);
    // TEXTURE 4, SHARK
    int img_width4, img_height4, color_channels4;
    unsigned char* tex_bytes4 = stbi_load("3D/shark.jpg", // might be different for us
        &img_width4,
        &img_height4,
        &color_channels4,
        0);

    GLuint texture4;
    glGenTextures(1, &texture4);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture4);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        img_width4,
        img_height4,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        tex_bytes4
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes4);
    // TEXTURE 5, SHARK1
    int img_width5, img_height5, color_channels5;
    unsigned char* tex_bytes5 = stbi_load("3D/shark2.jpg", // might be different for us
        &img_width5,
        &img_height5,
        &color_channels5,
        0);

    GLuint texture5;
    glGenTextures(1, &texture5);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture5);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        img_width5,
        img_height5,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        tex_bytes5
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes5);
    // TEXTURE 6, ALLIGATOR
    int img_width6, img_height6, color_channels6;
    unsigned char* tex_bytes6 = stbi_load("3D/alligator.jpg", // might be different for us
        &img_width6,
        &img_height6,
        &color_channels6,
        0);

    GLuint texture6;
    glGenTextures(1, &texture6);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture6);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        img_width6,
        img_height6,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        tex_bytes6
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes6);
    // TEXTURE 7, FISH3
    int img_width7, img_height7, color_channels7;
    unsigned char* tex_bytes7 = stbi_load("3D/scale.jpg", // might be different for us
        &img_width7,
        &img_height7,
        &color_channels7,
        0);

    GLuint texture7;
    glGenTextures(1, &texture7);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture7);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        img_width7,
        img_height7,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        tex_bytes7
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes7);

    // object 1, UV
    std::string path = "3D/ship.obj"; // might be different for us
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;
    tinyobj::attrib_t attributes;
    bool success = tinyobj::LoadObj(&attributes,
        &shapes,
        &material,
        &warning,
        &error,
        path.c_str());

    std::vector<GLuint> mesh_indices;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(
            shapes[0].mesh.indices[i].vertex_index
        );
    }

    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;

    for (int i = 0; i < shapes[0].mesh.indices.size(); i += 3) {
        tinyobj::index_t vData1 = shapes[0].mesh.indices[i]; //vertex 1
        tinyobj::index_t vData2 = shapes[0].mesh.indices[i + 1]; // vert 2
        tinyobj::index_t vData3 = shapes[0].mesh.indices[i + 2]; // vert 3

        // components of vertex 1, 2, 3
        glm::vec3 v1 = glm::vec3(
            attributes.vertices[vData1.vertex_index * 3],
            attributes.vertices[vData1.vertex_index * 3 + 1],
            attributes.vertices[vData1.vertex_index * 3 + 2]
        );

        glm::vec3 v2 = glm::vec3(
            attributes.vertices[vData2.vertex_index * 3],
            attributes.vertices[vData2.vertex_index * 3 + 1],
            attributes.vertices[vData2.vertex_index * 3 + 2]
        );

        glm::vec3 v3 = glm::vec3(
            attributes.vertices[vData3.vertex_index * 3],
            attributes.vertices[vData3.vertex_index * 3 + 1],
            attributes.vertices[vData3.vertex_index * 3 + 2]
        );

        // components of uv
        glm::vec2 uv1 = glm::vec2(
            attributes.texcoords[vData1.texcoord_index * 2],
            attributes.texcoords[vData1.texcoord_index * 2 + 1]
        );

        glm::vec2 uv2 = glm::vec2(
            attributes.texcoords[vData2.texcoord_index * 2],
            attributes.texcoords[vData2.texcoord_index * 2 + 1]
        );
        glm::vec2 uv3 = glm::vec2(
            attributes.texcoords[vData3.texcoord_index * 2],
            attributes.texcoords[vData3.texcoord_index * 2 + 1]
        );

        glm::vec3 deltaPos1 = v2 - v1;
        glm::vec3 deltaPos2 = v3 - v1;

        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float r = 1.f / ((deltaUV1.x * deltaUV2.y) - (deltaUV1.y * deltaUV2.x));

        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

        tangents.push_back(tangent);
        tangents.push_back(tangent);
        tangents.push_back(tangent);

        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
    }

    std::vector <GLfloat> fullVertexData;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData = shapes[0].mesh.indices[i];

        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3)]
        );
        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3) + 1]
        );
        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3) + 2]
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3)]
        );
        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3) + 1]
        );
        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3) + 2]
        );

        fullVertexData.push_back(
            attributes.texcoords[(vData.texcoord_index * 2)]
        );
        fullVertexData.push_back(
            attributes.texcoords[(vData.texcoord_index * 2) + 1]
        );

        fullVertexData.push_back(
            tangents[i].x
        );
        fullVertexData.push_back(
            tangents[i].y
        );
        fullVertexData.push_back(
            tangents[i].z
        );

        fullVertexData.push_back(
            bitangents[i].x
        );
        fullVertexData.push_back(
            bitangents[i].y
        );
        fullVertexData.push_back(
            bitangents[i].z
        );
    }

    /*
  7--------6
 /|       /|
4--------5 |
| |      | |
| 3------|-2
|/       |/
0--------1
*/
//Vertices for the cube
    float skyboxVertices[]{
        -1.f, -1.f, 1.f, //0
        1.f, -1.f, 1.f,  //1
        1.f, -1.f, -1.f, //2
        -1.f, -1.f, -1.f,//3
        -1.f, 1.f, 1.f,  //4
        1.f, 1.f, 1.f,   //5
        1.f, 1.f, -1.f,  //6
        -1.f, 1.f, -1.f  //7
    };

    //Skybox Indices
    unsigned int skyboxIndices[]{
        1,2,6,
        6,5,1,

        0,4,7,
        7,3,0,

        4,5,6,
        6,7,4,

        0,3,2,
        2,1,0,

        0,1,5,
        5,4,0,

        3,7,6,
        6,2,3
    };

    // VAO 1
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GL_FLOAT) * fullVertexData.size(),
        fullVertexData.data(),
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        14 * sizeof(GL_FLOAT),
        (void*)0
    );
    GLintptr normPtr = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        14 * sizeof(GL_FLOAT),
        (void*)normPtr
    );
    GLintptr uvPtr = 6 * sizeof(float);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        14 * sizeof(GL_FLOAT),
        (void*)uvPtr
    );
    GLintptr tangentPtr = 8 * sizeof(float);
    glVertexAttribPointer(
        3,
        3,
        GL_FLOAT,
        GL_FALSE,
        14 * sizeof(GL_FLOAT),
        (void*)tangentPtr
    );
    GLintptr bitangentPtr = 11 * sizeof(float);
    glVertexAttribPointer(
        4,
        3,
        GL_FLOAT,
        GL_FALSE,
        14 * sizeof(GL_FLOAT),
        (void*)bitangentPtr
    );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // OBJECT 1, SUBMARINE
    // OBJECT 2, UV, FISH
    std::string path2 = "3D/fish.obj"; // might be different for us
    std::vector<tinyobj::shape_t> shapes2;
    std::vector<tinyobj::material_t> material2;
    std::string warning2, error2;
    tinyobj::attrib_t attributes2;
    bool success2 = tinyobj::LoadObj(&attributes2,
        &shapes2,
        &material2,
        &warning2,
        &error2,
        path2.c_str());

    std::vector<GLuint> mesh_indices2;
    for (int i = 0; i < shapes2[0].mesh.indices.size(); i++) {
        mesh_indices2.push_back(
            shapes2[0].mesh.indices[i].vertex_index
        );
    }

    for (int i = 0; i < shapes2[0].mesh.indices.size(); i += 3) {
        tinyobj::index_t vData12 = shapes2[0].mesh.indices[i]; //vertex 1
        tinyobj::index_t vData22 = shapes2[0].mesh.indices[i + 1]; // vert 2
        tinyobj::index_t vData32 = shapes2[0].mesh.indices[i + 2]; // vert 3

        // components of vertex 1, 2, 3
        glm::vec3 v1 = glm::vec3(
            attributes2.vertices[vData12.vertex_index * 3],
            attributes2.vertices[vData12.vertex_index * 3 + 1],
            attributes2.vertices[vData12.vertex_index * 3 + 2]
        );

        glm::vec3 v2 = glm::vec3(
            attributes2.vertices[vData22.vertex_index * 3],
            attributes2.vertices[vData22.vertex_index * 3 + 1],
            attributes2.vertices[vData22.vertex_index * 3 + 2]
        );

        glm::vec3 v3 = glm::vec3(
            attributes2.vertices[vData32.vertex_index * 3],
            attributes2.vertices[vData32.vertex_index * 3 + 1],
            attributes2.vertices[vData32.vertex_index * 3 + 2]
        );
    }

    std::vector <GLfloat> fullVertexData2;
    for (int i = 0; i < shapes2[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData2 = shapes2[0].mesh.indices[i];

        fullVertexData2.push_back(
            attributes2.vertices[(vData2.vertex_index * 3)]
        );
        fullVertexData2.push_back(
            attributes2.vertices[(vData2.vertex_index * 3) + 1]
        );
        fullVertexData2.push_back(
            attributes2.vertices[(vData2.vertex_index * 3) + 2]
        );

        fullVertexData2.push_back(
            attributes2.normals[(vData2.normal_index * 3)]
        );
        fullVertexData2.push_back(
            attributes2.normals[(vData2.normal_index * 3) + 1]
        );
        fullVertexData2.push_back(
            attributes2.normals[(vData2.normal_index * 3) + 2]
        );

        fullVertexData2.push_back(
            attributes2.texcoords[(vData2.texcoord_index * 2)]
        );
        fullVertexData2.push_back(
            attributes2.texcoords[(vData2.texcoord_index * 2) + 1]
        );
    }

    // VAO 2
    GLuint VAO2, VBO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GL_FLOAT) * fullVertexData2.size(),
        fullVertexData2.data(),
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)0
    );
    GLintptr normPtr2 = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)normPtr2
    );
    GLintptr uvPtr2 = 6 * sizeof(float);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)uvPtr2
    );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // OBJECT 2 END
    // OBJECT 3, UV, FISH1
    std::string path3 = "3D/fish1.obj"; // might be different for us
    std::vector<tinyobj::shape_t> shapes3;
    std::vector<tinyobj::material_t> material3;
    std::string warning3, error3;
    tinyobj::attrib_t attributes3;
    bool success3 = tinyobj::LoadObj(&attributes3,
        &shapes3,
        &material3,
        &warning3,
        &error3,
        path3.c_str());

    std::vector<GLuint> mesh_indices3;
    for (int i = 0; i < shapes3[0].mesh.indices.size(); i++) {
        mesh_indices3.push_back(
            shapes2[0].mesh.indices[i].vertex_index
        );
    }

    for (int i = 0; i < shapes3[0].mesh.indices.size(); i += 3) {
        tinyobj::index_t vData13 = shapes3[0].mesh.indices[i]; //vertex 1
        tinyobj::index_t vData23 = shapes3[0].mesh.indices[i + 1]; // vert 2
        tinyobj::index_t vData33 = shapes3[0].mesh.indices[i + 2]; // vert 3

        // components of vertex 1, 2, 3
        glm::vec3 v1 = glm::vec3(
            attributes3.vertices[vData13.vertex_index * 3],
            attributes3.vertices[vData13.vertex_index * 3 + 1],
            attributes3.vertices[vData13.vertex_index * 3 + 2]
        );

        glm::vec3 v2 = glm::vec3(
            attributes3.vertices[vData23.vertex_index * 3],
            attributes3.vertices[vData23.vertex_index * 3 + 1],
            attributes3.vertices[vData23.vertex_index * 3 + 2]
        );

        glm::vec3 v3 = glm::vec3(
            attributes3.vertices[vData33.vertex_index * 3],
            attributes3.vertices[vData33.vertex_index * 3 + 1],
            attributes3.vertices[vData33.vertex_index * 3 + 2]
        );
    }

    std::vector <GLfloat> fullVertexData3;
    for (int i = 0; i < shapes3[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData3 = shapes3[0].mesh.indices[i];

        fullVertexData3.push_back(
            attributes3.vertices[(vData3.vertex_index * 3)]
        );
        fullVertexData3.push_back(
            attributes3.vertices[(vData3.vertex_index * 3) + 1]
        );
        fullVertexData3.push_back(
            attributes3.vertices[(vData3.vertex_index * 3) + 2]
        );

        fullVertexData3.push_back(
            attributes3.normals[(vData3.normal_index * 3)]
        );
        fullVertexData3.push_back(
            attributes3.normals[(vData3.normal_index * 3) + 1]
        );
        fullVertexData3.push_back(
            attributes3.normals[(vData3.normal_index * 3) + 2]
        );

        fullVertexData3.push_back(
            attributes3.texcoords[(vData3.texcoord_index * 2)]
        );
        fullVertexData3.push_back(
            attributes3.texcoords[(vData3.texcoord_index * 2) + 1]
        );
    }

    // VAO 3
    GLuint VAO3, VBO3;
    glGenVertexArrays(1, &VAO3);
    glGenBuffers(1, &VBO3);
    glBindVertexArray(VAO3);
    glBindBuffer(GL_ARRAY_BUFFER, VBO3);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GL_FLOAT) * fullVertexData3.size(),
        fullVertexData3.data(),
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)0
    );
    GLintptr normPtr3 = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)normPtr3
    );
    GLintptr uvPtr3 = 6 * sizeof(float);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)uvPtr3
    );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // OBJECT 3 END
    // OBJECT 4, UV, shark
    std::string path4 = "3D/shark.obj"; // might be different for us
    std::vector<tinyobj::shape_t> shapes4;
    std::vector<tinyobj::material_t> material4;
    std::string warning4, error4;
    tinyobj::attrib_t attributes4;
    bool success4 = tinyobj::LoadObj(&attributes4,
        &shapes4,
        &material4,
        &warning4,
        &error4,
        path4.c_str());

    std::vector<GLuint> mesh_indices4;
    for (int i = 0; i < shapes4[0].mesh.indices.size(); i++) {
        mesh_indices4.push_back(
            shapes4[0].mesh.indices[i].vertex_index
        );
    }

    for (int i = 0; i < shapes4[0].mesh.indices.size(); i += 3) {
        tinyobj::index_t vData14 = shapes4[0].mesh.indices[i]; //vertex 1
        tinyobj::index_t vData24 = shapes4[0].mesh.indices[i + 1]; // vert 2
        tinyobj::index_t vData34 = shapes4[0].mesh.indices[i + 2]; // vert 3

        // components of vertex 1, 2, 3
        glm::vec3 v1 = glm::vec3(
            attributes4.vertices[vData14.vertex_index * 3],
            attributes4.vertices[vData14.vertex_index * 3 + 1],
            attributes4.vertices[vData14.vertex_index * 3 + 2]
        );

        glm::vec3 v2 = glm::vec3(
            attributes4.vertices[vData24.vertex_index * 3],
            attributes4.vertices[vData24.vertex_index * 3 + 1],
            attributes4.vertices[vData24.vertex_index * 3 + 2]
        );

        glm::vec3 v3 = glm::vec3(
            attributes4.vertices[vData34.vertex_index * 3],
            attributes4.vertices[vData34.vertex_index * 3 + 1],
            attributes4.vertices[vData34.vertex_index * 3 + 2]
        );
    }

    std::vector <GLfloat> fullVertexData4;
    for (int i = 0; i < shapes4[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData4 = shapes4[0].mesh.indices[i];

        fullVertexData4.push_back(
            attributes4.vertices[(vData4.vertex_index * 3)]
        );
        fullVertexData4.push_back(
            attributes4.vertices[(vData4.vertex_index * 3) + 1]
        );
        fullVertexData4.push_back(
            attributes4.vertices[(vData4.vertex_index * 3) + 2]
        );

        fullVertexData4.push_back(
            attributes4.normals[(vData4.normal_index * 3)]
        );
        fullVertexData4.push_back(
            attributes4.normals[(vData4.normal_index * 3) + 1]
        );
        fullVertexData4.push_back(
            attributes4.normals[(vData4.normal_index * 3) + 2]
        );

        fullVertexData4.push_back(
            attributes4.texcoords[(vData4.texcoord_index * 2)]
        );
        fullVertexData4.push_back(
            attributes4.texcoords[(vData4.texcoord_index * 2) + 1]
        );
    }

    // VAO 4
    GLuint VAO4, VBO4;
    glGenVertexArrays(1, &VAO4);
    glGenBuffers(1, &VBO4);
    glBindVertexArray(VAO4);
    glBindBuffer(GL_ARRAY_BUFFER, VBO4);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GL_FLOAT) * fullVertexData4.size(),
        fullVertexData4.data(),
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)0
    );
    GLintptr normPtr4 = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)normPtr4
    );
    GLintptr uvPtr4 = 6 * sizeof(float);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)uvPtr4
    );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // OBJECT 4 END
    // OBJECT 5, UV, HAMMERHEAD SHARK1
    std::string path5 = "3D/shark1.obj"; // might be different for us
    std::vector<tinyobj::shape_t> shapes5;
    std::vector<tinyobj::material_t> material5;
    std::string warning5, error5;
    tinyobj::attrib_t attributes5;
    bool success5 = tinyobj::LoadObj(&attributes5,
        &shapes5,
        &material5,
        &warning5,
        &error5,
        path5.c_str());

    std::vector<GLuint> mesh_indices5;
    for (int i = 0; i < shapes5[0].mesh.indices.size(); i++) {
        mesh_indices5.push_back(
            shapes5[0].mesh.indices[i].vertex_index
        );
    }

    for (int i = 0; i < shapes5[0].mesh.indices.size(); i += 3) {
        tinyobj::index_t vData15 = shapes5[0].mesh.indices[i]; //vertex 1
        tinyobj::index_t vData25 = shapes5[0].mesh.indices[i + 1]; // vert 2
        tinyobj::index_t vData35 = shapes5[0].mesh.indices[i + 2]; // vert 3

        // components of vertex 1, 2, 3
        glm::vec3 v1 = glm::vec3(
            attributes5.vertices[vData15.vertex_index * 3],
            attributes5.vertices[vData15.vertex_index * 3 + 1],
            attributes5.vertices[vData15.vertex_index * 3 + 2]
        );

        glm::vec3 v2 = glm::vec3(
            attributes5.vertices[vData25.vertex_index * 3],
            attributes5.vertices[vData25.vertex_index * 3 + 1],
            attributes5.vertices[vData25.vertex_index * 3 + 2]
        );

        glm::vec3 v3 = glm::vec3(
            attributes5.vertices[vData35.vertex_index * 3],
            attributes5.vertices[vData35.vertex_index * 3 + 1],
            attributes5.vertices[vData35.vertex_index * 3 + 2]
        );
    }

    std::vector <GLfloat> fullVertexData5;
    for (int i = 0; i < shapes5[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData5 = shapes5[0].mesh.indices[i];

        fullVertexData5.push_back(
            attributes5.vertices[(vData5.vertex_index * 3)]
        );
        fullVertexData5.push_back(
            attributes5.vertices[(vData5.vertex_index * 3) + 1]
        );
        fullVertexData5.push_back(
            attributes5.vertices[(vData5.vertex_index * 3) + 2]
        );

        fullVertexData5.push_back(
            attributes5.normals[(vData5.normal_index * 3)]
        );
        fullVertexData5.push_back(
            attributes5.normals[(vData5.normal_index * 3) + 1]
        );
        fullVertexData5.push_back(
            attributes5.normals[(vData5.normal_index * 3) + 2]
        );

        fullVertexData5.push_back(
            attributes5.texcoords[(vData5.texcoord_index * 2)]
        );
        fullVertexData5.push_back(
            attributes5.texcoords[(vData5.texcoord_index * 2) + 1]
        );
    }

    // VAO 2
    GLuint VAO5, VBO5;
    glGenVertexArrays(1, &VAO5);
    glGenBuffers(1, &VBO5);
    glBindVertexArray(VAO5);
    glBindBuffer(GL_ARRAY_BUFFER, VBO5);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GL_FLOAT) * fullVertexData5.size(),
        fullVertexData5.data(),
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)0
    );
    GLintptr normPtr5 = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)normPtr5
    );
    GLintptr uvPtr5 = 6 * sizeof(float);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)uvPtr5
    );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // OBJECT 5 END
    // OBJECT 6, UV, ALLIGATOR
    std::string path6 = "3D/alligator.obj"; // might be different for us
    std::vector<tinyobj::shape_t> shapes6;
    std::vector<tinyobj::material_t> material6;
    std::string warning6, error6;
    tinyobj::attrib_t attributes6;
    bool success6 = tinyobj::LoadObj(&attributes6,
        &shapes6,
        &material6,
        &warning6,
        &error6,
        path6.c_str());

    std::vector<GLuint> mesh_indices6;
    for (int i = 0; i < shapes6[0].mesh.indices.size(); i++) {
        mesh_indices6.push_back(
            shapes6[0].mesh.indices[i].vertex_index
        );
    }

    for (int i = 0; i < shapes6[0].mesh.indices.size(); i += 3) {
        tinyobj::index_t vData16 = shapes6[0].mesh.indices[i]; //vertex 1
        tinyobj::index_t vData26 = shapes6[0].mesh.indices[i + 1]; // vert 2
        tinyobj::index_t vData36 = shapes6[0].mesh.indices[i + 2]; // vert 3

        // components of vertex 1, 2, 3
        glm::vec3 v1 = glm::vec3(
            attributes6.vertices[vData16.vertex_index * 3],
            attributes6.vertices[vData16.vertex_index * 3 + 1],
            attributes6.vertices[vData16.vertex_index * 3 + 2]
        );

        glm::vec3 v2 = glm::vec3(
            attributes6.vertices[vData26.vertex_index * 3],
            attributes6.vertices[vData26.vertex_index * 3 + 1],
            attributes6.vertices[vData26.vertex_index * 3 + 2]
        );

        glm::vec3 v3 = glm::vec3(
            attributes6.vertices[vData36.vertex_index * 3],
            attributes6.vertices[vData36.vertex_index * 3 + 1],
            attributes6.vertices[vData36.vertex_index * 3 + 2]
        );
    }

    std::vector <GLfloat> fullVertexData6;
    for (int i = 0; i < shapes6[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData6 = shapes6[0].mesh.indices[i];

        fullVertexData6.push_back(
            attributes6.vertices[(vData6.vertex_index * 3)]
        );
        fullVertexData6.push_back(
            attributes6.vertices[(vData6.vertex_index * 3) + 1]
        );
        fullVertexData6.push_back(
            attributes6.vertices[(vData6.vertex_index * 3) + 2]
        );

        fullVertexData6.push_back(
            attributes6.normals[(vData6.normal_index * 3)]
        );
        fullVertexData6.push_back(
            attributes6.normals[(vData6.normal_index * 3) + 1]
        );
        fullVertexData6.push_back(
            attributes6.normals[(vData6.normal_index * 3) + 2]
        );

        fullVertexData6.push_back(
            attributes6.texcoords[(vData6.texcoord_index * 2)]
        );
        fullVertexData6.push_back(
            attributes6.texcoords[(vData6.texcoord_index * 2) + 1]
        );
    }

    // VAO 6
    GLuint VAO6, VBO6;
    glGenVertexArrays(1, &VAO6);
    glGenBuffers(1, &VBO6);
    glBindVertexArray(VAO6);
    glBindBuffer(GL_ARRAY_BUFFER, VBO6);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GL_FLOAT) * fullVertexData6.size(),
        fullVertexData6.data(),
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)0
    );
    GLintptr normPtr6 = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)normPtr6
    );
    GLintptr uvPtr6 = 6 * sizeof(float);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)uvPtr6
    );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // OBJECT 6 END
    // OBJECT 7, UV, FISH 3
    std::string path7 = "3D/fish2.obj"; // might be different for us
    std::vector<tinyobj::shape_t> shapes7;
    std::vector<tinyobj::material_t> material7;
    std::string warning7, error7;
    tinyobj::attrib_t attributes7;
    bool success7 = tinyobj::LoadObj(&attributes7,
        &shapes7,
        &material7,
        &warning7,
        &error7,
        path7.c_str());

    std::vector<GLuint> mesh_indices7;
    for (int i = 0; i < shapes7[0].mesh.indices.size(); i++) {
        mesh_indices7.push_back(
            shapes7[0].mesh.indices[i].vertex_index
        );
    }

    for (int i = 0; i < shapes7[0].mesh.indices.size(); i += 3) {
        tinyobj::index_t vData17 = shapes7[0].mesh.indices[i]; //vertex 1
        tinyobj::index_t vData27 = shapes7[0].mesh.indices[i + 1]; // vert 2
        tinyobj::index_t vData37 = shapes7[0].mesh.indices[i + 2]; // vert 3

        // components of vertex 1, 2, 3
        glm::vec3 v1 = glm::vec3(
            attributes7.vertices[vData17.vertex_index * 3],
            attributes7.vertices[vData17.vertex_index * 3 + 1],
            attributes7.vertices[vData17.vertex_index * 3 + 2]
        );

        glm::vec3 v2 = glm::vec3(
            attributes7.vertices[vData27.vertex_index * 3],
            attributes7.vertices[vData27.vertex_index * 3 + 1],
            attributes7.vertices[vData27.vertex_index * 3 + 2]
        );

        glm::vec3 v3 = glm::vec3(
            attributes7.vertices[vData37.vertex_index * 3],
            attributes7.vertices[vData37.vertex_index * 3 + 1],
            attributes7.vertices[vData37.vertex_index * 3 + 2]
        );
    }

    std::vector <GLfloat> fullVertexData7;
    for (int i = 0; i < shapes7[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData7 = shapes7[0].mesh.indices[i];

        fullVertexData7.push_back(
            attributes7.vertices[(vData7.vertex_index * 3)]
        );
        fullVertexData7.push_back(
            attributes7.vertices[(vData7.vertex_index * 3) + 1]
        );
        fullVertexData7.push_back(
            attributes7.vertices[(vData7.vertex_index * 3) + 2]
        );

        fullVertexData7.push_back(
            attributes7.normals[(vData7.normal_index * 3)]
        );
        fullVertexData7.push_back(
            attributes7.normals[(vData7.normal_index * 3) + 1]
        );
        fullVertexData7.push_back(
            attributes7.normals[(vData7.normal_index * 3) + 2]
        );

        fullVertexData7.push_back(
            attributes7.texcoords[(vData7.texcoord_index * 2)]
        );
        fullVertexData7.push_back(
            attributes7.texcoords[(vData7.texcoord_index * 2) + 1]
        );
    }

    // VAO 7
    GLuint VAO7, VBO7;
    glGenVertexArrays(1, &VAO7);
    glGenBuffers(1, &VBO7);
    glBindVertexArray(VAO7);
    glBindBuffer(GL_ARRAY_BUFFER, VBO7);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GL_FLOAT) * fullVertexData7.size(),
        fullVertexData7.data(),
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)0
    );
    GLintptr normPtr7 = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)normPtr7
    );
    GLintptr uvPtr7 = 6 * sizeof(float);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GL_FLOAT),
        (void*)uvPtr7
    );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // OBJECT 7 END

    // skybox VAO VBO EBO
    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    // add in skybox vertices to buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    // vertices only take xyz so only 3
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // skybox ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * 36, &skyboxIndices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);

    // SKYBOX FACES
    std::string facesSkybox[]{
        "Skybox/right.jpg",
        "Skybox/left.jpg",
        "Skybox/top.jpg",
        "Skybox/bottom.jpg",
        "Skybox/front.jpg",
        "Skybox/back.jpg",
    };

    // skybox tex
    unsigned int skyboxTex;
    //generate skybox textures
    glGenTextures(1, &skyboxTex);
    //initialize as cube map    
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
    // this is to avoid skybox from pixelating when too big or small
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // instead of st only- cubemaps rely on RST
    // this is to make sure texture stretched to the edge
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; i++) {
        // declare the width height and color channel of our skybox textures for later
        int w, h, skyCChannel;
        //temporarily disable flipping image
        stbi_set_flip_vertically_on_load(false);
        //load image as usual
        unsigned char* data = stbi_load(facesSkybox[i].c_str(), &w, &h, &skyCChannel, 0);

        if (data) {
            glTexImage2D(
                //cubemap face address start at positive_X (Right)
                //incrementing by 1
                //right > left > top > bot > front > back
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB, // some pngs dont have alpha
                w, // width
                h, // height
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE, // data type
                data // texture data itself
            );
            //cleanup
            stbi_image_free(data);
        }
    }
    stbi_set_flip_vertically_on_load(true);


    projection_matrix = glm::perspective(
        glm::radians(60.0f),
        screenHeight / screenWidth,
        0.1f,
        100.f
    );

    glm::mat3 identity_matrix3 = glm::mat3(1.0f);
    glm::mat4 identity_matrix4 = glm::mat4(1.0f);

    float x, y, z;
    x = y = z = 0.0f;

    float scale_x, scale_y, scale_z;
    scale_x = scale_y = scale_z = 5.f;

    float rot_x, rot_y, rot_z;
    rot_x = rot_y = rot_z = 0;
    rot_y = 1.0f;
    float theta_x = 90.0f; //default 0.f
    float theta_y = 180.0f; //default 0.f
    float theta_z = -180.0f; //default -90.f

    // LIGHTING
    DirectionLight dirLight;
    SpotLight spotLight = SpotLight(persCam.Position, persCam.Front);


    lightingShader.use();

    // ^^^^ this is same as the commented line below
    // glUseProgram(lightingShader) or glUseProgram(shaderProg)

    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);

    skyboxShader.use();
    // same as glUseProgram(skyboxShader) or glUseProgram(skyboxShaderProg)
    skyboxShader.setInt("skybox", 0);
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //processInput(window);

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        if (!cameraControl) {
            if (perspControl == 0) {
                persCam.Position = glm::vec3(modx, mody, modz);
                persCam.Yaw = -theta_y;
                persCam.Pitch = 0.f;
                persCam.updateCameraVectors();
                view_matrix = persCam.lookAtOrigin();

            }
            else {
                persCam.updateCameraVectors();
                view_matrix = persCam.lookAtOrigin();
                persCam.Position = glm::vec3(modx, mody, (modz + 10.f));
            }

        }
        else {
            view_matrix = orthoCam.lookFromAbove();
        }

        // use shader first
        skyboxShader.use();

        glm::mat4 sky_view = glm::mat4(1.f);
        sky_view = glm::mat4(
            glm::mat3(view_matrix)
        );

        // then set the uniform variables inside skybox.vert
        skyboxShader.setMat4("view", sky_view);
        skyboxShader.setMat4("projection", projection_matrix);

        // ^^^^^^^ this is same as the commented lines below.
        //unsigned int skyboxViewLoc = glGetUniformLocation(skyboxShaderProg, "view");
        //glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, glm::value_ptr(sky_view));
        // 
        //unsigned int skyboxProjLoc = glGetUniformLocation(skyboxShaderProg, "projection");
        //glUniformMatrix4fv(skyboxProjLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        // remember to activate shader
        lightingShader.use();
        lightingShader.setVec3("viewPos", persCam.Position);
        lightingShader.setFloat("material.shininess", 32.0f);

        // MAIN OBJECT SHIP
        glm::mat4 transformation_matrix = glm::mat4(1.0f);

        // transformation matrix
        transformation_matrix = glm::translate(transformation_matrix, glm::vec3(0.0f, -10.0f, -10.0f));
        transformation_matrix = glm::scale(transformation_matrix, glm::vec3(0.003f, 0.003f, 0.003f)); // DEFAULT 5/5/5
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_x), glm::normalize(glm::vec3(1, 0, 0))); // spin clockwise
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_y), glm::normalize(glm::vec3(0, 1, 0)));
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta_z), glm::normalize(glm::vec3(0, 0, 1)));

        // directional light
        lightingShader.setVec3("dirLight.direction", dirLight.direction);
        dirLight.updateLightStr(0.7f); // default is 0.35f, trying 0.7f
        lightingShader.setVec3("dirLight.ambient", dirLight.ambient);
        lightingShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        lightingShader.setVec3("dirLight.specular", dirLight.specular);

        // spotLight
        lightingShader.setVec3("spotLight.position", persCam.Position);
        lightingShader.setVec3("spotLight.direction", persCam.Front);
        spotLight.updateLightStr(lightIntensity); // default is 0.0f, trying 1.0f
        lightingShader.setVec3("spotLight.ambient", spotLight.ambient);
        lightingShader.setVec3("spotLight.diffuse", spotLight.diffuse);
        lightingShader.setVec3("spotLight.specular", spotLight.specular);
        lightingShader.setFloat("spotLight.constant", spotLight.constant);
        lightingShader.setFloat("spotLight.linear", spotLight.linear);
        lightingShader.setFloat("spotLight.quadratic", spotLight.quadratic);
        lightingShader.setFloat("spotLight.cutOff", spotLight.cutOff);
        lightingShader.setFloat("spotLight.outerCutOff", spotLight.outerCutOff);

        lightingShader.setMat4("transform", transformation_matrix);
        lightingShader.setMat4("projection", projection_matrix);
        lightingShader.setMat4("view", view_matrix);

        // texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 14);

        // OBJECT 2 STARFISH
        glm::mat4 transformation_matrix2 = glm::mat4(1.0f);
        // transformation matrix
        transformation_matrix2 = glm::translate(transformation_matrix2, glm::vec3(0.0f, -100.f, -1000.f));
        transformation_matrix2 = glm::scale(transformation_matrix2, glm::vec3(4.f, 4.f, 4.f)); // DEFAULT 5/5/5
        transformation_matrix2 = glm::rotate(transformation_matrix2, glm::radians(theta_x), glm::normalize(glm::vec3(1, 0, 0))); // spin clockwise
        transformation_matrix2 = glm::rotate(transformation_matrix2, glm::radians(theta_y), glm::normalize(glm::vec3(0, 1, 0)));
        transformation_matrix2 = glm::rotate(transformation_matrix2, glm::radians(theta_z), glm::normalize(glm::vec3(0, 0, 1)));
        lightingShader.setVec3("dirLight.direction", dirLight.direction);
        dirLight.updateLightStr(0.7f); // default is 0.35f, trying 0.7f
        lightingShader.setVec3("dirLight.ambient", dirLight.ambient);
        lightingShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        lightingShader.setVec3("dirLight.specular", dirLight.specular);
        lightingShader.setMat4("transform", transformation_matrix2);
        lightingShader.setMat4("projection", projection_matrix);
        lightingShader.setMat4("view", view_matrix);
        // texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, fullVertexData2.size() / 8);
        // OBJECT 3 TRAVIS SCOTT FISH
        glm::mat4 transformation_matrix3 = glm::mat4(1.0f);
        // transformation matrix
        transformation_matrix3 = glm::translate(transformation_matrix3, glm::vec3(0.0f, -10.0f, -300.0f));
        transformation_matrix3 = glm::scale(transformation_matrix3, glm::vec3(0.5f, 0.5f, 0.5f)); // DEFAULT 5/5/5
        transformation_matrix3 = glm::rotate(transformation_matrix3, glm::radians(theta_x), glm::normalize(glm::vec3(1, 0, 0))); // spin clockwise
        transformation_matrix3 = glm::rotate(transformation_matrix3, glm::radians(theta_y), glm::normalize(glm::vec3(0, 1, 0)));
        transformation_matrix3 = glm::rotate(transformation_matrix3, glm::radians(theta_z), glm::normalize(glm::vec3(0, 0, 1)));
        lightingShader.setVec3("dirLight.direction", dirLight.direction);
        dirLight.updateLightStr(0.7f); // default is 0.35f, trying 0.7f
        lightingShader.setVec3("dirLight.ambient", dirLight.ambient);
        lightingShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        lightingShader.setVec3("dirLight.specular", dirLight.specular);
        lightingShader.setMat4("transform", transformation_matrix3);
        lightingShader.setMat4("projection", projection_matrix);
        lightingShader.setMat4("view", view_matrix);
        // texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glBindVertexArray(VAO3);
        glDrawArrays(GL_TRIANGLES, 0, fullVertexData3.size() / 8);
        // OBJECT 4 SHARK
        glm::mat4 transformation_matrix4 = glm::mat4(1.0f);
        // transformation matrix
        transformation_matrix4 = glm::translate(transformation_matrix4, glm::vec3(-10.0f, -50.0f, -650.0f));
        transformation_matrix4 = glm::scale(transformation_matrix4, glm::vec3(0.05f, 0.05f, 0.05f)); // DEFAULT 5/5/5
        transformation_matrix4 = glm::rotate(transformation_matrix4, glm::radians(theta_x), glm::normalize(glm::vec3(1, 0, 0))); // spin clockwise
        transformation_matrix4 = glm::rotate(transformation_matrix4, glm::radians(theta_y), glm::normalize(glm::vec3(0, 1, 0)));
        transformation_matrix4 = glm::rotate(transformation_matrix4, glm::radians(theta_z), glm::normalize(glm::vec3(0, 0, 1)));
        lightingShader.setVec3("dirLight.direction", dirLight.direction);
        dirLight.updateLightStr(0.7f); // default is 0.35f, trying 0.7f
        lightingShader.setVec3("dirLight.ambient", dirLight.ambient);
        lightingShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        lightingShader.setVec3("dirLight.specular", dirLight.specular);
        lightingShader.setMat4("transform", transformation_matrix4);
        lightingShader.setMat4("projection", projection_matrix);
        lightingShader.setMat4("view", view_matrix);
        // texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture4);
        glBindVertexArray(VAO4);
        glDrawArrays(GL_TRIANGLES, 0, fullVertexData4.size() / 8);
        // OBJECT 5 HAMMERHEAD SHARK
        glm::mat4 transformation_matrix5 = glm::mat4(1.0f);
        // transformation matrix
        transformation_matrix5 = glm::translate(transformation_matrix5, glm::vec3(10.0f, -40.0f, -500.0f));
        transformation_matrix5 = glm::scale(transformation_matrix5, glm::vec3(0.5f, 0.5f, 0.5f)); // DEFAULT 5/5/5
        transformation_matrix5 = glm::rotate(transformation_matrix5, glm::radians(theta_x), glm::normalize(glm::vec3(1, 0, 0))); // spin clockwise
        transformation_matrix5 = glm::rotate(transformation_matrix5, glm::radians(theta_y), glm::normalize(glm::vec3(0, 1, 0)));
        transformation_matrix5 = glm::rotate(transformation_matrix5, glm::radians(theta_z), glm::normalize(glm::vec3(0, 0, 1)));
        lightingShader.setVec3("dirLight.direction", dirLight.direction);
        dirLight.updateLightStr(0.7f); // default is 0.35f, trying 0.7f
        lightingShader.setVec3("dirLight.ambient", dirLight.ambient);
        lightingShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        lightingShader.setVec3("dirLight.specular", dirLight.specular);
        lightingShader.setMat4("transform", transformation_matrix5);
        lightingShader.setMat4("projection", projection_matrix);
        lightingShader.setMat4("view", view_matrix);
        // texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture5);
        glBindVertexArray(VAO5);
        glDrawArrays(GL_TRIANGLES, 0, fullVertexData5.size() / 8);
        // OBJECT 6 ALLIGATOR
        glm::mat4 transformation_matrix6 = glm::mat4(1.0f);
        // transformation matrix
        transformation_matrix6 = glm::translate(transformation_matrix6, glm::vec3(-10.0f, -80.0f, -800.0f));
        transformation_matrix6 = glm::scale(transformation_matrix6, glm::vec3(5.f, 5.f, 5.f)); // DEFAULT 5/5/5
        transformation_matrix6 = glm::rotate(transformation_matrix6, glm::radians(theta_x), glm::normalize(glm::vec3(1, 0, 0))); // spin clockwise
        transformation_matrix6 = glm::rotate(transformation_matrix6, glm::radians(theta_y), glm::normalize(glm::vec3(0, 1, 0)));
        transformation_matrix6 = glm::rotate(transformation_matrix6, glm::radians(theta_z), glm::normalize(glm::vec3(0, 0, 1)));
        lightingShader.setVec3("dirLight.direction", dirLight.direction);
        dirLight.updateLightStr(0.7f); // default is 0.35f, trying 0.7f
        lightingShader.setVec3("dirLight.ambient", dirLight.ambient);
        lightingShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        lightingShader.setVec3("dirLight.specular", dirLight.specular);
        lightingShader.setMat4("transform", transformation_matrix6);
        lightingShader.setMat4("projection", projection_matrix);
        lightingShader.setMat4("view", view_matrix);
        // texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture6);
        glBindVertexArray(VAO6);
        glDrawArrays(GL_TRIANGLES, 0, fullVertexData6.size() / 8);
        // OBJECT 7 COWFISH
        glm::mat4 transformation_matrix7 = glm::mat4(1.0f);
        // transformation matrix
        transformation_matrix7 = glm::translate(transformation_matrix7, glm::vec3(10.0f, -30.0f, -450.0f));
        transformation_matrix7 = glm::scale(transformation_matrix7, glm::vec3(5.5f, 5.5f, 5.5f)); // DEFAULT 5/5/5
        transformation_matrix7 = glm::rotate(transformation_matrix7, glm::radians(theta_x), glm::normalize(glm::vec3(1, 0, 0))); // spin clockwise
        transformation_matrix7 = glm::rotate(transformation_matrix7, glm::radians(theta_y), glm::normalize(glm::vec3(0, 1, 0)));
        transformation_matrix7 = glm::rotate(transformation_matrix7, glm::radians(theta_z), glm::normalize(glm::vec3(0, 0, 1)));
        lightingShader.setVec3("dirLight.direction", dirLight.direction);
        dirLight.updateLightStr(0.7f); // default is 0.35f, trying 0.7f
        lightingShader.setVec3("dirLight.ambient", dirLight.ambient);
        lightingShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        lightingShader.setVec3("dirLight.specular", dirLight.specular);
        lightingShader.setMat4("transform", transformation_matrix7);
        lightingShader.setMat4("projection", projection_matrix);
        lightingShader.setMat4("view", view_matrix);
        // texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture7);
        glBindVertexArray(VAO7);
        glDrawArrays(GL_TRIANGLES, 0, fullVertexData7.size() / 8);

        processInput(window);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }
    // delete buffers
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (cameraControl == 0 && perspControl == 1) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        persCam.ProcessMouseMovement(xoffset, yoffset);
    }
}

// for camera movement, taken from learnopengl.com
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = static_cast<float>(2.5 * deltaTime);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && cameraControl == 0) {
        if (theta_y <= 90.f) {
            modx -= (movespeed * (theta_y / 90));
            modz -= (movespeed * ((90 - theta_y) / 90));
        }
        if (theta_y > 90.f && theta_y <= 180.f) {
            modx -= (movespeed * ((180 - theta_y) / 90));
            modz += (movespeed * ((theta_y - 90) / 90));
        }
        if (theta_y > 180.f && theta_y <= 270.f) {
            modx += (movespeed * ((theta_y - 180) / 90));
            modz += (movespeed * ((270 - theta_y) / 90));
        }
        if (theta_y > 270.f && theta_y <= 360.f) {
            modx += (movespeed * ((360 - theta_y) / 90));
            modz -= (movespeed * ((theta_y - 270) / 90));
        }
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && cameraControl == 0) {
        if (theta_y <= 90.f) {
            modx += (movespeed * (theta_y / 90));
            modz += (movespeed * ((90 - theta_y) / 90));
        }
        if (theta_y > 90.f && theta_y <= 180.f) {
            modx += (movespeed * ((180 - theta_y) / 90));
            modz -= (movespeed * ((theta_y - 90) / 90));
        }
        if (theta_y > 180.f && theta_y <= 270.f) {
            modx -= (movespeed * ((theta_y - 180) / 90));
            modz -= (movespeed * ((270 - theta_y) / 90));
        }
        if (theta_y > 270.f && theta_y <= 360.f) {
            modx -= (movespeed * ((360 - theta_y) / 90));
            modz += (movespeed * ((theta_y - 270) / 90));
        }
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && cameraControl == 0) {
        if (theta_y < 0.f)
            theta_y = 360.f;
        else
            theta_y -= movespeed;

    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && cameraControl == 0) {
        if (theta_y >= 360.f)
            theta_y = 0;
        else
            theta_y += movespeed;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && cameraControl == 0) {
        if (mody <= 0) {
            mody += movespeed;
            std::cout << mody << "m deep" << std::endl;
        }

    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && cameraControl == 0) {
        mody -= movespeed;
        std::cout << mody << "m deep" << std::endl;
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if (cameraControl == 0) {
            if (perspControl == 1) {
                perspControl = 0; //first person
                oldPitch = persCam.Pitch;
                oldYaw = persCam.Yaw;
            }
            else {
                //third person
                perspControl = 1;
                persCam.Pitch = oldPitch;
                persCam.Yaw = oldYaw;
            }
        }
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (cameraControl == 0)
            cameraControl = 1;
        else
            cameraControl = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        cycleIntensity += 1; // 0, 1, 2
        if (cycleIntensity > 2) // cannot go higher than 2. highest setting
            cycleIntensity = 0;
        switch (cycleIntensity) {
        case 0:
            lightIntensity = 0.3f;
            break;
        case 1:
            lightIntensity = 0.6f;
            break;
        case 2:
            lightIntensity = 0.9f;
            break;
        default:
            lightIntensity = 0.3f;
            break;
        }
    }
};