#include "Camera.h"
#include <cmath>

Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	// Construimos una curva compuesta que hace un bucle alrededor del cubo
	float r = 2.0f;    // radio desde el centro del cubo
	float h = 0.8f;    // altura sobre el eje Y
	buildCircularBezier(r, h);
	// Iniciar la cámara en el inicio de la curva
	Position = evaluateBezierSegment(0, 0.0f);
	Orientation = glm::normalize(glm::vec3(0.0f) - Position);
	lastTime = glfwGetTime();
	// Construir LUT para parametrización por longitud de arco
	buildBezierLUT();
}

// Evalúa una curva Bézier cúbica dado 4 puntos de control y parámetro t
static glm::vec3 evalCubic(const std::array<glm::vec3,4>& P, float t)
{
	float u = 1.0f - t;
	float b0 = u * u * u;
	float b1 = 3.0f * u * u * t;
	float b2 = 3.0f * u * t * t;
	float b3 = t * t * t;
	return b0 * P[0] + b1 * P[1] + b2 * P[2] + b3 * P[3];
}

// Evaluación en la curva compuesta: segIdx en [0,3], t en [0,1]
glm::vec3 Camera::evaluateBezierSegment(int segIdx, float t)
{
	segIdx = segIdx % 4;
	return evalCubic(bezierSegs[segIdx], t);
}

// Evaluación global por t entre 0..1, mapeando sobre los 4 segmentos
glm::vec3 Camera::evaluateBezier(float t)
{
	// dividir t en segmento y t local
	float segF = t * 4.0f;
	int seg = std::min(3, (int)segF);
	float localT = segF - seg;
	return evaluateBezierSegment(seg, localT);
}

void Camera::buildBezierLUT()
{
	bezierLUT_T.clear();
	bezierLUT_len.clear();
	bezierLUT_T.reserve(bezierLUTSteps + 1);
	bezierLUT_len.reserve(bezierLUTSteps + 1);

	float prevT = 0.0f;
	glm::vec3 prevP = evaluateBezier(0.0f);
	float acc = 0.0f;
	bezierLUT_T.push_back(0.0f);
	bezierLUT_len.push_back(0.0f);

	for (int i = 1; i <= bezierLUTSteps; ++i)
	{
		float t = (float)i / bezierLUTSteps;
		glm::vec3 p = evaluateBezier(t);
		acc += glm::length(p - prevP);
		bezierLUT_T.push_back(t);
		bezierLUT_len.push_back(acc);
		prevP = p;
	}
	bezierTotalLength = acc;
	bezierArcPos = 0.0f;
}

// Genera 4 segmentos Bézier aproximando un cuarto de circunferencia cada uno
void Camera::buildCircularBezier(float radius, float height)
{
	// Usamos tangentes para aproximar un círculo con 4 curvas cubic Bezier.
	float k = 0.552284749831f; // aproximación para un cuarto de círculo

	// puntos clave en 0, 90, 180, 270 grados
	glm::vec3 p0 = glm::vec3(radius, height, 0.0f);
	glm::vec3 p1 = glm::vec3(0.0f, height, -radius);
	glm::vec3 p2 = glm::vec3(-radius, height, 0.0f);
	glm::vec3 p3 = glm::vec3(0.0f, height, radius);

	// construimos cada segmento con sus 4 puntos de control
	bezierSegs[0][0] = p0;
	bezierSegs[0][1] = p0 + glm::vec3(0.0f, 0.0f, -k * radius);
	bezierSegs[0][2] = p1 + glm::vec3(k * radius, 0.0f, 0.0f);
	bezierSegs[0][3] = p1;

	bezierSegs[1][0] = p1;
	bezierSegs[1][1] = p1 + glm::vec3(k * radius, 0.0f, 0.0f);
	bezierSegs[1][2] = p2 + glm::vec3(0.0f, 0.0f, k * radius);
	bezierSegs[1][3] = p2;

	bezierSegs[2][0] = p2;
	bezierSegs[2][1] = p2 + glm::vec3(0.0f, 0.0f, k * radius);
	bezierSegs[2][2] = p3 + glm::vec3(-k * radius, 0.0f, 0.0f);
	bezierSegs[2][3] = p3;

	bezierSegs[3][0] = p3;
	bezierSegs[3][1] = p3 + glm::vec3(-k * radius, 0.0f, 0.0f);
	bezierSegs[3][2] = p0 + glm::vec3(0.0f, 0.0f, k * radius);
	bezierSegs[3][3] = p0;
}

float Camera::getTForArcLength(float s)
{
	if (bezierLUT_len.empty() || bezierTotalLength <= 0.0f) return 0.0f;
	// clamp s
	if (s <= 0.0f) return 0.0f;
	if (s >= bezierTotalLength) return 1.0f;

	// binary search in LUT
	int lo = 0;
	int hi = (int)bezierLUT_len.size() - 1;
	while (lo <= hi)
	{
		int mid = (lo + hi) / 2;
		if (bezierLUT_len[mid] < s) lo = mid + 1;
		else hi = mid - 1;
	}
	// lo is first index with length >= s
	int idx = std::max(1, lo);
	float len0 = bezierLUT_len[idx - 1];
	float len1 = bezierLUT_len[idx];
	float t0 = bezierLUT_T[idx - 1];
	float t1 = bezierLUT_T[idx];
	float alpha = (s - len0) / (len1 - len0);
	return t0 + alpha * (t1 - t0);
}

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform)
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Forzar que la cámara siempre mire al centro del cubo (origen) para no perderlo de vista
	glm::vec3 cubeCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	view = glm::lookAt(Position, cubeCenter, Up);

	projection = glm::perspective(
		glm::radians(FOVdeg),
		(float)width / height,
		nearPlane,
		farPlane
	);

	glUniformMatrix4fv(
		glGetUniformLocation(shader.ID, uniform),
		1,
		GL_FALSE,
		glm::value_ptr(projection * view)
	);
}

void Camera::Inputs(GLFWwindow* window)
{
	// Detecta la tecla T para alternar el seguimiento de la curva Bezier
	int pState = glfwGetKey(window, GLFW_KEY_T);
	if (pState == GLFW_PRESS && !pWasPressed)
	{
		followBezier = !followBezier;
		pWasPressed = true;
		// Reiniciar t al activar
		if (followBezier) bezierT = 0.0f;
	}
	if (pState == GLFW_RELEASE)
	{
		pWasPressed = false;
	}

	// Si followBezier está activo, avanzamos sobre la curva y actualizamos la posición.
	if (followBezier)
	{
		double currentTime = glfwGetTime();
		double delta = currentTime - lastTime;
		lastTime = currentTime;
		// Avanza t y limita a 1.0
		bezierT += static_cast<float>(bezierSpeed * delta);
		// En lugar de detenernos, envolvemos t para que la curva haga loop continuo
		if (bezierT > 1.0f) bezierT -= std::floor(bezierT);

		Position = evaluateBezier(bezierT);
		// Orientamos hacia un punto ligeramente adelantado para mirar en la dirección de la curva
		glm::vec3 ahead = evaluateBezier(std::min(1.0f, bezierT + 0.01f));
		Orientation = glm::normalize(ahead - Position);
		// Mientras seguimos la curva, ignoramos el control manual de ratón/teclado
		return;
	}

	// ================= MOVIMIENTO =================

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += speed * Orientation;
	}



	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position += speed * -glm::normalize(glm::cross(Orientation, Up));
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position += speed * -Orientation;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += speed * glm::normalize(glm::cross(Orientation, Up));
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Position += speed * Up;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Position += speed * -Up;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 0.4f;
	}
	else
	{
		speed = 0.1f;
	}

	// ================= MOUSE =================

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (firstClick)
		{
			glfwSetCursorPos(window, width / 2, height / 2);
			firstClick = false;
		}

		double mouseX;
		double mouseY;

		glfwGetCursorPos(window, &mouseX, &mouseY);

		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		glm::vec3 newOrientation = glm::rotate(
			Orientation,
			glm::radians(-rotX),
			glm::normalize(glm::cross(Orientation, Up))
		);

		if (
			!(glm::angle(newOrientation, Up) <= glm::radians(5.0f)) &&
			!(glm::angle(newOrientation, -Up) <= glm::radians(5.0f))
			)
		{
			Orientation = newOrientation;
		}

		Orientation = glm::rotate(
			Orientation,
			glm::radians(-rotY),
			Up
		);

		glfwSetCursorPos(window, width / 2, height / 2);
	}

	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstClick = true;
	}
}