#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <pgmspace.h>

// Store the HTML code in program memory instead of RAM
const char webpage_html[] PROGMEM = R"rawliteral(
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
  <h1>AmpX Open Energy Gateway</h1>

  <h2>Energy Consumption Summary</h2>
  <table>
  <tr>
    <th>Meter</th>
    <th>Total active energy imported</th>
  </tr>
  <tr id='meter1_summary'>
    <td>1</td>
    <td id='m1_active_energy_imported_tot_summary'>0 kWh</td>
  </tr>
  <tr id='meter2_summary'>
    <td>2</td>
    <td id='m2_active_energy_imported_tot_summary'>0 kWh</td>
  </tr>
  <tr id='meter3_summary'>
    <td>3</td>
    <td id='m3_active_energy_imported_tot_summary'>0 kWh</td>
  </tr>
  <tr id='meter4_summary'>
    <td>4</td>
    <td id='m4_active_energy_imported_tot_summary'>0 kWh</td>
  </tr>
  </table>
  



  <br/>
  <div id='meter1'>
    <h3>Meter: 1, Serial number: m1_serial_number<span id='m1_name'>test<span></h3>
    <table>
    <tr>
      <th>Entity</th>
      <th>L1</th>
      <th>L2</<th>
      <th>L3</<th>
      <th>Avg/Max</<th>
    </tr>
    <tr>
      <td class='text-align-left'>Voltage</td>
      <td id='m1_voltage_L1'>0.00 V</td>
      <td id='m1_voltage_L2'>0.00 V</td>
      <td id='m1_voltage_L3'>0.00 V</td>
      <td></td>
    </tr>
    <tr>
      <td class='text-align-left'>Current</td>
      <td id='m1_current_L1'>0.00 A</td>
      <td id='m1_current_L2'>0.00 A</td>
      <td id='m1_current_L3'>0.00 A</td>
      <td id='m1_current_avg'>0.00 A</td>
    </tr>
    <tr>
      <td class='text-align-left'>Active Power</td>
      <td id='m1_active_power_L1'>0.00 KW</td>
      <td id='m1_active_power_L2'>0.00 KW</td>
      <td id='m1_active_power_L3'>0.00 KW</td>
      <td id='m1_active_power_tot'>0.00 KW</td>
    </tr>
    <tr>
      <td class='text-align-left'>Active Energy Imported</td>
      <td id='m1_active_energy_imported_L1'>0 kWh</td>
      <td id='m1_active_energy_imported_L2'>0 kWh</td>
      <td id='m1_active_energy_imported_L3'>0 kWh</td>
      <td id='m1_active_energy_imported_tot'>0 kWh</td>
    </tr>
    </table>
  </div>

  <br/>
  <div id='meter2'>
    <h3>Meter: 2, Serial number: m2_serial_number<span id='m2_name'><span></h3>
    <table>
    <tr>
      <th>Entity</th>
      <th>L1</th>
      <th>L2</<th>
      <th>L3</<th>
      <th>Avg/Max</<th>
    </tr>
    <tr>
      <td class='text-align-left'>Voltage</td>
      <td id='m2_voltage_L1'>0.00 V</td>
      <td id='m2_voltage_L2'>0.00 V</td>
      <td id='m2_voltage_L3'>0.00 V</td>
      <td></td>
    </tr>
    <tr>
      <td class='text-align-left'>Current</td>
      <td id='m2_current_L1'>0.00 A</td>
      <td id='m2_current_L2'>0.00 A</td>
      <td id='m2_current_L3'>0.00 A</td>
      <td id='m2_current_avg'>0.00 A</td>
    </tr>
    <tr>
      <td class='text-align-left'>Active Power</td>
      <td id='m2_active_power_L1'>0.00 KW</td>
      <td id='m2_active_power_L2'>0.00 KW</td>
      <td id='m2_active_power_L3'>0.00 KW</td>
      <td id='m2_active_power_tot'>0.00 KW</td>
    </tr>
    <tr>
      <td class='text-align-left'>Active Energy Imported</td>
      <td id='m2_active_energy_imported_L1'>0 kWh</td>
      <td id='m2_active_energy_imported_L2'>0 kWh</td>
      <td id='m2_active_energy_imported_L3'>0 kWh</td>
      <td id='m2_active_energy_imported_tot'>0 kWh</td>
    </tr>
    </table>
  </div>

  <br/>
  <div id='meter3'>
    <h3>Meter: 3, Serial number: m3_serial_number<span id='m3_name'><span></h3>
    <table>
    <tr>
      <th>Entity</th>
      <th>L1</th>
      <th>L2</<th>
      <th>L3</<th>
      <th>Avg/Max</<th>
    </tr>
    <tr>
      <td class='text-align-left'>Voltage</td>
      <td id='m3_voltage_L1'>0.00 V</td>
      <td id='m3_voltage_L2'>0.00 V</td>
      <td id='m3_voltage_L3'>0.00 V</td>
      <td></td>
    </tr>
    <tr>
      <td class='text-align-left'>Current</td>
      <td id='m3_current_L1'>0.00 A</td>
      <td id='m3_current_L2'>0.00 A</td>
      <td id='m3_current_L3'>0.00 A</td>
      <td id='m3_current_avg'>0.00 A</td>
    </tr>
    <tr>
      <td class='text-align-left'>Active Power</td>
      <td id='m3_active_power_L1'>0.00 KW</td>
      <td id='m3_active_power_L2'>0.00 KW</td>
      <td id='m3_active_power_L3'>0.00 KW</td>
      <td id='m3_active_power_tot'>0.00 KW</td>
    </tr>
    <tr>
      <td class='text-align-left'>Active Energy Imported</td>
      <td id='m3_active_energy_imported_L1'>0 kWh</td>
      <td id='m3_active_energy_imported_L2'>0 kWh</td>
      <td id='m3_active_energy_imported_L3'>0 kWh</td>
      <td id='m3_active_energy_imported_tot'>0 kWh</td>
    </tr>
    </table>
  </div>


  <br/>
  <div id='meter4'>
    <h3>Meter: 4, Serial number: m4_serial_number<span id='m4_name'><span></h3>
    <table>
    <tr>
      <th>Entity</th>
      <th>L1</th>
      <th>L2</<th>
      <th>L3</<th>
      <th>Avg/Max</<th>
    </tr>
    <tr>
      <td class='text-align-left'>Voltage</td>
      <td id='m4_voltage_L1'>0.00 V</td>
      <td id='m4_voltage_L2'>0.00 V</td>
      <td id='m4_voltage_L3'>0.00 V</td>
      <td></td>
    </tr>
    <tr>
      <td class='text-align-left'>Current</td>
      <td id='m4_current_L1'>0.00 A</td>
      <td id='m4_current_L2'>0.00 A</td>
      <td id='m4_current_L3'>0.00 A</td>
      <td id='m4_current_avg'>0.00 A</td>
    </tr>
    <tr>
      <td class='text-align-left'>Active Power</td>
      <td id='m4_active_power_L1'>0.00 KW</td>
      <td id='m4_active_power_L2'>0.00 KW</td>
      <td id='m4_active_power_L3'>0.00 KW</td>
      <td id='m4_active_power_tot'>0.00 KW</td>
    </tr>
    <tr>
      <td class='text-align-left'>Active Energy Imported</td>
      <td id='m4_active_energy_imported_L1'>0 kWh</td>
      <td id='m4_active_energy_imported_L2'>0 kWh</td>
      <td id='m4_active_energy_imported_L3'>0 kWh</td>
      <td id='m4_active_energy_imported_tot'>0 kWh</td>
    </tr>
    </table>
  </div>

<p>Values updates every 3 seconds.</p>
<p><a href="/settings">Settings </a></p>


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
  
      // Hide the div with id 'meter2', 'meter3', and 'meter4' based on the numberOfMeters
      // Also hide the summary table rows
    if (numberOfMeters == 1) {
        document.getElementById('meter2_summary').style.display = 'none';
        document.getElementById('meter3_summary').style.display = 'none';
        document.getElementById('meter4_summary').style.display = 'none';

        document.getElementById('meter2').style.display = 'none';
        document.getElementById('meter3').style.display = 'none';
        document.getElementById('meter4').style.display = 'none';
    } else if (numberOfMeters == 2) {
        document.getElementById('meter3_summary').style.display = 'none';
        document.getElementById('meter4_summary').style.display = 'none';

        document.getElementById('meter3').style.display = 'none';
        document.getElementById('meter4').style.display = 'none';
    } else if (numberOfMeters == 3) {
        document.getElementById('meter4_summary').style.display = 'none';

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
      for (let key in data) {
        let value = data[key];
        
        if (key.includes('voltage'))
          value += ' V';
        else if (key.includes('current'))
          value += ' A';
        else if (key.includes('power'))
          value += ' kW';
        else if (key.includes('energy'))
          value += ' kWh';
      else if (key.includes('name'))
          value = ', Name: ' + value;
           
        //console.log("Key:", key, "Value:", value);
        document.getElementById(key).innerHTML = value;
        
      }
    }
  }
  window.onload = function(event){
    init();
  }
</script>
</html>
)rawliteral";

// Replacement class - a memory-efficient way to handle the webpage with placeholders
class Webpage {
private:
  String _buffer; // Temporary buffer for processing

public:
  Webpage() {
    // Initialize an empty buffer
    _buffer = "";
  }

  // Get a fresh copy of the webpage from PROGMEM
  void load() {
    _buffer = FPSTR(webpage_html);
  }

  // Replace a placeholder with a value
  void replace(const String& placeholder, const String& value) {
    _buffer.replace(placeholder, value);
  }

  // Get the processed webpage content
  const String& getContent() const {
    return _buffer;
  }
};

// Create a global instance of the webpage handler
Webpage webpage;

#endif // WEBPAGE_H
