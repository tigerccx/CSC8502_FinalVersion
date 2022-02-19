#pragma once
#include <memory>

#include "Component.h"
#include "GameObject.h"

#include "Texture.h"
#include "RenderTargetTexture.h"
#include "Camera.h"
#include "Texture.h"
#include "Material.h"
#include "MeshRenderer.h"

using namespace std;

class MirrorCameraController :
    public Component
{
public:
    MirrorCameraController() = default;
    virtual ~MirrorCameraController() = default;

    Transform* transRefCam;
    Transform* transMirrorCam;
    Camera* mirrorCam;
    shared_ptr<Material> matMirror;
    MeshRenderer* rendererSelf;
    

    virtual void Update(float dt) override;

    virtual void PrePerCameraDraw(Camera* camera) override;
};

