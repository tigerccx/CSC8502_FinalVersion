#include "Matrix4.h"

#include "Quaternion.h"

const Matrix4 Matrix4::identity = Matrix4(identity_);
const Matrix4 Matrix4::zero = Matrix4(zero_);

Matrix4::Matrix4(void)	{
	ToIdentity();
}

Matrix4::Matrix4(const float elements[16] )	{
	memcpy(this->values,elements,16*sizeof(float));
}

Matrix4::Matrix4(const Matrix4& other)
{
	memcpy(values, other.values, sizeof(float) * 16);
}

Matrix4::~Matrix4(void)	{
	ToIdentity();
}

void Matrix4::ToIdentity() {
	ToZero();
	values[0]  = 1.0f;
	values[5]  = 1.0f;
	values[10] = 1.0f;
	values[15] = 1.0f;
}

void Matrix4::ToZero()	{
	for(int i = 0; i < 16; i++)	{
		values[i] = 0.0f;
	}
}

Vector3 Matrix4::GetPositionVector() const{
	return Vector3(values[12],values[13],values[14]);
}

void	Matrix4::SetPositionVector(const Vector3 in) {
	values[12] = in.x;
	values[13] = in.y;
	values[14] = in.z;		
}

Vector3 Matrix4::GetScalingVector() const{
	return Vector3(values[0],values[5],values[10]);
}

void	Matrix4::SetScalingVector(const Vector3 &in) {
	values[0]  = in.x;
	values[5]  = in.y;
	values[10] = in.z;		
}

Matrix4 Matrix4::Perspective(float znear, float zfar, float aspect, float fov) {
	Matrix4 m;

	const float h = 1.0f / tan(fov*PI_OVER_360);
	float neg_depth = znear-zfar;

	m.values[0]		= h / aspect;
	m.values[5]		= h;
	m.values[10]	= (zfar + znear)/neg_depth;
	m.values[11]	= -1.0f;
	m.values[14]	= 2.0f*(znear*zfar)/neg_depth;
	m.values[15]	= 0.0f;

	return m;
}

//http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml
Matrix4 Matrix4::Orthographic(float znear, float zfar,float right, float left, float top, float bottom)	{
	Matrix4 m;

	m.values[0]	= 2.0f / (right-left);
	m.values[5]	= 2.0f / (top-bottom);
	m.values[10]	= -2.0f / (zfar-znear);

	m.values[12]  = -(right+left)/(right-left);
	m.values[13]  = -(top+bottom)/(top-bottom);
	m.values[14]  = -(zfar+znear)/(zfar-znear);
	m.values[15]  = 1.0f;

	return m;
}

Matrix4 Matrix4::BuildViewMatrix(const Vector3 &from, const Vector3 &lookingAt, const Vector3 up /*= Vector3(1,0,0)*/ )	{
	Matrix4 r;
	r.SetPositionVector(Vector3(-from.x,-from.y,-from.z));

	Matrix4 m;

	Vector3 f = (lookingAt - from);
	f.Normalise();

	Vector3 s = Vector3::Cross(f,up);
	Vector3 u = Vector3::Cross(s,f);

	s.Normalise();
	u.Normalise();

	m.values[0] = s.x;
	m.values[4] = s.y;
	m.values[8] = s.z;

	m.values[1] = u.x;
	m.values[5] = u.y;
	m.values[9] = u.z;

	m.values[2]  = -f.x;
	m.values[6]  = -f.y;
	m.values[10] = -f.z;

	return m*r;
}

Matrix4 Matrix4::Rotation(float degrees, const Vector3 &inaxis)	 {
	Matrix4 m;

	Vector3 axis = inaxis;

	axis.Normalise();

	float c = cos((float)DegToRad(degrees));
	float s = sin((float)DegToRad(degrees));

	m.values[0]  = (axis.x * axis.x) * (1.0f - c) + c;
	m.values[1]  = (axis.y * axis.x) * (1.0f - c) + (axis.z * s);
	m.values[2]  = (axis.z * axis.x) * (1.0f - c) - (axis.y * s);

	m.values[4]  = (axis.x * axis.y) * (1.0f - c) - (axis.z * s);
	m.values[5]  = (axis.y * axis.y) * (1.0f - c) + c;
	m.values[6]  = (axis.z * axis.y) * (1.0f - c) + (axis.x * s);

	m.values[8]  = (axis.x * axis.z) * (1.0f - c) + (axis.y * s);
	m.values[9]  = (axis.y * axis.z) * (1.0f - c) - (axis.x * s);
	m.values[10] = (axis.z * axis.z) * (1.0f - c) + c;

	return m;
}

Matrix4 Matrix4::Rotation(float degreeX, float degreeY, float degreeZ)
{
	Matrix4 m;

	float cx = cos(DegToRad(degreeX));
	float cy = cos(DegToRad(degreeY));
	float cz = cos(DegToRad(degreeZ));

	float sx = sin(DegToRad(degreeX));
	float sy = sin(DegToRad(degreeY));
	float sz = sin(DegToRad(degreeZ));

	// cy * cz + sx * sy * sz		sx * sy * cz - cy * sz		sy * cx	
	// cx * sz						cx * cz						-sx
	// cy * sx * sz - sy * cz		cz * cy * sx + sy * sz		cx * cy
	// 
	// Ry*Rx*Rz
	m.values[0] = cy * cz + sx * sy * sz;
	m.values[1] = cx * sz;
	m.values[2] = cy * sx * sz - sy * cz;

	m.values[4] = sx * sy * cz - cy * sz;
	m.values[5] = cx * cz;
	m.values[6] = cz * cy * sx + sy * sz;

	m.values[8] = sy * cx;
	m.values[9] = -sx;
	m.values[10] = cx * cy;

	return m;
}

Matrix4 Matrix4::Rotation(const Vector3& rotation)
{
	return Rotation(rotation.x, rotation.y, rotation.z);
}

Matrix4 Matrix4::Rotation(const Quaternion& rotation)
{
	Matrix4 matrix;
	float x = rotation.x;
	float y = rotation.y;
	float z = rotation.z;
	float w = rotation.w;

	float xx2 = 2 * x * x;
	float yy2 = 2 * y * y;
	float zz2 = 2 * z * z;
	float xy2 = 2 * x * y;
	float wz2 = 2 * w * z;
	float xz2 = 2 * x * z;
	float wy2 = 2 * w * y;
	float yz2 = 2 * y * z;
	float wx2 = 2 * w * x;

	matrix[0] = 1 - yy2 - zz2;
	matrix[1] = xy2 + wz2;
	matrix[2] = xz2 - wy2;
	matrix[4] = xy2 - wz2;
	matrix[5] = 1 - xx2 - zz2;
	matrix[6] = yz2 + wx2;
	matrix[8] = xz2 + wy2;
	matrix[9] = yz2 - wx2;
	matrix[10] = 1 - xx2 - yy2;

	return matrix;
}

void Matrix4::ToEuler(Vector3& rotation, float eps)
{
	float thetaX = asin(-values[9]); // -90<=degreeX<=90
	float thetaY, thetaZ;

	if (-values[9] > (1 - eps)) { //degreeX=90
		thetaY = atan2(values[4], values[6]);
		thetaZ = 0;
	}
	else if (-values[9] < (-1 + eps)) {
		thetaY = atan2(-values[4], -values[6]);
		thetaZ = 0;
	}
	else {
		thetaY = atan2(values[8], values[10]);
		thetaZ = atan2(values[1], values[5]);
	}
	rotation = Vector3(RadToDeg(thetaX), RadToDeg(thetaY), RadToDeg(thetaZ));
}

Matrix4 Matrix4::Scale( const Vector3 &scale )	{
	Matrix4 m;

	m.values[0]  = scale.x;
	m.values[5]  = scale.y;
	m.values[10] = scale.z;	

	return m;
}

Matrix4 Matrix4::Translation( const Vector3 &translation )	{
	Matrix4 m;

	m.values[12] = translation.x;
	m.values[13] = translation.y;
	m.values[14] = translation.z;	

	return m;
}

Matrix4 Matrix4::GetTransposedRotation() {
	Matrix4 temp;
	temp.values[0] = values[0];
	temp.values[5] = values[5];
	temp.values[10] = values[10];
	temp.values[1] = values[4];
	temp.values[4] = values[1];
	temp.values[2] = values[8];
	temp.values[8] = values[2];
	temp.values[6] = values[9];
	temp.values[9] = values[6];
	return temp;
}

Matrix4 Matrix4::InterpolateTranslation(const Matrix4& from, const Matrix4& to, float by) {
	Matrix4 interp = from;
	interp.values[12] = MyMaths::Lerp(from.values[12], to.values[12], by);
	interp.values[13] = MyMaths::Lerp(from.values[13], to.values[13], by);
	interp.values[14] = MyMaths::Lerp(from.values[14], to.values[14], by);
	return interp;
}

Matrix4 Matrix4::InterpolateRotation(const Matrix4& from, const Matrix4& to, float by)
{
	Quaternion qFrom(from);
	Quaternion qTo(to);
	/*auto a = Rotation(qTo);*/
	Quaternion qInterp = Quaternion::Lerp(qFrom, qTo, by);
	Matrix4 interp = Rotation(qInterp);
	interp.values[12] = from.values[12];
	interp.values[13] = from.values[13];
	interp.values[14] = from.values[14];
	return interp;
}

Matrix4 Matrix4::InterpolateRotationAndTranslation(const Matrix4& from, const Matrix4& to, float by)
{
	Matrix4 interp = InterpolateRotation(from, to, by);
	interp = InterpolateTranslation(interp, to, by);
	return interp;
}

//Yoinked from the Open Source Doom 3 release - all credit goes to id software!
void    Matrix4::Invert() {
	float det, invDet;

	// 2x2 sub-determinants required to calculate 4x4 determinant
	float det2_01_01 = values[0] * values[5] - values[1] * values[4];
	float det2_01_02 = values[0] * values[6] - values[2] * values[4];
	float det2_01_03 = values[0] * values[7] - values[3] * values[4];
	float det2_01_12 = values[1] * values[6] - values[2] * values[5];
	float det2_01_13 = values[1] * values[7] - values[3] * values[5];
	float det2_01_23 = values[2] * values[7] - values[3] * values[6];

	// 3x3 sub-determinants required to calculate 4x4 determinant
	float det3_201_012 = values[8] * det2_01_12 - values[9] * det2_01_02 + values[10] * det2_01_01;
	float det3_201_013 = values[8] * det2_01_13 - values[9] * det2_01_03 + values[11] * det2_01_01;
	float det3_201_023 = values[8] * det2_01_23 - values[10] * det2_01_03 + values[11] * det2_01_02;
	float det3_201_123 = values[9] * det2_01_23 - values[10] * det2_01_13 + values[11] * det2_01_12;

	det = (-det3_201_123 * values[12] + det3_201_023 * values[13] - det3_201_013 * values[14] + det3_201_012 * values[15]);

	invDet = 1.0f / det;

	// remaining 2x2 sub-determinants
	float det2_03_01 = values[0] * values[13] - values[1] * values[12];
	float det2_03_02 = values[0] * values[14] - values[2] * values[12];
	float det2_03_03 = values[0] * values[15] - values[3] * values[12];
	float det2_03_12 = values[1] * values[14] - values[2] * values[13];
	float det2_03_13 = values[1] * values[15] - values[3] * values[13];
	float det2_03_23 = values[2] * values[15] - values[3] * values[14];

	float det2_13_01 = values[4] * values[13] - values[5] * values[12];
	float det2_13_02 = values[4] * values[14] - values[6] * values[12];
	float det2_13_03 = values[4] * values[15] - values[7] * values[12];
	float det2_13_12 = values[5] * values[14] - values[6] * values[13];
	float det2_13_13 = values[5] * values[15] - values[7] * values[13];
	float det2_13_23 = values[6] * values[15] - values[7] * values[14];

	// remaining 3x3 sub-determinants
	float det3_203_012 = values[8] * det2_03_12 - values[9] * det2_03_02 + values[10] * det2_03_01;
	float det3_203_013 = values[8] * det2_03_13 - values[9] * det2_03_03 + values[11] * det2_03_01;
	float det3_203_023 = values[8] * det2_03_23 - values[10] * det2_03_03 + values[11] * det2_03_02;
	float det3_203_123 = values[9] * det2_03_23 - values[10] * det2_03_13 + values[11] * det2_03_12;

	float det3_213_012 = values[8] * det2_13_12 - values[9] * det2_13_02 + values[10] * det2_13_01;
	float det3_213_013 = values[8] * det2_13_13 - values[9] * det2_13_03 + values[11] * det2_13_01;
	float det3_213_023 = values[8] * det2_13_23 - values[10] * det2_13_03 + values[11] * det2_13_02;
	float det3_213_123 = values[9] * det2_13_23 - values[10] * det2_13_13 + values[11] * det2_13_12;

	float det3_301_012 = values[12] * det2_01_12 - values[13] * det2_01_02 + values[14] * det2_01_01;
	float det3_301_013 = values[12] * det2_01_13 - values[13] * det2_01_03 + values[15] * det2_01_01;
	float det3_301_023 = values[12] * det2_01_23 - values[14] * det2_01_03 + values[15] * det2_01_02;
	float det3_301_123 = values[13] * det2_01_23 - values[14] * det2_01_13 + values[15] * det2_01_12;

	values[0] = -det3_213_123 * invDet;
	values[4] = +det3_213_023 * invDet;
	values[8] = -det3_213_013 * invDet;
	values[12] = +det3_213_012 * invDet;

	values[1] = +det3_203_123 * invDet;
	values[5] = -det3_203_023 * invDet;
	values[9] = +det3_203_013 * invDet;
	values[13] = -det3_203_012 * invDet;

	values[2] = +det3_301_123 * invDet;
	values[6] = -det3_301_023 * invDet;
	values[10] = +det3_301_013 * invDet;
	values[14] = -det3_301_012 * invDet;

	values[3] = -det3_201_123 * invDet;
	values[7] = +det3_201_023 * invDet;
	values[11] = -det3_201_013 * invDet;
	values[15] = +det3_201_012 * invDet;
}

Matrix4 Matrix4::Inverse()	const {
	if (*this == Matrix4::identity)
		return identity;
	Matrix4 temp(*this);
	temp.Invert();
	return temp;
}