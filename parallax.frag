#version 120

// parallax.frag

// ラスタライザから受け取る接空間の光線ベクトルの補間値
varying vec3 tlight;

// ラスタライザから受け取る接空間の視線ベクトルの補間値
varying vec3 tview;

// テクスチャのサンプラ
uniform sampler2D color;

// 拡散反射色のサンプラ
uniform sampler2D dcolor;

void main ()
{
  // テクスチャから画素の色を得る
  vec4 fcolor = texture2DProj(color, gl_TexCoord[0]);

  // 接空間における視線ベクトル
  vec3 view = normalize(tview);

  // 高さマップを使ってテクスチャ座標を接空間の視線ベクトル方向にずらす
  vec2 texcoord = gl_TexCoord[0].xy - view.xy * fcolor.a * 0.02;

  // ずらしたテクスチャ座標を使って法線マップを取り出して法線ベクトルを求める
  vec3 normal = vec3(texture2D(color, texcoord)) * 2.0 - 1.0;

  // ずらしたテクスチャ座標を使って拡散反射色を取り出す
  vec4 ecolor = texture2D(dcolor, texcoord);

  // 接空間における光線ベクトル
  vec3 light = normalize(tlight);

  // 拡散反射率
  float diffuse = max(dot(normal, light), 0.0);

  // 接空間における中間ベクトル
  vec3 halfway = normalize(light + view);

  // 鏡面反射率
  float specular = pow(max(dot(normal, halfway), 0.0),
    gl_FrontMaterial.shininess);

  // フラグメントの色
  gl_FragColor = gl_FrontLightProduct[0].ambient * ecolor
               + gl_FrontLightProduct[0].diffuse * diffuse * ecolor
               + gl_FrontLightProduct[0].specular * specular;
}
