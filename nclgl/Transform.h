#pragma once
#include <vector>
#include <algorithm>

#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"

#include "Component.h"
#include "BoundingGeometry.h"
#include "Utility.h"
#include "Mesh.h"

using namespace std;

class Transform: public Component
{
public:
	Transform(){
		position = Vector3::zero;
		scale = Vector3::one;
		mS = Matrix4::Scale(scale);
		mR = Matrix4::Rotation(rotationQuaternion);
		mT = Matrix4::Translation(position);

		boundingAABB.SetType(BoundingGeometryType_None);
		boundingAABBMesh.SetType(BoundingGeometryType_None);

		UpdateModelMatrix();
		
		SetParent(&root);
	}
	Transform(Transform* parent) {
		position = Vector3::zero;
		scale = Vector3::one;
		mS = Matrix4::Scale(scale);
		mR = Matrix4::Rotation(rotationQuaternion);
		mT = Matrix4::Translation(position);

		boundingAABB.SetType(BoundingGeometryType_None);
		boundingAABBMesh.SetType(BoundingGeometryType_None);

		UpdateModelMatrix();

		SetParent(parent);
	}
	Transform(const Vector3& position,
		const Vector3& rotationEuler,
		const Vector3& scale) {
		this->position = position;
		this->rotationQuaternion = Quaternion::FromEulerAngles(rotationEuler);
		this->scale = scale;
		mS = Matrix4::Scale(scale);
		mR = Matrix4::Rotation(rotationQuaternion);
		mT = Matrix4::Translation(position);

		boundingAABB.SetType(BoundingGeometryType_None);
		boundingAABBMesh.SetType(BoundingGeometryType_None);

		UpdateModelMatrix();

		SetParent(&root);
	}
	Transform(const Vector3& position,
		const Quaternion& rotation,
		const Vector3& scale) {
		this->position = position;
		this->rotationQuaternion = rotation;
		this->scale = scale;
		mS = Matrix4::Scale(scale);
		mR = Matrix4::Rotation(rotationQuaternion);
		mT = Matrix4::Translation(position);

		boundingAABB.SetType(BoundingGeometryType_None);
		boundingAABBMesh.SetType(BoundingGeometryType_None);

		UpdateModelMatrix();

		SetParent(&root);
	}
	Transform(const Transform& other) = default;
	inline Transform& operator=(const Transform& other) = default;

	virtual ~Transform() = default;


	//
	//SRT
	//
	inline void SetPositionLocal(const Vector3& position) {
		this->position = position;
		UpdateTranslationMatrix();
	}
	inline void SetPositionLocal(float posX, float posY, float posZ) {
		SetPositionLocal(Vector3(posX, posY, posZ));
	}
	inline void SetPositionWorld(const Vector3& position) {
		Matrix4 inverse;
		if (this->parent) {
			auto a = this->parent->GetModelMatrix();
			inverse = this->parent->GetModelMatrixInverse();
		}
		this->position = (inverse * Vector4(position, 1)).ToVector3();
		UpdateTranslationMatrix();
	}
	inline void SetPositionWorld(float posX, float posY, float posZ) {
		SetPositionWorld(Vector3(posX, posY, posZ));
	}
	inline Vector3 GetPositionLocal() const {
		return position;
	}
	inline Vector3 GetPositionWorld() const {
		Matrix4 parentModel;
		if (this->parent)
			parentModel = parent->GetModelMatrix();
		return (parentModel * Vector4(position,1)).ToVector3();
	}

	inline void SetRotationLocal(const Quaternion& rotation) {
		this->rotationQuaternion = rotation;
		UpdateRotationMatrix();
	}
	inline void SetRotationLocal(const Vector3& rotationEuler) {
		SetRotationLocal(Quaternion::FromEulerAngles(rotationEuler));
	}
	inline void SetRotationLocal(float rotEulerX, float rotEulerY, float rotEulerZ) {
		SetRotationLocal(Vector3(rotEulerX, rotEulerY, rotEulerZ));
	}
	inline void SetRotationWorld(const Quaternion& rotation) {
		Transform* transParent = parent;
		vector<Transform*> transList;
		while (transParent) {
			transList.emplace_back(transParent);
			transParent = transParent->parent;
		}
		Quaternion rotationLocal = rotation;
		for (int i = transList.size() - 1; i >= 0; --i) {
			rotationLocal = transList[i]->rotationQuaternion.Conjugate() * rotationLocal;
		}
		
		SetRotationLocal(rotationLocal);
	}
	inline void SetRotationWorld(const Vector3& rotationEuler) {
		SetRotationWorld(Quaternion::FromEulerAngles(rotationEuler));
	}
	inline void SetRotationWorld(float rotEulerX, float rotEulerY, float rotEulerZ) {
		SetRotationWorld(Vector3(rotEulerX, rotEulerY, rotEulerZ));
	}
	inline Quaternion GetRotationLocal() const {
		return rotationQuaternion;
	}
	inline Vector3 GetRotationEulerLocal() const {
		return rotationQuaternion.ToEuler();
	}
	inline Quaternion GetRotationWorld() const {
		Quaternion quaternionRet = rotationQuaternion;
		Transform* transParent = parent;
		while (transParent) {
			quaternionRet = transParent->rotationQuaternion * quaternionRet;
			transParent = transParent->parent;
		}
		return quaternionRet;
	}
	inline Vector3 GetRotationEulerWorld() const {
		return GetRotationWorld().ToEuler();
	}

	inline void SetScaleLocal(const Vector3& scale) {
		this->scale = scale;
		UpdateScaleMatrix();
	}
	inline void SetScaleLocal(float scaleX, float scaleY, float scaleZ) {
		SetScaleLocal(Vector3(scaleX, scaleY, scaleZ));
	}
	inline Vector3 GetScaleLocal() const {
		return scale;
	}

	inline void Translate(const Vector3& t) {
		position += t;
		UpdateTranslationMatrix();
	}

	inline void Translate(const float& x, const float& y, const float& z) {
		Translate(Vector3(x, y, z));
	}

	inline void Rotate(const Quaternion& rotation) {
		rotationQuaternion = rotation * rotationQuaternion;
		UpdateRotationMatrix();
	}
	inline void Rotate(const Vector3& rotationEuler) {
		Quaternion quatRot = Quaternion::FromEulerAngles(rotationEuler);
		rotationQuaternion = quatRot * rotationQuaternion;
		UpdateRotationMatrix();
	}
	inline void Rotate(const float& x, const float& y, const float& z) {
		Rotate(Vector3(x, y, z));
	}

	inline void RotateAround(const Quaternion& rotation, Vector3 rotationCenter) {
		Vector4 pos = Matrix4::Translation(rotationCenter - position) *
			(Matrix4::Rotation(rotation) *
			(Matrix4::Translation(position - rotationCenter) * Vector4(0, 0, 0, 1)));

		SetPositionWorld(pos.ToVector3() + position);
		Rotate(rotation);
	}
	inline void RotateAround(const Vector3& rotationEuler, Vector3 rotationCenter) {
		Vector4 pos = Matrix4::Translation(rotationCenter - position) *
			(Matrix4::Rotation(rotationEuler) *
			(Matrix4::Translation(position - rotationCenter) * Vector4(0, 0, 0, 1)));

		SetPositionWorld(pos.ToVector3()+position);
		Rotate(rotationEuler);
	}
	inline void RotateAround(const float& x, const float& y, const float& z, Vector3 rotationCenter) {
		RotateAround(Vector3(x, y, z), rotationCenter);
	}

	inline void Scale(const Vector3& s) {
		scale += s;
		UpdateScaleMatrix();
	}

	inline void Scale(const float& x, const float& y, const float& z) {
		Scale(Vector3(x, y, z));
	}

	inline Matrix4 GetModelMatrixLocal() const {
		return modelMatrix;
	}

	Matrix4 GetModelMatrix() const {
		Matrix4 matReturn = modelMatrix;
		Transform* transParent = parent;
		while (transParent) {
			matReturn = transParent->modelMatrix* matReturn;
			transParent = transParent->parent;
		}
		return matReturn;
	}

	Matrix4 GetManipulatedModelMatrix(Matrix4 mTranslate, Matrix4 mRotation, Matrix4 mScale) const {
		Matrix4 matReturn =  mTranslate*(mT*(mRotation*(mR*(mScale*mS))));
		Transform* transParent = parent;
		while (transParent) {
			matReturn = transParent->modelMatrix * matReturn;
			transParent = transParent->parent;
		}
		return matReturn;
	}

	Matrix4 GetModelMatrixInverse() const {
		return GetModelMatrix().Inverse();
	}

	void SetForward(Vector3 forward);
	inline Vector3 GetForward() const { return (mR * Vector4(0.0f, 0.0f, -1.0f, 0.0f)).ToVector3(); }
	inline Vector3 GetRight() const { return (mR * Vector4(1.0f, 0.0f, 0.0f, 0.0f)).ToVector3(); }
	inline Vector3 GetUp() const { return (mR * Vector4(0.0f, 1.0f, 0.0f, 0.0f)).ToVector3(); }

	void SetReflectionForMirror(Transform* transformRef, Vector3 normalPlane, Vector3 posPlane);

	//
	//Hierarchy management
	//
	static Transform root;

	inline bool AddChild(Transform* transformChild) {
		if (!transformChild) {
			return false;
		}
		transformChild->SetParent(this);
		return true;
	}

	bool ClearChildren() {
		for (int i = children.size() - 1; i >= 0; --i) {
			children.pop_back();
		}
		UpdateBoundingAABB();
	}

	inline Transform* GetChild(int idx) const {
		return children[idx];
	}
	inline void GetChildren(vector<Transform*>& childrenOut) const {
		childrenOut = children;
	}

	inline bool SetParent(Transform* parent, bool toKeepWorldPosRot=false) { //Can only keep world position and rotation if required. Local scale won't change.
		if (parent == this) {
			return false;
		}

		if (toKeepWorldPosRot) {
			Vector3 posW = GetPositionWorld();
			Quaternion rotW = GetRotationWorld();

			if (this->parent) {
				this->parent->RemoveChild(this);
			}
			if (parent) {
				this->parent = parent; //Set parent
			}
			else
			{
				if (this != &root)
					this->parent = &root; //If null, set root as parent
				else
					return false;
			}
			this->parent->children.emplace_back(this);

			SetPositionWorld(posW);
			SetRotationWorld(rotW);

			if (boundingAABB.GetType() != BoundingGeometryType_None)
				this->parent->UpdateBoundingAABB();
			return true;
		}
		else {
			if (this->parent) {
				this->parent->RemoveChild(this);
			}
			if (parent) {
				this->parent = parent; //Set parent
			}
			else
			{
				if (this != &root)
					this->parent = &root; //If null, set root as parent
				else
					return false;
			}
			this->parent->children.emplace_back(this);
			if (boundingAABB.GetType() != BoundingGeometryType_None)
				this->parent->UpdateBoundingAABB();
			return true;
		}
	}

	inline Transform* GetParent() const {
		return parent;
	}

	inline bool RemoveChild(Transform* child) {
		auto itr = find(children.begin(), children.end(), child);
		if (itr == children.end()) {
			return false;
		}
		itr = children.erase(itr);
		child->parent = nullptr;
		UpdateBoundingAABB();
		return true;
	}

	//
	//Bounding Geometry
	//
	inline void SetBoundingAABBMesh(shared_ptr<Mesh> mesh) { //TODO: need to accelerate this class someday (in case of encountering big mesh)
		if (mesh) {
			boundingAABBMesh.SetType(BoundingGeometryType_AABB);
			boundingAABBMesh.UpdateBoundingGeometryByMesh(mesh);
		}
		else {
			boundingAABBMesh.SetType(BoundingGeometryType_None);
		}
		UpdateBoundingAABB();
	}
	inline void SetBoundingAABB(const BoundingGeometry& boundingAABB) {
		if (boundingAABB.GetType() != BoundingGeometryType_AABB) {
			throw NotImplementedException();
			return;
		}
		else
		{
			boundingAABBMesh.SetAABB(boundingAABB.GetAABB());
		}
		UpdateBoundingAABB();
	}
	inline BoundingGeometry GetBoundingAABB() const { return boundingAABB; }

protected:
	//
	//SRT
	//
	Vector3 position; 
	Quaternion rotationQuaternion; //X:,Y, Z: right hand rotation with (0,0,-1) as rot(0,0,0);
	Vector3 scale;

	Matrix4 mS;
	Matrix4 mR;
	Matrix4 mT;

	Matrix4 modelMatrix;

	inline void UpdateModelMatrix() {
		modelMatrix = (mT * (mR * mS));
		UpdateBoundingAABB(); //TODO: Very inefficient! Figure out another way! (Maybe collect all changes in this frame and update in one traversal)
	}

	inline void UpdateScaleMatrix() {
		mS = Matrix4::Scale(scale);
		UpdateModelMatrix();
	}

	inline void UpdateRotationMatrix() {
		//mR = Matrix4::Rotation(rotation);
		mR = Matrix4::Rotation(rotationQuaternion);
		UpdateModelMatrix();
	}

	inline void UpdateTranslationMatrix() {
		mT = Matrix4::Translation(position);
		UpdateModelMatrix();
	}

	//
	//Hierarchy management
	//
	Transform* parent = nullptr;
	vector<Transform*> children;

	//
	//Bounding Geometry
	//
	BoundingGeometry boundingAABBMesh; //AABB of current mesh //TODO: Probably should put it back into MeshRenderer
	BoundingGeometry boundingAABB; //AABB of node

	void UpdateBoundingAABB() {
		Box aabb;
		if (boundingAABBMesh.GetType()==BoundingGeometryType_AABB) {
			aabb = boundingAABBMesh.GetAABB();
		}
		for (Transform* transChild : children) {
			if(transChild->boundingAABB.GetType()==BoundingGeometryType_AABB)
				aabb = aabb | Box::ModifyByModelMatrix(transChild->boundingAABB.GetAABB(), transChild->GetModelMatrixLocal());
		}
		if (aabb.IsAllZero()) {
			boundingAABB.SetType(BoundingGeometryType_None);
		}
		else
		{
			boundingAABB.SetType(BoundingGeometryType_AABB);
			boundingAABB.SetAABB(aabb);
		}
		if (parent != NULL) {
			parent->UpdateBoundingAABB();
		}
	}
};

