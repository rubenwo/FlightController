// #include <Arduino.h>
// #include "logic/controller.h"
// Controller controller({12, 13, 14, 15});

// void setup()
// {
//   controller.init();
// }

// void loop()
// {
//   controller.loop();
// }

// #include <Arduino.h>
// #include <TinyGPS++.h>

// TinyGPSPlus gps;
// HardwareSerial gps_serial(2);

// static uint32_t gps_amount = 0;

// struct GpsDataState_t
// {
//   double originLat = 0;
//   double originLon = 0;
//   double originAlt = 0;
//   double distMax = 0;
//   double dist = 0;
//   double altMax = -999999;
//   double altMin = 999999;
//   double spdMax = 0;
//   double prevDist = 0;
// };
// GpsDataState_t gpsState = {};

// void setup()
// {
//   Serial.begin(460800);
//   gps_serial.begin(9600);
// }
// void loop()
// {
//   static int p0 = 0;

//   gpsState.originLat = gps.location.lat();
//   gpsState.originLon = gps.location.lng();
//   gpsState.originAlt = gps.altitude.meters();

//   gpsState.distMax = 0;
//   gpsState.altMax = -999999;
//   gpsState.spdMax = 0;
//   gpsState.altMin = 999999;

//   while (gps_serial.available() > 0)
//   {
//     gps.encode(gps_serial.read());
//   }

//   if (gps.satellites.value() > 4)
//   {
//     gpsState.dist = TinyGPSPlus::distanceBetween(gps.location.lat(), gps.location.lng(), gpsState.originLat, gpsState.originLon);

//     if (gpsState.dist > gpsState.distMax && abs(gpsState.prevDist - gpsState.dist) < 50)
//     {
//       gpsState.distMax = gpsState.dist;
//     }
//     gpsState.prevDist = gpsState.dist;

//     if (gps.altitude.meters() > gpsState.altMax)
//     {
//       gpsState.altMax = gps.altitude.meters();
//     }

//     if (gps.speed.kmph() > gpsState.spdMax)
//     {
//       gpsState.spdMax = gps.speed.kmph();
//     }

//     if (gps.altitude.meters() < gpsState.altMin)
//     {
//       gpsState.altMin = gps.altitude.meters();
//     }
//   }

//   gps.speed.knots();

//   if (gps.satellites.value() != gps_amount)
//   {
//     gps_amount = gps.satellites.value();
//     Serial.printf("Got %d satellites.\n", gps_amount);
//     if (gps.satellites.value() > 4)
//       Serial.printf("Lat: %f, Lon: %f, Alt: %f, distMax: %f, dist: %f, altMax: %f, altMin: %f, spdMax: %f, prevDist: %f\n",
//                     gpsState.originLat,
//                     gpsState.originLon,
//                     gpsState.originAlt,
//                     gpsState.distMax,
//                     gpsState.dist,
//                     gpsState.altMax,
//                     gpsState.altMin,
//                     gpsState.spdMax,
//                     gpsState.prevDist);
//   }
// }
