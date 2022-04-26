<?php

$data = array();
$last_lines = array();
$team_no = 0;

// Download files from server
function getFile($base_url, $save_location) {
    // Init curl operations
    $curl_session = curl_init($base_url);
    $fp = fopen($save_location, 'wb');
    curl_setopt($curl_session, CURLOPT_FILE, $fp);
    curl_setopt($curl_session, CURLOPT_HEADER, 0);
    curl_exec($curl_session);
    curl_close($curl_session);

    // Close file
    fclose($fp);
}

// Get Number of lines to get the last line in the next function
function getNoOfLines($save_location) {
    $no_of_lines = 0;
    $fp = fopen($save_location, "r");

    while(!feof($fp)) {
        fgets($fp, 1024);
        $no_of_lines++;
    }

    return $no_of_lines;
}

// Get last line from text file
function getLastLine($save_location, $no_of_lines) {
    if ($no_of_lines > 0) {
        $lines = file($save_location);
        return $lines[$no_of_lines - 2];
    } else {
        return "";
    }
}

// Get lat and long data
function extractLatLong($last_line) {
    $cleaned_str = preg_replace("/[\s]+/", ",", $last_line);
    $str_arr = explode (",", $cleaned_str);

    if (sizeof($str_arr) > 2) {
        return [
            'latitude' => $str_arr[2],
            'longitude' => $str_arr[3],
            'temperature' => $str_arr[4],
            'pressure' => $str_arr[5],
            'altitude' => $str_arr[6],
        ];
    } else {
        return "There's no valid latitude and longitude data";
    }
}

function extractData() {
    global $last_lines;
    global $data;
    global $team_no;

    $base_urls = array('https://www.hh3dlab.fi/madrid/iot01/iotwrite.txt');

    foreach ($base_urls as $key => $base_url) {
        $key += 1;
        $dir = './';
        $url_arr = explode ("/", $base_url);
        $team_no = substr($url_arr[4], -2);

        $filename = 'iotwrite' . $team_no . '.txt';
        $save_location = $dir . $filename;

        getFile($base_url, $save_location);
        $no_of_lines = getNoOfLines($save_location);
        $last_line = getLastLine($save_location, $no_of_lines);
        $coordination = extractLatLong($last_line);
        array_push($last_lines, $last_line);
        array_push($data, $coordination);
    }
}