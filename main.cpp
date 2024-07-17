#include <Novice.h>
#include "MyMath.h"

#include <cstdint>
#include <cassert>

#define _USE_MATH_DEFINES
#include <math.h>

#include<imgui.h>

const char kWindowTitle[] = "LE2C_07_オオサキ_ハルキ_タイトル";
MyMath* myMath = new MyMath();

Vector3 operator+(const Vector3& v1, const Vector3& v2) { return myMath->Add(v1, v2); }
Vector3 operator-(const Vector3& v1, const Vector3& v2) { return myMath->Subtract(v1, v2); }
Vector3 operator*(float s, const Vector3& v) { return myMath->MultiplyFV(s, v); }
Vector3 operator*(const Vector3& v1, const Vector3& v2) { return myMath->MultiplyVV(v1, v2); }
Vector3 operator*(const Vector3& v, float s) { return s * v; }
Vector3 operator/(const Vector3& v, float s) { return myMath->MultiplyFV(1.0f / s,v); }


//enum a{
//	x,
//	y, 
//	z
//};
//Vector3& operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }


//Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) { return myMath->MultiplyMatrix(m1, m2); }




struct Spring {
	Vector3 anchor;
	float naturalLength;
	float stiffness;
	float dampingCoefficient;
};


struct  Ball {
	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;
	float mass;
	float radius;
	unsigned int color;
};

float Length(const Vector3& v) {
	float result;
	result = (float)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	return result;
}
float Dot(const Vector3& v1, const Vector3& v2) {
	float result;
	result = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
	return result;
}
Vector3 Normalize(const Vector3& v) {
	Vector3 result;
	result.x = v.x / (float)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	result.y = v.y / (float)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	result.z = v.z / (float)sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	return result;
}


Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result{};

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}



void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHandleWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHandleWidth * 2.0f) / float(kSubdivision);

	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHandleWidth + (xIndex * kGridEvery);
		unsigned int color = 0xAAAAAAFF;
		Vector3 start{ x,0.0f,-kGridHandleWidth };
		Vector3 end{ x,0.0f,kGridHandleWidth };

		Vector3 startScreen = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 endScreen = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		if (x == 0.0f) {
			color = BLACK;
		}
		Novice::DrawLine((int)startScreen.x, (int)startScreen.y, (int)endScreen.x, (int)endScreen.y, color);
	}
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHandleWidth + (zIndex * kGridEvery);
		unsigned int color = 0xAAAAAAFF;
		Vector3 start{ -kGridHandleWidth,0.0f,z };
		Vector3 end{ kGridHandleWidth,0.0f,z };

		Vector3 startScreen = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 endScreen = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		if (z == 0.0f) {
			color = BLACK;
		}
		Novice::DrawLine((int)startScreen.x, (int)startScreen.y, (int)endScreen.x, (int)endScreen.y, color);

	}
}

struct Sphere {
	Vector3 center;
	float radius;
};


void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 30;
	const float kLonEvery = float(M_PI) / 8.0f;
	const float kLatEvery = float(M_PI) / 8.0f;

	float pi = float(M_PI);

	Vector3 pointAB[kSubdivision] = {};
	Vector3 pointAC[kSubdivision] = {};

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex;//緯度 シ－タ

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {

			float lon = lonIndex * kLonEvery;//経度　ファイ

			Vector3 a{ sphere.center.x + sphere.radius * std::cos(lat) * std::cos(lon), sphere.center.y + sphere.radius * std::sin(lat),sphere.center.z + sphere.radius * std::cos(lat) * std::sin(lon) };
			Vector3 b{ sphere.center.x + sphere.radius * std::cos(lat + lat) * std::cos(lon), sphere.center.y + sphere.radius * std::sin(lat + lat),sphere.center.z + sphere.radius * std::cos(lat + lat) * std::sin(lon) };
			Vector3 c{ sphere.center.x + sphere.radius * std::cos(lat) * std::cos(lon + lon), sphere.center.y + sphere.radius * std::sin(lat), sphere.center.z + sphere.radius * std::cos(lat) * std::sin(lon + lon) };


			Vector3 aScreen = Transform(Transform(a, viewProjectionMatrix), viewportMatrix);
			Vector3 bScreen = Transform(Transform(b, viewProjectionMatrix), viewportMatrix);
			Vector3 cScreen = Transform(Transform(c, viewProjectionMatrix), viewportMatrix);


			if (pointAB[latIndex].x != 0 && pointAB[lonIndex].x != 0) {
				Novice::DrawLine((int)aScreen.x, (int)aScreen.y, (int)pointAB[latIndex].x, (int)pointAB[latIndex].y, color);
				Novice::DrawLine((int)aScreen.x, (int)aScreen.y, (int)pointAC[lonIndex].x, (int)pointAC[lonIndex].y, color);
			}

			pointAB[latIndex] = aScreen;
			pointAC[lonIndex] = aScreen;
		}
	}
}


bool IsCollision(const Sphere& s1, const Sphere& s2) {
	float distanceX = (s2.center.x - s1.center.x) * (s2.center.x - s1.center.x);
	float distanceY = (s2.center.y - s1.center.y) * (s2.center.y - s1.center.y);
	float distanceZ = (s2.center.z - s1.center.z) * (s2.center.z - s1.center.z);


	if (distanceX + distanceY + distanceZ <= s1.radius + s2.radius) {
		return true;
	}
	return false;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};


	Spring spring{};
	spring.anchor = { 0.0f,0.0f,0.0f };
	spring.naturalLength = 1.0f;
	spring.stiffness = 100.0f;
	spring.dampingCoefficient = 2.0f;

	Ball ball{};
	ball.position = { 1.2f,0.0f,0.0f };
	ball.mass = 2.0f;
	ball.radius = 0.05f;
	ball.color = BLUE;


	float deltaTime = 1.0f / 60.0f;

	Vector3 cameraPosition = { 0.0f ,0.0f,-20.0f };
	Vector3 cameraTranslate = { 0.0f,-1.0f,-6.49f };
	Vector3 cameraRotate = { -0.26f,0.0f,0.0f };

	bool start = false;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		if (start) {
			Vector3 diff = ball.position - spring.anchor;
			float length = Length(diff);
			if (length != 0) {
				Vector3 direction = Normalize(diff);
				Vector3 restPosition = spring.anchor + direction * spring.naturalLength;
				Vector3 displacement = length * (ball.position - restPosition);
				Vector3 restoringForce = -spring.stiffness * displacement;
				//Vector3 Force = restoringForce;	

				Vector3 dampingForce = -spring.dampingCoefficient * ball.velocity;
				Vector3 force = restoringForce + dampingForce;

				ball.acceleration = force / ball.mass;

			}


			//ball.velocity += ball.acceleration * deltaTime;
			ball.velocity.x += ball.acceleration.x * deltaTime;
			ball.velocity.y += ball.acceleration.y * deltaTime;
			ball.velocity.z += ball.acceleration.z * deltaTime;


			ball.position.x += ball.velocity.x * deltaTime;
			ball.position.y += ball.velocity.y * deltaTime;
			ball.position.z += ball.velocity.z * deltaTime;
		}

		Matrix4x4 worldMatrix = myMath->MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 cameraMatrix = myMath->MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, cameraPosition);
		Matrix4x4 viewMatrix = myMath->Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = myMath->MakePerspectiveFovMatrix(0.45f, float(1280.0f) / float(720.0f), 0.1f, 100.0f);
		Matrix4x4 WorldViewProjectionMatrix = myMath->Multiply(worldMatrix, myMath->Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = myMath->MakeViewportMatrix(0, 0, float(1280.0f), float(720.0f), 0.0f, 1.0f);


		DrawGrid(WorldViewProjectionMatrix, viewportMatrix);

		Sphere sphere = {
			ball.position,
			ball.radius
		};

		DrawSphere(sphere, WorldViewProjectionMatrix, viewportMatrix, ball.color);


		Vector3 startLine = Transform(Transform(spring.anchor, WorldViewProjectionMatrix), viewportMatrix);
		Vector3 endLine = Transform(Transform(ball.position, WorldViewProjectionMatrix), viewportMatrix);



		Novice::DrawLine((int)startLine.x, (int)startLine.y, (int)endLine.x, (int)endLine.y,WHITE);



		ImGui::Begin("window");
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);

		ImGui::Checkbox("start",&start);

		ImGui::End();

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
