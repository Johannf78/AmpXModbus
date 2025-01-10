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
    <label for='m1_name'>Meter name: </label><input type='text' id='m1_name' name='m1_name'>
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
  )";