#ifndef GUI_PHYSXUTIL_H
#define GUI_PHYSXUTIL_H

#include "PxPhysicsAPI.h"
#include "glm/vec3.hpp"
#include "glm/ext/scalar_constants.hpp"
#include <utility>


class PhysXUtil {
public:
    static physx::PxTransform transformFromPosRot(glm::vec3 pos, glm::vec3 rot = glm::vec3(0.0f)) {
      physx::PxQuat q = glmVec3RotToPhysQuat(rot);

      return {pos.x, pos.y, pos.z, q};
    }

    // Returns <pos, rot>
    static std::pair<glm::vec3, glm::vec3> posAndRotFromTransform(physx::PxTransform *transform) {
      glm::vec3 pos, rot;
      physx::PxQuat q = transform->q;

      // roll (x-axis rotation)
      double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
      double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
      rot.x = atan2(sinr_cosp, cosr_cosp);

      // pitch (y-axis rotation)
      double sinp = 2 * (q.w * q.y - q.z * q.x);
      if (std::abs(sinp) >= 1)
        rot.y = copysign(glm::pi<float>() / 2, sinp); // use 90 degrees if out of range
      else
        rot.y = asin(sinp);

      // yaw (z-axis rotation)
      double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
      double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
      rot.z = atan2(siny_cosp, cosy_cosp);

      pos.x = transform->p.x;
      pos.y = transform->p.y;
      pos.z = transform->p.z;
      return std::make_pair(pos, rot);
    }

    static physx::PxVec3 glmVec3ToPhys(glm::vec3 v) {
      return physx::PxVec3{v.x, v.y, v.z};
    }

    static glm::vec3 physVec3ToGlm(physx::PxVec3 v) {
      return glm::vec3{v.x, v.y, v.z};
    }

    static physx::PxQuat glmVec3RotToPhysQuat(glm::vec3 rot) {
      physx::PxQuat q;

      double cr = cos(rot.x * 0.5);
      double sr = sin(rot.x * 0.5);
      double cp = cos(rot.y * 0.5);
      double sp = sin(rot.y * 0.5);
      double cy = cos(rot.z * 0.5);
      double sy = sin(rot.z * 0.5);

      q.w = cr * cp * cy + sr * sp * sy;
      q.x = sr * cp * cy - cr * sp * sy;
      q.y = cr * sp * cy + sr * cp * sy;
      q.z = cr * cp * sy - sr * sp * cy;
      return q;
    }
};

#endif //GUI_PHYSXUTIL_H
