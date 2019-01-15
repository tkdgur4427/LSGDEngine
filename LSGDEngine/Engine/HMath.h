#pragma once

#include "SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace lsgd {

	// basic primitives
	using HVector2 = Vector2;
	using HVector3 = Vector3;
	using HVector4 = Vector4;
	using HMatrix = Matrix;
	using HPlane = Plane;
	using HQuaternion = Quaternion;
	using HColor = Color;
	using HRay = Ray;

	// collision
	using HBoundingSphere = BoundingSphere;
	using HBoundingBox = BoundingBox;
	using HBoundingOrientedBox = BoundingOrientedBox;
	using HBoundingFrustum = BoundingFrustum;

}