//Raw string literal quotation, webpage HTML...
String webpage_settings = R"(
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
  
  <form action='update_meters_name' method='post' enctype='application/x-www-form-urlencoded'>
    <h3>Meter: 1, Serial number: m1_serial_number</h3>
    <label for='m1_name'>Meter name: </label><input type='text' id='m1_name' name='m1_name' value='m1_name_value'>
    <input type='submit' value='update'>
  </form>

  <form action='update' method='post'>
    <p>Update firmware</p>
    <input type='submit' value='update'>
  </form>



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

    /*
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
    */
  }
  function processCommand(event){
    var data = JSON.parse(event.data);
    
    if (data) {
      for (let key in data) {
        let value = data[key];
        
        try {
          document.getElementById(key).value = value;
          console.log("Key:", key, "Value:", value);
        } catch {
        }
      }
    }
  }
  window.onload = function(event){
    init();
  }
</script>
  )";