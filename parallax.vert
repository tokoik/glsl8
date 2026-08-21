#version 120

// bump.vert

// 頂点の接線ベクトル
attribute vec3 tangent;

// ラスタライザに送る接空間の光線ベクトル
varying vec3 tlight;

// ラスタライザに送る接空間の視線ベクトル
varying vec3 tview;

void main()
{
  // 頂点のクリッピング座標値
  gl_Position = ftransform();

  // テクスチャ座標
  gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

  // 視点座標系の頂点の位置
  vec4 position = gl_ModelViewMatrix * gl_Vertex;

  // 視点座標系の法線ベクトル
  vec3 normal = normalize(gl_NormalMatrix * gl_Normal);

  // 視点座標系の光線ベクトル
  vec3 light = normalize((gl_LightSource[0].position * position.w
    - gl_LightSource[0].position.w * position).xyz);

  // 視点座標系の視線ベクトル
  vec3 view = -normalize(position.xyz);

  // 法線ベクトルと接線ベクトルから接空間への変換行列
  vec3 n = normal;
  vec3 t = normalize(gl_NormalMatrix * tangent);
  vec3 b = cross(n, t);

  // 接空間における光線ベクトル
  tlight.x = dot(t, light);
  tlight.y = dot(b, light);
  tlight.z = dot(n, light);

  // 接空間における視線ベクトル
  tview.x = dot(t, view);
  tview.y = dot(b, view);
  tview.z = dot(n, view);
}
