
/**
 * @file MamaDuck.ino
 * @author
 * @brief This example creates a MamaDuck that sends a counter message periodically. The Mama will also relay any messages it receives from
 * other ducks.
 *
 * It's using a pre-built MamaDuck available from the ClusterDuck SDK
 */

#include <arduino-timer.h>
#include <string>
#include <CDP.h>


#ifdef SERIAL_PORT_USBVIRTUAL
#define Serial SERIAL_PORT_USBVIRTUAL
#endif

std::vector<byte> stringToByteVector(const String& str);
bool sendSensorData();
bool runSensor(void *);

// create a built-in mama link
MamaDuck duck;

// create a timer with default settings
auto timer = timer_create_default();

// for sending the counter message
const int INTERVAL_MS = 60000;
int counter = 1;

bool setupOK = false;

void setup() {

  std::string deviceId("MAMA0001");
  std::vector<byte> devId;
  int rc;
  devId.insert(devId.end(), deviceId.begin(), deviceId.end());

  // Setup the duck link with default settings
  rc = duck.setupWithDefaults(devId);
  if (rc != DUCK_ERR_NONE) {
    Serial.print("[MAMA] Failed to setup ducklink: rc = ");Serial .println(rc);
    return;
  }

  setupOK = true;
  // Initialize the timer. The timer thread runs separately from the main loop
  // and will trigger sending a counter message.
  Serial.println("[MAMA] Setup OK!");

  // Send the first sensor data and start the timer
  if (sendSensorData()) {
    timer.every(INTERVAL_MS, runSensor);
  } else {
    Serial.println("[MAMA] ERROR - Failed to send sensor data");
  }
}

void loop() {
  if (!setupOK) {
    return; 
  }
  
  timer.tick();
  duck.run();
}

std::vector<byte> stringToByteVector(const String& str) {
    std::vector<byte> byteVec;
    byteVec.reserve(str.length());

    for (unsigned int i = 0; i < str.length(); ++i) {
        byteVec.push_back(static_cast<byte>(str[i]));
    }

    return byteVec;
}

bool sendSensorData() {
  bool result = false;
  const byte* buffer;
  
  String message = String("MamaCounter:") + String(counter);
  Serial.print("[MAMA] sensor data: ");
  Serial.println(message);
  
  int err = duck.sendData(topics::status, stringToByteVector(message));
  if (err == DUCK_ERR_NONE) {
     result = true;
     counter++;
  } else {
    Serial.println("[MAMA] Failed to send data. error = " + String(err));
    return false;
  }
  return result;
}

bool runSensor(void *) {
  return sendSensorData();
}