/**
 * @file    apkeypad.h
 * @brief   DCS-BIOS virtual switch class for buttons read through a PCA9555 I/O expander.
 *
 * @details Provides @c virtSwitch2PosT, a DCS-BIOS PollingInput that reads its
 *          pin state from a PCA9555 GPIO expander over I2C (via the clsPCA9555
 *          library) rather than from a native Arduino pin.
 *
 *          A single global PCA9555 instance (@c functionSelect at I2C address 0x20)
 *          is shared by all @c virtSwitch2PosT objects in the translation unit.
 *          Pins are configured as INPUT_PULLUP in the constructor.
 *
 *          Software debouncing is applied: the button state must be stable for
 *          @p debounceDelay milliseconds before a DCS-BIOS message is transmitted.
 *
 * @note    DcsBios.h must be included before this header (same requirement as
 *          MatPotentiometers.h — PollingInput and ResettableInput come from there).
 */

#include <math.h>
#include "Arduino.h"
#include "clsPCA9555.h"

/**
 * @brief  Shared PCA9555 I/O expander instance at I2C address 0x20.
 *
 * @details All @c virtSwitch2PosT objects in this translation unit read their
 *          pin states through this single device.  The I2C bus must be
 *          initialised (Wire.begin) before any @c virtSwitch2PosT is polled.
 */
PCA9555 functionSelect(0x20);

namespace DcsBios {

/**
 * @brief  DCS-BIOS 2-position switch read from a PCA9555 GPIO expander pin.
 *
 * @details Polls a single pin on the shared @c functionSelect PCA9555 device,
 *          applies software debouncing, and sends a DCS-BIOS "0"/"1" message
 *          when the stable state changes.  Active-low logic (INPUT_PULLUP) is
 *          the default; set @p reverse to true for active-high switches.
 *
 * @tparam pollIntervalMs  Minimum milliseconds between polls.  Defaults to
 *                         POLL_EVERY_TIME (every DCS-BIOS loop tick).
 */
template <unsigned long pollIntervalMs = POLL_EVERY_TIME>
class virtSwitch2PosT : PollingInput, public ResettableInput {
  private:
    /** @brief DCS-BIOS control identifier sent with each state change. */
    const char* msg_;

    /** @brief PCA9555 pin number (0-15) this switch is wired to. */
    char pin_;

    /**
     * @brief  Latched logical state used by the debounce algorithm.
     *
     * Updated only when the raw pin state has been stable for at least
     * @c debounceDelay_ milliseconds, preventing glitches from sending
     * spurious DCS-BIOS messages.
     */
    char debounceSteadyState_;

    /** @brief Last state successfully transmitted to DCS-BIOS. */
    char lastState_;

    /**
     * @brief  When true, the logical pin state is inverted before comparison.
     *
     * Set to true for active-high switches; leave false (default) for
     * active-low (INPUT_PULLUP) switches.
     */
    bool reverse_;

    /** @brief Minimum stable period (ms) required before a state change is accepted. */
    unsigned long debounceDelay_;

    /** @brief Timestamp (millis) of the last observed raw state change. */
    unsigned long lastDebounceTime = 0;

    /**
     * @brief  Invalidate lastState_ to force a retransmit on next poll.
     *
     * Called by the DCS-BIOS framework when the simulator reconnects so
     * that the current button state is re-sent unconditionally.
     */
    void resetState() {
      lastState_ = (lastState_ == 0) ? -1 : 0;
    }

    /**
     * @brief  Read pin state, debounce, and send DCS-BIOS message on change.
     *
     * @details Reads the raw digital state of @c pin_ from the PCA9555 expander.
     *          If @c reverse_ is true the state is inverted.  When the raw state
     *          differs from @c debounceSteadyState_, the debounce timer is reset.
     *          Once the state has remained stable for @c debounceDelay_ ms and
     *          differs from @c lastState_, a DCS-BIOS "0" (HIGH) or "1" (LOW)
     *          message is sent.  @c lastState_ is updated only if the message
     *          was accepted by the DCS-BIOS send queue.
     */
    void pollInput() {
      char state = functionSelect.digitalRead(pin_);
      if (reverse_) state = !state;

      unsigned long now = millis();

      if (state != debounceSteadyState_) {
        lastDebounceTime  = now;
        debounceSteadyState_ = state;
      }

      if ((now - lastDebounceTime) >= debounceDelay_) {
        if (debounceSteadyState_ != lastState_) {
          if (tryToSendDcsBiosMessage(msg_, state == HIGH ? "0" : "1")) {
            lastState_ = debounceSteadyState_;
          }
        }
      }
    }

  public:
    /**
     * @brief  Construct a virtSwitch2PosT and initialise the PCA9555 pin.
     *
     * @details Configures @p pin as INPUT_PULLUP on the shared PCA9555 expander,
     *          reads the current pin state, and initialises @c lastState_ so that
     *          the first poll only transmits if the button is already pressed.
     *
     * @param msg           DCS-BIOS control identifier (e.g. "UFC_AP").
     * @param pin           PCA9555 pin number (0-15) the button is wired to.
     * @param reverse       If true, invert the pin logic for active-high buttons.
     *                      Default false (active-low / INPUT_PULLUP).
     * @param debounceDelay Minimum stable period in milliseconds before a state
     *                      change is accepted and transmitted.  Default 50 ms.
     */
    virtSwitch2PosT(const char* msg, char pin,
                    bool reverse = false, unsigned long debounceDelay = 50)
      : PollingInput(pollIntervalMs)
    {
      msg_          = msg;
      pin_          = pin;
      debounceDelay_ = debounceDelay;
      reverse_      = reverse;

      functionSelect.pinMode(pin_, INPUT_PULLUP);
      lastState_ = functionSelect.digitalRead(pin_);
      if (reverse_) lastState_ = !lastState_;
    }

    /**
     * @brief  Reassign the DCS-BIOS control name at runtime.
     *
     * @param msg  New DCS-BIOS control identifier string.
     */
    void SetControl(const char* msg) {
      msg_ = msg;
    }

    /**
     * @brief  Public wrapper for resetState() invoked by the DCS-BIOS framework.
     *
     * Forces retransmission of the current switch state after a reconnect.
     */
    void resetThisState() {
      this->resetState();
    }
};

/**
 * @brief  Convenience typedef with default poll interval.
 *
 * Use @c DcsBios::virtSwitch2Pos in application code unless a custom poll
 * interval is needed.
 */
typedef virtSwitch2PosT<> virtSwitch2Pos;

} // namespace DcsBios
