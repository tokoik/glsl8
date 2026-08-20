#version 120

// bump.frag

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
  // 法線マップと高さマップを取り出す
  vec4 fcolor = texture2DProj(color, gl_TexCoord[0]);

  // 接空間の視線ベクトルを正規化する
  vec3 fview = normalize(tview);

  // 高さマップを使ってテクスチャ座標を接空間の視線ベクトル方向にずらす
  vec2 texcoord = gl_TexCoord[0].xy - fview.xy * fcolor.a * 0.02;

  // ずらしたテクスチャ座標を使って法線マップを取り出して法線ベクトルを求める
  vec3 fnormal = vec3(texture2D(color, texcoord)) * 2.0 - 1.0;

  // ずらしたテクスチャ座標を使って拡散反射色を取り出す
  vec4 dcolor = texture2D(dcolor, texcoord);

  // 接空間における光線ベクトル
  vec3 flight = normalize(tlight);

  // 拡散反射率
  float diffuse = max(dot(fnormal, flight), 0.0);

  // 接空間における中間ベクトル
  vec3 fhalfway = normalize(tlight + tview);

  // 鏡面反射率
  float specular = pow(max(dot(fnormal, fhalfway), 0.0), gl_FrontMaterial.shininess);

  // フラグメントの色
  gl_FragColor = gl_FrontLightProduct[0].ambient * dcolor
               + gl_FrontLightProduct[0].diffuse * diffuse * dcolor
               + gl_FrontLightProduct[0].specular * specular;
}
