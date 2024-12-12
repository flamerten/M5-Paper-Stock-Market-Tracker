#include <Arduino.h>
#include <M5EPD.h>

// For Wifi
#include <WiFi.h>
#include <HTTPClient.h> // Query Stock Market via HTTP
#include "secrets.h"

#include "rest_queries.h"

#define ENABLE_STOCK_QUERY

//Function Prototypes

bool connect_wifi(uint32_t timeout_ms);

void init_eink_display();

void draw_graph(float *results, int top_x, int top_y, int bot_x, int bot_y);
void draw_ticker(String ticker_short, String ticker_long, int x, int y);
void draw_numbers(float *results, int x, int y);

float mapf(float x, float in_min, float in_max, float out_min, float out_max);

// Defined Objects

M5EPD_Canvas canvas(&M5.EPD);

Datetime_t datetime_now;
Poylgon_params_t polygon_params;

AggResult_t results[MAX_BAR_NUMBER];
float prices[MAX_BAR_NUMBER];

// Main

void setup() {

    #ifdef ENABLE_STOCK_QUERY
    bool res = connect_wifi(30000);

    if (!res)
    {
        Serial.println("Error!");
        return;
    }

    delay(1000);

    //Failed timeupdate
    if (!update_time(&datetime_now)) return;

    #endif //ENABLE_STOCK_QUERY


    init_eink_display();


    #ifdef ENABLE_STOCK_QUERY
    update_polygon_param_date(
        &datetime_now,
        &polygon_params,
        Timespan_t::TS_DAY,
        30);

    Serial.println("Start: " + polygon_params.start_date);
    Serial.println("End:   " + polygon_params.end_date);

    delay(1000);
    #endif //ENABLE_STOCK_QUERY

    canvas.clear();

    int graph_x_top = 120;
    int graph_y_top = 60;

    int graph_x_bot = 380;
    int graph_y_bot = 210;

    int text_x = 20;
    int text_y = 100;

    int values_x = 400;
    int values_y = 100;

    int graph_offset = 220;
    int text_offset = 220;


    canvas.setTextSize(2);

    //Print todays date, top left corner
    canvas.setCursor(20,20);
    canvas.print(polygon_params.end_date);
    
    //Print range
    canvas.setCursor(20,920);
    canvas.print("Last " + String(MAX_BAR_NUMBER) + " days");

    //APPLE

    
    #ifdef ENABLE_STOCK_QUERY
    query_stock_market(
        &polygon_params,
        "AAPL", 1,
        results, MAX_BAR_NUMBER);

    #else 
    for(int i = 0; i < MAX_BAR_NUMBER; i++)
    {
        results[i].o_open_price = i*10;
    }

    #endif //ENABLE_STOCK_QUERY
    
    for(int i = 0; i < MAX_BAR_NUMBER; i++)
    {
        prices[i] = results[i].o_open_price;
    }

    //540*960 @4.7" E-ink display
    draw_graph(prices, graph_x_top, graph_y_top, graph_x_bot, graph_y_bot);
    draw_ticker("AAPL","Apple Inc", text_x, text_y);
    draw_numbers(prices, values_x, values_y);

    graph_y_top = graph_y_top + graph_offset;
    graph_y_bot = graph_y_bot + graph_offset;
    text_y = text_y + text_offset;
    values_y = values_y + text_offset;

    //TESLA


    #ifdef ENABLE_STOCK_QUERY
    query_stock_market(
        &polygon_params,
        "TSLA", 1,
        results, MAX_BAR_NUMBER);
    #endif //ENABLE_STOCK_QUERY
    
    for(int i = 0; i < MAX_BAR_NUMBER; i++)
    {
        prices[i] = results[i].o_open_price;
    }

    draw_graph(prices, graph_x_top, graph_y_top, graph_x_bot, graph_y_bot);
    draw_ticker("TSLA","Tesla", text_x, text_y);
    draw_numbers(prices, values_x, values_y);

    graph_y_top = graph_y_top + graph_offset;
    graph_y_bot = graph_y_bot + graph_offset;
    text_y = text_y + text_offset;
    values_y = values_y + text_offset;

    //Bitcoin

    #ifdef ENABLE_STOCK_QUERY
    query_stock_market(
        &polygon_params,
        "BTC", 1,
        results, MAX_BAR_NUMBER);
    #endif //ENABLE_STOCK_QUERY

    
    for(int i = 0; i < MAX_BAR_NUMBER; i++)
    {
        prices[i] = results[i].o_open_price;
    }

    draw_graph(prices, graph_x_top, graph_y_top, graph_x_bot, graph_y_bot);
    draw_ticker("BTC","Bitcoin", text_x, text_y);
    draw_numbers(prices, values_x, values_y);

    graph_y_top = graph_y_top + graph_offset;
    graph_y_bot = graph_y_bot + graph_offset;
    text_y = text_y + text_offset;
    values_y = values_y + text_offset;

    //Nvidia

    #ifdef ENABLE_STOCK_QUERY
    query_stock_market(
        &polygon_params,
        "NVDA", 1,
        results, MAX_BAR_NUMBER);
    #endif //ENABLE_STOCK_QUERY

    
    for(int i = 0; i < MAX_BAR_NUMBER; i++)
    {
        prices[i] = results[i].o_open_price;
    }

    draw_graph(prices, graph_x_top, graph_y_top, graph_x_bot, graph_y_bot);
    draw_ticker("NVDA","Nvidia", text_x, text_y);
    draw_numbers(prices, values_x, values_y);
    

    canvas.pushCanvas(0,0,UPDATE_MODE_GC16);
    Serial.println("Done");


    M5.EPD.Sleep();

}

void loop() {
}


// Functions

bool connect_wifi(uint32_t timeout_ms)
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    uint32_t time_start = millis();

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);

        Serial.print(".");

        if (millis() - time_start >= timeout_ms)
        {
            Serial.println("Wifi connection failed, timeout");
            return false;
        }
    }

    Serial.println("Wifi connection ok");
    return true;
}

void init_eink_display()
{
    M5.begin();
    M5.EPD.SetRotation(90);
    M5.EPD.Clear(true);
    M5.RTC.begin();

    //540*960 @4.7" E-ink display
    canvas.createCanvas(540, 960);
    canvas.setTextSize(3);

    canvas.drawString("Hello World", 0, 10);
    canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
}

float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/**
 * @brief Draws a graph of results, where results[0] is the right-most point on the graph
 * [1,2,3,4] will result in a descending graph
 * 
 * 
 * Note: markers (circles) removed
 * 
 * @param results of length MAX_BAR_NUMBER
 * @param top_x 
 * @param top_y 
 * @param bot_x 
 * @param bot_y 
 */
void draw_graph(float *results, int top_x, int top_y, int bot_x, int bot_y)
{
    float min_val = results[0];
    float max_val = results[0];

    const int color_point = M5EPD_Canvas::G15;
    const int color_line = M5EPD_Canvas::G15;
    const int radius = 5;

    //Find the bounds
    for(int i = 1; i < MAX_BAR_NUMBER; i++)
    {
        min_val = min(min_val, results[i]);
        max_val = max(max_val, results[i]);
    }

    Serial.println("minval" + String(min_val));
    Serial.println("maxval" + String(max_val));

    int x; int y; //Current price coord

    int x_prev; int y_prev; //Previous price coord

    //Get the first coord
    x = top_x;
    y = mapf(results[MAX_BAR_NUMBER-1], min_val, max_val, top_y, bot_y);
    y = top_y + bot_y - y; //top_y offset + reversed

    // Not sure what colour this is
    //canvas.drawCircle(x, y, radius, color_point);    

    x_prev = x;
    y_prev = y;

    // go backwards from MAX-2 is the most recent one.
    // want the leftmost to be the oldest data
    for(int i = MAX_BAR_NUMBER-2; i >= 0; i--)
    {
        Serial.println(String(x_prev) + "," + String(y_prev));

        x = mapf(MAX_BAR_NUMBER-1 - i, 0, MAX_BAR_NUMBER-1, top_x, bot_x);
        y = mapf(results[i], min_val, max_val, top_y, bot_y);
        y = top_y + bot_y - y;

        //canvas.drawCircle(x, y, radius, color_point);
        canvas.drawLine(x, y, x_prev, y_prev, color_line);

        x_prev = x;
        y_prev = y;

    }

    //Last point
    Serial.println(String(x_prev) + "," + String(y_prev));
}

/**
 * @brief Draws ticker short (big font) above ticker long (small font)
 * 
 * @param ticker_short AAPL
 * @param ticker_long Apple Inc
 */
void draw_ticker(String ticker_short, String ticker_long, int x, int y)
{
    canvas.setTextSize(2);
    canvas.setCursor(x,y);
    canvas.print(ticker_long);

    canvas.setTextSize(4);
    canvas.setCursor(x, y + 30);
    canvas.print(ticker_short);
}

/**
 * @brief calcaulte final value, and % change from the first value
 * 
 * @param results len MAX
 */
void draw_numbers(float *results, int x, int y)
{

    float final_val = results[0];
    float first_val = results[MAX_BAR_NUMBER-1];

    float percentage_change = abs(final_val - first_val)/first_val * 100;

    //Decimal Places
    String percentage_str = String(percentage_change,2);
    String val_str = "$" + String(final_val,2);

    if(final_val > first_val) percentage_str = "+" + percentage_str + "%";
    else                      percentage_str = "-" + percentage_str + "%";

    canvas.setTextSize(2);
    canvas.setCursor(x,y);
    canvas.print(val_str);

    canvas.setTextSize(3);
    canvas.setCursor(x, y + 30);
    canvas.print(percentage_str);

}