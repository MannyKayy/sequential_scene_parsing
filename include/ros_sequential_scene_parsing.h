#ifndef ROS_SEQUENTIAL_SCENE_PARSING_H
#define ROS_SEQUENTIAL_SCENE_PARSING_H

#include <boost/algorithm/string.hpp>

// ROS stuffs
#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseArray.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/io/pcd_io.h>

#include <std_msgs/Empty.h>

// use detected object list instead of tf name convention
#include <costar_objrec_msgs/ObjectSymmetry.h>
#include <costar_objrec_msgs/DetectedObject.h>
#include <costar_objrec_msgs/DetectedObjectList.h>

// object hypothesis associated with the detectedObjectList
#include <objrec_hypothesis_msgs/AllModelHypothesis.h>
#include <objrec_hypothesis_msgs/ModelHypothesis.h>
#include <objrec_hypothesis_msgs/Hypothesis.h>

#include "sequential_scene_parsing.h"
#include "symmetric_orientation_realignment.h"

#include "sequential_scene_parsing/SceneNodes.h"
#include "sequential_scene_parsing/StructureGraph.h"
#include "sequential_scene_parsing/SceneGraph.h"


// Ros bundling for scene parsing
class RosSceneHypothesisAssessor : public SceneHypothesisAssessor
{
public:
	RosSceneHypothesisAssessor();
	RosSceneHypothesisAssessor(const ros::NodeHandle &nh);
	void setNodeHandle(const ros::NodeHandle &nh);
	
	void publishTf();

	void setDebugMode(bool debug);
	void callGlutMain(int argc, char* argv[]);
	void exitGlutMain();

	PhysicsEngine physics_engine_;
	
private:
	bool fillObjectPropertyDatabase();
	void addBackgroundCallback(const sensor_msgs::PointCloud2 &pc);
	void addSceneCloud(const sensor_msgs::PointCloud2 &pc);
	void updateSceneFromDetectedObjectMsgs(const costar_objrec_msgs::DetectedObjectList &detected_objects);
	void fillObjectHypotheses(const objrec_hypothesis_msgs::AllModelHypothesis &detected_object_hypotheses);
	void processDetectedObjectMsgs();
	void processHypotheses();
	void updateTfFromObjTransformMap(const std::map<std::string, ObjectParameter> &input_tf_map, const bool &publish_object_list = true);

	sequential_scene_parsing::SceneGraph generateSceneGraphMsgs() const;
	// void initialize();
	// bool debug_messages_;
	bool class_ready_;
	bool physics_gravity_direction_set_;
	bool background_normal_as_gravity_;
	bool has_tf_;
	bool has_background_;
	bool has_scene_cloud_;

	bool scene_cloud_updated_;
	bool object_list_received_;
	bool object_list_updated_;
	bool hypothesis_list_received_;

	costar_objrec_msgs::DetectedObjectList detected_objects_;
	objrec_hypothesis_msgs::AllModelHypothesis detected_object_hypotheses_;
	std::map<std::string,costar_objrec_msgs::DetectedObject> object_msg_property_map_;

	// Background mode: 0 = BACKGROUND_PLANE, 1 = BACKGROUND_HULL, 2 = BACKGROUND_MESH
	int background_mode_;
	ros::Subscriber detected_object_sub;
	ros::Subscriber object_hypotheses_sub;
	ros::Subscriber background_pcl_sub;
	ros::Subscriber scene_pcl_sub;
	ros::Publisher done_message_pub;
	ros::Publisher scene_graph_pub;
	ros::Publisher scene_objects_pub;
	
	ros::NodeHandle nh_;
	tf::TransformListener listener_;
	tf::TransformBroadcaster tf_broadcaster_;
	// SceneHypothesisAssessor ros_scene_;
	
	// PhysicsEngine physics_engine_;
	std::map<std::string, tf::Transform> object_transforms_tf_;
	std::string parent_frame_;
	std::string tf_z_is_inverse_gravity_direction_;
	std::string tf_publisher_initial;
	std::map<std::string, PhysicalProperties> physical_properties_database_;
	std::size_t number_of_object_list_published_;

	boost::mutex mtx_;
};


#endif
