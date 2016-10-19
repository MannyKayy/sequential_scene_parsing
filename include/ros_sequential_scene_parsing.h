#ifndef ROS_SEQUENTIAL_SCENE_PARSING_H
#define ROS_SEQUENTIAL_SCENE_PARSING_H

// ROS stuffs
#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseArray.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>
#include <pcl_conversions/pcl_conversions.h>


// use detected object list instead of tf name convention
#include <costar_objrec_msgs/DetectedObject.h>
#include <costar_objrec_msgs/DetectedObjectList.h>

#include "sequential_scene_parsing.h"

// Ros bundling for scene parsing
class RosSceneGraph : public SceneGraph
{
public:
	RosSceneGraph();
	RosSceneGraph(const ros::NodeHandle &nh);
	void setNodeHandle(const ros::NodeHandle &nh);
	void addBackground(const sensor_msgs::PointCloud2 &pc);
	void updateSceneFromDetectedObjectMsgs(const costar_objrec_msgs::DetectedObjectList &detected_objects);
	void publishTf() const;

	void setDebugMode(bool debug);
private:
	// void initialize();
	bool debug_messages_;
	bool class_ready_;
	bool physics_gravity_direction_set_;
	ros::Subscriber detected_object_sub;
	ros::Subscriber background_pcl_sub;
	
	ros::NodeHandle nh_;
	tf::TransformListener listener_;
	SceneGraph ros_scene_;
	PhysicsEngine physics_engine_;
	std::map<std::string, ObjectParameter> object_transforms_;
	std::string parent_frame_;
	std::string tf_y_is_inverse_gravity_direction_;
	std::string tf_publisher_initial;
	ObjectDatabase obj_database_;
};


#endif