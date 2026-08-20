/* OpenGL / GLSL 関連の宣言 */
#include "glsl.h"

/* 法線マップを作成する関数の宣言 */
#include "normalmap.h"

/* 球を描く関数の宣言 */
#include "sphere.h"

/* トラックボール処理用関数の宣言 */
#include "trackball.h"

/* 標準ライブラリ */
#include <stdio.h>
#include <stdlib.h>

/* 1 ならティーポットを描く */
#define DRAW_TEAPOT 0

/*
** 光源
*/
static const GLfloat lightpos[] = { 4.0f, 5.0f, 6.0f, 1.0f }; /* 位置　　　 */
static const GLfloat lightcol[] = { 1.0f, 1.0f, 1.0f, 1.0f }; /* 直接光強度 */
static const GLfloat lightamb[] = { 0.1f, 0.1f, 0.1f, 1.0f }; /* 環境光強度 */

/*
** プログラムオブジェクト
*/
static GLuint gl2Program;

/*
** 接線ベクトルを格納する attribute 変数の場所
*/
static GLint tangentLoc;

/*
** テクスチャのサンプラの uniform 変数の場所
*/
static GLuint colorLoc;
static GLuint dcolorLoc;

/*
** テクスチャ
*/
#define TEXWIDTH  256                               /* テクスチャの幅　　　 */
#define TEXHEIGHT 256                               /* テクスチャの高さ　　 */
static const char texture_file[] = "dotbump.raw";   /* テクスチャファイル名 */

/*
** 初期化
*/
static void init()
{
  /* GLSL の初期化 */
  if (glslInit()) exit(1);

  /* シェーダオブジェクトの作成 */
  GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

  /* シェーダのソースプログラムの読み込み */
  if (readShaderSource(vertShader, "bump.vert")) exit(1);
  if (readShaderSource(fragShader, "bump.frag")) exit(1);

  /* シェーダプログラムのコンパイル結果 */
  GLint compiled;

  /* バーテックスシェーダのソースプログラムのコンパイル */
  glCompileShader(vertShader);
  glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
  printShaderInfoLog(vertShader);
  if (compiled == GL_FALSE) {
    fprintf(stderr, "Compile error in vertex shader.\n");
    exit(1);
  }

  /* フラグメントシェーダのソースプログラムのコンパイル */
  glCompileShader(fragShader);
  glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
  printShaderInfoLog(fragShader);
  if (compiled == GL_FALSE) {
    fprintf(stderr, "Compile error in fragment shader.\n");
    exit(1);
  }

  /* プログラムオブジェクトの作成 */
  gl2Program = glCreateProgram();

  /* シェーダオブジェクトのシェーダプログラムへの登録 */
  glAttachShader(gl2Program, vertShader);
  glAttachShader(gl2Program, fragShader);

  /* シェーダオブジェクトに削除マークを付ける */
  glDeleteShader(vertShader);
  glDeleteShader(fragShader);

  /* シェーダプログラムのリンク結果 */
  GLint linked;

  /* シェーダプログラムのリンク */
  glLinkProgram(gl2Program);
  glGetProgramiv(gl2Program, GL_LINK_STATUS, &linked);
  printProgramInfoLog(gl2Program);
  if (linked == GL_FALSE) {
    fprintf(stderr, "Link error.\n");
    exit(1);
  }

  /* 接線ベクトルを渡すために使う attribute 変数の場所を得る */
  tangentLoc = glGetAttribLocation(gl2Program, "tangent");

  /* テクスチャのサンプラの uniform 変数の場所を得る */
  colorLoc = glGetUniformLocation(gl2Program, "color");
  dcolorLoc = glGetUniformLocation(gl2Program, "dcolor");

#if defined(WIN32)
  glActiveTexture =
    (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
#endif

  /* テクスチャユニット０を指定する */
  glActiveTexture(GL_TEXTURE0);

  /* テクスチャオブジェクトの作成と結合 */
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  /* テクスチャを拡大・縮小する方法の指定 */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  /* テクスチャの繰り返し方法の指定 */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  /* テクスチャ画像はワード単位に詰め込まれている */
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

  /* テクスチャの読み込みに使う配列 */
  GLubyte texture[TEXHEIGHT][TEXWIDTH][4];

  /* 法線マップの作成 */
  makeNormalMap(texture, TEXWIDTH, TEXHEIGHT, 20.0, texture_file);

  /* テクスチャの割り当て */
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXWIDTH, TEXHEIGHT, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, texture);

  /* テクスチャユニット１を指定する */
  glActiveTexture(GL_TEXTURE1);

  /* テクスチャユニット１用のテクスチャオブジェクトの作成と結合 */
  GLuint dtex;
  glGenTextures(1, &dtex);
  glBindTexture(GL_TEXTURE_2D, dtex);
  
  /* テクスチャを拡大・縮小する方法の指定 */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
  /* テクスチャの繰り返し方法の指定 */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
  /* テクスチャ画像の読み込み */
  FILE *fp = fopen("dot.raw", "rb");
  if (fp != NULL) {
    fread(texture, sizeof texture, 1, fp);
    fclose(fp);
  }
  
  /* テクスチャの割り当て */
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXWIDTH, TEXHEIGHT, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, texture);

  /* 初期設定 */
  glClearColor(0.3f, 0.3f, 1.0f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  /* 光源の初期設定 */
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightcol);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightcol);
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
}

/*
** シーンの描画
*/
static void scene()
{
  /* 材質 */
  static const GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
  static const GLfloat specular[] = { 0.3f, 0.3f, 0.3f, 1.0f };

  /* 材質の設定 */
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);

#if DRAW_TEAPOT
  /* ティーポットを描く */
  glutSolidTeapot(1.0);
#else
  /* 球を描く */
  sphere(1.0, 64, 32, tangentLoc);
#endif
}

/****************************
** GLUT のコールバック関数 **
****************************/

static void display()
{
  /* シェーダプログラムの適用 */
  glUseProgram(gl2Program);

  /* テクスチャのサンプラにテクスチャユニットを指定する */
  glUniform1i(colorLoc, 0);
  glUniform1i(dcolorLoc, 1);

  /* モデルビュー変換行列の設定 */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  /* 光源の位置を設定 */
  glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

  /* 視点の移動（物体の方を奥に移動）*/
  glTranslated(0.0, 0.0, -3.0);

  /* トラックボール処理で図形を回転 */
  glMultMatrixd(trackballRotation());

  /* 画面クリア */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* シーンの描画 */
  scene();

  /* シェーダプログラムの適用解除 */
  glUseProgram(0);

  /* ダブルバッファリング */
  glutSwapBuffers();
}

static void resize(int w, int h)
{
  /* トラックボールする範囲 */
  trackballRegion(w, h);

  /* ウィンドウ全体をビューポートにする */
  glViewport(0, 0, w, h);

  /* 透視変換行列の指定 */
  glMatrixMode(GL_PROJECTION);

  /* 透視変換行列の初期化 */
  glLoadIdentity();
  gluPerspective(60.0, (double)w / (double)h, 0.1, 10.0);
}

static void idle()
{
  /* 画面の描き替え */
  glutPostRedisplay();
}

static void mouse(int button, int state, int x, int y)
{
  switch (button) {
  case GLUT_LEFT_BUTTON:
    switch (state) {
    case GLUT_DOWN:
      /* トラックボール開始 */
      trackballStart(x, y);
      glutIdleFunc(idle);
      break;
    case GLUT_UP:
      /* トラックボール停止 */
      trackballStop(x, y);
      glutIdleFunc(0);
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

static void motion(int x, int y)
{
  /* トラックボール移動 */
  trackballMotion(x, y);
}

static void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 'q':
  case 'Q':
  case '\033':
    /* ESC か q か Q をタイプしたら終了 */
    exit(0);
  default:
    break;
  }
}

/*
** メインプログラム
*/
int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);
  init();
  glutMainLoop();
  return 0;
}
