//Raw string literal quotation, webpage HTML...
String webpage = R"(
<!DOCTYPE html>
<html>
<head>
  <title>AmpX Open Energy Gateway</title>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <style>
    body { background-color: #EEEEEE; font-family: Arial, sans-serif; }
    .content { margin: 20px; }
    span { color: #003366; }
    table td {border: 1px solid black;}
    table th {background-color:cccccc;}
  </style>
</head>
<body>
  <div class='content'>
    <h1>AmpX Open Energy Gateway</h1>
    <span><a href='www.ampx.co/' target'_blank'>www.ampx.co</a></span>
    <br/>
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
      <td>voltage_on_L3 (V)</td>
    </tr>
    </table>
  </div>
</body>
</html>
  )";

// Function to generate webpage content
String getWebpage() {
  //Raw string literal quotation  
  return String(webpage);
}
