#ifndef SCENE_PHYSICS_ENGINE_W_RENDERING_H
#define SCENE_PHYSICS_ENGINE_W_RENDERING_H

#include <vector>
#include <map>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// Rendering platform
#ifdef _WINDOWS
#include "debugdrawer/Win32DemoApplication.h"
#define PlatformDemoApplication Win32DemoApplication
#else
#include "debugdrawer/GlutDemoApplication.h"
#define PlatformDemoApplication GlutDemoApplication
#endif

#include "debugdrawer/GlutStuff.h"
#include "debugdrawer/GLDebugDrawer.h"

#include "object_data_property.h"

class PhysicsEngineWRender : public PlatformDemoApplication
{
public:
	PhysicsEngineWRender();
	virtual ~PhysicsEngineWRender()
    {
        exitPhysics();
    }

    GLDebugDrawer gDebugDraw;
	// use plane as background (table)
	void addBackgroundPlane(btVector3 plane_normal, btScalar plane_constant, btVector3 plane_center);
	void addBackgroundConvexHull(const std::vector<btVector3> &plane_points);
	// TODO: use mesh as background (table)
	void addBackgroundMesh();

	// uses a frame that has Y direction up as a guide for gravity direction
	void setGravityVectorDirectionFromTfYUp(const btTransform &transform_y_is_inverse_gravity_direction);
	void setGravityVectorDirection(const btVector3 &gravity);
	void addObjects(const std::vector<ObjectWithID> &objects);
	std::map<std::string, btTransform>  getUpdatedObjectPose();
	void resetObjects();

	void setDebugMode(bool debug);
	void renderingLaunched();

// Additional functions used for rendering:
    void initPhysics();
    void exitPhysics();

    virtual void clientMoveAndDisplay();

    virtual void displayCallback();
    virtual void clientResetScene();

    virtual void setCameraClippingPlaneNearFar(btScalar near, btScalar far = 10000.f);
    virtual void setCameraPositionAndTarget(btVector3 cam_position, btVector3 cam_target);

    static DemoApplication* Create()
    {
        PhysicsEngineWRender* demo = new PhysicsEngineWRender;
        demo->myinit();
        demo->initPhysics();
        return demo;
    }

private:
	void simulate();

	bool debug_messages_;
	bool have_background_;
	bool rendering_launched_;
	bool in_simulation_;
	unsigned int counter_;
	// rigid body data from ObjectWithID input with ID information
	std::map<std::string, btRigidBody*> rigid_body_;
	btRigidBody* background_;

	// physics engine environment parameters
	btBroadphaseInterface* m_broadphase;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btSequentialImpulseConstraintSolver* m_solver;
	// DO NOT DECLARE m_dynamicworld here. It will break OPENGL simulation
    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
	
	btVector3 camera_coordinate_, target_coordinate_;
	boost::mutex mtx_;
};

#endif