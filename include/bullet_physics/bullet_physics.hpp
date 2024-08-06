#ifndef BULLET_PHYSICS_HPP
#define BULLET_PHYSICS_HPP

#include "btBulletDynamicsCommon.h"

class BulletPhysics {
  public:
    BulletPhysics();
    ~BulletPhysics();

    void StepSimulation(const float &deltaTime,
                        const float &maxSubSteps = 10.0f);
    void GetWorldTransform(float (&matrix)[16]);

  private:
    void InitPhysics();
    void ShutdownPhysics();
    void CreatePhysicsObjects();

    btCollisionConfiguration *b3CollisionConfiguration_;
    btCollisionDispatcher *b3Dispatcher_;
    btBroadphaseInterface *b3Broadphase_;
    btConstraintSolver *b3Solver_;
    btDynamicsWorld *b3World_;

    btDefaultMotionState *b3CubeMotionState_;
    btDefaultMotionState *b3PlaneMotionState_;
};

#endif // BULLET_PHYSICS_HPP
