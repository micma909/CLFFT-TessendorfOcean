#include "GLWindow.h"
#include <GLFW/glfw3.h>
#include "glsl.h"

#include <sstream>
#include <iomanip>

static double x_pos_;
static double y_pos_;

static double old_x_pos_;
static double old_y_pos_;

static bool leftIsPressed;
static bool middleIsPressed;

static double x_rot_;
static double y_rot_;

static double old_x_rot_;
static double old_y_rot_;

static double zoom_;

//panning
static double x_pan_;
static double y_pan_;

static double x_offset_;
static double y_offset_;

static double last_x_offset_;
static double last_y_offset_;

GLWindow::GLWindow():window(nullptr),mainLoop(nullptr){
    
}

GLWindow::~GLWindow(){
    glDeleteBuffers(1,&vbo);
    glDeleteBuffers(1,&nbo);
    glDeleteBuffers(1,&ibo);
    glDeleteBuffers(1,&vbo2);

}

GLuint createVBO(const void* data, int dataSize, GLenum target, GLenum usage)
{
	GLuint id = 0;  // 0 is reserved, glGenBuffersARB() will return non-zero id if success

	glGenBuffers(1, &id);                        // create a vbo
	glBindBuffer(target, id);                    // activate vbo id to use
	glBufferData(target, dataSize, data, usage); // upload data to video card

	// check data size in VBO is same as input array, if not return 0 and delete VBO
	int bufferSize = 0;
	glGetBufferParameteriv(target, GL_BUFFER_SIZE, &bufferSize);
	if (dataSize != bufferSize)
	{
		glDeleteBuffers(1, &id);
		id = 0;
		//cout << "[createVBO()] Data size is mismatch with input array\n";
		printf("[createVBO()] Data size is mismatch with input array\n");
	}
	//this was important for working inside blender!
	glBindBuffer(target, 0);
	return id;      // return VBO id
}

int GLWindow::init(int window_width, int window_height,
                  GLfloat *verts,GLsizei v_size,
                  GLuint *ind,GLsizei i_size){
    
    
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    //Error callback
    glfwSetErrorCallback([](int code,const char *error){
        std::cout << "GLFW error code: " << code << std::endl << "\"" << error << "\"" << std::endl;
    });
    
    glfwWindowHint(GLFW_DECORATED,1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE );



    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(window_width, window_height, "Ocean", NULL, NULL);
    if (!window){
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    /* Sweeet! */
    printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

    
    // Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
    glEnable (GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
    glDepthMask(GL_TRUE);
    //glEnable(GL_CULL_FACE);
    glfwSwapInterval(10);

    
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);
    
    glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, v_size*sizeof(GLfloat), verts, GL_STATIC_DRAW);
    
    //normals
    glGenBuffers(1, &nbo);
    glBindBuffer(GL_ARRAY_BUFFER, nbo);
    glBufferData(GL_ARRAY_BUFFER, v_size * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
    
    glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, i_size*sizeof(GLuint), ind, GL_STATIC_DRAW );
	
    glGenBuffers(1, &vbo2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBufferData(GL_ARRAY_BUFFER, 2*v_size*sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    
    //Mouse
    glfwSetMouseButtonCallback(window, mouse_pressed);
    glfwSetCursorPosCallback(window, mouse_pos);
    glfwSetScrollCallback(window, mouse_scroll);

    std::cout << "GL_SHADING_LANGUAGE_VERSION : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    //setUpcamera();
    return 1;
}

void GLWindow::mouse_pos(GLFWwindow *window,double x,double y){
    x_pos_ = x;
    y_pos_ = y;
    if (leftIsPressed) {
        double delta_x = (x_pos_-old_x_pos_)/(double)100.f;
        double delta_y = (y_pos_-old_y_pos_)/(double)100.f;
        
        x_rot_ = delta_x+old_x_rot_;
        y_rot_ = delta_y+old_y_rot_;

		old_x_rot_ = x_rot_;
		old_y_rot_ = y_rot_;
    }

	//Panning. 
	//Works but acts weird when user pans to far...
	if (middleIsPressed){
		x_offset_ = old_x_pos_ + x;
		if (!(last_x_offset_ == 0.0)){
			x_pan_ += (last_x_offset_ - x_offset_)*0.001f; //scaled it a bit, otherwise goes nuts.
		}
		last_x_offset_ = x_pos_;

		y_offset_ = old_y_pos_ + y;
		if (!(last_y_offset_ == 0.0)){
			y_pan_ -= (last_y_offset_ - y_offset_)*0.001f; 
		}
		last_y_offset_ = y_pos_;
	}

}

void GLWindow::mouse_pressed(GLFWwindow *window,int button,int action,int){
    if (action == GLFW_PRESS){
		
		if (button == GLFW_MOUSE_BUTTON_MIDDLE && (button != GLFW_MOUSE_BUTTON_LEFT && button != GLFW_MOUSE_BUTTON_RIGHT)){
			middleIsPressed = true; 
			last_x_offset_ = 0.0f;
			last_y_offset_ = 0.0f;
		}
		else if (button == GLFW_MOUSE_BUTTON_LEFT){
			leftIsPressed = true;
			old_x_pos_ = x_pos_;
            old_y_pos_ = y_pos_;
		}

    }else if(action == GLFW_REPEAT) {

    }else if (action == GLFW_RELEASE){
       // std::cout << "released!" << std::endl;
		if (button == GLFW_MOUSE_BUTTON_LEFT){
			old_x_pos_ = x_rot_;
			old_y_pos_ = y_rot_;
			leftIsPressed = false;
		}
		else if(button == GLFW_MOUSE_BUTTON_MIDDLE){
			middleIsPressed = false;
		}
	
    }
}

void GLWindow::mouse_scroll(GLFWwindow *window,double x,double y){
    zoom_ += y*0.5;
}


void GLWindow::run(){
    int frames = 0;
    double last = glfwGetTime();
    std::ostringstream title;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)){
        if (mainLoop) {
            mainLoop(*this);
        }

        frames++;
        if (glfwGetTime()-last > 1.0) { //Update every second
            title.str(""); //Clear content
            title << std::setprecision(3) << "Ocean simulation @ " << double(frames)/(glfwGetTime()-last) << " fps";
            glfwSetWindowTitle(window, title.str().c_str());
            frames = 0;
            last = glfwGetTime();
        }
    }
}

float GLWindow::x_rot() const{
    return x_rot_;
}

float GLWindow::y_rot() const{
    return y_rot_;
}

float GLWindow::x_pan() const{
    return x_pan_;
}

float GLWindow::y_pan() const{
    return y_pan_;
}

float GLWindow::zoom() const{
    return zoom_;
}



