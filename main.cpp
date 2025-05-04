#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <stdio.h>
#include <string.h>

///Iþýklandýrma ve Normalizasyon için vertex shader ve fragment shaderýn tanýmlanamasý

//Vertex Shader

const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

//Fragment Shader
//Phong Aydýnlatma ile sahnede ýþýk-gölge efekti uygulanarak gerçekçilik saðlandý.
const char* fragmentShaderSource = R"(
//Shader fragment
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos = vec3(1.0, 2.0, 2.0);
uniform vec3 viewPos = vec3(0.0, 0.0, 3.0);
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 objectColor = vec3(0.8, 0.4, 0.2);

void main()
{
    // Ambient Aydýnlatma
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse Aydýnlatma 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular Aydýnlatma
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    
    //Phong Bu üç aydýnlatmanýnda toplamý
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
)";


// Ekran Ayarlarý
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// Kamera Ayarlarý
glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 5.0f);//Kamera konumu x=0,y=1,z=5
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);//Kameranýn baktýðý yön
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);//Kameranýn yukarýsý
float yaw = -90.0f, pitch = 0.0f;//Kameranýn açýsý 
float lastX = WIDTH / 2.0f, lastY = HEIGHT / 2.0f;//farenin pozisyonu için deðerler
bool firstMouse = true;
float deltaTime = 0.0f;	// Bir önceki frame ile bu frame arasýndaki zaman
float lastFrame = 0.0f;

// Küp vertex (pozisyon + normal)
float cubeVertices[] = {
    // pozisyonlar       // normal vektörleri
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
     0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
     0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
     0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
     0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
     0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

     0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
     0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
     0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
     0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
};

///Üçgen tanýmlama
float triangleVertices[] = {
    // Pozisyonlar        // Normal vektörler (opsiyonel - aydýnlatma için gerekli)
     0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,  // Üst tepe
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,  // Sol alt
     0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f   // Sað alt
};



// Fonksiyon bildirimi
void framebuffer_size_callback(GLFWwindow* window, int width, int height);//Frame buffer
void processInput(GLFWwindow* window);//Klavye tuþlarýnýn fonksiyon çaðrýsý
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
unsigned int createShaderProgram();

int main()
{
    //GLFW'ýn baþlatýlmasý
    if (!glfwInit())
    {
        printf("GLFW baþatýlamadý");
        glfwTerminate();
        return -1;
    }
    //OpenGL Versiyonu 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // Pencerenin oluþturulmasý
    GLFWwindow* mainwindow = glfwCreateWindow(WIDTH, HEIGHT, "Kutuphane Sahnesi Proje Odevi", NULL, NULL);
    if (!mainwindow)
    {
        printf("Pencere olusturulamadi!");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(mainwindow);
    glfwSetFramebufferSizeCallback(mainwindow, framebuffer_size_callback);
    glfwSetCursorPosCallback(mainwindow, mouse_callback);
    glfwSetInputMode(mainwindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLAD yükle
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("GLAD yuklenemedi!");
        return -1;
    }
    glEnable(GL_DEPTH_TEST);//Derinlik testi (z-buffering) açýlýr. 3D sahnelerde görsel doðruluk
    unsigned int shaderProgram = createShaderProgram();

    // VAO & VBO tanýmlamalarý
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);//poziyon bilgisi
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));//
    glEnableVertexAttribArray(1);




    // Ana döngü
    while (!glfwWindowShouldClose(mainwindow))
    {
        // Zaman hesapla
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Girdi iþle
        processInput(mainwindow);

        // Ekraný temizle
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glBindVertexArray(VAO);

        //Nesneler---------------------------------------
        glm::mat4 model;
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");

        // Zemin
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.05f, 2.5f));
        model = glm::scale(model, glm::vec3(10.0f, 0.1f, 17.5f));//x, 10 kat y 0.1 kat z
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(objectColorLoc, 0.3f, 0.3f, 0.3f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Duvarlar ve tavan
        glm::vec3 duvar_rengi(0.7f, 0.7f, 0.75f);
        glm::vec3 tavan_rengi(1.0f, 1.0f, 1.0f);

        //Kameranýn karþýsýndaki duvar 
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f, -5.0f));
        model = glm::scale(model, glm::vec3(10.0f, 5.0f, 0.1f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(objectColorLoc, 1, glm::value_ptr(duvar_rengi));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Baþlangýçta Kameranýn Solunda Kalan Duvar
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 2.5f, 2.5f));
        model = glm::scale(model, glm::vec3(0.1f, 5.0f, 17.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Baþlangýçta Kameranýn Saðýnda Kalan Duvar
        model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 2.5f, 2.5f));
        model = glm::scale(model, glm::vec3(0.1f, 5.0f, 17.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Baþlangýçta Kameranýn Arkasýnda Kalan Duvar
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f, 11.25f));
        model = glm::scale(model, glm::vec3(10.0f, 5.0f, 0.1f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(objectColorLoc, 1, glm::value_ptr(duvar_rengi));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Tavan 
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, 2.5f));
        model = glm::scale(model, glm::vec3(10.0f, 0.1f, 17.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(objectColorLoc, 1, glm::value_ptr(tavan_rengi));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //-----------------------------Kütüphanenin Oda Kýsmý--------------------------------------------------------------
        //Kitaplýk
        // Kitaplýk 1 arka panel ve yan paneller Sol Taraf
        glUniform3f(objectColorLoc, 0.5f, 0.2f, 0.1f);
        for (float c = 0; c < 8.4; c += 2.4) {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.80f, 2.5f, -2.0f + c));
            model = glm::scale(model, glm::vec3(0.1f, 5.0f, 2.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);

            for (float dz : {-0.95f, 0.95f}) {
                model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.65f, 2.5f, dz - 2.0f + c));
                model = glm::scale(model, glm::vec3(0.5f, 5.0f, 0.1f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            for (int i = 0; i < 4; i++) {
                model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.65f, 0.8f + i * 1.0f, -2.0f + c));
                model = glm::scale(model, glm::vec3(0.5f, 0.05f, 2.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
        //Sað Taraftaki Kitaplýklar
        for (float c = 0; c < 8.4; c += 2.4) {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(4.80f, 2.5f, -2.0f + c));
            model = glm::scale(model, glm::vec3(0.1f, 5.0f, 2.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);

            for (float dz : {-0.95f, 0.95f}) {
                model = glm::translate(glm::mat4(1.0f), glm::vec3(4.65f, 2.5f, dz - 2.0f + c));
                model = glm::scale(model, glm::vec3(0.5f, 5.0f, 0.1f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            //Raflar
            for (int i = 0; i < 4; i++) {
                model = glm::translate(glm::mat4(1.0f), glm::vec3(4.65f, 0.8f + i * 1.0f, -2.0f + c));
                model = glm::scale(model, glm::vec3(0.5f, 0.05f, 2.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        //Masa----------------------------------- 
        // Masanýn Üst Kýsmý
        for (int t = 1; t > -2; t -= 2) {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.8f * t, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(1.4f, 0.1f, 5.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(objectColorLoc, 0.7f, 0.4f, 0.2f);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            //Masaya panel
            model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.1f * t, 1.3f, 0.0f));
            model = glm::scale(model, glm::vec3(0.05f, 0.7f, 5.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
            //Masa Ayaklarý

            for (float dx : {-0.5f, 0.5f}) {
                for (float dz : {-2.0f, 2.0f}) {
                    model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.75f * t + dx, 0.45f, -0.1f + dz));
                    model = glm::scale(model, glm::vec3(0.1f, 1.0f, 0.1f));
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }
        //---------------------------Sandalye-------------------------------------------------------------
        for (int s = 1; s > -2; s -= 2) {
            for (float d = 0; d < 3.0; d += 2.4) {
                //Sandalyenin oturma alaný
                model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f * s, 0.65f, -1.5f + d));
                model = glm::scale(model, glm::vec3(0.9f, 0.1f, 1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform3f(objectColorLoc, 0.6f, 0.3f, 0.15f);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                //Sandalyenin sýrt kýsmý
                model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.45f * s, 1.0f, -1.5f + d));
                model = glm::scale(model, glm::vec3(0.1f, 0.9f, 1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 0, 36);

                //Sandalyenin ayaklarý
                for (float dx : {-0.4f, 0.4f}) {
                    for (float dz : {-0.4f, 0.4f}) {
                        model = glm::translate(glm::mat4(1.0f), glm::vec3((-3.05f + dx) * s, 0.35f, -1.5f + dz + d));
                        model = glm::scale(model, glm::vec3(0.1f, 0.7f, 0.1f));
                        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                        glDrawArrays(GL_TRIANGLES, 0, 36);
                    }

                }
            }
        }
        //Kapý

        model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.0f, 1.9f, 11.4f));
        model = glm::scale(model, glm::vec3(1.7f, 3.8f, 0.45f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);


        //Raf

        for (int k = 1; k > -2; k -= 2)
    {       model = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f * k, 2.5f, 11.15f));
        model = glm::scale(model, glm::vec3(2.0f, 0.1f, 0.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
        //Pencere ve tablo dekoratif--------------------
        //Pencere
        for (int pencere_boy = 0; pencere_boy < 4; pencere_boy += 2) {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f+pencere_boy, 2.5f, -4.88f));
            model = glm::scale(model, glm::vec3(0.1f, 2.0f, 0.1f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        for (int pencere_en = 0; pencere_en < 4; pencere_en += 2) {
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.0f , 1.45f + pencere_en, -4.88f));
        model = glm::scale(model, glm::vec3(2.0f, 0.1f, 0.1f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        //Arka Plan 
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f, -4.88f));
        model = glm::scale(model, glm::vec3(2.0f, 2.0f, 0.01f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 0.4f, 0.7f, 1.0f); // mavi gökyüzü
        glDrawArrays(GL_TRIANGLES, 0, 36);
        

        //-------------Kitaplar----------------------------------------------------------------------------
        // Kitap renkleri 1
        glm::vec3 bookColors[] = {
            glm::vec3(0.8f, 0.1f, 0.1f),
            glm::vec3(0.1f, 0.8f, 0.1f),
            glm::vec3(0.1f, 0.1f, 0.8f),
            glm::vec3(0.8f, 0.8f, 0.1f)
        };
        // Kitap renkleri 2
        glm::vec3 bookColors2[] = {
            glm::vec3(0.13f, 0.55f, 0.13f),
            glm::vec3(0.25f, 0.88f, 0.82f),
            glm::vec3(1.0f, 0.98f, 0.8f),
            glm::vec3(1.0f, 0.55f, 0.0f)
        };

        //Sol en köþedeki kitaplýk kitaplar Kapak gözükücek þekilde yerleþtirilenler
        for (float b = 0; b < 8.4; b += 2.4){
            for (int i = 0; i < 4; i++) {
                model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.75f, 1.1f + i * 1.0f, -1.5f + b));
                model = glm::scale(model, glm::vec3(0.1f, 0.6f, 0.3f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform3fv(objectColorLoc, 1, glm::value_ptr(bookColors[i % 4]));
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        //Sol en köþedeki kitaplýk kitaplar 2- Kitap Sýrtý gözükücek þekilde yerleþtirlenler 1
        for (int i = 0; i < 4; i++) {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.75f, 1.1f + i * 1.0f, -2.0f + b));
            model = glm::scale(model, glm::vec3(0.3f, 0.6f, 0.1f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(objectColorLoc, 1, glm::value_ptr(bookColors[i % 4]));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        //Sol en köþedeki kitaplýk kitaplar 2- Kitap Sýrtý gözükücek þekilde yerleþtirlenler 2
        for (float a = 0; a < 0.6; a += 0.12) {
            for (int i = 0; i < 4; i++) {
                model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.75f, 1.1f + i * 1.0f, -2.11 - a + b));
                model = glm::scale(model, glm::vec3(0.3f, 0.6f, 0.1f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform3fv(objectColorLoc, 1, glm::value_ptr(bookColors2[i % 4]));
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
    }

        //Sað en köþedeki kitaplýk kitaplar Kapak gözükücek þekilde yerleþtirilenler For döngüsüyle 
        for (float b = 0; b < 8.4; b += 2.4) {
            for (int i = 0; i < 4; i++) {
                model = glm::translate(glm::mat4(1.0f), glm::vec3(4.75f, 1.1f + i * 1.0f, -1.5f + b));
                model = glm::scale(model, glm::vec3(0.2f, 0.6f, 0.3f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform3fv(objectColorLoc, 1, glm::value_ptr(bookColors2[i % 4]));
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            //Sol en köþedeki kitaplýk kitaplar 2- Kitap Sýrtý gözükücek þekilde yerleþtirlenler 1
            for (int i = 0; i < 4; i++) {
                model = glm::translate(glm::mat4(1.0f), glm::vec3(4.75f, 1.1f + i * 1.0f, -2.0f + b));
                model = glm::scale(model, glm::vec3(0.4f, 0.6f, 0.1f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform3fv(objectColorLoc, 1, glm::value_ptr(bookColors2[i % 4]));
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            //Sol en köþedeki kitaplýk kitaplar 2- Kitap Sýrtý gözükücek þekilde yerleþtirlenler 2
            for (float a = 0; a < 0.6; a += 0.12) {
                for (int i = 0; i < 4; i++) {
                    model = glm::translate(glm::mat4(1.0f), glm::vec3(4.75f, 1.1f + i * 1.0f, -2.11 - a + b));
                    model = glm::scale(model, glm::vec3(0.4f, 0.6f, 0.1f));
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                    glUniform3fv(objectColorLoc, 1, glm::value_ptr(bookColors[i % 4]));
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }

        // --- MASA ÜSTÜ DEFTER ---
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-1.8f, 1.079f, 0.0f)); // masanýn hemen üstü
            model = glm::scale(model, glm::vec3(0.5f, 0.05f, 0.3f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 0.95f, 0.95f, 0.1f); // sarý defter
            glDrawArrays(GL_TRIANGLES, 0, 36);
        
         //Açýk Defter
            for (float m = 0; m < 0.31; m += 0.3) {
                //Defter kapaðý
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(-1.8f, 1.075f, 0.8+m)); // masanýn hemen üstü
                model = glm::scale(model, glm::vec3(0.5f, 0.01f, 0.3f));
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
                glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 0.95f, 0.95f, 0.1f); // sarý defter
                glDrawArrays(GL_TRIANGLES, 0, 36);

                for (float sayfa = 0; sayfa > -0.3; sayfa -= 0.08) {
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(-1.8f, 1.079f, 0.80 + m)); // masanýn hemen üstü
                    model = glm::scale(model, glm::vec3(0.48f, 0.005f, 0.3f+sayfa));
                    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
                    glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 1.0f, 1.0f, 1.0f); // beyaz defter
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        // Buffers
        glfwSwapBuffers(mainwindow);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// Görüntünün ekranda nereye çizileceði ekranýn tamamýna çizdirmiþ oluyoruz.
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// Klavyeden Tuþlarla Kontrol
void processInput(GLFWwindow* window)
{
    const float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)//Ekrandan çýkýþ
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

//Fare hareketlerinin kontrolü
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) //Ýlk fare hareketi
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    //Poziyon farklarý
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos; lastY = ypos;
    //Hassasiyet
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    //Yön açýlarý 
    yaw += xoffset;//saða sola dönüþ 
    pitch += yoffset;//Yukarý aþaðý dönüþ
    //Sýnýrlandýrma baþ aþaðý olamamsý için 90 derecede
    if (pitch > 89.0f)
    { 
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }
    //Yeni yön vektörünün hesabý
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}
//Shader kodlarýnýn derlenmesi
unsigned int createShaderProgram() {
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);//Vertex shader
    glShaderSource(vertex, 1, &vertexShaderSource, NULL);
    glCompileShader(vertex);
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);//Fragment Shader
    glShaderSource(fragment, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragment);
    //Shader programmý oluþturulur
    unsigned int shaderProgram = glCreateProgram();
    //Vertex shader la fragment shader programa baðlanýr
    glAttachShader(shaderProgram, vertex);
    glAttachShader(shaderProgram, fragment);
    //Programa linklenir
    glLinkProgram(shaderProgram);
    //LÝnklendiði için silinebirlirler.
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return shaderProgram;
}