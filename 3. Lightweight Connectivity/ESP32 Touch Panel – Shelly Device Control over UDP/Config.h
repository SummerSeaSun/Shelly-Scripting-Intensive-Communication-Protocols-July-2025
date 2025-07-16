#define WIFI_SSID "Shelly_WiFi"
#define WIFI_PWD "Shelly123456"

#define TOUCH_SDA  33
#define TOUCH_SCL  32
#define TOUCH_INT 21
#define TOUCH_RST 25
#define TOUCH_WIDTH  320
#define TOUCH_HEIGHT 480

/*Set to your screen resolution and rotation*/
#define TFT_HOR_RES   320
#define TFT_VER_RES   480
#define TFT_ROTATION  LV_DISPLAY_ROTATION_0

//lights configuration
#define NUM_LIGHTS     3
#define LIGHTS_START_X 30
#define LIGHTS_Y       150
#define LIGHTS_WIDTH   80
#define LIGHTS_HEIGHT  96
#define LIGHTS_SPACE   10
#define LIGHTS_LABEL_Y 130

//temperature arc configuration
#define TEMP_ARC_MIN -20
#define TEMP_ARC_MAX 40
#define ARC_TOP_Y LIGHTS_Y + LIGHTS_HEIGHT + 30
#define ARC_TOP_X 10

//humidity reading configuration
#define HUM_TOP_X ARC_TOP_X + 160 + 20
#define HUM_TOP_Y ARC_TOP_Y + 57
