//Raw string literal quotation, webpage HTML...
String webpage_admin = R"(
<!DOCTYPE html>
<html>
<head>
  <title>AmpX Open Energy Gateway - Admin</title>
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
  <h1>AmpX Open Energy Gateway - Admin</h1>
  <br>
  <h2>Gateway ID Management</h2>
  <form action='update_gateway_id' method='post' enctype='application/x-www-form-urlencoded'>
    <label for='gateway_id'>Current Gateway ID: </label>
    <span id='current_gateway_id'>m_gateway_id</span>
    <br><br>
    <label for='gateway_id'>Set New Gateway ID: </label>
    <input type='text' id='gateway_id' name='gateway_id' value='m_gateway_id'>
    <br><br>
    <input type='submit' value='Update Gateway ID'>
  </form>
  <br>
  <p><a href="/">Home</a> | <a href="/settings">Settings</a></p>
</main>
<footer>
<br/><br/>
<span><a href='www.ampx.co/' target='_blank'>www.ampx.co</a></span>
</footer>
</body>
<script>
  // Optionally, you can add JS to enhance UX or validate input
</script>
</html>
  )"; 