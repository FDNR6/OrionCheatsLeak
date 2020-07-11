class Vector4
{
public:
	float x, y, z, w;

	inline Vector4() {
		x = y = z = w = 0.0f;
	}

	inline Vector4(float X, float Y, float Z, float W) {
		x = X; y = Y; z = Z; w = W;
	}

	inline float operator[](int i) const {
		return ((float*)this)[i];
	}

	inline Vector4& operator+=(float v) {
		x += v; y += v; z += v; w += v;  return *this;
	}

	inline Vector4& operator-=(float v) {
		x -= v; y -= v; z -= v; w -= v;  return *this;
	}

	inline Vector4& operator-=(const Vector4& v) {
		x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this;
	}

	inline Vector4 operator*(float v) const {
		return Vector4(x * v, y * v, z * v, w * v);
	}

	inline Vector4 operator/(float v) const {
		return Vector4(x / v, y / v, z / v, w / v);
	}

	inline Vector4& operator+=(const Vector4& v) {
		x += v.x; y += v.y; z += v.z; w += v.w; return *this;
	}

	inline Vector4 operator-(const Vector4& v) const {
		return Vector4(x - v.x, y - v.y, z - v.z, z - v.w);
	}

	inline Vector4 operator+(const Vector4& v) const {
		return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	inline Vector4& operator/=(float v) {
		x /= v; y /= v; z /= v; w /= v;  return *this;
	}
};

class Vector3
{
public:
	float x, y, z;

	inline Vector3() {
		x = y = z = 0.0f;
	}


	inline Vector3(float X, float Y, float Z) {
		x = X; y = Y; z = Z;
	}


	inline float operator[](int i) const {
		return ((float*)this)[i];
	}

	inline Vector3& operator+=(float v) {
		x += v; y += v; z += v; return *this;
	}

	inline Vector3& operator-=(float v) {
		x -= v; y -= v; z -= v; return *this;
	}

	inline Vector3& operator-=(const Vector3& v) {
		x -= v.x; y -= v.y; z -= v.z; return *this;
	}

	inline Vector3 operator*(float v) const {
		return Vector3(x * v, y * v, z * v);
	}

	inline Vector3 operator/(float v) const {
		return Vector3(x / v, y / v, z / v);
	}

	inline Vector3& operator+=(const Vector3& v) {
		x += v.x; y += v.y; z += v.z; return *this;
	}

	inline Vector3 operator-(const Vector3& v) const {
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	inline Vector3 operator+(const Vector3& v) const {
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	inline Vector3& operator/=(float v) {
		x /= v; y /= v; z /= v; return *this;
	}

	inline Vector3& operator*=(float v) {
		x *= v; y *= v; z *= v; return *this;
	}

	inline bool Zero() const {
		return (x > -0.1f && x < 0.1f && y > -0.1f && y < 0.1f && z > -0.1f && z < 0.1f);
	}
};

class Vector2 {
public:
	float x, y;

	inline Vector2() {
		x = y = 0.0f;
	}

	inline Vector2(float X, float Y) {
		x = X; y = Y;
	}

	inline Vector2 operator+(float v) const {
		return Vector2(x + v, y + v);
	}

	inline Vector2 operator-(float v) const {
		return Vector2(x - v, y - v);
	}

	inline Vector2& operator+=(float v) {
		x += v; y += v; return *this;
	}

	inline Vector2& operator*=(float v) {
		x *= v; y *= v; return *this;
	}

	inline Vector2& operator/=(float v) {
		x /= v; y /= v; return *this;
	}

	inline Vector2 operator-(const Vector2& v) const {
		return Vector2(x - v.x, y - v.y);
	}

	inline Vector2 operator+(const Vector2& v) const {
		return Vector2(x + v.x, y + v.y);
	}

	inline Vector2& operator+=(const Vector2& v) {
		x += v.x; y += v.y; return *this;
	}

	inline Vector2& operator-=(const Vector2& v) {
		x -= v.x; y -= v.y; return *this;
	}

	inline Vector2 operator/(float v) const {
		return Vector2(x / v, y / v);
	}

	inline Vector2 operator*(float v) const {
		return Vector2(x * v, y * v);
	}

	inline Vector2 operator/(const Vector2& v) const {
		return Vector2(x / v.x, y / v.y);
	}

	inline bool Zero() const {
		return (x > -0.1f && x < 0.1f && y > -0.1f && y < 0.1f);
	}
};

class Matrix3x4
{
public:
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
		};

		float m[3][4];
		float mm[12];
	};
};

class Matrix4x4
{
public:
	float m[4][4];
};

namespace Math
{
#define M_PI 3.14159265358979323846f
#define M_RADPI	57.2957795131f
#define M_DEGPI 0.01745329251f
#define M_PI_F ((float)(M_PI))
#define RAD2DEG(x) ((float)(x) * (float)(180.f / M_PI_F))
#define DEG2RAD(x) ((float)(x) * (float)(M_PI_F / 180.f))

#define sinf(a) ((float)FC(ntdll, sin, (double)(a)))
#define cosf(a) ((float)FC(ntdll, cos, (double)(a)))
#define atanf(a) ((float)FC(ntdll, atan, (double)(a)))
#define atan2f(a, b) ((float)FC(ntdll, atan2, (double)(a), (double)(b)))

#define powf(a, b) ((a) * (a))
#define fabs(a) ((a < 0) ? -a : a);
#define copysign(x,y) (((x < 0 && y > 0) || (x > 0 && y < 0)) ? -x : x)


	float acosf(float x) {
		float negate = float(x < 0);
		x = abs(x);
		float ret = -0.0187293;
		ret = ret * x;
		ret = ret + 0.0742610;
		ret = ret * x;
		ret = ret - 0.2121144;
		ret = ret * x;
		ret = ret + 1.5707288;
		ret = ret * sqrt(1.0 - x);
		ret = ret - 2 * negate * ret;
		return negate * 3.14159265358979 + ret;
	}

	float asinf(float x) {
		float negate = float(x < 0);
		x = abs(x);
		float ret = -0.0187293;
		ret *= x;
		ret += 0.0742610;
		ret *= x;
		ret -= 0.2121144;
		ret *= x;
		ret += 1.5707288;
		ret = 3.14159265358979 * 0.5 - sqrt(1.0 - x) * ret;
		return ret - 2 * negate * ret;
	}

	__forceinline float FastSQRT(float x) {
		union { int i; float x; } u;
		u.x = x; u.i = (u.i >> 1) + 0x1FC00000;
		u.x = u.x + x / u.x;
		return .25f * u.x + x / u.x;
	}

	__forceinline float FastDist2D(const Vector2& Src, const Vector2& Dst) {
		return FastSQRT(powf(Src.x - Dst.x, Src.x - Dst.x) + powf(Src.y - Dst.y, Src.y - Dst.y));
	}

	__forceinline float FastDist3D(const Vector3& Src, const Vector3& Dst) {
		return FastSQRT(powf(Src.x - Dst.x, Src.x - Dst.x) + powf(Src.y - Dst.y, Src.x - Dst.x) + powf(Src.z - Dst.z, Src.z - Dst.z));
	}

	__forceinline float Vec3Length(const Vector3& Src) {
		return sqrtf(powf(Src.x, 2.f) + powf(Src.y, 2.f) + powf(Src.z, 2.f));
	}

	__forceinline void ClampAngle(Vector3& Ang) {
		if (Ang.x < -75.f) Ang.x = -75.f;
		if (Ang.x > 75.f) Ang.x = 75.f;
		while (Ang.z < -180.f) Ang.z += 360.f;
		while (Ang.z > 180.f) Ang.z -= 360.f;
		Ang.y = 0.f;
	}

	__forceinline Vector3 CalcAngle(const Vector3& Src, const Vector3& Dst) {
		Vector3 Delta = Src - Dst;
		float Lenght = Vec3Length(Delta);
		float x = asinf(Delta.z / Lenght) * M_RADPI;
		float z = atanf(Delta.y / Delta.x) * M_RADPI;
		if (Delta.x >= 0.f) z += 180.f;
		return Vector3(x, 0.f, z + 90.f);
	}

	__forceinline Vector4 ToQuaternion(Vector3 Euler)
	{
		float yaw = (Euler.z * M_DEGPI) * 0.5f;
		float sy = sinf(yaw);
		float cy = cosf(yaw);
		
		float roll = (Euler.x * M_DEGPI) * 0.5f;
		float sr = sinf(roll);
		float cr = cosf(roll);

		float sp = 0.f;
		float cp = 1.f;

		return Vector4(
			cy * sr * cp - sy * cr * sp,
			cy * cr * sp + sy * sr * cp,
			sy * cr * cp - cy * sr * sp,
			cy * cr * cp + sy * sr * sp
		);
	}

	__forceinline Vector3 ToEuler(Vector4 Quaternion)
	{
		float x = atan2f(2.f * (Quaternion.w * Quaternion.x + Quaternion.y * Quaternion.z), (1.f - 2.f * (Quaternion.x * Quaternion.x + Quaternion.y * Quaternion.y))) * M_RADPI;
		float z = atan2f(2.f * (Quaternion.w * Quaternion.z + Quaternion.x * Quaternion.y), (1.f - 2.f * (Quaternion.y * Quaternion.y + Quaternion.z * Quaternion.z))) * M_RADPI;
		return Vector3(x, 0.f, z);
	}

	__forceinline float Dot(const Vector3& v1, const Vector3& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	__forceinline void SmoothAngle(Vector3 Src, Vector3& Dst, float FOV) {
		Vector3 Delta = Dst - Src;
		Math::ClampAngle(Delta);
		Dst = Src + Delta / FOV;
	}
}