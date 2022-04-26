<!DOCTYPE html >
<head>
    <meta name="viewport" content="initial-scale=1.0, user-scalable=no" />
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"/>
    <link href="https://unpkg.com/leaflet@1.6.0/dist/leaflet.css" rel="stylesheet"/>
    <title>Madrid Seminar - IoT</title>
    <style>
        #map {
            width: 100%;
            height: 80%;
        }

        html, body {
            height: 100%;
            margin: 0;
            padding: 0;
        }

        .container {
            display: flex;
            flex-direction: column;
            align-items: center;
        }

        .fetch-btn {
            margin: 1rem auto;
        }
    </style>
    <script src="https://unpkg.com/leaflet@1.6.0/dist/leaflet.js"></script>
</head>

<html lang="en">
<body>
<?php
include 'extract_data.php';

if (isset($_POST["Fetch"])) {
    extractData();
}
?>
<div class="container">
    <form method="POST">
        <input type="submit" value="Fetch Data" name="Fetch" class="fetch-btn">
    </form>
</div>

<div id="map"></div>

<script defer type="text/javascript">
    const data = <?php echo json_encode($data); ?>;
    const teamNo = <?php echo json_encode($team_no); ?>;
    console.log(data);

    // const mapOptions = {
    //     center: [60.17116785151859, 24.942630453799243],
    //     zoom: 16
    // }

    // const map = new L.map('map' , mapOptions);
    // const layer = new L.TileLayer('http://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png');
    // map.addLayer(layer);

    data.forEach((c, i) => {
        const mapOptions = {
            center: [c.latitude, c.longitude],
            zoom: 16
        }

        const map = new L.map('map' , mapOptions);
        const layer = new L.TileLayer('http://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png');
        map.addLayer(layer);
        i += 1;
        const marker = new L.Marker([c.latitude, c.longitude]);
        marker.addTo(map);
        marker.bindPopup(`Team ${teamNo} - ${c.temperature}, ${c.pressure}, ${c.altitude}`).openPopup();
    })
</script>
</body>
</html>

