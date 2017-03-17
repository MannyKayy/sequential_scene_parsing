#include "scene_physics_penalty.h"
#include <iostream>

inline double stabilityPenaltyFormula(const double &a_t, const double &a_r, const double &w_t, const double &w_r)
{
	// use logistic regression
	return (1 - 1/exp(-12*(a_t*w_t - 0.35))) * (1 - 1/exp(-12*(a_r*w_r - 0.35)));
}

double calculateStabilityPenalty(const MovementComponent &acceleration,
	const ObjectPenaltyParameters &penalty_params, const double &gravity_magnitude)
{
	double translation_penalty = acceleration.linear_.norm() / gravity_magnitude,
		angular_penalty = acceleration.angular_.norm() / penalty_params.maximum_angular_acceleration_;
	std::cerr << "lin penalty: " << translation_penalty << ", ang penalty: " << angular_penalty << std::endl;
	return stabilityPenaltyFormula(translation_penalty, angular_penalty, 
		penalty_params.translational_acceleration_weight_, penalty_params.angular_acceleration_weight_);
}

btScalar getObjectMaximumGravityTorqueLength(const btCollisionShape &object_shape)
{
	// Calculate the maximum acceleration that can happen for a particular object under gravity
	std::vector<btVector3> object_corner_point_list;
	object_corner_point_list.reserve(150);

	// get all points in the compound object
	if (object_shape.isCompound())
	{
		const btCompoundShape * object_compound_shape = (btCompoundShape *)&object_shape;

		for (int i = 0; i < object_compound_shape->getNumChildShapes(); i++)
		{
			const btCollisionShape * child_shape = object_compound_shape->getChildShape(i);
			const btTransform child_transform = object_compound_shape->getChildTransform(i);
			if (child_shape->isConvex())
			{
				const btConvexHullShape * child_hull_shape = (btConvexHullShape *)child_shape;
				for (int pts = 0; pts < child_hull_shape->getNumPoints(); pts++)
				{
					btVector3 corner_points = child_hull_shape->getScaledPoint(pts);
					object_corner_point_list.push_back(child_transform * corner_points);
				}
			}
		}
	}

	// get the farthest distance from the object center of gravity
	// const btVector3 cog = object.getCenterOfMassPosition();
	const btVector3 cog(0.,0.,0.);
	btScalar max_dist_squared = 0;
	for (std::vector<btVector3>::iterator it = object_corner_point_list.begin(); it!= object_corner_point_list.end(); ++it)
	{
		btScalar point_distance_to_cog =  cog.distance2(*it);
		max_dist_squared = (point_distance_to_cog > max_dist_squared) ? point_distance_to_cog : max_dist_squared;
	}

	return sqrt(max_dist_squared);
}

btScalar getObjectMaximumAngularAcceleration(const btCollisionShape &object_shape, const btScalar &mass, const btVector3 &inertia)
{
	btScalar max_gravity_torque_length = getObjectMaximumGravityTorqueLength(object_shape);
	btScalar scaled_gravity_magnitude = GRAVITY_MAGNITUDE * SCALING;
	// Torque = I * alpha = (mg) * length => alpha = mgl / I
	// maximum gravity angular acceleration happened when object is supported on the point with highest distance from cog
	// and object orientation makes this distance vector perpendicular to the gravity direction

	btVector3 inv_maximum_gravity_angular_acceleration = inertia /(scaled_gravity_magnitude * max_gravity_torque_length * mass);
	// std::cerr << "inertia: " << inertia.x() << ", " << inertia.y() << ", " << inertia.z() << ": " << scaled_gravity_magnitude * max_gravity_torque_length * mass << std::endl;
	return ( 1/inv_maximum_gravity_angular_acceleration.norm() );
}

btScalar getObjectSupportContribution(const scene_support_vertex_properties &support_graph_vertex)
{
	const double &support_value =  support_graph_vertex.support_contributions_;
	return (1 / 1 + exp(-0.5 * support_value));
}

btScalar getObjectCollisionPenalty(const scene_support_vertex_properties &support_graph_vertex)
{
	// TODO: Find a good parameter for the penetration depth
	return 1.;
}

