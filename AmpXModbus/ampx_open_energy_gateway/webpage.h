//Raw string literal quotation, webpage HTML...
String webpage = R"(
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
  
  <br/>
  <div id='meter1'>
    <h3>Meter: 1, Serial number: m1_serial_number</h3>
    Test: <span id='test'>--</span>
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
    <h3>Meter: 2, Serial number: m2_serial_number</h3>
    Test: <span id='test'>--</span>
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


</main>
<footer>
<br/><br/>
<span><a href='www.ampx.co/' target'_blank'>www.ampx.co</a></span>
</footer>
</body>
<script>
  var socket;
  function init(){
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

        document.getElementById(key).innerHTML = value;
      }
    }
  }
  window.onload = function(event){
    init();
  }
</script>
</html>
  )";
