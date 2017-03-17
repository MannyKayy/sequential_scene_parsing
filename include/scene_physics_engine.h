#ifndef SCENE_PHYSICS_ENGINE_H
#define SCENE_PHYSICS_ENGINE_H

#include <iostream>
#include <math.h>
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

#include "scene_physics_penalty.h"
#include "scene_physics_support.h"

static void _worldTickCallback(btDynamicsWorld *world, btScalar timeStep);

class PhysicsEngine : public PlatformDemoApplication
{
public:
	PhysicsEngine();
	virtual ~PhysicsEngine()
    {
        exitPhysics();
    }

    GLDebugDrawer gDebugDraw;
	// use plane as background (table)
	void addBackgroundPlane(btVector3 plane_normal, btScalar plane_constant, btVector3 plane_center);
	void addBackgroundConvexHull(const std::vector<btVector3> &plane_points, btVector3 plane_normal);
	void addBackgroundMesh(btTriangleMesh* trimesh, btVector3 plane_normal, btVector3 plane_center);

	// uses a frame that has Y direction up as a guide for gravity direction
	void setGravityVectorDirectionFromTfYUp(const btTransform &transform_y_is_inverse_gravity_direction);
	void setGravityVectorDirection(const btVector3 &gravity);
	void setGravityFromBackgroundNormal(const bool &input);
	void addObjects(const std::vector<ObjectWithID> &objects);
	std::map<std::string, btTransform>  getUpdatedObjectPose();
	void resetObjects();

	void setObjectPenaltyDatabase(std::map<std::string, ObjectPenaltyParameters> * penalty_database);

	void setDebugMode(bool debug);
	void renderingLaunched();

	// Scene analysis
	void resetObjectPoseToBestDataPosition();
	SceneSupportGraph getCurrentSceneGraph() const;
	vertex_t getObjectVertexFromSupportGraph(const std::string &object_name, btTransform &object_position);


// Additional functions used for rendering:
    void initPhysics();
    void exitPhysics();
    void worldTickCallback(const btScalar &timeStep);

    virtual void clientMoveAndDisplay();

    virtual void displayCallback();
    virtual void clientResetScene();

    virtual void setCameraClippingPlaneNearFar(btScalar near, btScalar far = 10000.f);
    virtual void setCameraPositionAndTarget(btVector3 cam_position, btVector3 cam_target);

    static DemoApplication* Create()
    {
        PhysicsEngine* demo = new PhysicsEngine;
        // demo->myinit();
        demo->initPhysics();
        return demo;
    }

private:
	void simulate();
	bool checkSteadyState();
	void cacheObjectVelocities(const btScalar &timeStep);
	
	bool debug_messages_;
	bool have_background_;
	bool use_background_normal_as_gravity_;
	bool rendering_launched_;
	bool in_simulation_;
	unsigned int counter_;
	// rigid body data from ObjectWithID input with ID information
	std::map<std::string, btRigidBody*> rigid_body_;
	std::map<std::string, btTransform> object_best_pose_from_data_;
	btRigidBody* background_;
	btVector3 background_surface_normal_;

	// physics engine environment parameters
	btBroadphaseInterface* m_broadphase;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btSequentialImpulseConstraintSolver* m_solver;
	// DO NOT DECLARE m_dynamicworld here. It will break OPENGL simulation
    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
	
	std::map<std::string, ObjectPenaltyParameters> object_penalty_parameter_database_by_id_;
    std::map<std::string, ObjectPenaltyParameters> * object_penalty_parameter_database_;
    double gravity_magnitude_;
    btVector3 gravity_vector_;
    
    std::map<std::string, MovementComponent> object_velocity_;
    std::map<std::string, MovementComponent> object_acceleration_;
    SceneSupportGraph scene_graph_;
    std::map<std::string, vertex_t> vertex_map_;

	btVector3 camera_coordinate_, target_coordinate_;
	double simulation_step_;
	boost::mutex mtx_;
};

#endif
