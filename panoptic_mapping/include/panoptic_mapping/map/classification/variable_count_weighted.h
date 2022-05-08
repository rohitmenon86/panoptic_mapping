#ifndef PANOPTIC_MAPPING_MAP_CLASSIFICATION_VARIABLE_COUNT_WEIGHTED_H_
#define PANOPTIC_MAPPING_MAP_CLASSIFICATION_VARIABLE_COUNT_WEIGHTED_H_

#include <memory>
#include <unordered_map>
#include <vector>

#include "panoptic_mapping/3rd_party/config_utilities.hpp"
#include "panoptic_mapping/Submap.pb.h"
#include "panoptic_mapping/map/classification/class_layer_impl.h"
#include "panoptic_mapping/map/classification/class_voxel.h"

namespace panoptic_mapping {

/**
 * @brief Keep track of arbitrary number of IDs in an unordered map. ID 0 is
 * generally used to store the belonging submap and shifting other IDs by 1.
 */
struct VariableCountWeightedVoxel : public ClassVoxel {
 public:
  // Implement interfaces.
  ClassVoxelType getVoxelType() const override;
  bool isObserverd() const override;
  bool belongsToSubmap() const override;
  float getBelongingProbability() const override;
  int getBelongingID() const override;
  float getProbability(const int id) const override;
  void incrementCount(const int id, const float weight = 1.f) override;
  bool mergeVoxel(const ClassVoxel& other) override;
  std::vector<uint32_t> serializeVoxelToInt() const override;
  bool deseriliazeVoxelFromInt(const std::vector<uint32_t>& data,
                               size_t* data_index) override;
  // Data.
  std::unordered_map<int, FloatingPoint> weights;
  int current_index = 0;
  FloatingPoint current_max_weight = 0;
  FloatingPoint weight_sum = 0;
};

class VariableCountWeightedLayer
    : public ClassLayerImpl<VariableCountWeightedVoxel> {
 public:
  struct Config : public config_utilities::Config<Config> {
    Config() { setConfigName("VariableCountWeightedLayer"); }

   protected:
    void fromRosParam() override {}
    void printFields() const override {}
  };

  VariableCountWeightedLayer(const Config& config, const float voxel_size,
                             const int voxels_per_side);

  ClassVoxelType getVoxelType() const override;
  std::unique_ptr<ClassLayer> clone() const override;
  static std::unique_ptr<ClassLayer> loadFromStream(
      const SubmapProto& submap_proto, std::istream* /* proto_file_ptr */,
      uint64_t* /* tmp_byte_offset_ptr */);

 protected:
  const Config config_;
  static config_utilities::Factory::RegistrationRos<
      ClassLayer, VariableCountWeightedLayer, float, int>
      registration_;
};

}  // namespace panoptic_mapping

#endif  // PANOPTIC_MAPPING_MAP_CLASSIFICATION_VARIABLE_COUNT_WEIGHTED_H_
