/*
ZJ Wood CPE 471 Lab 3 base code
*/

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime =glfwGetTime();
	double difference = actualtime- lasttime;
	lasttime = actualtime;
	return difference;
}

/**********************UPDATED WITH OWN MATRIX CLASS*******************/
class Matrix
{
public:
	float matrix[16] = { 0 };

	// stored in one-dimensional array as:
	/*x: 0 4 8 12
	* y: 1 5 9 13
	* z: 2 6 10 14
	*    3 7 11 15
	*/
	void createIdentityMat()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				matrix[i + 4 * j] = 0;
			}
		}
		matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1;
	}

	void createTranslateMat(float x, float y, float z)
	{
		createIdentityMat();
		matrix[12] += x;
		matrix[13] += y;
		matrix[14] += z;
	}

	void createScaleMat(float x, float y, float z)
	{
		createIdentityMat();
		matrix[0] *= x;
		matrix[5] *= y;
		matrix[10] *= z;
	}

	void createRotationMatrixX(float angle_x) //around x
	{
		createIdentityMat();
		matrix[5] = cos(angle_x);
		matrix[6] = sin(angle_x);
		matrix[9] = -1 * sin(angle_x);
		matrix[10] = cos(angle_x);
	}

	void createRotationMatrixY(float angle_y) //around y
	{
		createIdentityMat();
		matrix[0] = cos(angle_y);
		matrix[2] = -1 * sin(angle_y);
		matrix[8] = sin(angle_y);
		matrix[10] = cos(angle_y);
	}

	void createRotationMatrixZ(float angle_z) //around z
	{
		createIdentityMat();
		matrix[0] = cos(angle_z);
		matrix[1] = -1 * sin(angle_z);
		matrix[4] = sin(angle_z);
		matrix[5] = cos(angle_z);
	}

	//dot product for matrices
	void multMatrix(Matrix A, Matrix B)
	{
		float product = 0;
		for (int Bcol = 0; Bcol < 4; ++Bcol) {
			// Process kth column of C
			for (int Arow = 0; Arow < 4; ++Arow) {
				product = 0;
				for (int j = 0; j < 4; ++j) {
					product += A.matrix[Arow + 4 * j] * B.matrix[j + 4 * Bcol];
				}
				matrix[Arow + 4 * Bcol] = product;
			}
		}
	}

};

class camera
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d;
	camera()
	{
		w = a = s = d = 0;
		pos = rot = glm::vec3(0, 0, -1);
	}
	glm::mat4 process()
	{
		double ftime = get_last_elapsed_time();
		float speed = 0;
		if (w == 1)
		{
			speed = 1*ftime;
		}
		else if (s == 1)
		{
			speed = -1*ftime;
		}
		float yangle=0;
		if (a == 1)
			yangle = -1*ftime;
		else if(d==1)
			yangle = 1*ftime;
		rot.y += yangle;
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::vec4 dir = glm::vec4(0, 0, speed,1);
		dir = dir*R;
		pos += glm::vec3(dir.x, dir.y, dir.z);
		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return R*T;
	}
};

camera mycam;
float key_n, key_m, key_x, key_c, wave = 0;
bool waspressed = false;
bool swingingUp = true;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	Program prog;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexColorIDBox, IndexBufferIDBox;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
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

		/********************ADDED MOVEMENT KEYS**************************/

		if (key == GLFW_KEY_N && action == GLFW_PRESS)
		{
			key_n = 1;
		}
		if (key == GLFW_KEY_N && action == GLFW_RELEASE)
		{
			key_n = 0;
		}
		if (key == GLFW_KEY_M && action == GLFW_PRESS)
		{
			key_m = 1;
		}
		if (key == GLFW_KEY_M && action == GLFW_RELEASE)
		{
			key_m = 0;
		}
		if (key == GLFW_KEY_X && action == GLFW_PRESS)
		{
			if (waspressed)
				waspressed = false;
			else
				waspressed = true;
		}
		if (key == GLFW_KEY_C && action == GLFW_PRESS)
		{
			key_c = 1;
			waspressed = false;
		}
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;
		float newPt[2];
		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			std::cout << "Pos X " << posX <<  " Pos Y " << posY << std::endl;

		}
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

		//color
		GLfloat cube_colors[] = {
			// front colors
			1.0, 0.0, 0.5,
			1.0, 0.0, 0.5,
			1.0, 0.0, 0.5,
			1.0, 0.0, 0.5,
			// back colors
			0.5, 0.5, 0.0,
			0.5, 0.5, 0.0,
			0.5, 0.5, 0.0,
			0.5, 0.5, 0.0,
			// tube colors
			0.0, 1.0, 1.0,
			0.0, 1.0, 1.0,
			0.0, 1.0, 1.0,
			0.0, 1.0, 1.0,
			0.0, 1.0, 1.0,
			0.0, 1.0, 1.0,
			0.0, 1.0, 1.0,
			0.0, 1.0, 1.0,
		};
		glGenBuffers(1, &VertexColorIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexColorIDBox);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);
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

		// Initialize the GLSL program.
		
		prog.setVerbose(true);
		prog.setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		if (!prog.init())
			{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1); //[SHADERBUG] ? breakpoint here!! if program hits that codeline, check the command line window, it will contain the line and place of the error in the shader file
			}
		prog.addUniform("P");
		prog.addUniform("V");
		prog.addUniform("M");
		prog.addAttribute("vertPos");
		prog.addAttribute("vertColor");
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		
		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = glm::mat4(1);
		M = glm::mat4(1);
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones

		//animation with the model matrix:
		static float w = 0.0;
		w += 0.01;//rotation angle
		static float t = 0;
		t += 0.01;
		float trans = 0;// sin(t) * 2;
		glm::mat4 T = glm::mat4(1.0f);
		glm::mat4 RotateX = glm::rotate(glm::mat4(1.0f), w, glm::vec3(-1.0f, 1.0f, 0.0f));
		glm::mat4 TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3 + trans));
		glm::mat4 TransX = glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.0f, 0.0));
		glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, .1f, 0.1f));
		
		// Draw the box using GLSL.
		prog.bind();

		V = mycam.process();
		//send the matrices to the shaders
		glUniformMatrix4fv(prog.getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog.getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(VertexArrayID);

		/**************************ADDING ROBOT*************************************/
		//actually draw from vertex 0, 3 vertices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		
		//torso
		TransX = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		M = TransX  * S;
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//head
		Matrix _To, _T, _R, _S, finalmat;
		finalmat.createIdentityMat();
		_S.createScaleMat(0.05, 0.05, 0.05);
		_R.createRotationMatrixY(3.14159265/4);
		_T.createTranslateMat(0, 3, 0);
		finalmat.multMatrix(_S, _R);
		finalmat.multMatrix(finalmat, _T);
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, finalmat.matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//buttons
		finalmat.createIdentityMat();
		_S.createScaleMat(0.01, 0.01, 0.01);
		_R.createRotationMatrixY(3.14159265 / 2);
		_T.createTranslateMat(0.07, 0.07, 0.1);
		finalmat.multMatrix(_T, _R);
		finalmat.multMatrix(finalmat, _S);
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, finalmat.matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		finalmat.createIdentityMat();
		_S.createScaleMat(0.01, 0.01, 0.01);
		_R.createRotationMatrixY(3.14159265 / 2);
		_T.createTranslateMat(0.07, 0.04, 0.1);
		finalmat.multMatrix(_T, _R);
		finalmat.multMatrix(finalmat, _S);
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, finalmat.matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		finalmat.createIdentityMat();
		_S.createScaleMat(0.01, 0.01, 0.01);
		_R.createRotationMatrixY(3.14159265 / 2);
		_T.createTranslateMat(0.07, 0.01, 0.1);
		finalmat.multMatrix(_T, _R);
		finalmat.multMatrix(finalmat, _S);
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, finalmat.matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//lower body
		finalmat.createIdentityMat();
		_S.createScaleMat(0.1, 0.04, 0.075);
		_T.createTranslateMat(0, -3, 0);
		finalmat.multMatrix(_S, _T);
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, finalmat.matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//legs
		finalmat.createIdentityMat();
		_S.createScaleMat(0.02, 0.09, 0.05);
		_T.createTranslateMat(-2, -2.5, 0);
		finalmat.multMatrix(_S, _T);
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, finalmat.matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		finalmat.createIdentityMat();
		_S.createScaleMat(0.02, 0.09, 0.05);
		_T.createTranslateMat(2, -2.5, 0);
		finalmat.multMatrix(_S, _T);
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, finalmat.matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		//feet
		finalmat.createIdentityMat();
		_S.createScaleMat(0.03, 0.015, 0.05);
		_T.createTranslateMat(-1.7, -20, 0);
		finalmat.multMatrix(_S, _T);
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, finalmat.matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		finalmat.createIdentityMat();
		_S.createScaleMat(0.03, 0.015, 0.05);
		_T.createTranslateMat(1.7, -20, 0);
		finalmat.multMatrix(_S, _T);
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, finalmat.matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);


		//left upperarm
		Matrix upperarm;
		static float aa = 0;
		if (key_n == 1 && aa > -1.5)
		{
			aa -= 0.01;
		}
		else if (key_m == 1 && aa < 1.5)
		{
			aa += 0.01;
		}
		upperarm.createIdentityMat();
		_To.createTranslateMat(-0.1, 0, 0);
		_R.createRotationMatrixZ(aa);
		_T.createTranslateMat(-0.09, 0.08, 0);
		upperarm.multMatrix(_R, _To);
		upperarm.multMatrix(_T, upperarm);

		//left lowerarm
		Matrix lowerarm; 
		wave += waving(wave, waspressed, &swingingUp);

		lowerarm.createIdentityMat();
		_To.createTranslateMat(-0.085, 0, 0); // define point of rotation
		_R.createRotationMatrixZ(-wave);
		_S.createScaleMat(0.1, 0.01, 0.01);
		_T.createTranslateMat(-0.1, 0, 0); // remember translate is based on CENTER of obj
		lowerarm.multMatrix(_R, _To); // translate first, then rotate
		lowerarm.multMatrix(_T, lowerarm);
		lowerarm.multMatrix(upperarm, lowerarm);

		lowerarm.multMatrix(lowerarm, _S);
		upperarm.multMatrix(upperarm, _S);
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, lowerarm.matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, upperarm.matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//right upperarm
		upperarm.createIdentityMat();
		_To.createTranslateMat(0.1, 0, 0);
		_R.createRotationMatrixZ(-aa);
		_T.createTranslateMat(0.09, 0.08, 0);
		upperarm.multMatrix(_R, _To);
		upperarm.multMatrix(_T, upperarm);

		//right lowerarm
		lowerarm.createIdentityMat();
		_To.createTranslateMat(0.085, 0, 0); // define point of rotation
		_R.createRotationMatrixZ(wave);
		_S.createScaleMat(0.1, 0.01, 0.01);
		_T.createTranslateMat(0.1, 0, 0); // remember translate is based on CENTER of obj
		lowerarm.multMatrix(_R, _To); // translate first, then rotate
		lowerarm.multMatrix(_T, lowerarm);
		lowerarm.multMatrix(upperarm, lowerarm);
		lowerarm.multMatrix(lowerarm, _S);
		upperarm.multMatrix(upperarm, _S);
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, lowerarm.matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, upperarm.matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		glBindVertexArray(0);

		prog.unbind();

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
			return -0.025;
		else
			return 0.025;
	}
};
//******************************************************************************************
int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom();

	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
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
