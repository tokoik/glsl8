#if defined(WIN32)
#  include "glut.h"
#  include "glext.h"
extern PFNGLMULTITEXCOORD3DPROC glMultiTexCoord3d;
#elif defined(__APPLE__) || defined(MACOSX)
#  include <GLUT/glut.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/glut.h>
#endif

#include "rectangle.h"
#include "glsl.h"

/*
** 矩形の描画
*/
void rectangle(double w, double h)
{
  /* 頂点の座標値 */
  const GLdouble vertex[4][3] = {
    { -w, -h, 0.0 },
    {  w, -h, 0.0 },
    {  w,  h, 0.0 },
    { -w,  h, 0.0 }
  };
  extern GLint tangent;
  
  /* 頂点のテクスチャ座標 */
  static const GLdouble texcoord[4][2] = {
    { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 }
  };

  /* 矩形を描く */
  glBegin(GL_QUADS);
  
  glVertexAttrib3d(tangent, 1.0, 0.0, 0.0);
  for (int i = 0; i < 4; ++i) {
    /* 法線マップのテクスチャ座標を設定する */
    glTexCoord2dv(texcoord[i]);
    
    /* 対応する頂点座標の指定 */
    glVertex3dv(vertex[i]);
  }
  glEnd();
}
