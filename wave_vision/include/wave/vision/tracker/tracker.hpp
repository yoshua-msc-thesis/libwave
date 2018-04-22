/**
 * @file
 * Feature tracker implementation.
 * @ingroup vision
 */
#ifndef WAVE_VISION_TRACKER_HPP
#define WAVE_VISION_TRACKER_HPP

#include <chrono>
#include <string>
#include <vector>

#include "wave/containers/landmark_measurement.hpp"
#include "wave/containers/landmark_measurement_container.hpp"
#include "wave/utils/utils.hpp"
#include "wave/vision/utils.hpp"

namespace wave {
/** @addtogroup vision
 *  @{ */

using FeatureTrack = std::vector<LandmarkMeasurement<Vec2, int>>;

/** Image tracker class.
 *
 * The Tracker class is templated on a feature detector, descriptor, and matcher
 * to track features over a sequence of images.
 *
 * @tparam TDetector The feature detector object (FAST, ORB, etc...).
 * @tparam TDescriptor The feature descriptor object (BRISK, ORB, etc...).
 * @tparam TMatcher The descriptor matcher object (BruteForceMatcher, FLANN).
 */
template <typename TDetector, typename TDescriptor, typename TMatcher>
class Tracker {
 public:
    /** Default constructor.
     *
     * @param detector The feature detector object (FAST, ORB, etc...).
     * @param descriptor The feature descriptor object (BRISK, ORB, etc...).
     * @param matcher The descriptor matcher object (BruteForceMatcher, FLANN).
     */
    Tracker(TDetector detector,
            TDescriptor descriptor,
            TMatcher matcher,
            uint64_t window_size = 0)
        : detector(detector),
          descriptor(descriptor),
          matcher(matcher),
          window_size(window_size) {
        if (this->window_size < 0) {
            throw std::invalid_argument("window_size cannot be negative!");
        }
    }

    ~Tracker() = default;

    /** Get the tracks of all features in the requested image from the sequence.
     *
     * @param stamp The timestamp at which to obtain tracks.
     * @return Tracks corresponding to all detected landmarks in the image, from
     * the start of time to the given image.
     */
    std::vector<FeatureTrack> getTracks(const TimePoint &stamp) const;

    /** Add a new image to the tracker (presumably the next in a sequence).
     *
     * @param image The image to add.
     * @param stamp The time at which the image was captured.
     */
    void addImage(const cv::Mat &image, const TimePoint &stamp);

    /** Draw tracks for the requested image.
     *
     * @param feature_tracks The vector of FeatureTracks present in the image.
     * @param image The image to draw the tracks on.
     * @return The image with the tracks illustrated as arrows.
     */
    cv::Mat drawTracks(const std::vector<FeatureTrack> &feature_tracks,
                       const cv::Mat &image) const;

    /** Offline feature tracking, using list of images already loaded.
     *
     * @param image_sequence The sequence of images to analyze.
     * @return The vector of FeatureTracks in each image.
     */
    std::vector<std::vector<FeatureTrack>> offlineTracker(
      const std::vector<cv::Mat> &image_sequence);

 public:
    /** The templated FeatureDetector */
    TDetector detector;

    /** The templated DescriptorExtractor */
    TDescriptor descriptor;

    /** The templated DescriptorMatcher */
    TMatcher matcher;

    /** The size of the LandmarkMeasurementContainer */
    size_t lmc_size = 0;

 private:
    /** Generate a new ID for each newly detected feature.
     *
     * @return The assigned ID.
     */
    uint64_t generateFeatureID() const {
        static uint64_t id = 0;
        return id++;
    }

    /** Detect features and compute descriptors.
     *
     * Detects features and computes descriptors using the templated detector
     * and descriptor.
     *
     * @param image The image to detect features in.
     * @param keypoints The keypoints detected by the feature detector.
     * @param descriptor The descriptors for keypoints detected in the image.
     */
    void detectAndCompute(const cv::Mat &image,
                          std::vector<cv::KeyPoint> &keypoints,
                          cv::Mat &descriptor);

    /** Cleans out the LandmarkMeasurementContainer for images outside the
     *  requested window_size.
     */
    void maintainContainers();

    /** Registers the latest matched keypoints with IDs. Assigns a new ID if one
     * has not already been provided.
     *
     * @param curr_kp the keypoints detected in the current image.
     * @param matches the matches between the current and previous images.
     * @return the map corresponding current keypoints to IDs.
     */
    std::map<int, LandmarkId> registerKeypoints(
      const std::vector<cv::KeyPoint> &curr_kp,
      const std::vector<cv::DMatch> &matches);

 private:
    /** For online, sliding window tracker operation. Maintains memory by
     *  clearing out values from the measurement container that are outside of
     *  this time window.
     *
     *  If set to zero (default), all measurements are kept for offline use.
     */
    size_t window_size;

    /// Keypoints from the previous timestep.
    std::vector<cv::KeyPoint> prev_kp;

    /// Descriptors from the previous timestep.
    cv::Mat prev_desc;

    /// Correspondence map between keypoints and landmark IDs in the prev image.
    std::map<int, LandmarkId> prev_ids;

    /// Times at which each image occurred.
    std::vector<TimePoint> image_stamps;

    /// Measurement container.
    LandmarkMeasurementContainer<LandmarkMeasurement<Vec2, int>> landmarks;

    /// The sensor ID
    int sensor_id = 0;
};

/** @} group vision */
}  // namespace wave

#include "impl/tracker.hpp"

#endif  // WAVE_VISION_TRACKER_HPP
