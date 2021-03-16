﻿#pragma once

#include "Header.h"
#include "Interaction.h"
#include "Material.h"
#include "../math/animatedtransform.h"

RENDERING_BEGIN

class Primitive {
public:
    virtual ~Primitive() {

    }
    virtual AABB3f worldBound() const = 0;
    virtual bool intersect(const Ray& r, SurfaceInteraction*) const = 0;
    virtual bool intersectP(const Ray& r) const = 0;
    virtual const AreaLight* getAreaLight() const = 0;
    virtual const Material* getMaterial() const = 0;
    virtual void computeScatteringFunctions(SurfaceInteraction* isect,
        MemoryArena& arena,
        TransportMode mode,
        bool allowMultipleLobes) const = 0;
};

// 几何片元，有形状，材质，是否发光等属性，是需要渲染的具体物件
class GeometricPrimitive : public Primitive {
public:
    GeometricPrimitive(const std::shared_ptr<Shape>& shape,
        const std::shared_ptr<Material>& material,
        const std::shared_ptr<AreaLight>& areaLight,
        const MediumInterface& mediumInterface);

    virtual AABB3f worldBound() const;
    virtual bool intersect(const Ray& r, SurfaceInteraction* isect) const;
    virtual bool intersectP(const Ray& r) const;
    virtual const AreaLight* getAreaLight() const;
    virtual const Material* getMaterial() const;
    virtual void computeScatteringFunctions(SurfaceInteraction* isect,
        MemoryArena& arena, TransportMode mode,
        bool allowMultipleLobes) const;

private:
    std::shared_ptr<Shape> _shape;
    std::shared_ptr<Material> _material;
    std::shared_ptr<AreaLight> _areaLight;
    MediumInterface _mediumInterface;
};

RENDERING_END