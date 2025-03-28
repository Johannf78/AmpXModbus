#ifndef WEB_SETTINGS_H
#define WEB_SETTINGS_H

#include <pgmspace.h>

// Store the settings HTML in program memory
const char settings_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>AmpX Open Energy Gateway</title>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <style>
    body { background-color: #EEEEEE; font-family: Arial, sans-serif; }
    main { margin: auto;  border: 3px solid black;  padding: 20px; }
    footer { margin: auto;   border: 0px solid black;  padding: 20px; }
    span { color: #003366; }
    table {border-collapse: collapse; }
    table th {border: 1px solid black; padding: 5px; background-color: #cccccc;}
    table td {border: 1px solid black; padding: 5px;  text-align: right;}
    .text-align-left {text-align: left;}
  </style>
</head>
<body>
<main>
  <h1>AmpX Open Energy Gateway - Settings</h1>
  <br>
  <h2>Information/Status</h2>
  <ul>
    <li>Gateway ID: m_gateway_id</li>
    <li id='m_wifi_rssi'>Wi-Fi Strength: </li>
    <li>Connected Meters: m_connected_meters_num</li>
  </ul>
  <h2>Set Meter Names</h2>
  <p>To idetify meters, set a name for each meter.</p>
  
  <form action='update_meters_name' method='post' enctype='application/x-www-form-urlencoded'>
    <div id='meter1'>
      <h3>Meter: 1, Serial number: m1_serial_number</h3>
      <label for='m1_name'>Meter name: </label><input type='text' id='m1_name' name='m1_name' value='m1_name_value'>
    </div>

    <div id='meter2'>
      <h3>Meter: 2, Serial number: m2_serial_number</h3>
      <label for='m2_name'>Meter name: </label><input type='text' id='m2_name' name='m2_name' value='m2_name_value'>
    </div>

    <div id='meter3'>
      <h3>Meter: 3, Serial number: m3_serial_number</h3>
      <label for='m3_name'>Meter name: </label><input type='text' id='m3_name' name='m3_name' value='m3_name_value'>
    </div>

    <div id='meter4'>
      <h3>Meter: 4, Serial number: m4_serial_number</h3>
      <label for='m4_name'>Meter name: </label><input type='text' id='m4_name' name='m4_name' value='m4_name_value'>
    </div>

    <br>
    <input type='submit' value='update'>
  </form>

  <br>
  <h2>Meter Firmware Update</h2>
  <form action='update' method='post'>
    <p>
    <lable>Click the button to download and update the device firmware:</lable>
    <input type='submit' value='update'></p>
  </form>

  <br>
  <p><a href="/">Home </a></p>

</main>
<footer>
<br/><br/>
<span><a href='www.ampx.co/' target'_blank'>www.ampx.co</a></span>
</footer>
</body>
<script>
  var socket;
  function init(){
    //This variable is updated by the Arduino code before sending the HTML
    var numberOfMeters = numberOfMetersValue; // Define the number of meters as an integer
    //document.getElementById(key).value = value;

    // Hide the div with id 'meter2', 'meter3', and 'meter4' based on the numberOfMeters
    if (numberOfMeters == 1) {
        document.getElementById('meter2').style.display = 'none';
        document.getElementById('meter3').style.display = 'none';
        document.getElementById('meter4').style.display = 'none';
    } else if (numberOfMeters == 2) {
        document.getElementById('meter3').style.display = 'none';
        document.getElementById('meter4').style.display = 'none';
    } else if (numberOfMeters == 3) {
        document.getElementById('meter4').style.display = 'none';
    }
 
    socket = new WebSocket('ws://' + window.location.hostname + ':81/');
    socket.onmessage = function(event) {
      processCommand(event);
    };
  }
  function processCommand(event){
    var data = JSON.parse(event.data);
    if (data) {
      key = 'm_wifi_rssi';
      document.getElementById(key).textContent = "Wi-Fi Strength: " + data[key];
    }
  }
  window.onload = function(event){
    init();
  }
</script>
)rawliteral";

// Settings page handler class
class WebSettings {
private:
  String _buffer; // Temporary buffer for processing

public:
  WebSettings() {
    // Initialize an empty buffer
    _buffer = "";
  }

  // Get a fresh copy of the settings page from PROGMEM
  void load() {
    _buffer = FPSTR(settings_html);
  }

  // Replace a placeholder with a value
  void replace(const String& placeholder, const String& value) {
    _buffer.replace(placeholder, value);
  }

  // Get the processed settings page content
  const String& getContent() const {
    return _buffer;
  }
};

// Create a global instance of the settings page handler
WebSettings webpage_settings;

#endif // WEB_SETTINGS_H