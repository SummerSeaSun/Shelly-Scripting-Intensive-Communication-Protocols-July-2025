#include <WebServer.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <TAMC_GT911.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <Arduino.h>

#include "Images.h"
#include "Config.h"
#include "Wifi.h"
#include "ControlLightsUDP.h"

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

bool lights_states[NUM_LIGHTS] = {0};
lv_obj_t *lights[NUM_LIGHTS] = {0};

lv_obj_t * text_label_temp_value;
lv_obj_t * arc;
float temperature = 0;

lv_obj_t * labelHumidity;
float humidity = 0;

const String lights_labels[] = {"Living room", "Hallway", "Bedroom"};
String lights_ids[] = {"3030f9eadad8", "3030f9ec9db4", "8cbfea90eea0"};
String lights_addresses[] = {"192.168.10.127", "192.168.10.162", "192.168.10.167"};

TAMC_GT911 tp = TAMC_GT911(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, TOUCH_WIDTH, TOUCH_HEIGHT);
TFT_eSPI tft = TFT_eSPI();

ControlLightsUDP *controlLights = new ControlLightsUDP;

WebServer server(80);

/*Read the touchpad*/
void my_touchpad_read(lv_indev_t * indev, lv_indev_data_t * data)
{
  uint16_t touchX, touchY;

  tp.read();
  if (!tp.isTouched || tp.touches == 0)
  {
    //data->state = LV_INDEV_STATE_REL;
    data->state = LV_INDEV_STATE_RELEASED;
  }
  else
  {
    /*Set the coordinates*/
    data->point.x = tp.points[0].x;
    data->point.y = tp.points[0].y;
    data->state = LV_INDEV_STATE_PRESSED;
  }
}

static uint32_t my_tick(void)
{
    return millis();
}

#if LV_USE_LOG != 0
void my_print(const char * buf)
{
  Serial.printf(buf);
  Serial.flush();
}
#endif

void lv_create_lights_gui(){
  LV_IMAGE_DECLARE(light_on_80x96);
  LV_IMAGE_DECLARE(light_off_80x96);

  for (int i=0; i < NUM_LIGHTS; i++) {
    int light_x = LIGHTS_START_X + (LIGHTS_WIDTH + LIGHTS_SPACE) * i;

    lv_obj_t * labelLight = lv_label_create(lv_screen_active());
    String labelText = lights_labels[i];
    lv_label_set_text(labelLight, labelText.c_str());
    lv_obj_align(labelLight, LV_ALIGN_TOP_LEFT, light_x + (LIGHTS_WIDTH/2) - (labelText.length() * 8 / 2), LIGHTS_LABEL_Y);
    lv_obj_set_style_text_font(labelLight, &lv_font_montserrat_12, LV_STATE_DEFAULT);

    lights[i] = lv_image_create(lv_screen_active());
    int* btn_num = new int;
    *btn_num = i + 2;
    lv_obj_add_flag(lights[i], LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(lights[i], event_handler, LV_EVENT_CLICKED, btn_num);
    const lv_image_dsc_t *light = lights_states[i]?&light_on_80x96:&light_off_80x96;
    lv_image_set_src(lights[i], light);
    lv_obj_align(lights[i], LV_ALIGN_TOP_LEFT, light_x, LIGHTS_Y);
  }
}

void update_lights(){
  for (int i=0; i < NUM_LIGHTS; i++) {
    const lv_image_dsc_t *light = lights_states[i]?&light_on_80x96:&light_off_80x96;
    lv_image_set_src(lights[i], light);
  }
}

// Set the temperature value in the arc and text label
static void update_temp() {
  if (temperature <= 10.0) {
    lv_obj_set_style_text_color((lv_obj_t*) text_label_temp_value, lv_palette_main(LV_PALETTE_BLUE), 0);
  }
  else if (temperature > 10.0 && temperature <= 29.0) {
    lv_obj_set_style_text_color((lv_obj_t*) text_label_temp_value, lv_palette_main(LV_PALETTE_GREEN), 0);
  }
  else {
    lv_obj_set_style_text_color((lv_obj_t*) text_label_temp_value, lv_palette_main(LV_PALETTE_RED), 0);
  }
  const char degree_symbol[] = "\u00B0C";

  lv_arc_set_value(arc, map(int(temperature), TEMP_ARC_MIN, TEMP_ARC_MAX, 0, 100));

  String temp_text = String(temperature) + degree_symbol;
  lv_label_set_text((lv_obj_t*) text_label_temp_value, temp_text.c_str());
  Serial.print("Temperature: ");
  Serial.println(temp_text);
}

void lv_create_temp_arc_gui() {
  // Create an Arc
  arc = lv_arc_create(lv_screen_active());
  lv_obj_set_size(arc, 160, 160);
  lv_arc_set_rotation(arc, 135);
  lv_arc_set_bg_angles(arc, 0, 270);
  lv_obj_set_style_arc_color(arc, lv_color_hex(0x666666), LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(arc, lv_color_hex(0x333333), LV_PART_KNOB);
  lv_obj_align(arc, LV_ALIGN_TOP_LEFT, ARC_TOP_X, ARC_TOP_Y);

  // Create a text label in font size 32 to display the latest temperature reading
  text_label_temp_value = lv_label_create(lv_screen_active());
  lv_label_set_text(text_label_temp_value, "--.--");
  lv_obj_align(text_label_temp_value, LV_ALIGN_TOP_LEFT, ARC_TOP_X + 22, ARC_TOP_Y + 57);
  static lv_style_t style_temp;
  lv_style_init(&style_temp);
  lv_style_set_text_font(&style_temp, &lv_font_montserrat_32);
  lv_obj_add_style(text_label_temp_value, &style_temp, 0);

  update_temp();
}

static void update_humidity() {
  if (humidity <= 30.0) {
    lv_obj_set_style_text_color(labelHumidity, lv_palette_main(LV_PALETTE_BLUE), 0);
  }
  else if (humidity > 30.0 && humidity <= 70.0) {
    lv_obj_set_style_text_color(labelHumidity, lv_palette_main(LV_PALETTE_GREEN), 0);
  }
  else {
    lv_obj_set_style_text_color(labelHumidity, lv_palette_main(LV_PALETTE_RED), 0);
  }

  String hum_text = String(humidity) + "%";
  lv_label_set_text(labelHumidity, hum_text.c_str());
  Serial.print("Humidity: ");
  Serial.println(hum_text);
}

void lv_create_humidity_gui() {
  labelHumidity = lv_label_create(lv_screen_active());
  lv_obj_align(labelHumidity, LV_ALIGN_TOP_LEFT, HUM_TOP_X, HUM_TOP_Y);
  lv_obj_set_style_text_font(labelHumidity, &lv_font_montserrat_32, LV_STATE_DEFAULT);
  update_humidity();
}

void lv_create_main_screen_gui(void)
{
  static lv_style_t style;
  lv_style_init(&style);

  lv_style_set_radius(&style, 3);

  lv_style_set_bg_opa(&style, LV_OPA_100);
  lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_BLUE));
  lv_style_set_bg_grad_color(&style, lv_palette_darken(LV_PALETTE_BLUE, 2));
  lv_style_set_bg_grad_dir(&style, LV_GRAD_DIR_VER);

  lv_style_set_border_opa(&style, LV_OPA_40);
  lv_style_set_border_width(&style, 2);
  lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_GREY));

  lv_style_set_shadow_width(&style, 8);
  lv_style_set_shadow_color(&style, lv_palette_main(LV_PALETTE_GREY));
  lv_style_set_shadow_ofs_y(&style, 8);

  lv_style_set_outline_opa(&style, LV_OPA_COVER);
  lv_style_set_outline_color(&style, lv_palette_main(LV_PALETTE_BLUE));

  lv_style_set_text_color(&style, lv_color_white());
  lv_style_set_pad_all(&style, 10);

  //Init the pressed style
  static lv_style_t style_pr_2;
  lv_style_init(&style_pr_2);

  //Add a large outline when pressed
  lv_style_set_outline_width(&style_pr_2, 30);
  lv_style_set_outline_opa(&style_pr_2, LV_OPA_TRANSP);

  lv_style_set_translate_y(&style_pr_2, 5);
  lv_style_set_shadow_ofs_y(&style_pr_2, 3);
  lv_style_set_bg_color(&style_pr_2, lv_palette_darken(LV_PALETTE_BLUE, 2));
  lv_style_set_bg_grad_color(&style_pr_2, lv_palette_darken(LV_PALETTE_BLUE, 4));

  lv_obj_t * btnLightsOn = lv_btn_create(lv_scr_act());
  int* btn_num = new int;
  *btn_num = 0;
  lv_obj_add_event_cb(btnLightsOn, event_handler, LV_EVENT_CLICKED, btn_num);
  lv_obj_remove_style_all(btnLightsOn);                          /*Remove the style coming from the theme*/
  lv_obj_add_style(btnLightsOn, &style, 0);
  lv_obj_add_style(btnLightsOn, &style_pr_2, LV_STATE_PRESSED);
  lv_obj_set_size(btnLightsOn, 145, 100);
  lv_obj_align(btnLightsOn, LV_ALIGN_TOP_LEFT, 10, 10);

  lv_obj_t * labelLightsOn = lv_label_create(btnLightsOn);
  lv_label_set_text(labelLightsOn, "All lights\n    ON");
  lv_obj_set_style_text_font(labelLightsOn, &lv_font_montserrat_20, LV_STATE_DEFAULT);
  lv_obj_center(labelLightsOn);


  lv_obj_t * btnLightsOff = lv_btn_create(lv_scr_act());
  btn_num = new int;
  *btn_num = 1;
  lv_obj_add_event_cb(btnLightsOff, event_handler, LV_EVENT_CLICKED, btn_num);
  lv_obj_remove_style_all(btnLightsOff);                          /*Remove the style coming from the theme*/
  lv_obj_add_style(btnLightsOff, &style, 0);
  lv_obj_add_style(btnLightsOff, &style_pr_2, LV_STATE_PRESSED);
  lv_obj_set_size(btnLightsOff, 145, 100);
  lv_obj_align(btnLightsOff, LV_ALIGN_TOP_LEFT, 165, 10);

  lv_obj_t * labelLightsOff = lv_label_create(btnLightsOff);
  lv_label_set_text(labelLightsOff, "All lights\n   OFF");
  lv_obj_set_style_text_font(labelLightsOff, &lv_font_montserrat_20, LV_STATE_DEFAULT);
  lv_obj_center(labelLightsOff);

  lv_create_lights_gui();
  lv_create_temp_arc_gui();
  lv_create_humidity_gui();
}

long msgId=0;

static void event_handler(lv_event_t * e)
{
    int *btn = (int*)lv_event_get_user_data(e);
    Serial.print("Clicked ");Serial.println(*btn);

    if (*btn == 0) {
      // turn on all the lights
      Serial.println("Turning on all the lights!");
      // Send UDP packet
      String rpcMessage = "{\"id\":"+ String(msgId++) +",\"method\":\"Switch.Set\", \"params\":{\"id\":0,\"on\":true}}";
      controlLights->sendAllRPC(rpcMessage);
    } else if (*btn == 1) {
      // turn off all the lights
      Serial.println("Turning off all the lights!");
      String rpcMessage = "{\"id\":"+ String(msgId++) +",\"method\":\"Switch.Set\", \"params\":{\"id\":0,\"on\":false}}";
      controlLights->sendAllRPC(rpcMessage);
    } else if (*btn > 1) {
      int light_idx = *btn - 2;
      Serial.print("Toggling "); Serial.print(lights_labels[light_idx]); Serial.println(" light");
      String rpcMessage = "{\"id\":"+ String(msgId++) +", \"method\":\"Switch.Toggle\", \"params\":{\"id\":0}}";
      controlLights->sendRPC(rpcMessage, lights_addresses[light_idx]);
    }
}

//_______________________
void my_disp_flush( lv_display_t *disp, const lv_area_t *area, uint8_t * px_map )
{
  uint32_t w = ( area->x2 - area->x1 + 1 );
  uint32_t h = ( area->y2 - area->y1 + 1 );

  tft.startWrite();
  tft.setAddrWindow( area->x1, area->y1, w, h );
  //tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
  tft.endWrite();

  lv_disp_flush_ready( disp );
}
//_______________________

void setSensor() {
  String temperatureStr = server.arg("temperature");
  String humidityStr = server.arg("humidity");
  String msg = "Updated ";
  if (!temperatureStr.isEmpty()) {
    temperature = temperatureStr.toFloat();
    msg += "temperature to " + temperatureStr + ", ";
  }
  if (!humidityStr.isEmpty()) {
    humidity = humidityStr.toFloat();
    msg += "humidity to " + humidityStr;
  }
  update_temp();
  update_humidity();
  server.send(200, "text/plain", msg);
}

void setup_routing() {     
  server.on("/set_sensor", HTTP_GET, setSensor);

  server.begin();
}

void setup()
{ 
  Serial.begin( 115200 );

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  Serial.begin( 115200 );
  Serial.println( LVGL_Arduino );

  connectToWiFi(WIFI_SSID, WIFI_PWD);

  setup_routing();

  // initialize lights control functionality
  controlLights->begin(onStatusChange);

  lv_init();

  /*Set a tick source so that LVGL will know how much time elapsed. */
  lv_tick_set_cb(my_tick);

  /* register print function for debugging */
#if LV_USE_LOG != 0
  lv_log_register_print_cb( my_print );
#endif

  lv_display_t * disp;

#if LV_USE_TFT_ESPI
  /*TFT_eSPI can be enabled lv_conf.h to initialize the display in a simple way*/
  disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, sizeof(draw_buf));
  lv_display_set_rotation(disp, TFT_ROTATION);
#else
  /*Else create a display yourself*/
  disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
  lv_display_set_flush_cb(disp, my_disp_flush);
  lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif

  /*Initialize the input device driver*/
  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);

  lv_create_main_screen_gui();

  tp.begin();
  tp.setRotation(ROTATION_INVERTED);

  Serial.println( "Setup done" );
}

void onStatusChange(StaticJsonDocument<200> doc) {
  if (doc["method"].is<String>() && doc["method"]=="NotifyStatus") {
    if (doc["params"]["switch:0"]["output"].is<bool>()) {
      bool output = doc["params"]["switch:0"]["output"];
      // extract the device id from the source. it is located at the end (12 chars)
      String id = doc["src"];
      id = id.substring(id.length()-12);
      for (int i=0; i < sizeof(lights_states); i++) {
        if (lights_ids[i]==id) {
          lights_states[i] = output;
          break;
        }

      }
      
      update_lights();
    }
  }
}

void loop()
{
  server.handleClient();
  controlLights->timerHandler();
  lv_timer_handler();
  delay( 5 );
}