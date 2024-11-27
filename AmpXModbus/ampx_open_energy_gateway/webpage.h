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
      <td id='voltage_L1'>voltage_L1 V</td>
      <td id='voltage_L2'>voltage_L2 V</td>
      <td id='voltage_L3'>voltage_L3 V</td>
      <td></td>
    </tr>
    <tr>
      <td class='text-align-left'>Current</td>
      <td id='current_L1'>current_L1 A</td>
      <td id='current_L2'>current_L2 A</td>
      <td id='current_L3'>current_L3 A</td>
      <td id='current_avg'>current_avg A</td>
    </tr>
    <tr>
      <td class='text-align-left'>Active Power</td>
      <td id='active_power_L1'>active_power_L1 W</td>
      <td id='active_power_L2'>active_power_L2 W</td>
      <td id='active_power_L3'>active_power_L3 W</td>
      <td id='active_power_tot'>active_power_tot W</td>
    </tr>
    <tr>
      <td class='text-align-left'>Active Energy Imported</td>
      <td id='active_energy_imported_L1'>active_energy_imported_L1 Wh</td>
      <td id='active_energy_imported_L2'>active_energy_imported_L2 Wh</td>
      <td id='active_energy_imported_L3'>active_energy_imported_L3 Wh</td>
      <td id='active_energy_imported_tot'>active_energy_imported_tot Wh</td>
    </tr>
    </table>
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
    document.getElementById('test').innerHTML = event.data;
    document.getElementById('current_L1').innerHTML = event.data;
    console.log(event.data);
  }
  window.onload = function(event){
    init();
  }
</script>
</html>
  )";
