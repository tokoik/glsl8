#include <math.h>

#if defined(WIN32)
#  include "glut.h"
#  include "glext.h"
#elif defined(__APPLE__) || defined(MACOSX)
#  include <GLUT/glut.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/glut.h>
#endif

#include "sphere.h"
#include "glsl.h"

#define PI 3.14159265358979323846

/*
** ローカル座標系から n を法線ベクトルとする接ベクトル t を求める
*/
static void setTangent(const double n[3])
{
  double l = n[0] * n[0] + n[2] * n[2];
  double t[3];
  /* 接ベクトルを格納する attribute 変数のハンドル */
  extern GLint tangent;
  
  /* 接ベクトル = (0, 1, 0) × n */
  if (l > 0) {
    double a = sqrt(l);
    
    t[0] = n[2] / a;
    t[1] = 0.0;
    t[2] = -n[0] / a;
  }
  else {
    t[0] = t[1] = t[2] = 0.0;
  }
  
  glVertexAttrib3dv(tangent, t);
}

/*
** 球の描画
*/
void sphere(double radius, int slices, int stacks)
{
  /* 球を描く */
  for (int j = 0; j < stacks; ++j) {
    double t0 = (double)(j + 1) / (double)stacks;
    double t1 = (double)j / (double)stacks;
    double r0 = sin(PI * t0);
    double r1 = sin(PI * t1);
    double n[2][3], p[2][3];
    
    /* 法線単位ベクトルの y 成分 */
    n[0][1] = -cos(PI * t0);
    n[1][1] = -cos(PI * t1);
    
    /* 頂点の y 座標値 */
    p[0][1] = radius * n[0][1];
    p[1][1] = radius * n[1][1];
    
    /* 法線マップのテクスチャ座標の算出 */
    t0 *= 4.0;
    t1 *= 4.0;
    
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; ++i) {
      double s = (double)i / (double)slices;
      double a = -2.0 * PI * s;
      
      /* 法線単位ベクトルの x, z 成分 */
      n[0][0] = r0 * cos(a);
      n[0][2] = r0 * sin(a);
      n[1][0] = r1 * cos(a);
      n[1][2] = r1 * sin(a);
      
      /* 頂点の x, z 座標値 */
      p[0][0] = radius * n[0][0];
      p[0][2] = radius * n[0][2];
      p[1][0] = radius * n[1][0];
      p[1][2] = radius * n[1][2];
      
      /* 法線マップのテクスチャ座標の算出 */
      s *= 8.0;

      /* 法線マップのテクスチャ座標を設定する */
      glTexCoord2d(s, t0);
      
      /* 法線ベクトルを設定する */
      glNormal3dv(n[0]);

      /* 接ベクトルを設定する */
      setTangent(n[0]);
      
      /* 頂点位置 */
      glVertex3dv(p[0]);
      
      /* 法線マップのテクスチャ座標を設定する */
      glTexCoord2d(s, t1);
      
      /* 法線ベクトルを設定する */
      glNormal3dv(n[1]);

      /* 接ベクトルを設定する */
      setTangent(n[1]);
      
      /* 頂点位置 */
      glVertex3dv(p[1]);
    }
    glEnd();
  }
}
