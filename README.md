# M5 Paper Stock Market Tracker
Based on https://github.com/m5stack/M5Paper_FactoryTest/tree/main


To recreate this, create a `src/secrets.h` file with the following paramters.

```c
#ifndef SECRETS_H
#define SECRETS_H

#define WIFI_SSID "wifi_ssid"
#define WIFI_PASSWORD "wifipassword"
#define POLYGON_API_KEY "polygonapikey"

#endif //SECRETS_H
```

Note that the free tier of polygon only gives access to at most 5 API calls per minute, and the timeframe given is only end-of-day data.

To add the NTPClient Libary
```bash
cd lib
git clone https://github.com/taranais/NTPClient.git
```

# Future Tasks
- Integrate wake every day to check the latest prices
- Integrate battery life remaining
- Possibly integrate touch to select range (though only 5 calls per minute are allowed)
- Allow different WIFI SSIDs for connection
- Allow different stocks instead of the current hard coded ones