#pragma once

#include <memory>

#include "drake/automotive/gen/maliput_railcar_config.h"
#include "drake/automotive/gen/maliput_railcar_state.h"
#include "drake/automotive/maliput/api/lane.h"
#include "drake/common/drake_copyable.h"
#include "drake/systems/framework/leaf_system.h"
#include "drake/systems/framework/sparsity_matrix.h"
#include "drake/systems/rendering/pose_vector.h"

namespace drake {
namespace automotive {

/// MaliputRailcar -- models a vehicle that follows a maliput::api::Lane as if
/// it were on rails and neglecting all physics. It can only move forward at a
/// predetermined speed.
///
/// Configuration:
///   * See MaliputRailcarConfig.
///
/// State vector:
///   * See MaliputRailcarState.
///
/// <B>Input Port Accessors:</B>
///
///   - command_input(): Contains the desired acceleration. This port
///     contains a systems::BasicVector of size 1. It is optional in that it
///     need not be connected. When it is not connected, the railcar will travel
///     at its initial velocity.
///
/// <B>Output Port Accessors:</B>
///
///   - state_output(): Contains this system's state vector. See
///     MaliputRailcarState.
///
///   - pose_output(): Contains PoseVector `X_WC`, where `C` is the car frame
///     and `W` is the world frame.
///
/// @tparam T must support certain arithmetic operations;
/// for details, see drake::symbolic::Expression.
///
/// Instantiated templates for the following ScalarTypes are provided:
/// - double
///
/// They are already available to link against in the containing library.
///
/// @ingroup automotive_systems
template <typename T>
class MaliputRailcar : public systems::LeafSystem<T> {
 public:
  DRAKE_NO_COPY_NO_MOVE_NO_ASSIGN(MaliputRailcar)

  /// The constructor.
  ///
  /// @param lane The lane on which this MaliputRailcar travels. The lifetime
  /// of this parameter must exceed that of this class's instance.
  ///
  /// @param start_time The time at which this vehicle starts moving.
  ///
  explicit MaliputRailcar(const maliput::api::Lane& lane,
                          double start_time = 0);

  // System<T> overrides.
  void DoCalcOutput(const systems::Context<T>& context,
                    systems::SystemOutput<T>* output) const override;
  void DoCalcTimeDerivatives(
      const systems::Context<T>& context,
      systems::ContinuousState<T>* derivatives) const override;

  // LeafSystem<T> overrides.
  void SetDefaultParameters(const systems::LeafContext<T>& context,
                            systems::Parameters<T>* params) const override;

  /// Sets `config` to contain the default parameters for MaliputRailcar.
  static void SetDefaultParameters(MaliputRailcarConfig<T>* config);

  void SetDefaultState(const systems::Context<T>& context,
                       systems::State<T>* state) const override;

  /// Sets `railcar_state` to contain the default state for MaliputRailcar.
  static void SetDefaultState(MaliputRailcarState<T>* railcar_state);

  /// Getter methods for input and output port descriptors.
  /// @{
  const systems::InputPortDescriptor<T>& command_input() const;
  const systems::OutputPortDescriptor<T>& state_output() const;
  const systems::OutputPortDescriptor<T>& pose_output() const;
  /// @}

  static constexpr T kDefaultR = T(0);
  static constexpr T kDefaultH = T(0);
  static constexpr T kDefaultInitialS = T(0);
  static constexpr T kDefaultInitialSpeed = T(1);
  static constexpr T kDefaultMaxSpeed = T(45);
  static constexpr T kDefaultVelocityLimitKp = T(10);

 protected:
  // LeafSystem<T> overrides.
  std::unique_ptr<systems::ContinuousState<T>> AllocateContinuousState()
      const override;
  std::unique_ptr<systems::BasicVector<T>> AllocateOutputVector(
      const systems::OutputPortDescriptor<T>& descriptor) const override;
  std::unique_ptr<systems::Parameters<T>> AllocateParameters() const override;
  bool DoHasDirectFeedthrough(const systems::SparsityMatrix* sparsity,
                              int input_port, int output_port) const override;

 private:
  void ImplCalcOutput(
      const MaliputRailcarState<T>& state,
      MaliputRailcarState<T>* output) const;

  void ImplCalcPose(
      const MaliputRailcarConfig<T>& config,
      const MaliputRailcarState<T>& state,
      systems::rendering::PoseVector<T>* pose) const;

  void ImplCalcTimeDerivatives(
      const MaliputRailcarConfig<T>& config,
      const MaliputRailcarState<T>& state,
      const systems::BasicVector<T>& input,
      MaliputRailcarState<T>* rates) const;

  void ImplCalcTimeDerivativesDouble(
    const MaliputRailcarConfig<double>& config,
    const MaliputRailcarState<double>& state,
    MaliputRailcarState<double>* rates) const;

  const maliput::api::Lane& lane_;
  const double start_time_{};
  int command_input_port_index_{};
  int state_output_port_index_{};
  int pose_output_port_index_{};
};

}  // namespace automotive
}  // namespace drake
