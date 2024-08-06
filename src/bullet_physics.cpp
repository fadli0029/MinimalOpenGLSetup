#include "bullet_physics/bullet_physics.hpp"

BulletPhysics::BulletPhysics() { InitPhysics(); }

BulletPhysics::~BulletPhysics() { ShutdownPhysics(); }

void BulletPhysics::InitPhysics() {
    b3CollisionConfiguration_ = new btDefaultCollisionConfiguration();
    b3Dispatcher_ = new btCollisionDispatcher(b3CollisionConfiguration_);
    b3Broadphase_ = new btDbvtBroadphase();
    b3Solver_ = new btSequentialImpulseConstraintSolver();
    b3World_ = new btDiscreteDynamicsWorld(
        b3Dispatcher_, b3Broadphase_, b3Solver_, b3CollisionConfiguration_);

    CreatePhysicsObjects();
}

void BulletPhysics::ShutdownPhysics() {
    delete b3World_;
    delete b3Solver_;
    delete b3Broadphase_;
    delete b3Dispatcher_;
    delete b3CollisionConfiguration_;
}

void BulletPhysics::CreatePhysicsObjects() {
    // ======================================================================================
    // Cube physics
    // =======================================================================================
    // Create a box shape of size (1,1,1), defined by half-extents
    btBoxShape *boxShape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));

    // Set box initial pose
    btTransform cubeTransform;
    cubeTransform.setIdentity();                           // init. orientation
    cubeTransform.setOrigin(btVector3(0.0f, 15.0f, 0.0f)); // init. position

    // Create a motion state
    b3CubeMotionState_ = new btDefaultMotionState(cubeTransform);

    // Create the rigid body construction info object, giving it a
    // mass of 1, the motion state, and the shape
    float cubeMass = 1.0f;
    btRigidBody::btRigidBodyConstructionInfo cubeRbInfo(
        cubeMass, b3CubeMotionState_, boxShape);
    btRigidBody *b3CubeRigidBody = new btRigidBody(cubeRbInfo);

    // Inform our world that we just created a new rigid body for it to manage
    b3World_->addRigidBody(b3CubeRigidBody);

    // ======================================================================================
    // Plane physics
    // ======================================================================================
    // Create a static plane shape
    btStaticPlaneShape *planeShape =
        new btStaticPlaneShape(btVector3(0, 1, 0), 0);

    // Set plane initial pose
    btTransform planeTransform;
    planeTransform.setIdentity(); // init. orientation

    // Create a motion state for the plane
    b3PlaneMotionState_ = new btDefaultMotionState(planeTransform);

    // Create the rigid body construction info object for the plane with zero
    // mass
    float planeMass = 0.0f;
    btRigidBody::btRigidBodyConstructionInfo planeRbInfo(
        planeMass, b3PlaneMotionState_, planeShape);
    btRigidBody *b3PlaneRigidBody = new btRigidBody(planeRbInfo);

    // Inform our world that we just created a new rigid body for it to manage
    b3World_->addRigidBody(b3PlaneRigidBody);
}

void BulletPhysics::StepSimulation(const float &deltaTime,
                                   const float &maxSubSteps) {
    b3World_->stepSimulation(deltaTime, maxSubSteps);
}

void BulletPhysics::GetWorldTransform(float (&matrix)[16]) {
    if (b3CubeMotionState_) {
        btTransform trans;
        b3CubeMotionState_->getWorldTransform(trans);
        trans.getOpenGLMatrix(matrix);
    }
}
