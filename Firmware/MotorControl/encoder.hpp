#ifndef __ENCODER_HPP
#define __ENCODER_HPP

#ifndef __ODRIVE_MAIN_HPP
#error "This file should not be included directly. Include odrive_main.hpp instead."
#endif

struct EncoderConfig_t {
    bool use_index = false;
    bool hand_calibrated = false;
    float idx_search_speed = 10.0f; // [rad/s electrical]
    int32_t cpr = (2048 * 4); // Default resolution of CUI-AMT102 encoder,
    int32_t offset = 0;
    float calib_range = 0.02;
};

class Encoder {
public:
    enum Error_t {
        ERROR_NONE,
        ERROR_NUMERICAL,
        ERROR_CPR_OUT_OF_RANGE,
        ERROR_RESPONSE,
    };

    Encoder(const EncoderHardwareConfig_t& hw_config,
                     EncoderConfig_t& config);
    
    void setup();

    void enc_index_cb();

    void set_count(int32_t count);
    bool calib_enc_offset(float voltage_magnitude);
    bool scan_for_enc_idx(float omega, float voltage_magnitude);

    bool update(float* pos_estimate, float* vel_estimate, float* phase);
    bool run_calibration();

    const EncoderHardwareConfig_t& hw_config_;
    EncoderConfig_t& config_;
    Axis* axis_ = nullptr; // set by Axis constructor

    Error_t error_ = ERROR_NONE;
    bool index_found_ = false;
    bool is_calibrated_ = config_.hand_calibrated;
    int32_t state_ = 0;
    float phase_ = 0.0f;    // [rad]
    float pll_pos_ = 0.0f;  // [rad]
    float pll_vel_ = 0.0f;  // [rad/s]
    float pll_kp_ = 0.0f;   // [rad/s / rad]
    float pll_ki_ = 0.0f;   // [(rad/s^2) / rad]

    // Communication protocol definitions
    auto make_protocol_definitions() {
        return make_protocol_member_list(
            make_protocol_object("config",
                make_protocol_property("use_index", &config_.use_index),
                make_protocol_property("hand_calibrated", &config_.hand_calibrated),
                make_protocol_property("idx_search_speed", &config_.idx_search_speed),
                make_protocol_property("cpr", &config_.cpr),
                make_protocol_property("offset", &config_.offset),
                make_protocol_property("calib_range", &config_.calib_range)
            ),
            make_protocol_property("error", &error_),
            make_protocol_ro_property("index_found", const_cast<bool*>(&index_found_)),
            make_protocol_property("state", &state_),
            make_protocol_property("phase", &phase_),
            make_protocol_property("pll_pos", &pll_pos_),
            make_protocol_property("pll_vel", &pll_vel_),
            make_protocol_property("pll_kp", &pll_kp_),
            make_protocol_property("pll_ki", &pll_ki_)
        );
    }
};

#endif // __ENCODER_HPP
