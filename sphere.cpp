/*
** 球の描画
*/
#include "sphere.h"

/* 標準ライブラリ */
#include <math.h>

/*
** ローカル座標系から normal を法線ベクトルとする接線ベクトル tangent を求める
*/
static void setTangent(const double normal[3], double tangent[3])
{
  double length = hypot(normal[0], normal[2]);
  
  /* 接線ベクトル = (0, 1, 0) × n */
  if (length > 0) {
    tangent[0] = normal[2] / length;
    tangent[1] = 0.0;
    tangent[2] = -normal[0] / length;
  }
  else {
    tangent[0] = tangent[1] = tangent[2] = 0.0;
  }
}

/*
** 球の描画
*/
void sphere(double radius, int slices, int stacks, GLint tangentLoc)
{
  /* 球を描く */
  for (int j = 0; j < stacks; ++j) {
    double t0 = (double)(j + 1) / (double)stacks;
    double t1 = (double)j / (double)stacks;
    double r0 = sin(M_PI * t0);
    double r1 = sin(M_PI * t1);
    double normal[2][3], position[2][3], tangent[3];
    
    /* 法線単位ベクトルの y 成分 */
    normal[0][1] = -cos(M_PI * t0);
    normal[1][1] = -cos(M_PI * t1);
    
    /* 頂点の y 座標値 */
    position[0][1] = radius * normal[0][1];
    position[1][1] = radius * normal[1][1];
    
    /* 法線マップのテクスチャ座標の算出 */
    t0 *= 4.0;
    t1 *= 4.0;
    
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; ++i) {
      double s = (double)i / (double)slices;
      double a = -2.0 * M_PI * s;
      
      /* 法線単位ベクトルの x, z 成分 */
      normal[0][0] = r0 * cos(a);
      normal[0][2] = r0 * sin(a);
      normal[1][0] = r1 * cos(a);
      normal[1][2] = r1 * sin(a);
      
      /* 頂点の x, z 座標値 */
      position[0][0] = radius * normal[0][0];
      position[0][2] = radius * normal[0][2];
      position[1][0] = radius * normal[1][0];
      position[1][2] = radius * normal[1][2];
      
      /* 法線マップのテクスチャ座標の算出 */
      s *= 8.0;

      /* 法線マップのテクスチャ座標を設定する */
      glTexCoord2d(s, t0);
      
      /* 法線ベクトルを設定する */
      glNormal3dv(normal[0]);

      /* 接線ベクトルを求める */
      setTangent(normal[0], tangent);

      /* 接線ベクトルを attribute 変数に設定する */
      glVertexAttrib3dv(tangentLoc, tangent);

      /* 頂点位置 */
      glVertex3dv(position[0]);
      
      /* 法線マップのテクスチャ座標を設定する */
      glTexCoord2d(s, t1);
      
      /* 法線ベクトルを設定する */
      glNormal3dv(normal[1]);

      /* 接線ベクトルを求める */
      setTangent(normal[1], tangent);
      
      /* 接線ベクトルを attribute 変数に設定する */
      glVertexAttrib3dv(tangentLoc, tangent);

      /* 頂点位置 */
      glVertex3dv(position[1]);
    }
    glEnd();
  }
}
