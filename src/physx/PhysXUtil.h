#ifndef GUI_PHYSXUTIL_H
#define GUI_PHYSXUTIL_H

#include "PxPhysicsAPI.h"
#include "glm/vec3.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/detail/type_quat.hpp"
#include "glm/gtc/quaternion.hpp"
#include <utility>

class PhysXUtil {
public:
    static physx::PxTransform transformFromPosRot(glm::vec3 pos, glm::vec3 rot = glm::vec3(0.0f)) {
      physx::PxQuat q = glmVec3RotToPhysQuat(rot);

      return {pos.x, pos.y, pos.z, q};
    }

    // Returns <pos, rot>
    static std::pair<glm::vec3, glm::quat> posAndRotFromTransform(physx::PxTransform *transform) {
      glm::vec3 pos, rot;
      physx::PxQuat q = transform->q;

//      rot = physQuatRotToGlmEuler(q);

      pos.x = transform->p.x;
      pos.y = transform->p.y;
      pos.z = transform->p.z;
      return std::make_pair(pos, physQuatToGlm(q));
    }

    static physx::PxVec3 glmVec3ToPhys(glm::vec3 v) {
      return physx::PxVec3{v.x, v.y, v.z};
    }

    static glm::vec3 physVec3ToGlm(const physx::PxVec3 &v) {
      return glm::vec3{v.x, v.y, v.z};
    }

    static glm::quat physQuatToGlm(const physx::PxQuat &q) {
      return glm::quat{q.w, q.x, q.y, q.z};
    }

    static physx::PxQuat glmQuatToPhys(const glm::quat &q) {
      return physx::PxQuat{q.x, q.y, q.z, q.w};
    }

    static glm::vec3 physQuatRotToGlmEuler(const physx::PxQuat &q) {
      glm::quat glmQ = physQuatToGlm(q);
      glm::vec3 rot = glm::degrees(glm::eulerAngles(glmQ));
//      Log::logf("Converting phys quat {%f, %f, %f, %f} to euler {%f, %f, %f}", q.x, q.y, q.z, q.w, rot.x, rot.y, rot.z);
      return rot;
    }

    static physx::PxQuat glmVec3RotToPhysQuat(glm::vec3 rotDeg) {
      glm::vec3 rot = glm::radians(rotDeg);

      const double c1 = cos(rot.x / 2);
      const double c2 = cos(rot.y / 2);
      const double c3 = cos(rot.z / 2);

      const double s1 = sin(rot.x / 2);
      const double s2 = sin(rot.y / 2);
      const double s3 = sin(rot.z / 2);

      physx::PxQuat q;

      q.x = s1 * c2 * c3 + c1 * s2 * s3;
      q.y = c1 * s2 * c3 - s1 * c2 * s3;
      q.z = c1 * c2 * s3 + s1 * s2 * c3;
      q.w = c1 * c2 * c3 - s1 * s2 * s3;

//      double cr = cos(rot.x * 0.5);
//      double sr = sin(rot.x * 0.5);
//      double cp = cos(rot.y * 0.5);
//      double sp = sin(rot.y * 0.5);
//      double cy = cos(rot.z * 0.5);
//      double sy = sin(rot.z * 0.5);
//
//      q.w = cr * cp * cy + sr * sp * sy;
//      q.x = sr * cp * cy - cr * sp * sy;
//      q.y = cr * sp * cy + sr * cp * sy;
//      q.z = cr * cp * sy - sr * sp * cy;

//      Log::logf("Converting euler {%f, %f, %f} to phys quat {%f, %f, %f, %f}", rotDeg.x, rotDeg.y, rotDeg.z, q.x, q.y, q.z, q.w);
      return q;
    }
};

#endif //GUI_PHYSXUTIL_H
