#ifndef __Ocean__VertexStrip__
#define __Ocean__VertexStrip__

#include <iostream>
/**
 @brief Helper to generate geometry programatically for OpenGL.
 */
namespace Geometry {
    /**
     @brief Generates a plane divided in triangles of equal size.
     @param[in] x Number of rows
     @param[in] y Number of coloumns
     @param[out] verticies_size Number of verticies that will be needed
     @param[out] index_size Number of indices that will be needed
     */
    void computeVertexStripSizes(int x,int y,GLsizei &verticies_size,GLsizei &index_size);
    
    /**
     @brief Generates a plane divided in triangles of equal size. 
     @see computeVertexStripSizes
     @param[in] x Number of rows
     @param[in] y Number of coloumns
     @param[in] spacing_x Spacing between each row
     @param[in] spacing_y Spacing between each coloumns
     @param[out] verticies Array containing each vertex. Needs to bee allocated in beforhand.
     @param[out] indices Array containing each index information. Needs to bee allocated in beforhand.
     */
    void generateVertexStrip(int x,int y,GLfloat *verticies,GLuint *indices,bool normalized = false );
};
#endif /* defined(__Ocean__VertexStrip__) */
