<?php

$post_body = file_get_contents("php://stdin");
parse_str($post_body, $form_inputs);

$a = (float)$form_inputs['number1'];
$b = (float)$form_inputs['number2'];
$addition = $a + $b;
$substraction = $a - $b;
$multiplication = $a * $b;
$division = $b != 0 ? $a / $b : "Can't divide by 0";
$result = "Addition: $addition\nSubstraction: $substraction\nMultiplication: $multiplication\nDivision: $division\n";


echo "HTTP/1.1 200 OK\r\n";
echo "Content-Length: " . strlen($result) . "\r\n";
echo "Content-Type: text/plain\r\n\r\n";
echo $result;

?>
