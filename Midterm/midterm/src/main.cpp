/*
ZJ Wood, Dunn, Eckhardt CPE 471 Lab base code
*/

#include <iostream>
#include <glad/glad.h>
#include <cstdlib>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;
Shape shape;
Shape heart;

float snowFall = 1;

float incSnow(float* snowFall) {
	if (*snowFall < -1) {
		*snowFall = 1.2;
	}
	return -0.01;
}

double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime = glfwGetTime();
	double difference = actualtime - lasttime;
	lasttime = actualtime;
	return difference;
}
class camera
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d;
	camera()
	{
		w = a = s = d = 0;
		pos = rot = glm::vec3(0, 0, 0);
	}
	glm::mat4 process(double ftime)
	{
		float speed = 0;
		if (w == 1)
		{
			speed = 1 * ftime;
		}
		else if (s == 1)
		{
			speed = -1 * ftime;
		}
		float yangle = 0;
		if (a == 1)
			yangle = -1 * ftime;
		else if (d == 1)
			yangle = 1 * ftime;
		rot.y += yangle;
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::vec4 dir = glm::vec4(0, 0, speed, 1);
		dir = dir * R;
		pos += glm::vec3(dir.x, dir.y, dir.z);
		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return R * T;
	}
};

camera mycam;
float leftleg, rightleg, leftarm, rightarm = 0;

float key_t, key_g, key_y, key_h = 0;
float waveL, waveR, wavehandL, wavehandR = 0;
bool waspressedL = false;
bool waspressedR = false;
bool swingingUpL = true;
bool swingingUpR = true;
bool swingingUpHandL = true;
bool swingingUpHandR = true;
bool swingingUpParentLA = true; 
bool swingingUpParentRA = true;

float key_r, key_f, key_u, key_j, key_b = 0;
float kickLL, kickRL, kickfootL, kickfootR = 0;
bool waspressedLL = false;
bool waspressedRL = false;
bool swingingUpLL = true;
bool swingingUpRL = true;
bool swingingUpFootL = true;
bool swingingUpFootR = true;
bool swingingUpParentLL = true;
bool swingingUpParentRL = true;

float x[100], y[100], z[100];
int iteration;

class Application : public EventCallbacks
{

public:

	WindowManager* windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;
	std::shared_ptr<Program> robotProg;
	std::shared_ptr<Program> heartProg;
	std::shared_ptr<Program> snowProg;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexColorIDBox, IndexBufferIDBox;

	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			mycam.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			mycam.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			mycam.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			mycam.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			mycam.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			mycam.d = 0;
		}
		/*******************************ADDED MOVEMENT KEYS***************************************/
		if (key == GLFW_KEY_T && action == GLFW_PRESS)
		{
			key_t = 1;
			waspressedL = true;
		}
		if (key == GLFW_KEY_T && action == GLFW_RELEASE)
		{
			key_t = 0;
			waspressedL = false;
		}
		if (key == GLFW_KEY_G && action == GLFW_PRESS)
		{
			key_g = 1;
			waspressedL = true;
		}
		if (key == GLFW_KEY_G && action == GLFW_RELEASE)
		{
			key_g = 0;
			waspressedL = false;
		}
		if (key == GLFW_KEY_Y && action == GLFW_PRESS)
		{
			key_y = 1;
			waspressedR = true;
		}
		if (key == GLFW_KEY_Y && action == GLFW_RELEASE)
		{
			key_y = 0;
			waspressedR = false;
		}
		if (key == GLFW_KEY_H && action == GLFW_PRESS)
		{
			key_h = 1;
			waspressedR = true;
		}
		if (key == GLFW_KEY_H && action == GLFW_RELEASE)
		{
			key_h = 0;
			waspressedR = false;
		}
		if (key == GLFW_KEY_R && action == GLFW_PRESS)
		{
			key_r = 1;
			waspressedLL = true;
		}
		if (key == GLFW_KEY_R && action == GLFW_RELEASE)
		{
			key_r = 0;
			waspressedLL = false;
		}
		if (key == GLFW_KEY_F && action == GLFW_PRESS)
		{
			key_f = 1;
			waspressedLL = true;
		}
		if (key == GLFW_KEY_F && action == GLFW_RELEASE)
		{
			key_f = 0;
			waspressedLL = false;
		}
		if (key == GLFW_KEY_U && action == GLFW_PRESS)
		{
			key_u = 1;
			waspressedRL = true;
		}
		if (key == GLFW_KEY_U && action == GLFW_RELEASE)
		{
			key_u = 0;
			waspressedRL = false;
		}
		if (key == GLFW_KEY_J && action == GLFW_PRESS)
		{
			key_j = 1;
			waspressedRL = true;
		}
		if (key == GLFW_KEY_J && action == GLFW_RELEASE)
		{
			key_j = 0;
			waspressedRL = false;
		}
		if (key == GLFW_KEY_B && action == GLFW_PRESS)
		{
			if (key_b == 1)
				key_b = 0;
			else
				key_b = 1;
			if (waspressedL)
				waspressedL = false;
			else
				waspressedL = true;
			if (waspressedR)
				waspressedR = false;
			else
				waspressedR = true;
			if (waspressedLL)
				waspressedLL = false;
			else
				waspressedLL = true;
			if (waspressedRL)
				waspressedRL = false;
			else
				waspressedRL = true;
		}

	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow* window, int button, int action, int mods)
	{
		double posX, posY;
		float newPt[2];
		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			std::cout << "Pos X " << posX << " Pos Y " << posY << std::endl;

			//change this to be the points converted to WORLD
			//THIS IS BROKEN< YOU GET TO FIX IT - yay!
			newPt[0] = 0;
			newPt[1] = 0;

			std::cout << "converted:" << newPt[0] << " " << newPt[1] << std::endl;
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
			//update the vertex array with the updated points
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 6, sizeof(float) * 2, newPt);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow* window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
		string resourceDirectory = "../resources";
		// Initialize mesh.

		// shape obj holds SPHERE
		shape.loadMesh(resourceDirectory + "/sphere.obj");
		shape.resize();
		shape.init();

		heart.loadMesh(resourceDirectory + "/heart.obj");
		heart.resize();
		heart.init();

		initSnow();

		/**********************own VAO for squares*********************************/
		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

		GLfloat cube_vertices[] = {
			// front
			-1.0, -1.0,  1.0,
			1.0, -1.0,  1.0,
			1.0,  1.0,  1.0,
			-1.0,  1.0,  1.0,
			// back
			-1.0, -1.0, -1.0,
			1.0, -1.0, -1.0,
			1.0,  1.0, -1.0,
			-1.0,  1.0, -1.0,
			//tube 8 - 11
			-1.0, -1.0,  1.0,
			1.0, -1.0,  1.0,
			1.0,  1.0,  1.0,
			-1.0,  1.0,  1.0,
			//12 - 15
			-1.0, -1.0, -1.0,
			1.0, -1.0, -1.0,
			1.0,  1.0, -1.0,
			-1.0,  1.0, -1.0
		};
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_DYNAMIC_DRAW);

		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &VertexColorIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexColorIDBox);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &IndexBufferIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		GLushort cube_elements[] = {

			// front
			0, 1, 2,
			2, 3, 0,
			// back
			7, 6, 5,
			5, 4, 7,
			//tube 8-11, 12-15
			8,12,13,
			8,13,9,
			9,13,14,
			9,14,10,
			10,14,15,
			10,15,11,
			11,15,12,
			11,12,8

		};
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);

		glBindVertexArray(0);
	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		// transparency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		// Initialize the GLSL program.
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addAttribute("vertPos");

		robotProg = std::make_shared<Program>();
		robotProg->setVerbose(true);
		robotProg->setShaderNames(resourceDirectory + "/robot_vertex.glsl", resourceDirectory + "/robot_fragment.glsl");
		robotProg->init();
		robotProg->addUniform("P");
		robotProg->addUniform("V");
		robotProg->addUniform("M");
		robotProg->addAttribute("vertPos");
		robotProg->addAttribute("vertNor");
		robotProg->addAttribute("vertTex");

		heartProg = std::make_shared<Program>();
		heartProg->setVerbose(true);
		heartProg->setShaderNames(resourceDirectory + "/heart_vertex.glsl", resourceDirectory + "/heart_fragment.glsl");
		heartProg->init();
		heartProg->addUniform("P");
		heartProg->addUniform("V");
		heartProg->addUniform("M");
		heartProg->addAttribute("vertPos");
		heartProg->addAttribute("vertNor");
		heartProg->addAttribute("vertTex");

		// snow shaders
		snowProg = std::make_shared<Program>();
		snowProg->setVerbose(true);
		snowProg->setShaderNames(resourceDirectory + "/snow_vertex.glsl", resourceDirectory + "/snow_fragment.glsl");
		snowProg->init();
		snowProg->addUniform("P");
		snowProg->addUniform("V");
		snowProg->addUniform("M");
		snowProg->addAttribute("vertPos");
		snowProg->addAttribute("vertNor");
		snowProg->addAttribute("vertTex");

	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
		double frametime = get_last_elapsed_time();

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width / (float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now

		glm::mat4 V, M, P, ML, MH, Temp; //View, Model and Perspective matrix
		V = glm::mat4(1);
		M = glm::mat4(1);
		ML = glm::mat4(1);
		MH = glm::mat4(1);
		Temp = glm::mat4(1);

		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width / (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones

		//animation with the model matrix:
		static float w = 0.0;
		w += 1.0 * frametime;//rotation angle
		float trans = 0;// sin(t) * 2;
		glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 1.0f, 0.0f));
		float angle = -3.1415926 / 2.0;

		glm::mat4 RotateX = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 TransX = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		glm::mat4 TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3 + trans));
		glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));


		// Draw the box using GLSL.
		V = mycam.process(frametime);		
		
		/**********************ADDING OWN CREATURE***************************************/

		robotProg->bind();
		glUniformMatrix4fv(robotProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(robotProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(robotProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(VertexArrayID);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);

		//head
		mat4 Thead = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.3, -3));
		mat4 Shead = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
		mat4 RYhead = glm::rotate(glm::mat4(1.0f), (float)5, glm::vec3(0.0f, 1.0f, 0.0f));
		M = Thead * Shead * RYhead;
		glUniformMatrix4fv(robotProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		
		//body
		mat4 Tbody = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3));
		mat4 Sbody = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
		M = Tbody * Sbody;
		glUniformMatrix4fv(robotProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//lowerbody
		mat4 Tlowbody = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.25, -3));
		mat4 Slowbody = glm::scale(glm::mat4(1.0f), glm::vec3(0.15f, 0.1f, 0.15f));
		M = Tlowbody * Slowbody;
		glUniformMatrix4fv(robotProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//left upper leg
		leftleg += waveParentRF(leftleg, &swingingUpParentLL);
		mat4 TOleg = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.1, 0));
		mat4 RXleg = glm::rotate(glm::mat4(1.0f), leftleg, glm::vec3(1.0f, 0.0f, 0.0f));
		mat4 Tleg = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1, -0.35, -3));
		mat4 Sleg = glm::scale(glm::mat4(1.0f), glm::vec3(0.04f, 0.1f, 0.04f));
		M = Tleg * RXleg * TOleg;
		Temp = M * Sleg;
		glUniformMatrix4fv(robotProg->getUniform("M"), 1, GL_FALSE, &Temp[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		//left lower leg
		kickLL += waving(kickLL, waspressedLL, &swingingUpLL);
		TOleg = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.1, 0));
		RXleg = glm::rotate(glm::mat4(1.0f), -kickLL, glm::vec3(1.0f, 0.0f, 0.0f));
		Tleg = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.08, 0));
		Sleg = glm::scale(glm::mat4(1.0f), glm::vec3(0.03f, 0.1f, 0.03f));
		ML = M * Tleg * RXleg * TOleg;
		Temp = ML * Sleg;
		glUniformMatrix4fv(robotProg->getUniform("M"), 1, GL_FALSE, &Temp[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		//left foot
		kickfootL += waving(kickfootL, waspressedLL, &swingingUpFootL) * 2;
		TOleg = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0.06));
		RXleg = glm::rotate(glm::mat4(1.0f), -kickfootL, glm::vec3(1.0f, 0.0f, 0.0f));
		Tleg = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.09, 0));
		Sleg = glm::scale(glm::mat4(1.0f), glm::vec3(0.04f, 0.02f, 0.06f));
		MH = ML * Tleg * RXleg * TOleg * Sleg;
		glUniformMatrix4fv(robotProg->getUniform("M"), 1, GL_FALSE, &MH[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//right upperleg
		rightleg += waveParentUJ(rightleg, &swingingUpParentRL);
		TOleg = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.1, 0));
		RXleg = glm::rotate(glm::mat4(1.0f), rightleg, glm::vec3(1.0f, 0.0f, 0.0f));
		Tleg = glm::translate(glm::mat4(1.0f), glm::vec3(0.1, -0.35, -3));
		Sleg = glm::scale(glm::mat4(1.0f), glm::vec3(0.04f, 0.1f, 0.04f));
		M = Tleg * RXleg * TOleg;
		Temp = M * Sleg;
		glUniformMatrix4fv(robotProg->getUniform("M"), 1, GL_FALSE, &Temp[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		//right lower leg
		kickRL += waving(kickRL, waspressedRL, &swingingUpRL);
		TOleg = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.1, 0));
		RXleg = glm::rotate(glm::mat4(1.0f), -kickRL, glm::vec3(1.0f, 0.0f, 0.0f));
		Tleg = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.08, 0));
		Sleg = glm::scale(glm::mat4(1.0f), glm::vec3(0.03f, 0.1f, 0.03f));
		ML = M * Tleg * RXleg * TOleg;
		Temp = ML * Sleg;
		glUniformMatrix4fv(robotProg->getUniform("M"), 1, GL_FALSE, &Temp[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		//right foot
		kickfootR += waving(kickfootR, waspressedRL, &swingingUpFootR) * 2;
		TOleg = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0.06));
		RXleg = glm::rotate(glm::mat4(1.0f), -kickfootR, glm::vec3(1.0f, 0.0f, 0.0f));
		Tleg = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.09, 0));
		Sleg = glm::scale(glm::mat4(1.0f), glm::vec3(0.04f, 0.02f, 0.06f));
		MH = ML * Tleg * RXleg * TOleg * Sleg;
		glUniformMatrix4fv(robotProg->getUniform("M"), 1, GL_FALSE, &MH[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//leftarm
		leftarm += waveParentTG(leftarm, &swingingUpParentLA);
		mat4 TOarm = glm::translate(glm::mat4(1.0f), glm::vec3(-0.15, 0, 0));
		mat4 RZarm = glm::rotate(glm::mat4(1.0f), leftarm, glm::vec3(0.0f, 0.0f, 1.0f));
		mat4 Tarm = glm::translate(glm::mat4(1.0f), glm::vec3(-0.17, 0.09, -3));
		mat4 Sarm = glm::scale(glm::mat4(1.0f), glm::vec3(0.15f, 0.05f, 0.05f));
		M = Tarm * RZarm * TOarm;
		Temp = M * Sarm; 
		glUniformMatrix4fv(robotProg->getUniform("M"), 1, GL_FALSE, &Temp[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		//left lowerarm
		waveL += waving(waveL, waspressedL, &swingingUpL);
		TOarm = glm::translate(glm::mat4(1.0f), glm::vec3(-0.185, 0, 0));
		RZarm = glm::rotate(glm::mat4(1.0f), waveL, glm::vec3(0.0f, 0.0f, 1.0f));
		Tarm = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1, 0, 0));
		Sarm = glm::scale(glm::mat4(1.0f), glm::vec3(0.15f, 0.04f, 0.04f));
		ML = M * Tarm * RZarm * TOarm;
		Temp = ML * Sarm;
		glUniformMatrix4fv(robotProg->getUniform("M"), 1, GL_FALSE, &Temp[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		//left hand
		wavehandL += wavinghand(wavehandL, waspressedL, &swingingUpHandL) * 2;
		TOarm = glm::translate(glm::mat4(1.0f), glm::vec3(-0.06, 0, 0));
		RZarm = glm::rotate(glm::mat4(1.0f), wavehandL, glm::vec3(0.0f, 0.0f, 1.0f));
		Tarm = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1, 0, 0));
		Sarm = glm::scale(glm::mat4(1.0f), glm::vec3(0.06f, 0.03f, 0.02f));
		MH = ML * Tarm * RZarm * TOarm * Sarm;
		glUniformMatrix4fv(robotProg->getUniform("M"), 1, GL_FALSE, &MH[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);


		//rightarm
		rightarm += waveParentYH(rightarm, &swingingUpParentRA);
		TOarm = glm::translate(glm::mat4(1.0f), glm::vec3(0.15, 0, 0));
		RZarm = glm::rotate(glm::mat4(1.0f), rightarm, glm::vec3(0.0f, 0.0f, 1.0f));
		Tarm = glm::translate(glm::mat4(1.0f), glm::vec3(0.17, 0.09, -3));
		Sarm = glm::scale(glm::mat4(1.0f), glm::vec3(0.15f, 0.05f, 0.05f));
		M = Tarm * RZarm * TOarm;
		Temp = M * Sarm;
		glUniformMatrix4fv(robotProg->getUniform("M"), 1, GL_FALSE, &Temp[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		//right lowerarm
		waveR += waving(waveR, waspressedR, &swingingUpR);
		TOarm = glm::translate(glm::mat4(1.0f), glm::vec3(0.185, 0, 0));
		RZarm = glm::rotate(glm::mat4(1.0f), -waveR, glm::vec3(0.0f, 0.0f, 1.0f));
		Tarm = glm::translate(glm::mat4(1.0f), glm::vec3(0.1, 0, 0));
		Sarm = glm::scale(glm::mat4(1.0f), glm::vec3(0.15f, 0.04f, 0.04f));
		ML = M * Tarm * RZarm * TOarm;
		Temp = ML * Sarm;
		glUniformMatrix4fv(robotProg->getUniform("M"), 1, GL_FALSE, &Temp[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		//right hand
		wavehandR += wavinghand(wavehandR, waspressedR, &swingingUpHandR) * 2;
		TOarm = glm::translate(glm::mat4(1.0f), glm::vec3(0.06, 0, 0));
		RZarm = glm::rotate(glm::mat4(1.0f), -wavehandR, glm::vec3(0.0f, 0.0f, 1.0f));
		Tarm = glm::translate(glm::mat4(1.0f), glm::vec3(0.1, 0, 0));
		Sarm = glm::scale(glm::mat4(1.0f), glm::vec3(0.06f, 0.03f, 0.02f));
		MH = ML * Tarm * RZarm * TOarm * Sarm;
		glUniformMatrix4fv(robotProg->getUniform("M"), 1, GL_FALSE, &MH[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		glBindVertexArray(VertexArrayID);

		// eyes
		mat4 RYeye = glm::rotate(glm::mat4(1.0f), (float)2, glm::vec3(0.0f, 1.0f, 0.0f));
		mat4 Seye = glm::scale(glm::mat4(1.0f), glm::vec3(0.02, 0.02, 0.02));
		mat4 Teye = glm::translate(glm::mat4(1.0f), glm::vec3(0.08, 0.3, -2.92));
		M = Teye * Seye*RYeye;
		glUniformMatrix4fv(robotProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(robotProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(robotProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(robotProg);
		RYeye = glm::rotate(glm::mat4(1.0f), (float)2, glm::vec3(0.0f, 1.0f, 0.0f));
		Seye = glm::scale(glm::mat4(1.0f), glm::vec3(0.02, 0.02, 0.02));
		Teye = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.3, -2.89));
		M = Teye * Seye * RYeye;
		glUniformMatrix4fv(robotProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(robotProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(robotProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(robotProg);
		robotProg->unbind();

		heartProg->bind();
		mat4 RXheart = glm::rotate(glm::mat4(1.0f), -(float)3.1415926/2, glm::vec3(1.0f, 0.0f, 0.0f));
		mat4 Sheart = glm::scale(glm::mat4(1.0f), glm::vec3(0.05, 0.05, 0.05));
		mat4 Theart = glm::translate(glm::mat4(1.0f), glm::vec3(0.1, 0.05, -2.8));
		M = Theart * Sheart * RXheart;
		glUniformMatrix4fv(heartProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(heartProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(heartProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		heart.draw(heartProg);
		heartProg->unbind();


		snowProg->bind();
		for (int i = 0; i < 100; i++) {
			mat4 Ssnow = glm::scale(glm::mat4(1.0f), glm::vec3(.007, .007, .007));
			mat4 Tsnow = glm::translate(glm::mat4(1.0f), glm::vec3(x[i], snowFall, z[i]-3));
			M = Tsnow * Ssnow;
			glUniformMatrix4fv(snowProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(snowProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(snowProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			shape.draw(snowProg);
		}
		snowProg->unbind();

		// draw snowglobe last
		prog->bind();
		M = TransZ * RotateY * RotateX * S;
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(prog);
		prog->unbind();

	}

	float initSnow()
	{
		for (int i = 0; i < 100; i++)
		{
			do
			{
				x[i] = -1 + (float)(rand()) / (float)((RAND_MAX / (2)));
				y[i] = ((float)rand()) / (float)RAND_MAX;
				z[i] = -1 + (float)(rand()) / (float)((RAND_MAX / (2)));
			} while (1.0 < (x[i] * x[i]) + (y[i] * y[i]) + (z[i] * z[i]));

		}
		return 0;
	}

	float waveParentTG(float wave, bool* swingingUpParent)
	{
		if (key_t == 1 && wave > -1.5)
			return -0.01;
		else if (key_t == 1 && wave <= -1.5)
			return 0;
		else if (key_g == 1 && wave < 1.2)
			return 0.01;
		else if (key_g == 1 && wave >= 1.2)
			return 0;
		else if (key_b == 1)
		{
			if (wave < -1.5)
				*swingingUpParent = true;
			else if (wave > 1.2)
				*swingingUpParent = false;
			// update based on direction
			if (*swingingUpParent == false)
				return -0.01;
			else
				return 0.01;
		}
		else
			return 0;
	}

	float waveParentYH(float wave, bool* swingingUpParent)
	{
		if (key_h == 1 && wave > -1.2)
			return -0.01;
		else if (key_h == 1 && wave <= -1.2)
			return 0;
		else if (key_y == 1 && wave < 1.5)
			return 0.01;
		else if (key_y == 1 && wave >= 1.5)
			return 0;
		else if (key_b == 1)
		{
			if (wave < -1.2)
				*swingingUpParent = true;
			else if (wave > 1.5)
				*swingingUpParent = false;
			// update based on direction
			if (*swingingUpParent == false)
				return -0.01;
			else
				return 0.01;
		}
		else
			return 0;
	}
	
	float waveParentRF(float wave, bool* swingingUpParent)
	{
		if (key_r == 1 && wave > -1.5)
			return -0.01;
		else if (key_r == 1 && wave <= -1.5)
			return 0;
		else if (key_f == 1 && wave < 1.2)
			return 0.01;
		else if (key_f == 1 && wave >= 1.2)
			return 0;
		else if (key_b == 1)
		{
			if (wave < -1.5)
				*swingingUpParent = true;
			else if (wave > 1.2)
				*swingingUpParent = false;
			// update based on direction
			if (*swingingUpParent == false)
				return -0.01;
			else
				return 0.01;
		}
		else
			return 0;
	}

	float waveParentUJ(float wave, bool* swingingUpParent)
	{
		if (key_u == 1 && wave > -1.5)
			return -0.01;
		else if (key_u == 1 && wave <= -1.5)
			return 0;
		else if (key_j == 1 && wave < 1.2)
			return 0.01;
		else if (key_j == 1 && wave >= 1.2)
			return 0;
		else if (key_b == 1)
		{
			if (wave < -1.5)
				*swingingUpParent = true;
			else if (wave > 1.2)
				*swingingUpParent = false;
			// update based on direction
			if (*swingingUpParent == false)
				return -0.01;
			else
				return 0.01;
		}
		else
			return 0;
	}

	float waving(float wave, bool waspressed, bool* swingingUp) {
		if (!waspressed)
			return 0;
		if (wave < -2.5)
			*swingingUp = true;
		else if (wave > 0)
			*swingingUp = false;
		// update based on direction
		if (*swingingUp == false)
			return -0.02;
		else
			return 0.02;
	}
	float wavinghand(float wave, bool waspressed, bool* swingingUpHand) {
		if (!waspressed)
			return 0;
		if (wave < -1)
			*swingingUpHand = true;
		else if (wave > 1)
			*swingingUpHand = false;
		// update based on direction
		if (*swingingUpHand == false)
			return -0.02;
		else
			return 0.02;
	}

};
//******************************************************************************************
int main(int argc, char** argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application* application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager* windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
		// Initialize scene.
	srand(static_cast <unsigned> (time(0)));
	application->init(resourceDir);
	application->initGeom();

	// Loop until the user closes the window.
	while (!glfwWindowShouldClose(windowManager->getHandle()))
	{
		snowFall += incSnow(&snowFall);
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
