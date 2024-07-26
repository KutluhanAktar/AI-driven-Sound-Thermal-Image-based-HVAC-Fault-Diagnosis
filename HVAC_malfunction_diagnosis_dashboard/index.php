<!DOCTYPE html>
<html>
<head>
<title>HVAC System Diagnosis Dashboard</title>

<!--link to index.css-->
<link rel="stylesheet" type="text/css" href="assets/index.css"></link>

<!--link to favicon-->
<link rel="icon" type="image/png" sizes="36x36" href="assets/icon.png">

<!-- link to FontAwesome-->
<link rel="stylesheet" href="https://use.fontawesome.com/releases/v6.2.1/css/all.css">
 
<!-- link to font -->
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link href="https://fonts.googleapis.com/css2?family=Kanit&display=swap" rel="stylesheet">

<!--link to jQuery script-->
<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>

</head>
<body>
<img src="assets/head_img.jpg" class="head_img" />
<div class="category_menu">
<button class="active" id="0">All</button>
<button id="1">Cooling Malfunction Detections</button>
<button id="2">Thermal Image Samples</button>
<button id="3">Anamolous Sound Samples</button>
</div>
<div class="log_updates">
<section style="background-color:lightgray;"></section>
<section style="background-color:lightgray;"></section>
<section style="background-color:lightgray;"></section>
<section style="background-color:lightgray;"></section>
<section style="background-color:lightgray;"></section>
<section style="background-color:lightgray;"></section>
<!-- Generated HTML Elements -->
</div>

<!--Add the index.js file-->
<script type="text/javascript" src="assets/index.js"></script>

</body>
</html>