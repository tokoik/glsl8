# glsl8 - 第８回 視差マッピング サンプルプログラム

## 1. 概要

このプログラムは、OpenGL と GLSL (OpenGL Shading Language) を用いて「視差マッピング (Parallax Mapping)」を実装するための、学生向けのサンプルプログラムです。本プログラムは、以下のブログ記事の解説に沿って学習を進めるための雛形として提供されています。

- [第８回 視差マッピング](https://tokoik.github.io/blog/opengl/2006/05/25/glsl.html)

従来のバンプマッピングでは陰影の変化のみで凹凸を表現していたため、斜めから見たときに凹凸のずれ（視差）が生じないという問題がありました。このプログラムでは、法線マップのアルファチャンネルに格納された高さ情報と視線方向ベクトルを用いてサンプリングするテクスチャ座標をずらすことで、斜めから見たときにもリアルな凹凸の立体感（視差効果）を再現します。

## 2. ビルド方法

このプログラムは [CMake](https://cmake.org/) を用いてビルド環境を整備します。各OSとも、ソースコードが置かれているディレクトリにターミナル（またはコマンドプロンプト）で移動してから、以下の手順を実行してください。なお、プログラムをビルドするためのバイナリディレクトリは、バージョン管理ファイル（.gitignore）の設定に合わせて **build** という名前にします。

### 2.1 Windows (Visual Studio 2022 の場合)

1. コマンドプロンプトまたは PowerShell を開き、このプロジェクトのディレクトリに移動します。
2. 以下のコマンドを実行してビルドディレクトリを作成し、CMake で構成を行います。

   ```bat
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022"
   ```

3. 生成された build フォルダ内の glsl8.sln を Visual Studio で開きます。
4. ソリューションエクスプローラーで **glsl8** プロジェクトを右クリックし、「スタートアップ プロジェクトに設定」を選択します。
5. 「ローカル Windows デバッガー」をクリックするか、F5 キーを押してビルドおよび実行します。

### 2.2 macOS (Xcode の場合)

1. ターミナルを開き、このプロジェクトのディレクトリに移動します。
2. 以下のコマンドを実行してビルドディレクトリを作成し、Xcode 用のプロジェクトを生成します。

   ```sh
   mkdir build
   cd build
   cmake .. -G Xcode
   ```

3. 生成された build/glsl8.xcodeproj を Xcode で開きます。
4. 左上のスキーム選択（再生ボタンの横）が **glsl8** になっていることを確認します。
5. 「Run」ボタン（再生ボタン）をクリックするか、Command + R を押してビルドおよび実行します。

### 2.3 Ubuntu Linux

1. ターミナルを開き、このプロジェクトのディレクトリに移動します。
2. 必要なパッケージ（freeglut3-dev など）がインストールされていることを確認し、以下のコマンドでビルドします。

   ```sh
   mkdir build
   cd build
   cmake ..
   make
   ```

## 3. 使い方

### 3.1 プログラムの起動方法

- **Windows**: `build\Debug\glsl8.exe`
- **macOS**: `open build/Debug/glsl8.app` または Xcode 上で Run
- **Ubuntu Linux**: `cd build && ./glsl8`

### 3.2 操作方法

- **マウスの左ボタンでドラッグ**: 画面内のオブジェクトを 3 次元的に回転
- **キーボードの q, Q または ESC キー**: プログラムを終了

## 4. 解説

### 4.1 ディフューズテクスチャと法線マップのマルチテクスチャ

- テクスチャユニット0: 高さ情報がアルファチャンネルに入った法線マップ (`dotbump.raw`)
- テクスチャユニット1: ディフューズテクスチャ (`dot.raw`)

### 4.2 視差マッピングの計算 (bump.frag)

フラグメントシェーダ内で視線方向単位ベクトル `fview` を求め、テクスチャ座標をずらします。

```glsl
vec4 color = texture2DProj(texture, gl_TexCoord[0]);
vec3 fview = normalize(view);

// 視線方向の xy 成分と高さ情報 (color.a) からずらしたテクスチャ座標を算出
vec2 texcoord = gl_TexCoord[0].xy - fview.xy * color.a * 0.02;

// ずらしたテクスチャ座標で法線マップとディフューズテクスチャをサンプリング
vec3 fnormal = vec3(texture2D(texture, texcoord)) * 2.0 - 1.0;
vec4 dcolor = texture2D(dtexture, texcoord);
```
