#ifndef PANOPTIC_MAPPING_ROS_VISUALIZATION_SUBMAP_VISUALIZER_H_
#define PANOPTIC_MAPPING_ROS_VISUALIZATION_SUBMAP_VISUALIZER_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <panoptic_mapping/common/common.h>
#include <panoptic_mapping/common/globals.h>
#include <panoptic_mapping/tools/thread_safe_submap_collection.h>
#include <ros/node_handle.h>
#include <tf2_ros/transform_broadcaster.h>
#include <visualization_msgs/MarkerArray.h>
#include <voxblox/mesh/mesh_integrator.h>
#include <voxblox/utils/color_maps.h>
#include <voxblox_msgs/MultiMesh.h>
#include <voxblox_ros/mesh_vis.h>

namespace panoptic_mapping {

class SubmapVisualizer {
 public:
  // Config.
  struct Config : public config_utilities::Config<Config> {
    int verbosity = 1;
    std::string visualization_mode = "all";  // Initial visualization mode.
    std::string color_mode = "color";        // Initial color mode.
    int submap_color_discretization = 20;
    bool visualize_mesh = true;
    bool visualize_tsdf_blocks = true;
    bool visualize_free_space = true;
    bool visualize_bounding_volumes = true;
    bool include_free_space = false;
    std::string ros_namespace;

    Config() { setConfigName("SubmapVisualizer"); }

   protected:
    void setupParamsAndPrinting() override;
    void fromRosParam() override;
    void printFields() const override;
    void checkParams() const override;
  };

  // Constructors.
  SubmapVisualizer(const Config& config, std::shared_ptr<Globals> globals,
                   bool print_config = true);
  virtual ~SubmapVisualizer() = default;

  // Visualization modes.
  enum class ColorMode {
    kColor = 0,
    kNormals,
    kSubmaps,
    kInstances,
    kClasses,
    kChange,
    kClassification,
    kUncertainty,
    kEntropy,
    kPersistent,
    kScore
  };
  enum class VisualizationMode {
    kAll = 0,
    kActive,
    kActiveOnly,
    kInactive,
    kPersistent
  };

  // Visualization mode conversion.
  static ColorMode colorModeFromString(const std::string& color_mode);
  static std::string colorModeToString(ColorMode color_mode);
  static VisualizationMode visualizationModeFromString(
      const std::string& visualization_mode);
  static std::string visualizationModeToString(
      VisualizationMode visualization_mode);

  // Visualization message creation.
  virtual std::vector<voxblox_msgs::MultiMesh> generateMeshMsgs(
      ThreadSafeSubmapCollection* submaps);
  virtual visualization_msgs::MarkerArray generateBlockMsgs(
      const ThreadSafeSubmapCollection& submaps);
  virtual pcl::PointCloud<pcl::PointXYZI> generateFreeSpaceMsg(
      const ThreadSafeSubmapCollection& submaps);
  virtual visualization_msgs::MarkerArray generateBoundingVolumeMsgs(
      const ThreadSafeSubmapCollection& submaps);

  // Publish visualization requests.
  virtual void visualizeAll(ThreadSafeSubmapCollection* submaps);
  virtual void visualizeMeshes(ThreadSafeSubmapCollection* submaps);
  virtual void visualizeTsdfBlocks(const ThreadSafeSubmapCollection& submaps);
  virtual void visualizeFreeSpace(const ThreadSafeSubmapCollection& submaps);
  virtual void visualizeBoundingVolume(const ThreadSafeSubmapCollection& submaps);
  virtual void publishTfTransforms(const ThreadSafeSubmapCollection& submaps);

  // Interaction.
  virtual void reset();
  virtual void clearMesh();
  virtual void setVisualizationMode(VisualizationMode visualization_mode);
  virtual void setColorMode(ColorMode color_mode);
  virtual void setGlobalFrameName(const std::string& frame_name) {
    global_frame_name_ = frame_name;
  }

 protected:
  static const Color kUnknownColor_;

  struct SubmapVisInfo {
    // General.
    int id = 0;  // Corresponding submap id.
    std::string name_space;

    // Visualization data.
    bool republish_everything = false;
    bool was_deleted = false;
    bool change_color = true;
    Color color = kUnknownColor_;
    float alpha = 1.0;

    // Tracking.
    ChangeState previous_change_state;        // kChange
    bool was_active;                          // kActive
    voxblox::BlockIndexList previous_blocks;  // Track deleted blocks.
  };

  virtual void updateVisInfos(const ThreadSafeSubmapCollection& submaps);
  virtual void setSubmapVisColor(const Submap& submap, SubmapVisInfo* info);
  virtual void generateClassificationMesh(const Submap* submap,
                                          voxblox_msgs::Mesh* mesh);

 protected:
  // Settings.
  VisualizationMode visualization_mode_;
  ColorMode color_mode_;
  std::string global_frame_name_ = "mission";

  // Members.
  std::shared_ptr<Globals> globals_;
  tf2_ros::TransformBroadcaster tf_broadcaster_;
  voxblox::ExponentialOffsetIdColorMap id_color_map_;

  // Cached / tracked data.
  std::unordered_map<int, SubmapVisInfo> vis_infos_;
  bool vis_infos_are_updated_ = false;
  const ThreadSafeSubmapCollection* previous_submaps_ =
      nullptr;  // Only for tracking, not for use!

  // ROS.
  ros::NodeHandle nh_;
  ros::Publisher freespace_pub_;
  ros::Publisher mesh_pub_;
  ros::Publisher tsdf_blocks_pub_;
  ros::Publisher bounding_volume_pub_;

 private:
  const Config config_;
  static config_utilities::Factory::RegistrationRos<
      SubmapVisualizer, SubmapVisualizer, std::shared_ptr<Globals>>
      registration_;
};

}  // namespace panoptic_mapping

#endif  // PANOPTIC_MAPPING_ROS_VISUALIZATION_SUBMAP_VISUALIZER_H_
