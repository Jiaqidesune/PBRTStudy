﻿#pragma once

#include "Header.h"
#include "Interaction.h"
#include "../math/transform.h"

RENDERING_BEGIN

enum class LightFlags {
    // 点光源
    DeltaPosition = 1,
    // 方向光，只有一个方向
    DeltaDirection = 2,
    // 面光源
    Area = 4,
    Infinite = 8
};

inline bool isDeltaLight(int flags) {
    return flags & (int)LightFlags::DeltaPosition ||
        flags & (int)LightFlags::DeltaDirection;
}

STAT_COUNTER("Scene/Lights", numLights);
STAT_COUNTER("Scene/AreaLights", numAreaLights);

class Light {
public:
    virtual ~Light() { }

    Light(int flags, const Transform& LightToWorld,
        const MediumInterface& mediumInterface, int nSamples = 1)
        : flags(flags),
        nSamples(std::max(1, nSamples)),
        mediumInterface(mediumInterface),
        _lightToWorld(LightToWorld),
        _worldToLight(LightToWorld.getInverse()) {
        ++numLights;
    }

    inline bool isDelta() const {
        return isDeltaLight(flags);
    }

    virtual Spectrum sample_Li(const Interaction& ref, const Point2f& u,
        Vector3f* wi, Float* pdf,
        VisibilityTester* vis) const = 0;

    virtual Spectrum power() const = 0;

    virtual void preprocess(const Scene& scene) {}

    virtual Spectrum Le(const RayDifferential& r) const { 
        return Spectrum(0.f); 
    }
    virtual Float pdf_Li(const Interaction& ref, const Vector3f& wi) const = 0;

    virtual Spectrum sample_Le(const Point2f& u1, const Point2f& u2, Float time,
        Ray* ray, Normal3f* nLight, Float* pdfPos,
        Float* pdfDir) const = 0;

    virtual void pdf_Le(const Ray& ray, const Normal3f& nLight, Float* pdfPos,
        Float* pdfDir) const = 0;

    // LightFlags
    const int flags;
    // 为了计算soft shadow的采样数量
    const int nSamples;

    const MediumInterface mediumInterface;

protected:
    const Transform _lightToWorld, _worldToLight;
};

class AreaLight : public Light {
public:
    AreaLight(const Transform& LightToWorld,
        const MediumInterface& mi,
        int nSamples)
        :Light((int)LightFlags::Area, LightToWorld, mi, nSamples) {
        ++numAreaLights;
    }

    virtual Spectrum L(const Interaction& intr, const Vector3f& w) const = 0;
};

/**
 * 可见测试器
 * 测试两个指定位置之间有没有阻挡
 */
class VisibilityTester {
public:
    VisibilityTester(const Interaction& p0, const Interaction& p1)
        : _p0(p0), _p1(p1) { }
    const Interaction& P0() const { return _p0; }
    const Interaction& P1() const { return _p1; }
    bool Unoccluded(const Scene& scene) const;
    Spectrum Tr(const Scene& scene, Sampler& sampler) const;
private:
    Interaction _p0, _p1;
};

RENDERING_END