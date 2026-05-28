#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include <array>
#include <algorithm>
#include <vector>

#include"shaderClass.h"

class Camera
{
public:
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    
	bool firstClick = true; 

	int width;
	int height;

	float speed = 0.1f;
	float sensitivity = 100.0f;

	// Bezier movement
	bool followBezier = false;           // Si true, la cámara sigue la curva
	bool pWasPressed = false;            // Estado previo de la tecla P para detectar flancos
	float bezierT = 0.0f;                // Parámetro t (0..1) sobre la curva
	float bezierSpeed = 0.5f;            // Velocidad de avance sobre la curva (unidades t por segundo)
	double lastTime = 0.0;               // Tiempo previo para cálculo de delta
	// Curva compuesta: 4 segmentos cúbicos que forman un bucle cerrado alrededor del cubo
	std::array<std::array<glm::vec3,4>, 4> bezierSegs;
	// Construye una aproximación circular con 4 segmentos de Bézier
	void buildCircularBezier(float radius, float height);
	// LUT para parametrización por longitud de arco (para velocidad constante)
	std::vector<float> bezierLUT_T;
	std::vector<float> bezierLUT_len;
	float bezierTotalLength = 0.0f;
	float bezierArcPos = 0.0f; // posición a lo largo de la longitud total (0..bezierTotalLength)
	int bezierLUTSteps = 200; // resolución de la LUT


	Camera(int width, int height, glm::vec3 position);

	void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform);
	void Inputs(GLFWwindow* window);
	// Evalúa la curva de Bezier cúbica en t y devuelve la posición.
	glm::vec3 evaluateBezier(float t);
	// Evalúa un segmento de la curva Bezier compuesta (segment index, t 0..1)
	glm::vec3 evaluateBezierSegment(int segIdx, float t);
	// Construye la tabla LUT de longitud de arco para la curva actual.
	void buildBezierLUT();
	// Dado un desplazamiento de arco (0..bezierTotalLength) devuelve el t correspondiente.
	float getTForArcLength(float s);

};

#endif
