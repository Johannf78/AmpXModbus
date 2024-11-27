//Raw string literal quotation, webpage HTML...
String webpage = R"(
<!DOCTYPE html>
<html>
<head>
  <title>AmpX Open Energy Gateway</title>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <style>
    body { background-color: #EEEEEE; font-family: Arial, sans-serif; }
    main { margin: auto;  width: 60%;  border: 3px solid black;  padding: 20px; }
    footer { margin: auto;  width: 60%;  border: 0px solid black;  padding: 20px; }
    span { color: #003366; }
    table {border-collapse: collapse; }
    table td {border: 1px solid black; padding: 5px;}
    table th {border: 1px solid black; padding: 5px; background-color: #cccccc;}
  </style>
</head>
<body>
  <main>
    <h1>AmpX Open Energy Gateway</h1>
    
    <br/>
    Meter: 1<br/>, Serial number: m1_serial_number

    <table>
    <tr>
      <th>Entity</th>
      <th>L1</th>
      <th>L2</<th>
      <th>L3</<th>
      <th>Avg/Max</<th>
    </tr>
    <tr>
      <td>Voltage</td>
      <td>voltage_on_L1 (V)</td>
      <td>voltage_on_L2 (V)</td>
      <td>voltage_on_L3 (V)</td>
      <td></td>
    </tr>
    <tr>
      <td>Current</td>
      <td>current_on_L1 (V)</td>
      <td>current_on_L2 (V)</td>
      <td>current_on_L3 (V)</td>
      <td></td>
    </tr>

    </table>
  </main>
  <footer>
  <br/><br/>
  <span><a href='www.ampx.co/' target'_blank'>www.ampx.co</a></span>
  </footer>
</body>
</html>
  )";
