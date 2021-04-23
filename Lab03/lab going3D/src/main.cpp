/*
ZJ Wood CPE 471 Lab 3 base code
*/

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "WindowManager.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

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

class Application : public EventCallbacks
{

public:

	int kw = 0, ks = 0;

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
			kw = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			kw = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			ks = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			ks = 0;
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


		glGenBuffers(1, &VertexBufferID);
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
		};
		//make it a bit smaller
		for (int i = 0; i < 24; i++)
			cube_vertices[i] *= 0.5;
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_DYNAMIC_DRAW);

		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

		//color
		GLfloat cube_colors[] = {
			// front colors
			1.0, 0.0, 0.0,
			0.0, 1.0, 0.0,
			0.0, 0.0, 1.0,
			1.0, 1.0, 1.0,
			// back colors
			1.0, 0.0, 0.0,
			0.0, 1.0, 0.0,
			0.0, 0.0, 1.0,
			1.0, 1.0, 1.0,
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
			// top
			1, 5, 6,
			6, 2, 1,
			// back
			7, 6, 5,
			5, 4, 7,
			// bottom
			4, 0, 3,
			3, 7, 4,
			// left
			4, 5, 1,
			1, 0, 4,
			// right
			3, 2, 6,
			6, 7, 3,
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
		float aspect = width / (float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrices
		//PER MODEL MATRIX - every object has one model matrix, one view matrix
		glm::mat4 M, P; //View, Model and Perspective matrix - multiplied with every vertices seen
		//V = glm::mat4(1); // view is always in terms of 'other way around'
		M = glm::mat4(1);
		/*mat4 R, T, S;
		static float w = 0.0;
		w += 0.1;

		S = scale(mat4(1), vec3(.1, 2, 1)); // standard - scales TOWARDS origin
		T = translate(mat4(1), vec3(2, 0, 0));
		R = rotate(mat4(1), w, vec3(0, 0, 1)); // always rotates around origin - should rotate FIRST
		*/
		// Apply orthographic projection....
		P = glm::ortho(-1 * aspect, 1 * aspect, -1.0f, 1.0f, -2.0f, 100.0f);
		if (width < height)
			P = glm::ortho(-1.0f, 1.0f, -1.0f / aspect, 1.0f / aspect, -2.0f, 100.0f);
		// ...but we overwrite it (optional) with a perspective projection.
		//(opening angle of perspective matrix, ratio of window, new plane, far plane)
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width / (float)height), 0.1f, 500.0f); //so much type casting... GLM metods are quite funny ones

		// Draw the box using GLSL.
		prog.bind();
		//bind the cube's VAO:
		glBindVertexArray(VertexArrayID);
		//send the matrices to the shaders
		glUniformMatrix4fv(prog.getUniform("P"), 1, GL_FALSE, &P[0][0]);

		/*************MOVING WINDOW********************/
		static float cam_z = -10;
		if (kw == 1)
		{
			cam_z += 0.1;
		}
		else if (ks == 1)
		{
			cam_z -= 0.1;
		}
		/***********************************************/
		
		/*****************ROTATE SCENE*******************/
		Matrix V;
		Matrix V_Trans;
		Matrix V_Rotate;
		V.createIdentityMat();
		V_Trans.createIdentityMat();
		V_Rotate.createIdentityMat();

		V_Rotate.createRotationMatrixY(0.2);
		V_Trans.createTranslateMat(0, 0, -10);
		V.multMatrix(V_Trans, V_Rotate);
		glUniformMatrix4fv(prog.getUniform("V"), 1, GL_FALSE, V.matrix);
		//V = glm::translate(mat4(1), vec3(0, 0, cam_z));
		//glUniformMatrix4fv(prog.getUniform("V"), 1, GL_FALSE, &V[0][0]);

		//set model matrix and draw the cube		
		/*
		S = scale(mat4(1), vec3(.3, 2, .3));
		mat4 sR = rotate(mat4(1), w, vec3(0, 1, 0));
		mat4 isR = rotate(mat4(1), -w, vec3(0, 1, 0));

		for (int m = 1; m <= 3; m++)
		{
			for (int i = 0; i < 100; i++)
			{
				float lw = w + (float)i / 0.02;
				float ro = w * .001 + (float)i / 0.1;
				mat4 sR = rotate(mat4(1), lw, vec3(0, 1, 0));
				mat4 isR = rotate(mat4(1), -lw, vec3(0, 1, 0));

				T = translate(mat4(1), vec3(2, 2, -i * 2) * (float)m);
				float qp = 3.1415926 * .25 + ro;
				R = rotate(mat4(1), qp, vec3(0, 0, 1));
				M = T * R * sR * S; // multiplying matrices DOES BOTH - goes from RIGHT to LEFT --> S --> T3
				glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &M[0][0]);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

				T = translate(mat4(1), vec3(-2, 2, -i * 2) * (float)m);
				qp = -3.1415926 * .25 + ro;
				R = rotate(mat4(1), qp, vec3(0, 0, 1));
				M = T * R * isR * S; // multiplying matrices DOES BOTH - goes from RIGHT to LEFT --> S --> T3
				glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &M[0][0]);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

				T = translate(mat4(1), vec3(2, -2, -i * 2) * (float)m);
				qp = -3.1415926 * .25 + ro;
				R = rotate(mat4(1), qp, vec3(0, 0, 1));
				M = T * R * sR * S; // multiplying matrices DOES BOTH - goes from RIGHT to LEFT --> S --> T3
				glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &M[0][0]);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

				T = translate(mat4(1), vec3(-2, -2, -i * 2) * (float)m);
				qp = 3.1415926 * .25 + ro;
				R = rotate(mat4(1), qp, vec3(0, 0, 1));
				M = T * R * isR * S; // multiplying matrices DOES BOTH - goes from RIGHT to LEFT --> S --> T3
				glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, &M[0][0]);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
			}
		}
		*/
		/*******************LETTERS*****************************/
		Matrix H_left;
		Matrix H_leftscale;
		Matrix H_lefttranslate;
		H_left.createIdentityMat();
		H_leftscale.createScaleMat(0.5, 3, 1);
		H_lefttranslate.createTranslateMat(-3, 0, 0);
		H_left.multMatrix(H_leftscale, H_lefttranslate);
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, H_left.matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		Matrix H_right;
		H_right.createScaleMat(0.5, 3, 1);
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, H_right.matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		Matrix H_mid;
		Matrix H_midrotate;
		Matrix H_midscale;
		Matrix H_midtranslate;
		H_mid.createIdentityMat();
		H_midrotate.createRotationMatrixZ(0.5);
		H_midscale.createScaleMat(2.5, 0.5, 0.5);
		H_midtranslate.createTranslateMat(-0.75, 0, 0);
		H_mid.multMatrix(H_midrotate, H_midscale);
		H_mid.multMatrix(H_midtranslate, H_mid);
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, H_mid.matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		Matrix I_right;
		Matrix I_rightscale;
		Matrix I_righttranslate;
		I_right.createIdentityMat();
		I_rightscale.createScaleMat(0.5, 3, 1);
		I_righttranslate.createTranslateMat(3, 0, 0);
		I_right.multMatrix(I_rightscale, I_righttranslate);
		glUniformMatrix4fv(prog.getUniform("M"), 1, GL_FALSE, I_right.matrix);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		glBindVertexArray(0);

		prog.unbind();
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
