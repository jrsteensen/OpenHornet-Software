/**
 * @file    MatPotentiometers.h
 * @brief   DCS-BIOS potentiometer input class driven by an externally supplied 16-bit value.
 *
 * @details Extends the standard DCS-BIOS PollingInput infrastructure to accept a
 *          pointer to a @c volatile @c uint16_t that is maintained externally
 *          (e.g. written by an ADS1115 interrupt handler) rather than calling
 *          @c analogRead() on a local pin.
 *
 *          The class applies an Exponentially Weighted Moving Average (EWMA) filter
 *          to smooth rapid fluctuations, then compares the filtered result against
 *          the last transmitted value using a configurable hysteresis band.  A
 *          DCS-BIOS message is sent only when the change exceeds the hysteresis
 *          threshold (or the value crosses the absolute endpoints of the 16-bit
 *          range), preventing unnecessary USB traffic for small jitter on the ADC.
 *
 *          The input range of the external value is mapped to the full 0-65535
 *          DCS-BIOS range before filtering.  Reverse mode flips the mapping so
 *          that a physically reversed potentiometer produces the correct
 *          in-simulator response.
 *
 * @note    DcsBios.h **must** be included before this header.  The class
 *          relies on PollingInput, ResettableInput, POLL_EVERY_TIME, and
 *          tryToSendDcsBiosMessage which are all defined inside DcsBios.h.
 *          Including this header before DcsBios.h will produce compile errors.
 *
 * @note    The include guard was deliberately renamed from __DCSBIOS_POTS_H
 *          (used by the upstream DCS-BIOS Potentiometers.h) to avoid the
 *          preprocessor silently skipping this file when both are included.
 */

// Guard renamed from __DCSBIOS_POTS_H to avoid collision with the identical guard in
// dcs-bios-arduino-library/src/internal/Potentiometers.h — when DcsBios.h is included first
// it defines __DCSBIOS_POTS_H, causing this file to be silently skipped by the preprocessor.
#ifndef OH_MAT_POTENTIOMETERS_H
#define OH_MAT_POTENTIOMETERS_H

#include <math.h>
#include "Arduino.h"
// PollingInput.h is an internal DCS-BIOS header not on the project include path; its types
// (PollingInput, ResettableInput, POLL_EVERY_TIME, tryToSendDcsBiosMessage) are already
// available because DcsBios.h must be included before this file.

namespace DcsBios {

/**
 * @brief  DCS-BIOS potentiometer driven by an external 16-bit value.
 *
 * @details Reads *value_ on each poll, applies EWMA smoothing, maps the result
 *          into the 0-65535 DCS-BIOS range, and sends a message when the change
 *          exceeds the hysteresis band.  Intended for use with off-chip ADCs
 *          (e.g. ADS1115) whose results are written to the pointed-to variable
 *          from an interrupt handler or a dedicated sampling loop.
 *
 * @tparam pollIntervalMs  Minimum milliseconds between polls. Defaults to
 *                         POLL_EVERY_TIME (poll on every DCS-BIOS loop tick).
 * @tparam hysteresis      Dead-band around the last transmitted value (0-65535
 *                         scale).  Changes smaller than this are ignored.
 *                         Default 128 ≈ 0.2% of full scale.
 * @tparam ewma_divisor    EWMA smoothing factor.  Larger values produce more
 *                         smoothing but slower response.  Default 5.
 */
template <unsigned long pollIntervalMs = POLL_EVERY_TIME,
          unsigned int  hysteresis     = 128,
          unsigned int  ewma_divisor   = 5>
class matPotentiometer : PollingInput, public ResettableInput {
  private:
    /**
     * @brief  Force the next poll to transmit by invalidating lastState_.
     *
     * @details Flips lastState_ to a value that differs from any real reading
     *          so the hysteresis check fails and a message is sent immediately
     *          on the next poll.  Used by the DCS-BIOS reset mechanism when
     *          the simulator reconnects.
     */
    void resetState() {
      lastState_ = (lastState_ == 0) ? -1 : 0;
    }

    /**
     * @brief  Poll the external value, apply EWMA, and send if threshold exceeded.
     *
     * @details Called automatically by the PollingInput base class at the
     *          configured interval.  Maps *value_ from [input_min_, input_max_]
     *          to [0, 65535] (or reversed), applies the EWMA filter, then
     *          compares the filtered integer against lastState_.  If the delta
     *          exceeds @p hysteresis, or the value has crossed the 0/65535
     *          endpoints, tryToSendDcsBiosMessage() is called with the new value
     *          formatted as a decimal ASCII string.  lastState_ is updated only
     *          if the message was accepted by the send queue.
     */
    void pollInput() {
      unsigned int state;
      if (reverse_)
        state = map(*value_, input_min_, input_max_, 65535, 0);
      else
        state = map(*value_, input_min_, input_max_, 0, 65535);

      accumulator += ((float)state - accumulator) / (float)ewma_divisor;
      state = (unsigned int)accumulator;

      if (((lastState_ > state) && (lastState_ - state > hysteresis))
      || ((state > lastState_) && (state - lastState_ > hysteresis))
      || ((state > (65535 - hysteresis)) && (state > lastState_))
      || ((state < hysteresis) && (state < lastState_))
      ) {
        char buf[6];
        utoa(state, buf, 10);
        if (tryToSendDcsBiosMessage(msg_, buf))
          lastState_ = state;
      }
    }

    /** @brief DCS-BIOS control name string sent with every update message. */
    const char*           msg_;

    /**
     * @brief  Pointer to the DCS-BIOS address register (kept for API
     *         compatibility with other MatAction classes; not used internally).
     */
    volatile unsigned char* address_;

    /** @brief Pointer to the externally maintained 16-bit ADC result. */
    volatile uint16_t*    value_;

    /** @brief Last value successfully transmitted to DCS-BIOS (0-65535 scale). */
    unsigned int          lastState_;

    /**
     * @brief  EWMA accumulator maintained between polls.
     *
     * Stored as float to accumulate fractional contributions from small
     * per-sample steps.  Cast to unsigned int before threshold comparison.
     */
    float                 accumulator;

    /**
     * @brief  When true, the mapping from raw value to DCS-BIOS range is
     *         inverted so that a physically reversed potentiometer produces
     *         the expected in-simulator direction.
     */
    bool                  reverse_;

    /** @brief Lower bound of the raw input value range (maps to 0 or 65535). */
    unsigned int          input_min_;

    /** @brief Upper bound of the raw input value range (maps to 65535 or 0). */
    unsigned int          input_max_;

  public:
    /**
     * @brief  Construct a matPotentiometer and seed its initial state.
     *
     * @details Stores all configuration, seeds the EWMA accumulator with the
     *          current value of *value_ to prevent a large spurious delta on
     *          the very first poll, and initialises lastState_ by applying the
     *          same mapping used in pollInput().
     *
     * @param msg        DCS-BIOS control identifier string (e.g. "UFC_COMM1_VOL").
     * @param argAddress Pointer to a DCS-BIOS address register.  Pass nullptr
     *                   when not used (this class ignores it).
     * @param value      Pointer to the volatile uint16_t holding the current
     *                   ADC reading.  Must remain valid for the lifetime of
     *                   this object.
     * @param reverse    If true, invert the 0-65535 output range.  Default false.
     * @param input_min  Minimum expected raw value from the ADC source.
     *                   Default 0.
     * @param input_max  Maximum expected raw value from the ADC source.
     *                   Default 65535 (full 16-bit range).
     */
    matPotentiometer(const char*             msg,
                     volatile unsigned char* argAddress,
                     volatile uint16_t*      value,
                     bool                    reverse   = false,
                     unsigned int            input_min = 0,
                     unsigned int            input_max = 65535)
      : PollingInput(pollIntervalMs)
    {
      msg_       = msg;
      address_   = argAddress;
      value_     = value;
      reverse_   = reverse;
      input_min_ = input_min;
      input_max_ = input_max;

      // Seed the EWMA accumulator and lastState from the current value so the
      // first poll does not produce a large spurious delta.
      accumulator = (float)*value_;
      if (reverse_)
        lastState_ = map(*value_, input_min_, input_max_, 65535, 0);
      else
        lastState_ = map(*value_, input_min_, input_max_, 0, 65535);
    }

    /**
     * @brief  Reassign the DCS-BIOS control name at runtime.
     *
     * @details Useful when the same physical potentiometer must control
     *          different simulator axes depending on a mode switch.
     *
     * @param msg  New DCS-BIOS control identifier string.
     */
    void SetControl(const char* msg) {
      msg_ = msg;
    }

    /**
     * @brief  Public wrapper for resetState() called by the DCS-BIOS framework.
     *
     * @details Forces the next pollInput() call to retransmit the current value
     *          unconditionally, used when DCS reconnects after a disconnect.
     */
    void resetThisState() {
      this->resetState();
    }
};

/**
 * @brief  Convenience typedef for matPotentiometer with default template parameters.
 *
 * Use @c DcsBios::MatPotentiometer in application code unless custom poll
 * intervals or hysteresis values are required.
 */
typedef matPotentiometer<> MatPotentiometer;

} // namespace DcsBios

#endif // OH_MAT_POTENTIOMETERS_H
