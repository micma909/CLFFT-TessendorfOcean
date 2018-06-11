#include <iostream>
#include <fstream>
#include <map>

#include <GL/glew.h>
#include <cl_lib.h>
#include "Geometry.h"
#include <GLFW/glfw3.h>
#if __APPLE__
#define USE_MATLAB 0
#include <OpenGL/OpenGL.h>
#include <OpenGL/glext.h>
#else
#define USE_MATLAB 0
#include <GL/GL.h>
#include "getopt.h"
#endif

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <algorithm>

#include "GLWindow.h"
#include "GLHelper.h"
#include "Spectrum.h"
#include "IFFT.h"

#include "Shader.h"

#if USE_MATLAB
#include "MatlabHelper.h"
#include <engine.h>
#include <mex.h>
mxArray *mY,*mX,*mZ;
Engine *engine;
#endif

int N; //FFT resolution
int G; //Grid resolution

int work_group_size[2] = { 1, 1 };

int window_width = 640*2, window_height = 480*2;
GLsizei v_size,i_size;

GLint light_position;
GLuint displacementMap,normalMap,foamMap;
Shader sh("vertexshader.glsl", "fragmentshader.glsl");


CL::platform platform;
CL::device device;// = CL:
CL::gl_context context;//
CL::command_queue queue;
CL::program program;
CL::kernel setHeight,setNormal;//,computeCurvature;
CL::kernel createfftWave;
CL::image_opengl displacement,normals,foam,tmp_foam;
CL::shared_data<cl_float> Y,y, X,x, Z,z;

CL::shared_data<cl_float> amplitude, phase;
cl_float L;
FFT2 fftY,fftX,fftZ;
bool slowTime = false;
bool repeat = false;
bool simplify = false;

template<class T>
void i_mat_txt(const T *val,int n,int m,const char *filename){
    std::ofstream myfile;
    myfile.open (filename);
    myfile.precision(15); //Double precision
    for (int j = 0; j < m; j++) {
        for (int i = 0; i < n; i++) {
            const int idx = 2*i+j*2*n;
            myfile << " "<< val[idx] << (val[idx+1] < 0 ? "" : "+") << val[idx+1] << "i, ";
        }
        myfile << std::endl;
    }
    myfile.close();
}

template<class T>
void mat_txt(const T *val,int n,int m,const char *filename){
    std::ofstream myfile;
    myfile.open (filename);
    myfile.precision(15); //Double precision
    for (int j = 0; j < m; j++) {
        for (int i = 0; i < n; i++) {
            myfile << " "<< val[i+j*n] << ", ";
        }
        myfile << std::endl;
    }
    myfile.close();
}
enum mat_type{
	complex = 0,
	real,
};


#define Q(...)  #__VA_ARGS__ "\n"
#define MATLAB(x) Q(x)
void matlabcode(){
	std::fstream myfile;
	//chk if file empty
	if (myfile.peek() == std::ifstream::traits_type::eof()) {
		myfile.open("matlab.m");
        
		const char *mbcode =
        MATLAB(clear all;)
        MATLAB(close all;)
        MATLAB(Nx = 10;)
        MATLAB(Ny = 10;)
        MATLAB(Y = load('Y.txt');)
        MATLAB(y = load('y_.txt') * 40;)
        MATLAB(A = load('Amp.txt');)
        
        MATLAB(imshow(Y);)
        MATLAB(figure;)
        MATLAB(imshow(y))
        MATLAB(figure;)
        MATLAB(imshow(A))
        
        MATLAB(mat = zeros(Nx, Ny);)
        
        MATLAB(for j = 1:Ny)
        MATLAB(for i = 1 : Nx)
        MATLAB(mat(j, i) = sin(i);)
        MATLAB(end)
        MATLAB(end)
        MATLAB(mat;)
        MATLAB(ifft2(mat);) ;
        
		myfile << mbcode;
	}
	myfile.close();
}

template<typename T>
void matlabMatrix(CL::shared_data<cl_float> *mem, CL::command_queue queue, int col, int row, mat_type mtype, const char* name, T modifyData){
	//add fileending
	size_t len = std::strlen(name);
	char *filename = new char[len + 3];
	strcpy(filename, name);
	strcat(filename, ".txt");
    
	mem->sync_read(queue);
	modifyData();
	(mtype == complex) ? i_mat_txt(mem->ptr(), N, N, filename) : mat_txt(mem->ptr(), N, N, filename);
}

void openCLStuff(cl_float time){
    
	//Translate vertices
    CL::event e1 = displacement.aquireGLObject(queue);
    CL::event e2 = normals.aquireGLObject(queue);
    
    e1.wait();
    e2.wait();

#if 0
	naiveWave.setArg(5,time);
	e1 = naiveWave.async_execute2D(queue, std::min(work_group_size[0], n), std::min(work_group_size[1], n), n, n);
	e1.wait();
#else
	//Fill Y
	{
        createfftWave.setArg(7, time);
		CL::event e = createfftWave.async_execute2D(queue,
                                                    std::min(work_group_size[0], N),
                                                    std::min(work_group_size[1], N),
                                                    N,
                                                    N);
		e.wait();
	}
    
    //Shift Y

	/*
     matlabcode();
     matlabMatrix(&amplitude, queue, n, n, real, "Amp", [](){});
     matlabMatrix(&Y, queue, n, n, complex, "Y", [=](){
     for (int i = 0; i < 2 * n; i++) {
     Y.ptr()[i] = 0;
     }
     Y.sync_write(queue);
     });
     */
#if USE_MATLAB
    Y.sync_read(queue);
    MatlabHelper::copyToMatlabComplex(Y.ptr(), mY);
    engPutVariable(engine, "mY", mY);
    engEvalString(engine, "my = if2(mY);");
    engEvalString(engine, "my = single(my);");
    mxArray *my = engGetVariable(engine, "my");
    static cl_float *tmp = new cl_float[n*n];
    MatlabHelper::copyFromMatlabReal(my, tmp);
    
    for (int i = 0; i < n*n; i++) {
        y.ptr()[2*i] = tmp[i];
    }
    y.sync_write(queue);
    
    mxDestroyArray(my);
    
#else
    
    fftY(Y.dataHandle(), y.dataHandle());
	fftX(X.dataHandle(), x.dataHandle());
	fftZ(Z.dataHandle(), z.dataHandle());

#endif
    
    //set heights..
    {
		setHeight.setArg(5, (cl_int)simplify);
		CL::event e = setHeight.async_execute2D(queue, std::min(work_group_size[0], N), std::min(work_group_size[1], N), N, N);
        e.wait();
    }
    
    //set normals
    {
		CL::event e = setNormal.async_execute2D(queue, std::min(work_group_size[0], N), std::min(work_group_size[1], N), N, N);
        e.wait();
    }
    
    //Compute curvature
    {
		//CL::event e = computeCurvature.async_execute2D(queue, std::min(work_group_size[0], n), std::min(work_group_size[1], n), n, n);
        //e.wait();
    }
#endif

    e1 = displacement.releaseGLObject(queue);
    e2 = normals.releaseGLObject(queue);

	e1.wait();
    e2.wait();
}

void mainLoop(const GLWindow &stuff){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1, 1, 1, 1);
	
    //Time
    static cl_float time = 0;
	
	if (slowTime){
		//time += 0.0016*2;
       // time += 0.0016*2;
	}
	else{
		time += 0.016*3;
	}
    
    glFinish();
    
    openCLStuff(time);
    
    clFinish(queue.handle());
    
    
    //Camera
    glm::mat4 ProjectionMatrix = glm::perspective(3.14f,         // The horizontal Field of View, in degrees : the amount of "zoom".
                                                  // Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
                                                  (float)window_width / (float)window_height,         // Aspect Ratio. Depends on the size of your window.
                                                  // Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
                                                  0.1f,               // Near clipping plane. Keep as big as possible, or you'll get precision issues.
                                                  100.0f);            // Far clipping plane. Keep as little as possible.
    
    glm::mat4 ViewMatrix = glm::mat4(1.0);
	
    ViewMatrix = glm::translate(glm::vec3(0.0,0.0, stuff.zoom()-4.0));
	
    ViewMatrix = ViewMatrix * glm::rotate(glm::mat4(1.f),20+(float)stuff.x_rot(), glm::vec3(0.0f, 1.0f, 0.0f));
    ViewMatrix = ViewMatrix * glm::rotate(glm::mat4(1.f),35+(float)stuff.y_rot(), glm::vec3(1.0f, 0.0f, 0.0f));
    
    ViewMatrix = ViewMatrix * glm::translate(glm::vec3((float)stuff.x_pan(), (float)stuff.y_pan(), 0.0f));
    
    
    glm::mat4 ModelMatrix(1.0f);
    ModelMatrix = ModelMatrix*glm::scale(glm::vec3(1.f/float(G-1), 1.f/float(N-1),1.f/float(G-1)));
    ModelMatrix = glm::translate(glm::vec3(-0.5f,0.f,-0.5f))*ModelMatrix;
    
    //glm::mat4 ModelMatrix = glm::scale(1.f, 1.0f,1.f);
    
    // Render here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    
#if 1
	glUseProgram(sh.program());
    
    GLint vertexLoc = sh.attribute("vertex_");
    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(vertexLoc);
    glBindBuffer(GL_ARRAY_BUFFER,stuff.vbo);
    glVertexAttribPointer(vertexLoc,                  // attribute
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0);          // array buffer offset

    
    auto trans = glm::translate(glm::vec3(0.0, 0.0, 1.0));
    glMultMatrixf((GLfloat*)glm::value_ptr(trans));
    
    
    
#if 1
	GLint projectionLoc = sh.uniform("Projection");
    GLint viewLoc       = sh.uniform("View");
	GLint ModelLoc      = sh.uniform("Model");
    GLint timeLoc       = sh.uniform("time");
    GLint gLoc       = sh.uniform("g");

    GLint displacementMapLoc = sh.uniform("displacementMap");
    GLint normalMapLoc = sh.uniform("normalMap");
    GLint foamMapLoc = sh.uniform("foamMap");

#else
    GLint projectionLoc = glGetUniformLocation(sh.program(),"Projection"); // nope.
	GLint viewLoc       = glGetUniformLocation(sh.program(),"View");
	GLint ModelLoc      = glGetUniformLocation(sh.program(),"Model");
#endif
    
	static bool once = true;
	if (once){
		once = false;
		std::cout << "Attr: " << vertexLoc << " " << " Mat4: " << projectionLoc << " " << viewLoc << " " << ModelLoc << " sampler2D " << displacementMapLoc << " " << normalMapLoc << " " <<foamMapLoc  <<  std::endl;
	}
    
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (GLfloat*)glm::value_ptr(ProjectionMatrix)); // NOPE.
	glUniformMatrix4fv(viewLoc,       1, GL_FALSE, (GLfloat*)glm::value_ptr(ViewMatrix));
	glUniformMatrix4fv(ModelLoc,      1, GL_FALSE, (GLfloat*)glm::value_ptr(ModelMatrix));
    
    glUniform1f(timeLoc, time);
    glUniform1i(gLoc, G);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, displacementMap);
    glUniform1i(displacementMapLoc,0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMap);
    glUniform1i(normalMapLoc,1);

    // Draw the triangles !
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, stuff.ibo);


	if (repeat){
		for (int i = 0; i < 2; i++){
			for (int j = 0; j < 2; j++){
                glm::mat4 trans = glm::translate(glm::vec3((float)i-0.5f, 0.0f, (float)j-0.5f));
				trans = trans*ModelMatrix;
				glUniformMatrix4fv(ModelLoc,      1, GL_FALSE, (GLfloat*)glm::value_ptr(trans));
				glDrawElements(GL_TRIANGLES,      // mode
					   i_size,            // count
					   GL_UNSIGNED_INT,   // type
					   (void*)0);         // element array buffer offset
			}
		}
	}
	else{
		glDrawElements(GL_TRIANGLES,      // mode
                i_size,            // count
                GL_UNSIGNED_INT,   // type
                (void*)0);         // element array buffer offset
	}
    
    glDisableVertexAttribArray(vertexLoc);

	glUseProgram(0); //stop using program
#endif
    // Swap front and back buffers
    glfwSwapBuffers(stuff.window);
    
    // Poll for and process events
    glfwPollEvents();
}

void print_platforms_devices(){
    std::map<std::string,bool> n_devices;
    auto platforms = CL::platform::all_platforms();
    std::cout << "----- Platforms -----" << std::endl;
    for (auto p : platforms){
        std::cout << "   *" << p.name() << std::endl;
        auto devices = CL::device::all_devices(p, CL_DEVICE_TYPE_ALL);
        for (CL::device &d : devices) {
            n_devices.insert(std::pair<std::string, bool>(d.name(),d.available()));
        }
    }
    
    std::cout << "------ Devices ------" << std::endl;
    for (auto it : n_devices) {
        std::cout << "   *" << it.first;
        if (!it.second) {
            std::cout << " (not available)";
        }
        std::cout << std::endl;
    }
    std::cout << "---------------------" << std::endl;
    
}

int run_arguments(int argc,  char * argv[],
                  char *&preffered_platform,char *&preffered_device,bool &list_all){
    list_all = false;
    int c;
    while ((c = getopt (argc, argv, "p:d:lx:y:n:g:")) != -1)
        switch (c)
    {
        case 'p':
            if (optarg && strlen(optarg) != 0){
                size_t size = strlen(optarg);
                preffered_platform = new char[size+1];
                strcpy(preffered_platform, optarg);
				preffered_platform[size] = 0;
            }
            break;
		case 'x':
			work_group_size[0] = atoi(optarg);
			break;
		case 'y':
			work_group_size[1] = atoi(optarg);
			break;
        case 'n':
			N = atoi(optarg);
			break;
        case 'g':
			G = atoi(optarg);
			break;
        case 'd':
            if (optarg){
                preffered_device = new char[strlen(optarg)+1];
                strcpy(preffered_device,optarg);
				preffered_device[strlen(optarg)] = 0;
            }
            break;
        case 'l':
            list_all = true;
            break;
        case '?':
            if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr,
                         "Unknown option character `\\x%x'.\n",
                         optopt);
            return 0;
        default:
            abort ();
    }
    return 1;
}

void keyboardFunc(GLFWwindow* w,int key, int scancode, int action, int mods){
	if (action == GLFW_PRESS){
		if (key == GLFW_KEY_F1){
			slowTime = !slowTime;
		}else if (key == GLFW_KEY_F2){
			repeat = !repeat;
		}else if (key == GLFW_KEY_F3){
			simplify = !simplify;
		}		
	}
}


int main(int argc,  char * argv[]){
    //Options
    char *p_platform = nullptr;
    char *p_device = nullptr;
    bool list_opencl;
    if(!run_arguments(argc,argv,p_platform,p_device,list_opencl)){
        std::cout << "Bad arguments, terminating" << std::endl;
        return -1;
    }
    
    // --
	try{
		Geometry::computeVertexStripSizes(G-1, G-1, v_size, i_size);
		//
		GLfloat *verts = new GLfloat[v_size];
		GLfloat *normal = new GLfloat[v_size];
		std::fill(normal, normal + v_size, 1.f);
		GLuint *ind = new GLuint[i_size];
        
		Geometry::generateVertexStrip(G-1, G-1, verts, ind,false);
		GLWindow gl;
		gl.init(window_width, window_height, verts, v_size, ind, i_size);
		sh.createProgram();
        
        
        GLfloat *image = new GLfloat[N*N*4];
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                image[3*i+3*j*N+0] = 0;
                image[3*i+3*j*N+1] = 0;
                image[3*i+3*j*N+2] = 0;
            }
        }
        displacementMap  = generateImage2D(image,N,N);
        normalMap = generateImage2D(image, N, N);
        
        GLuint tmp_foamMap = generateImage2D(image, N, N);
        foamMap = generateImage2D(image, N, N);

        delete [] image;
        
        
		glFinish();
		//OpenCL
        
        //Print all platforms and devices!
        if(list_opencl){
            std::cout << std::endl;
            print_platforms_devices();
        }
        
        //Select device & platform
        std::cout << std::endl;
        if (p_platform) {
            platform = CL::platform::preferred_platform(p_platform);
        }else{
            platform = CL::platform::any_platform();
        }
        std::cout << "Platform loaded: " << platform.name() << std::endl;
        if (p_device) {
            device = CL::device::preferred_device(platform, p_device);
        }else{
            device = CL::device::any_avaiable_device(platform);
        }
        std::cout << "Device loaded: " << device.name() << std::endl;
        cl_ulong global_bytes = device.global_mem_size();
        std::cout << "          mem: " << global_bytes << " bytes, " << global_bytes/(1024*1024) << " mb"  << std::endl;
        
        
#if __APPLE__
		context = CL::gl_context(device);
#else
		context = CL::gl_context(device, platform);
#endif
        
		queue         = CL::command_queue(context, device);
		program       = CL::program::file(context, device, "kernel.cl","-cl-nv-verbose");
        createfftWave = CL::kernel(program, "fftWave");
		setHeight     = CL::kernel(program, "setHeight");
        setNormal     = CL::kernel(program, "setNormal");

        //computeCurvature = CL::kernel(program, "computeCurvature");
        
		//std::cout << "computeCurvature.private_mem_size(device) "<< computeCurvature.private_mem_size(device) << std::endl;
        
		amplitude = CL::shared_data<cl_float>(context, CL_MEM_READ_ONLY, N*N);
		phase     = CL::shared_data<cl_float>(context, CL_MEM_READ_ONLY, N*N);
        
		L = 50.f;
        float g = 9.82;
        
        glm::vec2 wind(1.f,0.f);
        float wind_speed = 10.5;
        
        //float max_wind_speed = std::sqrt(dx*1000*g);
        wind = glm::normalize(wind);
        
        wind *= wind_speed;// max_wind_speed;
        
        /*
         What range of scales is reasonable to choose? The answer to this question comes down to choosing values for Lx, Lz, M, and N. The smallest facet in either direction is dx ≡ Lx/M or dz ≡ Lz /N . Generally, dx and dz need never go below 2 cm or so. Below this scale, the amount of wave action is small compared to the rest of the waves. Also, the physics of wave behavior below 2 cm begins to take on a very differ- ent character, involving surface tension and “nonlinear” processes. From the form of the spectrum, waves with a wave- length larger than V^2/g are suppressed. So make sure that dx and dz are smaller than V^2/g by a substantial amount (10 - 1000) or most of the interesting waves will be lost. The se- cret to realistic looking waves (e.g. figure 12 (a) compared to figure 12 (c)) is to have M and N as large as reasonable.
         */
        
        float V = glm::length(wind);
        float dx = L/float(N);
        std::cout << "dx : " << dx << " V*V/9.82 " << V*V/g << " dx*1000 " << dx*1000 << std::endl;
        //assert(dx > 0.02f);
        //assert(V*V/g >= dx*1000);
        
        
		Spectrum::phillips_wave( N, L, glm::value_ptr(wind), amplitude.ptr() );
		Spectrum::random_phase( N, phase.ptr());
        
		amplitude.sync_write(queue);
		phase.sync_write(queue);
        
		//cls handle to vbo /nbo...
        
        displacement = CL::image_opengl(context,
                                        displacementMap,
                                        CL_MEM_READ_WRITE,
                                        4*sizeof(cl_float)*N*N);
        
        normals = CL::image_opengl(context,
                                   normalMap,
                                   CL_MEM_READ_WRITE,
                                   4*sizeof(cl_float)*N*N);
        
        
        foam = CL::image_opengl(context,
                                foamMap,
                                CL_MEM_READ_WRITE,
                                4*sizeof(cl_float)*N*N);
        
        tmp_foam = CL::image_opengl(context,
                                    tmp_foamMap,
                                    CL_MEM_READ_WRITE,
                                    4*sizeof(cl_float)*N*N);
        
        
        std::cout << "          phase: " << phase.dataHandle().mem_size()/(1024) << " kb"  << std::endl;
        std::cout << "          amp: " << amplitude.dataHandle().mem_size()/(1024) << " kb"  << std::endl;
        
        std::cout << std::endl;
        
        X = CL::shared_data<cl_float>(context,CL_MEM_READ_WRITE,2*N*N);
        x = CL::shared_data<cl_float>(context,CL_MEM_READ_WRITE,2*N*N);
		Y = CL::shared_data<cl_float>(context,CL_MEM_READ_WRITE,2*N*N);
        y = CL::shared_data<cl_float>(context,CL_MEM_READ_WRITE,2*N*N);
		Z = CL::shared_data<cl_float>(context,CL_MEM_READ_WRITE,2*N*N);
        z = CL::shared_data<cl_float>(context,CL_MEM_READ_WRITE,2*N*N);
        
        CL::event e3 = normals.aquireGLObject(queue);
        CL::event e4 = displacement.aquireGLObject(queue);
        e3.wait();
        e4.wait();
        
        createfftWave.setArgs(Y.handle(), X.handle(), Z.handle(), amplitude.handle(), phase.handle(), L, N, 0.f);
        setHeight.setArgs(displacement.data().handle(), y.handle(), x.handle(), z.handle(),N);
        setNormal.setArgs(normals.data().handle(),displacement.data().handle(),N);

        //computeCurvature.setArgs(displacement.data().handle(),normals.data().handle(),curvature.handle(),n);

        
        e3 = normals.releaseGLObject(queue);
        e4 = displacement.releaseGLObject(queue);
        e3.wait();
        e4.wait();
        
        fftY.init(context, queue, N, N);
        fftX.init(context, queue, N, N);
        fftZ.init(context, queue, N, N);

#if USE_MATLAB
        engine = MatlabHelper::createEngine();
        mY = mxCreateNumericMatrix_730(n, n, mxSINGLE_CLASS, mxCOMPLEX);
#endif
        
		gl.mainLoop = mainLoop;

		glfwSetKeyCallback(gl.window, keyboardFunc);
        
		gl.run();
		glfwTerminate();
	}
	catch (std::exception& e){
        
		std::cout << e.what() << std::endl;
	}
    if(p_device) delete [] p_device;
    if(p_platform) delete [] p_platform;
    fftY.destroy();
    fftX.destroy();
    fftZ.destroy();
    FFT2::teardown();
    
#if USE_MATLAB
    mxDestroyArray(mY);
#endif
    return 0;
}
