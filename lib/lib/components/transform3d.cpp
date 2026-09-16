#include "transform3d.hpp"




namespace lib
{
	// Decomposes a matrix while preserving non-uniform scale and rotation perfectly
	static void Transform3DFromMatrixPreserved(const Matrix& mat, Transform3D& result) {

		// 1. Extract Translation directly from the final column
		result.position = Vector3{ mat.m12, mat.m13, mat.m14 };

		// 2. Extract Basis Columns (The X, Y, and Z spatial orientation vectors)
		Vector3 basisX = { mat.m0,  mat.m1,  mat.m2 };
		Vector3 basisY = { mat.m4,  mat.m5,  mat.m6 };
		Vector3 basisZ = { mat.m8,  mat.m9,  mat.m10 };

		// 3. Extract Scale by calculating the geometric length of each basis column
		result.size.x = Vector3Length(basisX);
		result.size.y = Vector3Length(basisY);
		result.size.z = Vector3Length(basisZ);

		// 4. Handle negative scale flipping (Sign preservation)
		// Check if the coordinate system's determinant is flipped (handedness inversion)
		float det = mat.m0 * (mat.m5 * mat.m10 - mat.m6 * mat.m9) -
			mat.m4 * (mat.m1 * mat.m10 - mat.m2 * mat.m9) +
			mat.m8 * (mat.m1 * mat.m6 - mat.m2 * mat.m5);
		if (det < 0.0f) {
			result.size.x = -result.size.x; // Invert an axis to preserve handedness
		}

		// 5. Cleanly isolate the Pure Rotation matrix by dividing out the extracted scale
		Matrix pureRotationMat = MatrixIdentity();

		if (result.size.x != 0.0f) { basisX = Vector3Scale(basisX, 1.0f / result.size.x); }
		if (result.size.y != 0.0f) { basisY = Vector3Scale(basisY, 1.0f / result.size.y); }
		if (result.size.z != 0.0f) { basisZ = Vector3Scale(basisZ, 1.0f / result.size.z); }

		pureRotationMat.m0 = basisX.x; pureRotationMat.m1 = basisX.y; pureRotationMat.m2 = basisX.z;
		pureRotationMat.m4 = basisY.x; pureRotationMat.m5 = basisY.y; pureRotationMat.m6 = basisY.z;
		pureRotationMat.m8 = basisZ.x; pureRotationMat.m9 = basisZ.y; pureRotationMat.m10 = basisZ.z;

		// 6. Convert the pure rotation matrix to a Quaternion safely
		result.orientation = QuaternionFromMatrix(pureRotationMat);
	}

	static float AngleBetweenQuaternions(Quaternion current, Quaternion target) {

		// 1. Calculate the dot product of the two quaternions
		// Note: Quaternions must be normalized (unit quaternions)
		float dot = Vector4DotProduct(current, target);

		// 2. Clamp the dot product to avoid NaN from precision errors with acos
		if (dot < -1.0f) dot = -1.0f;
		if (dot > 1.0f) dot = 1.0f;

		// 3. Calculate the angle in radians
		// The angle between two unit quaternions q1 and q2 is 2 * acos(|q1 . q2|)
		// We use fabsf(dot) to ensure we get the shortest arc (0 to PI)
		float angle = 2.0f * acosf(fabsf(dot));

		return angle; // Result in radians
	}
	Transform3D::Transform3D(Vector3 Position)
		: position(Position)
	{
	}
	Transform3D::Transform3D(Vector3 Position, Vector3 Size)
		:position(Position), size(Size)
	{
	}
	Transform3D::Transform3D(Vector3 Position, Vector3 Size, Quaternion Orientation)
		:position(Position), size(Size), orientation(Orientation)
	{
	}
	Transform3D::Transform3D(Matrix mat4x4)
	{
		Transform3DFromMatrixPreserved(mat4x4, *this);
	}
	Matrix Transform3D::toMatrix() const
	{
		return MatrixMultiply(MatrixMultiply(MatrixScale(size.x, size.y, size.z),
			QuaternionToMatrix(orientation)),
			MatrixTranslate(position.x, position.y, position.z));
	}
	BoundingBox Transform3D::toBoundingBox() const
	{
		BoundingBox aabb;
		Vector3 halfExtent = size / 2;
		aabb.min = position - halfExtent;
		aabb.max = position + halfExtent;

		return aabb;
	}
	Vector3 Transform3D::GetEuler() const
	{
		return QuaternionToEuler(orientation);
	}
	void Transform3D::SetEuler(Vector3 euler)
	{
		orientation = QuaternionFromEuler(euler.x, euler.y, euler.z);
	}
	Vector3 Transform3D::Front() const
	{
		Matrix mat = toMatrix();
		return Vector3{ mat.m0, mat.m4, mat.m8 };
	}
	Vector3 Transform3D::Right() const
	{
		Matrix mat = toMatrix();

		return Vector3{ mat.m0, mat.m1, mat.m2 };
	}
	Vector3 Transform3D::Up() const
	{
		Matrix mat = toMatrix();
		return Vector3{ mat.m4, mat.m5, mat.m6 };
	}
	Quaternion Transform3D::LookAt(Vector3 direction, Vector3 up_axis) const
	{
		direction.z *= -1.0f;
		Matrix look = MatrixLookAt(Vector3Zeros, direction, up_axis);
		Quaternion target = QuaternionFromMatrix(look);
		return target;
	}
	Quaternion Transform3D::RotateTowards(Vector3 direction, Vector3 up_axis, float radian) const
	{
		Quaternion target = LookAt(direction, up_axis);

		float angle = AngleBetweenQuaternions(orientation, target);

		if (angle < 0.0001f)
			return target;

		float maxAngle = radian;
		float t = maxAngle / angle;
		if (t > 1.0f) t = 1.0f;

		return QuaternionSlerp(orientation, target, t);
	}
	void Transform3D::RotateAroundAxis(Vector3 rotation_axis, float deg_angle)
	{
		Quaternion deltaRotation = QuaternionFromAxisAngle(rotation_axis, deg_angle * DEG2RAD);
		this->orientation = QuaternionMultiply(this->orientation, deltaRotation);
	}
}

