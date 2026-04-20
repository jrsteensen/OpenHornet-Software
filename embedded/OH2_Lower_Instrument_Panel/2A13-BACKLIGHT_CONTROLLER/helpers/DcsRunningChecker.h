#ifndef DCS_RUNNING_CHECKER_H
#define DCS_RUNNING_CHECKER_H

/**
 * @file    DcsRunningChecker.h
 * @brief   DCS Connection Monitor for OpenHornet 
 * @details This function monitors DCS-BIOS communication and returns TRUE as long as 
 *          DCS is running; FALSE if DCS has stopped sending data, after a timeout.  
 */

#include <DcsBios.h>


// DCS connection monitoring variables
static int                 currDcsHeartbeat = 0;
static int                 prevDcsHeartbeat = 0;
static unsigned long       dcsLastUpdateTime = 0;
static const unsigned long DCS_TIMEOUT_MS = 10000;


/**
 * @brief   Check if DCS World is currently running and sending data
 * @details Monitors DCS-BIOS heartbeat counter to determine if DCS is actively
 *          communicating. Call this function regularly in your main loop.
 * @return  true if DCS is running and sending data
 * @return  false if DCS has stopped or communication timeout occurred
 * @note    Must be called regularly (every loop cycle) to maintain accurate timing
 */
bool checkDcsRunning() {
  if (currDcsHeartbeat != prevDcsHeartbeat) {
    dcsLastUpdateTime = millis();
    prevDcsHeartbeat = currDcsHeartbeat;
    return true;
  }
  
  // Check if we've exceeded the timeout period
  if (millis() - dcsLastUpdateTime >= DCS_TIMEOUT_MS) {
    // No communication from DCS for over 1 second
    return false;
  }
  
  // Within timeout window - assume DCS is still running
  return true;
}

// DCS-BIOS callback for update counter changes
void onDcsUpdateCounterChange(unsigned int newValue) {
    currDcsHeartbeat = newValue;
  }
DcsBios::IntegerBuffer dcsUpdateCounterBuffer(0xfffe, 0x00ff, 0, onDcsUpdateCounterChange);

#endif // DCS_RUNNING_CHECKER_H